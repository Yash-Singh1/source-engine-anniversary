#!/usr/bin/env python3
"""Infer a retail DLL's object link order from its .text layout and compare it with the VPC order.

link.exe lays out .text$mn contributions in input order: the module's own
objects in command-line order, then library members as they are pulled in.
The symbol-port map attributes most retail functions to a macOS object; the
median address of each object's confidently named, non-trivial functions
gives its position. (Header inlines go to the first object that emits them,
so medians, not first addresses.)

usage: link-order.py MODULE [--maps DIR] [--manifest FILE] [--json]
"""
import argparse, collections, json, os, statistics, sys

HERE = os.path.dirname(os.path.abspath(__file__))
TREE = os.path.abspath(os.path.join(HERE, "..", "..", "..", ".."))
sys.path.insert(0, os.path.join(TREE, "devtools", "bytematch"))
from bytematch.model import object_stem  # noqa: E402

MAPS = "/Volumes/Storage/sco/byte-match-survey/symbols/out"


def retail_order(rows, min_conf=0.9, min_size=17):
    addrs = collections.defaultdict(list)
    for r in rows:
        if r.get("mac_obj") and float(r.get("confidence") or 0) >= min_conf and (r.get("size") or 0) >= min_size:
            addrs[object_stem(r["mac_obj"])].append(int(r["address"], 16))
    pos = dict((o, statistics.median(a)) for o, a in addrs.items() if len(a) >= 2)
    return sorted(pos, key=pos.get), dict((o, len(addrs[o])) for o in pos)


def inversions(seq, rank):
    s = [rank[x] for x in seq if x in rank]
    return sum(1 for i in range(len(s)) for j in range(i + 1, len(s)) if s[i] > s[j]), len(s)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("module")
    ap.add_argument("--maps", default=MAPS)
    ap.add_argument("--manifest", default=os.path.join(TREE, "devtools", "bytematch", "manifests", "retail-win32.json"))
    ap.add_argument("--json", action="store_true")
    a = ap.parse_args()
    rows = json.load(open(os.path.join(a.maps, a.module + ".win.json")))
    order, counts = retail_order(rows)
    m = json.load(open(a.manifest))["modules"]
    mod = m[a.module]
    own = [object_stem(s["path"]) for s in mod["sources"] if s["lang"] in ("c", "c++", "asm")]
    libs = []
    for l in mod["libs"]:
        p = m.get(l.get("project") or "")
        if p and p["type"] == "lib":
            libs.append((l["project"], set(object_stem(s["path"]) for s in p["sources"])))
    ownset = set(own)
    retail_own = [o for o in order if o in ownset]
    rank = dict((o, i) for i, o in enumerate(retail_own))
    inv, n = inversions(own, rank)
    first_lib = next((i for i, o in enumerate(order) if o not in ownset), len(order))
    tail_own = [o for o in order[first_lib:] if o in ownset]

    def lib_of(o):
        return next((name for name, members in libs if o in members), "?")

    if a.json:
        json.dump(dict(module=a.module, retail_order=order, vpc_order=own, inversions=inv, compared=n,
                       library_members=[(o, lib_of(o)) for o in order if o not in ownset]), sys.stdout, indent=1)
        return
    print(f"{a.module}: {len(order)} objects located in retail .text ({sum(counts.values())} functions)")
    print(f"  own objects: VPC order vs retail layout, {inv} inversions over {n} objects "
          f"({n * (n - 1) // 2} pairs)")
    if tail_own:
        print(f"  own objects placed after library members: {', '.join(tail_own)}")
    for i, o in enumerate(order):
        where = "own" if o in ownset else lib_of(o)
        vpc = own.index(o) if o in ownset else None
        print(f"  {i:3d}  {o:32s} {where:16s} {'vpc #%d' % vpc if vpc is not None else ''}  ({counts[o]} fns)")


if __name__ == "__main__":
    main()
