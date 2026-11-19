#!/usr/bin/env python3
"""Restore retail line numbering in sources from __LINE__-only differences.

With RAD Telemetry on, every VPROF/tmZone site in a retail Windows function
carries its source line, so a function that differs only in those constants
says how far our numbering is off at that point. For each source file this
collects (our line, retail line) pairs from bytematch report JSONs (functions
whose only difference is line constants, at lines of ours that hold a
__LINE__-using macro), groups consecutive pairs with the same offset, and
writes `#line` directives at the start of the first function of each group;
existing `#line` directives inside a group get the group's offset too.

Lines are counted with BYTEMATCH_RETAIL defined (`#ifdef BYTEMATCH_RETAIL`
blocks active, `#ifndef` blocks inactive); other conditionals are assumed not
to contain `#line`. Re-measure each edited file: code between two pieces of
evidence takes the earlier group's offset up to the function that holds the
next piece.

usage: linefix.py REPORT.json [...] [--file SRC ...] [--apply]
"""
import argparse, collections, json, os, re, sys

HERE = os.path.dirname(os.path.abspath(__file__))
TREE = os.path.abspath(os.path.join(HERE, "..", "..", "..", ".."))
MACRO = re.compile(r"\b(VPROF\w*|tmZone\w*|TM_ZONE\w*|tmEnter|tmMessage|TM_MESSAGE|SNPROF\w*|MEM_ALLOC_CREDIT\w*|"
                   r"Assert\w*|Verify\w*|Warning|COMPILE_TIME_ASSERT|DBG_\w+|ETW\w+|CETWScope|Error)\s*\(")

def effective_lines(lines):
    """Physical index -> effective line number, as the preprocessor numbers them in retail mode."""
    eff, n = [], 1
    stack = []  # True when the enclosing block is active
    for raw in lines:
        s = raw.strip()
        active = all(stack)
        m = re.match(r"#\s*(ifdef|ifndef|if|elif|else|endif)\b(.*)", s)
        eff.append(n)
        if m:
            kw, rest = m.group(1), m.group(2).strip()
            if kw in ("ifdef", "ifndef", "if"):
                val = True
                cond = re.sub(r"\s+", "", rest)
                if kw == "ifndef" and rest == "BYTEMATCH_RETAIL" or cond in ("!defined(BYTEMATCH_RETAIL)", "!defined BYTEMATCH_RETAIL"):
                    val = False
                stack.append(val)
            elif kw in ("else", "elif"):
                if stack:
                    stack[-1] = not stack[-1] if kw == "else" else stack[-1]
            elif kw == "endif" and stack:
                stack.pop()
            n += 1
            continue
        m = re.match(r"#\s*line\s+(\d+)", s)
        if m and active:
            n = int(m.group(1))
            continue
        n += 1
    return eff


def brace_depths(lines):
    """Brace depth before each line (comments, strings and character literals ignored)."""
    out, depth, block = [], 0, False
    for l in lines:
        out.append(depth)
        s = re.sub(r'"(\\.|[^"\\])*"|\'(\\.|[^\'\\])*\'', '""', l)
        if block:
            if "*/" not in s:
                continue
            s, block = s.split("*/", 1)[1], False
        s = re.sub(r"/\*.*?\*/", "", s)
        if "/*" in s:
            s, block = s.split("/*", 1)[0], True
        s = s.split("//", 1)[0]
        if s.lstrip().startswith("#"):
            continue
        depth += s.count("{") - s.count("}")
    return out


def function_start(lines, idx, depths):
    """Index of the first signature line of the file-level definition that contains line idx.
    Valve's style puts a function's opening brace alone in column 0; brace depth is the fallback."""
    j = idx
    while j >= 0 and lines[j].rstrip() != "{" and lines[j].rstrip() != "}":
        j -= 1
    if j < 0 or lines[j].rstrip() == "}":
        j = idx
        while j >= 0 and not (depths[j] == 0 and j + 1 < len(depths) and depths[j + 1] > 0):
            j -= 1
    if j < 0:
        return None
    # j holds the opening brace; the signature is the run of lines above (or on) it
    k = j
    while k > 0 and lines[k - 1].strip() and not lines[k - 1].lstrip().startswith(("#", "//")) and \
            not lines[k - 1].rstrip().endswith((";", "}", "*/")):
        k -= 1
    return k


