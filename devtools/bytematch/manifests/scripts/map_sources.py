#!/usr/bin/env python3
"""Map retail macOS objects onto our tree and diff them against our waf build.

Reads modules/*.json (from extract_stabs.py), indexes this tree and Valve's
Source SDK 2013, evaluates our wscripts for darwin/x86 (waf_sources.py), and
writes:
  mapping/<module>.json   per-object source mapping, file and library diffs,
                          and a "waf_manifest" (ordered sources and archives)
  mapping/static_libs.json  retail archive membership vs our stlib sources
  DIFF.md                 the human-readable diff list

Usage: map_sources.py [--sdk DIR]
"""
import argparse, json, os, re, sys
from collections import Counter, OrderedDict, defaultdict

HERE = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.normpath(os.path.join(HERE, ".."))
ROOT = os.path.normpath(os.path.join(HERE, "..", "..", "..", ".."))
SDK = "/Volumes/Storage/sco/source-sdk-2013/src"
SRC_EXTS = (".c", ".cc", ".cpp", ".cxx", ".m", ".mm")
sys.path.insert(0, HERE)
import waf_sources  # noqa: E402

# retail module -> (our waf target, GAMES value, note)
MODULE_TARGETS = {
    "bin/datacache": ("datacache", None), "bin/engine": ("engine", None),
    "bin/filesystem_stdio": ("filesystem_stdio", None), "bin/GameUI": ("GameUI", None),
    "bin/inputsystem": ("inputsystem", None), "bin/launcher": ("launcher", None),
    "bin/libtier0": ("tier0", None), "bin/libtogl": ("togl", None), "bin/libvstdlib": ("vstdlib", None),
    "bin/materialsystem": ("materialsystem", None), "bin/scenefilecache": ("scenefilecache", None),
    "bin/ServerBrowser": ("serverbrowser", None), "bin/shaderapidx9": ("shaderapidx9", None),
    "bin/shaderapiempty": ("shaderapiempty", None), "bin/soundemittersystem": ("soundemittersystem", None),
    "bin/stdshader_dx9": ("stdshader_dx9", None), "bin/studiorender": ("studiorender", None),
    "bin/vaudio_miles": ("vaudio_miles", None), "bin/vaudio_speex": ("vaudio_speex", None), "bin/vgui2": ("vgui2", None),
    "bin/vguimatsurface": ("vguimatsurface", None), "bin/video_services": ("video_services", None),
    "bin/vphysics": ("vphysics", None), "bin/vtex_dll": ("vtex_dll", None), "hl2_osx": ("hl2_launcher", None),
    "hl2/client": ("client", "hl2"), "hl2/server": ("server", "hl2"),
    "hl2_complete/client": ("client", "hl2"), "hl2_complete/server": ("server", "episodic"),
    "episodic/client": ("client", "episodic"), "episodic/server": ("server", "episodic"),
    "lostcoast/client": ("client", "hl2"), "lostcoast/server": ("server", "hl2"),
    "hl2mp/client": ("client", "hl2mp"), "hl2mp/server": ("server", "hl2mp"),
}
ARCHIVE_TARGETS = {
    "libappframework.a": "appframework", "libbitmap.a": "bitmap", "libbzip2.a": "bzip2",
    "libchoreoobjects.a": "choreoobjects", "libdmxloader.a": "dmxloader", "libmathlib.a": "mathlib",
    "libmatsys_controls.a": "matsys_controls", "libparticles.a": "particles", "libshaderlib.a": "shaderlib",
    "libtier1.a": "tier1", "libtier2.a": "tier2", "libtier3.a": "tier3", "libvgui_controls.a": "vgui_controls",
    "libvgui_surfacelib.a": "vgui_surfacelib", "libvpklib.a": "vpklib", "libvtf.a": "vtf",
    "libhavana_constraints.a": "havana_constraints", "libhk_base.a": "hk_base", "libhk_math.a": "hk_math",
    "libivp_compactbuilder.a": "ivp_compactbuilder", "libivp_physics.a": "ivp_physics",
}
# retail module -> (tree, VPC path). "sdk" = Source SDK 2013 src/, the closest
# public game-code VPCs; hl2_complete and lostcoast have no public VPC.
MODULE_VPCS = {
    "bin/datacache": ("ours", "datacache/datacache.vpc"), "bin/engine": ("ours", "engine/engine.vpc"),
    "bin/filesystem_stdio": ("ours", "filesystem/filesystem_stdio.vpc"), "bin/GameUI": ("ours", "gameui/GameUI.vpc"),
    "bin/inputsystem": ("ours", "inputsystem/inputsystem.vpc"), "bin/launcher": ("ours", "launcher/launcher.vpc"),
    "bin/libtier0": ("ours", "tier0/tier0.vpc"), "bin/libtogl": ("ours", "togl/togl.vpc"),
    "bin/libvstdlib": ("ours", "vstdlib/vstdlib.vpc"), "bin/materialsystem": ("ours", "materialsystem/materialsystem.vpc"),
    "bin/scenefilecache": ("ours", "scenefilecache/scenefilecache.vpc"),
    "bin/ServerBrowser": ("ours", "serverbrowser/ServerBrowser.vpc"),
    "bin/shaderapidx9": ("ours", "materialsystem/shaderapidx9/shaderapidx9.vpc"),
    "bin/shaderapiempty": ("ours", "materialsystem/shaderapiempty/shaderapiempty.vpc"),
    "bin/soundemittersystem": ("ours", "soundemittersystem/soundemittersystem.vpc"),
    "bin/stdshader_dx9": ("ours", "materialsystem/stdshaders/stdshader_dx9.vpc"),
    "bin/studiorender": ("ours", "studiorender/studiorender.vpc"),
    "bin/vaudio_miles": ("ours", "engine/voice_codecs/miles/vaudio_miles.vpc"),
    "bin/vaudio_speex": ("ours", "engine/voice_codecs/speex/vaudio_speex.vpc"),
    "bin/vgui2": ("ours", "vgui2/src/vgui_dll.vpc"), "bin/vguimatsurface": ("ours", "vguimatsurface/vguimatsurface.vpc"),
    "bin/video_services": ("ours", "video/video_services.vpc"), "bin/vphysics": ("ours", "vphysics/vphysics.vpc"),
    "bin/vtex_dll": ("ours", "utils/vtex/vtex_dll.vpc"), "hl2_osx": ("ours", "launcher_main/launcher_main.vpc"),
    "bin/replay": ("ours", "replay/replay.vpc"), "bin/bsppack": ("ours", "utils/bsppack/bsppack.vpc"),
    "bin/bugreporter_public": ("ours", "utils/bugreporter_public/bugreporter_public.vpc"),
    "bin/bugreporter_filequeue": ("ours", "utils/bugreporter_filequeue/bugreporter_filequeue.vpc"),
    "bin/sourcevr": ("ours", "sourcevr/sourcevr.vpc"), "bin/vpk_osx32": ("ours", "utils/vpk/vpk.vpc"),
    "hl2/client": ("sdk", "game/client/client_hl2.vpc"), "hl2/server": ("sdk", "game/server/server_hl2.vpc"),
    "hl2_complete/client": ("sdk", "game/client/client_hl2.vpc"),
    "hl2_complete/server": ("sdk", "game/server/server_episodic.vpc"),
    "lostcoast/client": ("sdk", "game/client/client_hl2.vpc"), "lostcoast/server": ("sdk", "game/server/server_hl2.vpc"),
    "episodic/client": ("sdk", "game/client/client_episodic.vpc"),
    "episodic/server": ("sdk", "game/server/server_episodic.vpc"),
    "hl2mp/client": ("ours", "game/client/client_hl2mp.vpc"), "hl2mp/server": ("ours", "game/server/server_hl2mp.vpc"),
}
# From src/devtools/base.xcconfig + release.xcconfig in source-sdk-2013 (singleplayer HEAD).
XCCONFIG_DEFINES = ["_DLL_EXT=.dylib", "NO_MALLOC_OVERRIDE=1", "VPROF_LEVEL=1", "NO_HOOK_MALLOC=1",
                    "PNG_NO_PEDANTIC_WARNINGS", "CPP11_NO_LIBCXX"]
