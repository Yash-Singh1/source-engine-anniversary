#!/usr/bin/env python3
"""Install the pinned macOS wine build and create the wine prefix MSVC runs in.

Downloads Gcenx's wine-stable build (versions.WINE_URL, sha256-checked) into
$BYTEMATCH_TOOLCHAIN/win32 and initialises the prefix versions.PREFIX. The
build is WoW64: 32-bit cl.exe/link.exe run inside a 64-bit macOS process,
which Rosetta 2 translates. (Linux containers under Rosetta for Linux cannot
do this and fall back to qemu-i386; see README.md.)

usage: setup-wine.py [--tarball FILE]
"""
import argparse, hashlib, os, shutil, subprocess, sys, urllib.request

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import versions as V


def sha256(path):
    h = hashlib.sha256()
    with open(path, "rb") as f:
        for b in iter(lambda: f.read(1 << 20), b""):
            h.update(b)
    return h.hexdigest()


def install_msxml6(syswow64):
    """Native 32-bit msxml6.dll for mc.exe (msvc.py loads it with msxml6=n,b)."""
    if all(os.path.exists(os.path.join(syswow64, d)) for d in V.MSXML6_X86_FILES.values()):
        return
    cabextract = shutil.which("cabextract") or sys.exit("need cabextract on PATH (brew install cabextract)")
    exe = os.path.join(V.DOWNLOADS, os.path.basename(V.MSXML6_URL))
    if not os.path.exists(exe) or sha256(exe) != V.MSXML6_SHA256:
        urllib.request.urlretrieve(V.MSXML6_URL, exe + ".part")
        os.replace(exe + ".part", exe)
    if sha256(exe) != V.MSXML6_SHA256:
        sys.exit("%s: sha256 mismatch" % exe)
    tmp = os.path.join(V.ROOT, "msxml6.tmp")
    shutil.rmtree(tmp, ignore_errors=True)
    subprocess.check_call([cabextract, "-q", "-d", tmp, exe])
    subprocess.check_call([cabextract, "-q", "-d", tmp, os.path.join(tmp, "msxml6.msi")])
    for src, dst in V.MSXML6_X86_FILES.items():
        shutil.copyfile(os.path.join(tmp, src), os.path.join(syswow64, dst))
    shutil.rmtree(tmp)
    print("installed native msxml6 into %s" % syswow64)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--tarball", help="use this copy of the release tarball instead of downloading")
    a = ap.parse_args()
    if sys.platform != "darwin":
        sys.exit("setup-wine.py installs the macOS wine build; on Linux use docker/ (see README.md)")
    os.makedirs(V.DOWNLOADS, exist_ok=True)
    tar = os.path.join(V.DOWNLOADS, os.path.basename(V.WINE_URL))
    if a.tarball:
        shutil.copyfile(a.tarball, tar)
    if not os.path.exists(tar) or sha256(tar) != V.WINE_SHA256:
        print("downloading %s" % V.WINE_URL, flush=True)
        urllib.request.urlretrieve(V.WINE_URL, tar + ".part")
        os.replace(tar + ".part", tar)
    got = sha256(tar)
    if got != V.WINE_SHA256:
        sys.exit("%s: sha256 %s, expected %s" % (tar, got, V.WINE_SHA256))

    if not os.path.exists(os.path.join(V.WINE_BIN, "wine")):
        tmp = V.WINE_DIR + ".tmp"
        shutil.rmtree(tmp, ignore_errors=True)
        os.makedirs(tmp)
        subprocess.check_call(["tar", "-xJf", tar, "-C", tmp])
        # downloaded with urllib, so no quarantine attribute; clear it anyway for copies from a browser
        subprocess.call(["xattr", "-dr", "com.apple.quarantine", tmp], stderr=subprocess.DEVNULL)
        shutil.rmtree(V.WINE_DIR, ignore_errors=True)
        os.replace(tmp, V.WINE_DIR)
    # Compile-only install: without FreeType, wine skips enumerating every macOS
    # font (through CoreText and the wineserver) in each process, which costs
    # about 3 s per tool run and serialises parallel builds on the wineserver.
    ft = os.path.join(os.path.dirname(V.WINE_BIN), "lib", "libfreetype.6.dylib")
    if os.path.exists(ft):
        os.replace(ft, ft + ".disabled")

    env = dict(os.environ, WINEPREFIX=V.PREFIX, WINEDEBUG="-all", MVK_CONFIG_LOG_LEVEL="0",
               WINEDLLOVERRIDES="mscoree,mshtml=;winemenubuilder.exe=d")
    if not os.path.exists(os.path.join(V.PREFIX, "system.reg")):
        os.makedirs(V.PREFIX, exist_ok=True)
        print("creating wine prefix %s" % V.PREFIX, flush=True)
        subprocess.check_call([os.path.join(V.WINE_BIN, "wine"), "wineboot", "-i"], env=env,
                              stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        subprocess.call([os.path.join(V.WINE_BIN, "wineserver"), "-w"], env=env)
    syswow64 = os.path.join(V.PREFIX, "drive_c", "windows", "syswow64")
    if not os.path.isdir(syswow64):
        sys.exit("prefix has no syswow64: this wine cannot run 32-bit programs here")
    install_msxml6(syswow64)
    print("wine %s ready: %s, prefix %s" % (V.WINE_VERSION, V.WINE_BIN, V.PREFIX))


if __name__ == "__main__":
    main()
