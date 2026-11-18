#!/usr/bin/env python3
"""Cross-check the macOS object lists against the Windows (222) and Linux (224) builds.

Windows and Linux binaries have no debug map, but __FILE__ strings (asserts,
Msg/Warning helpers, memory tracking) embed build paths. A .c/.cpp/.mm path in
a binary means that translation unit is linked into it. For each Mac module,
collect those paths from all three platforms and compare them with the Mac
N_OSO source list. Also list first-party modules that ship on only some OSes.

Usage: crossplat.py   (writes ../crossplat.json and ../CROSSPLAT.md)
"""
import json, os, re
from collections import OrderedDict

HERE = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.normpath(os.path.join(HERE, ".."))
DEPOTS = "/Volumes/Expansion/mac-ports/hl2-anniversary-depot/depots"
B = "19307283"
ROOTS = [
    ("win", re.compile(rb"[A-Za-z]:\\buildslave\\[A-Za-z0-9_]+\\build\\src\\([\x20-\x7e]+?\.(?:cpp|c|cc|cxx|mm|h|inl))(?![A-Za-z0-9_])", re.I)),
    ("linux", re.compile(rb"/data/src/([\x20-\x7e]+?\.(?:cpp|c|cc|cxx|mm|h|inl))(?![A-Za-z0-9_])")),
    ("mac", re.compile(rb"/Users/buildbot/buildslave/[A-Za-z0-9_]+/build/src/([\x20-\x7e]+?\.(?:cpp|c|cc|cxx|mm|h|inl))(?![A-Za-z0-9_])")),
]
SRC = (".c", ".cc", ".cpp", ".cxx", ".mm")

# third-party redistributables (scope.md) and tools are skipped
THIRD_PARTY = re.compile(r"^(lib(cef|fontconfig|freetype|harfbuzz|MilesX86|Miles|SDL2|steam_api|Telemetry|mimalloc|profiler|dxvk|ffmpegsumo|fbxsdk|xml2|sasl)|"
                         r"ffmpegsumo|steam_api|SDL2|avcodec|avformat|avutil|binkw32|dxvk_d3d9|icudt|Mss32|msvcr71|unicows|"
                         r"xinput|telemetry32|sixense|mss|libfontconfig)", re.I)


def norm(p):
    p = p.decode("latin-1").replace("\\", "/")
    return os.path.normpath(p)


def paths_in(path, kind):
    if not path or not os.path.exists(path):
        return None
    data = open(path, "rb").read()
    rx = dict(ROOTS)[kind]
    return sorted({norm(m.group(1)) for m in rx.finditer(data)})


def counterpart(depot, rel, exts):
    d, base = os.path.split(rel)
    stem = os.path.splitext(base)[0]
    stem = {"libtier0": "tier0", "libvstdlib": "vstdlib", "hl2_osx": "hl2"}.get(stem, stem)
    full = os.path.join(DEPOTS, depot, B, d)
    if not os.path.isdir(full):
        return None
    cands = {f.lower(): f for f in os.listdir(full)}
    for e in exts:
        for name in (stem + e, "lib" + stem + e):
            if name.lower() in cands:
                return os.path.join(full, cands[name.lower()])
    return None


def first_party(depot, sub, exts):
    full = os.path.join(DEPOTS, depot, B, sub)
    if not os.path.isdir(full):
        return set()
    out = set()
    for f in os.listdir(full):
        stem, ext = os.path.splitext(f)
        if os.path.isdir(os.path.join(full, f)):
            continue
        if ext.lower() in exts and not THIRD_PARTY.match(f):
            out.add({"tier0": "libtier0", "vstdlib": "libvstdlib", "togl": "libtogl"}.get(stem.lower(), stem.lower()))
    return out


