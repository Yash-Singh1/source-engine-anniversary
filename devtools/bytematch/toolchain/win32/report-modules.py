#!/usr/bin/env python3
"""Function match summary for a --retail-win32 build directory, with a macOS cross-check.

Runs devtools/bytematch on every linked DLL against its depot 222 counterpart,
and on every static library against retail engine.dll, using the symbol-port
maps (byte-match-survey/symbols/out/<module>.win.json) for names and per-object
scope. Prints a markdown table.

With --mac-build (a --retail-osx32 build directory) the same modules are also
matched against depot 223, and functions are paired across the two reports by
normalised signature: ones that match on one platform but not the other
usually point at source or define differences rather than codegen.

usage: report-modules.py [--build build-win32] [--out DIR] [--mac-build DIR] [--top N]
"""
import argparse, glob, json, os, subprocess, sys

HERE = os.path.dirname(os.path.abspath(__file__))
TREE = os.path.abspath(os.path.join(HERE, "..", "..", "..", ".."))
sys.path.insert(0, os.path.join(TREE, "devtools", "bytematch"))
from bytematch import winmap  # noqa: E402

DEPOTS = "/Volumes/Expansion/mac-ports/hl2-anniversary-depot/depots"
MAPS = "/Volumes/Storage/sco/byte-match-survey/symbols/out"
MATCHED = ("exact", "matches-after-masking")
MAC_NAMES = {"tier0": "libtier0", "vstdlib": "libvstdlib"}


def run(bm, retail, ours, out, maps=()):
    cmd = [bm, "report", "--retail", retail, "--ours", ours, "--out", out]
    for m in maps:
        cmd += ["--retail-map", m]
    r = subprocess.run(cmd, capture_output=True, text=True)
    if r.returncode or not os.path.exists(out + ".json"):
        return None, (r.stderr.strip().splitlines() or ["failed"])[-1]
    return json.load(open(out + ".json")), None


def keyed(doc, demangle):
    """{signature-or-name: (status, retail size, line constants?)} for in-scope functions."""
    names = [f["name"] for f in doc["functions"]]
    dem = demangle(names)
    out = {}
    for f in doc["functions"]:
        n = f["name"]
        k = n if n.startswith("sig:") else winmap.sig(dem[n]) if n in dem else None
        out[k or "c:" + n.lstrip("_")] = (f["status"], f.get("retail_size") or f.get("ours_size") or 0,
                                         bool(f.get("line_constants")))
    return out


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--build", default=os.path.join(TREE, "build-win32"))
    ap.add_argument("--bytematch", default=os.path.join(TREE, "devtools", "bytematch", "bin", "bytematch"))
    ap.add_argument("--depots", default=DEPOTS)
    ap.add_argument("--maps", default=MAPS)
    ap.add_argument("--out", default="/tmp/bytematch-win32-report")
    ap.add_argument("--mac-build", help="--retail-osx32 build directory for the cross-check")
    ap.add_argument("--top", type=int, default=15)
    a = ap.parse_args()
    os.makedirs(a.out, exist_ok=True)
    win = os.path.join(a.depots, "222", "19307283", "bin")

    jobs = []
    for d in sorted(glob.glob(os.path.join(a.build, "**", "*.dll"), recursive=True)):
        stem = os.path.splitext(os.path.basename(d))[0]
        if os.path.exists(os.path.join(win, stem + ".dll")):
            jobs.append((stem, stem, os.path.dirname(d)))
    for lib in sorted(glob.glob(os.path.join(a.build, "**", "*.lib"), recursive=True)):
        stem = os.path.splitext(os.path.basename(lib))[0]
        if "retail-win32-libs" in lib or os.path.exists(os.path.join(os.path.dirname(lib), stem + ".dll")):
            continue  # import libraries
        jobs.append((stem + ".lib (vs engine)", "engine", lib))

    rows, cross = [], []
    for name, mod, ours in jobs:
        out = os.path.join(a.out, name.split()[0])
        m = os.path.join(a.maps, mod + ".win.json")
        doc, err = run(a.bytematch, os.path.join(win, mod + ".dll"), ours, out, [m] if os.path.exists(m) else [])
        rows.append((name, doc and doc["summary"], err))
        if doc and a.mac_build:
            mac_mod = MAC_NAMES.get(mod, mod)
            mac_retail = os.path.join(a.depots, "223", "19307283", "bin", mac_mod + ".dylib")
            if name.endswith("(vs engine)"):
                cand = glob.glob(os.path.join(a.mac_build, "**", "lib" + name.split(".")[0] + ".a"), recursive=True)
            else:
                cand = glob.glob(os.path.join(a.mac_build, "**", mac_mod + ".dylib"), recursive=True)
            if cand and os.path.exists(mac_retail):
                mdoc, _ = run(a.bytematch, mac_retail, cand[0], out + ".mac")
                if mdoc:
                    cross.append((name, keyed(doc, winmap.undname), keyed(mdoc, winmap.cxxfilt)))

    print("| module | functions matched | % functions | bytes matched | % bytes | exact | differs | `__LINE__` only | missing |")
    print("|---|---:|---:|---:|---:|---:|---:|---:|---:|")
    for name, s, err in rows:
        if s is None:
            print(f"| {name} | error: {err} | | | | | | | |")
            continue
        f, b = s["functions"], s["bytes"]
        print(f"| {name} | {f['matched']}/{f['total']} | {f['pct']}% | {b['matched']}/{b['total']} | "
              f"{b['pct']}% | {f['exact']} | {f['differs']} | {s['line_constants_only']['functions']} | {f['missing']} |")

    for name, w, m in cross:
        both = set(w) & set(m)
        mac_only = sorted((k for k in both if m[k][0] in MATCHED and w[k][0] not in MATCHED), key=lambda k: -w[k][1])
        win_only = sorted((k for k in both if w[k][0] in MATCHED and m[k][0] not in MATCHED), key=lambda k: -m[k][1])
        agree = sum(1 for k in both if (w[k][0] in MATCHED) == (m[k][0] in MATCHED))
        print(f"\n### {name}: {len(both)} functions on both platforms, {agree} agree; "
              f"{len(mac_only)} match only on macOS, {len(win_only)} only on Windows")
        for label, ks, side in (("macOS only", mac_only, w), ("Windows only", win_only, m)):
            for k in ks[:a.top]:
                note = " (Windows: `__LINE__` immediates only)" if label == "macOS only" and w[k][2] else ""
                print(f"- {label}: `{k.split(':', 1)[1] if k.startswith(('sig:', 'c:')) else k}` "
                      f"{side[k][1]} B, other side {side[k][0]}{note}")


if __name__ == "__main__":
    main()
