"""Data audit for Mach-O i386: string literals, constant data, and the data
behind the references that function matching masks.

Function matching masks PIC displacements, so a function that loads a
different string (or global, or float constant) at the same offset still
counts as matching. This module resolves those references on both sides to a
comparable identity (string text, symbol name, pointee of a pointer slot,
literal bytes) and compares the __cstring, __const and __data contents of two
modules."""
import bisect
import collections
import difflib
import re
import struct

from . import macho, x86
from .model import canonical, object_stem

S_ZEROFILL = (0x1, 0xc, 0x12)
S_CSTRING = 0x2
S_NON_LAZY, S_LAZY, S_STUBS = 0x6, 0x7, 0x8
N_STAB, N_TYPE, N_SECT, N_EXT = 0xe0, 0x0e, 0x0e, 0x01
N_GSYM, N_FUN, N_STSYM, N_LCSYM, N_SO, N_OSO, N_SOL = 0x20, 0x24, 0x26, 0x28, 0x64, 0x66, 0x84
GENERIC_RELOC_PAIR, GENERIC_RELOC_SECTDIFF, GENERIC_RELOC_LOCAL_SECTDIFF = 1, 2, 4
INDIRECT_LOCAL, INDIRECT_ABS = 0x80000000, 0x40000000

# Data sections whose symbols are compared by content.
DATA_SECTIONS = (('__DATA', '__data'), ('__DATA', '__const'), ('__TEXT', '__const'))


class Section:
    __slots__ = ('seg', 'name', 'addr', 'size', 'offset', 'data', 'type', 'index', 'reloff', 'nreloc', 'r1', 'r2')

    def __init__(self, **kw):
        for k, v in kw.items():
            setattr(self, k, v)

    @property
    def end(self):
        return self.addr + self.size

    @property
    def code(self):
        return self.seg == '__TEXT' and self.name in ('__text', '__textcoal_nt', '__StaticInit')