XCCONFIG_FLAGS = ["-O2", "-ffast-math", "-ftree-vectorize", "-funswitch-loops", "-fvisibility=hidden",
                  "-fvisibility-inlines-hidden", "-std=gnu++11 (C++)", "-std=gnu99 (C)", "-fobjc-exceptions"]
NOTES = {
    "public/tier0/memoverride.cpp": "no retail Mac module links a memoverride object: Valve's Mac build "
                                    "has no malloc override (malloc from libSystem, operator new from libstdc++)",
}


class Tree:
    def __init__(self, root, skip=(".git",)):
        self.root, self.by_lower, self.by_base = root, {}, defaultdict(list)
        if not os.path.isdir(root):
            return
        for d, dirs, files in os.walk(root):
            dirs[:] = [x for x in dirs if x not in skip]
            for f in files:
                rel = os.path.relpath(os.path.join(d, f), root)
                self.by_lower[rel.lower()] = rel
                self.by_base[f.lower()].append(rel)

    def lookup(self, rel):
        """('exact'|'case'|'moved'|'missing', actual path or candidates)."""
        if self.by_lower.get(rel.lower()) == rel:
            return "exact", rel
        if rel.lower() in self.by_lower:
            return "case", self.by_lower[rel.lower()]
        alts = self.by_base.get(os.path.basename(rel).lower(), [])
        return ("moved", alts) if alts else ("missing", None)


