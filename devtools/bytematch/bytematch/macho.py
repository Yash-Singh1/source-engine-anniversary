"""Mach-O i386 (objects, dylibs, executables, fat files)."""
import struct

from .build import Entry, build_functions
from .model import Module, canonical, object_stem

MH_MAGIC = 0xfeedface
FAT_MAGIC = 0xcafebabe
CPU_TYPE_I386 = 7
MH_OBJECT = 1

LC_SEGMENT, LC_SYMTAB, LC_DYSYMTAB = 0x1, 0x2, 0xb
LC_DYLD_INFO, LC_DYLD_INFO_ONLY = 0x22, 0x80000022
LC_FUNCTION_STARTS, LC_DATA_IN_CODE = 0x26, 0x29

N_STAB, N_TYPE, N_SECT, N_EXT = 0xe0, 0x0e, 0x0e, 0x01
N_FUN, N_SO, N_OSO = 0x24, 0x64, 0x66

S_NON_LAZY, S_LAZY, S_STUBS = 6, 7, 8
S_ATTR_PURE, S_ATTR_SOME = 0x80000000, 0x400
INDIRECT_LOCAL, INDIRECT_ABS = 0x80000000, 0x40000000

GENERIC_RELOC_PAIR = 1
GENERIC_RELOC_SECTDIFF, GENERIC_RELOC_LOCAL_SECTDIFF = 2, 4


def is_macho(data):
    if len(data) < 8:
        return False
    if struct.unpack_from('<I', data)[0] == MH_MAGIC:
        return True
    if struct.unpack_from('>I', data)[0] == FAT_MAGIC:
        n = struct.unpack_from('>I', data, 4)[0]
        return 0 < n < 20          # Java class files share the magic but have a large version here
    return False


def thin(data):
    if struct.unpack_from('>I', data)[0] != FAT_MAGIC:
        return data
    n = struct.unpack_from('>I', data, 4)[0]
    for i in range(n):
        cpu, sub, off, size, align = struct.unpack_from('>iiIII', data, 8 + 20 * i)
        if cpu == CPU_TYPE_I386:
            return data[off:off + size]
    raise ValueError('fat file has no i386 slice')


def uleb(data, pos):
    val = shift = 0
    while True:
        b = data[pos]
        pos += 1
        val |= (b & 0x7f) << shift
        shift += 7
        if b < 0x80:
            return val, pos


