#!/usr/bin/env python3
"""Generate devtools/bytematch/manifests/retail-win32.json from Valve's VPC scripts.

Evaluates each module's .vpc from the 2018 hl2_src tree (Win32, VS2013,
Retail; see vpc.py) and records what MSBuild would have passed to cl, link and
lib: defines, include directories, switches, precompiled-header use, per-file
overrides, sources in project order, and link libraries. $Lib entries that
name a library project are followed, so the static libraries a module links
come along. Paths are mapped onto this tree; files Valve compiled that we
don't have are listed as missing.

usage: make-manifest.py [--hl2src DIR] [--out FILE] [MODULE ...]
"""
import argparse, json, ntpath, os, re, sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import vpc as VPC

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", "..", ".."))
HL2SRC = os.environ.get("HL2SRC", os.path.expanduser("~/p/sco/source-engine-2018-hl2_src"))

# Modules built by --retail-win32 by default, with their path in depot 222
# ("DEPOT:path" for another depot).
DEFAULT_MODULES = {
    "tier0": "bin/tier0.dll",
    "vstdlib": "bin/vstdlib.dll",
    "scenefilecache": "bin/scenefilecache.dll",
    "soundemittersystem": "bin/soundemittersystem.dll",
    "datacache": "bin/datacache.dll",
    "studiorender": "bin/StudioRender.dll",
    "inputsystem": "bin/inputsystem.dll",
    "engine": "bin/engine.dll",
    "filesystem_stdio": "bin/FileSystem_Stdio.dll",
    "materialsystem": "bin/MaterialSystem.dll",
    "shaderapidx9": "bin/shaderapidx9.dll",
    "stdshader_dx9": "bin/stdshader_dx9.dll",
    "vgui2": "bin/vgui2.dll",
    "vguimatsurface": "bin/vguimatsurface.dll",
    "GameUI": "bin/GameUI.dll",
    "ServerBrowser": "bin/ServerBrowser.dll",
    "vphysics": "bin/vphysics.dll",
    "replay": "bin/replay.dll",
    "launcher": "bin/launcher.dll",
    "launcher_main": "hl2.exe",
    "client_hl2": "hl2/bin/client.dll",
    "server_hl2": "hl2/bin/server.dll",
    "client_episodic": "380:episodic/bin/client.dll",
    "server_episodic": "380:episodic/bin/server.dll",
}

# Modules whose VPC isn't the one projects.vgc names for them (game projects pick
# theirs by game conditional; vgui2's project is called vgui_dll).
VPC_FILES = {
    "vgui2": "vgui2/src/vgui_dll.vpc",
    # retail hl2 is the episodic build (same objects and code; --retail-osx32 builds it so too)
    "client_hl2": "game/client/client_episodic.vpc",
    "server_hl2": "game/server/server_episodic.vpc",
    "client_episodic": "game/client/client_episodic.vpc",
    "server_episodic": "game/server/server_episodic.vpc",
}

# Library projects hl2_src lacks; their VPCs are in this tree (the ivp submodule)
# and include Valve's vpc_scripts from hl2_src.
TREE_VPC_FILES = {
    "ivp_physics": "ivp/ivp_physics/ivp_physics.vpc",
    "ivp_compactbuilder": "ivp/ivp_compact_builder/ivp_compactbuilder.vpc",
    "havana_constraints": "ivp/havana/havana_constraints.vpc",
    "hk_base": "ivp/havana/havok/hk_base/hk_base.vpc",
    "hk_math": "ivp/havana/havok/hk_math/hk_math.vpc",
}

# Files the 2018 VPCs list that the retail build doesn't have: no texture
# compositor in retail MaterialSystem.dll or stdshader_dx9.dll (no strings, no
# macOS object), and this tree drops it from the BYTEMATCH_RETAIL interfaces.
# Retail tier0.dll has no stack tools or CPU frequency monitor either (none of
# their exports or strings, no macOS object). The retail engine measures MP3s with
# minimp3; the CMPAFile parser's MPAHeader/VBRHeader have no macOS object.
RETAIL_EXCLUDE = {"materialsystem": ["materialsystem/ctexturecompositor.cpp"],
                  "stdshader_dx9": ["materialsystem/stdshaders/compositor.cpp"],
                  "tier0": ["tier0/stacktools.cpp", "tier0/cpumonitoring.cpp"],
                  "engine": ["engine/audio/private/MPAHeader.cpp", "engine/audio/private/VBRHeader.cpp"]}

