"""ELF32 i386 (relocatable objects, shared objects, executables)."""
import struct

from .build import Entry, build_functions
from .model import Module, canonical, object_stem

SHT_SYMTAB, SHT_RELA, SHT_REL, SHT_DYNSYM = 2, 4, 9, 11
SHF_EXECINSTR = 0x4
STT_SECTION, STT_FUNC, STT_GNU_IFUNC = 3, 2, 10
ET_REL = 1

R_386_GOT32, R_386_GOT32X = 3, 43
_WIDTH = {20: 2, 21: 2, 22: 1, 23: 1}
_PCREL = {2, 4, 10, 21, 23}            # PC32, PLT32, GOTPC, PC16, PC8
_KIND = {1: 'abs', 2: 'pcrel', 3: 'got', 4: 'pcrel', 9: 'pic', 10: 'pic', 43: 'got'}


def is_elf(data):
    return data[:4] == b'\x7fELF'


def load(path, data, member=None):
    if data[4] != 1 or data[5] != 1:
        raise ValueError('only little-endian ELF32 is supported')
    e_type, e_machine = struct.unpack_from('<HH', data, 16)
    if e_machine != 3:
        raise ValueError(f'unsupported ELF machine {e_machine}')
    shoff, = struct.unpack_from('<I', data, 32)
    shentsize, shnum, shstrndx = struct.unpack_from('<HHH', data, 46)
    linked = e_type != ET_REL
    mod = Module(path, 'elf', linked, member)
    secs = []
    for i in range(shnum):
        name, typ, flags, addr, off, size, link, info, align, entsize = struct.unpack_from('<10I', data, shoff + i * shentsize)
        secs.append(dict(nameoff=name, type=typ, flags=flags, addr=addr, offset=off, size=size,
                         link=link, info=info, entsize=entsize, index=i))
    if shstrndx < len(secs):
        st = secs[shstrndx]
        strs = data[st['offset']:st['offset'] + st['size']]
        for s in secs:
            s['name'] = strs[s['nameoff']:strs.find(b'\0', s['nameoff'])].decode('latin-1')

    def sdata(s):
        return b'' if s['type'] == 8 else data[s['offset']:s['offset'] + s['size']]

    alloc = [s for s in secs if s['flags'] & 0x2]
    if linked and alloc:
        mod.image_base = min(s['addr'] for s in alloc)
        mod.image_end = max(s['addr'] + s['size'] for s in alloc)
    code_secs = [s for s in secs if s['type'] == 1 and s['flags'] & SHF_EXECINSTR and not s['name'].startswith('.plt')]
    code_idx = {s['index']: s for s in code_secs}
    for s in secs:
        if s['type'] == 1 and s['flags'] & SHF_EXECINSTR:
            mod.code.append((s['addr'] if linked else _obj_va(s), sdata(s), s['name']))

    def read_syms(s):
        out = []
        strtab = secs[s['link']]
        strs = data[strtab['offset']:strtab['offset'] + strtab['size']]
        for name, value, size, info, other, shndx in struct.iter_unpack('<IIIBBH', sdata(s)):
            out.append((strs[name:strs.find(b'\0', name)].decode('latin-1'), value, size, info, shndx))
        return out

    symtabs = {s['index']: read_syms(s) for s in secs if s['type'] in (SHT_SYMTAB, SHT_DYNSYM)}
    own_stem = object_stem(member or path) if not linked else None

    def va_of(shndx, value):
        if linked:
            return value
        s = secs[shndx] if shndx < len(secs) else None
        return _obj_va(s) + value if s else value

    entries = {i: [] for i in code_idx}
    seen = set()
    for idx in sorted(symtabs, key=lambda i: secs[i]['type'] != SHT_SYMTAB):   # .symtab first
        for name, value, size, info, shndx in symtabs[idx]:
            typ, bind = info & 0xf, info >> 4
            if shndx == 0 or shndx >= 0xff00 or not name or typ == STT_SECTION:
                continue
            va = va_of(shndx, value)
            mod.add_name(va, canonical('elf', name))
            if typ not in (STT_FUNC, STT_GNU_IFUNC) or shndx not in code_idx:
                continue
            if (name, va) in seen:
                continue
            seen.add((name, va))
            entries[shndx].append(Entry(va, [(bind != 0, name)], size or None, exact=bool(size), obj=own_stem))

    fields = {i: [] for i in code_idx}
    for s in secs:
        if s['type'] not in (SHT_REL, SHT_RELA):
            continue
        syms = symtabs.get(s['link'], [])
        esz = 8 if s['type'] == SHT_REL else 12
        rel = sdata(s)
        if not linked:
            tgt = code_idx.get(s['info'])
            if not tgt:
                continue
            tdat = sdata(tgt)
            for k in range(len(rel) // esz):
                r_off, r_info = struct.unpack_from('<II', rel, k * esz)
                rtype, rsym = r_info & 0xff, r_info >> 8
                if rtype == 0:
                    continue
                width = _WIDTH.get(rtype, 4)
                addend = (struct.unpack_from('<i', rel, k * esz + 8)[0] if esz == 12
                          else int.from_bytes(tdat[r_off:r_off + width], 'little', signed=True))
                target = None
                if rsym < len(syms):
                    name, value, size, info, shndx = syms[rsym]
                    if info & 0xf == STT_SECTION and shndx < len(secs):
                        # section-relative reference: name the symbol at the referenced offset
                        toff = value + addend + (width if rtype in _PCREL else 0)
                        names = mod.resolve(_obj_va(secs[shndx]) + toff)
                        target = names[0] if names else f'{secs[shndx]["name"]}+{toff:#x}'
                    elif name:
                        target = canonical('elf', name)
                kind = _KIND.get(rtype, 'pcrel' if rtype in _PCREL else 'abs')
                va = _obj_va(tgt) + r_off
                if rtype in (R_386_GOT32, R_386_GOT32X):
                    fields[tgt['index']].append((va - 2, 2, 'got', None, 0))   # mov->lea relaxation rewrites opcode/ModRM
                fields[tgt['index']].append((va, width, kind, target, addend))
        else:
            for k in range(len(rel) // esz):
                r_off, r_info = struct.unpack_from('<II', rel, k * esz)
                for cs in code_secs:
                    if cs['addr'] <= r_off < cs['addr'] + cs['size']:
                        fields[cs['index']].append((r_off, 4, 'abs', None, 0))
    if linked:
        _plt_names(mod, secs, symtabs, sdata)
        # stripped images keep only exported symbols; every gcc function has an FDE
        eh = next((s for s in secs if s['name'] == '.eh_frame'), None)
        if eh:
            mod.fde_starts = set()
            for start, size in fde_ranges(sdata(eh), eh['addr']):
                mod.fde_starts.add(start)
                for s in code_secs:
                    if s['addr'] <= start < s['addr'] + s['size'] and size:
                        entries[s['index']].append(Entry(start, [], size, exact=True))
            for s in code_secs:
                entries[s['index']] += _gaps(sdata(s), s['addr'], entries[s['index']])

    for s in code_secs:
        build_functions(mod, s['name'], s['addr'] if linked else _obj_va(s), sdata(s), entries[s['index']], fields[s['index']])
    return mod


def _gaps(data, va, entries):
    """Entries for code between known extents that isn't padding (code without an FDE,
    such as glibc's crti.o __x86.get_pc_thunk.bx)."""
    from .x86 import skip_padding
    spans = sorted((e.addr, e.addr + (e.size or 0)) for e in entries)
    out, pos = [], va
    for a, b in spans + [(va + len(data), va + len(data))]:
        if a > pos:
            chunk = data[pos - va:a - va]
            lead = skip_padding(chunk)
            if lead < len(chunk) and chunk[lead:].strip(b'\0'):
                out.append(Entry(pos + lead, [], None))
        pos = max(pos, b)
    return out


def _uleb(d, i):
    v = shift = 0
    while True:
        b = d[i]
        i += 1
        v |= (b & 0x7f) << shift
        shift += 7
        if b < 0x80:
            return v, i


def _encoded(d, i, enc, va):
    """Read a DW_EH_PE-encoded pointer at d[i] (section VA va); returns (value, next index)."""
    if enc == 0xff:
        return None, i
    fmt = enc & 0x0f
    size, signed = {0x00: (4, False), 0x02: (2, False), 0x03: (4, False), 0x0a: (2, True), 0x0b: (4, True)}.get(fmt, (4, False))
    v = int.from_bytes(d[i:i + size], 'little', signed=signed)
    if enc & 0x70 == 0x10:
        v += va + i
    return v & 0xffffffff, i + size


def fde_ranges(d, va):
    """(start, size) of every FDE in a linked .eh_frame section at VA va."""
    cies, out, off = {}, [], 0
    while off + 8 <= len(d):
        length, = struct.unpack_from('<I', d, off)
        if length == 0 or length == 0xffffffff:
            break
        cid, = struct.unpack_from('<I', d, off + 4)
        end = off + 4 + length
        if cid == 0:
            i = off + 9
            aug = d[i:d.index(b'\0', i)]
            i += len(aug) + 1
            enc = 0
            if b'eh' in aug:
                i += 4
            _, i = _uleb(d, i)
            _, i = _uleb(d, i)
            i = i + 1 if d[off + 8] == 1 else _uleb(d, i)[1]
            if aug.startswith(b'z'):
                _, i = _uleb(d, i)
                for c in aug[1:].decode('latin-1'):
                    if c == 'R':
                        enc = d[i]
                        i += 1
                    elif c == 'L':
                        i += 1
                    elif c == 'P':
                        penc = d[i]
                        _, i = _encoded(d, i + 1, penc & 0x7f, va)
            cies[off] = enc
        else:
            enc = cies.get(off + 4 - cid)
            if enc is not None:
                start, i = _encoded(d, off + 8, enc, va)
                size, _ = _encoded(d, i, enc & 0x0f, va)
                out.append((start, size))
        off = end
    return out


def _obj_va(s):
    # Relocatable sections all start at 0; spread them apart so symbol lookups stay unambiguous.
    return s['index'] << 24


def _plt_names(mod, secs, symtabs, sdata):
    relplt = next((s for s in secs if s['name'] == '.rel.plt'), None)
    if not relplt:
        return
    syms = symtabs.get(relplt['link'], [])
    slot_name = {}
    rel = sdata(relplt)
    order = []
    for k in range(len(rel) // 8):
        r_off, r_info = struct.unpack_from('<II', rel, k * 8)
        if (r_info >> 8) < len(syms):
            n = canonical('elf', syms[r_info >> 8][0])
            slot_name[r_off] = n
            order.append(n)
    gotplt = next((s for s in secs if s['name'] == '.got.plt'), None)
    for s in secs:
        if not s['name'].startswith('.plt'):
            continue
        d = sdata(s)
        found = False
        for off in range(0, len(d) - 5, 2):
            if d[off] != 0xff or d[off + 1] not in (0x25, 0xa3):
                continue
            imm = struct.unpack_from('<I', d, off + 2)[0]
            slot = imm if d[off + 1] == 0x25 else ((gotplt['addr'] if gotplt else 0) + imm) & 0xffffffff
            if slot in slot_name:
                mod.add_name(s['addr'] + off, slot_name[slot])
                found = True
        if not found and s['name'] == '.plt':
            for i, n in enumerate(order):
                mod.add_name(s['addr'] + 16 * (i + 1), n)
