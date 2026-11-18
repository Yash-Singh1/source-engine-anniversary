#!/usr/bin/env python3
"""String and constant-data audit for a --retail-osx32 build directory.

Runs `bytematch data` (bytematch/audit.py) on every linked module found the
same way as report-modules.py, plus the game modules given with --game, and
writes PREFIX.json and PREFIX.md: per module, differing literals (with the
function or data symbol that shows them and the source line), retail-only and
ours-only literals, literal order, matched functions whose masked data
references resolve to different strings or symbols, and differing constant
data symbols. Per-module JSON/markdown go to --out.

Usage: strings-report.py [--build build-retail] [--game GAME=DIR ...] [--out DIR]
                         [--save PREFIX] [--jobs N] [--only NAME ...] [--reuse]
"""
import argparse
import collections
import importlib.util
import json
import os
import sys
import time
from concurrent.futures import ProcessPoolExecutor

HERE = os.path.dirname(os.path.abspath(__file__))
TREE = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(TREE, "devtools", "bytematch"))
spec = importlib.util.spec_from_file_location("report_modules", os.path.join(HERE, "report-modules.py"))
rm = importlib.util.module_from_spec(spec)
spec.loader.exec_module(rm)

from bytematch import audit  # noqa: E402
from bytematch.model import object_stem  # noqa: E402

PRISTINE = "/Users/yashsingh/p/sco/source-engine-2018-hl2_src"


def run_one(job, out_dir, tree, pristine, reuse=False):
    kind, key, name, retail, ours = job[:5]
    prefix = os.path.join(out_dir, name.replace("/", "-"))
    t = time.time()
    if reuse and os.path.exists(prefix + ".json") and os.path.getmtime(prefix + ".json") > os.path.getmtime(ours):
        return dict(kind=kind, module=key, name=name, retail=retail, ours=ours, seconds=0, prefix=prefix)
    try:
        doc = audit.audit(retail, ours, tree=tree, pristine=pristine)
        audit.write(prefix, doc, title=f"data audit: {name}")
    except Exception as e:                       # one bad module must not sink the run
        import traceback
        return dict(kind=kind, module=key, name=name,
                    error=f"{type(e).__name__}: {e} ({traceback.format_exc().strip().splitlines()[-3].strip()})")
    return dict(kind=kind, module=key, name=name, retail=retail, ours=ours, seconds=round(time.time() - t, 1),
                prefix=prefix)


def loc(hits, n=2):
    return ", ".join(f"{h['file']}:{h['line']}" for h in (hits or [])[:n])


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--build", default=os.path.join(TREE, "build-retail"))
    ap.add_argument("--game", action="append", default=[], metavar="GAME=DIR")
    ap.add_argument("--out", default="/tmp/bytematch-strings")
    ap.add_argument("--save", help="write PREFIX.json and PREFIX.md")
    ap.add_argument("--jobs", type=int, default=4)
    ap.add_argument("--only", action="append", help="module names to include (default all)")
    ap.add_argument("--tree", default=TREE, help="source tree for locating literals")
    ap.add_argument("--pristine", default=PRISTINE)
    ap.add_argument("--reuse", action="store_true", help="keep per-module results newer than our module")
    a = ap.parse_args()
    os.makedirs(a.out, exist_ok=True)
    manifest = json.load(open(rm.MANIFEST))
    jobs, seen = [], set()
    for job in rm.jobs_for(a.build, manifest) + rm.game_jobs(a.game, manifest):
        if job[0] == "archive" or job[1] in seen:
            continue
        if a.only and job[2] not in a.only and job[1] not in a.only:
            continue
        seen.add(job[1])
        jobs.append(job)
    # biggest first so the pool stays busy
    jobs.sort(key=lambda j: -os.path.getsize(j[4]))
    rows = []
    with ProcessPoolExecutor(max_workers=a.jobs) as ex:
        futs = [ex.submit(run_one, j, a.out, a.tree, a.pristine, a.reuse) for j in jobs]
        for f in futs:
            r = f.result()
            print(f"{r['name']}: {r.get('error') or str(r['seconds']) + ' s'}", file=sys.stderr)
            rows.append(r)

    modules = []
    for r in rows:
        if "error" in r:
            modules.append(r)
            continue
        doc = json.load(open(r["prefix"] + ".json"))
        doc.pop("moved", None)                   # long; kept in the per-module files
        doc["module"], doc["name"], doc["kind"] = r["module"], r["name"], r["kind"]
        modules.append(doc)
    modules.sort(key=lambda d: (d.get("kind") != "module", d["name"]))
    total = collections.Counter()
    for d in modules:
        for k, v in d.get("summary", {}).items():
            if isinstance(v, int):
                total[k] += v
    meta = dict(generated=time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()), build=os.path.abspath(a.build),
                games=a.game, depot_build=os.path.basename(rm.rf.RETAIL_DEPOT))
    if a.save:
        os.makedirs(os.path.dirname(os.path.abspath(a.save)), exist_ok=True)
        with open(a.save + ".json", "w") as fh:
            json.dump(dict(meta=meta, total=dict(total), modules=modules), fh, indent=1, default=str)
            fh.write("\n")
        with open(a.save + ".md", "w") as fh:
            fh.write(markdown(meta, total, modules))
    print(markdown(meta, total, modules).split("\n## ")[0])


CATS = ("text", "format", "whitespace/case", "path", "build-stamp")


