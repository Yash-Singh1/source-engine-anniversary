#!/usr/bin/env python3
"""Function match summary for a --retail-osx32 build directory.

Runs the tools lane's matcher (devtools/bytematch/bin/bytematch, or --bytematch)
on every linked module (dylibs and executables) against its retail counterpart,
found through the retail manifest, and on every static library against the
retail modules that pull members from it (engine.dylib when it is one of
them), scoped per object by the retail debug map. Prints a markdown table;
--save PREFIX also writes PREFIX.json and PREFIX.md.

The total covers linked modules only: static libraries are linked into them,
so their rows would count the same functions twice.

Game modules come from one --retail-osx32 --build-games=GAME build each.
--game GAME=DIR (repeatable) compares DIR/server.dylib and DIR/client.dylib
with the manifest's GAME/server and GAME/client; they get their own rows and
a second total that includes them.

Usage: report-modules.py [--build build-retail] [--bytematch PATH] [--out DIR] [--save PREFIX]
                         [--game GAME=DIR ...] [--diff-lines N]
"""
import argparse
import glob
import json
import os
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import retailflags as rf  # noqa: E402

TREE = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
MANIFEST = os.path.join(TREE, "devtools", "bytematch", "manifests", "retail-osx32.json")
DEPOTS = os.path.dirname(os.path.dirname(rf.RETAIL_DEPOT))
# Per-module static-library variants (retail_osx32.py's retail_stlib_variant):
# archive name suffix -> the module that links them.
ARCHIVE_VARIANTS = {"_105": "bin/sourcevr"}


def retail_file(mod):
    r = mod["retail"]
    return os.path.join(DEPOTS, r["depot"], os.path.basename(rf.RETAIL_DEPOT), r["path"])


def is_macho_exe(path):
    try:
        with open(path, "rb") as f:
            head = f.read(16)
    except OSError:
        return False
    return head[:4] == b"\xce\xfa\xed\xfe" and int.from_bytes(head[12:16], "little") == 2


def jobs_for(build, manifest):
    by_name = {}
    for key, mod in manifest["modules"].items():
        by_name.setdefault(os.path.basename(mod["retail"]["path"]), []).append((key, mod))
    jobs = []
    # The identity flow keeps relinked copies and extracted retail archives under <build>/identity.
    identity = os.path.join(build, "identity") + os.sep
    linked = sorted(glob.glob(os.path.join(build, "**", "*.dylib"), recursive=True))
    linked += sorted(p for p in glob.glob(os.path.join(build, "**", "*"), recursive=True)
                     if os.path.isfile(p) and "." not in os.path.basename(p) and is_macho_exe(p))
    linked = [p for p in linked if not p.startswith(identity)]
    for path in linked:
        if "retail-depot-libs" in path:
            continue
        cands = by_name.get(os.path.basename(path), [])
        # Engine-side binaries live in bin/; game modules share names across games.
        cands = [c for c in cands if c[0].startswith("bin/") or "/" not in c[0]]
        if len(cands) != 1:
            continue
        key, mod = cands[0]
        jobs.append(("module", key, os.path.basename(path), retail_file(mod), path))
    engine = manifest["modules"]["bin/engine"]
    for lib in sorted(glob.glob(os.path.join(build, "**", "lib*.a"), recursive=True)):
        if lib.startswith(identity):
            continue
        name = os.path.basename(lib)
        variant = [m for s, m in ARCHIVE_VARIANTS.items() if name.endswith(s + ".a")]
        users = variant or manifest["archives"].get(name, {}).get("used_by", [])
        user = "bin/engine" if "bin/engine" in users or not users else users[0]
        mod = manifest["modules"].get(user, engine)
        jobs.append(("archive", name, name, retail_file(mod), lib, user))
    return jobs


