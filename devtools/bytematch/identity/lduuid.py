#!/usr/bin/env python3
"""Recompute an i386 Mach-O image's LC_UUID the way ld64-242 does.

ld64 (OutputFile::computeContentUUID) hashes the whole output with MD5 but
skips the debug-note (stab) nlist entries and the stab strings, then forces
the RFC 4122 version-3 bits. ld64 writes stabs first in the symbol table and
their strings first in the string pool, so the skipped ranges are
[symoff, symoff + 12*nstabs) and [stroff + first stab strx, stroff + first
non-stab strx). The UUID field itself is hashed as zeros.

  lduuid.py FILE            print stored and recomputed UUIDs
  lduuid.py --check FILE    exit 1 unless they agree
"""
import hashlib
import struct
import sys


def analyse(data):
    ncmds = struct.unpack_from('<I', data, 16)[0]
    off, uuid_off, symtab = 28, None, None
    for _ in range(ncmds):
        cmd, size = struct.unpack_from('<2I', data, off)
        if cmd == 0x1B:
            uuid_off = off + 8
        elif cmd == 0x2:
            symtab = struct.unpack_from('<4I', data, off + 8)
        off += size
    return uuid_off, symtab


def compute(data):
    uuid_off, symtab = analyse(data)
    if uuid_off is None:
        return None, None
    buf = bytearray(data)
    stored = bytes(buf[uuid_off:uuid_off + 16])
    buf[uuid_off:uuid_off + 16] = b'\0' * 16
    skip = []
    if symtab:
        symoff, nsyms, stroff, strsize = symtab
        nstabs = 0
        while nstabs < nsyms and buf[symoff + 12 * nstabs + 4] & 0xE0:
            nstabs += 1
        if nstabs:
            first_str = struct.unpack_from('<I', buf, symoff)[0]
            end_str = stroff + strsize
            if nstabs < nsyms:
                end_str = stroff + struct.unpack_from('<I', buf, symoff + 12 * nstabs)[0]
            skip = [(symoff, symoff + 12 * nstabs), (stroff + first_str, end_str)]
    h = hashlib.md5()
    pos = 0
    for a, b in skip:
        h.update(buf[pos:a])
        pos = b
    h.update(buf[pos:])
    d = bytearray(h.digest())
    d[6] = (d[6] & 0x0F) | 0x30
    d[8] = (d[8] & 0x3F) | 0x80
    return stored, bytes(d)


def main(argv):
    check = '--check' in argv
    paths = [a for a in argv if a != '--check']
    rc = 0
    for p in paths:
        with open(p, 'rb') as f:
            stored, calc = compute(f.read())
        if stored is None:
            print('%s: no LC_UUID' % p)
            continue
        ok = stored == calc
        rc |= 0 if ok else 1
        print('%s: stored %s computed %s %s' % (p, stored.hex(), calc.hex(), 'ok' if ok else 'MISMATCH'))
    return rc if check else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
