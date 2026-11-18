#!/usr/bin/env python3
"""Build the period third-party static libraries retail macOS i386 embeds but
for which no period osx32 archive exists locally (see thirdparty-static.md):

  libjpeg 8b      engine, clients, GameUI (retail compiled it as C++)
  libpng 1.5.2    engine, clients, GameUI, replay, chromehtml (system zlib)
  Crypto++ 5.6.1  filesystem_stdio, vpk (lib/common/osx32/libcryptopp.a in Valve's tree)
  speex 1.0.4     vaudio_speex (lib/common/osx32/libspeex.a, built 2008)

Valve's archives were built years earlier with other compilers and unknown
flags, so these are known non-matching inputs: they let modules link in
--retail-osx32 mode, not byte-match. Sources are pinned by sha256. protobuf
2.3.0 is not built here; Source SDK 2013 ships the period osx32 archive.

Installs <prefix>/<retailflags.THIRDPARTY_DIR>/{lib,include}.
Usage: build-thirdparty.py [--prefix DIR] [--only jpeg,png,cryptopp,speex]
"""
import argparse
import concurrent.futures
import glob
import hashlib
import os
import shutil
import subprocess
import sys
import tarfile
import urllib.request
import zipfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import retailflags as rf  # noqa: E402

TREE = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
# libjpeg comes from utils/jpeglib: Valve's copy of 8b with the casts that let it
# compile as C++ (README.valve), which is how the retail Mac archive was built.
IN_TREE = {"jpeg": "utils/jpeglib"}
PATCHES = {"cryptopp": ["cryptopp561-two-phase-lookup.patch"]}
SOURCES = {
    "png": (["https://download.sourceforge.net/libpng/libpng-1.5.2.tar.gz"],
            "1ed281810348dbb61d02465c0dff854c17b826f5c5d76cce32bc0e6adca02156", "libpng-1.5.2"),
    "cryptopp": (["https://www.cryptopp.com/cryptopp561.zip",
                  "https://download.sourceforge.net/cryptopp/cryptopp561.zip"],
                 "98e74d8cb17a38033354519ac8ba9c5d98a6dc00bf5d1ec3c533c2e8ec86f268", "cryptopp561"),
    "speex": (["https://downloads.xiph.org/releases/speex/speex-1.0.4.tar.gz"],
              "d0708ab9d1a6ae6aa8f59cf7b8c37c1cf03fc2c2fe20401a03b08c57c9898630", "speex-1.0.4"),
}

JPEG_LIB = ("jaricom jcapimin jcapistd jcarith jccoefct jccolor jcdctmgr jchuff jcinit jcmainct "
            "jcmarker jcmaster jcomapi jcparam jcprepct jcsample jctrans jdapimin jdapistd jdarith "
            "jdatadst jdatasrc jdcoefct jdcolor jddctmgr jdhuff jdinput jdmainct jdmarker jdmaster "
            "jdmerge jdpostct jdsample jdtrans jerror jfdctflt jfdctfst jfdctint jidctflt jidctfst "
            "jidctint jquant1 jquant2 jutils jmemmgr jmemnobs").split()
PNG_LIB = ("png pngerror pngget pngmem pngpread pngread pngrio pngrtran pngrutil pngset pngtrans "
           "pngwio pngwrite pngwtran pngwutil").split()
CRYPTOPP_TESTS = {"bench", "bench2", "test", "validat1", "validat2", "validat3", "adhoc", "datatest",
                  "regtest", "fipsalgt", "dlltest"}


def fetch(work, name):
    if name in IN_TREE:
        src = os.path.join(work, name + "-tree")
        shutil.rmtree(src, ignore_errors=True)
        shutil.copytree(os.path.join(TREE, IN_TREE[name]), src)
        return src
    urls, sha, top = SOURCES[name]
    path = os.path.join(work, os.path.basename(urls[0]))
    if not os.path.exists(path):
        for u in urls:
            try:
                with urllib.request.urlopen(u, timeout=60) as r, open(path + ".part", "wb") as f:
                    shutil.copyfileobj(r, f)
                os.replace(path + ".part", path)
                break
            except OSError as e:
                print(f"{name}: {u}: {e}", file=sys.stderr)
        else:
            raise SystemExit(f"{name}: download failed")
    got = hashlib.sha256(open(path, "rb").read()).hexdigest()
    if got != sha:
        raise SystemExit(f"{name}: sha256 mismatch {got}")
    src = os.path.join(work, top)
    shutil.rmtree(src, ignore_errors=True)
    if path.endswith(".zip"):
        os.makedirs(src)
        zipfile.ZipFile(path).extractall(src)
    else:
        tarfile.open(path).extractall(work)
    for patch in PATCHES.get(name, []):
        with open(os.path.join(HERE, "patches", patch)) as f:
            subprocess.run(["patch", "-p1", "--forward", "-s"], cwd=src, stdin=f, check=True)
    return src