def game_jobs(games, manifest):
    jobs = []
    for spec in games:
        game, _, d = spec.partition("=")
        for part in ("server", "client"):
            key = f"{game}/{part}"
            ours = os.path.join(d, f"{part}.dylib")
            if key not in manifest["modules"] or not os.path.exists(ours):
                sys.exit(f"--game {spec}: no manifest module {key} or no {ours}")
            jobs.append(("game", key, f"{key}.dylib", retail_file(manifest["modules"][key]), ours))
    return jobs


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--build", default=os.path.join(TREE, "build-retail"))
    ap.add_argument("--bytematch", default=os.path.join(TREE, "devtools", "bytematch", "bin", "bytematch"))
    ap.add_argument("--out", default="/tmp/bytematch-retail-report", help="per-module matcher reports")
    ap.add_argument("--save", help="write PREFIX.json and PREFIX.md")
    ap.add_argument("--game", action="append", default=[], metavar="GAME=DIR",
                    help="game build output holding server.dylib and client.dylib (repeatable)")
    ap.add_argument("--diff-lines", type=int, help="passed to bytematch report (per-module output only)")
    ap.add_argument("--data-check", action="store_true",
                    help="passed to bytematch report: masked data references must resolve to the same data")
    a = ap.parse_args()
    if not os.path.exists(a.bytematch):
        sys.exit(f"matcher not found at {a.bytematch}; pass --bytematch")
    os.makedirs(a.out, exist_ok=True)
    manifest = json.load(open(MANIFEST))

    rows = []
    for job in jobs_for(a.build, manifest) + game_jobs(a.game, manifest):
        kind, key, name, retail, ours = job[:5]
        out = os.path.join(a.out, name.replace("/", "-"))
        cmd = [a.bytematch, "report", "--retail", retail, "--ours", ours, "--out", out]
        if a.diff_lines:
            cmd += ["--diff-lines", str(a.diff_lines)]
        if a.data_check:
            cmd.append("--data-check")
        r = subprocess.run(cmd, capture_output=True, text=True)
        row = {"kind": kind, "module": key, "name": name, "retail": os.path.relpath(retail, DEPOTS),
               "ours": os.path.relpath(ours, a.build) if kind != "game" else ours}
        if kind == "archive":
            row["compared_with"] = job[5]
        if r.returncode or not os.path.exists(out + ".json"):
            row["error"] = (r.stderr.strip().splitlines() or ["failed"])[-1]
        else:
            row["summary"] = json.load(open(out + ".json"))["summary"]
        rows.append(row)

    def sum_rows(kinds):
        tot = {"functions": {}, "bytes": {}}
        for part in tot:
            for field in ("total", "exact", "matches_after_masking", "differs", "missing", "matched"):
                tot[part][field] = sum(r["summary"][part][field] for r in rows
                                       if r["kind"] in kinds and "summary" in r)
            t = tot[part]
            t["pct"] = round(100.0 * t["matched"] / t["total"], 2) if t["total"] else 0.0
        return tot

    total = sum_rows(("module",))
    total_games = sum_rows(("module", "game")) if a.game else None

    lines = ["| module | functions matched | % functions | bytes matched | % bytes | exact | masked | differs | missing |",
             "|---|---:|---:|---:|---:|---:|---:|---:|---:|"]

    def fmt(name, s):
        f, b = s["functions"], s["bytes"]
        return (f"| {name} | {f['matched']}/{f['total']} | {f['pct']}% | {b['matched']}/{b['total']} | "
                f"{b['pct']}% | {f['exact']} | {f['matches_after_masking']} | {f['differs']} | {f['missing']} |")

    sections = [("module", None), ("archive", "static libraries (members pulled by the named retail module)")]
    if a.game:
        sections.insert(1, ("game", "game modules (one --build-games build each)"))
    for kind, title in sections:
        if title:
            lines.append(f"| **{title}** | | | | | | | | |")
        for r in sorted((r for r in rows if r["kind"] == kind),
                        key=lambda r: -r.get("summary", {}).get("functions", {}).get("pct", -1)):
            name = r["name"] if kind != "archive" else f"{r['name']} (vs {r['compared_with'].split('/')[-1]})"
            lines.append(f"| {name} | error: {r['error']} | | | | | | | |" if "error" in r else fmt(name, r["summary"]))
        if kind == "module":
            lines.append(fmt("**total, linked modules**", total))
        if kind == "game":
            lines.append(fmt("**total, linked modules and games**", total_games))
    table = "\n".join(lines)
    print(table)

    if a.save:
        os.makedirs(os.path.dirname(os.path.abspath(a.save)), exist_ok=True)
        meta = {"generated": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()),
                "depot_build": os.path.basename(rf.RETAIL_DEPOT),
                "clang": rf.CLANG_VERSION, "ld64": rf.LD64_VERSION}
        with open(a.save + ".json", "w") as f:
            doc = {"meta": meta, "total": total, "rows": rows}
            if total_games:
                doc["total_with_games"] = total_games
            json.dump(doc, f, indent=1)
            f.write("\n")
        with open(a.save + ".md", "w") as f:
            f.write(f"# --retail-osx32 match report ({meta['generated']})\n\n")
            f.write(f"clang-{meta['clang']}, ld64-{meta['ld64']}, retail build {meta['depot_build']}. "
                    "Matched = exact + matches-after-masking (bytematch). Percentages are of the "
                    "retail functions in scope (debug-map objects); bytes are retail bytes. "
                    "The totals count linked modules only, since static libraries are linked into them. "
                    "Game modules (server/client per game, against depots 380, 340 and 223) have their "
                    "own rows and a second total.\n\n")
            f.write(table + "\n")


if __name__ == "__main__":
    main()
