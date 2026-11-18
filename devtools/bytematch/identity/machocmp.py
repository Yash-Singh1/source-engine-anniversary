#!/usr/bin/env python3
"""Whole-file comparison of two 32-bit Mach-O images (thin i386).

Splits each file into named regions (header, each load command, each
section's file contents, padding gaps, and every LINKEDIT blob named by
LC_DYLD_INFO / LC_SYMTAB / LC_DYSYMTAB / LC_FUNCTION_STARTS / LC_DATA_IN_CODE /
LC_CODE_SIGNATURE), then reports which regions differ. With --symbols the
symbol tables are compared entry by entry (strings resolved), which is where
build-path and object-mtime differences show up.

Exit status: 0 identical, 1 different, 2 error.
"""
import argparse
import hashlib
import struct
import sys

MH_MAGIC = 0xFEEDFACE
FAT_MAGIC = 0xCAFEBABE

LC_NAMES = {
    0x1: 'LC_SEGMENT', 0x2: 'LC_SYMTAB', 0xB: 'LC_DYSYMTAB', 0xC: 'LC_LOAD_DYLIB',
    0xD: 'LC_ID_DYLIB', 0x1B: 'LC_UUID', 0x24: 'LC_VERSION_MIN_MACOSX',
    0x26: 'LC_FUNCTION_STARTS', 0x29: 'LC_DATA_IN_CODE', 0x1D: 'LC_CODE_SIGNATURE',
    0x22: 'LC_DYLD_INFO', 0x80000022: 'LC_DYLD_INFO_ONLY', 0x80000018: 'LC_LOAD_WEAK_DYLIB',
    0x8000001F: 'LC_REEXPORT_DYLIB', 0x80000028: 'LC_MAIN', 0x5: 'LC_UNIXTHREAD',
    0xE: 'LC_LOAD_DYLINKER', 0x2A: 'LC_SOURCE_VERSION', 0x1E: 'LC_SEGMENT_SPLIT_INFO',
}

STAB_NAMES = {
    0x20: 'GSYM', 0x22: 'FNAME', 0x24: 'FUN', 0x26: 'STSYM', 0x28: 'LCSYM', 0x2e: 'BNSYM',
    0x4e: 'ENSYM', 0x64: 'SO', 0x66: 'OSO', 0x84: 'SOL', 0x3c: 'OPT',
}


class Image:
    def __init__(self, path):
        self.path = path
        with open(path, 'rb') as f:
            self.data = f.read()
        d = self.data
        if struct.unpack_from('>I', d)[0] == FAT_MAGIC:
            raise ValueError('%s: fat file; thin it to i386 first' % path)
        magic, self.cputype, self.cpusub, self.filetype, self.ncmds, self.sizeofcmds, self.flags = \
            struct.unpack_from('<7I', d)
        if magic != MH_MAGIC:
            raise ValueError('%s: not a 32-bit little-endian Mach-O' % path)
        self.regions = [('mach_header', 0, 28)]
        self.cmds = []
        self.symtab = self.dysymtab = None
        off = 28
        for i in range(self.ncmds):
            cmd, size = struct.unpack_from('<2I', d, off)
            name = LC_NAMES.get(cmd, 'LC_%#x' % cmd)
            label = 'load command %d (%s' % (i, name)
            if cmd == 0x1:
                seg = d[off + 8:off + 24].rstrip(b'\0').decode()
                label += ' ' + seg
                nsects = struct.unpack_from('<I', d, off + 48)[0]
                for s in range(nsects):
                    so = off + 56 + 68 * s
                    sect = d[so:so + 16].rstrip(b'\0').decode()
                    seg2 = d[so + 16:so + 32].rstrip(b'\0').decode()
                    addr, ssize, foff, align, reloff, nreloc, flags = struct.unpack_from('<7I', d, so + 32)
                    if foff and (flags & 0xff) not in (0x1, 0xc, 0x12):  # zerofill kinds
                        self.regions.append(('section %s,%s' % (seg2, sect), foff, ssize))
            elif cmd == 0x2:
                self.symtab = struct.unpack_from('<4I', d, off + 8)
                symoff, nsyms, stroff, strsize = self.symtab
                self.regions.append(('symbol table', symoff, nsyms * 12))
                self.regions.append(('string table', stroff, strsize))
            elif cmd == 0xB:
                self.dysymtab = struct.unpack_from('<18I', d, off + 8)
                ind_off, nind = self.dysymtab[12], self.dysymtab[13]
                if nind:
                    self.regions.append(('indirect symbols', ind_off, nind * 4))
            elif cmd in (0x22, 0x80000022):
                vals = struct.unpack_from('<10I', d, off + 8)
                for j, n in enumerate(('rebase info', 'bind info', 'weak bind info', 'lazy bind info', 'export trie')):
                    if vals[2 * j + 1]:
                        self.regions.append((n, vals[2 * j], vals[2 * j + 1]))
            elif cmd in (0x26, 0x29, 0x1D, 0x1E):
                doff, dsize = struct.unpack_from('<2I', d, off + 8)
                if dsize:
                    self.regions.append((name[3:].lower().replace('_', ' '), doff, dsize))
            self.regions.append((label + ')', off, size))
            self.cmds.append((cmd, d[off:off + size]))
            off += size
        self.regions.sort(key=lambda r: (r[1], -r[2]))
        # fill gaps so every byte belongs to a region
        filled, pos = [], 0
        for r in self.regions:
            if r[1] > pos:
                filled.append(('padding after %s' % (filled[-1][0] if filled else 'start'), pos, r[1] - pos))
            filled.append(r)
            pos = max(pos, r[1] + r[2])
        if pos < len(d):
            filled.append(('trailing bytes', pos, len(d) - pos))
        self.regions = filled

    def uuid(self):
        for cmd, raw in self.cmds:
            if cmd == 0x1B:
                return raw[8:24]
        return None

    def symbols(self):
        if not self.symtab:
            return []
        symoff, nsyms, stroff, strsize = self.symtab
        d, out = self.data, []
        for i in range(nsyms):
            strx, ntype, sect, desc, value = struct.unpack_from('<IBBhI', d, symoff + 12 * i)
            end = d.index(b'\0', stroff + strx) if strx else stroff
            name = d[stroff + strx:end].decode('latin-1') if strx else ''
            out.append((i, strx, ntype, sect, desc & 0xffff, value, name))
        return out


