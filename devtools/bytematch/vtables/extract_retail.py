#!/usr/bin/env python3
"""Decode vtables, typeinfo and interface registrations from the retail macOS
i386 dylibs (HL2 Anniversary, build 19307283).

The retail Mac binaries keep full local symbols, so every vtable slot resolves
to a demangled method name. For each module this writes
<cache>/<module>.json with:

  classes:    { "Class": { "bases": [[base, offset], ...],
                           "tables": [ { "offset_to_top": 0,
                                         "slots": ["Class::Method(args)", ...] } ] } }
  interfaces: [ { "version": "VEngineClient013", "create": "...",
                  "class": "CEngineClient", "interface": "IVEngineClient" } ]

Slots hold the demangled symbol, "__cxa_pure_virtual", an imported symbol
("libtier0/CThread::Init()"), or null for a zero pointer.

Usage: extract_retail.py [--depots DIR] [--cache DIR] [module-glob ...]
"""
import argparse
import bisect
import fnmatch
import json
import os
import re
import struct
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
DEFAULT_DEPOTS = '/Volumes/Expansion/mac-ports/hl2-anniversary-depot/depots'
DEFAULT_CACHE = os.path.join(HERE, '.cache', 'retail')

# module name -> path relative to the depot root
MODULE_DIRS = [
    ('223/19307283/bin', 'bin'),
    ('223/19307283/hl2/bin', 'hl2'),
    ('223/19307283/hl2_complete/bin', 'hl2_complete'),
    ('223/19307283/hl2mp/bin', 'hl2mp'),
    ('380/19307283/episodic/bin', 'episodic'),
    ('340/19307283/lostcoast/bin', 'lostcoast'),
]

# Third-party redistributables (see byte-match-survey/scope.md).
SKIP = re.compile(r'^(libcef|libSDL2|libsteam_api|libMiles|libav|libsw|libtcmalloc|libpng|libjpeg|libfreetype|libfontconfig)', re.I)


def run(cmd, inp=None):
    return subprocess.run(cmd, input=inp, capture_output=True, text=True, errors='replace').stdout


def demangle(names):
    out = run(['c++filt'], '\n'.join(names) + '\n').splitlines()
    if len(out) != len(names):
        raise RuntimeError('c++filt output mismatch')
    return out