# VPC conditionals Valve's buildbot set beyond the platform ones (no 2018 script sets them):
# retail engine.dll has the remote perf test commands (rpt_start, CRPTClient, ...), which
# engine.vpc compiles with $ENABLE_RPT. The --retail-osx32 engine defines it too.
RETAIL_CONDITIONALS = {"engine": ("ENABLE_RPT",)}

# Per-file switches the 2018 scripts set that the November 2024 build no longer used:
# engine.vpc compiles sys_dll2.cpp with /EHa, but no retail function of that file sets up an
# EH frame (CEngineAPI::RunListenServer's CModAppSystemGroup local, ModInit's new, ...).
RETAIL_FILE_CL_DROP = {"engine/sys_dll2.cpp": ("/EHa",)}

# Include directories this fork's sources need beyond Valve's (the fork's
# vector_fpu.inl includes ivu_types.hxx for intp).
FORK_INCLUDES = {"hk_math": ["ivp/ivp_utility"], "replay": ["thirdparty/curl/include"],
                 "vgui_surfacelib": ["thirdparty/freetype/include", "thirdparty/fontconfig", "thirdparty/SDL"],
                 "vguimatsurface": ["thirdparty/freetype/include", "thirdparty/fontconfig"]}

# Searched before the VPC's include directories: Valve's 2024 shader combo headers
# (SDK 2013 TF2 branch include/), which the 2018 fxctmp9/vshtmp9 predate; the
# --retail-osx32 build puts the same directory first.
RETAIL_INCLUDES_FIRST = {"stdshader_dx9": ["materialsystem/stdshaders/include"]}

# Switches and defines this fork's sources need under MSVC 2013: the retail-mode
# inertia clamp in ivp_core.cxx calls POSIX finite(); qhull calls sqrt( int ),
# which needs <cmath>'s integral overloads (Valve linked prebuilt ivp libraries).
FORK_CL = {"ivp_compactbuilder": ["/FIcmath"]}

# Switches of this tree's VPCs that Valve's retail build didn't use: ivp_physics.vpc asks
# for /Ob1, but retail vphysics.dll inlines ivp_physics' plain member functions (ivp_physics
# 557 to 674 of 811 functions with /Ob2; ivp_compactbuilder does match /Ob1: 362 vs 306).
RETAIL_CL_REPLACE = {"ivp_physics": {"/Ob1": "/Ob2"}}

# Valve's Windows ivp was built with IVP_PIII (Pentium III paths in ivu_linear and
# _mm_prefetch hints: retail vphysics.dll has 66 prefetcht1); the ivp submodule's VPCs lack it.
_IVP_PIII = ["IVP_PIII"]
FORK_DEFINES = {"ivp_physics": ["finite=_finite"] + _IVP_PIII, "ivp_compactbuilder": _IVP_PIII,
                "havana_constraints": _IVP_PIII, "hk_base": _IVP_PIII, "hk_math": _IVP_PIII,
                "vphysics": _IVP_PIII,
                # this tree gates its JPEG/PNG code on these (the --retail-osx32 build defines them
                # for every module that links libjpeg/libpng; retail engine.dll and client.dll have both)
                "engine": ["HAVE_JPEG", "HAVE_PNG"], "client_hl2": ["HAVE_JPEG", "HAVE_PNG"],
                "client_episodic": ["HAVE_JPEG", "HAVE_PNG"], "GameUI": ["HAVE_JPEG", "HAVE_PNG"],
                "vgui_surfacelib": ["HAVE_FC=1"], "vguimatsurface": ["HAVE_FC=1"]}

# Static-library projects whose outputs other projects name in $Lib.
LIB_PROJECTS = ["jpeglib", "tier1", "tier2", "tier3", "mathlib", "bitmap", "vtf", "appframework", "dmxloader",
                "matsys_controls", "vgui_controls", "bzip2", "replay_common", "choreoobjects",
                "particles", "vpklib", "shaderlib", "socketlib", "vgui_surfacelib", "lzma"] + list(TREE_VPC_FILES)