def compile_all(jobs):
    def one(cmd):
        r = subprocess.run(cmd, capture_output=True, text=True)
        return cmd[-1], r.returncode, r.stderr
    failed = []
    with concurrent.futures.ThreadPoolExecutor(os.cpu_count()) as ex:
        for out, rc, err in ex.map(one, jobs):
            if rc:
                errs = [l for l in err.splitlines() if "error:" in l] or err.strip().splitlines()
                failed.append((out, errs[:2]))
    return failed


def build(name, src, p, out):
    sdk = p["sdk"]
    objdir = os.path.join(out, "obj", name)
    shutil.rmtree(objdir, ignore_errors=True)
    os.makedirs(objdir)
    inc = os.path.join(out, "include")
    defs = ["-DNDEBUG"]
    jobs, headers = [], []
    if name == "jpeg":
        shutil.copy(os.path.join(src, "jconfig.txt"), os.path.join(src, "jconfig.h"))
        # thirdparty-static.md: retail's libjpeg static functions are C++-mangled
        flags = [p["cxx"], "-x", "c++"] + rf.cxxflags(sdk)
        files = [os.path.join(src, f + ".c") for f in JPEG_LIB]
        headers = [(os.path.join(src, h), "") for h in ("jpeglib.h", "jconfig.h", "jmorecfg.h", "jerror.h")]
    elif name == "png":
        shutil.copy(os.path.join(src, "scripts", "pnglibconf.h.prebuilt"), os.path.join(src, "pnglibconf.h"))
        # Retail modules that link libpng export its functions (lane-chromehtml2.md),
        # so dead stripping keeps them all; libjpeg stays hidden as in retail.
        flags = [p["cc"]] + [f for f in rf.cflags(sdk) if f != "-fvisibility=hidden"]
        files = [os.path.join(src, f + ".c") for f in PNG_LIB]
        headers = [(os.path.join(src, h), "") for h in ("png.h", "pngconf.h", "pnglibconf.h")]
    elif name == "cryptopp":
        # 5.6.1 is C++98 code; clang-600's integrated assembler rejects its GNU
        # intel-syntax inline asm, so use the portable C++ paths.
        flags = [p["cxx"]] + [("-std=gnu++98" if f == "-std=gnu++11" else f) for f in rf.cxxflags(sdk)]
        defs.append("-DCRYPTOPP_DISABLE_ASM")
        files = [f for f in sorted(glob.glob(os.path.join(src, "*.cpp")))
                 if os.path.splitext(os.path.basename(f))[0] not in CRYPTOPP_TESTS]
        headers = [(h, "cryptopp") for h in glob.glob(os.path.join(src, "*.h"))]
    elif name == "speex":
        flags = [p["cc"]] + rf.cflags(sdk) + ["-I" + os.path.join(src, "include")]
        files = [f for f in sorted(glob.glob(os.path.join(src, "libspeex", "*.c")))
                 if not os.path.basename(f).startswith("testenc")]
        headers = [(h, "speex") for h in glob.glob(os.path.join(src, "include", "speex", "*.h"))]
    for f in files:
        o = os.path.join(objdir, os.path.splitext(os.path.basename(f))[0] + ".o")
        jobs.append(flags + ["-I" + src] + defs + ["-c", f, "-o", o])
    failed = compile_all(jobs)
    objs = sorted(glob.glob(os.path.join(objdir, "*.o")))
    lib = os.path.join(out, "lib", f"lib{name}.a")
    os.makedirs(os.path.dirname(lib), exist_ok=True)
    if os.path.exists(lib):
        os.unlink(lib)
    if failed:
        for o, err in failed:
            print(f"{name}: FAILED {os.path.basename(o)}: {' | '.join(err)}", file=sys.stderr)
        return f"{name}: {len(failed)}/{len(files)} files failed, no archive written"
    subprocess.run(["libtool", "-static", "-arch_only", "i386", "-o", lib] + objs, check=True,
                   capture_output=True)
    for h, sub in headers:
        d = os.path.join(inc, sub)
        os.makedirs(d, exist_ok=True)
        shutil.copy(h, d)
    return f"{name}: {len(objs)} objects -> {lib}"


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--prefix", default=rf.TOOLCHAIN)
    ap.add_argument("--only", default="jpeg,png,cryptopp,speex")
    a = ap.parse_args()
    p = rf.paths(a.prefix)
    out = os.path.join(a.prefix, rf.THIRDPARTY_DIR)
    work = os.path.join(a.prefix, "work", "thirdparty")
    os.makedirs(work, exist_ok=True)
    for name in a.only.split(","):
        print(build(name, fetch(work, name), p, out))


if __name__ == "__main__":
    main()