def shared_prefix(a, b):
    n = 0
    for x, y in zip(a.lower().split("/"), b.lower().split("/")):
        if x != y:
            break
        n += 1
    return n


def guess_source(obj, siblings, trees, relocated):
    """Place an object whose stabs don't name its source, by stem and nearby paths.

    `relocated` maps our directory -> retail directory for siblings whose file
    lives elsewhere in our tree, so a guess found in our layout is reported at
    the retail location.
    """
    stem = obj["stem"].lower()
    cands = set()
    for t in trees:
        for ext in SRC_EXTS:
            cands.update(t.by_base.get(stem + ext, []))
    if not cands:
        return None
    anchors = [s for s in siblings if s] + [f for f in obj["files"] if not f.startswith("/")]

    def score(c):
        c = retail_path(c, relocated)
        return max([shared_prefix(c, a) for a in anchors] or [0])
    ranked = sorted(cands, key=lambda c: (-score(c), c))
    return retail_path(ranked[0], relocated)


def retail_path(p, relocated):
    d = os.path.dirname(p)
    return os.path.join(relocated[d], os.path.basename(p)) if d in relocated else p


def project_dir(srcs):
    """Valve's Xcode build phase lists sources sorted by their path relative to
    the project directory, compared uppercased; find the directory that makes
    the observed order sorted (fewest inversions, deepest on ties)."""
    srcs = [s for s in srcs if s and not s.startswith("/")]
    dirs = set()
    for s in srcs:
        d = os.path.dirname(s)
        while d:
            dirs.add(d)
            d = os.path.dirname(d)

    def inv(d):
        k = [os.path.relpath(s, d).upper() for s in srcs]
        return sum(k[i] < k[i - 1] for i in range(1, len(k)))
    if not dirs:
        return None, None
    best = min(sorted(dirs), key=lambda d: (inv(d), -len(d)))
    return best, inv(best)


def lis_len(seq):
    import bisect
    tails = []
    for x in seq:
        i = bisect.bisect_left(tails, x)
        tails[i:i + 1] = [x]
    return len(tails)


def our_targets():
    out = {}
    for games in ("hl2", "episodic", "hl2mp"):
        tgs, errors = waf_sources.evaluate(ROOT, waf_sources.all_subprojects(ROOT), games=games)
        if errors:
            print("wscript errors:", errors, file=sys.stderr)
        for t in tgs:
            if t["subproject"] == "togles":
                continue
            key = (t["target"], games if t["subproject"].startswith("game/") else None)
            out[key] = t
    return out


