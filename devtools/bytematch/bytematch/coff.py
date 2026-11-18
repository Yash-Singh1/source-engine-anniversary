"""COFF i386 objects (.obj, incl. /bigobj) and PE32 images (.dll/.exe)."""
import struct

from .build import Entry, build_functions
from .model import Module, canonical, object_stem

IMAGE_FILE_MACHINE_I386 = 0x14c
SCN_CNT_CODE, SCN_LNK_COMDAT, SCN_MEM_EXECUTE = 0x20, 0x1000, 0x20000000
BIGOBJ_CLSID = bytes.fromhex('c7a1bad1eebaa94baf20faf66aa4dcb8')

# i386 relocation type -> (width, kind)
_RELOC = {1: (2, 'abs'), 2: (2, 'pcrel'), 6: (4, 'abs'), 7: (4, 'abs'), 9: (4, 'abs'), 0xa: (2, 'secrel'),
          0xb: (4, 'secrel'), 0xc: (4, 'abs'), 0xd: (1, 'secrel'), 0x14: (4, 'pcrel')}


def is_pe(data):
    if data[:2] != b'MZ' or len(data) < 0x40:
        return False
    lfanew = struct.unpack_from('<I', data, 0x3c)[0]
    return data[lfanew:lfanew + 4] == b'PE\0\0'


def is_coff_obj(data):
    if len(data) < 20:
        return False
    machine, nsec = struct.unpack_from('<HH', data)
    if machine == IMAGE_FILE_MACHINE_I386 and 0 < nsec < 0xff00:
        return True
    sig1, sig2 = struct.unpack_from('<HH', data)
    return sig1 == 0 and sig2 == 0xffff


def _strtab_name(raw, strtab):
    if raw[:4] == b'\0\0\0\0':
        off = struct.unpack_from('<I', raw, 4)[0]
        return strtab[off:strtab.find(b'\0', off)].decode('latin-1')
    return raw.rstrip(b'\0').decode('latin-1')


def _read_symbols(data, symptr, nsym, symsz, big):
    """[(index, name, value, secno, type, class, naux, aux_bytes)]"""
    strtab = data[symptr + nsym * symsz:]
    out = []
    i = 0
    fmt = '<IiHBB' if big else '<IhHBB'
    while i < nsym:
        o = symptr + i * symsz
        raw = data[o:o + 8]
        val, sec, typ, cls, naux = struct.unpack_from(fmt, data, o + 8)
        name = _strtab_name(raw, strtab)
        aux = data[o + symsz:o + symsz * (1 + naux)]
        out.append((i, name, val, sec, typ, cls, naux, aux))
        i += 1 + naux
    return out, strtab


