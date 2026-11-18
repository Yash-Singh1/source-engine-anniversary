#!/usr/bin/env python3
"""Evaluate Valve VPC project scripts for one platform and configuration.

Enough of VPC to reproduce what `vpc /2013` put into a Win32 Release .vcxproj:
$Macro / $MacroRequired / $Conditional, $Include, [$COND] suffixes on any
line or block header, unnamed and named $Configuration blocks, "$BASE"
accumulation, $Project/$Folder/$File trees with -$File removal and per-file
$Configuration overrides, and $Lib/$Implib link libraries.

Usage: vpc.py PROJECT.vpc [--srcroot DIR] [--cond NAME ...] [--json]
"""
import argparse, json, ntpath, os, re, sys

WIN32_RETAIL = ("WIN32", "WINDOWS", "VS2013", "RETAIL")

# Keys whose $BASE accumulation is a separator-joined list. Everything else is
# scalar (the last assignment wins, $BASE still substitutes textually).
LIST_KEYS = {
    "preprocessordefinitions": ";", "additionalincludedirectories": ";",
    "disablespecificwarnings": ";", "additionallibrarydirectories": ";",
    "ignorespecificlibrary": ";", "forceincludes": ";",
    "additionaloptions": " ", "additionaldependencies": " ",
    "systemlibraries": ";", "delayloadeddlls": ";",
}

TOOL_BLOCKS = {"general", "debugging", "compiler", "linker", "librarian", "manifesttool",
               "xmldocumentgenerator", "browseinformation", "resources", "prebuildevent",
               "prelinkevent", "postbuildevent", "custombuildstep", "intellisense", "xbox"}

FILE_KEYS = {"$file", "$dynamicfile", "$schemafile", "$shaders", "$lib", "$implib",
             "$libexternal", "$implibexternal"}


class VPCError(Exception):
    pass


def logical_lines(path):
    """Yield token lists, one per logical line (backslash continuations joined)."""
    with open(path, encoding="utf-8-sig", errors="replace") as f:
        text = f.read().replace("\r\n", "\n").replace("\r", "\n")
    pending = []
    for raw in text.split("\n"):
        toks, cont = tokenize(raw)
        pending += toks
        if cont:
            continue
        if pending:
            yield pending
        pending = []
    if pending:
        yield pending


def tokenize(line):
    """Split one physical line into strings, words, [conditions] and braces.

    Returns (tokens, continued). Comments start at // outside quotes.
    """
    toks, i, n = [], 0, len(line)
    while i < n:
        c = line[i]
        if c in " \t":
            i += 1
        elif line.startswith("//", i):
            break
        elif c == '"':
            j = line.find('"', i + 1)
            if j < 0:
                j = n
            toks.append(("str", line[i + 1:j]))
            i = j + 1
        elif c == "[":
            j = line.find("]", i)
            if j < 0:
                j = n
            toks.append(("cond", line[i + 1:j]))
            i = j + 1
        elif c in "{}":
            toks.append((c, c))
            i += 1
        elif c == "\\" and (not line[i + 1:].strip() or line[i + 1:].lstrip().startswith("//")):
            return toks, True
        else:
            j = i
            while j < n and line[j] not in ' \t"[]{}':
                j += 1
            toks.append(("word", line[i:j]))
            i = j
    return toks, False