def load(path, data, member=None):
    data = thin(data)
    magic, cpu, sub, ftype, ncmds, sizeofcmds, flags = struct.unpack_from('<IiiIIII', data)
    if magic != MH_MAGIC:
        raise ValueError('not a 32-bit Mach-O (64-bit images are not supported)')
    if cpu != CPU_TYPE_I386:
        raise ValueError(f'unsupported Mach-O cputype {cpu}')
    linked = ftype != MH_OBJECT
    mod = Module(path, 'macho', linked, member)
    segs, sects = [], []
    symtab = dysym = dyld = fstarts = None
    off = 28
    for _ in range(ncmds):
        cmd, size = struct.unpack_from('<II', data, off)
        if cmd == LC_SEGMENT:
            segname = data[off + 8:off + 24].rstrip(b'\0').decode()
            vmaddr, vmsize, fileoff, filesize, maxprot, initprot, nsects, sflags = struct.unpack_from('<8I', data, off + 24)
            segs.append((segname, vmaddr, vmsize))
            for k in range(nsects):
                so = off + 56 + 68 * k
                sn = data[so:so + 16].rstrip(b'\0').decode()
                sg = data[so + 16:so + 32].rstrip(b'\0').decode()
                addr, ssize, soff, align, reloff, nreloc, sfl, r1, r2 = struct.unpack_from('<9I', data, so + 32)
                sects.append(dict(name=sn, seg=sg, addr=addr, size=ssize, offset=soff, reloff=reloff,
                                  nreloc=nreloc, flags=sfl, r1=r1, r2=r2, index=len(sects) + 1))
        elif cmd == LC_SYMTAB:
            symtab = struct.unpack_from('<4I', data, off + 8)
        elif cmd == LC_DYSYMTAB:
            dysym = struct.unpack_from('<18I', data, off + 8)
        elif cmd in (LC_DYLD_INFO, LC_DYLD_INFO_ONLY):
            dyld = struct.unpack_from('<10I', data, off + 8)
        elif cmd == LC_FUNCTION_STARTS:
            fstarts = struct.unpack_from('<2I', data, off + 8)
        off += size

    def sdata(s):
        if s['offset'] == 0 or (s['flags'] & 0xff) in (1, 0xc, 0x12):   # zerofill
            return b''
        return data[s['offset']:s['offset'] + s['size']]

    if segs:
        lo = min(v for n, v, s in segs if n != '__PAGEZERO') if any(n != '__PAGEZERO' for n, _, _ in segs) else 0
        mod.image_base = lo
        mod.image_end = max(v + s for _, v, s in segs)

    # symbols
    syms = []
    if symtab:
        symoff, nsyms, stroff, strsize = symtab
        strs = data[stroff:stroff + strsize]
        for i, (strx, ntype, nsect, ndesc, nvalue) in enumerate(struct.iter_unpack('<IBBhI', data[symoff:symoff + 12 * nsyms])):
            end = strs.find(b'\0', strx)
            syms.append((strs[strx:end].decode('latin-1'), ntype, nsect, ndesc, nvalue))

    code_secs = [s for s in sects if s['flags'] & (S_ATTR_PURE | S_ATTR_SOME)
                 and (s['flags'] & 0xff) != S_STUBS and s['name'] != '__stub_helper']
    code_idx = {s['index'] for s in code_secs}
    for s in sects:
        if s['flags'] & (S_ATTR_PURE | S_ATTR_SOME):
            mod.code.append((s['addr'], sdata(s), s['name']))

    # indirect symbols: stubs and pointer tables
    if dysym:
        indoff, nind = dysym[12], dysym[13]
        ind = struct.unpack_from(f'<{nind}I', data, indoff) if nind else ()
        for s in sects:
            t = s['flags'] & 0xff
            if t not in (S_NON_LAZY, S_LAZY, S_STUBS):
                continue
            esz = s['r2'] if t == S_STUBS else 4
            if not esz:
                continue
            for k in range(s['size'] // esz):
                if s['r1'] + k >= len(ind):
                    break
                si = ind[s['r1'] + k]
                if si & (INDIRECT_LOCAL | INDIRECT_ABS) or si >= len(syms):
                    continue
                mod.add_name(s['addr'] + k * esz, canonical('macho', syms[si][0]))

    # stabs debug map: object stem and size per function
    stab_obj, stab_size, stab_path = {}, {}, {}
    cur_obj = cur_path = pending = None
    mod.debug_map = []                       # [(object path, mtime)] in link order
    for name, ntype, nsect, ndesc, nvalue in syms:
        if not ntype & N_STAB:
            continue
        if ntype == N_OSO:
            cur_obj, cur_path = object_stem(name), name
            mod.debug_map.append((name, nvalue))
        elif ntype == N_FUN:
            if name:
                pending = nvalue
                stab_obj[nvalue] = cur_obj
                stab_path[nvalue] = cur_path
            elif pending is not None:
                stab_size[pending] = nvalue
                pending = None

    own_stem = object_stem(member or path) if not linked else None
    entries = {s['index']: [] for s in code_secs}
    for name, ntype, nsect, ndesc, nvalue in syms:
        if ntype & N_STAB or (ntype & N_TYPE) != N_SECT:
            continue
        mod.add_name(nvalue, canonical('macho', name))
        if nsect not in code_idx or name[:1] in ('L', 'l'):
            continue
        entries[nsect].append(Entry(nvalue, [(bool(ntype & N_EXT), name)], stab_size.get(nvalue),
                                    obj=stab_obj.get(nvalue, own_stem)))
    if fstarts and linked:
        foff, fsize = fstarts
        text_base = next((v for n, v, s in segs if n == '__TEXT'), 0)
        pos, addr = foff, text_base
        while pos < foff + fsize:
            delta, pos = uleb(data, pos)
            if not delta:
                break
            addr += delta
            for s in code_secs:
                if s['addr'] <= addr < s['addr'] + s['size']:
                    entries[s['index']].append(Entry(addr, [], stab_size.get(addr), obj=stab_obj.get(addr)))
                    break
    for addr, sz in stab_size.items():
        for s in code_secs:
            if s['addr'] <= addr < s['addr'] + s['size']:
                entries[s['index']].append(Entry(addr, [], sz, obj=stab_obj.get(addr)))
                break

    fields = {s['index']: [] for s in code_secs}
    if linked:
        for addr in _rebases(data, dyld, segs) if dyld else ():
            for s in code_secs:
                if s['addr'] <= addr < s['addr'] + s['size']:
                    fields[s['index']].append((addr, 4, 'rebase', None, 0))
        if dysym:
            for roff, n in ((dysym[16], dysym[17]), (dysym[14], dysym[15])):   # locrel, extrel
                for k in range(n):
                    a, w = struct.unpack_from('<II', data, roff + 8 * k)
                    if a & 0x80000000:
                        continue
                    addr = (segs[0][1] if segs else 0) + a
                    for s in code_secs:
                        if s['addr'] <= addr < s['addr'] + s['size']:
                            fields[s['index']].append((addr, 1 << ((w >> 25) & 3), 'abs', None, 0))
    else:
        for s in code_secs:
            fields[s['index']] = _object_relocs(data, s, sects, syms, mod)

    for s in code_secs:
        build_functions(mod, s['name'], s['addr'], sdata(s), entries[s['index']], fields[s['index']])
    mod.obj_paths = stab_path
    if linked:
        for f in mod.functions:
            f.obj_path = stab_path.get(f.addr)
            for a in f.aliases:
                mod.add_name(f.addr, canonical('macho', a))
    return mod


def _object_relocs(data, s, sects, syms, mod):
    out = []
    sdat = data[s['offset']:s['offset'] + s['size']]
    for k in range(s['nreloc']):
        w0, w1 = struct.unpack_from('<II', data, s['reloff'] + 8 * k)
        if w0 & 0x80000000:                                   # scattered
            raddr = w0 & 0xffffff
            rtype = (w0 >> 24) & 0xf
            length = (w0 >> 28) & 3
            pcrel = (w0 >> 30) & 1
            if rtype == GENERIC_RELOC_PAIR:
                continue
            names = mod.resolve(w1)
            target = names[0] if names else None
            kind = 'pic' if rtype in (GENERIC_RELOC_SECTDIFF, GENERIC_RELOC_LOCAL_SECTDIFF) else ('pcrel' if pcrel else 'abs')
        else:
            raddr = w0
            symnum = w1 & 0xffffff
            pcrel = (w1 >> 24) & 1
            length = (w1 >> 25) & 3
            extern = (w1 >> 27) & 1
            rtype = (w1 >> 28) & 0xf
            if rtype == GENERIC_RELOC_PAIR:
                continue
            width = 1 << length
            target = None
            if extern and symnum < len(syms):
                target = canonical('macho', syms[symnum][0])
            elif raddr + width <= len(sdat) and width == 4:
                v = struct.unpack_from('<i', sdat, raddr)[0]
                tva = (s['addr'] + raddr + 4 + v) & 0xffffffff if pcrel else v & 0xffffffff
                names = mod.resolve(tva)
                target = names[0] if names else None
            kind = 'pcrel' if pcrel else 'abs'
        width = 1 << length
        addend = int.from_bytes(sdat[raddr:raddr + width], 'little', signed=True) if raddr + width <= len(sdat) else 0
        out.append((s['addr'] + raddr, width, kind, target, addend))
    return out


def _rebases(data, dyld, segs):
    off, size = dyld[0], dyld[1]
    if not size:
        return []
    out = []
    pos, end = off, off + size
    seg = addr = 0
    while pos < end:
        b = data[pos]
        pos += 1
        op, imm = b & 0xf0, b & 0x0f
        if op == 0x00:
            break
        elif op == 0x10:
            pass
        elif op == 0x20:
            seg = imm
            v, pos = uleb(data, pos)
            addr = segs[seg][1] + v
        elif op == 0x30:
            v, pos = uleb(data, pos)
            addr = (addr + v) & 0xffffffff
        elif op == 0x40:
            addr += imm * 4
        elif op == 0x50:
            for _ in range(imm):
                out.append(addr)
                addr += 4
        elif op == 0x60:
            n, pos = uleb(data, pos)
            for _ in range(n):
                out.append(addr)
                addr += 4
        elif op == 0x70:
            out.append(addr)
            v, pos = uleb(data, pos)
            addr += 4 + v
        elif op == 0x80:
            n, pos = uleb(data, pos)
            skip, pos = uleb(data, pos)
            for _ in range(n):
                out.append(addr)
                addr += 4 + skip
    return out
