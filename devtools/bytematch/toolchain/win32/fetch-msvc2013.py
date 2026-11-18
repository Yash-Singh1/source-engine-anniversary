#!/usr/bin/env python3
"""Fetch and assemble MSVC 2013 Update 5 + Windows 7.1A SDK from Microsoft's public ISO.

Reads only the needed packages from the Visual Studio Express 2013 for Windows
Desktop (Update 5) ISO with HTTP range requests (about 400 MB of 3.5 GB),
checks each against versions.ISO_FILES, unpacks the MSIs, overlays the Update 5
compiler files and Update 2 CRT libraries from the KB2829760 patches, and
verifies the result against versions.EXPECTED. Output: versions.VS
($BYTEMATCH_TOOLCHAIN/win32/vs2013u5). Microsoft binaries never enter the repo.

Needs msiextract (msitools), cabextract and 7z/7zz on PATH (Homebrew:
msitools cabextract sevenzip). See README.md for the license terms.

usage: fetch-msvc2013.py [--mirror DIR] [--keep-work]
  --mirror DIR   take ISO files from DIR/<iso path> when present (still hash-checked)
"""
import argparse, hashlib, os, shutil, struct, subprocess, sys, urllib.request

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import versions as V

SECTOR = 2048


def sha256(path):
    h = hashlib.sha256()
    with open(path, "rb") as f:
        for b in iter(lambda: f.read(1 << 20), b""):
            h.update(b)
    return h.hexdigest()


def http_range(url, off, n):
    req = urllib.request.Request(url, headers={"Range": "bytes=%d-%d" % (off, off + n - 1)})
    with urllib.request.urlopen(req, timeout=120) as r:
        data = r.read()
    if len(data) != n:
        raise IOError("short range read at %d: %d of %d bytes" % (off, len(data), n))
    return data