def markdown(meta, total, modules):
    L = [f"# --retail-osx32 string and data audit ({meta['generated']})", "",
         "Generated by `devtools/bytematch/toolchain/strings-report.py` (`bytematch data`, see the README). "
         "Function matching masks data references; this audit resolves them. Literals are the `__TEXT,__cstring` "
         "contents of each linked module, compared as sets and in order against retail build "
         f"{meta['depot_build']}. A *differing* pair is a retail literal and one of ours that replace each other: "
         "seen at the same reference in a matched function (`function`), in the same pointer slot of a data symbol "
         "(`data`), or edits of each other within one object (`similar R`). *Not truly matching* counts matched "
         "functions whose masked data references resolve to a different string, global (including member offset), "
         "function or constant; `__FILE__` paths and `__DATE__`/`__TIME__` stamps (build-side) are counted apart. "
         "Data symbols are named symbols in `__DATA,__data`, `__DATA,__const` and `__TEXT,__const` compared by "
         "content, pointers resolved to what they point at (an extent runs to the next symbol, so anonymous data "
         "after a symbol is attributed to it).", "",
         "| module | literals retail/ours | differing (text, format, path) | retail-only | ours-only | moved | "
         "not truly matching (strings) / matched | data symbols differing (text) |",
         "|---|---:|---:|---:|---:|---:|---:|---:|"]
    for d in modules:
        if "error" in d:
            L.append(f"| {d['name']} | error: {d['error']} | | | | | | |")
            continue
        s = d["summary"]
        c = s["differing_by_category"]
        L.append(f"| {d['name']} | {s['retail_strings']}/{s['ours_strings']} | {s['differing']} "
                 f"({c.get('text', 0) + c.get('whitespace/case', 0)}, {c.get('format', 0)}, {c.get('path', 0)}) | "
                 f"{s['missing']} | {s['extra']} | {s['moved']} | {s['functions_not_truly_matching']} "
                 f"({s['functions_string_mismatch']}) / {s['matched_functions']} | {s['data_symbols_differing']} "
                 f"({s['data_symbol_kinds'].get('string', 0)}) |")
    L.append(f"| **total** | {total['retail_strings']}/{total['ours_strings']} | {total['differing']} | "
             f"{total['missing']} | {total['extra']} | {total['moved']} | {total['functions_not_truly_matching']} "
             f"({total['functions_string_mismatch']}) / {total['matched_functions']} | "
             f"{total['data_symbols_differing']} |")
    L.append("")

    L += ["## Differing literals (text and format)", "",
          "Retail text first, then ours, then where ours is written. Shared headers appear once per module that "
          "compiles them.", ""]
    for d in modules:
        rows = [x for x in d.get("differing", []) if x["category"] in ("text", "format", "whitespace/case")]
        if not rows:
            continue
        L.append(f"### {d['name']}")
        L.append("")
        for x in rows:
            L.append(f"- [{x['category']}; {x['evidence']}] {object_stem(x['object'] or '?')}: "
                     f"`{x['retail'][:220]}` → ours `{x['ours'][:220]}`"
                     + (f" (after `{x['context'][:80]}`)" if x.get("context") else "")
                     + (f" at {loc(x.get('source'))}" if x.get("source") else ""))
        L.append("")

    L += ["## Matched functions whose data references differ (non-string)", "",
          "Masked references that resolve to a different global, member offset (`layout`), function or constant.", ""]
    for d in modules:
        rows = [m for m in d.get("functions", []) if m["kind"] != "string"]
        if not rows:
            continue
        L.append(f"### {d['name']}")
        L.append("")
        for m in rows[:80]:
            L.append(f"- `{m['function']}` +{m['offset']:#x} ({m.get('category') or m['kind']}) "
                     f"{object_stem(m['object'] or '?')}: retail `{m['retail']}`, ours `{m['ours']}`")
        if len(rows) > 80:
            L.append(f"- ... {len(rows) - 80} more in the JSON")
        L.append("")

    L += ["## Data symbols with differing text", ""]
    for d in modules:
        rows = [x for x in d.get("data_symbols", []) if x["kind"] == "string"]
        if not rows:
            continue
        L.append(f"### {d['name']}")
        L.append("")
        for x in rows[:60]:
            y = next((y for y in x["diffs"] if y["kind"] == "string"), None)
            if y is None:
                continue
            L.append(f"- `{x['name']}` ({x['section']}, {object_stem(x['object'] or '?')}) +{y['offset']}: "
                     f"retail `{y['retail'][:160]}`, ours `{y['ours'][:160]}`")
        L.append("")

    L += ["## Retail-only literals present in our copy of the owning source file", "",
          "The text exists in the right file but not in our module: the code is compiled out (`#if`), in a function that "
          "differs or is missing, dead-stripped, or on a different path.", ""]
    for d in modules:
        rows = [m for m in d.get("missing", []) if m.get("in_owner_source")]
        if not rows:
            continue
        L.append(f"### {d['name']} ({len(rows)})")
        L.append("")
        for m in rows[:40]:
            L.append(f"- {', '.join(object_stem(o) for o in m['objects'][:2]) or '?'}: `{m['text'][:160]}` "
                     f"({loc(m.get('in_tree'))})")
        if len(rows) > 40:
            L.append(f"- ... {len(rows) - 40} more in the JSON")
        L.append("")

    L += ["## Retail-only and ours-only literals by object", "",
          "Top objects per module by unpaired literal count (full lists in the JSON).", ""]
    for d in modules:
        objs = d.get("objects", {})
        top = sorted(((k, v) for k, v in objs.items() if v.get("missing", 0) + v.get("extra", 0)),
                     key=lambda kv: -(kv[1].get("missing", 0) + kv[1].get("extra", 0)))[:12]
        if not top:
            continue
        L.append(f"- **{d['name']}**: " + ", ".join(
            f"{k} (-{v.get('missing', 0)}/+{v.get('extra', 0)})" for k, v in top))
    L.append("")
    return "\n".join(L) + "\n"


if __name__ == "__main__":
    main()