# Valve include directories that live elsewhere in this tree.
INCLUDE_ALIASES = {"thirdparty/SDL2": "thirdparty/SDL", "engine/audio/private": "engine/audio",
                   "engine/audio/public": "public/engine/audio"}

# This fork's sources a module needs because other files call into them unconditionally.
FORK_SOURCES = {"engine": ["engine/masterserver.cpp"]}

# Valve sources retail has that the 2018 VPCs lack (retail macOS objects), placed in
# generator order with the settings of the file before them.
RETAIL_SOURCES = {"GameUI": ["gameui/OptionsSubGamepad.cpp"],
                  # retail replay.dll and engine.dll have their strings too
                  "replay": ["replay/sv_ftp.cpp"], "engine": ["engine/workshop.cpp", "engine/mm_events.cpp", "common/imageutils.cpp"],
                  "client_hl2": ["common/imageutils.cpp", "game/client/steamshare.cpp"],
                  "client_episodic": ["common/imageutils.cpp", "game/client/steamshare.cpp"],
                  "vgui_surfacelib": ["vgui2/vgui_surfacelib/linuxfont.cpp"]}

# Libraries retail links beyond the 2018 VPCs: retail macOS stdshader_dx9 has
# libtier2.a(tier2.o), which defines the g_pMaterialSystem CShaderDLL::Connect sets.
# Retail client.dll and engine.dll also link libjpeg and libpng (imageutils); client and server import rpcrt4 (tier1 uniqueid).
_CLIENT_IMAGE_LIBS = [{"project": "jpeglib", "path": "lib/common/libjpeg.lib"}, {"prebuilt": "lib/public/libpng.lib"}]
_FREETYPE_LIBS = [{"prebuilt": "lib/public/libfreetype-6.lib"}, {"prebuilt": "lib/public/libfontconfig-1.lib"}, {"prebuilt": "lib/public/SDL2.lib"}]
RETAIL_LIBS = {"stdshader_dx9": [{"project": "tier2", "path": "lib/public/tier2.lib"}],
               "client_hl2": _CLIENT_IMAGE_LIBS, "client_episodic": _CLIENT_IMAGE_LIBS,
               "engine": [{"prebuilt": "lib/public/libpng.lib"}],
               "vguimatsurface": _FREETYPE_LIBS}
RETAIL_SYSTEM_LIBS = dict((m, ["rpcrt4.lib"]) for m in ("client_hl2", "client_episodic", "server_hl2", "server_episodic"))
RETAIL_SYSTEM_LIBS["GameUI"] = ["nvtc.lib"]

# Valve library paths ($File "...lib") that live elsewhere in this tree.
LIB_FILE_ALIASES = {"dx9sdk/lib": "dx9sdk/lib/x86"}

# Library file stem -> project that builds it. Retail links an older prebuilt
# libjpeg (VS2010 SP1, see thirdparty-static.md); Valve's own project is the stand-in.
LIB_ALIASES = {"libjpeg": "jpeglib"}

# Switches that only affect diagnostics, the IDE or build bookkeeping.
DROP_SWITCHES = {"/WX", "/errorReport:prompt", "/ERRORREPORT:PROMPT", "/MP", "/Gm", "/FR", "/Fr",
                 "/Wp64", "/NOLOGO", "/nologo"}

CL_SCALAR = ["optimization", "inlinefunctionexpansion", "enableintrinsicfunctions", "favorsizeorspeed",
             "omitframepointers", "enablefibersafeoptimizations", "wholeprogramoptimization",
             "enablestringpooling", "enableminimalrebuild", "smallertypecheck", "basicruntimechecks",
             "runtimelibrary", "structmemberalignment", "enablefunctionlevellinking",
             "enableenhancedinstructionset", "floatingpointmodel", "enablefloatingpointexceptions",
             "disablelanguageextensions", "defaultcharunsigned", "forceconformanceinforloopscope",
             "enableruntimetypeinfo", "openmpsupport", "callingconvention", "compileas",
             "usefullpaths", "omitdefaultlibrarynames", "debuginformationformat", "warninglevel",
             "treatwarningsaserrors", "treatwchar_tasbuiltintype"]

