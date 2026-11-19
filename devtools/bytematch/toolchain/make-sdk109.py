#!/usr/bin/env python3
"""Synthesize an i386 MacOSX10.9 stub SDK for the retail toolchain.

Headers and every library the retail modules do not use come from the thinned
10.6 SDK. Every system dylib that retail modules load is replaced by a stub
built with ld64 that carries:
  - the retail LC_LOAD_DYLIB install name, current and compatibility version
  - the union of the 10.6 library's exports (private re-exported sublibraries
    flattened in) and every symbol retail modules bind to that library
  - weak-definition flags from 10.6 or from retail weak-binding entries
  - LC_REEXPORT_DYLIB for re-exports at public locations (ld64 links those
    implicitly, which decides the client's LC_LOAD_DYLIB set)
The directory is named MacOSX10.9.sdk because ld64 derives the LC_VERSION_MIN
sdk field from the -syslibroot path.

Usage: make-sdk109.py [--prefix DIR] [--sdk106 DIR] [--depot DIR ...] [--out DIR]
"""
import argparse
import glob
import json
import os
import shutil
import struct
import subprocess
import sys
import tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import macho  # noqa: E402

DEFAULT_PREFIX = os.environ.get("BYTEMATCH_TOOLCHAIN",
                                os.path.expanduser("~/.cache/source-bytematch/toolchain"))
DEPOT = "/Volumes/Expansion/mac-ports/hl2-anniversary-depot/depots"
THIRD_PARTY = ("libcef", "libSDL2", "libMilesX86", "libsteam_api")

SDK_SETTINGS = """<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>CanonicalName</key>
	<string>macosx10.9</string>
	<key>DefaultProperties</key>
	<dict>
		<key>MACOSX_DEPLOYMENT_TARGET</key>
		<string>10.9</string>
		<key>PLATFORM_NAME</key>
		<string>macosx</string>
	</dict>
	<key>DisplayName</key>
	<string>OS X 10.9 (bytematch stub: 10.6 headers, retail-derived i386 stub libraries)</string>
	<key>MaximumDeploymentTarget</key>
	<string>10.9</string>
	<key>MinimalDisplayName</key>
	<string>10.9</string>
	<key>Version</key>
	<string>10.9</string>
</dict>
</plist>
"""


def find_retail_modules(depots):
    mods = []
    for d in depots:
        for root, _, files in os.walk(d):
            for f in files:
                p = os.path.join(root, f)
                if os.path.islink(p) or f.startswith(THIRD_PARTY):
                    continue
                if not (f.endswith(".dylib") or f.endswith("_osx") or f.endswith("_osx32")):
                    continue
                try:
                    mods.append(macho.MachO(p))
                except ValueError:
                    pass
    return mods


def retail_weak_binds(path, dyldinfo):
    out = subprocess.run([dyldinfo, "-weak_bind", path], capture_output=True, text=True).stdout
    names = set()
    for line in out.splitlines()[2:]:
        parts = line.split()
        if len(parts) >= 6:
            names.add(parts[-1])
    return names


def is_system(name):
    return name.startswith("/usr/lib/") or name.startswith("/System/Library/")


def collect_retail(mods, dyldinfo):
    libs = {}  # install name -> {"current", "compat", "imports": set, "modules": int}
    weak = set()
    for m in mods:
        imports = m.imports()
        wb = retail_weak_binds(m.path, dyldinfo)
        for d in m.dylibs:
            if not is_system(d.name):
                continue
            e = libs.setdefault(d.name, {"current": d.current, "compat": d.compat,
                                         "imports": set(), "modules": 0})
            if (e["current"], e["compat"]) != (d.current, d.compat):
                print(f"warning: {d.name} version mismatch in {m.path}", file=sys.stderr)
            e["modules"] += 1
        for sym, lib in imports.items():
            if lib in libs:
                libs[lib]["imports"].add(sym)
                if sym in wb:
                    weak.add(sym)
    return libs, weak