def map_module(doc, ours, sdk, targets):
    key = doc["module"]
    tgt_name, games = MODULE_TARGETS.get(key, (None, None))
    tgt = targets.get((tgt_name, games)) if tgt_name else None
    trees = [ours, sdk]
    tgt_low = {s.lower() for s in tgt["sources"]} if tgt else set()
    group_srcs = defaultdict(list)
    relocated = Counter()
    for o in doc["objects"]:
        if o["source"]:
            group_srcs[o["archive"] or "<loose>"].append(o["source"])
            if not o["source"].startswith("/") and ours.lookup(o["source"])[0] == "moved":
                alts = ours.lookup(o["source"])[1]
                if len(alts) == 1:
                    relocated[(os.path.dirname(alts[0]), os.path.dirname(o["source"]))] += 1
    reloc = {}
    for (od, rd), n in relocated.most_common():
        reloc.setdefault(od, rd)
    objs = []
    for o in doc["objects"]:
        src, how = o["source"], o["source_resolution"]
        if src and src.startswith("/"):
            how = "outside_src_root"
        elif not src:
            src = guess_source(o, group_srcs[o["archive"] or "<loose>"], trees, reloc)
            how = "guess:stem+siblings" if src else "unplaced"
        st_ours, p_ours = ours.lookup(src) if src and not src.startswith("/") else ("n/a", None)
        st_sdk, p_sdk = sdk.lookup(src) if src and not src.startswith("/") else ("n/a", None)
        ours_path = p_ours if st_ours in ("exact", "case") else None
        if st_ours == "moved":
            in_tgt = [p for p in p_ours if p.lower() in tgt_low]
            if len(in_tgt) == 1 or len(p_ours) == 1:
                ours_path = (in_tgt or p_ours)[0]
        objs.append(OrderedDict(
            index=o["index"], kind=o["kind"], archive=o["archive"], stem=o["stem"],
            mtime_utc=o["mtime_utc"], retail_source=src, resolution=how,
            ours=st_ours, ours_path=ours_path,
            ours_alternatives=p_ours if st_ours == "moved" else None,
            sdk=st_sdk, multi_cpp=[f for f in o["files"] if f.lower().endswith(SRC_EXTS) and f != src]))
    loose = [m for m in objs if m["kind"] == "object"]
    pdir, inversions = project_dir([m["retail_source"] for m in loose])
    if key.endswith(("/client", "/server")):
        pdir = "game/" + key.rsplit("/", 1)[1]
        k = [os.path.relpath(m["retail_source"], pdir).upper() for m in loose if m["retail_source"]]
        inversions = sum(k[i] < k[i - 1] for i in range(1, len(k)))
    res = OrderedDict(module=key, our_target=tgt_name, our_games=games,
                      project_dir=pdir, order_rule_inversions=inversions,
                      counts=OrderedDict(Counter(m["ours"] for m in loose)),
                      counts_sdk=OrderedDict(Counter(m["sdk"] for m in loose)))
    retail_order = []
    for m in loose:
        p = m["ours_path"] or m["retail_source"]
        retail_order.append(p)
    archives = [a["archive"] for a in doc["static_libraries"]]
    res["waf_manifest"] = OrderedDict(
        sources=[m["ours_path"] for m in loose if m["ours_path"]],
        missing=[m["retail_source"] for m in loose if not m["ours_path"]],
        archives=archives,
        archive_targets=[ARCHIVE_TARGETS.get(os.path.basename(a)) for a in archives])
    if tgt:
        ours_srcs = tgt["sources"]
        olow = {s.lower(): s for s in ours_srcs}
        rlow = {p.lower(): p for p in retail_order if p}
        member_stems = {os.path.splitext(m["stem"])[0].lower(): m["archive"] for m in objs if m["archive"]}
        add = []
        for m in loose:
            p = m["ours_path"] or m["retail_source"]
            if p and p.lower() not in olow:
                add.append(OrderedDict(path=m["retail_source"] if m["ours"] == "moved" else p,
                                       in_our_tree=m["ours"], in_sdk=m["sdk"],
                                       alternatives=m["ours_alternatives"]))
        remove = []
        for s in ours_srcs:
            if s.lower() in rlow:
                continue
            stem = os.path.splitext(os.path.basename(s))[0].lower()
            note = NOTES.get(s)
            if not note and stem in member_stems:
                note = f"retail links {stem}.o from {member_stems[stem]}"
            remove.append(OrderedDict(path=s, note=note))
        common_r = [p.lower() for p in retail_order if p and p.lower() in olow]
        pos = {s.lower(): i for i, s in enumerate(ours_srcs)}
        seq = [pos[p] for p in common_r]
        keep = lis_len(seq)
        our_libs = [u for u in tgt["use"] if u in ARCHIVE_TARGETS.values()]
        retail_libs = [ARCHIVE_TARGETS.get(os.path.basename(a), os.path.basename(a)) for a in archives]
        moved = [OrderedDict(retail=m["retail_source"], ours=m["ours_path"]) for m in loose
                 if m["ours"] == "moved" and m["ours_path"] and m["ours_path"].lower() in olow]
        res["diff"] = OrderedDict(
            add=add, remove=remove, path_differs=moved,
            order=OrderedDict(common=len(common_r), in_order_lis=keep, need_moving=len(common_r) - keep,
                              identical=seq == sorted(seq)),
            libraries=OrderedDict(retail=retail_libs, ours=our_libs,
                                  retail_only=[x for x in retail_libs if x not in our_libs],
                                  ours_only=[x for x in our_libs if x not in retail_libs],
                                  same_relative_order=[x for x in retail_libs if x in our_libs] ==
                                  [x for x in our_libs if x in retail_libs]))
    res["objects"] = objs
    return res