class Project:
    def __init__(self, srcroot, vpc, conditionals=WIN32_RETAIL, config="Release", macros=None):
        self.srcroot = os.path.abspath(srcroot)
        self.vpc = os.path.abspath(vpc)
        self.projdir = os.path.dirname(self.vpc)
        self.config = config
        self.conds = dict((c.upper(), "1") for c in conditionals)
        self.macros = {}
        for k, v in (macros or {}).items():
            self.macros[k.upper()] = v
        self.macros.setdefault("QUOTE", '"')
        self.macros.setdefault("_DLL_EXT", ".dll")
        self.macros.setdefault("_IMPLIB_EXT", ".lib")
        self.macros.setdefault("_STATICLIB_EXT", ".lib")
        self.macros.setdefault("_EXE_EXT", ".exe")
        self.macros.setdefault("PROJECTNAME", os.path.splitext(os.path.basename(vpc))[0])
        self.settings = {}  # (block, key) -> value; block e.g. "compiler"
        self.files = []     # dicts: path, folder, settings, excluded
        self.libs = []      # dicts: kind, path
        self.name = None
        self.included = []

    # --- conditions and macros -------------------------------------------
    def truth(self, name):
        name = name.upper()
        v = self.conds.get(name)
        if v is None:
            v = self.macros.get(name)
        return bool(v) and v.strip() not in ("", "0")

    def cond(self, text):
        if text is None:
            return True
        expr = text.strip()
        if not expr:
            return True
        expr = re.sub(r"\$(\w+)", lambda m: " %s " % self.truth(m.group(1)), expr)
        expr = expr.replace("&&", " and ").replace("||", " or ").replace("!", " not ")
        try:
            return bool(eval(expr, {"__builtins__": {}}, {}))
        except Exception as e:
            raise VPCError("bad condition [%s]: %s" % (text, e))

    def expand(self, s, base=None):
        def sub(m):
            name = m.group(1)
            if name.upper() == "BASE":
                return base if base is not None else ""
            v = self.macros.get(name.upper())
            return v if v is not None else m.group(0)
        prev = None
        while prev != s:
            prev = s
            s = re.sub(r"\$(\w+)(?!\()", sub, s)
        return s

    # --- parsing -------------------------------------------------------------
    def parse(self):
        self._parse_file(self.vpc)
        return self

    def _resolve(self, path):
        p = self.expand(path).replace("\\", "/")
        if not os.path.isabs(p):
            p = os.path.normpath(os.path.join(self.projdir, p))
        return p

    def _parse_file(self, path):
        if not os.path.exists(path):
            # VPC paths are case-insensitive on Windows.
            path = case_lookup(path) or path
        if not os.path.exists(path):
            raise VPCError("missing include %s" % path)
        self.included.append(os.path.relpath(path, self.srcroot))
        lines = list(logical_lines(path))
        self._block(lines, 0, len(lines), ctx=("top",))

    def _split(self, toks):
        """(head, args, cond, opens_block) for one logical line."""
        cond = None
        opens = False
        body = []
        for kind, val in toks:
            if kind == "cond":
                cond = val
            elif kind == "{":
                opens = True
            else:
                body.append((kind, val))
        return body, cond, opens

    def _find_block(self, lines, i, end):
        """Given that a block opens at/after line i, return (start, stop) of its body."""
        toks = lines[i]
        depth = 0
        j = i
        # opening brace may be on this line or the next one
        if not any(k == "{" for k, _ in toks):
            j = i + 1
            if j >= end or lines[j][0][0] != "{":
                return None
        start = j
        while j < end:
            for k, _ in lines[j]:
                if k == "{":
                    depth += 1
                elif k == "}":
                    depth -= 1
                    if depth == 0:
                        return start, j
            j += 1
        raise VPCError("unbalanced braces")

    def _body_range(self, lines, i, end):
        blk = self._find_block(lines, i, end)
        if blk is None:
            return None, i + 1
        start, stop = blk
        # body lines exclude the brace-only tokens at start/stop
        return (start, stop), stop + 1

    def _block(self, lines, i, end, ctx):
        while i < end:
            toks = lines[i]
            body, cond, opens = self._split(toks)
            if not body:
                i += 1
                continue
            head = body[0][1]
            hl = head.lower()
            args = [v for _, v in body[1:]]
            rng, nxt = (None, i + 1)
            if opens or (i + 1 < end and lines[i + 1] and lines[i + 1][0][0] == "{"):
                rng, nxt = self._body_range(lines, i, end)
            ok = self.cond(cond)
            if hl in ("$macro", "$macrorequired", "$macrorequiredallowempty"):
                if ok and args:
                    name = args[0].lstrip("$").upper()
                    val = self.expand(args[1]) if len(args) > 1 else ""
                    if hl == "$macro" or name not in self.macros:
                        if hl == "$macro" or len(args) > 1 or hl == "$macrorequiredallowempty":
                            self.macros[name] = val
            elif hl == "$conditional":
                if ok and args:
                    self.conds[args[0].lstrip("$").upper()] = self.expand(args[1]) if len(args) > 1 else "1"
            elif hl == "$include":
                if ok and args:
                    self._parse_file(self._resolve(args[0]))
            elif hl == "$configuration":
                if rng and ok and (not args or args[0].lower() == self.config.lower()):
                    self._config(lines, rng[0], rng[1] + 1, target=None)
            elif hl == "$project":
                if args and ok:
                    self.name = self.name or args[0]
                if rng and ok:
                    self._project(lines, rng[0], rng[1] + 1, folder="")
            elif hl in ("$loadaddressmacro", "$loadaddressmacroauto", "$ignoreredundancywarning",
                        "$linux", "$customcommands"):
                pass
            elif hl in ("{", "}"):
                pass
            i = nxt

    def _config(self, lines, i, end, target):
        """Inside $Configuration: tool blocks with key/value lines."""
        while i < end:
            body, cond, opens = self._split(lines[i])
            if not body:
                i += 1
                continue
            head = body[0][1].lower().lstrip("$")
            rng, nxt = (None, i + 1)
            if opens or (i + 1 < end and lines[i + 1] and lines[i + 1][0][0] == "{"):
                rng, nxt = self._body_range(lines, i, end)
            if rng and head in TOOL_BLOCKS:
                if self.cond(cond):
                    self._tool(lines, rng[0], rng[1] + 1, head, target)
            i = nxt

    def _tool(self, lines, i, end, block, target):
        store = self.settings if target is None else target
        for toks in lines[i:end]:
            body, cond, _ = self._split(toks)
            if not body or body[0][0] != "word" or not body[0][1].startswith("$"):
                continue
            if not self.cond(cond):
                continue
            key = body[0][1][1:].lower()
            vals = [v for _, v in body[1:]]
            k = (block, key)
            prev = store.get(k)
            if prev is None and target is not None:
                prev = self.settings.get(k)
            if not vals:
                store[k] = ""
                continue
            raw = " ".join(vals) if key == "commandline" else vals[0]
            store[k] = self.expand(raw, base=prev or "")

    def _project(self, lines, i, end, folder):
        while i < end:
            body, cond, opens = self._split(lines[i])
            if not body:
                i += 1
                continue
            head = body[0][1]
            hl = head.lower()
            args = [v for _, v in body[1:]]
            rng, nxt = (None, i + 1)
            if opens or (i + 1 < end and lines[i + 1] and lines[i + 1][0][0] == "{"):
                rng, nxt = self._body_range(lines, i, end)
            ok = self.cond(cond)
            remove = hl.startswith("-")
            kw = hl.lstrip("-")
            if kw == "$folder":
                if ok and rng:
                    self._project(lines, rng[0], rng[1] + 1, folder + "/" + (args[0] if args else ""))
            elif kw in ("$file", "$dynamicfile", "$schemafile", "$shaders"):
                if ok:
                    for a in args:
                        self._file(a, folder, remove, kw, lines, rng)
            elif kw in ("$lib", "$implib", "$libexternal", "$implibexternal"):
                if ok:
                    for a in args:
                        self._lib(kw[1:], a, remove)
            i = nxt

    def _file(self, name, folder, remove, kind, lines, rng):
        path = self._resolve(name)
        key = path.lower()
        if remove:
            self.files = [f for f in self.files if f["path"].lower() != key]
            return
        existing = next((f for f in self.files if f["path"].lower() == key), None)
        if existing is None:
            existing = {"path": path, "folder": folder, "kind": kind, "settings": {}}
            self.files.append(existing)
        if rng:
            self._file_body(lines, rng[0], rng[1] + 1, existing["settings"])

    def _file_body(self, lines, i, end, store):
        while i < end:
            body, cond, opens = self._split(lines[i])
            if not body:
                i += 1
                continue
            hl = body[0][1].lower()
            args = [v for _, v in body[1:]]
            rng, nxt = (None, i + 1)
            if opens or (i + 1 < end and lines[i + 1] and lines[i + 1][0][0] == "{"):
                rng, nxt = self._body_range(lines, i, end)
            if hl == "$configuration" and rng and self.cond(cond) and (not args or args[0].lower() == self.config.lower()):
                self._config(lines, rng[0], rng[1] + 1, target=store)
            i = nxt

    def _lib(self, kind, name, remove):
        path = self.expand(name).replace("\\", "/")
        base = ntpath.basename(path).lower()
        if remove:
            def name(l):
                return ntpath.basename(l["path"] or l["name"]).lower()
            self.libs = [l for l in self.libs if name(l) != base and os.path.splitext(name(l))[0] != base]
            return
        if not os.path.splitext(path)[1]:
            path += self.macros.get("_STATICLIB_EXT" if kind in ("lib", "libexternal") else "_IMPLIB_EXT", ".lib")
        if "/" not in path:
            if kind.endswith("external"):
                self.libs.append({"kind": kind, "path": None, "name": path})
                return
            path = self.expand("$LIBPUBLIC").replace("\\", "/") + "/" + path
        if not os.path.isabs(path):
            path = os.path.normpath(os.path.join(self.projdir, path))
        self.libs.append({"kind": kind, "path": path})

    # --- results -------------------------------------------------------------
    def get(self, block, key, file=None):
        if file is not None and (block, key) in file["settings"]:
            return file["settings"][(block, key)]
        return self.settings.get((block, key))

    def excluded(self, f):
        v = self.get("general", "excludedfrombuild", f)
        if v is None:
            v = f["settings"].get(("compiler", "excludedfrombuild"))
        return (v or "").strip().lower().startswith("yes")

    def rel(self, path):
        return os.path.relpath(path, self.srcroot).replace(os.sep, "/")

    def summary(self):
        files = []
        for f in self.files:
            ent = {"path": self.rel(f["path"]), "folder": f["folder"].strip("/"), "kind": f["kind"]}
            if self.excluded(f):
                ent["excluded"] = True
            over = dict(("%s.%s" % k, v) for k, v in f["settings"].items())
            if over:
                ent["settings"] = over
            files.append(ent)
        return {
            "vpc": self.rel(self.vpc),
            "name": self.name,
            "macros": dict((k, self.macros[k]) for k in ("OUTBINNAME", "OUTLIBNAME", "OUTBINDIR", "OUTLIBDIR")
                           if k in self.macros),
            "settings": dict(("%s.%s" % k, v) for k, v in sorted(self.settings.items())),
            "files": files,
            "libs": [dict(kind=l["kind"], **({"path": self.rel(l["path"])} if l["path"] else {"name": l["name"]}))
                     for l in self.libs],
            "included": self.included,
        }


def case_lookup(path):
    """Find PATH on a case-sensitive filesystem by matching each component case-insensitively."""
    parts = os.path.abspath(path).split(os.sep)
    cur = os.sep
    for p in parts[1:]:
        if not p:
            continue
        cand = os.path.join(cur, p)
        if os.path.exists(cand):
            cur = cand
            continue
        try:
            names = os.listdir(cur)
        except OSError:
            return None
        hit = next((n for n in names if n.lower() == p.lower()), None)
        if hit is None:
            return None
        cur = os.path.join(cur, hit)
    return cur


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("vpc")
    ap.add_argument("--srcroot", default=None, help="tree root [default: two levels above the .vpc's vpc_scripts]")
    ap.add_argument("--cond", action="append", default=None, help="conditional set to 1 (repeatable)")
    ap.add_argument("--config", default="Release")
    args = ap.parse_args()
    srcroot = args.srcroot or os.path.dirname(os.path.dirname(os.path.abspath(args.vpc)))
    p = Project(srcroot, args.vpc, args.cond or WIN32_RETAIL, args.config).parse()
    json.dump(p.summary(), sys.stdout, indent=1)
    print()


if __name__ == "__main__":
    main()