class MachO:
    def __init__(self, path):
        self.path = path
        self.data = open(path, 'rb').read()
        self.sections = self._sections()
        self._symbols()
        self._fixups()

    def _sections(self):
        secs, cur = [], None
        for line in run(['otool', '-l', self.path]).splitlines():
            p = line.split()
            if not p:
                continue
            if p[0] == 'sectname':
                cur = {'sect': p[1]}
            elif cur is not None and p[0] == 'segname':
                cur['seg'] = p[1]
            elif cur is not None and p[0] == 'addr':
                cur['addr'] = int(p[1], 16)
            elif cur is not None and p[0] == 'size':
                cur['size'] = int(p[1], 16)
            elif cur is not None and p[0] == 'offset':
                cur['off'] = int(p[1])
                secs.append(cur)
                cur = None
        return secs

    def _symbols(self):
        raw = []
        for line in run(['nm', '-n', self.path]).splitlines():
            p = line.split(' ', 2)
            if len(p) == 3 and p[0]:
                raw.append((int(p[0], 16), p[1], p[2]))
        dem = demangle([r[2] for r in raw])
        self.syms = [(a, t, m, d) for (a, t, m), d in zip(raw, dem)]
        self.by_addr = {}
        for a, t, m, d in self.syms:
            # prefer the first (usually global) name at an address
            self.by_addr.setdefault(a, (t, m, d))
        self.addrs = [s[0] for s in self.syms]

    def _fixups(self):
        """Decode the non-lazy bind opcode stream from LC_DYLD_INFO.

        `dyld_info -fixups` misattributes symbols for these i386 classic bind
        streams, so the opcodes are interpreted here directly."""
        d = self.data
        magic, _, _, _, ncmds, _, _ = struct.unpack_from('<7I', d, 0)
        if magic != 0xfeedface:
            raise RuntimeError(f'{self.path}: not a 32-bit Mach-O')
        off = 28
        segs, dylibs, bind = [], [], None
        for _ in range(ncmds):
            cmd, size = struct.unpack_from('<2I', d, off)
            if cmd == 0x1:  # LC_SEGMENT
                segs.append(struct.unpack_from('<I', d, off + 24)[0])
            elif cmd in (0xc, 0x80000018, 0x8000001f, 0x20, 0x80000023):  # LC_LOAD_*DYLIB
                name_off = struct.unpack_from('<I', d, off + 8)[0]
                name = d[off + name_off:d.index(b'\0', off + name_off)].decode()
                base = os.path.basename(name)
                dylibs.append(re.sub(r'(\.[\d.]+)?\.dylib$', '', base))
            elif cmd in (0x22, 0x80000022):  # LC_DYLD_INFO(_ONLY)
                bind = struct.unpack_from('<2I', d, off + 16)
            off += size
        self.binds = {}
        if not bind:
            return

        def uleb(p):
            v = s = 0
            while True:
                b = d[p]; p += 1
                v |= (b & 0x7f) << s; s += 7
                if b < 0x80:
                    return v, p

        def sleb(p):
            v = s = 0
            while True:
                b = d[p]; p += 1
                v |= (b & 0x7f) << s; s += 7
                if b < 0x80:
                    if b & 0x40:
                        v -= 1 << s
                    return v, p

        p, end = bind[0], bind[0] + bind[1]
        lib = sym = None
        addend = 0
        addr = 0
        while p < end:
            b = d[p]; p += 1
            op, imm = b & 0xf0, b & 0x0f
            if op == 0x00:
                break
            elif op == 0x10:
                lib = imm
            elif op == 0x20:
                lib, p = uleb(p)
            elif op == 0x30:
                lib = 0
            elif op == 0x40:
                e = d.index(b'\0', p)
                sym = d[p:e].decode('latin-1'); p = e + 1
            elif op == 0x50:
                pass
            elif op == 0x60:
                addend, p = sleb(p)
            elif op == 0x70:
                o, p = uleb(p)
                addr = segs[imm] + o
            elif op == 0x80:
                o, p = uleb(p)
                addr = (addr + o) & 0xffffffff
            elif op in (0x90, 0xa0, 0xb0, 0xc0):
                libname = dylibs[lib - 1] if lib and 0 < lib <= len(dylibs) else 'self'
                if op == 0xc0:
                    count, p = uleb(p)
                    skip, p = uleb(p)
                    for _ in range(count):
                        self.binds[addr] = (f'{libname}/{sym}', addend)
                        addr += 4 + skip
                    continue
                self.binds[addr] = (f'{libname}/{sym}', addend)
                addr += 4
                if op == 0xa0:
                    o, p = uleb(p)
                    addr = (addr + o) & 0xffffffff
                elif op == 0xb0:
                    addr += imm * 4
            else:
                raise RuntimeError(f'{self.path}: unknown bind opcode {b:#x}')

    def section_of(self, addr):
        for s in self.sections:
            if s['addr'] <= addr < s['addr'] + s['size']:
                return s
        return None

    def word(self, addr):
        s = self.section_of(addr)
        if s is None or s['sect'] in ('__bss', '__common') or not s.get('off'):
            return None
        return struct.unpack_from('<I', self.data, s['off'] + addr - s['addr'])[0]

    def cstr(self, addr):
        s = self.section_of(addr)
        if s is None or s['sect'] != '__cstring':
            return None
        o = s['off'] + addr - s['addr']
        return self.data[o:self.data.index(b'\0', o)].decode('latin-1')

    def next_sym_addr(self, addr):
        i = bisect.bisect_right(self.addrs, addr)
        while i < len(self.addrs) and self.addrs[i] == addr:
            i += 1
        return self.addrs[i] if i < len(self.addrs) else None

    # A pointer-sized slot: ('bind', name) | ('sym', mangled, demangled, type) | ('zero',) | ('raw', value)
    def pointer(self, addr):
        b = self.binds.get(addr)
        if b:
            return ('bind', b[0], b[1])
        v = self.word(addr)
        if v is None:
            return None
        if v == 0:
            return ('zero',)
        s = self.by_addr.get(v)
        if s:
            return ('sym', s[1], s[2], s[0])
        return ('raw', v)


def bind_name(lib_sym):
    lib, _, sym = lib_sym.partition('/')
    return lib, demangle([sym])[0]