PLATFORM_FILE_RE = re.compile(r"(osx|_mac|mac_|cocoa|posix|unix|linux|sdl|win32|_win|openal|audioqueue|quicktime|\.mm$)", re.I)


def vpc_hints(tree_root, vpc_rel, outbinname):
    import vpc_defines
    path = os.path.join(tree_root, vpc_rel)
    if not os.path.exists(path):
        return None
    macros = {"_DLL_EXT": ".dylib", "OUTBINNAME": outbinname, "PROJECTNAME": outbinname}
    r = vpc_defines.evaluate(path, tree_root, macros=macros)
    r["vpc_root"] = tree_root
    return r


def find_vpc(trees, name):
    for t in trees:
        hits = t.by_base.get(name.lower() + ".vpc", [])
        if hits:
            return t.root, sorted(hits, key=len)[0]
    return None, None


def merge_member_order(lists):
    """Topologically merge per-module member orders into one archive order."""
    succ, nodes, indeg = defaultdict(set), [], Counter()
    for lst in lists:
        for x in lst:
            if x not in nodes:
                nodes.append(x)
        for a, b in zip(lst, lst[1:]):
            if b not in succ[a]:
                succ[a].add(b)
                indeg[b] += 1
    out, ready = [], [n for n in nodes if not indeg[n]]
    while ready:
        ready.sort(key=lambda s: s.upper())
        n = ready.pop(0)
        out.append(n)
        for m in succ[n]:
            indeg[m] -= 1
            if not indeg[m]:
                ready.append(m)
    return out + [n for n in nodes if n not in out]


