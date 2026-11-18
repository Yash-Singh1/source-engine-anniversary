#!/usr/bin/env python3
"""Build stand-ins for third-party static libraries retail Windows links but
that exist nowhere locally (see thirdparty-static.md):

  Crypto++ 5.6.1  filesystem_stdio, GameUI (lib/public/2013/release/cryptlib.lib in Valve's tree)
  libpng 1.5.2    GameUI (lib/public/libpng.lib)

Same pinned sources as ../build-thirdparty.py (the macOS equivalent), compiled
with MSVC 2013 Update 5 and Valve's Retail library switches. These let modules
link in --retail-win32 mode; they are known non-matching inputs (Valve's
archives came from other builds). libz, libcurl and libprotobuf come from
Source SDK 2013's prebuilt archives instead.

Installs $BYTEMATCH_TOOLCHAIN/win32/thirdparty/{lib,include}.
usage: build-thirdparty.py [--only cryptopp,png]
"""
import argparse, concurrent.futures, glob, importlib.util, os, shutil, subprocess, sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import versions as V  # noqa: E402

spec = importlib.util.spec_from_file_location("mac_thirdparty", os.path.join(HERE, "..", "build-thirdparty.py"))
MAC = importlib.util.module_from_spec(spec)
spec.loader.exec_module(MAC)

TREE = os.path.abspath(os.path.join(HERE, "..", "..", "..", ".."))
OUT = os.path.join(V.ROOT, "thirdparty")
MSVC = [sys.executable, os.path.join(HERE, "msvc.py")]
CL = ["/nologo", "/c", "/O2", "/Ob2", "/Oi", "/Ot", "/GF", "/MT", "/Gy", "/GS-", "/arch:SSE", "/fp:fast",
      "/Zc:forScope", "/Zc:wchar_t", "/Gd", "/Z7", "/W0", "/DWIN32", "/D_WINDOWS", "/DNDEBUG", "/D_LIB",
      "/D_MBCS", "/D_USING_V110_SDK71_"]
BATCH = 12


def compile_all(srcs, flags, objdir):
    batches = [srcs[i:i + BATCH] for i in range(0, len(srcs), BATCH)]

    def one(b):
        r = subprocess.run(MSVC + ["cl"] + flags + ["/Fo" + objdir + "/"] + b, capture_output=True, text=True)
        return b, r.returncode, (r.stdout + r.stderr)
    failed = []
    with concurrent.futures.ThreadPoolExecutor(4) as ex:
        for b, rc, out in ex.map(one, batches):
            if rc:
                failed.append([l for l in out.splitlines() if "error" in l][:3])
    return failed


def build(name, src):
    objdir = os.path.join(OUT, "obj", name)
    shutil.rmtree(objdir, ignore_errors=True)
    os.makedirs(objdir)
    if name == "png":
        shutil.copy(os.path.join(src, "scripts", "pnglibconf.h.prebuilt"), os.path.join(src, "pnglibconf.h"))
        srcs = [os.path.join(src, f + ".c") for f in MAC.PNG_LIB]
        flags = CL + ["/TC", "/I" + src, "/I" + os.path.join(TREE, "thirdparty", "zlib")]
        lib, headers = "libpng.lib", [(os.path.join(src, h), "") for h in ("png.h", "pngconf.h", "pnglibconf.h")]
    elif name == "cryptopp":
        srcs = [f for f in sorted(glob.glob(os.path.join(src, "*.cpp")))
                if os.path.splitext(os.path.basename(f))[0] not in MAC.CRYPTOPP_TESTS]
        flags = CL + ["/TP", "/EHsc", "/GR", "/I" + src]
        lib, headers = "cryptlib.lib", [(h, "cryptopp") for h in glob.glob(os.path.join(src, "*.h"))]
    failed = compile_all(srcs, flags, objdir)
    if failed:
        for f in failed:
            print("%s: %s" % (name, " | ".join(f)), file=sys.stderr)
        return "%s: %d batches failed, no library written" % (name, len(failed))
    os.makedirs(os.path.join(OUT, "lib"), exist_ok=True)
    objs = sorted(glob.glob(os.path.join(objdir, "*.obj")))
    out = os.path.join(OUT, "lib", lib)
    subprocess.run(MSVC + ["lib", "/nologo", "/MACHINE:X86", "/OUT:" + out] + objs, check=True)
    for h, sub in headers:
        d = os.path.join(OUT, "include", sub)
        os.makedirs(d, exist_ok=True)
        shutil.copy(h, d)
    return "%s: %d objects -> %s" % (name, len(objs), out)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--only", default="cryptopp,png")
    a = ap.parse_args()
    work = os.path.join(V.ROOT, "work", "thirdparty")
    os.makedirs(work, exist_ok=True)
    subprocess.call(MSVC + ["--start"])
    for name in a.only.split(","):
        print(build(name, MAC.fetch(work, name)))


if __name__ == "__main__":
    main()
