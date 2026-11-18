#!/usr/bin/env python3
"""Evaluate a VPC project's preprocessor defines for one platform/configuration.

Follows $Include, honours $Macro/$Conditional and [$COND] suffixes, skips
$Configuration blocks for other configurations, and collects
$PreprocessorDefinitions and $AdditionalIncludeDirectories in order. This is
a hint generator, not a VPC reimplementation: per-file $Configuration
overrides and the Xcode generator's own additions aren't modelled.

Usage: vpc_defines.py PROJECT.vpc [--flag OSXALL ...] [--config Release]
"""
import argparse, json, os, re

OSX32_FLAGS = ("OSXALL", "OSX32", "POSIX", "GL", "SDL")


class VPC:
    def __init__(self, srcroot, flags=OSX32_FLAGS, config="Release", macros=None):
        self.srcroot, self.flags, self.config = srcroot, set(flags), config
        self.macros = dict(macros or {})
        self.defines, self.includes, self.files = [], [], []

    def cond(self, text):
        if not text:
            return True
        expr = text.strip()[1:-1]
        expr = re.sub(r"\$(\w+)", lambda m: str(self.truth(m.group(1))), expr)
        expr = expr.replace("&&", " and ").replace("||", " or ").replace("!", " not ")
        try:
            return bool(eval(expr, {"__builtins__": {}}, {}))
        except Exception:
            return False

    def truth(self, name):
        if name in self.flags:
            return True
        v = self.macros.get(name)
        return bool(v) and v != "0"

    def expand(self, s):
        return re.sub(r"\$(\w+)", lambda m: self.macros.get(m.group(1), m.group(0)) if m.group(1) != "BASE" else "$BASE", s)

    def lines(self, path):
        text = open(path, errors="replace").read().replace("\\\r\n", " ").replace("\\\n", " ")
        for raw in text.splitlines():
            line = re.sub(r"//.*$", "", raw).strip()
            if line:
                yield line

    def parse(self, vpc, projdir=None):
        projdir = projdir or os.path.dirname(os.path.abspath(vpc))
        self.macros.setdefault("SRCDIR", os.path.relpath(self.srcroot, projdir))
        self.files.append(os.path.relpath(vpc, self.srcroot))
        depth, skip_at = 0, None
        for line in self.lines(vpc):
            m = re.match(r'(.*?)\s*(\[[^\]]*\])\s*$', line)
            body, cond = (m.group(1), m.group(2)) if m and not line.startswith("$File") else (line, None)
            if line == "{":
                depth += 1
                continue
            if line == "}":
                depth -= 1
                if skip_at is not None and depth < skip_at:
                    skip_at = None
                continue
            if skip_at is not None:
                continue
            if not self.cond(cond):
                if body.startswith("$Configuration") or body.startswith("$Folder") or body.startswith("$Project"):
                    skip_at = depth + 1
                continue
            km = re.match(r'\$(\w+)\s*(.*)', body)
            if not km:
                continue
            key, rest = km.group(1), km.group(2).strip()
            key = {k.lower(): k for k in ("Macro", "Conditional", "Include", "Configuration",
                                         "PreprocessorDefinitions", "AdditionalIncludeDirectories")
                   }.get(key.lower(), key)
            val = rest[1:-1] if rest.startswith('"') and rest.endswith('"') else rest
            if key == "Macro":
                mm = re.match(r'(\w+)\s+"([^"]*)"', rest)
                if mm:
                    self.macros[mm.group(1)] = self.expand(mm.group(2))
            elif key == "Conditional":
                mm = re.match(r'(\w+)\s+"([^"]*)"', rest)
                if mm and mm.group(2) not in ("", "0"):
                    self.flags.add(mm.group(1))
            elif key == "Include":
                inc = self.expand(val).replace("\\", "/")
                p = os.path.normpath(os.path.join(projdir, inc))
                if os.path.exists(p):
                    self.parse(p, projdir)
            elif key == "Configuration":
                name = val.strip('"')
                if name and name != self.config:
                    skip_at = depth + 1
            elif key == "PreprocessorDefinitions":
                for d in re.split(r"[;,]", self.expand(val)):
                    d = d.strip()
                    if d and d != "$BASE" and d not in self.defines:
                        self.defines.append(d)
            elif key == "AdditionalIncludeDirectories":
                for d in re.split(r"[;,]", self.expand(val)):
                    d = d.strip().replace("\\", "/")
                    if d and d != "$BASE" and d not in self.includes:
                        self.includes.append(d)
        return self


def evaluate(vpc, srcroot, flags=OSX32_FLAGS, config="Release", macros=None):
    v = VPC(srcroot, flags, config, macros).parse(vpc)
    return {"vpc_files": v.files, "defines": v.defines, "include_dirs": v.includes,
            "flags": sorted(v.flags)}


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("vpc")
    ap.add_argument("--srcroot", default=None)
    ap.add_argument("--flag", action="append", default=list(OSX32_FLAGS))
    ap.add_argument("--config", default="Release")
    a = ap.parse_args()
    root = a.srcroot or os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(a.vpc)), ".."))
    print(json.dumps(evaluate(a.vpc, root, a.flag, a.config), indent=1))


if __name__ == "__main__":
    main()
