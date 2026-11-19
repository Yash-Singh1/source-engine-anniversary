#!/usr/bin/env python3
"""Function match summary for a --retail-win32 build directory, with a macOS cross-check.

Runs devtools/bytematch on every module of manifests/retail-win32.json that the
build linked, against its retail binary (depot 222, or the depot the manifest
names), and on every static library against the retail module that links it
(engine.dll when the engine does). Names and
per-object scope come from the symbol-port maps (<key>.win.json in the --maps
directories, key = lower-case retail file stem, or the module name for game
DLLs); call targets our own link folded together (/OPT:ICF) are accepted
through the link's /MAP file.

With --mac-build (a --retail-osx32 build directory; --mac-game-build GAME=DIR
for the per-game builds) the same modules are matched against depot 223 (or
380), and functions are paired across the two reports by normalised signature:
ones that match on one platform but not the other usually point at source or
define differences rather than codegen.

usage: report-modules.py [--build DIR] [--maps DIR ...] [--mac-build DIR] [--mac-game-build hl2=DIR ...]
                         [--out DIR] [--json FILE] [--top N] [--modules a,b,...]
"""
import argparse, glob, json, os, subprocess, sys

HERE = os.path.dirname(os.path.abspath(__file__))
TREE = os.path.abspath(os.path.join(HERE, "..", "..", "..", ".."))
sys.path.insert(0, os.path.join(TREE, "devtools", "bytematch"))
from bytematch import winmap  # noqa: E402

DEPOTS = "/Volumes/Expansion/mac-ports/hl2-anniversary-depot/depots"
# symbol-port maps: the 16 modules beyond the survey's eight, then the survey's
MAPS = ["/Volumes/Storage/sco/byte-match-survey/win32-maps", "/Volumes/Storage/sco/byte-match-survey/symbols/out"]
MATCHED = ("exact", "matches-after-masking")
BUILD = "19307283"
# manifest module -> (macOS depot, path in it, file name in a --retail-osx32 build)
MAC = {"tier0": ("223", "bin/libtier0.dylib", "libtier0.dylib"),
       "vstdlib": ("223", "bin/libvstdlib.dylib", "libvstdlib.dylib"),
       "launcher_main": ("223", "hl2_osx", "hl2_osx"),
       "client_hl2": ("223", "hl2/bin/client.dylib", "hl2:client.dylib"),
       "server_hl2": ("223", "hl2/bin/server.dylib", "hl2:server.dylib"),
       "client_episodic": ("380", "episodic/bin/client.dylib", "episodic:client.dylib"),
       "server_episodic": ("380", "episodic/bin/server.dylib", "episodic:server.dylib")}


def map_key(name, m):
    if m["type"] == "lib" or name.startswith(("client_", "server_")):
        return name
    if name == "launcher_main":
        return "hl2"
    return os.path.splitext(os.path.basename(m["retail"]))[0].lower()


def run(bm, retail, ours, out, maps=(), link_maps=()):
    cmd = [bm, "report", "--retail", retail, "--ours", ours, "--out", out]
    for m in maps:
        cmd += ["--retail-map", m]
    for m in link_maps:
        cmd += ["--ours-link-map", m]
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
        k = n if n.startswith("sig:") else (winmap.itanium_key(n, dem[n]) if demangle is winmap.cxxfilt else winmap.sig(dem[n])) if n in dem else None
        out[k or "c:" + n.lstrip("_")] = (f["status"], f.get("retail_size") or f.get("ours_size") or 0,
                                         bool(f.get("line_constants")))
    return out


def build_index(man):
    """Module -> the index scripts/waifulib/retail_win32.py gives it (objects are <source>.<index>.o)."""
    import ast
    tree = ast.parse(open(os.path.join(TREE, "scripts", "waifulib", "retail_win32.py")).read())
    mods = next(ast.literal_eval(n.value) for n in tree.body if isinstance(n, ast.Assign)
                and getattr(n.targets[0], "id", "") == "RETAIL_MODULES")
    order = []

    def visit(n):
        if n in order or n not in man:
            return
        for l in man[n]["libs"]:
            if l.get("project"):
                visit(l["project"])
        order.append(n)
    for n in mods:
        visit(n)
    return dict((n, i + 1) for i, n in enumerate(order))