def load_obj(path, data, member=None):
    sig1, sig2 = struct.unpack_from('<HH', data)
    big = False
    if sig1 == 0 and sig2 == 0xffff:
        version = struct.unpack_from('<H', data, 4)[0]
        if version >= 2 and data[12:28] == BIGOBJ_CLSID:
            big = True
            machine = struct.unpack_from('<H', data, 6)[0]
            nsec, symptr, nsym = struct.unpack_from('<III', data, 44)
            hdr, symsz = 56, 20
        elif version == 0:
            raise ValueError('import-library member, no code')
        else:
            raise ValueError('anonymous COFF object (likely /GL LTCG IL, not machine code)')
    else:
        machine, nsec, ts, symptr, nsym, opt, ch = struct.unpack_from('<HHIIIHH', data)
        hdr, symsz = 20 + opt, 18
    if machine != IMAGE_FILE_MACHINE_I386:
        raise ValueError(f'unsupported COFF machine {machine:#x}')
    mod = Module(path, 'coff', False, member)
    syms, strtab = _read_symbols(data, symptr, nsym, symsz, big)
    secs = []
    for i in range(nsec):
        o = hdr + 40 * i
        raw = data[o:o + 8]
        vs, va, size, ptr, rptr, lptr, nrel, nln, chars = struct.unpack_from('<IIIIIIHHI', data, o + 8)
        name = raw.rstrip(b'\0').decode('latin-1')
        if name.startswith('/') and name[1:].isdigit():
            name = _strtab_name(b'\0\0\0\0' + struct.pack('<I', int(name[1:])), strtab)
        if chars & 0x01000000 and nrel == 0xffff:          # IMAGE_SCN_LNK_NRELOC_OVFL
            nrel = struct.unpack_from('<I', data, rptr)[0]
            rptr += 10
            nrel -= 1
        secs.append(dict(name=name, size=size, data=data[ptr:ptr + size] if ptr else b'', rptr=rptr,
                         nrel=nrel, chars=chars, index=i + 1, va=(i + 1) << 24))
    stem = object_stem(member or path)
    by_index = {s[0]: s for s in syms}
    comdat_sel = {}
    for idx, name, val, sec, typ, cls, naux, aux in syms:
        if 0 < sec <= nsec and cls == 3 and naux and val == 0 and name == secs[sec - 1]['name'] and len(aux) >= 15:
            comdat_sel.setdefault(sec, aux[14])
        if 0 < sec <= nsec:
            mod.add_name(secs[sec - 1]['va'] + val, canonical('coff', name))
    for s in secs:
        if s['chars'] & (SCN_CNT_CODE | SCN_MEM_EXECUTE):
            mod.code.append((s['va'], s['data'], s['name']))

    for s in secs:
        if not s['chars'] & (SCN_CNT_CODE | SCN_MEM_EXECUTE) or not s['size']:
            continue
        sec_no = s['index']
        comdat = bool(s['chars'] & SCN_LNK_COMDAT)
        ents = []
        for idx, name, val, sec, typ, cls, naux, aux in syms:
            if sec != sec_no or cls not in (2, 3) or (cls == 3 and naux and name == s['name']):
                continue
            if (typ >> 4) != 2 and not (cls == 2 and not ents):
                continue
            ents.append(Entry(s['va'] + val, [(cls == 2, name)], obj=stem, comdat=comdat))
        if not ents:
            continue
        func_starts = sorted({e.addr for e in ents})
        if len(func_starts) == 1:
            # gcc pads COMDAT sections to their alignment with NOPs, so still trim
            for e in ents:
                e.size = s['size'] - (e.addr - s['va'])
        fields, own = [], []
        for k in range(s['nrel']):
            va, si, ty = struct.unpack_from('<IIH', data, s['rptr'] + 10 * k)
            if ty not in _RELOC:
                continue
            width, kind = _RELOC[ty]
            sym = by_index.get(si)
            target = canonical('coff', sym[1]) if sym else None
            if sym and sym[5] == 3 and sym[2] == 0 and sym[6] and 0 < sym[3] <= nsec and sym[1] == secs[sym[3] - 1]['name']:
                target = None    # section-relative reference (static data/label), name is meaningless
            addend = int.from_bytes(s['data'][va:va + width], 'little', signed=True)
            fields.append((s['va'] + va, width, kind, target, addend))
            if ty == 6 and sym and sym[3] == sec_no:
                own.append((va, sym[2] + addend))
        # MSVC emits a switch's jump table (and byte index table) into the function's own
        # section after its code; a linked image's function extent ends before it.
        locs = {l for l, _ in own}
        for t in sorted({t for l, t in own if t in locs and t > l}, reverse=True):
            owner = max((e for e in ents if e.addr - s['va'] < t), key=lambda e: e.addr, default=None)
            if owner is not None:
                off = owner.addr - s['va']
                if owner.size is None or off + owner.size > t:
                    owner.size = t - off
        build_functions(mod, s['name'], s['va'], s['data'], ents, fields)
    return mod