def typeinfo_name(ptr):
    """Name of the class for a pointer resolving to a typeinfo object, else None."""
    if ptr is None:
        return None
    if ptr[0] == 'sym' and ptr[2].startswith('typeinfo for '):
        return ptr[2][len('typeinfo for '):]
    if ptr[0] == 'bind':
        _, d = bind_name(ptr[1])
        if d.startswith('typeinfo for '):
            return d[len('typeinfo for '):]
    return None


def slot_name(ptr):
    if ptr is None:
        return False
    if ptr[0] == 'zero':
        return None
    if ptr[0] == 'sym':
        return ptr[2] if ptr[3] in 'tT' else False
    if ptr[0] == 'bind':
        lib, d = bind_name(ptr[1])
        if d in ('___cxa_pure_virtual', '__cxa_pure_virtual'):
            return '__cxa_pure_virtual'
        if d.startswith(('typeinfo for ', 'vtable for ')):
            return False
        return f'{lib}/{d}'
    return False


def decode_vtable(mo, addr):
    end = mo.next_sym_addr(addr)
    sec = mo.section_of(addr)
    sec_end = sec['addr'] + sec['size']
    end = min(end or sec_end, sec_end)
    n = (end - addr) // 4
    ptrs = [mo.pointer(addr + 4 * i) for i in range(n)]
    words = [mo.word(addr + 4 * i) for i in range(n)]
    tables = []
    i = 0
    while i + 1 < n:
        # classes with virtual bases put vbase/vcall offsets before offset_to_top
        skip = 0
        while skip < 8 and i + skip + 1 < n and typeinfo_name(ptrs[i + skip + 1]) is None and \
                words[i + skip] is not None and slot_name(ptrs[i + skip]) in (False, None):
            skip += 1
        vbase = words[i:i + skip]
        i += skip
        if i + 1 >= n:
            break
        ott = words[i]
        ti = typeinfo_name(ptrs[i + 1])
        if ott is None or ti is None:
            break
        ott = struct.unpack('<i', struct.pack('<I', ott))[0]
        slots = []
        j = i + 2
        while j < n:
            if j + 1 < n and typeinfo_name(ptrs[j + 1]) is not None and words[j] is not None:
                v = struct.unpack('<i', struct.pack('<I', words[j]))[0]
                if -0x100000 < v <= 0 and v % 4 == 0:
                    break
            s = slot_name(ptrs[j])
            if s is False:
                break
            slots.append(s)
            j += 1
        while slots and slots[-1] is None:
            slots.pop()
        t = {'offset_to_top': ott, 'typeinfo': ti, 'slots': slots}
        if vbase:
            t['vbase_offsets'] = [struct.unpack('<i', struct.pack('<I', w))[0] for w in vbase]
        tables.append(t)
        i = j
    return tables


def decode_typeinfo(mo, addr):
    kind_ptr = mo.binds.get(addr)
    if not kind_ptr:
        return None
    kind = kind_ptr[0]
    if '__si_class_type_info' in kind:
        return [[typeinfo_name(mo.pointer(addr + 8)), 0]]
    if '__vmi_class_type_info' in kind:
        count = mo.word(addr + 12) or 0
        bases = []
        for k in range(min(count, 64)):
            b = typeinfo_name(mo.pointer(addr + 16 + 8 * k))
            flags = mo.word(addr + 20 + 8 * k) or 0
            off = struct.unpack('<i', struct.pack('<I', flags))[0] >> 8
            entry = [b, off]
            if flags & 1:
                entry.append('virtual')
            bases.append(entry)
        return bases
    if '__class_type_info' in kind:
        return []
    return None


PIC_CALL = re.compile(r'\tcalll\t0x([0-9a-f]+)$')
POP = re.compile(r'\tpopl\t%(e\w\w)$')
LEA = re.compile(r'\tleal\t(-?0x[0-9a-f]+)\(%(e\w\w)\), %(e\w\w)$')
STORE = re.compile(r'\tmovl\t%(e\w\w), (0x[0-9a-f]+)?\(%esp\)$')
REGCALL = re.compile(r'\tcalll\t__ZN12InterfaceRegC[12]EPFPvvEPKc')