# MSBuild's ClCompile defaults for properties a VPC leaves empty (codegen-relevant ones).
CL_DEFAULTS = {
    "buffersecuritycheck": "/GS", "enablec++exceptions": "/EHsc", "treatwchar_tasbuiltintype": "/Zc:wchar_t",
    "forceconformanceinforloopscope": "/Zc:forScope", "callingconvention": "/Gd",
    "floatingpointmodel": "/fp:precise", "runtimelibrary": "/MD",
}


def switch(value):
    """'Maximize Speed (/O2)' -> '/O2'."""
    if not value:
        return None
    m = re.search(r"\((/[^)]+)\)\s*$", value.strip())
    return m.group(1).strip() if m else None


def yes(value):
    return (value or "").strip().lower() in ("yes", "true")


def no(value):
    return (value or "").strip().lower() in ("no", "false")


def split_list(value, sep):
    if not value:
        return []
    parts = value.split(sep) if sep == ";" else value.split()
    return [p.strip() for p in parts if p.strip() and p.strip() not in ("%(PreprocessorDefinitions)",
            "%(AdditionalIncludeDirectories)", "%(AdditionalDependencies)", "%(DisableSpecificWarnings)")]


def dedupe(seq):
    out = []
    for x in seq:
        if x not in out:
            out.append(x)
    return out