def build_manifest(docs, results, ours, sdk, targets):
    by_key = {r["module"]: r for r in results}
    trees = {"ours": ours, "sdk": sdk}
    modules = OrderedDict()
    for d in waf_order(docs):
        r = by_key[d["module"]]
        info = d["macho"]
        install = (info.get("install_name") or {}).get("path") or d["path"]
        outbin = os.path.splitext(os.path.basename(install))[0]
        tree, vpc = MODULE_VPCS.get(d["module"], (None, None))
        hints = vpc_hints(trees[tree].root, vpc, outbin) if tree else None
        objects = []
        for o, m in zip(d["objects"], r["objects"]):
            objects.append(OrderedDict(
                kind=o["kind"], source=m["ours_path"], retail_source=m["retail_source"],
                status=m["ours"], archive=os.path.basename(o["archive"]) if o["archive"] else None,
                member=o["member"], mtime=o["mtime"], ccache_name=o["so_name"], lang=o["lang"]))
        archives = [OrderedDict(archive=os.path.basename(a["archive"]), path=a["archive"], prebuilt=a["prebuilt"],
                                waf_target=ARCHIVE_TARGETS.get(os.path.basename(a["archive"])),
                                members=a["members"]) for a in d["static_libraries"]]
        loose = [m for m in r["objects"] if m["kind"] == "object"]
        modules[d["module"]] = OrderedDict(
            retail=OrderedDict(depot=d["depot"], path=d["path"], aliases=d["aliases"], install_name=install,
                               filetype=info.get("filetype"), uuid=info.get("uuid"),
                               min_macosx=info.get("min_macosx"), sdk=info.get("sdk"), sha256=d["sha256"],
                               built_mtime_range=d["built_mtime_range"], xcode_targets=d["xcode_targets"]),
            waf_target=r["our_target"], waf_games=r["our_games"],
            project_dir=r["project_dir"], order_rule_inversions=r["order_rule_inversions"],
            sources=[m["ours_path"] for m in loose if m["ours_path"]],
            missing_sources=[m["retail_source"] for m in loose if not m["ours_path"]],
            archives=archives,
            dylibs=info.get("dylibs", []),
            defines=OrderedDict(
                xcconfig=XCCONFIG_DEFINES,
                vpc=hints["defines"] if hints else None,
                vpc_files=[f"{tree}:{f}" for f in hints["vpc_files"]] if hints else None,
                vpc_include_dirs=hints["include_dirs"] if hints else None,
                platform_files=[m["retail_source"] for m in loose
                                if m["retail_source"] and PLATFORM_FILE_RE.search(os.path.basename(m["retail_source"]))]),
            objects=objects)
    archives = OrderedDict()
    for d in docs:
        for a in d["static_libraries"]:
            e = archives.setdefault(os.path.basename(a["archive"]), {"path": a["archive"], "prebuilt": a["prebuilt"],
                                                                    "lists": [], "users": []})
            e["lists"].append(a["members"])
            e["users"].append(d["module"])
    arc_out = OrderedDict()
    for name in sorted(archives, key=str.lower):
        e = archives[name]
        tname = ARCHIVE_TARGETS.get(name)
        stem = name[3:-2] if name.startswith("lib") else name[:-2]
        root, vpc = find_vpc([ours, sdk], stem) if not e["prebuilt"] else (None, None)
        hints = vpc_hints(root, vpc, stem) if vpc else None
        members = merge_member_order(e["lists"])
        srcs = OrderedDict()
        for d in docs:
            r = by_key[d["module"]]
            for o, m in zip(d["objects"], r["objects"]):
                if o["archive"] and os.path.basename(o["archive"]) == name:
                    srcs.setdefault(o["member"], OrderedDict(source=m["ours_path"], retail_source=m["retail_source"],
                                                             status=m["ours"]))
        tree_tag = "ours" if root == ours.root else "sdk"
        arc_out[name] = OrderedDict(
            path=e["path"], prebuilt=e["prebuilt"], waf_target=tname, used_by=sorted(set(e["users"])),
            members=[OrderedDict([("member", mb)] + list(srcs.get(mb, {}).items())) for mb in members],
            defines=OrderedDict(xcconfig=XCCONFIG_DEFINES, vpc=hints["defines"] if hints else None,
                                vpc_files=[f"{tree_tag}:{f}" for f in hints["vpc_files"]] if hints else None))
    return OrderedDict(
        schema=1,
        build="19307283",
        generated_by="devtools/bytematch/manifests/scripts/{extract_stabs,map_sources}.py",
        notes=OrderedDict(
            order="objects are in ld64 link order (N_OSO order == __text layout order in every module): loose "
                  "objects in Xcode build-phase order, then archives in link-line order, each archive's pulled "
                  "members in archive order. Xcode build-phase order = sources sorted by path relative to "
                  "project_dir, compared uppercased; game client/server use three such sorted runs.",
            archives="an archive lists only members some module pulled; prebuilt archives built without -g "
                     "(libpng, libjpeg compiled as C++, protobuf, zlib) leave no N_OSO at all",
            source="'source' is the path in our tree (null if absent); 'retail_source' is Valve's path relative "
                   "to build/src; status is exact|case|moved|missing relative to our tree",
            defines="layered hints: xcconfig (SDK devtools/*.xcconfig) + vpc (evaluated for OSXALL/OSX32/POSIX "
                    "Release) + platform_files selected by retail. NO_MALLOC_OVERRIDE is confirmed by the "
                    "binaries: no memoverride object anywhere, malloc from libSystem, no g_pMemAlloc.",
            toolchain="replay.dylib (2024) carries lib/clang/6.0/include line info (Apple LLVM 6.0, Xcode 6); "
                      "hl2mp (2014) carries lib/clang/5.1; SDK 10.9 libstdc++ 4.2.1 headers; "
                      "LC_VERSION_MIN 10.6. release.xcconfig adds -funswitch-loops."),
        xcconfig_flags=XCCONFIG_FLAGS,
        modules=modules,
        archives=arc_out)