class Image:
    """Sections, symbols, pointer slots and debug map of a Mach-O i386 file."""

    def __init__(self, path, data=None):
        if data is None:
            with open(path, 'rb') as fh:
                data = fh.read()
        self.path = path
        data = macho.thin(data)
        self.raw = data
        magic, cpu, sub, ftype, ncmds, sizeofcmds, flags = struct.unpack_from('<IiiIIII', data)
        if magic != macho.MH_MAGIC or cpu != macho.CPU_TYPE_I386:
            raise ValueError(f'{path}: not a Mach-O i386 file')
        self.linked = ftype != macho.MH_OBJECT
        self.absolute = ftype == 2 and not flags & 0x200000      # MH_EXECUTE without MH_PIE: no fixups
        self.sections, self.segs = [], []
        symtab = dysym = dyld = None
        off = 28
        for _ in range(ncmds):
            cmd, size = struct.unpack_from('<II', data, off)
            if cmd == macho.LC_SEGMENT:
                segname = data[off + 8:off + 24].rstrip(b'\0').decode()
                vmaddr, vmsize, fileoff, filesize, maxprot, initprot, nsects, sflags = struct.unpack_from('<8I', data, off + 24)
                self.segs.append((segname, vmaddr, vmsize))
                for k in range(nsects):
                    so = off + 56 + 68 * k
                    sn = data[so:so + 16].rstrip(b'\0').decode()
                    sg = data[so + 16:so + 32].rstrip(b'\0').decode()
                    addr, ssize, soff, align, reloff, nreloc, sfl, r1, r2 = struct.unpack_from('<9I', data, so + 32)
                    t = sfl & 0xff
                    body = None if t in S_ZEROFILL or not soff else data[soff:soff + ssize]
                    self.sections.append(Section(seg=sg, name=sn, addr=addr, size=ssize, offset=soff, data=body, type=t,
                                                 index=len(self.sections) + 1, reloff=reloff, nreloc=nreloc,
                                                 r1=r1, r2=r2))
            elif cmd == macho.LC_SYMTAB:
                symtab = struct.unpack_from('<4I', data, off + 8)
            elif cmd == macho.LC_DYSYMTAB:
                dysym = struct.unpack_from('<18I', data, off + 8)
            elif cmd in (macho.LC_DYLD_INFO, macho.LC_DYLD_INFO_ONLY):
                dyld = struct.unpack_from('<10I', data, off + 8)
            off += size
        self._sec_starts = sorted((s.addr, i) for i, s in enumerate(self.sections) if s.size)

        self.syms = []
        if symtab:
            symoff, nsyms, stroff, strsize = symtab
            strs = data[stroff:stroff + strsize]
            for strx, ntype, nsect, ndesc, nvalue in struct.iter_unpack('<IBBhI', data[symoff:symoff + 12 * nsyms]):
                end = strs.find(b'\0', strx)
                self.syms.append((strs[strx:end].decode('latin-1'), ntype, nsect, ndesc, nvalue))

        # defined symbols: address -> raw names; per-section sorted starts
        self.names = collections.defaultdict(list)
        self.sym_ext = {}
        per_sec = collections.defaultdict(set)
        for name, ntype, nsect, ndesc, nvalue in self.syms:
            if ntype & N_STAB or (ntype & N_TYPE) != N_SECT or not name or name.startswith('L'):
                continue
            if name not in self.names[nvalue]:
                self.names[nvalue].append(name)
            self.sym_ext[name] = bool(ntype & N_EXT)
            per_sec[nsect].add(nvalue)
        self.sec_syms = {k: sorted(v) for k, v in per_sec.items()}

        self._debug_map()
        self.indirect = {}                    # pointer/stub slot -> raw name (None: local, read the slot)
        if dysym:
            indoff, nind = dysym[12], dysym[13]
            ind = struct.unpack_from(f'<{nind}I', data, indoff) if nind else ()
            for s in self.sections:
                if s.type not in (S_NON_LAZY, S_LAZY, S_STUBS):
                    continue
                esz = s.r2 if s.type == S_STUBS else 4
                if not esz:
                    continue
                for k in range(s.size // esz):
                    if s.r1 + k >= len(ind):
                        break
                    si = ind[s.r1 + k]
                    local = si & (INDIRECT_LOCAL | INDIRECT_ABS) or si >= len(self.syms)
                    self.indirect[s.addr + k * esz] = None if local else self.syms[si][0]
        # pointer slots in data: addr -> ('rebase', None) | ('bind', raw name)
        self.slots = {}
        self.code_fields = {}                 # objects: code field VA -> (target VA or None, raw name or None)
        if self.linked:
            if dyld:
                for a in macho._rebases(data, dyld, self.segs):
                    self.slots[a] = ('rebase', None)
                for a, name in _binds(data, dyld[2], dyld[3], self.segs):
                    self.slots[a] = ('bind', name)
                for a, name in _binds(data, dyld[4], dyld[5], self.segs):
                    self.slots.setdefault(a, ('bind', name))
            if dysym and not (dyld and dyld[1]):
                # 10.5-era images: classic local (rebase) and external (bind) relocations,
                # addresses relative to the first segment
                base = self.segs[0][1] if self.segs else 0
                for roff, n, ext in ((dysym[16], dysym[17], False), (dysym[14], dysym[15], True)):
                    for k in range(n):
                        w0, w1 = struct.unpack_from('<II', data, roff + 8 * k)
                        if w0 & 0x80000000:
                            self.slots.setdefault(base + (w0 & 0xffffff), ('rebase', None))
                        elif ext and (w1 >> 27) & 1 and (w1 & 0xffffff) < len(self.syms):
                            self.slots[base + w0] = ('bind', self.syms[w1 & 0xffffff][0])
                        else:
                            self.slots.setdefault(base + w0, ('rebase', None))
            if self.absolute:
                self._absolute_slots()
        else:
            self._object_relocs()
        self._cstr_cache = {}

    # -- debug map -------------------------------------------------------
    def _debug_map(self):
        """objects: [dict(path, source, headers)]; addr/name -> object index."""
        self.objects = []
        self.addr_obj, self.name_obj = {}, {}
        so_dir = so_name = None
        cur = None
        for name, ntype, nsect, ndesc, nvalue in self.syms:
            if not ntype & N_STAB:
                continue
            if ntype == N_SO:
                if not name:
                    so_dir = so_name = None
                    cur = None
                elif name.endswith('/'):
                    so_dir = name
                else:
                    so_name = name
            elif ntype == N_OSO:
                cur = len(self.objects)
                src = (so_dir or '') + (so_name or '')
                self.objects.append(dict(path=name, stem=object_stem(name), source=src, headers=[]))
            elif cur is None:
                continue
            elif ntype == N_SOL:
                self.objects[cur]['headers'].append(name)
            elif ntype in (N_FUN, N_STSYM, N_LCSYM) and name:
                self.addr_obj[nvalue] = cur
            elif ntype == N_GSYM and name:
                self.name_obj[name] = cur
        if not self.linked:
            self.objects = [dict(path=self.path, stem=object_stem(self.path), source='', headers=[])]

    def has_name(self, key):
        """Whether a canonical name (linkage ignored) is defined in this image."""
        if not hasattr(self, '_name_set'):
            self._name_set = {unlink(canonical('macho', n)) for ns in self.names.values() for n in ns}
        return unlink(key) in self._name_set

    def object_of(self, addr=None, name=None):
        i = self.addr_obj.get(addr) if addr is not None else None
        if i is None and name:
            i = self.name_obj.get(name)
        if i is None and not self.linked:
            i = 0
        return self.objects[i] if i is not None else None

    def in_image(self, v):
        s = self.section_at(v)
        return s is not None and s.seg != '__PAGEZERO'

    def _absolute_slots(self):
        """Non-PIE executables carry no rebase info: aligned words in data that hold
        an address inside the image are taken as pointers."""
        for s in self.sections:
            if s.data is None or s.code or s.type in (S_CSTRING, S_NON_LAZY, S_LAZY, S_STUBS):
                continue
            for k in range(0, len(s.data) - 3, 4):
                v = struct.unpack_from('<I', s.data, k)[0]
                if v > 0x1000 and self.in_image(v):
                    self.slots.setdefault(s.addr + k, ('rebase', None))

    # -- object relocations ------------------------------------------------
    def _object_relocs(self):
        d = self.raw
        for s in self.sections:
            if not s.nreloc or s.data is None:
                continue
            k = 0
            while k < s.nreloc:
                w0, w1 = struct.unpack_from('<II', d, s.reloff + 8 * k)
                k += 1
                if w0 & 0x80000000:
                    raddr, rtype, length = w0 & 0xffffff, (w0 >> 24) & 0xf, (w0 >> 28) & 3
                    pcrel = (w0 >> 30) & 1
                    if rtype == GENERIC_RELOC_PAIR:
                        continue
                    va = s.addr + raddr
                    if rtype in (GENERIC_RELOC_SECTDIFF, GENERIC_RELOC_LOCAL_SECTDIFF) and k < s.nreloc:
                        p0, p1 = struct.unpack_from('<II', d, s.reloff + 8 * k)
                        k += 1
                        base = p1                       # the PAIR's r_value: the subtrahend (PIC base)
                        val = struct.unpack_from('<i', s.data, raddr)[0] if length == 2 else 0
                        tgt = (val + base) & 0xffffffff
                    elif length == 2:
                        v = struct.unpack_from('<i', s.data, raddr)[0]
                        tgt = (v + va + 4) & 0xffffffff if pcrel else v & 0xffffffff
                    else:
                        continue
                    self._add_field(s, va, tgt, None)
                else:
                    raddr = w0
                    symnum, pcrel, length = w1 & 0xffffff, (w1 >> 24) & 1, (w1 >> 25) & 3
                    extern, rtype = (w1 >> 27) & 1, (w1 >> 28) & 0xf
                    if rtype == GENERIC_RELOC_PAIR or length != 2:
                        continue
                    va = s.addr + raddr
                    if extern:
                        self._add_field(s, va, None, self.syms[symnum][0] if symnum < len(self.syms) else None)
                    else:
                        v = struct.unpack_from('<i', s.data, raddr)[0]
                        tgt = (v + va + 4) & 0xffffffff if pcrel else v & 0xffffffff
                        self._add_field(s, va, tgt, None)

    def _add_field(self, s, va, tgt, name):
        if s.code:
            self.code_fields[va] = (tgt, name)
        elif name:
            self.slots[va] = ('bind', name)
        else:
            self.slots[va] = ('rebase', None)

    # -- lookup ------------------------------------------------------------
    def section_at(self, addr):
        i = bisect.bisect_right(self._sec_starts, (addr, 1 << 30)) - 1
        if i < 0:
            return None
        s = self.sections[self._sec_starts[i][1]]
        return s if s.addr <= addr < s.end else None

    def read(self, addr, n):
        s = self.section_at(addr)
        if s is None or s.data is None:
            return b''
        return s.data[addr - s.addr:addr - s.addr + n]

    def u32(self, addr):
        b = self.read(addr, 4)
        return struct.unpack('<I', b)[0] if len(b) == 4 else None

    def cstring_at(self, addr):
        s = self.section_at(addr)
        if s is None or s.type != S_CSTRING or s.data is None:
            return None
        o = addr - s.addr
        e = s.data.find(b'\0', o)
        return s.data[o:e if e >= 0 else len(s.data)]

    def cstrings(self):
        """[(addr, bytes)] of every literal in cstring sections, in address order."""
        out = []
        for s in self.sections:
            if s.type != S_CSTRING or s.data is None:
                continue
            pos = 0
            while pos < len(s.data):
                e = s.data.find(b'\0', pos)
                if e < 0:
                    e = len(s.data)
                out.append((s.addr + pos, s.data[pos:e]))
                pos = e + 1
        return out

    def string_start(self, addr):
        """Start of the literal containing addr (literals are NUL-separated)."""
        s = self.section_at(addr)
        if s is None or s.type != S_CSTRING or s.data is None:
            return None
        o = addr - s.addr
        p = s.data.rfind(b'\0', 0, o)
        return s.addr + p + 1

    def symbol_at(self, addr, sec=None):
        """(raw name, offset) of the nearest symbol at or below addr in its section."""
        sec = sec or self.section_at(addr)
        if sec is None:
            return None, 0
        starts = self.sec_syms.get(sec.index, [])
        i = bisect.bisect_right(starts, addr) - 1
        if i < 0:
            return None, 0
        a = starts[i]
        return _pick(self.names[a]), addr - a

    def pointee(self, slot):
        """Identity of what a data pointer slot points at."""
        kind, name = self.slots.get(slot, (None, None))
        if kind == 'bind':
            return ('ptr', canonical('macho', name))
        v = self.u32(slot)
        if v is None:
            return None
        if self.linked:
            return self.ident(v)
        return self.ident(v)

    def ident(self, addr, size=0):
        """Comparable identity of the data at addr:
        ('str', text) | ('cfstr', text) | ('ptr', name) | ('code', name, off) |
        ('sym', name, off) | ('lit', bytes) | ('?', section)."""
        s = self.section_at(addr)
        if s is None:
            return ('?', None)
        if s.type == S_CSTRING:
            return ('str', self.cstring_at(addr))
        if s.name == '__cfstring':
            base = addr - (addr - s.addr) % 16
            p = self.u32(base + 8)
            if not self.linked:
                t = self.slots.get(base + 8)
                p = p if t and t[0] == 'rebase' else None
            txt = self.cstring_at(p) if p is not None else None
            return ('cfstr', txt)
        if s.type in (S_NON_LAZY, S_LAZY) or s.name in ('__nl_symbol_ptr', '__la_symbol_ptr', '__pointers'):
            slot = addr - (addr - s.addr) % 4
            name = self.indirect.get(slot)
            if name is None:
                v = self.u32(slot)
                if v is not None and v:
                    n, off = self.symbol_at(v)
                    if n:
                        return ('ptr', canonical('macho', n) + (f'+{off}' if off else ''))
                return ('ptr', None)
            return ('ptr', canonical('macho', name))
        if s.code:
            n, off = self.symbol_at(addr, s)
            return ('code', generic(canonical('macho', n)) if n else None, off)
        n, off = self.symbol_at(addr, s)
        if n and n.startswith('__ZTS') and not off:
            # typeinfo names are C strings; where they live (__cstring or a _ZTS symbol) is placement
            b = self.read(addr, 512)
            return ('str', b[:b.find(b'\0')] if b'\0' in b else b)
        constant = s.name == '__const' or (n or '').startswith(('__ZTV', '__ZTI', '__ZTT', '__ZTS'))
        if constant and (n is None or off or generated(n)):
            # constant data: symbol extents are unknown (anonymous data follows named
            # symbols), so compare what is there rather than name+offset
            if addr in self.slots:
                return ('cdata', self.pointee(addr))
            return ('lit', self.read(addr, max(size, 4)))
        if n is None:
            return ('?', s.name)
        return ('sym', canonical('macho', n), off)

    # -- data symbols ------------------------------------------------------
    def data_symbols(self):
        """{section key: [(addr, size, raw names)]} for DATA_SECTIONS."""
        out = {}
        for s in self.sections:
            if (s.seg, s.name) not in DATA_SECTIONS or s.data is None:
                continue
            starts = self.sec_syms.get(s.index, [])
            lst = []
            for i, a in enumerate(starts):
                end = starts[i + 1] if i + 1 < len(starts) else s.end
                lst.append((a, end - a, self.names[a]))
            out[f'{s.seg},{s.name}'] = lst
        return out


_ZL = re.compile(r'^_ZL\d+')


_GENERATED = re.compile(r'^_?(\.memset_pattern|__cxx_global_array_dtor|___?cxx_global_var_init|GCC_except_table|'
                        r'_?_GLOBAL__I_|L_|l_)')


def generated(name):
    return bool(name and _GENERATED.match(name))


def generic(name):
    """Compiler-generated names without their per-TU counters."""
    return re.sub(r'\d+$', 'N', name) if generated(name) else name


def unlink(name):
    """Symbol name with internal linkage mangled as external: _ZL5s_foo -> s_foo
    (namespace-scope variables are unmangled), _ZL3fooi -> _Z3fooi (functions)."""
    if name and name.startswith('_ZZL'):
        name = '_ZZ' + name[4:]                       # static local of a static function
        return name
    m = _ZL.match(name or '')
    if not m:
        return name
    n = int(m.group(0)[3:])
    ident, rest = name[m.end():m.end() + n], name[m.end() + n:]
    return ident if not rest else f'_Z{n}{ident}{rest}'


def named(ident):
    """The symbol name an identity refers to (through one pointer level), or None."""
    while ident and ident[0] == 'cdata':
        ident = ident[1]
    if ident and ident[0] in ('sym', 'code', 'ptr') and ident[1]:
        return ident[1].split('+')[0]
    return None


def renamed(rimg, oimg, a, b):
    """Different names where neither exists on the other side: a rename, not another object."""
    na, nb = named(a), named(b)
    return bool(na and nb and na != nb and not oimg.has_name(na) and not rimg.has_name(nb))


def same_modulo_linkage(a, b):
    return (a and b and a[0] == b[0] and a[0] in ('sym', 'ptr', 'code') and a[1:] != b[1:]
            and unlink(a[1]) == unlink(b[1]) and a[2:] == b[2:])


def _pick(names):
    if not names:
        return None
    return sorted(names, key=lambda n: (n.startswith('l'), len(n), n))[0]


def _binds(data, off, size, segs):
    """(addr, raw name) for dyld bind opcodes (regular or weak)."""
    out = []
    if not size:
        return out
    pos, end = off, off + size
    seg = addr = 0
    name = None
    while pos < end:
        b = data[pos]
        pos += 1
        op, imm = b & 0xf0, b & 0x0f
        if op == 0x00:
            continue                                  # DONE separates lazy entries
        elif op in (0x10, 0x30):
            pass                                      # dylib ordinal (imm), special
        elif op == 0x20:
            _, pos = macho.uleb(data, pos)
        elif op == 0x40:
            e = data.index(b'\0', pos)
            name = data[pos:e].decode('latin-1')
            pos = e + 1
        elif op == 0x50:
            pass
        elif op == 0x60:
            _, pos = _sleb(data, pos)
        elif op == 0x70:
            seg = imm
            v, pos = macho.uleb(data, pos)
            addr = segs[seg][1] + v if seg < len(segs) else 0
        elif op == 0x80:
            v, pos = macho.uleb(data, pos)
            addr = (addr + v) & 0xffffffff
        elif op == 0x90:
            out.append((addr, name))
            addr += 4
        elif op == 0xa0:
            out.append((addr, name))
            v, pos = macho.uleb(data, pos)
            addr = (addr + 4 + v) & 0xffffffff
        elif op == 0xb0:
            out.append((addr, name))
            addr += 4 + imm * 4
        elif op == 0xc0:
            n, pos = macho.uleb(data, pos)
            skip, pos = macho.uleb(data, pos)
            for _ in range(n):
                out.append((addr, name))
                addr += 4 + skip
        else:
            break
    return out


def _sleb(data, pos):
    val = shift = 0
    while True:
        b = data[pos]
        pos += 1
        val |= (b & 0x7f) << shift
        shift += 7
        if b < 0x80:
            if b & 0x40:
                val -= 1 << shift
            return val, pos


# -- references from code ----------------------------------------------------

_REG32 = {'eax', 'ebx', 'ecx', 'edx', 'esi', 'edi', 'ebp', 'esp'}
_SUBREG = {'al': 'eax', 'ah': 'eax', 'ax': 'eax', 'bl': 'ebx', 'bh': 'ebx', 'bx': 'ebx',
           'cl': 'ecx', 'ch': 'ecx', 'cx': 'ecx', 'dl': 'edx', 'dh': 'edx', 'dx': 'edx',
           'si': 'esi', 'di': 'edi', 'bp': 'ebp', 'sp': 'esp'}
_NOWRITE = re.compile(r'^(cmp|test|push|j\w+|call|ret|u?comis[sd]|bt|nop|cmps\w*|scas\w*|fst\w*|fist\w*|fcom\w*|'
                      r'movs[bwd]|stos\w*|lods\w*|leave|int3|ud2|out|prefetch\w*)$')
_MEM = re.compile(r'(?:(byte|word|dword|qword|xmmword|tbyte|ymmword) ptr )?(?:[a-z]s:)?\[([^\]]+)\]')
_SIZES = {'byte': 1, 'word': 2, 'dword': 4, 'qword': 8, 'tbyte': 10, 'xmmword': 16, 'ymmword': 32}


def _parse_mem(expr):
    """'ebx + ecx*4 + 0x10' -> (base, index, scale, disp)."""
    base = index = None
    scale, disp = 1, 0
    for sign, term in re.findall(r'([+-]?)\s*([^+\-\s]+)', expr.replace(' - ', ' + -').replace('+ -', '-')):
        neg = sign == '-'
        if '*' in term:
            index, sc = term.split('*')
            scale = int(sc)
        elif term in _REG32:
            if base is None:
                base = term
            else:
                index = term
        else:
            try:
                v = int(term, 0)
            except ValueError:
                return None
            disp += -v if neg else v
    return base, index, scale, disp


def code_refs(img, func):
    """{field offset in func: (target VA, operand size)} for data references.

    Objects: from relocations. Linked images: PIC base tracking over a linear
    decode (call next/pop reg, register copies, stack spills; writes and
    calls kill tracked registers)."""
    out = {}
    if not img.linked:
        for va, (tgt, name) in img.code_fields.items():
            if func.addr <= va < func.addr + func.size and tgt is not None:
                out[va - func.addr] = (tgt, 0)
        return out
    if not x86.have_capstone():
        return out
    code, base = func.data, func.addr
    pic, slots = {}, {}
    pending = None
    for addr, size, mnem, ops in x86._lite.disasm_lite(code, base):
        off = addr - base
        nxt = addr + size
        if mnem == 'call':
            try:
                tgt = int(ops, 0)
            except ValueError:
                tgt = None
            for r in ('eax', 'ecx', 'edx'):
                pic.pop(r, None)
            pending = nxt if tgt == nxt else None
            continue
        if mnem == 'pop' and pending is not None and ops in _REG32:
            pic[ops] = pending
            pending = None
            continue
        pending = None
        parts = ops.split(', ', 1) if ops else []
        dst = parts[0] if parts else ''
        src = parts[1] if len(parts) > 1 else ''
        m = _MEM.search(ops) if '[' in ops else None
        if m:
            pm = _parse_mem(m.group(2))
            if pm:
                b, ix, sc, disp = pm
                pb = pic.get(b) if b else None
                if pb is None and ix and sc == 1:
                    pb = pic.get(ix)
                if pb is not None and abs(disp) > 0x7f:
                    raw = code[off:off + size]
                    fo = raw.find(struct.pack('<i', disp), 1)
                    if fo > 0:
                        out[off + fo] = ((pb + disp) & 0xffffffff, _SIZES.get(m.group(1) or '', 0))
                elif img.absolute and pb is None and img.in_image(disp & 0xffffffff) and disp & 0xffffffff > 0x1000:
                    raw = code[off:off + size]
                    fo = raw.find(struct.pack('<I', disp & 0xffffffff), 1)
                    if fo > 0:
                        out[off + fo] = (disp & 0xffffffff, _SIZES.get(m.group(1) or '', 0))
        elif img.absolute and (src.startswith('0x') or (mnem == 'push' and dst.startswith('0x'))):
            try:
                v = int(src if src.startswith('0x') else dst, 16)
            except ValueError:
                v = 0
            if v > 0x1000 and img.in_image(v) and not mnem.startswith(('j', 'call')):
                raw = code[off:off + size]
                fo = raw.find(struct.pack('<I', v), 1)
                if fo > 0:
                    out[off + fo] = (v, 0)
        # tracking
        if mnem == 'mov' and m and dst.startswith('dword ptr [') and src in pic:
            pm = _parse_mem(m.group(2))
            if pm:
                slots[(pm[0], pm[3])] = pic[src]
            continue
        dreg = _SUBREG.get(dst, dst)
        if dreg in _REG32:
            if mnem == 'mov' and src in pic:
                pic[dreg] = pic[src]
                continue
            if mnem == 'mov' and m and src.startswith('dword ptr ['):
                pm = _parse_mem(m.group(2))
                if pm and (pm[0], pm[3]) in slots:
                    pic[dreg] = slots[(pm[0], pm[3])]
                    continue
            if not _NOWRITE.match(mnem):
                pic.pop(dreg, None)
                if mnem == 'xchg' and src in _REG32:
                    pic.pop(src, None)
        if mnem in ('cdq', 'mul', 'div', 'idiv') or (mnem == 'imul' and ',' not in ops):
            pic.pop('eax', None)
            pic.pop('edx', None)
    return out


def check_function(rimg, oimg, r, o):
    """Data references of a paired function that resolve differently.

    Returns [(offset, retail identity, ours identity)]. Only references found
    on both sides at the same field offset are compared, and only when at
    least one side resolves to something definite."""
    rr, orf = code_refs(rimg, r), code_refs(oimg, o)
    out = []
    for k in sorted(rr.keys() & orf.keys()):
        (rt, rsz), (ot, osz) = rr[k], orf[k]
        ri, oi = rimg.ident(rt, rsz), oimg.ident(ot, osz or rsz)
        if ri == oi or not comparable(ri, oi):
            continue
        out.append((k, ri, oi))
    return out


def comparable(a, b):
    if a[0] == '?' and b[0] == '?':
        return False
    if a[0] == 'code' or b[0] == 'code':
        # function pointers only: a mid-function target means the PIC tracking lost the register
        return a[0] == b[0] and bool(a[1] and b[1]) and not a[2] and not b[2]
    if a[0] == 'lit' and b[0] == 'lit' and (not a[1] or not b[1]):
        return False
    return True


def kind_of(a, b):
    k = {a[0], b[0]}
    for x in (a, b):
        if x[0] == 'cdata' and x[1]:
            k.add(x[1][0])
    if k & {'str', 'cfstr'}:
        return 'string'
    if k & {'lit', 'cdata'}:
        return 'constant'
    if 'code' in k:
        return 'function'
    return 'symbol'


def show(ident, limit=160):
    if ident is None:
        return '-'
    k = ident[0]
    if k in ('str', 'cfstr'):
        return ('@' if k == 'cfstr' else '') + (c_quote(ident[1], limit) if ident[1] is not None else '<?>')
    if k == 'lit':
        return 'bytes ' + ident[1].hex()
    if k == 'ptr':
        return f'&{ident[1]}'
    if k == 'cdata':
        return f'-> {show(ident[1], limit)}'
    if k in ('sym', 'code'):
        return f'{ident[1]}' + (f'+{ident[2]}' if ident[2] else '')
    return f'<{ident[1]}>'


def c_quote(b, limit=None):
    s = c_escape(b)
    if limit and len(s) > limit:
        s = s[:limit] + '...'
    return '"' + s + '"'


def c_escape(b):
    out = []
    for c in b:
        if c == 0x0a:
            out.append('\\n')
        elif c == 0x09:
            out.append('\\t')
        elif c == 0x0d:
            out.append('\\r')
        elif c == 0x22:
            out.append('\\"')
        elif c == 0x5c:
            out.append('\\\\')
        elif 0x20 <= c < 0x7f:
            out.append(chr(c))
        else:
            out.append(f'\\x{c:02x}')
    return ''.join(out)


# -- module audit -------------------------------------------------------------

def string_refs(img, funcs):
    """{string start VA: set(object index or name)} from code and data pointers."""
    refs = collections.defaultdict(set)
    funcs_by = {}
    for f in funcs:
        oi = img.addr_obj.get(f.addr)
        for k, (t, sz) in code_refs(img, f).items():
            st = img.string_start(t)
            if st is not None:
                refs[st].add(oi)
                funcs_by.setdefault(st, f)
    for slot, (kind, name) in img.slots.items():
        if kind != 'rebase':
            continue
        v = img.u32(slot)
        st = img.string_start(v) if v is not None else None
        if st is None:
            sec = img.section_at(v) if v is not None else None
            if sec is not None and sec.name == '__cfstring':
                p = img.u32(v - (v - sec.addr) % 16 + 8)
                st = img.string_start(p) if p is not None else None
            if st is None:
                continue
        n, off = img.symbol_at(slot)
        refs[st].add(img.addr_obj.get(slot - off) if n else None)
        if n:
            oi = img.name_obj.get(n)
            if oi is not None:
                refs[st].add(oi)
    for s in refs.values():
        if len(s) > 1:
            s.discard(None)
    return refs, funcs_by


def _obj_names(img, idxs):
    return sorted(img.objects[i]['path'] for i in idxs if i is not None)


def compare_strings(rimg, oimg, rfuncs, ofuncs):
    """Set and order comparison of cstring literals, attributed to objects."""
    def distinct(lst):
        seen = set()                                  # alignment padding shows up as repeated ""
        return [(a, s) for a, s in lst if not (s in seen or seen.add(s))]
    rl, ol = distinct(rimg.cstrings()), distinct(oimg.cstrings())
    rset, oset = {s for _, s in rl}, {s for _, s in ol}
    rrefs, _ = string_refs(rimg, rfuncs)
    orefs, _ = string_refs(oimg, ofuncs)
    raddr = {s: a for a, s in rl}
    oaddr = {s: a for a, s in ol}

    def attr(img, refs, lst, i):
        """Objects for literal i of lst: referencing objects, else nearest referenced neighbour."""
        a, s = lst[i]
        objs = refs.get(a)
        if objs and objs != {None}:
            return _obj_names(img, objs), 'ref'
        for d in range(1, 40):
            for j in (i - d, i + d):
                if 0 <= j < len(lst):
                    o2 = refs.get(lst[j][0])
                    if o2 and o2 != {None}:
                        return _obj_names(img, o2), 'near'
        return [], 'none'

    missing, extra = [], []
    for i, (a, s) in enumerate(rl):
        if s not in oset:
            objs, how = attr(rimg, rrefs, rl, i)
            missing.append(dict(text=s, retail_addr=a, objects=objs, attributed=how))
    for i, (a, s) in enumerate(ol):
        if s not in rset:
            objs, how = attr(oimg, orefs, ol, i)
            extra.append(dict(text=s, ours_addr=a, objects=objs, attributed=how))
    seq_r = [s for _, s in rl if s in oset]
    seq_o = [s for _, s in ol if s in rset]
    sm = difflib.SequenceMatcher(None, seq_r, seq_o, autojunk=False)
    in_order = set()
    for blk in sm.get_matching_blocks():
        in_order.update(seq_r[blk.a:blk.a + blk.size])
    moved = []
    for s in seq_r:
        if s not in in_order:
            objs = rrefs.get(raddr[s])
            moved.append(dict(text=s, objects=_obj_names(rimg, objs or ())))
    return dict(retail=len(rl), ours=len(ol), common=len(seq_r), in_order=len(in_order),
                missing=missing, extra=extra, moved=moved)


def compare_data(rimg, oimg):
    """Content comparison of named symbols in DATA_SECTIONS."""
    rsyms, osyms = rimg.data_symbols(), oimg.data_symbols()
    out = []
    for sec in sorted(set(rsyms) | set(osyms)):
        rl, ol = rsyms.get(sec, []), osyms.get(sec, [])
        rcount = collections.Counter(unlink(canonical('macho', _pick(n))) for _, _, n in rl)
        ocount = collections.Counter(unlink(canonical('macho', _pick(n))) for _, _, n in ol)

        def keyed(img, lst, count):
            d = {}
            for a, size, names in lst:
                nm = unlink(canonical('macho', _pick(names)))
                k = nm
                if count[nm] > 1 or (rcount[nm] > 1 or ocount[nm] > 1):
                    ob = img.object_of(a, _pick(names))
                    k = f'{nm}@{ob["stem"] if ob else "?"}'
                d.setdefault(k, (a, size, names))
            return d

        rd, od = keyed(rimg, rl, rcount), keyed(oimg, ol, ocount)
        for k in sorted(rd.keys() & od.keys()):
            (ra, rsz, rn), (oa, osz, on) = rd[k], od[k]
            diffs = _diff_symbol(rimg, oimg, ra, rsz, oa, osz)
            if not diffs and rsz == osz:
                continue
            ob = rimg.object_of(ra, _pick(rn))
            out.append(dict(name=k.split('@')[0], section=sec, object=ob['path'] if ob else None,
                            retail_addr=ra, retail_size=rsz, ours_size=osz,
                            kind=_data_kind(k, diffs, rsz, osz), ndiffs=len(diffs),
                            diffs=sorted(diffs, key=lambda x: x['kind'] != 'string')[:12]))
    return out


def _diff_symbol(rimg, oimg, ra, rsz, oa, osz):
    diffs = []
    n = min(rsz, osz)
    k = 0
    raw_start = None
    while k < n:
        rs, os_ = rimg.slots.get(ra + k), oimg.slots.get(oa + k)
        if rs or os_:
            if raw_start is not None:
                diffs.append(_raw_diff(rimg, oimg, ra, oa, raw_start, k))
                raw_start = None
            ri = rimg.pointee(ra + k) if rs else ('lit', rimg.read(ra + k, 4))
            oi = oimg.pointee(oa + k) if os_ else ('lit', oimg.read(oa + k, 4))
            if ri != oi and not same_modulo_linkage(ri, oi):
                diffs.append(dict(offset=k, retail=show(ri), ours=show(oi), kind=kind_of(ri or ('?',), oi or ('?',)),
                                  _r=ri, _o=oi))
            k += 4
            continue
        same = rimg.read(ra + k, 1) == oimg.read(oa + k, 1)
        if not same and raw_start is None:
            raw_start = k
        elif same and raw_start is not None:
            diffs.append(_raw_diff(rimg, oimg, ra, oa, raw_start, k))
            raw_start = None
        k += 1
    if raw_start is not None:
        diffs.append(_raw_diff(rimg, oimg, ra, oa, raw_start, n))
    return diffs


def _raw_diff(rimg, oimg, ra, oa, a, b):
    # widen to the surrounding printable run so text differences read as strings
    def text_run(img, base, a, b):
        lo, hi = a, b
        while lo > 0 and 0x20 <= (img.read(base + lo - 1, 1) or b'\0')[0] < 0x7f:
            lo -= 1
        while 0x20 <= (img.read(base + hi, 1) or b'\0')[0] < 0x7f and hi - lo < 200:
            hi += 1
        return lo, hi
    rb, ob = rimg.read(ra + a, b - a), oimg.read(oa + a, b - a)
    lo, hi = text_run(rimg, ra, a, b)
    lo2, hi2 = text_run(oimg, oa, a, b)
    lo, hi = min(lo, lo2), max(hi, hi2)
    rt, ot = rimg.read(ra + lo, hi - lo), oimg.read(oa + lo, hi - lo)
    if hi - lo >= 4 and _printable(rt) and _printable(ot):
        return dict(offset=lo, retail=c_quote(rt), ours=c_quote(ot), kind='string')
    return dict(offset=a, retail=rb[:32].hex(), ours=ob[:32].hex(), kind='bytes')


def _printable(b):
    return b and sum(0x20 <= c < 0x7f or c in (9, 10, 13) for c in b) >= 0.9 * len(b)


def _data_kind(name, diffs, rsz, osz):
    kinds = {d['kind'] for d in diffs}
    if 'string' in kinds:
        return 'string'
    if name.startswith('_ZTV'):
        return 'vtable'
    if rsz != osz and not diffs:
        return 'size'
    if 'function' in kinds or 'symbol' in kinds:
        return 'pointer'
    return 'bytes'


def pair_similar(missing, extra, threshold=0.6):
    """Pair retail-only with ours-only literals that look like edits of each other,
    preferring pairs attributed to the same object stem."""
    pairs = []
    used = set()
    by_obj = collections.defaultdict(list)
    for j, e in enumerate(extra):
        for ob in e['objects'] or ['']:
            by_obj[object_stem(ob)].append(j)
    for i, m in enumerate(missing):
        cands = set()
        for ob in m['objects'] or ['']:
            cands.update(by_obj.get(object_stem(ob), ()))
        best, bj = threshold, None
        mt = m['text']
        for j in cands:
            if j in used:
                continue
            et = extra[j]['text']
            if abs(len(et) - len(mt)) > max(len(et), len(mt)) * 0.5:
                continue
            sm = difflib.SequenceMatcher(None, mt, et, autojunk=False)
            if sm.real_quick_ratio() < best or sm.quick_ratio() < best:
                continue
            r = sm.ratio()
            if r > best:
                best, bj = r, j
        if bj is not None:
            used.add(bj)
            pairs.append((i, bj, round(best, 3)))
    return pairs
