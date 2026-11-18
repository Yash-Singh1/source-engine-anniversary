#!/usr/bin/env python3
"""Function match summary for a --retail-osx32 build directory.

Runs the tools lane's matcher (devtools/bytematch/bin/bytematch, or --bytematch)
on every linked dylib against its retail counterpart, and on every static
library against retail engine.dylib (which links all of them), scoped per
object by the retail debug map. Prints a markdown table.

Usage: report-modules.py [--build build-retail] [--bytematch PATH] [--out DIR]
"""
import argparse
import glob
import json
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import retailflags as rf  # noqa: E402

TREE = os.path.abspath(os.path.join(HERE, "..", "..", ".."))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--build", default=os.path.join(TREE, "build-retail"))
    ap.add_argument("--bytematch", default=os.path.join(TREE, "devtools", "bytematch", "bin", "bytematch"))
    ap.add_argument("--depot", default=rf.RETAIL_DEPOT)
    ap.add_argument("--out", default="/tmp/bytematch-retail-report")
    a = ap.parse_args()
    if not os.path.exists(a.bytematch):
        sys.exit(f"matcher not found at {a.bytematch}; pass --bytematch")
    os.makedirs(a.out, exist_ok=True)

    jobs = []
    for d in sorted(glob.glob(os.path.join(a.build, "**", "*.dylib"), recursive=True)):
        if "retail-depot-libs" in d:
            continue
        retail = os.path.join(a.depot, "bin", os.path.basename(d))
        if os.path.exists(retail):
            jobs.append((os.path.basename(d), retail, d))
    engine = os.path.join(a.depot, "bin", "engine.dylib")
    for lib in sorted(glob.glob(os.path.join(a.build, "**", "lib*.a"), recursive=True)):
        jobs.append((os.path.basename(lib) + " (vs engine)", engine, lib))

    rows = []
    for name, retail, ours in jobs:
        out = os.path.join(a.out, name.split()[0])
        r = subprocess.run([a.bytematch, "report", "--retail", retail, "--ours", ours, "--out", out],
                           capture_output=True, text=True)
        if r.returncode or not os.path.exists(out + ".json"):
            rows.append((name, None, r.stderr.strip().splitlines()[-1:] or ["failed"]))
            continue
        s = json.load(open(out + ".json"))["summary"]
        rows.append((name, s, None))

    print("| module | functions matched | % functions | bytes matched | % bytes | exact | differs | missing |")
    print("|---|---:|---:|---:|---:|---:|---:|---:|")
    for name, s, err in rows:
        if s is None:
            print(f"| {name} | error: {err[0]} | | | | | | |")
            continue
        f, b = s["functions"], s["bytes"]
        print(f"| {name} | {f['matched']}/{f['total']} | {f['pct']}% | {b['matched']}/{b['total']} | "
              f"{b['pct']}% | {f['exact']} | {f['differs']} | {f['missing']} |")


if __name__ == "__main__":
    main()