def dylib_id(path):
    try:
        m = macho.MachO(path)
    except (ValueError, OSError):
        return None
    return m.id.name if m.id else None


def locate(sdk, install_name):
    """(file whose LC_ID_DYLIB is install_name, other regular files with that ID).

    The 10.6 SDK has e.g. libgcc_s.1.dylib -> libSystem.B.dylib while the real
    /usr/lib/libgcc_s.1.dylib stub is libgcc_s.10.5.dylib.
    """
    path = sdk + install_name
    d = os.path.dirname(path)
    same = []
    if os.path.isdir(d):
        for f in sorted(os.listdir(d)):
            p = os.path.join(d, f)
            if os.path.isfile(p) and not os.path.islink(p) and dylib_id(p) == install_name:
                same.append(p)
    aliases = [p for p in same if p != path]
    if os.path.exists(path) and dylib_id(path) == install_name:
        return path, aliases
    if same:
        return same[0], aliases
    return (path if os.path.exists(path) else None), []


def flatten_exports(sdk, install_name, seen=None):
    """Exports of a 10.6 dylib with non-public re-exported children merged in."""
    seen = seen if seen is not None else set()
    if install_name in seen:
        return {}, []
    seen.add(install_name)
    path, _ = locate(sdk, install_name)
    if not path:
        return {}, []
    m = macho.MachO(path)
    exports = dict(m.exports())
    public = []
    for child in m.reexports():
        if macho.is_public_location(child):
            public.append(child)
            continue
        sub, subpub = flatten_exports(sdk, child, seen)
        for k, v in sub.items():
            exports.setdefault(k, v)
        public += subpub
    return exports, public


def asm_name(s):
    return '"' + s.replace("\\", "\\\\").replace('"', '\\"') + '"'


def write_stub_asm(path, exports):
    groups = {("text", False): "__TEXT,__text,regular,pure_instructions",
              ("text", True): "__TEXT,__textcoal_nt,coalesced,pure_instructions",
              ("data", False): "__DATA,__data",
              ("data", True): "__DATA,__datacoal_nt,coalesced"}
    with open(path, "w") as f:
        for (kind, weak), sect in groups.items():
            names = sorted(n for n, (k, w) in exports.items() if k == kind and w == weak)
            if not names:
                continue
            f.write(f".section {sect}\n")
            for n in names:
                q = asm_name(n)
                f.write(f".globl {q}\n")
                if weak:
                    f.write(f".weak_definition {q}\n")
                f.write(f"{q}:\n.byte 0\n")
        for n in sorted(n for n, (k, _) in exports.items() if k == "abs"):
            q = asm_name(n)
            f.write(f".globl {q}\n{q} = 0\n")


