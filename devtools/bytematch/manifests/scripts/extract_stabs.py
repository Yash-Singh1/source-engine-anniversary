#!/usr/bin/env python3
"""Reconstruct Valve's macOS i386 build manifest from Mach-O stab debug maps.

For each retail module, walks the nlist table in symbol-table order and emits
one JSON describing every linked object (N_OSO) in link order: archive member,
mtime, ccache temp name (N_SO), the files that contributed line info (N_SOL),
the primary source file when the stabs name it, plus Mach-O load-command facts.
Functions (N_FUN start/size) and data (N_STSYM/N_LCSYM/N_GSYM) per object go to
a separate gzipped JSON.

ld64 sorts debug notes by input-file ordinal, then address, so the N_OSO order
is the link-line order: loose objects in Xcode build-phase order, then each
static archive's pulled members in archive-member order. ld64 emits one N_SOL
per distinct file per translation unit (first-seen order), so N_SOL is a set,
not a per-function attribution, and it can omit the main source file.

Usage: extract_stabs.py [--depots DIR] [--out DIR] [module-key ...]
"""
import argparse, datetime, gzip, hashlib, json, os, re, struct, sys
from collections import Counter, OrderedDict

DEPOTS = "/Volumes/Expansion/mac-ports/hl2-anniversary-depot/depots"
BUILD = "19307283"
SRC_ROOT_RE = re.compile(r"^/Users/buildbot/buildslave/(rel_[a-z0-9_]+)/build/src/")

# (key, depot, relative path). Episodic 380/420 dylibs are byte-identical; the
# 420 copy is recorded as an alias of the 380 one.
MODULES = [
    ("bin/bsppack", "223", "bin/bsppack.dylib"),
    ("bin/bugreporter_filequeue", "223", "bin/bugreporter_filequeue.dylib"),
    ("bin/bugreporter_public", "223", "bin/bugreporter_public.dylib"),
    ("bin/chromehtml", "223", "bin/chromehtml.dylib"),
    ("bin/datacache", "223", "bin/datacache.dylib"),
    ("bin/engine", "223", "bin/engine.dylib"),
    ("bin/filesystem_stdio", "223", "bin/filesystem_stdio.dylib"),
    ("bin/GameUI", "223", "bin/GameUI.dylib"),
    ("bin/inputsystem", "223", "bin/inputsystem.dylib"),
    ("bin/launcher", "223", "bin/launcher.dylib"),
    ("bin/libtier0", "223", "bin/libtier0.dylib"),
    ("bin/libtogl", "223", "bin/libtogl.dylib"),
    ("bin/libvstdlib", "223", "bin/libvstdlib.dylib"),
    ("bin/materialsystem", "223", "bin/materialsystem.dylib"),
    ("bin/replay", "223", "bin/replay.dylib"),
    ("bin/scenefilecache", "223", "bin/scenefilecache.dylib"),
    ("bin/ServerBrowser", "223", "bin/ServerBrowser.dylib"),
    ("bin/shaderapidx9", "223", "bin/shaderapidx9.dylib"),
    ("bin/shaderapiempty", "223", "bin/shaderapiempty.dylib"),
    ("bin/soundemittersystem", "223", "bin/soundemittersystem.dylib"),
    ("bin/sourcevr", "223", "bin/sourcevr.dylib"),
    ("bin/stdshader_dx9", "223", "bin/stdshader_dx9.dylib"),
    ("bin/studiorender", "223", "bin/studiorender.dylib"),
    ("bin/vaudio_miles", "223", "bin/vaudio_miles.dylib"),
    ("bin/vaudio_speex", "223", "bin/vaudio_speex.dylib"),
    ("bin/vgui2", "223", "bin/vgui2.dylib"),
    ("bin/vguimatsurface", "223", "bin/vguimatsurface.dylib"),
    ("bin/video_quicktime", "223", "bin/video_quicktime.dylib"),
    ("bin/video_services", "223", "bin/video_services.dylib"),
    ("bin/vphysics", "223", "bin/vphysics.dylib"),
    ("bin/vpk_osx32", "223", "bin/vpk_osx32"),
    ("bin/vtex_dll", "223", "bin/vtex_dll.dylib"),
    ("hl2_osx", "223", "hl2_osx"),
    ("hl2/client", "223", "hl2/bin/client.dylib"),
    ("hl2/server", "223", "hl2/bin/server.dylib"),
    ("hl2_complete/client", "223", "hl2_complete/bin/client.dylib"),
    ("hl2_complete/server", "223", "hl2_complete/bin/server.dylib"),
    ("hl2mp/client", "223", "hl2mp/bin/client.dylib"),
    ("hl2mp/server", "223", "hl2mp/bin/server.dylib"),
    ("lostcoast/client", "340", "lostcoast/bin/client.dylib"),
    ("lostcoast/server", "340", "lostcoast/bin/server.dylib"),
    ("episodic/client", "380", "episodic/bin/client.dylib"),
    ("episodic/server", "380", "episodic/bin/server.dylib"),
]
ALIASES = {
    "episodic/client": [("420", "episodic/bin/client.dylib")],
    "episodic/server": [("420", "episodic/bin/server.dylib")],
}