def interface_regs(mo):
    """Recover (create function, version string) from InterfaceReg constructor calls."""
    out = run(['otool', '-tV', mo.path])
    regs = []
    pic, regval, args = {}, {}, {}
    lines = out.splitlines()
    for idx, line in enumerate(lines):
        if line and not line[0].isdigit():
            pic, regval, args = {}, {}, {}
            continue
        m = PIC_CALL.search(line)
        if m and idx + 1 < len(lines):
            p = POP.search(lines[idx + 1])
            if p:
                pic[p.group(1)] = int(m.group(1), 16)
                continue
        m = LEA.search(line)
        if m and m.group(2) in pic:
            regval[m.group(3)] = pic[m.group(2)] + int(m.group(1), 16)
            continue
        m = STORE.search(line)
        if m:
            slot = int(m.group(2), 16) if m.group(2) else 0
            args[slot] = regval.get(m.group(1))
            continue
        if REGCALL.search(line):
            # i386 thiscall: (%esp) is the InterfaceReg object, then fn, then name
            obj, fn, ver = args.get(0), args.get(4), args.get(8)
            sym = mo.by_addr.get(fn) if fn is not None else None
            osym = mo.by_addr.get(obj) if obj is not None else None
            regs.append({'create': sym[2] if sym else None,
                         'reg': osym[2] if osym else None,
                         'version': mo.cstr(ver) if ver is not None else None})
            args = {}
    return regs


CREATE = re.compile(r'__(?:g_)?Create(\w+?)_(?:interface|reg)\b')


def split_create(name, classes):
    """'__CreateCEngineClientIVEngineClient_interface()' -> ('CEngineClient', 'IVEngineClient')."""
    m = CREATE.search(name or '')
    if not m:
        return None, None
    body = m.group(1)
    if not classes and body[:1] == 'I':
        return None, body
    for k in range(len(body) - 1, 0, -1):
        if body[:k] in classes:
            return body[:k], body[k:]
    # interface names conventionally start with I followed by an uppercase letter
    for k in range(1, len(body) - 1):
        if body[k] == 'I' and body[k + 1].isupper():
            return body[:k], body[k:]
    return body, None


def extract(path):
    mo = MachO(path)
    classes = {}
    for a, t, m, d in mo.syms:
        if d.startswith('vtable for ') and m.startswith('__ZTV'):
            cls = d[len('vtable for '):]
            if cls in classes:
                continue
            tables = decode_vtable(mo, a)
            if tables:
                classes[cls] = {'tables': tables}
    for a, t, m, d in mo.syms:
        if d.startswith('typeinfo for ') and m.startswith('__ZTI'):
            cls = d[len('typeinfo for '):]
            bases = decode_typeinfo(mo, a)
            if bases is not None:
                classes.setdefault(cls, {'tables': []})['bases'] = bases
    regs = interface_regs(mo)
    for r in regs:
        if r['create'] and CREATE.search(r['create']):
            r['class'], r['interface'] = split_create(r['create'], classes)
        else:
            # EXPOSE_INTERFACE_FN: the reg object is named after the interface only
            _, r['interface'] = split_create(r['reg'], {})
            r['class'] = None
        if r['interface'] is None and (r['version'] or '').endswith('_IMaterialProxy003'):
            r['interface'] = 'IMaterialProxy'
        if r['interface'] is None and r['class'] == 'MaterialSystem_Config_tMaterialSystem_Config_t':
            r['class'] = r['interface'] = 'MaterialSystem_Config_t'
    return {'module': os.path.basename(path), 'classes': classes, 'interfaces': regs}


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--depots', default=DEFAULT_DEPOTS)
    ap.add_argument('--cache', default=DEFAULT_CACHE)
    ap.add_argument('modules', nargs='*', help='module globs such as bin/engine or hl2/*')
    a = ap.parse_args()
    os.makedirs(a.cache, exist_ok=True)
    for rel, prefix in MODULE_DIRS:
        d = os.path.join(a.depots, rel)
        if not os.path.isdir(d):
            continue
        for f in sorted(os.listdir(d)):
            if not f.endswith('.dylib') or SKIP.match(f):
                continue
            mod = f'{prefix}/{f[:-len(".dylib")]}'
            if a.modules and not any(fnmatch.fnmatch(mod, g) for g in a.modules):
                continue
            res = extract(os.path.join(d, f))
            res['module'] = mod
            outp = os.path.join(a.cache, mod.replace('/', '__') + '.json')
            with open(outp, 'w') as fh:
                json.dump(res, fh, indent=0, sort_keys=True)
            print(f'{mod:28} {len(res["classes"]):5} classes {len(res["interfaces"]):3} interfaces', file=sys.stderr)


if __name__ == '__main__':
    main()
