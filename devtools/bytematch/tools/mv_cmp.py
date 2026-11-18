#!/usr/bin/env python3
"""Compare compiled tier0/vstdlib objects against the retail Mac i386 dylibs.

Usage: mv_cmp.py OUTDIR [-v] [--show REGEX] [--json FILE]

Functions are paired per source object using the retail N_OSO/N_FUN stabs.
"norm" = same normalized instruction stream (symdiff.py rules);
"byte" = norm plus identical encodings for every unmasked instruction and
equal lengths for masked ones (PIC displacement, external call, large imm).
Compiler-generated helpers whose names repeat across objects (static
initializers, array dtors, __clang_call_terminate) are not counted.
"""
import collections, difflib, glob, json, os, re, subprocess, sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from symdiff import dump, strip_nops  # noqa: E402

DEPOT = "/Volumes/Expansion/mac-ports/hl2-anniversary-depot/depots/223/19307283/bin"
RETAIL = {"tier0": f"{DEPOT}/libtier0.dylib", "vstdlib": f"{DEPOT}/libvstdlib.dylib"}
SKIP = re.compile(r"^(__GLOBAL__I_|___cxx_global_array_dtor|___clang_call_terminate)")
CACHE = os.path.join(os.path.dirname(os.path.abspath(__file__)), ".cache")


def objmap(path):
    out = subprocess.run(["nm", "-ap", path], capture_output=True, text=True).stdout
    cur, m = None, {}
    for line in out.splitlines():
        r = re.match(r"[0-9a-f]{8} - .. .... +(OSO|FUN) ?(.*)", line)
        if not r:
            continue
        if r.group(1) == "OSO":
            cur = r.group(2).rsplit("/", 1)[-1]
            a = re.match(r".*\((.+\.o)\)$", cur)  # libcoroutine_osx.a(coroutine.o)
            if a:
                cur = a.group(1)
        elif r.group(2):
            m[r.group(2)] = cur
    return m


def retail_dump(mod):
    os.makedirs(CACHE, exist_ok=True)
    c = os.path.join(CACHE, mod + ".json")
    if not os.path.exists(c):
        json.dump({"funcs": dump(RETAIL[mod]), "omap": objmap(RETAIL[mod])}, open(c, "w"))
    d = json.load(open(c))
    return d["funcs"], d["omap"]


def byte_same(r, m):
    n = len(strip_nops(m["n"]))
    for i in range(n):
        if "P(" in m["n"][i] or "EXT" in m["n"][i] or "X" in m["n"][i]:
            if len(r["ib"][i]) != len(m["ib"][i]):
                return False
        elif r["ib"][i] != m["ib"][i]:
            return False
    return True


def main():
    out = sys.argv[1]
    verbose = "-v" in sys.argv
    show = sys.argv[sys.argv.index("--show") + 1] if "--show" in sys.argv else None
    jout = sys.argv[sys.argv.index("--json") + 1] if "--json" in sys.argv else None
    retail = {m: retail_dump(m) for m in RETAIL}
    owner = {}
    for mod, (funcs, omap) in retail.items():
        for k, o in omap.items():
            if k in funcs and o and not SKIP.match(k):
                owner.setdefault(o, (mod, set()))[1].add(k)
    tot = collections.Counter()
    status = {}
    for o in sorted(glob.glob(os.path.join(out, "*.o"))):
        member = os.path.basename(o)
        if member not in owner:
            print(f"{member:24s} (no retail object)")
            continue
        mod, rset = owner[member]
        rfuncs = retail[mod][0]
        mine = dump(o)
        common = sorted(k for k in rset if k in mine)
        same = [k for k in common if strip_nops(rfuncs[k]["n"]) == strip_nops(mine[k]["n"])]
        exact = [k for k in same if byte_same(rfuncs[k], mine[k])]
        print(f"{mod:7s} {member:22s} retail={len(rset):4d} common={len(common):4d} "
              f"norm={len(same):4d} byte={len(exact):4d}")
        for k in rset:
            status[k] = ("byte" if k in exact else "norm" if k in same
                         else "diff" if k in common else "missing")
        tot["retail"] += len(rset); tot["common"] += len(common)
        tot["norm"] += len(same); tot["byte"] += len(exact)
        if verbose:
            for k in sorted(set(common) - set(same)):
                print("   DIFF", k, len(rfuncs[k]["n"]), len(mine[k]["n"]))
            for k in sorted(set(same) - set(exact)):
                print("   BYTEDIFF", k)
            for k in sorted(rset - set(common)):
                print("   MISSING", k)
        if show:
            for k in common:
                if re.search(show, k) and k not in exact:
                    print("=" * 20, k)
                    for l in difflib.unified_diff(strip_nops(rfuncs[k]["n"]), strip_nops(mine[k]["n"]),
                                                  "retail", "ours", lineterm="", n=3):
                        print(l)
    print(f"TOTAL retail={tot['retail']} common={tot['common']} norm={tot['norm']} byte={tot['byte']}")
    if jout:
        json.dump(status, open(jout, "w"), indent=0, sort_keys=True)


if __name__ == "__main__":
    main()
