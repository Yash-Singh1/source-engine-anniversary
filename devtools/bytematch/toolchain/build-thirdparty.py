#!/usr/bin/env python3
"""Build the period third-party static libraries retail macOS i386 embeds but
for which no period osx32 archive exists locally (see thirdparty-static.md):

  libjpeg 8b      engine, clients, GameUI (retail compiled it as C++)
  libpng 1.5.2    engine, clients, GameUI, replay, chromehtml (system zlib)
  Crypto++ 5.6.1  filesystem_stdio, vpk (lib/common/osx32/libcryptopp.a in Valve's tree)
  speex 1.0.1     vaudio_speex (lib/common/osx32/libspeex.a, built 2008)

Valve's archives came from other compilers than the modules around them. The
default (--compiler period) rebuilds them with those compilers, identified
from the retail code and the archives' own debug info (lane-thirdparty2.md):

  Crypto++  Apple gcc 4.2.1 (gcc-5666.3, build-gcc.sh), Valve's Steam copy of
            5.6.1 (external/crypto++-5.61: patches/cryptopp561-valve*.patch) and
            its GNUmakefile flags; MacOSX10.5.sdk (out-of-line libstdc++ atomics)
  speex     Valve's copy is upstream 1.0.1. Apple gcc 4.2.1 build 5564, except
            nb_celp.c, which Valve compiled with gcc 4.0.1 build 5484; MacOSX10.5.sdk
  libpng    Apple gcc 4.2.1 -O2, MacOSX10.6.sdk
  libjpeg   Apple clang 3.1 (clang-318.0.45, Xcode 4.3, build-clang.sh) -O0, no debug info

--compiler clang builds everything with clang-600 and the retail flags (the
previous behaviour; no period compilers or 10.5 SDK needed). Sources are
pinned by sha256. protobuf 2.3.0 is not built here; Source SDK 2013 ships the
period osx32 archive.

Installs <prefix>/<retailflags.THIRDPARTY_DIR>/{lib,include}.
--speex-archive takes Valve's own prebuilt instead (the 2018 leak's
engine/voice_codecs/speex/source/osx/libspeex.a, the 2008 archive retail
links): our gcc 4.0.1 build of nb_celp.c differs from it in nb_decode only.

Usage: build-thirdparty.py [--prefix DIR] [--only jpeg,png,cryptopp,speex] [--compiler period|clang]
                           [--speex-archive LIBSPEEX_A]
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
# cryptopp561-valve.patch is Valve's external/crypto++-5.61 (2018 leak) against
# upstream; -2012 drops a constructor Valve added later for clang.
PATCHES = {"cryptopp": ["cryptopp561-valve.patch"]}
PERIOD_PATCHES = {"cryptopp": ["cryptopp561-valve-2012.patch"]}
SOURCES = {
    "png": (["https://download.sourceforge.net/libpng/libpng-1.5.2.tar.gz"],
            "1ed281810348dbb61d02465c0dff854c17b826f5c5d76cce32bc0e6adca02156", "libpng-1.5.2"),
    "cryptopp": (["https://www.cryptopp.com/cryptopp561.zip",
                  "https://download.sourceforge.net/cryptopp/cryptopp561.zip"],
                 "98e74d8cb17a38033354519ac8ba9c5d98a6dc00bf5d1ec3c533c2e8ec86f268", "cryptopp561"),
    "speex": (["https://downloads.xiph.org/releases/speex/speex-1.0.1.tar.gz"],
              "2fdd4303bab90c240df478138a744890cde7fa25fbfd4ed29665cd7cb904b26b", "speex-1.0.1"),
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
# libspeex_la_SOURCES order, which is the member order of Valve's archive.
SPEEX_LIB = ("nb_celp sb_celp lpc ltp lsp quant_lsp lsp_tables_nb gain_table gain_table_lbr "
             "cb_search filters bits modes vq high_lsp_tables vbr hexc_table exc_5_256_table "
             "exc_5_64_table exc_8_128_table exc_10_32_table exc_10_16_table exc_20_32_table "
             "hexc_10_32_table misc speex_header speex_callbacks math_approx stereo").split()

# libjpeg's compiler (build-clang.sh): Xcode 4.3's clang.
JPEG_CLANG = "318.0.45"
# Period compilers (build-gcc.sh): version, gcc target, gcc base version.
GCC = {"5666.3": ("i686-apple-darwin10", "4.2.1"), "5564": ("i686-apple-darwin9", "4.2.1"),
       "5484": ("i686-apple-darwin9", "4.0.1")}


def gcc_driver(prefix, version, cxx=False):
    """Driver command for a build-gcc.sh compiler. Darwin 9 targets look for their
    own headers under the SDK, which only carries Darwin 10's, so point at ours."""
    target, base = GCC[version]
    root = os.path.join(prefix, "gcc-" + version)
    cmd = [os.path.join(root, "bin", "g++" if cxx else "gcc")]
    if not os.path.exists(cmd[0]):
        raise SystemExit(f"{cmd[0]} missing; run {HERE}/build-gcc.sh {version}")
    if target.endswith("darwin9"):
        cmd += ["-isystem", os.path.join(root, "lib", "gcc", target, base, "include")]
    return cmd