def main():
    manifest = json.load(open(os.path.join(OUT, "retail-osx32.json")))["modules"]
    mods = {}
    for key in sorted(manifest):
        d = json.load(open(os.path.join(OUT, "modules", key.replace("/", ".") + ".json")))
        mods[key] = (d, manifest[key])
    result = OrderedDict()
    for key, (d, m) in mods.items():
        rel = d["path"]
        mac_bin = os.path.join(DEPOTS, d["depot"], B, rel)
        relw = rel.replace("hl2_osx", "hl2.exe")
        win_depot = "222" if d["depot"] == "223" else d["depot"]
        linux_depot = "224" if d["depot"] == "223" else d["depot"]
        win = counterpart(win_depot, relw, [".dll", ".exe"])
        lin = counterpart(linux_depot, rel.replace("hl2_osx", "hl2_linux"), [".so", ""])
        mac_src = {o["retail_source"] for o in m["objects"] if o["retail_source"]}
        strs = {"mac": paths_in(mac_bin, "mac"), "win": paths_in(win, "win"), "linux": paths_in(lin, "linux")}
        e = OrderedDict(mac=rel, win=os.path.relpath(win, DEPOTS) if win else None,
                        linux=os.path.relpath(lin, DEPOTS) if lin else None)
        for k in ("win", "linux"):
            s = strs[k]
            if s is None:
                continue
            srcs = [p for p in s if p.lower().endswith(SRC)]
            mac_low = {x.lower() for x in mac_src}
            e[k + "_src_paths"] = len(srcs)
            e[k + "_also_in_mac_objects"] = sum(p.lower() in mac_low for p in srcs)
            e[k + "_not_in_mac_objects"] = [p for p in srcs if p.lower() not in mac_low]
        if strs["mac"] is not None:
            e["mac_src_paths"] = len([p for p in strs["mac"] if p.lower().endswith(SRC)])
        result[key] = e
    inv = OrderedDict()
    for sub in ("bin", "hl2/bin", "hl2_complete/bin", "hl2mp/bin"):
        mac = first_party("223", sub, {".dylib", ""}) - {"vpk_osx32"}
        win = first_party("222", sub, {".dll", ".exe", ".com"})
        lin = first_party("224", sub, {".so", ""}) - {"vpk_linux32"}
        inv[sub] = OrderedDict(win_only=sorted(win - mac - lin), linux_only=sorted(lin - mac - win),
                               win_and_linux_not_mac=sorted((win & lin) - mac),
                               mac_only=sorted(mac - win - lin), mac_and_linux_not_win=sorted((mac & lin) - win))
    for depot, sub in (("340", "lostcoast/bin"), ("380", "episodic/bin")):
        full = os.path.join(DEPOTS, depot, B, sub)
        stems = {}
        for f in os.listdir(full):
            s, x = os.path.splitext(f)
            stems.setdefault(s, set()).add(x)
        inv[f"{depot}:{sub}"] = OrderedDict((s, sorted(x)) for s, x in sorted(stems.items()))
    doc = OrderedDict(modules=result, inventory=inv)
    json.dump(doc, open(os.path.join(OUT, "crossplat.json"), "w"), indent=1)
    write_md(doc, os.path.join(OUT, "CROSSPLAT.md"))


def write_md(doc, path):
    L = ["# macOS object lists vs Windows/Linux `__FILE__` evidence", "",
         "Generated by `scripts/crossplat.py`. Windows (depot 222) and Linux (224) binaries carry build paths in "
         "assert/`__FILE__` strings; a `.cpp` path proves that translation unit is linked into the module. Only "
         "files that use `__FILE__` show up, so absence means nothing; presence outside the Mac list marks a "
         "platform-specific object (or a Linux-only later change, since 224 is a May 2025 rebuild).", "",
         "## Module inventory", ""]
    for sub, v in doc["inventory"].items():
        if ":" in sub:
            L.append(f"- `{sub}`: " + ", ".join(f"{s} ({'/'.join(x)})" for s, x in v.items()))
        else:
            L.append(f"- `{sub}`: " + "; ".join(f"{k.replace('_', ' ')}: {', '.join(x) if x else 'none'}" for k, x in v.items()))
    L += ["", "## Per module", "",
          "| module | Windows src paths (also Mac / not Mac) | Linux src paths (also Mac / not Mac) |", "|---|---|---|"]
    for k, e in doc["modules"].items():
        def cell(p):
            if p + "_src_paths" not in e:
                return "no counterpart"
            return f"{e[p + '_src_paths']} ({e[p + '_also_in_mac_objects']} / {len(e[p + '_not_in_mac_objects'])})"
        L.append(f"| {k} | {cell('win')} | {cell('linux')} |")
    L += ["", "## Source files linked on Windows/Linux but not on macOS", ""]
    for k, e in doc["modules"].items():
        w, l = e.get("win_not_in_mac_objects") or [], e.get("linux_not_in_mac_objects") or []
        if w or l:
            L.append(f"- **{k}**" + (f" — Windows: {', '.join('`'+x+'`' for x in w)}" if w else "")
                     + (f" — Linux: {', '.join('`'+x+'`' for x in l)}" if l else ""))
    L.append("")
    open(path, "w").write("\n".join(L))


if __name__ == "__main__":
    main()