def module_objects(build, name, m, idx, out):
    """A directory of links to the module's own objects (a linked DLL has no symbols to scope by)."""
    d = out + ".objs"
    if os.path.isdir(d):
        for f in os.listdir(d):
            os.remove(os.path.join(d, f))
    os.makedirs(d, exist_ok=True)
    for s in m["sources"]:
        if s.get("missing") or s["lang"] not in ("c", "c++", "asm"):
            continue
        base = os.path.join(os.path.abspath(build), s["path"])
        for cand in (base + ".%d.o" % idx, os.path.splitext(base)[0] + ".%d.obj" % idx):
            if os.path.exists(cand):
                link = os.path.join(d, os.path.basename(cand))
                if not os.path.lexists(link):
                    os.symlink(cand, link)
                break
    return d


def find_one(root, name):
    hits = glob.glob(os.path.join(root, "**", name), recursive=True)
    return sorted(hits, key=len)[0] if hits else None


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--build", default=os.path.join(TREE, "build-win32"))
    ap.add_argument("--bytematch", default=os.path.join(TREE, "devtools", "bytematch", "bin", "bytematch"))
    ap.add_argument("--depots", default=DEPOTS)
    ap.add_argument("--maps", action="append", help="symbol-port map directory (repeatable; first hit wins)")
    ap.add_argument("--out", default="/tmp/bytematch-win32-report")
    ap.add_argument("--json", help="write the per-module summary and cross-check counts here")
    ap.add_argument("--mac-build", help="--retail-osx32 build directory for the cross-check")
    ap.add_argument("--mac-game-build", action="append", default=[], help="GAME=DIR: --build-games=GAME build")
    ap.add_argument("--modules", help="comma-separated manifest modules (default: every one built)")
    ap.add_argument("--top", type=int, default=15)
    a = ap.parse_args()
    maps = a.maps or MAPS
    games = dict(g.split("=", 1) for g in a.mac_game_build)
    os.makedirs(a.out, exist_ok=True)
    man = json.load(open(os.path.join(TREE, "devtools", "bytematch", "manifests", "retail-win32.json")))["modules"]
    want = a.modules.split(",") if a.modules else list(man)
    a.modules_explicit = set(a.modules.split(",")) if a.modules else set()
    index = build_index(man)

    def ours_path(name, m):
        ext = {"dll": ".dll", "exe": ".exe", "lib": ".lib"}[m["type"]]
        d = os.path.join(a.build, m["project_dir"])
        for p in (os.path.join(d, name, m["output"] + ext), os.path.join(d, m["output"] + ext)):
            if os.path.exists(p):
                return p
        return None

    def map_for(key):
        return next((os.path.join(d, key + ".win.json") for d in maps
                     if os.path.exists(os.path.join(d, key + ".win.json"))), None)

    def host(lib):
        """The module a static library is matched in: engine if it links it, else the first that does."""
        users = [n for n, m in man.items() if m["type"] != "lib" and m.get("retail") and ours_path(n, m)
                 and any(l.get("project") == lib for l in m["libs"])]
        return "engine" if "engine" in users else (users[0] if users else None)

    jobs = []
    for name in want:
        m = man.get(name)
        if not m:
            continue
        ours = ours_path(name, m)
        if not ours and m["type"] != "lib" and name in a.modules_explicit:
            # objects without a (successful) link: measured anyway, without folding information
            ours = os.path.join(a.build, m["project_dir"], m["output"] + ".dll")
        if not ours:
            continue
        if m["type"] == "lib":
            h = host(name)
            if not h:
                continue
            hm = man[h]
            retail = os.path.join(a.depots, hm.get("retail_depot", "222"), BUILD, hm["retail"])
            link = os.path.splitext(ours_path(h, hm))[0] + ".map"
            jobs.append(("%s.lib (vs %s)" % (name, h), name, h, retail, ours, map_for(map_key(h, hm)), link))
        elif m.get("retail"):
            retail = os.path.join(a.depots, m.get("retail_depot", "222"), BUILD, m["retail"])
            link = os.path.splitext(ours)[0] + ".map"
            objs = module_objects(a.build, name, m, index[name], os.path.join(a.out, name))
            jobs.append((name, name, name, retail, objs, map_for(map_key(name, m)), link))

    rows, cross = [], []
    for label, name, against, retail, ours, mp, link in jobs:
        out = os.path.join(a.out, label.split()[0])
        doc, err = run(a.bytematch, retail, ours, out, [mp] if mp else [],
                       [link] if link and os.path.exists(link) else [])
        mdoc = None
        if doc and a.mac_build:
            m = man[name]
            lib = m["type"] == "lib"
            hm = man[against]
            depot, rel, fname = MAC.get(against, ("223", "bin/%s.dylib" % hm["output"], hm["output"] + ".dylib"))
            if lib:
                mac_retail = os.path.join(a.depots, depot, BUILD, rel)
                root = a.mac_build
                if ":" in fname:
                    root = games.get(fname.split(":")[0])
                cand = find_one(root, "lib%s.a" % m["output"]) if root else None
            else:
                mac_retail = os.path.join(a.depots, depot, BUILD, rel)
                if ":" in fname:
                    game, fname = fname.split(":")
                    cand = find_one(os.path.join(games[game], "game"), fname) if game in games else None
                else:
                    cand = find_one(a.mac_build, fname)
            if cand and os.path.exists(mac_retail):
                mdoc, _ = run(a.bytematch, mac_retail, cand, out + ".mac")
        rows.append((label, name, doc and doc["summary"], err, mdoc and mdoc["summary"], bool(mp)))
        if doc and mdoc:
            cross.append((label, keyed(doc, winmap.undname), keyed(mdoc, winmap.cxxfilt)))

    print("| module | functions matched | % functions | bytes matched | % bytes | exact | differs | `__LINE__` only "
          "| missing | macOS functions | macOS % |")
    print("|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|")
    summary = []
    for label, name, s, err, ms, has_map in rows:
        if s is None:
            print(f"| {label} | error: {err} | | | | | | | | | |")
            continue
        f, b = s["functions"], s["bytes"]
        mac = f"{ms['functions']['matched']}/{ms['functions']['total']} | {ms['functions']['pct']}%" if ms else " | "
        note = "" if has_map else " (exports only)"
        print(f"| {label}{note} | {f['matched']}/{f['total']} | {f['pct']}% | {b['matched']}/{b['total']} | "
              f"{b['pct']}% | {f['exact']} | {f['differs']} | {s['line_constants_only']['functions']} | "
              f"{f['missing']} | {mac} |")
        summary.append(dict(module=label, name=name, windows=s, macos=ms, symbol_map=has_map))

    xsum = []
    for label, w, m in cross:
        both = set(w) & set(m)
        mac_only = sorted((k for k in both if m[k][0] in MATCHED and w[k][0] not in MATCHED), key=lambda k: -w[k][1])
        win_only = sorted((k for k in both if w[k][0] in MATCHED and m[k][0] not in MATCHED), key=lambda k: -m[k][1])
        agree = sum(1 for k in both if (w[k][0] in MATCHED) == (m[k][0] in MATCHED))
        xsum.append(dict(module=label, both=len(both), agree=agree, mac_only=len(mac_only), win_only=len(win_only),
                         mac_only_top=[(k, w[k][1], w[k][0]) for k in mac_only[:a.top]],
                         win_only_top=[(k, m[k][1], m[k][0]) for k in win_only[:a.top]]))
        print(f"\n### {label}: {len(both)} functions on both platforms, {agree} agree; "
              f"{len(mac_only)} match only on macOS, {len(win_only)} only on Windows")
        for lab, ks, side in (("macOS only", mac_only, w), ("Windows only", win_only, m)):
            for k in ks[:a.top]:
                note = " (Windows: `__LINE__` immediates only)" if lab == "macOS only" and w[k][2] else ""
                print(f"- {lab}: `{k.split(':', 1)[1] if k.startswith(('sig:', 'c:')) else k}` "
                      f"{side[k][1]} B, other side {side[k][0]}{note}")
    if a.json:
        with open(a.json, "w") as f:
            json.dump(dict(modules=summary, cross=xsum), f, indent=1)


if __name__ == "__main__":
    main()