class Module:
    def __init__(self, name, vpc_rel, hl2src, retail=None, include_alias=None):
        self.name, self.vpc_rel, self.hl2src, self.retail = name, vpc_rel, hl2src, retail
        self.p = VPC.Project(hl2src, os.path.join(hl2src, vpc_rel), VPC.WIN32_RETAIL + RETAIL_CONDITIONALS.get(name, ()),
                             include_alias=include_alias).parse()
        self.projdir = os.path.dirname(os.path.join(hl2src, vpc_rel))

    def tree_rel(self, win, base=None):
        """Valve path (relative to the project dir) -> repo-relative path."""
        p = win.replace("\\", "/")
        if "$(" in p:
            return None
        full = os.path.normpath(os.path.join(base or self.projdir, p))
        return os.path.relpath(full, self.hl2src).replace(os.sep, "/")

    def get(self, key, f=None):
        b, k = key.split(".", 1)
        return self.p.get(b, k, f)

    def cl_switches(self, f=None):
        sw = []
        for key in CL_SCALAR:
            v = self.get("compiler." + key, f)
            s = switch(v)
            if s:
                sw.append(s)
            elif key in CL_DEFAULTS and not v:
                sw.append(CL_DEFAULTS[key])
        gs = self.get("compiler.buffersecuritycheck", f)
        sw.append("/GS-" if no(gs) else "/GS")
        eh = self.get("compiler.enablec++exceptions", f)
        if switch(eh):
            sw.append(switch(eh))
        elif not eh:
            sw.append("/EHsc")
        for w in split_list(self.get("compiler.disablespecificwarnings", f), ";"):
            sw.append("/wd" + w)
        for fi in split_list(self.get("compiler.forceincludes", f), ";"):
            sw.append("/FI" + fi)
        sw += (self.get("compiler.additionaloptions", f) or "").split()
        rep = RETAIL_CL_REPLACE.get(self.name, {})
        return dedupe([rep.get(s, s) for s in sw if s not in DROP_SWITCHES] + FORK_CL.get(self.name, []))

    def defines(self, f=None):
        d = split_list(self.get("compiler.preprocessordefinitions", f), ";")
        if "dll" in (self.get("general.configurationtype") or "").lower():
            d.append("_WINDLL")
        if (self.get("general.platformtoolset") or "").endswith("_xp"):
            d.append("_USING_V110_SDK71_")  # added by the v1x0_xp toolset props
        cs = (self.get("general.characterset") or "").lower()
        if "multi-byte" in cs:
            d.append("_MBCS")
        elif "unicode" in cs:
            d += ["_UNICODE", "UNICODE"]
        return dedupe(d + FORK_DEFINES.get(self.name, []))

    def includes(self, f=None):
        out = []
        for i in split_list(self.get("compiler.additionalincludedirectories", f), ";"):
            r = self.tree_rel(i)
            if r is not None:
                out.append(INCLUDE_ALIASES.get(r, r))
        return dedupe(RETAIL_INCLUDES_FIRST.get(self.name, []) + out + FORK_INCLUDES.get(self.name, []))

    def pch(self, f=None):
        mode = (self.get("compiler.create/useprecompiledheader", f) or "").lower()
        if "/yc" in mode:
            return "create"
        if "/yu" in mode:
            return "use"
        return None

    def ctype(self):
        t = (self.get("general.configurationtype") or "").lower()
        return "dll" if "dll" in t else "lib" if "static" in t else "exe" if "exe" in t or "application" in t else t

    def link_switches(self):
        sw = []
        for key in ("enableincrementallinking", "generatedebuginfo", "subsystem", "references",
                    "enablecomdatfolding", "targetmachine", "enablelargeaddresses", "fixedbaseaddress",
                    "setchecksum", "linktimecodegeneration", "terminalserver"):
            s = switch(self.get("linker." + key))
            if s:
                sw.append(s)
        sub = [i for i, s in enumerate(sw) if s.upper().startswith("/SUBSYSTEM:") and "," not in s]
        for i in sub:
            sw[i] += ",5.01"  # v120_xp toolset: MinimumRequiredVersion 5.01
        if no(self.get("linker.generatemanifest")):
            sw.append("/MANIFEST:NO")
        if yes(self.get("linker.randomizedbaseaddress")):
            sw.append("/DYNAMICBASE")
        elif no(self.get("linker.randomizedbaseaddress")):
            sw.append("/DYNAMICBASE:NO")
        sw.append("/NXCOMPAT")
        if no(self.get("linker.imagehassafeexceptionhandlers")):
            sw.append("/SAFESEH:NO")
        base = (self.get("linker.baseaddress") or "").strip()
        if base:
            sw.append("/BASE:" + base)
        for lib in split_list(self.get("linker.ignorespecificlibrary"), ";"):
            sw.append("/NODEFAULTLIB:" + lib)
        objs, extra = [], []
        for opt in (self.get("linker.additionaloptions") or "").split():
            if opt.lower().endswith(".obj"):
                objs.append(self.tree_rel(opt))
            elif opt not in DROP_SWITCHES:
                extra.append(opt)
        return dedupe(sw + extra), objs

    def generator_order(self):
        """Files in the order VPC's vcxproj generator writes them, which is MSBuild's
        compile and link order: per folder, files sorted case-insensitively by file
        name alone (ties keep insertion order), then subfolders sorted by name
        (external/vpc/utils/vpc/projectgenerator_vcproj.cpp, CProjectFolder::AddFile)."""
        root = {"files": [], "folders": {}}
        for f in self.p.files:
            node = root
            for part in [p for p in f["folder"].split("/") if p]:
                node = node["folders"].setdefault(part, {"files": [], "folders": {}})
            node["files"].append(f)

        def walk(node):
            out = sorted(node["files"], key=lambda f: ntpath.basename(f["path"].replace("/", "\\")).lower())
            for name in sorted(node["folders"], key=str.lower):
                out += walk(node["folders"][name])
            return out
        return walk(root)

    def sources(self):
        out = []
        for f in self.generator_order():
            ext = os.path.splitext(f["path"])[1].lower()
            lang = {".cpp": "c++", ".cxx": "c++", ".cc": "c++", ".c": "c", ".asm": "asm", ".rc": "rc",
                    ".man": "mc"}.get(ext)
            if lang is None or self.p.excluded(f):
                continue
            rel = self.p.rel(f["path"])
            if rel in RETAIL_EXCLUDE.get(self.name, []):
                continue
            ent = {"path": rel, "lang": lang}
            ours = self.locate(rel)
            if ours is None:
                ent["missing"] = True
            elif ours != rel:
                ent["path"], ent["valve_path"] = ours, rel
            if lang in ("c++", "c"):
                base = self.cl_switches()
                mine = [c for c in self.cl_switches(f) if c not in RETAIL_FILE_CL_DROP.get(rel, ())]
                if mine != base:
                    ent["cl"] = mine
                if self.defines(f) != self.defines():
                    ent["defines"] = self.defines(f)
                if self.includes(f) != self.includes():
                    ent["includes"] = self.includes(f)
                pch = self.pch(f)
                if pch:
                    ent["pch"] = pch
                    hdr = self.get("compiler.create/usepchthroughfile", f)
                    if hdr and hdr != self.get("compiler.create/usepchthroughfile"):
                        ent["pch_header"] = hdr
            elif f["settings"].get(("custombuildstep", "commandline")):
                ent["custom"] = f["settings"][("custombuildstep", "commandline")]
            out.append(ent)
        for path in RETAIL_SOURCES.get(self.name, []):
            d, n = os.path.split(path)
            prev = [(os.path.basename(e["path"]).lower(), k) for k, e in enumerate(out)
                    if os.path.dirname(e["path"]) == d and os.path.basename(e["path"]).lower() < n.lower()]
            i = max(prev)[1] if prev else len(out) - 1
            ent = dict((k, v) for k, v in out[i].items() if k not in ("path", "valve_path", "missing"))
            if ent.get("pch") == "create":
                ent["pch"] = "use"
            ent["path"] = path
            out.insert(i + 1, ent)
        for path in FORK_SOURCES.get(self.name, []):
            out.append({"path": path, "lang": "c++", "fork_only": True})
        return out

    def locate(self, rel):
        """Our path for Valve's REL: same path, same path ignoring case, or the
        only file of that name under the project directory (our tree flattened
        some directories, e.g. engine/audio/private)."""
        full = os.path.join(REPO, rel)
        if os.path.exists(full):
            return rel
        hit = VPC.case_lookup(full)
        if hit:
            return os.path.relpath(hit, REPO)
        name = os.path.basename(rel).lower()
        root = os.path.join(REPO, os.path.relpath(self.projdir, self.hl2src))
        found = [os.path.join(d, f) for d, _, fs in os.walk(root) for f in fs if f.lower() == name]
        return os.path.relpath(found[0], REPO) if len(found) == 1 else None

    def file_libs(self):
        """.lib files listed as $File: MSBuild passes them to link as library inputs."""
        out = []
        for f in self.generator_order():
            if not f["path"].lower().endswith(".lib") or self.p.excluded(f):
                continue
            rel = self.p.rel(f["path"])
            d, base = os.path.split(rel)
            d = next((v for k, v in LIB_FILE_ALIASES.items() if d.lower() == k.lower()), d)
            ours = self.locate(os.path.join(d, base))
            out.append({"path": ours or rel, "valve_path": rel} if ours else {"path": rel, "missing": True})
        return out

    def libs(self):
        out = []
        for l in self.p.libs:
            if l.get("name"):
                out.append({"external": l["name"]})
                continue
            rel = self.p.rel(l["path"])
            stem = os.path.splitext(os.path.basename(rel))[0]
            stem = LIB_ALIASES.get(stem.lower(), stem)
            hit = next((p for p in LIB_PROJECTS if p.lower() == stem.lower()), None)
            if rel.startswith("lib/") and hit:
                out.append({"project": hit, "path": rel})
            elif rel.startswith("lib/public/") and stem.lower() in (n.lower() for n in DEFAULT_MODULES):
                out.append({"project": stem.lower(), "path": rel, "implib": True})
            else:
                out.append({"prebuilt": rel})
        return out + [l for l in RETAIL_LIBS.get(self.name, []) if l not in out]

    def manifest(self):
        lsw, lobjs = self.link_switches()
        pchhdr = self.get("compiler.create/usepchthroughfile")
        depot, _, retail = (self.retail or "").rpartition(":")
        m = {
            "vpc": self.vpc_rel.replace("\\", "/"),
            "type": self.ctype(),
            "output": self.p.macros.get("OUTBINNAME") or self.p.macros.get("OUTLIBNAME") or self.name,
            "retail": retail or None,
            "project_dir": os.path.relpath(self.projdir, self.hl2src).replace(os.sep, "/"),
            "defines": self.defines(),
            "includes": self.includes(),
            "cl": self.cl_switches(),
            "pch_header": pchhdr if self.pch() or any(self.pch(f) for f in self.p.files) else None,
            "sources": self.sources(),
            "libs": self.libs(),
            "file_libs": self.file_libs(),
        }
        if depot:
            m["retail_depot"] = depot
        if m["type"] != "lib":
            m["link"] = lsw
            m["link_objects"] = lobjs
            m["system_libs"] = []
            for lib in split_list(self.get("linker.additionaldependencies"), " "):
                if "\\" not in lib and "/" not in lib:
                    m["system_libs"].append(lib)
                    continue
                # a path relative to the project (launcher: ..\dx9sdk\lib\dinput8.lib)
                rel = os.path.normpath(os.path.join(os.path.relpath(self.projdir, self.hl2src), lib.replace("\\", "/")))
                d, base = os.path.split(rel)
                d = next((v for k, v in LIB_FILE_ALIASES.items() if d.lower() == k.lower()), d)
                ours = self.locate(os.path.join(d, base))
                m["file_libs"].append({"path": ours, "valve_path": rel} if ours else {"path": rel, "missing": True})
            m["system_libs"] += [l for l in RETAIL_SYSTEM_LIBS.get(self.name, []) if l not in m["system_libs"]]
        return m