N_STAB = 0xE0
N_GSYM, N_FUN, N_STSYM, N_LCSYM = 0x20, 0x24, 0x26, 0x28
N_BNSYM, N_ENSYM, N_SO, N_OSO, N_SOL = 0x2E, 0x4E, 0x64, 0x66, 0x84
SRC_EXTS = (".c", ".cc", ".cpp", ".cxx", ".m", ".mm")

LC = {0x1: "SEGMENT", 0x2: "SYMTAB", 0xB: "DYSYMTAB", 0xC: "LOAD_DYLIB", 0xD: "ID_DYLIB",
      0x18 | 0x80000000: "LOAD_WEAK_DYLIB", 0x1B: "UUID", 0x22 | 0x80000000: "DYLD_INFO_ONLY",
      0x22: "DYLD_INFO", 0x24: "VERSION_MIN_MACOSX", 0x26: "FUNCTION_STARTS",
      0x29: "DATA_IN_CODE", 0x2A: "SOURCE_VERSION", 0x28 | 0x80000000: "MAIN", 0x5: "UNIXTHREAD",
      0xE: "LOAD_DYLINKER", 0x1C | 0x80000000: "RPATH", 0x1F | 0x80000000: "REEXPORT_DYLIB"}


def ts(v):
    return datetime.datetime.fromtimestamp(v, datetime.timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")


def ver(v):
    return f"{v >> 16}.{(v >> 8) & 0xFF}.{v & 0xFF}"


def cstr(buf, off):
    end = buf.index(b"\0", off)
    return buf[off:end].decode("utf-8", "replace")


def i386_slice(data):
    magic = struct.unpack_from(">I", data)[0]
    if magic == 0xCAFEBABE:
        for i in range(struct.unpack_from(">I", data, 4)[0]):
            cpu, _, off, size, _ = struct.unpack_from(">5I", data, 8 + 20 * i)
            if cpu == 7:
                return data[off:off + size]
        raise ValueError("no i386 slice")
    return data


def parse_macho(data):
    data = i386_slice(data)
    magic, cpu, _, ftype, ncmds, _, flags = struct.unpack_from("<7I", data)
    if magic != 0xFEEDFACE:
        raise ValueError("not a 32-bit Mach-O")
    info = OrderedDict(filetype={2: "MH_EXECUTE", 6: "MH_DYLIB", 8: "MH_BUNDLE"}.get(ftype, ftype),
                       flags=hex(flags), segments=[], dylibs=[])
    symtab = None
    off = 28
    for _ in range(ncmds):
        cmd, size = struct.unpack_from("<2I", data, off)
        name = LC.get(cmd, hex(cmd))
        if cmd == 0x1:
            segname = data[off + 8:off + 24].rstrip(b"\0").decode()
            vmaddr, vmsize, fileoff, filesize, _, _, nsects, _ = struct.unpack_from("<8I", data, off + 24)
            secs = []
            for s in range(nsects):
                so = off + 56 + 68 * s
                sname = data[so:so + 16].rstrip(b"\0").decode()
                addr, ssize, _, align = struct.unpack_from("<4I", data, so + 32)
                secs.append([sname, hex(addr), ssize, 1 << align])
            info["segments"].append({"name": segname, "vmaddr": hex(vmaddr), "vmsize": vmsize, "sections": secs})
        elif cmd == 0x2:
            symtab = struct.unpack_from("<4I", data, off + 8)
        elif cmd in (0xC, 0xD, 0x18 | 0x80000000, 0x1F | 0x80000000):
            stroff, _, cur, compat = struct.unpack_from("<4I", data, off + 8)
            entry = {"path": cstr(data, off + stroff), "current": ver(cur), "compat": ver(compat)}
            if cmd == 0xD:
                info["install_name"] = entry
            else:
                entry["kind"] = name
                info["dylibs"].append(entry)
        elif cmd == 0x1B:
            info["uuid"] = data[off + 8:off + 24].hex().upper()
        elif cmd == 0x24:
            v, sdk = struct.unpack_from("<2I", data, off + 8)
            info["min_macosx"], info["sdk"] = ver(v), ver(sdk)
        elif cmd == 0x2A:
            info["source_version"] = struct.unpack_from("<Q", data, off + 8)[0]
        info.setdefault("load_commands", []).append(name)
        off += size
    return data, info, symtab


def iter_nlist(data, symtab):
    symoff, nsyms, stroff, strsize = symtab
    strtab = data[stroff:stroff + strsize]
    for i in range(nsyms):
        strx, ntype, nsect, ndesc, nvalue = struct.unpack_from("<IBBhI", data, symoff + 12 * i)
        yield i, ntype, nsect, ndesc & 0xFFFF, nvalue, (cstr(strtab, strx) if strx else "")


def rel_to_root(f):
    r = SRC_ROOT_RE.match(f)
    return os.path.normpath(f[r.end():]) if r else f


def split_oso(path):
    """Return (kind, archive, member, xcode_target, rel) for an N_OSO path."""
    rel = rel_to_root(path) if SRC_ROOT_RE.match(path) else path
    m = re.match(r"(.*)\(([^()]+)\)$", rel)
    if m:
        kind = "prebuilt_member" if m.group(1).startswith("lib/") else "archive_member"
        return kind, m.group(1), m.group(2), None, rel
    t = re.search(r"/([^/]+)\.build/Objects-normal/[^/]+/[^/]+$", rel)
    return "object", None, None, (t.group(1) if t else None), rel


def resolve_source(obj):
    """Primary source of a compile unit, when the stabs name it.

    ccache feeds clang a preprocessed temp whose name keeps the first 10
    characters of the source basename; match that against the N_SOL files.
    """
    so = obj["so_name"] or ""
    tmp = re.match(r"(.+?)\.tmp\.(.+)\.(\d+)\.(ii|i|mi|mii)$", so)
    stem = obj["stem"]
    srcs = [f for f in obj["files"] if f.lower().endswith(SRC_EXTS)]
    pick, how = None, None
    if tmp:
        pre = tmp.group(1)
        hits = [f for f in srcs if os.path.basename(f).startswith(pre)]
        exact = [f for f in hits if os.path.splitext(os.path.basename(f))[0] == stem]
        if len(exact) == 1:
            pick, how = exact[0], "sol:ccache_prefix+stem"
        elif len(hits) == 1:
            pick, how = hits[0], "sol:ccache_prefix"
    elif so.lower().endswith(SRC_EXTS):
        pick, how = so if so.startswith("/") else (obj["so_dir"] or "") + so, "n_so"
    if not pick:
        hits = [f for f in srcs if os.path.splitext(os.path.basename(f))[0].lower() == stem.lower()]
        if len(hits) == 1:
            pick, how = hits[0], "sol:stem"
    if pick:
        r = SRC_ROOT_RE.match(pick)
        return (rel_to_root(pick) if r else pick), (r.group(1) if r else None), how
    return None, None, "unresolved"


def extract(path):
    raw = open(path, "rb").read()
    data, info, symtab = parse_macho(raw)
    objs, cur, func = [], None, None
    pending_dir = pending_name = None
    stab_count = Counter()
    text_syms = {}
    for _, ntype, nsect, _, nvalue, name in iter_nlist(data, symtab):
        if not ntype & N_STAB:
            if (ntype & 0x0E) == 0x0E and nsect == 1:
                text_syms.setdefault(nvalue, name)
            continue
        stab_count[ntype] += 1
        if ntype == N_SO:
            if not name:
                cur = func = None
                pending_dir = pending_name = None
            elif pending_dir is None and name.endswith("/"):
                pending_dir = name
            else:
                pending_name = name
        elif ntype == N_OSO:
            kind, archive, member, target, rel = split_oso(name)
            tmp = re.match(r".+?\.tmp\.(.+)\.\d+\.(ii|i|mi|mii)$", pending_name or "")
            cur = OrderedDict(index=len(objs), oso=rel, kind=kind, archive=archive, member=member,
                              xcode_target=target, mtime=nvalue, mtime_utc=ts(nvalue) if nvalue else None,
                              so_dir=pending_dir, so_name=pending_name,
                              ccache_host=tmp.group(1) if tmp else None,
                              lang={"ii": "c++", "i": "c", "mi": "objc", "mii": "objc++"}.get(tmp.group(2)) if tmp else None,
                              stem=re.sub(r"\.o$", "", member or os.path.basename(rel)),
                              files=[], functions=[], data=[])
            objs.append(cur)
        elif cur is None:
            continue
        elif ntype == N_FUN:
            if name:
                func = [name, nvalue, None]
                cur["functions"].append(func)
            elif func is not None:
                func[2] = nvalue
                func = None
        elif ntype == N_SOL:
            cur["files"].append(name)
        elif ntype in (N_STSYM, N_LCSYM, N_GSYM):
            cur["data"].append([{N_STSYM: "static", N_LCSYM: "bss", N_GSYM: "global"}[ntype], name,
                                hex(nvalue) if ntype != N_GSYM else None])
    for o in objs:
        o["source"], o["build_root"], o["source_resolution"] = resolve_source(o)
        o["files"] = [rel_to_root(f) if SRC_ROOT_RE.match(f) else f for f in o["files"]]
        fs = [f for f in o["functions"] if f[2] is not None]
        o["n_functions"] = len(o["functions"])
        o["n_data"] = len(o["data"])
        o["text_bytes"] = sum(f[2] for f in fs)
        o["first_addr"] = hex(min(f[1] for f in fs)) if fs else None
    return raw, info, objs, stab_count, unstabbed_code(info, objs, text_syms)


def unstabbed_code(info, objs, text_syms):
    """__text symbols outside every N_FUN range: code from archives built without -g."""
    ranges = sorted((f[1], f[1] + (f[2] or 0)) for o in objs for f in o["functions"])
    text_end = None
    for seg in info["segments"]:
        for s in seg["sections"]:
            if s[0] == "__text":
                text_end = int(s[1], 16) + s[2]
    addrs = sorted(text_syms)
    out, j = [], 0
    for n, a in enumerate(addrs):
        while j < len(ranges) and ranges[j][1] <= a:
            j += 1
        if j < len(ranges) and ranges[j][0] <= a < ranges[j][1]:
            continue
        nxt = addrs[n + 1] if n + 1 < len(addrs) else text_end
        if j < len(ranges):
            nxt = min(nxt, ranges[j][0]) if nxt else ranges[j][0]
        out.append([text_syms[a], hex(a), (nxt - a) if nxt else None])
    groups = Counter()
    for name, _, size in out:
        cxx = re.match(r"__Z(?:L|N[KVr]*)?(?:S[t_]|St)?(\d+)", name)
        if cxx:
            start = cxx.end()
            key = "c++:" + name[start:start + int(cxx.group(1))]
        else:
            m = re.match(r"_+([A-Za-z]+?)(?:_|[A-Z0-9]|$)", name)
            key = m.group(1).lower() if m else name
        groups[key] += size or 0
    return OrderedDict(n_symbols=len(out), bytes=sum(s or 0 for _, _, s in out),
                       by_prefix=OrderedDict(groups.most_common(25)), symbols=out)


OBJ_KEYS = ("index", "oso", "kind", "archive", "member", "xcode_target", "stem", "mtime", "mtime_utc",
            "so_dir", "so_name", "ccache_host", "lang", "source", "build_root", "source_resolution",
            "n_functions", "n_data", "text_bytes", "first_addr", "files")


def build_docs(key, depot, rel):
    path = os.path.join(DEPOTS, depot, BUILD, rel)
    raw, info, objs, stab_count, unstabbed = extract(path)
    archives = OrderedDict()
    for o in objs:
        if o["archive"]:
            a = archives.setdefault(o["archive"], OrderedDict(
                archive=o["archive"], prebuilt=o["kind"] == "prebuilt_member",
                first_index=o["index"], members=[]))
            a["members"].append(o["member"])
    built_mtimes = sorted(o["mtime"] for o in objs if o["mtime"] and o["kind"] != "prebuilt_member")
    headers = Counter()
    for o in objs:
        for f in o["files"]:
            if not f.lower().endswith(SRC_EXTS):
                headers[f] += 1
    by_addr = [o["index"] for o in sorted((o for o in objs if o["first_addr"]),
                                          key=lambda o: int(o["first_addr"], 16))]
    doc = OrderedDict(
        module=key, depot=depot, build=BUILD, path=rel,
        aliases=[f"{d}/{BUILD}/{p}" for d, p in ALIASES.get(key, [])],
        sha256=hashlib.sha256(raw).hexdigest(), size=len(raw),
        macho=info,
        build_roots=dict(Counter(o["build_root"] for o in objs if o["build_root"])),
        ccache_hosts=dict(Counter(o["ccache_host"] for o in objs if o["ccache_host"])),
        xcode_targets=dict(Counter(o["xcode_target"] for o in objs if o["xcode_target"])),
        stab_counts={hex(k): v for k, v in sorted(stab_count.items())},
        n_objects=len(objs),
        n_loose_objects=sum(o["kind"] == "object" for o in objs),
        n_archive_members=sum(o["kind"] == "archive_member" for o in objs),
        n_prebuilt_members=sum(o["kind"] == "prebuilt_member" for o in objs),
        built_mtime_range=[ts(built_mtimes[0]), ts(built_mtimes[-1])] if built_mtimes else None,
        static_libraries=list(archives.values()),
        text_order_matches_oso_order=by_addr == sorted(by_addr),
        headers_with_code=OrderedDict(sorted(headers.items())),
        unstabbed_text=OrderedDict((k, v) for k, v in unstabbed.items() if k != "symbols"),
        objects=[OrderedDict((k, o[k]) for k in OBJ_KEYS) for o in objs],
    )
    funcs = OrderedDict(module=key, unstabbed_text_symbols=unstabbed["symbols"], objects=[
        OrderedDict(index=o["index"], oso=o["oso"],
                    functions=[[f[0], hex(f[1]), f[2]] for f in o["functions"]], data=o["data"])
        for o in objs])
    return doc, funcs


def dump_doc(doc, f):
    """Pretty top level, one line per object so diffs stay readable."""
    f.write("{\n")
    items = list(doc.items())
    for n, (k, v) in enumerate(items):
        f.write(f" {json.dumps(k)}: ")
        if k == "objects":
            f.write("[\n" + ",\n".join("  " + json.dumps(o) for o in v) + "\n ]")
        else:
            f.write(json.dumps(v, indent=1).replace("\n", "\n "))
        f.write(",\n" if n + 1 < len(items) else "\n")
    f.write("}\n")


def main():
    global DEPOTS
    here = os.path.dirname(os.path.abspath(__file__))
    ap = argparse.ArgumentParser()
    ap.add_argument("--depots", default=DEPOTS)
    ap.add_argument("--out", default=os.path.normpath(os.path.join(here, "..")))
    ap.add_argument("only", nargs="*")
    a = ap.parse_args()
    DEPOTS = a.depots
    os.makedirs(os.path.join(a.out, "modules"), exist_ok=True)
    os.makedirs(os.path.join(a.out, "functions"), exist_ok=True)
    for key, depot, rel in MODULES:
        if a.only and key not in a.only:
            continue
        doc, funcs = build_docs(key, depot, rel)
        name = key.replace("/", ".")
        with open(os.path.join(a.out, "modules", name + ".json"), "w") as f:
            dump_doc(doc, f)
        with gzip.GzipFile(os.path.join(a.out, "functions", name + ".json.gz"), "wb", mtime=0) as f:
            f.write(json.dumps(funcs, separators=(",", ":")).encode())
        unresolved = sum(o["source_resolution"] == "unresolved" for o in doc["objects"])
        print(f"{key:26} objs={doc['n_objects']:4} loose={doc['n_loose_objects']:4} "
              f"libs={len(doc['static_libraries']):2} unresolved={unresolved:3} "
              f"order_ok={doc['text_order_matches_oso_order']}", file=sys.stderr)


if __name__ == "__main__":
    main()
