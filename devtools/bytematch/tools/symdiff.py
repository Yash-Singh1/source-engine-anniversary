#!/usr/bin/env python3
"""Per-symbol normalized disassembly comparison for i386 Mach-O images/objects.

Usage:
  symdiff.py dump BIN OUT.json [--syms regex]
  symdiff.py cmp A.json B.json [--syms regex] [--show N] [--list]

Normalisation: relative branch targets inside the function become function
offsets; targets outside become EXT; hex constants >= 0x1000 (PIC-relative
displacements, absolute addresses) become X; symbolic <...> comments dropped.
"""
import json, re, subprocess, sys

OBJDUMP = "/opt/homebrew/opt/llvm/bin/llvm-objdump"
BR = re.compile(r"^(j\w+|call\w*|loop\w*)\s+(0x[0-9a-f]+|[0-9a-f]+)\b")
HDR = re.compile(r"^([0-9a-f]+) <(.+)>:$")
INS = re.compile(r"^\s*([0-9a-f]+):\s+((?:[0-9a-f]{2} )+)\s*(.*)$")


def dump(path):
    cmd = [OBJDUMP, "-d", "--arch=i386", "--no-print-imm-hex", "-j", "__text", "-j", "__textcoal_nt", path]
    out = subprocess.run(cmd, capture_output=True, text=True).stdout
    funcs, cur = {}, None
    for line in out.splitlines():
        m = HDR.match(line)
        if m:
            cur = {"addr": int(m.group(1), 16), "ins": []}
            funcs.setdefault(m.group(2), cur)
            continue
        m = INS.match(line)
        if m and cur is not None:
            # objdump may wrap long encodings; continuation lines have no text.
            text = m.group(3).split("#")[0].split("<")[0].strip()
            raw = m.group(2).split()
            if not text and cur["ins"]:
                cur["ins"][-1][1] += raw
                continue
            cur["ins"].append([int(m.group(1), 16), raw, text])
    res = {}
    for name, f in funcs.items():
        ins = f["ins"]
        if not ins:
            continue
        lo = f["addr"]
        hi = ins[-1][0] + len(ins[-1][1])
        norm, raw = [], []
        pic = set()
        prev_call_next = False
        for addr, b, text in ins:
            raw += b
            t = re.sub(r"\s+", " ", text)
            mp = re.match(r"popl (%\w+)$", t)
            if prev_call_next and mp:
                pic.add(mp.group(1))
            prev_call_next = False
            mb = BR.match(t)
            if mb:
                tgt = int(mb.group(2), 16)
                if mb.group(1).startswith("call") and tgt == addr + len(b):
                    prev_call_next = True
                local = lo <= tgt < hi and not mb.group(1).startswith("call")
                t = f"{mb.group(1)} " + (f"L{tgt - lo}" if local else "EXT")
            else:
                for r in pic:
                    t = re.sub(r"-?\d+\(" + re.escape(r) + r"([,)])", "P(" + r + r"\1", t)
                t = re.sub(r"(?<![\w$])(-?)(\d+)(?=[(,\s]|$)",
                           lambda m: "X" if int(m.group(2)) >= 0x1000 else m.group(0), t)
                t = re.sub(r"0x[0-9a-f]{4,}", "X", t)
            norm.append(t)
        res[name] = {"n": norm, "size": hi - lo, "raw": "".join(raw),
                     "ib": ["".join(b) for _, b, _ in ins]}
    return res


def strip_nops(n):
    while n and n[-1].startswith("nop"):
        n = n[:-1]
    return n


def cmp(a, b, rx, show, listing):
    common = sorted(k for k in a if k in b and (not rx or re.search(rx, k)))
    same = samelen = 0
    diffs = []
    for k in common:
        na, nb = strip_nops(a[k]["n"]), strip_nops(b[k]["n"])
        if na == nb:
            same += 1
            if listing:
                print("SAME", k)
        else:
            if len(na) == len(nb):
                samelen += 1
            diffs.append(k)
            if listing:
                print("DIFF", k, len(na), len(nb))
    print(f"common={len(common)} identical={same} ({100*same/max(1,len(common)):.1f}%) "
          f"diff_same_count={samelen} diff={len(diffs)}")
    import difflib
    for k in diffs[:show]:
        print("=" * 20, k)
        for l in difflib.unified_diff(strip_nops(a[k]["n"]), strip_nops(b[k]["n"]), lineterm="", n=2):
            print(l)


if __name__ == "__main__":
    args = sys.argv[1:]
    rx = None
    show = 0
    listing = "--list" in args
    if "--syms" in args:
        rx = args[args.index("--syms") + 1]
    if "--show" in args:
        show = int(args[args.index("--show") + 1])
    if args[0] == "dump":
        d = dump(args[1])
        if rx:
            d = {k: v for k, v in d.items() if re.search(rx, k)}
        json.dump(d, open(args[2], "w"))
        print(len(d), "functions")
    else:
        cmp(json.load(open(args[1])), json.load(open(args[2])), rx, show, listing)
