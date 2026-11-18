#!/usr/bin/env python3
"""Thin every fat Mach-O file under a directory to its i386 slice, in place.

Fat files without an i386 slice are left alone. Symlinks are not followed.
"""
import os
import struct
import subprocess
import sys

FAT_MAGIC = 0xCAFEBABE
CPU_TYPE_I386 = 7


def fat_arches(path):
    with open(path, "rb") as f:
        hdr = f.read(8)
        if len(hdr) < 8:
            return None
        magic, n = struct.unpack(">II", hdr)
        if magic != FAT_MAGIC or n > 32:
            return None
        return [struct.unpack(">iiIII", f.read(20))[0] for _ in range(n)]


def main(root):
    thinned = 0
    for dirpath, dirnames, filenames in os.walk(root):
        for name in filenames:
            p = os.path.join(dirpath, name)
            if os.path.islink(p) or not os.path.isfile(p):
                continue
            arches = fat_arches(p)
            if not arches or CPU_TYPE_I386 not in arches:
                continue
            tmp = p + ".thin"
            subprocess.run(["lipo", p, "-thin", "i386", "-output", tmp], check=True)
            os.chmod(tmp, os.stat(p).st_mode & 0o7777)
            os.replace(tmp, p)
            thinned += 1
    print(f"thinned {thinned} files under {root}")


if __name__ == "__main__":
    main(sys.argv[1])