def load_pe(path, data, member=None):
    lfanew = struct.unpack_from('<I', data, 0x3c)[0]
    machine, nsec, ts, symptr, nsym, optsz, chars = struct.unpack_from('<HHIIIHH', data, lfanew + 4)
    if machine != IMAGE_FILE_MACHINE_I386:
        raise ValueError(f'unsupported PE machine {machine:#x}')
    opt = lfanew + 24
    if struct.unpack_from('<H', data, opt)[0] != 0x10b:
        raise ValueError('not a PE32 image')
    image_base, = struct.unpack_from('<I', data, opt + 28)
    size_image, = struct.unpack_from('<I', data, opt + 56)
    ndirs, = struct.unpack_from('<I', data, opt + 92)
    dirs = [struct.unpack_from('<II', data, opt + 96 + 8 * i) for i in range(min(ndirs, 16))]
    mod = Module(path, 'pe', True, member)
    mod.image_base, mod.image_end = image_base, image_base + size_image
    secs = []
    for i in range(nsec):
        o = opt + optsz + 40 * i
        name = data[o:o + 8].rstrip(b'\0').decode('latin-1')
        vs, va, rawsz, rawptr = struct.unpack_from('<IIII', data, o + 8)
        sch, = struct.unpack_from('<I', data, o + 36)
        body = data[rawptr:rawptr + min(rawsz, vs or rawsz)]
        secs.append(dict(name=name, va=va, vsize=vs, data=body, chars=sch, index=i + 1))
    code_secs = [s for s in secs if s['chars'] & (SCN_CNT_CODE | SCN_MEM_EXECUTE)]
    for s in code_secs:
        mod.code.append((image_base + s['va'], s['data'], s['name']))

    def rva_bytes(rva, n):
        for s in secs:
            if s['va'] <= rva < s['va'] + max(s['vsize'], len(s['data'])):
                o = rva - s['va']
                return s['data'][o:o + n]
        return b''

    def cstr(rva):
        b = rva_bytes(rva, 512)
        return b[:b.find(b'\0')].decode('latin-1') if b'\0' in b else b.decode('latin-1')

    def in_code(rva):
        return any(s['va'] <= rva < s['va'] + len(s['data']) for s in code_secs)

    entries = {s['index']: [] for s in code_secs}

    def add_entry(rva, names, bounded=False, size=None):
        for s in code_secs:
            if s['va'] <= rva < s['va'] + len(s['data']):
                entries[s['index']].append(Entry(image_base + rva, names, size, bounded=bounded or bool(size)))
                return

    # exports
    if len(dirs) > 0 and dirs[0][1]:
        erva, esz = dirs[0]
        ed = rva_bytes(erva, 40)
        if len(ed) == 40:
            base, nfunc, nnames, afuncs, anames, aords = struct.unpack_from('<IIIIII', ed, 16)
            funcs = struct.unpack('<%dI' % nfunc, rva_bytes(afuncs, 4 * nfunc))
            names = struct.unpack('<%dI' % nnames, rva_bytes(anames, 4 * nnames))
            ords = struct.unpack('<%dH' % nnames, rva_bytes(aords, 2 * nnames))
            named = {}
            for n, o in zip(names, ords):
                named.setdefault(o, []).append(cstr(n))
            for i, rva in enumerate(funcs):
                if not rva or erva <= rva < erva + esz:
                    continue
                nms = named.get(i, [f'ord_{base + i}'])
                for n in nms:
                    mod.add_name(image_base + rva, canonical('pe', n))
                if in_code(rva):
                    add_entry(rva, [(True, n) for n in nms])

    # imports (IAT slot -> imported name)
    def walk_thunks(int_rva, iat_rva, dll):
        k = 0
        while True:
            ent = rva_bytes(int_rva + 4 * k, 4)
            if len(ent) < 4:
                break
            v, = struct.unpack('<I', ent)
            if not v:
                break
            name = f'{dll}#{v & 0xffff}' if v & 0x80000000 else cstr(v + 2)
            mod.add_name(image_base + iat_rva + 4 * k, canonical('pe', name))
            k += 1

    if len(dirs) > 1 and dirs[1][1]:
        rva = dirs[1][0]
        while True:
            d = rva_bytes(rva, 20)
            if len(d) < 20:
                break
            oft, ts_, fc, name_rva, ft = struct.unpack('<IIIII', d)
            if not (oft or ft):
                break
            walk_thunks(oft or ft, ft, cstr(name_rva))
            rva += 20
    if len(dirs) > 13 and dirs[13][1]:
        rva = dirs[13][0]
        while True:
            d = rva_bytes(rva, 32)
            if len(d) < 32:
                break
            attrs, dll, hmod, iat, int_, *_ = struct.unpack('<8I', d)
            if not dll:
                break
            fix = 0 if attrs & 1 else image_base
            walk_thunks(int_ - fix, iat - fix, cstr(dll - fix))
            rva += 32

    # base relocations: exact list of absolute-address fields
    fields = {s['index']: [] for s in code_secs}
    if len(dirs) > 5 and dirs[5][1]:
        rva, end = dirs[5][0], dirs[5][0] + dirs[5][1]
        while rva < end:
            hdr = rva_bytes(rva, 8)
            if len(hdr) < 8:
                break
            page, bsize = struct.unpack('<II', hdr)
            if bsize < 8:
                break
            body = rva_bytes(rva + 8, bsize - 8)
            for (e,) in struct.iter_unpack('<H', body[:len(body) & ~1]):
                if e >> 12 == 3:
                    frva = page + (e & 0xfff)
                    for s in code_secs:
                        if s['va'] <= frva < s['va'] + len(s['data']):
                            fields[s['index']].append((image_base + frva, 4, 'abs', None, 0))
                            break
            rva += bsize

    # COFF symbol table (mingw keeps one unless stripped)
    if symptr and nsym:
        syms, _ = _read_symbols(data, symptr, nsym, 18, False)
        for idx, name, val, sec, typ, cls, naux, aux in syms:
            if not 0 < sec <= len(secs) or cls not in (2, 3):
                continue
            s = secs[sec - 1]
            va = image_base + s['va'] + val
            mod.add_name(va, canonical('pe', name))
            if (typ >> 4) == 2 and s in code_secs:
                entries[s['index']].append(Entry(va, [(cls == 2, name)]))
    for s in code_secs:
        build_functions(mod, s['name'], image_base + s['va'], s['data'], entries[s['index']], fields[s['index']])
    return mod