def plan(path, evidence):
    lines = open(path, newline="").read().split("\n")
    eff = effective_lines(lines)
    depths = brace_depths(lines)
    pos = collections.defaultdict(list)
    for i, e in enumerate(eff):
        pos[e].append(i)
    ev = []
    for ours, retail, fn in evidence:
        cands = [i for i in pos.get(ours, []) if MACRO.search(lines[i])]
        if len(cands) != 1:
            continue
        ev.append((cands[0], retail - ours, fn, ours, retail))
    ev = sorted(set(ev))
    groups = []
    for e in ev:
        if groups and groups[-1][0][1] == e[1]:
            groups[-1].append(e)
        else:
            groups.append([e])
    edits = []  # (index, delta) boundaries
    prev_end, prev_delta = -1, 0
    for g in groups:
        first, delta = g[0][0], g[0][1]
        if delta == prev_delta:
            prev_end = g[-1][0]
            continue
        start = function_start(lines, first, depths)
        if start is None or start <= prev_end:
            start = first
        edits.append((start, delta, g))
        prev_end, prev_delta = g[-1][0], delta
    return lines, eff, edits, ev


def apply(path, lines, eff, edits):
    out, bounds = [], {s: d for s, d, _ in edits}
    delta = 0
    for i, l in enumerate(lines):
        if i in bounds:
            delta = bounds[i]
            out.append("#line %d" % (eff[i] + delta))
        m = re.match(r"(\s*#\s*line\s+)(\d+)(.*)", l)
        if m and delta:
            l = "%s%d%s" % (m.group(1), int(m.group(2)) + delta, m.group(3))
        out.append(l)
    open(path, "w", newline="").write("\n".join(out))


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("reports", nargs="+")
    ap.add_argument("--file", action="append", help="only these sources (tree-relative)")
    ap.add_argument("--manifest", default=os.path.join(TREE, "devtools", "bytematch", "manifests", "retail-win32.json"))
    ap.add_argument("--apply", action="store_true")
    a = ap.parse_args()
    mods = json.load(open(a.manifest))["modules"]
    by_stem = collections.defaultdict(set)
    for m in mods.values():
        for s in m["sources"]:
            if s["lang"] in ("c", "c++"):
                by_stem[os.path.splitext(os.path.basename(s["path"]))[0].lower()].add(s["path"])
    evidence = collections.defaultdict(set)
    for r in a.reports:
        for f in json.load(open(r))["functions"]:
            if f.get("status") != "differs" or not f.get("line_constants"):
                continue
            stem = os.path.basename(f.get("ours_object") or f.get("object") or "")
            stem = re.sub(r"(\.(cpp|cxx|cc|c))?(\.\d+)?\.(o|obj)$", "", stem, flags=re.I).lower()
            for p in by_stem.get(stem, ()):
                for lc in f["line_constants"]:
                    evidence[p].add((lc["ours"], lc["retail"], f["name"]))
    for p in sorted(evidence):
        if a.file and p not in a.file:
            continue
        path = os.path.join(TREE, p)
        if not os.path.exists(path):
            continue
        lines, eff, edits, ev = plan(path, evidence[p])
        if not edits:
            continue
        print("%s: %d usable pairs" % (p, len(ev)))
        for s, d, g in edits:
            print("  before line %d (%s): #line %d  [offset %+d; %s]" % (
                s + 1, lines[s].strip()[:60], eff[s] + d, d, ", ".join("%d->%d" % (o, r) for _, _, _, o, r in g[:4])))
        if a.apply:
            apply(path, lines, eff, edits)


if __name__ == "__main__":
    sys.exit(main())