def iso_listing(url, wanted):
    """{path: (lba, size)} for WANTED paths, walking only the directories that lead to them."""
    lowered = [w.lower() for w in wanted]
    cache = {}

    def sectors(lba, count):
        if (lba, count) not in cache:
            cache[(lba, count)] = http_range(url, lba * SECTOR, count * SECTOR)
        return cache[(lba, count)]

    root, joliet = None, False
    for lba in range(16, 32):
        vd = sectors(lba, 1)
        if vd[0] == 255:
            break
        if vd[0] == 2 and vd[88:91] in (b"%/@", b"%/C", b"%/E"):
            root, joliet = vd[156:190], True
            break
        if vd[0] == 1 and root is None:
            root = vd[156:190]
    out = {}

    def walk(lba, size, prefix):
        data = sectors(lba, (size + SECTOR - 1) // SECTOR)
        pos = 0
        while pos < size:
            ln = data[pos]
            if ln == 0:
                pos = (pos // SECTOR + 1) * SECTOR
                continue
            rec = data[pos:pos + ln]
            pos += ln
            ext, dsz, flags, nlen = struct.unpack_from("<I", rec, 2)[0], struct.unpack_from("<I", rec, 10)[0], rec[25], rec[32]
            name = rec[33:33 + nlen]
            if name in (b"\0", b"\1"):
                continue
            name = (name.decode("utf-16-be") if joliet else name.decode("ascii")).split(";")[0]
            path = prefix + "/" + name
            if flags & 2:
                if any(w.startswith(path.lower() + "/") for w in lowered):
                    walk(ext, dsz, path)
            elif path.lower() in lowered:
                out[wanted[lowered.index(path.lower())]] = (ext, dsz)

    walk(struct.unpack_from("<I", root, 2)[0], struct.unpack_from("<I", root, 10)[0], "")
    return out


def fetch(mirror):
    listing = None
    for path, (size, digest) in sorted(V.ISO_FILES.items()):
        dst = os.path.join(V.DOWNLOADS, "iso", path.lstrip("/"))
        if os.path.exists(dst) and os.path.getsize(dst) == size and sha256(dst) == digest:
            continue
        os.makedirs(os.path.dirname(dst), exist_ok=True)
        src = os.path.join(mirror, path.lstrip("/")) if mirror else None
        if src and os.path.exists(src):
            shutil.copyfile(src, dst + ".part")
        else:
            if listing is None:
                print("reading ISO directory from %s" % V.ISO_URL, flush=True)
                listing = iso_listing(V.ISO_URL, list(V.ISO_FILES))
            lba, isize = listing[path]
            if isize != size:
                sys.exit("%s: ISO says %d bytes, pinned %d" % (path, isize, size))
            with open(dst + ".part", "wb") as f:
                off, left = lba * SECTOR, size
                while left:
                    n = min(left, 32 << 20)
                    f.write(http_range(V.ISO_URL, off, n))
                    off += n
                    left -= n
        got = sha256(dst + ".part")
        if got != digest:
            sys.exit("%s: sha256 %s, expected %s" % (path, got, digest))
        os.replace(dst + ".part", dst)
        print("fetched %10d %s" % (size, path), flush=True)


def tool(*names):
    for n in names:
        p = shutil.which(n)
        if p:
            return p
    sys.exit("need one of %s on PATH (brew install msitools cabextract sevenzip)" % "/".join(names))


def run(cmd, cwd=None):
    r = subprocess.run(cmd, cwd=cwd, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
    if r.returncode:
        sys.exit("%s failed:\n%s" % (" ".join(cmd), r.stderr.decode(errors="replace")[-2000:]))


def copy_tree(src, dst):
    for d, _, files in os.walk(src):
        rel = os.path.relpath(d, src)
        os.makedirs(os.path.join(dst, rel), exist_ok=True)
        for f in files:
            shutil.copyfile(os.path.join(d, f), os.path.join(dst, rel, f))


def assemble(keep):
    msiextract, cabextract, sevenzip = tool("msiextract"), tool("cabextract"), tool("7zz", "7z")
    iso = os.path.join(V.DOWNLOADS, "iso", "packages")
    work = V.VS + ".work"
    out = V.VS + ".tmp"
    shutil.rmtree(work, ignore_errors=True)
    shutil.rmtree(out, ignore_errors=True)
    os.makedirs(out)

    for pkg, sub, dest in V.MSI_LAYOUT:
        pdir = os.path.join(iso, pkg)
        msi = [f for f in os.listdir(pdir) if f.lower().endswith(".msi")][0]
        x = os.path.join(work, "msi", pkg)
        os.makedirs(x)
        run([msiextract, "-C", x, msi], cwd=pdir)
        copy_tree(os.path.join(x, sub), os.path.join(out, dest))
        print("unpacked %s" % pkg, flush=True)

    for msp, table, base in (("kb2829760vc/kb2829760vc.msp", V.COMPILER_QFE, "VC/bin"),
                             ("kb2829760/kb2829760v2.msp", V.LIBRARY_QFE, "VC")):
        x = os.path.join(work, "msp", os.path.basename(msp))
        os.makedirs(x)
        run([sevenzip, "x", "-y", "-o" + x, os.path.join(iso, msp), "QFE.cab"])
        run([cabextract, "-q", "-d", os.path.join(x, "qfe"), os.path.join(x, "QFE.cab")])
        for member, rel in table:
            shutil.copyfile(os.path.join(x, "qfe", member), os.path.join(out, base, rel))
        print("overlaid %d files from %s" % (len(table), msp), flush=True)

    crt = os.path.join(out, "VC", "redist", "x86", "Microsoft.VC120.CRT")
    for dll in V.NATIVE_RUNTIME:
        shutil.copyfile(os.path.join(crt, dll), os.path.join(out, "VC", "bin", dll))

    bad = [(rel, sha256(os.path.join(out, rel)) if os.path.exists(os.path.join(out, rel)) else "missing")
           for rel, digest in V.EXPECTED.items()
           if not os.path.exists(os.path.join(out, rel)) or sha256(os.path.join(out, rel)) != digest]
    if bad:
        for rel, got in bad:
            print("MISMATCH %s: %s" % (rel, got))
        sys.exit("assembled tree does not match versions.EXPECTED; left in %s" % out)
    shutil.rmtree(V.VS, ignore_errors=True)
    os.replace(out, V.VS)
    if not keep:
        shutil.rmtree(work, ignore_errors=True)
    print("MSVC 2013 Update 5 ready in %s (%d key files verified)" % (V.VS, len(V.EXPECTED)))


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--mirror", help="local copy of the ISO's files, laid out by ISO path")
    ap.add_argument("--keep-work", action="store_true")
    a = ap.parse_args()
    os.makedirs(V.DOWNLOADS, exist_ok=True)
    fetch(a.mirror)
    assemble(a.keep_work)


if __name__ == "__main__":
    main()
