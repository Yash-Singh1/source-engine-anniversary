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

# Modules built by --retail-win32 by default, with their path in depot 222.
DEFAULT_MODULES = {
    "tier0": "bin/tier0.dll",
    "vstdlib": "bin/vstdlib.dll",
    "scenefilecache": "bin/scenefilecache.dll",
    "soundemittersystem": "bin/soundemittersystem.dll",
    "datacache": "bin/datacache.dll",
    "studiorender": "bin/studiorender.dll",
    "inputsystem": "bin/inputsystem.dll",
    "engine": "bin/engine.dll",
}

# Static-library projects whose outputs other projects name in $Lib.
LIB_PROJECTS = ["jpeglib", "tier1", "tier2", "tier3", "mathlib", "bitmap", "vtf", "appframework", "dmxloader",
                "matsys_controls", "vgui_controls", "bzip2", "replay_common", "choreoobjects",
                "particles", "vpklib", "shaderlib", "socketlib"]

# Valve include directories that live elsewhere in this tree.
INCLUDE_ALIASES = {"thirdparty/SDL2": "thirdparty/SDL"}

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
    def __init__(self, name, vpc_rel, hl2src, retail=None):
        self.name, self.vpc_rel, self.hl2src, self.retail = name, vpc_rel, hl2src, retail
        self.p = VPC.Project(hl2src, os.path.join(hl2src, vpc_rel)).parse()
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
        return dedupe([s for s in sw if s not in DROP_SWITCHES])

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
        return dedupe(d)

    def includes(self, f=None):
        out = []
        for i in split_list(self.get("compiler.additionalincludedirectories", f), ";"):
            r = self.tree_rel(i)
            if r is not None:
                out.append(INCLUDE_ALIASES.get(r, r))
        return dedupe(out)

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

    def sources(self):
        out = []
        for f in self.p.files:
            ext = os.path.splitext(f["path"])[1].lower()
            lang = {".cpp": "c++", ".cxx": "c++", ".cc": "c++", ".c": "c", ".asm": "asm", ".rc": "rc",
                    ".man": "mc"}.get(ext)
            if lang is None or self.p.excluded(f):
                continue
            rel = self.p.rel(f["path"])
            ent = {"path": rel, "lang": lang}
            ours = self.locate(rel)
            if ours is None:
                ent["missing"] = True
            elif ours != rel:
                ent["path"], ent["valve_path"] = ours, rel
            if lang in ("c++", "c"):
                base = self.cl_switches()
                mine = self.cl_switches(f)
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
            elif rel.startswith("lib/public/") and stem.lower() in DEFAULT_MODULES:
                out.append({"project": stem.lower(), "path": rel, "implib": True})
            else:
                out.append({"prebuilt": rel})
        return out

    def manifest(self):
        lsw, lobjs = self.link_switches()
        pchhdr = self.get("compiler.create/usepchthroughfile")
        m = {
            "vpc": self.vpc_rel.replace("\\", "/"),
            "type": self.ctype(),
            "output": self.p.macros.get("OUTBINNAME") or self.p.macros.get("OUTLIBNAME") or self.name,
            "retail": self.retail,
            "project_dir": os.path.relpath(self.projdir, self.hl2src).replace(os.sep, "/"),
            "defines": self.defines(),
            "includes": self.includes(),
            "cl": self.cl_switches(),
            "pch_header": pchhdr if self.pch() or any(self.pch(f) for f in self.p.files) else None,
            "sources": self.sources(),
            "libs": self.libs(),
        }
        if m["type"] != "lib":
            m["link"] = lsw
            m["link_objects"] = lobjs
            m["system_libs"] = split_list(self.get("linker.additionaldependencies"), " ")
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
        vpc_rel = projects.get(name.lower())
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