def add_local_symbol(path, name, sectname, offset):
    """Insert a local N_SECT symbol `name` at `offset` in section `sectname` of a 32-bit
    MH_OBJECT, in place (atomic rename). No-op if the object already defines it."""
    with open(path, "rb") as f:
        d = bytearray(f.read())
    magic, _, _, filetype, ncmds, _, _ = struct.unpack_from("<7I", d)
    if magic != 0xFEEDFACE or filetype != 1:
        raise ValueError(f"{path}: not a 32-bit MH_OBJECT")
    off, sects, symtab, dysym = 28, [], None, None
    for _ in range(ncmds):
        cmd, size = struct.unpack_from("<2I", d, off)
        if cmd == 0x1:
            for s in range(struct.unpack_from("<I", d, off + 48)[0]):
                so = off + 56 + 68 * s
                sects.append((d[so:so + 16].rstrip(b"\0").decode(), so))
        elif cmd == 0x2:
            symtab = off
        elif cmd == 0xB:
            dysym = off
        off += size
    symoff, nsyms, stroff, strsize = struct.unpack_from("<4I", d, symtab + 8)
    if stroff != symoff + 12 * nsyms or stroff + strsize != len(d):
        raise ValueError(f"{path}: symbol and string tables are not the last two blobs")
    strtab = bytes(d[stroff:])
    for i in range(nsyms):
        strx = struct.unpack_from("<I", d, symoff + 12 * i)[0]
        if strtab[strx:strtab.index(b"\0", strx)] == name.encode():
            return False
    nsect = next(i + 1 for i, (n, _) in enumerate(sects) if n == sectname)
    addr = struct.unpack_from("<I", d, sects[nsect - 1][1] + 32)[0]
    dy = list(struct.unpack_from("<18I", d, dysym + 8)) if dysym else [0] * 18
    at = dy[0] + dy[1]  # end of the locals
    # extern relocations and indirect symbols that index past the insertion point
    for _, so in sects:
        reloff, nreloc = struct.unpack_from("<2I", d, so + 48)
        for r in range(nreloc):
            ro = reloff + 8 * r
            info = struct.unpack_from("<I", d, ro + 4)[0]
            if struct.unpack_from("<I", d, ro)[0] & 0x80000000 or not info & (1 << 27):
                continue
            num = info & 0xFFFFFF
            if num >= at:
                struct.pack_into("<I", d, ro + 4, (info & ~0xFFFFFF) | (num + 1))
    for k in range(dy[13]):
        io = dy[12] + 4 * k
        v = struct.unpack_from("<I", d, io)[0]
        if v < 0x40000000 and v >= at:
            struct.pack_into("<I", d, io, v + 1)
    if dysym:
        dy[1] += 1
        for i in (2, 4):  # iextdefsym, iundefsym
            if dy[i] >= at:
                dy[i] += 1
        struct.pack_into("<18I", d, dysym + 8, *dy)
    strx = strsize
    new_str = name.encode() + b"\0"
    new_str += b"\0" * (-(strsize + len(new_str)) % 4)
    entry = struct.pack("<IBBHI", strx, 0x0E, nsect, 0, addr + offset)
    out = d[:symoff + 12 * at] + entry + d[symoff + 12 * at:] + new_str
    struct.pack_into("<4I", out, symtab + 8, symoff, nsyms + 1, stroff + 12, strsize + len(new_str))
    tmp = path + ".tmp"
    with open(tmp, "wb") as f:
        f.write(out)
    os.chmod(tmp, os.stat(path).st_mode & 0o7777)
    os.rename(tmp, path)
    return True


