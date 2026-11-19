#!/usr/bin/env python3
"""Compile one source file the way --retail-win32 does and diff it against retail.

The Windows counterpart of devtools/bytematch/macos-cc.sh. Switches, defines
and include paths come from the module's entry in manifests/retail-win32.json
(per-file overrides included); the object is built with /Z7 instead of /Zi and
without precompiled headers, neither of which changes code. With --retail the
object is diffed against the module's depot 222 binary, named through its
symbol-port map when one exists.

usage: win32-cc.py MODULE SOURCE [-o OUT.obj] [--retail] [--against MODULE] [--symbol NAME]
                   [--build build-win32] [-- cl args]
BM_WIN32_MAPS overrides the symbol-port map directories (colon-separated, first hit wins).
"""
import argparse, json, os, subprocess, sys, tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
TREE = os.path.abspath(os.path.join(HERE, "..", "..", "..", ".."))
sys.path.insert(0, HERE)
import versions as V  # noqa: E402

DEPOTS = "/Volumes/Expansion/mac-ports/hl2-anniversary-depot/depots"
MAPS = os.environ.get("BM_WIN32_MAPS", "/Volumes/Storage/sco/byte-match-survey/win32-maps:"
                      "/Volumes/Storage/sco/byte-match-survey/symbols/out").split(":")
RETAIL_DEFINES = ["BYTEMATCH_RETAIL", "NO_X360_XDK"]  # as scripts/waifulib/retail_win32.py


def main():
    argv = sys.argv[1:]
    extra = []
    if "--" in argv:
        i = argv.index("--")
        argv, extra = argv[:i], argv[i + 1:]
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("module")
    ap.add_argument("source")
    ap.add_argument("-o", dest="out")
    ap.add_argument("--retail", action="store_true", help="diff against the module's retail binary")
    ap.add_argument("--symbol", help="show an instruction diff for this function")
    ap.add_argument("--against", help="diff against this module's retail binary (static libraries: engine)")
    ap.add_argument("--build", default=os.environ.get("BM_WIN32_BUILD", os.path.join(TREE, "build-win32")),
                    help="--retail-win32 build directory, for generated headers")
    a = ap.parse_args(argv)

    mods = json.load(open(os.path.join(TREE, "devtools", "bytematch", "manifests", "retail-win32.json")))["modules"]
    m = mods[a.module]
    rel = os.path.relpath(os.path.abspath(a.source), TREE)
    ent = next((s for s in m["sources"] if s["path"] == rel), None)
    if ent is None:
        sys.exit("%s is not a source of %s" % (rel, a.module))
    cl = [s for s in (ent.get("cl") or m["cl"]) if s not in ("/Zi", "/Z7")] + ["/Z7"]
    defines = (ent.get("defines") or m["defines"]) + RETAIL_DEFINES
    incs = [os.path.join(TREE, i) for i in (ent.get("includes") or m["includes"])]
    incs += [os.path.join(a.build, m["project_dir"]), os.path.join(a.build, "retail-win32-shim"),
             os.path.join(V.ROOT, "thirdparty", "include")]
    if "dx9sdk/include" in m["includes"]:
        incs.insert(0, os.path.join(a.build, "retail-win32-override"))
    telemetry = os.path.join(a.build, "retail-win32-telemetry", "include", "tier0")  # see retail_win32.py
    if os.path.isdir(telemetry):
        incs.append(telemetry)
    else:
        defines.append("RAD_TELEMETRY_DISABLED")
    out = os.path.realpath(a.out or os.path.join(tempfile.gettempdir(),
                                                os.path.splitext(os.path.basename(rel))[0] + ".obj"))
    os.makedirs(os.path.dirname(out), exist_ok=True)
    out = os.path.join(os.path.realpath(os.path.dirname(out)), os.path.basename(out))
    cmd = [sys.executable, os.path.join(HERE, "msvc.py"), "cl", "/nologo"] + cl
    cmd += ["/FI" + os.path.join(HERE, "msvc2013_compat.h")]
    cmd += ["/I" + i for i in incs] + ["/D" + d for d in defines]
    cmd += ["/c", "/FC", "/Fo" + out, os.path.join(TREE, rel)] + extra
    r = subprocess.run(cmd, cwd=os.path.join(TREE, m["project_dir"]))
    if r.returncode:
        return r.returncode
    print(out)
    if not a.retail:
        return 0
    against = a.against or a.module
    m = mods[against]
    if not m.get("retail"):
        sys.exit("%s has no retail binary in the manifest; use --against" % against)
    depot = m.get("retail_depot", "222")
    retail = os.path.join(DEPOTS, depot, "19307283", m["retail"])
    bm = [os.path.join(TREE, "devtools", "bytematch", "bin", "bytematch"), "diff", "--retail", retail, "--ours", out]
    key = against if against.startswith(("client_", "server_")) else \
        os.path.splitext(os.path.basename(m["retail"]))[0].lower()
    mp = next((os.path.join(d, key + ".win.json") for d in MAPS if os.path.exists(os.path.join(d, key + ".win.json"))), "")
    if mp:
        bm += ["--retail-map", mp]
    if a.symbol:
        bm += ["--symbol", a.symbol]
    return subprocess.call(bm)


if __name__ == "__main__":
    sys.exit(main())
