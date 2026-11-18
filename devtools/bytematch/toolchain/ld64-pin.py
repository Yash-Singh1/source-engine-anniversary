#!/usr/bin/env python3
"""Pin the ld64 version: link one module with every installed ld64 and compare
the output's linker-controlled structure with the retail module.

Compiles the module's loose objects and static-library members (from the
retail manifest, in retail link order) with the retail flags, archives the
members like Xcode (libtool -static), links with ld64 236.3 / 241.9 / 242
through the clang-600 driver against the retail tier0/vstdlib, and reports
load commands, LINKEDIT blob order and padding, and dyld opcode streams.

Usage: ld64-pin.py [--module bin/scenefilecache] [--tree DIR] [--work DIR]
"""
import argparse
import hashlib
import json
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import macho  # noqa: E402
import retailflags as rf  # noqa: E402

TREE = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
LIB_DEFINES = {"tier1": ["TIER1_STATIC_LIB"], "mathlib": ["MATHLIB_LIB"]}
LIB_INCLUDES = {"tier1": ["common", "public", "public/tier0", "public/tier1", "tier1"],
                "mathlib": ["common", "public", "public/tier0", "public/mathlib", "mathlib"]}
SDK2013 = "/Volumes/Storage/sco/source-sdk-2013/src"
# Archive members whose source in our tree pulls in symbols retail's member does
# not reference; the experiment substitutes the SDK 2013 file retail was built from
# so the linker sees a retail-shaped input set.
OVERRIDES = {"libtier1.a(lzmaDecoder.o)": SDK2013}


def run(cmd, **kw):
    r = subprocess.run(cmd, capture_output=True, text=True, **kw)
    if r.returncode:
        sys.stderr.write(" ".join(cmd) + "\n" + r.stderr)
        raise SystemExit(1)
    return r.stdout


def compile_one(p, src, out, defines, includes):
    lang = rf.cxxflags(p["sdk"]) if src.endswith((".cpp", ".cc")) else rf.cflags(p["sdk"])
    cmd = [p["cxx"] if src.endswith((".cpp", ".cc")) else p["cc"]] + lang
    cmd += ["-I" + i for i in includes] + ["-D" + d for d in defines] + rf.UNDEFS
    cmd += ["-c", src, "-o", out]
    run(cmd)


def opcodes(dyldinfo, path):
    out = run([dyldinfo, "-opcodes", path])
    # keep opcode names only, so modules with different code still compare by encoder style
    ops = []
    for line in out.splitlines():
        for tok in line.split():
            if tok.startswith(("BIND_OPCODE_", "REBASE_OPCODE_")):
                ops.append(tok.split("(")[0])
    return ops


def describe(path, dyldinfo):
    m = macho.MachO(path)
    lay = m.linkedit_layout()
    gaps = []
    for (n1, o1, s1), (n2, o2, _) in zip(lay, lay[1:]):
        if o2 != o1 + s1:
            gaps.append(f"{n1}->{n2}:{o2 - (o1 + s1)}")
    return {
        "load_commands": m.load_command_names(),
        "sizeofcmds": sum(s for _, _, s in m.commands),
        "linkedit_order": [n for n, _, _ in lay],
        "linkedit_gaps": gaps,
        "linkedit_align": {n: o % 8 for n, o, _ in lay},
        "opcode_kinds": sorted(set(opcodes(dyldinfo, path))),
        "sections": [f"{s}.{c}" for s, c in m.sections],
        "section_align": [m.section_align(i) for i in range(len(m.sections))],
        "flags": hex(m.flags),
    }


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--module", default="bin/scenefilecache")
    ap.add_argument("--tree", default=TREE)
    ap.add_argument("--work", default="/tmp/bytematch-ld64-pin")
    ap.add_argument("--ld64", nargs="*", default=["236.3", "241.9", "242"])
    a = ap.parse_args()

    man = json.load(open(os.path.join(a.tree, "devtools/bytematch/manifests/retail-osx32.json")))
    mod = man["modules"][a.module]
    p = rf.paths()
    os.makedirs(a.work, exist_ok=True)
    dyldinfo = os.path.join(rf.paths()["ld_dir"], "dyldinfo")

    mdefs = [d for d in mod["defines"]["xcconfig"] + mod["defines"]["vpc"]
             if d.split("=")[0] not in rf.UNDEFINED_NAMES]
    proj = os.path.join(a.tree, mod["project_dir"])
    minc = [os.path.normpath(os.path.join(proj, i)) for i in mod["defines"]["vpc_include_dirs"]]

    loose, archives = [], {}
    for o in mod["objects"]:
        if not o["source"]:
            print("missing source for", o.get("member") or o["retail_source"])
            continue
        root = OVERRIDES.get(f"{o['archive']}({o['member']})", a.tree)
        src = os.path.join(root, o["source"])
        name = os.path.splitext(os.path.basename(o["source"]))[0] + ".o"
        if o["archive"]:
            lib = o["archive"][3:-2]
            out = os.path.join(a.work, lib, name)
            os.makedirs(os.path.dirname(out), exist_ok=True)
            compile_one(p, src, out, rf.DEFINES + LIB_DEFINES.get(lib, []),
                        [os.path.join(root, i) for i in LIB_INCLUDES.get(lib, ["public"])])
            archives.setdefault(o["archive"], []).append(out)
        else:
            out = os.path.join(a.work, name)
            compile_one(p, src, out, mdefs, minc)
            loose.append(out)
    libs = []
    for arc in mod["archives"]:
        objs = archives.get(arc["archive"])
        if not objs:
            continue
        path = os.path.join(a.work, arc["archive"])
        if os.path.exists(path):
            os.unlink(path)
        run(["libtool", "-static", "-arch_only", "i386", "-o", path] + objs)
        libs.append(path)

    retail = os.path.join(rf.RETAIL_DEPOT, mod["retail"]["path"])
    dylibs = []
    for d in mod["dylibs"]:
        if d["path"].startswith("@loader_path/"):
            dylibs.append(os.path.join(os.path.dirname(retail), d["path"][len("@loader_path/"):]))
        elif d["path"].startswith("/usr/lib/lib") and "stdc++" not in d["path"] and "System" not in d["path"]:
            dylibs.append("-l" + os.path.basename(d["path"])[3:].split(".")[0])
        elif d["path"].startswith("/System/Library/Frameworks/"):
            dylibs += ["-framework", d["path"].split("/")[4].split(".")[0]]

    results = {"retail": describe(retail, dyldinfo)}
    hashes = {}
    for v in a.ld64:
        pv = rf.paths(ld64=v)
        out = os.path.join(a.work, f"{os.path.basename(a.module)}.ld64-{v}.dylib")
        cmd = [pv["cxx"]] + rf.linkflags(pv) + rf.dylib_flags(mod["retail"]["install_name"])
        cmd += loose + libs + dylibs + ["-o", out]
        run(cmd)
        results[v] = describe(out, dyldinfo)
        hashes[v] = hashlib.sha256(open(out, "rb").read()).hexdigest()[:16]

    print("output sha256:", hashes)
    same = len(set(hashes.values())) == 1
    print("all ld64 versions byte-identical:", same)
    keys = results["retail"].keys()
    for k in keys:
        vals = {n: r[k] for n, r in results.items()}
        flag = "same" if all(v == vals["retail"] for v in vals.values()) else "DIFF"
        print(f"\n== {k} [{flag}]")
        for n, v in vals.items():
            print(f"  {n:7} {v}")
    json.dump({"hashes": hashes, "results": results},
              open(os.path.join(a.work, "ld64-pin.json"), "w"), indent=1)


if __name__ == "__main__":
    main()