def sdk_dir(prefix, name):
    d = os.path.join(prefix, "sdk", name)
    if not os.path.isdir(d):
        raise SystemExit(f"{d} missing; run {HERE}/extract-sdk106.sh")
    return d


def fetch(work, name, period):
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
                # python.org builds may lack a CA bundle; curl uses the system's.
                if subprocess.run(["curl", "-fsSL", "-o", path + ".part", u]).returncode == 0:
                    os.replace(path + ".part", path)
                    break
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
    for patch in PATCHES.get(name, []) + (PERIOD_PATCHES.get(name, []) if period else []):
        with open(os.path.join(HERE, "patches", patch)) as f:
            subprocess.run(["patch", "-p1", "--forward", "-s"], cwd=src, stdin=f, check=True)
    return src


def compile_all(jobs, env=None):
    def one(cmd):
        r = subprocess.run(cmd, capture_output=True, text=True, env=env)
        return cmd[-1], r.returncode, r.stderr
    failed = []
    with concurrent.futures.ThreadPoolExecutor(int(os.environ.get("BYTEMATCH_JOBS", os.cpu_count()))) as ex:
        for out, rc, err in ex.map(one, jobs):
            if rc:
                errs = [l for l in err.splitlines() if "error:" in l] or err.strip().splitlines()
                failed.append((out, errs[:2]))
    return failed


def clang_flags(name, src, p):
    """The clang-600 build with the retail flags (--compiler clang)."""
    sdk = p["sdk"]
    if name == "jpeg":
        return [p["cxx"], "-x", "c++"] + rf.cxxflags(sdk), {}
    if name == "png":
        return [p["cc"]] + [f for f in rf.cflags(sdk) if f != "-fvisibility=hidden"], {}
    if name == "cryptopp":
        # 5.6.1 is C++98 code; clang-600's integrated assembler rejects its GNU
        # intel-syntax inline asm, so use the portable C++ paths.
        return ([p["cxx"]] + [("-std=gnu++98" if f == "-std=gnu++11" else f) for f in rf.cxxflags(sdk)]
                + ["-DCRYPTOPP_DISABLE_ASM"]), {}
    return [p["cc"]] + rf.cflags(sdk), {}


def period_flags(name, src, p, prefix):
    """Valve's compiler and flags for each archive, as identified from retail.
    Returns (default command, {source stem: command}) without -c/-o."""
    if name == "jpeg":
        # Retail's libjpeg is Apple clang 3.1 -O0 output without debug info. Its fast-isel
        # still drops a branch to the fall-through block when the branch is alone in its
        # block; LLVM keeps it from April 2012 (Xcode 4.4) on.
        cxx = os.path.join(prefix, "clang-" + JPEG_CLANG, "bin", "clang++")
        if not os.path.exists(cxx):
            raise SystemExit(f"{cxx} missing; run {HERE}/build-clang.sh {JPEG_CLANG}")
        return [cxx, "-x", "c++", "-arch", "i386", "-mmacosx-version-min=" + rf.MIN_OSX,
                "-isysroot", p["sdk"], "-O0", "-fvisibility=hidden", "-w"], {}
    if name == "png":
        # Retail modules export libpng (lane-chromehtml2.md), so default visibility.
        return gcc_driver(prefix, "5666.3") + [
            "-isysroot", sdk_dir(prefix, "MacOSX10.6.sdk"), "-mmacosx-version-min=10.6", "-O2"], {}
    if name == "cryptopp":
        # Valve's GNUmakefile: -DNDEBUG -g -O2 -fPIC, the Steam build's -fvisibility=hidden -w,
        # -pipe, and CRYPTOPP_DISABLE_ASM because Apple's as fails its GNU-as version probe.
        return gcc_driver(prefix, "5666.3", cxx=True) + [
            "-isysroot", sdk_dir(prefix, "MacOSX10.5.sdk"), "-mmacosx-version-min=10.5",
            "-DNDEBUG", "-g", "-O2", "-fPIC", "-fvisibility=hidden", "-w", "-pipe",
            "-DCRYPTOPP_DISABLE_ASM"], {}
    # speex: the archive's DWARF producers name gcc 4.2.1 build 5564 for every object
    # but nb_celp.o (gcc 4.0.1 build 5484), -O2 -g.
    sdk = sdk_dir(prefix, "MacOSX10.5.sdk")
    common = ["-isysroot", sdk, "-mmacosx-version-min=10.5", "-g", "-O2"]
    return gcc_driver(prefix, "5564") + common, {"nb_celp": gcc_driver(prefix, "5484") + common}


