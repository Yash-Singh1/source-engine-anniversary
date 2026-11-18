#!/usr/bin/env python3
"""Per-function status tables for devtools/bytematch/notes/<module>.md.

usage: mv_notes.py MODULE BEFORE_DIR AFTER_DIR OVERLAY_DIR > table.md
Each *_DIR is an mv_measure.sh output directory. Status per retail function:
  identical    matched before and after
  edited       matched after, not before (source changed in this lane)
  new          absent before, matched after
  header       matches only with the measurement overlays (tier1 / interface headers)
  differs      still differs with the overlays
"""
import json, os, subprocess, sys

OK = ("exact", "matches-after-masking")


def load(d, mod):
    rep = json.load(open(os.path.join(d, f"report_{mod}.json")))
    return {f["name"]: f for f in rep["functions"]}


def main():
    mod, before, after, overlay = sys.argv[1:5]
    b, a, o = load(before, mod), load(after, mod), load(overlay, mod)
    rows = []
    for name in sorted(o, key=lambda n: (o[n].get("object") or "", n)):
        fo = o[name]
        fa = a.get(name, {})
        fb = b.get(name)
        ok_a = fa.get("status") in OK
        ok_o = fo.get("status") in OK
        if ok_a:
            status = "identical" if fb and fb.get("status") in OK else ("new" if not fb or fb.get("status") == "missing" else "edited")
        elif ok_o:
            status = "header"
        else:
            status = "differs"
        note = "" if ok_o else fo.get("reason", "")
        rows.append((fo.get("object") or "?", fo.get("demangled") or name, status, note))
    names = subprocess.run(["c++filt", "-_"], input="\n".join(r[1] for r in rows), capture_output=True,
                           text=True).stdout.splitlines()
    rows = [(r[0], n.replace("|", "\\|"), r[2], r[3]) for r, n in zip(rows, names)]
    counts = {}
    for r in rows:
        counts[r[2]] = counts.get(r[2], 0) + 1
    print("| object | function | status | note |")
    print("|---|---|---|---|")
    for obj, fn, st, note in rows:
        print(f"| {obj} | `{fn}` | {st} | {note} |")
    print()
    print("Counts: " + ", ".join(f"{k} {v}" for k, v in sorted(counts.items())))


if __name__ == "__main__":
    main()