def label_crt(sdk):
    """Apple's 10.9 SDK dylib1.10.5.o names its __data word (the image's mach header,
    pushed by dyld_stub_binding_helper) `dyld__mach_header`; the 10.6 SDK's leaves it
    unnamed. Retail's 10.5-target modules (sourcevr, hl2mp client and server) carry
    the local symbol."""
    return add_local_symbol(os.path.join(sdk, "usr", "lib", "dylib1.10.5.o"),
                            "dyld__mach_header", "__data", 0)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--prefix", default=DEFAULT_PREFIX)
    ap.add_argument("--sdk106", help="thinned 10.6 SDK (default <prefix>/sdk/MacOSX10.6-i386.sdk)")
    ap.add_argument("--depot", action="append",
                    help="retail module tree(s) to read imports from (repeatable; default depot 223 build 19307283)")
    ap.add_argument("--out", help="output SDK (default <prefix>/sdk/MacOSX10.9.sdk)")
    ap.add_argument("--clang", help="assembler driver (default <prefix>/clang-600.0.57/bin/clang)")
    ap.add_argument("--ld", help="ld64 (default <prefix>/ld64-242/bin/ld)")
    a = ap.parse_args()
    sdk106 = a.sdk106 or os.path.join(a.prefix, "sdk", "MacOSX10.6-i386.sdk")
    out = a.out or os.path.join(a.prefix, "sdk", "MacOSX10.9.sdk")
    clang = a.clang or os.path.join(a.prefix, "clang-600.0.57", "bin", "clang")
    ld = a.ld or os.path.join(a.prefix, "ld64-242", "bin", "ld")
    dyldinfo = os.path.join(os.path.dirname(ld), "dyldinfo")
    depots = a.depot or [os.path.join(DEPOT, d, "19307283") for d in ("223", "340", "380", "420")]

    mods = find_retail_modules(depots)
    print(f"read {len(mods)} retail modules")
    libs, weak = collect_retail(mods, dyldinfo)

    if os.path.exists(out):
        shutil.rmtree(out)
    shutil.copytree(sdk106, out, symlinks=True)
    with open(os.path.join(out, "SDKSettings.plist"), "w") as f:
        f.write(SDK_SETTINGS)
    # Known 10.6 -> 10.9 header differences that break compilation.
    here = os.path.dirname(os.path.abspath(__file__))
    for patch in sorted(glob.glob(os.path.join(here, "patches", "sdk109-*.patch"))):
        with open(patch) as f:
            subprocess.run(["patch", "-p1", "--forward", "-s"], cwd=out, stdin=f, check=True)
        print(f"applied {os.path.basename(patch)}")
    label_crt(out)

    plan = {}
    for name, e in libs.items():
        exports, public = flatten_exports(sdk106, name)
        added = 0
        for sym in e["imports"]:
            if sym not in exports:
                exports[sym] = ("abs" if sym.startswith(".objc_class_name_") else "text", sym in weak)
                added += 1
            elif sym in weak and not exports[sym][1]:
                exports[sym] = (exports[sym][0], True)
        plan[name] = {"exports": exports, "public": public, "added": added, **e}

    # Children before parents so -reexport_library finds the stub.
    order, state = [], {}

    def visit(n):
        if state.get(n) == 2 or n not in plan:
            return
        state[n] = 1
        for c in plan[n]["public"]:
            if state.get(c) != 1:
                visit(c)
        state[n] = 2
        order.append(n)

    for n in sorted(plan):
        visit(n)

    manifest = {}
    with tempfile.TemporaryDirectory() as tmp:
        for i, name in enumerate(order):
            p = plan[name]
            s = os.path.join(tmp, f"stub{i}.s")
            o = os.path.join(tmp, f"stub{i}.o")
            write_stub_asm(s, p["exports"])
            subprocess.run([clang, "-arch", "i386", "-c", s, "-o", o], check=True)
            dest = out + name
            os.makedirs(os.path.dirname(dest), exist_ok=True)
            if os.path.lexists(dest):
                os.unlink(dest)
            cmd = [ld, "-arch", "i386", "-dylib", "-o", dest, o,
                   "-install_name", name,
                   "-current_version", macho.fmt_version(p["current"]),
                   "-compatibility_version", macho.fmt_version(p["compat"]),
                   "-macosx_version_min", "10.9", "-sdk_version", "10.9", "-no_uuid",
                   "-syslibroot", out]
            for c in p["public"]:
                cmd += ["-reexport_library", out + c]
            subprocess.run(cmd, check=True)
            _, aliases = locate(sdk106, name)
            for al in aliases:
                al = out + al[len(sdk106):]
                if os.path.lexists(al):
                    os.unlink(al)
                os.symlink(os.path.relpath(dest, os.path.dirname(al)), al)
            manifest[name] = {
                "current_version": macho.fmt_version(p["current"]),
                "compatibility_version": macho.fmt_version(p["compat"]),
                "retail_modules": p["modules"],
                "retail_imports": len(p["imports"]),
                "exports": len(p["exports"]),
                "added_beyond_10_6": p["added"],
                "weak_exports": sum(1 for _, w in p["exports"].values() if w),
                "reexports": p["public"],
            }
            print(f"{name}: {macho.fmt_version(p['current'])} exports={len(p['exports'])} "
                  f"(+{p['added']} retail-only) reexports={len(p['public'])}")
    with open(os.path.join(out, "bytematch-stubs.json"), "w") as f:
        json.dump(manifest, f, indent=1, sort_keys=True)
    print(f"SDK: {out}")


if __name__ == "__main__":
    main()