def static_libs(docs, ours, targets):
    members = defaultdict(lambda: defaultdict(set))
    for d in docs:
        for o in d["objects"]:
            if o["kind"] == "archive_member":
                members[os.path.basename(o["archive"])][o["stem"]].add(d["module"])
    out = OrderedDict()
    for arc in sorted(members):
        tname = ARCHIVE_TARGETS.get(arc)
        tgt = targets.get((tname, None)) if tname else None
        ours_stems = {}
        if tgt:
            for s in tgt["sources"]:
                ours_stems[os.path.splitext(os.path.basename(s))[0].lower()] = s
        rstems = {s.lower(): s for s in members[arc]}
        out[arc] = OrderedDict(
            our_target=tname,
            retail_members=OrderedDict((s, sorted(m)) for s, m in sorted(members[arc].items(), key=lambda x: x[0].lower())),
            retail_members_not_in_ours=sorted(s for s in rstems if tgt and s not in ours_stems),
            ours_never_pulled_by_retail=sorted(v for k, v in ours_stems.items() if k not in rstems))
    return out


def fmt_list(xs, n=None):
    xs = list(xs)
    if n and len(xs) > n:
        return ", ".join(f"`{x}`" for x in xs[:n]) + f", … (+{len(xs) - n})"
    return ", ".join(f"`{x}`" for x in xs) if xs else "none"