def build(name, src, p, out, prefix, period):
    objdir = os.path.join(out, "obj", name)
    shutil.rmtree(objdir, ignore_errors=True)
    os.makedirs(objdir)
    inc = os.path.join(out, "include")
    defs = ["-DNDEBUG"]
    headers = []
    if name == "jpeg":
        shutil.copy(os.path.join(src, "jconfig.txt"), os.path.join(src, "jconfig.h"))
        files = [os.path.join(src, f + ".c") for f in JPEG_LIB]
        headers = [(os.path.join(src, h), "") for h in ("jpeglib.h", "jconfig.h", "jmorecfg.h", "jerror.h")]
    elif name == "png":
        shutil.copy(os.path.join(src, "scripts", "pnglibconf.h.prebuilt"), os.path.join(src, "pnglibconf.h"))
        files = [os.path.join(src, f + ".c") for f in PNG_LIB]
        headers = [(os.path.join(src, h), "") for h in ("png.h", "pngconf.h", "pnglibconf.h")]
    elif name == "cryptopp":
        files = [f for f in sorted(glob.glob(os.path.join(src, "*.cpp")))
                 if os.path.splitext(os.path.basename(f))[0] not in CRYPTOPP_TESTS]
        headers = [(h, "cryptopp") for h in glob.glob(os.path.join(src, "*.h"))]
    elif name == "speex":
        # 1.0.1 keeps its public headers in libspeex/; modules include <speex/speex.h>.
        defs = []
        lib = os.path.join(src, "libspeex")
        files = [os.path.join(lib, f + ".c") for f in SPEEX_LIB]
        headers = [(os.path.join(lib, h), "speex") for h in
                   ("speex.h", "speex_bits.h", "speex_header.h", "speex_callbacks.h", "speex_stereo.h")]
    if period:
        flags, per_file = period_flags(name, src, p, prefix)
        defs = [] if name in ("cryptopp", "speex") else defs
    else:
        flags, per_file = clang_flags(name, src, p)
    env = dict(os.environ, MACOSX_DEPLOYMENT_TARGET="10.5") if period else None
    jobs = []
    for f in files:
        stem = os.path.splitext(os.path.basename(f))[0]
        o = os.path.join(objdir, stem + ".o")
        jobs.append(per_file.get(stem, flags) + ["-I" + os.path.dirname(f)] + defs + ["-c", f, "-o", o])
    failed = compile_all(jobs, env)
    objs = [j[-1] for j in jobs]
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
    ap.add_argument("--compiler", choices=("period", "clang"), default="period")
    ap.add_argument("--speex-archive", help="link this prebuilt libspeex.a instead of building it")
    a = ap.parse_args()
    p = rf.paths(a.prefix)
    period = a.compiler == "period"
    out = os.path.join(a.prefix, rf.THIRDPARTY_DIR)
    work = os.path.join(a.prefix, "work", "thirdparty")
    os.makedirs(work, exist_ok=True)
    for name in a.only.split(","):
        print(build(name, fetch(work, name, period), p, out, a.prefix, period))
        if name == "speex" and a.speex_archive:
            shutil.copy(a.speex_archive, os.path.join(out, "lib", "libspeex.a"))
            print(f"speex: using {a.speex_archive}")


if __name__ == "__main__":
    main()