def vgc_projects(hl2src):
    text = open(os.path.join(hl2src, "vpc_scripts", "projects.vgc"), encoding="utf-8-sig", errors="replace").read()
    return dict((m.group(1).lower(), m.group(2).replace("\\", "/"))
                for m in re.finditer(r'\$Project\s+"([^"]+)"\s*\{\s*"([^"]+)"', text))


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("modules", nargs="*")
    ap.add_argument("--hl2src", default=HL2SRC)
    ap.add_argument("--out", default=os.path.join(REPO, "devtools", "bytematch", "manifests", "retail-win32.json"))
    a = ap.parse_args()
    projects = vgc_projects(a.hl2src)
    want = a.modules or list(DEFAULT_MODULES)
    mods, todo, order = {}, list(want), []
    while todo:
        name = todo.pop(0)
        if name in mods:
            continue
        if name in TREE_VPC_FILES:
            mod = Module(name, TREE_VPC_FILES[name], REPO, None,
                         {os.path.join(REPO, "vpc_scripts"): os.path.join(a.hl2src, "vpc_scripts")}).manifest()
        else:
            vpc_rel = VPC_FILES.get(name) or projects.get(name.lower())
            if not vpc_rel or not os.path.exists(os.path.join(a.hl2src, vpc_rel)):
                print("warning: no VPC for %s" % name, file=sys.stderr)
                mods[name] = None
                continue
            mod = Module(name, vpc_rel, a.hl2src, DEFAULT_MODULES.get(name)).manifest()
        mods[name] = mod
        order.append(name)
        for l in mod["libs"]:
            if l.get("project") and not l.get("implib"):
                todo.append(l["project"])
    out = {
        "schema": 1,
        "generated_by": "devtools/bytematch/toolchain/win32/make-manifest.py",
        "source": "Valve VPC scripts, 2018 hl2_src tree (conditionals %s, Release)" % ",".join(VPC.WIN32_RETAIL),
        "modules": dict((n, mods[n]) for n in order),
        "missing_projects": sorted(n for n, m in mods.items() if m is None),
    }
    with open(a.out, "w") as f:
        json.dump(out, f, indent=1)
        f.write("\n")
    for n in order:
        m = mods[n]
        miss = sum(1 for s in m["sources"] if s.get("missing"))
        print("%-20s %-4s %3d sources (%d missing here) libs: %s" % (
            n, m["type"], len(m["sources"]), miss,
            " ".join(l.get("project") or l.get("external") or os.path.basename(l.get("prebuilt", "")) for l in m["libs"])))
    if out["missing_projects"]:
        print("no VPC in hl2_src for: %s" % " ".join(out["missing_projects"]))


if __name__ == "__main__":
    main()