def write_diff_md(results, libs, path):
    L = ["# Retail macOS build vs our waf build: file and link-order diff", "",
         "Generated by `scripts/map_sources.py` from the N_OSO debug maps of depot 223/340/380/420 "
         "build 19307283, against this tree evaluated for darwin/x86 (`scripts/waf_sources.py`).",
         "Paths are relative to the tree root. *add* = retail compiles it into the module, our target "
         "doesn't. *remove* = our target compiles it, retail doesn't link an object for it (objects that "
         "compile to nothing on OSX also leave no N_OSO). Order counts are over files both sides compile.", ""]
    for r in results:
        d = r.get("diff")
        c = r["counts"]
        L.append(f"## {r['module']}" + (f" (ours: `{r['our_target']}`" + (f", GAMES={r['our_games']}" if r['our_games'] else "") + ")" if r["our_target"] else " (no waf target)"))
        L.append("")
        L.append(f"Retail loose objects: {sum(c.values())} ({', '.join(f'{k} {v}' for k, v in c.items())} in our tree).")
        if not d:
            missing = r["waf_manifest"]["missing"]
            if missing:
                L.append(f"Not in our tree: {fmt_list(missing, 40)}")
            L.append(f"Archives: {fmt_list(os.path.basename(a) for a in r['waf_manifest']['archives'])}")
            L.append("")
            continue
        add_tree = [a for a in d["add"] if a["in_our_tree"] in ("exact", "case")]
        add_moved = [a for a in d["add"] if a["in_our_tree"] == "moved"]
        add_new = [a for a in d["add"] if a["in_our_tree"] in ("missing", "n/a")]
        if add_tree:
            L.append(f"- **add (present in tree)** {len(add_tree)}: {fmt_list((a['path'] for a in add_tree), 60)}")
        if add_moved:
            L.append(f"- **add (same name elsewhere in tree)** {len(add_moved)}: " +
                     ", ".join(f"`{a['path']}` → `{a['alternatives'][0]}`" for a in add_moved[:30]))
        if add_new:
            sdk = [a for a in add_new if a["in_sdk"] in ("exact", "case")]
            L.append(f"- **add (missing from tree, new in retail)** {len(add_new)}"
                     + (f", {len(sdk)} available in SDK 2013" if sdk else "") + f": {fmt_list((a['path'] for a in add_new), 60)}")
        if d["path_differs"]:
            dirs = Counter((os.path.dirname(x["retail"]), os.path.dirname(x["ours"])) for x in d["path_differs"])
            L.append(f"- **same file, different directory** {len(d['path_differs'])}: " + ", ".join(
                f"`{r}/` → `{o}/` ×{n}" for (r, o), n in dirs.most_common()))
        if d["remove"]:
            L.append(f"- **remove** {len(d['remove'])}: " + ", ".join(
                f"`{x['path']}`" + (f" ({x['note']})" if x["note"] else "") for x in d["remove"][:60])
                + (f", … (+{len(d['remove']) - 60})" if len(d["remove"]) > 60 else ""))
        o = d["order"]
        L.append(f"- **order**: {o['common']} common files; " + ("identical order" if o["identical"] else
                 f"{o['need_moving']} must move to match retail (longest in-order run {o['in_order_lis']})"))
        lb = d["libraries"]
        L.append(f"- **static libs** retail link order: {fmt_list(lb['retail'])}; ours: {fmt_list(lb['ours'])}"
                 + (f"; retail-only: {fmt_list(lb['retail_only'])}" if lb["retail_only"] else "")
                 + (f"; ours-only: {fmt_list(lb['ours_only'])}" if lb["ours_only"] else ""))
        L.append("")
    L += ["## Static libraries", "",
          "Archive members pulled by at least one retail module, against our stlib sources. Archives only "
          "show pulled members, so *never pulled* is weak evidence that a file isn't in Valve's library.", ""]
    for arc, v in libs.items():
        L.append(f"- `{arc}` (ours: `{v['our_target']}`): {len(v['retail_members'])} members pulled"
                 + (f"; not compiled by ours: {fmt_list(v['retail_members_not_in_ours'])}" if v["retail_members_not_in_ours"] else "")
                 + (f"; ours never pulled: {fmt_list((os.path.basename(x) for x in v['ours_never_pulled_by_retail']), 30)}" if v["ours_never_pulled_by_retail"] else ""))
    L.append("")
    open(path, "w").write("\n".join(L))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--sdk", default=SDK)
    a = ap.parse_args()
    ours = Tree(ROOT, skip=(".git", "devtools"))
    sdk = Tree(a.sdk, skip=(".git",))
    targets = our_targets()
    docs = []
    for fn in sorted(os.listdir(os.path.join(OUT, "modules"))):
        docs.append(json.load(open(os.path.join(OUT, "modules", fn))))
    order = [m for m in waf_order(docs)]
    os.makedirs(os.path.join(OUT, "mapping"), exist_ok=True)
    results = []
    for d in order:
        r = map_module(d, ours, sdk, targets)
        results.append(r)
        with open(os.path.join(OUT, "mapping", d["module"].replace("/", ".") + ".json"), "w") as f:
            json.dump(OrderedDict((k, v) for k, v in r.items() if k not in ("objects", "waf_manifest")), f, indent=1)
            f.write("\n")
    libs = static_libs(docs, ours, targets)
    with open(os.path.join(OUT, "mapping", "static_libs.json"), "w") as f:
        json.dump(libs, f, indent=1)
        f.write("\n")
    write_diff_md(results, libs, os.path.join(OUT, "DIFF.md"))
    manifest = build_manifest(docs, results, ours, sdk, targets)
    with open(os.path.join(OUT, "retail-osx32.json"), "w") as f:
        json.dump(manifest, f, indent=1)
        f.write("\n")


def waf_order(docs):
    rank = {k: i for i, k in enumerate(MODULE_TARGETS)}
    return sorted(docs, key=lambda d: (d["module"] not in rank, rank.get(d["module"], 0), d["module"]))


if __name__ == "__main__":
    main()