def kind(ntype):
    if ntype & 0xe0:
        return STAB_NAMES.get(ntype, 'stab%#x' % ntype)
    return 'sym%#04x' % ntype


def fmt_sym(s):
    i, strx, ntype, sect, desc, value, name = s
    return '#%-5d %-5s sect=%-3d desc=%#06x value=%#010x strx=%-6d %s' % (i, kind(ntype), sect, desc, value, strx, name)


def compare(a, b, args):
    out = []
    same = a.data == b.data
    out.append('%s: %d bytes, sha256 %s' % (a.path, len(a.data), hashlib.sha256(a.data).hexdigest()[:16]))
    out.append('%s: %d bytes, sha256 %s' % (b.path, len(b.data), hashlib.sha256(b.data).hexdigest()[:16]))
    if same:
        out.append('IDENTICAL')
        return 0, out
    first = next((i for i in range(min(len(a.data), len(b.data))) if a.data[i] != b.data[i]), min(len(a.data), len(b.data)))
    reg = next((r for r in a.regions if r[1] <= first < r[1] + r[2]), ('?', 0, 0))
    out.append('first difference at file offset %#x, in %s' % (first, reg[0]))
    # region by region, matched by name and order of appearance
    bnames = {}
    for r in b.regions:
        bnames.setdefault(r[0], []).append(r)
    seen = {}
    for name, off, size in a.regions:
        k = seen.get(name, 0)
        seen[name] = k + 1
        lst = bnames.get(name, [])
        if k >= len(lst):
            out.append('  only in first:  %-46s off=%#x size=%d' % (name, off, size))
            continue
        _, boff, bsize = lst[k]
        ra, rb = a.data[off:off + size], b.data[boff:boff + bsize]
        if ra == rb:
            if args.verbose:
                out.append('  same            %-46s off=%#x size=%d' % (name, off, size))
            continue
        where = '' if off == boff else ' (offsets %#x vs %#x)' % (off, boff)
        nd = sum(1 for x, y in zip(ra, rb) if x != y) + abs(size - bsize)
        out.append('  DIFFERS         %-46s size %d vs %d, %d bytes differ%s' % (name, size, bsize, nd, where))
    for name, lst in bnames.items():
        for r in lst[seen.get(name, 0):]:
            out.append('  only in second: %-46s off=%#x size=%d' % (name, r[1], r[2]))
    ua, ub = a.uuid(), b.uuid()
    if ua != ub:
        out.append('LC_UUID %s vs %s' % (ua.hex() if ua else None, ub.hex() if ub else None))
    if args.symbols:
        out += symdiff(a, b, args.max)
    return 1, out


def symdiff(a, b, limit):
    import difflib
    sa, sb = a.symbols(), b.symbols()
    out = ['symbol table: %d vs %d entries' % (len(sa), len(sb))]
    key = lambda s: (s[2], s[3], s[4], s[5], s[6])
    ka, kb = [key(s) for s in sa], [key(s) for s in sb]
    shown = 0
    sm = difflib.SequenceMatcher(None, ka, kb, autojunk=False)
    for op, i1, i2, j1, j2 in sm.get_opcodes():
        if op == 'equal':
            continue
        for s in sa[i1:i2]:
            if shown < limit:
                out.append('  - ' + fmt_sym(s))
            shown += 1
        for s in sb[j1:j2]:
            if shown < limit:
                out.append('  + ' + fmt_sym(s))
            shown += 1
    if shown > limit:
        out.append('  ... %d more lines' % (shown - limit))
    # string table: same strings in same order?
    if a.symtab and b.symtab:
        ta = a.data[a.symtab[2]:a.symtab[2] + a.symtab[3]]
        tb = b.data[b.symtab[2]:b.symtab[2] + b.symtab[3]]
        if ta != tb:
            la, lb = ta.split(b'\0'), tb.split(b'\0')
            out.append('string table: %d vs %d strings, %d vs %d bytes' % (len(la), len(lb), len(ta), len(tb)))
    return out


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('ours')
    ap.add_argument('retail')
    ap.add_argument('-s', '--symbols', action='store_true', help='diff symbol table entries')
    ap.add_argument('-v', '--verbose', action='store_true', help='list identical regions too')
    ap.add_argument('--max', type=int, default=60, help='symbol diff lines to show')
    args = ap.parse_args()
    try:
        a, b = Image(args.ours), Image(args.retail)
    except (OSError, ValueError) as e:
        print('error: %s' % e, file=sys.stderr)
        return 2
    rc, out = compare(a, b, args)
    print('\n'.join(out))
    return rc


if __name__ == '__main__':
    sys.exit(main())
