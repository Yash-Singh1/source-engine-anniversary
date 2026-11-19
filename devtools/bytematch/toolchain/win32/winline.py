#!/usr/bin/env python3
"""winline.py FILE OURS:RETAIL[,OURS:RETAIL...] [--from N --to M] [--dry]
winline.py --check [FILE...]

Give the lines whose effective __LINE__ is OURS the number RETAIL, on every platform (Valve built
all of them from one file; retail Windows and Linux Telemetry zones carry these numbers, macOS
compiles Telemetry out). Each run of consecutive targets with the same offset gets, before its
first line,

    #line RETAIL

and after its last line a `#line` with the number the next line has without the first one, so
no other line's number changes. --from/--to limit the physical range searched (an effective
number can repeat after a #line reset).

The trailing `#line` is absolute, so it goes stale when lines above it are added or removed later
(a merge): the lines below keep their old numbers and the edit's shift is silently cancelled.
--check verifies every such run in the given files (default: every tracked source with a #line):
a `#line` after an ordinary line and before a Telemetry call or VPROF, followed by up to 60
ordinary lines and then another `#line` before any conditional, is a run, and the second `#line`
must equal the number its next line would have without the first one. Runs written by older
versions of this script inside
`#if defined( _MSC_VER ) && defined( BYTEMATCH_RETAIL )` blocks are checked as before: the `#else`
branch's `#line` must be the `#if` line's number minus one. Exits 1 on a stale run."""
import os, re, subprocess, sys

LINE = re.compile(r'^\s*#\s*line\s+(\d+)')
DIRECTIVE = re.compile(r'^\s*#')
MSVC_IF = re.compile(r'^\s*#\s*if\s+defined\(\s*_MSC_VER\s*\)\s*&&\s*defined\(\s*BYTEMATCH_RETAIL\s*\)\s*$')
COND = re.compile(r'^\s*#\s*(if|ifdef|ifndef|elif|else|endif)\b')
# Telemetry calls and VPROFs (a Telemetry zone in Windows and Linux retail builds) carry __LINE__
ZONE = re.compile(r'^\s*(tm[A-Z]\w*|VPROF\w*)\s*\(')
WINDOW = 60


def effective(lines):
    out, cur = [], 1
    for l in lines:
        m = LINE.match(l)
        out.append(None if m else cur)
        cur = int(m.group(1)) if m else cur + 1
    return out


def other_numbers(lines):
    """Per physical line, its number in non-MSVC builds (#line lines included), as gateline.py
    counts: a #line in the MSVC branch of an MSVC_IF block is an ordinary line there."""
    out, cur, branch = [], 1, None
    for l in lines:
        s = l.rstrip('\r')
        if branch is None and MSVC_IF.match(s):
            branch = 'msvc'
        elif branch == 'msvc' and re.match(r'^\s*#\s*else\b', s):
            branch = 'other'
        elif branch is not None and re.match(r'^\s*#\s*endif\b', s):
            branch = None
        out.append(cur)
        m = LINE.match(s)
        cur = int(m.group(1)) if m and branch != 'msvc' else cur + 1
    return out


def restore_after(lines, start):
    """Index of the #line ending the run that starts at line `start`, or None."""
    for j in range(start, min(len(lines), start + WINDOW)):
        t = lines[j].rstrip('\r')
        if COND.match(t):
            return None
        if LINE.match(t):
            return j
    return None


def check(path):
    lines = open(path, newline='').read().split('\n')
    num = other_numbers(lines)
    s = [l.rstrip('\r') for l in lines]
    bad = 0
    i = 0
    while i < len(s) - 2:
        # older form: an MSVC-only pair, the #else branch must keep the #if line's number
        if MSVC_IF.match(s[i]) and i + 4 < len(s) and LINE.match(s[i + 1]) and re.match(r'^\s*#\s*else\s*$', s[i + 2]) \
                and LINE.match(s[i + 3]) and re.match(r'^\s*#\s*endif\s*$', s[i + 4]):
            end = restore_after(lines, i + 5)
            if end is not None:
                b = int(LINE.match(s[i + 3]).group(1))
                if b != num[i] - 1:
                    print('%s:%d: stale #line %d in the #else branch, want %d' % (path, i + 4, b, num[i] - 1))
                    bad += 1
                c = int(LINE.match(s[end]).group(1))
                if c != num[end - 1] + 1:
                    print('%s:%d: stale trailing #line %d, want %d' % (path, end + 1, c, num[end - 1] + 1))
                    bad += 1
            i += 5
            continue
        # a run: <ordinary line> #line W <zone line> <ordinary lines> #line M
        if LINE.match(s[i]) and i > 0 and not DIRECTIVE.match(s[i - 1]) and ZONE.match(s[i + 1]):
            end = restore_after(lines, i + 1)
            if end is not None and all(not DIRECTIVE.match(t) for t in s[i + 1:end]):
                want = num[i - 1] + (end - i - 1) + 1
                c = int(LINE.match(s[end]).group(1))
                if c != want:
                    print('%s:%d: stale trailing #line %d, want %d' % (path, end + 1, c, want))
                    bad += 1
                i = end
                continue
        i += 1
    return bad


def check_main(files):
    if not files:
        top = subprocess.run(['git', 'rev-parse', '--show-toplevel'], capture_output=True, text=True).stdout.strip()
        r = subprocess.run(['git', '-C', top, 'grep', '-l', '-E', r'^\s*#\s*line [0-9]',
                            '--', '*.cpp', '*.h', '*.c', '*.inl', ':!devtools'], capture_output=True, text=True)
        files = [os.path.join(top, f) for f in r.stdout.split()]
    bad = sum(check(f) for f in files)
    print('%d file(s) checked, %d stale #line(s)' % (len(files), bad))
    sys.exit(1 if bad else 0)


def main():
    if sys.argv[1:2] == ['--check']:
        return check_main(sys.argv[2:])
    path, pairs = sys.argv[1], sys.argv[2]
    lo = int(sys.argv[sys.argv.index('--from') + 1]) if '--from' in sys.argv else 1
    hi = int(sys.argv[sys.argv.index('--to') + 1]) if '--to' in sys.argv else 10 ** 9
    want = sorted((int(a), int(b)) for a, b in (p.split(':') for p in pairs.split(',')))
    lines = open(path, newline='').read().split('\n')
    eff = effective(lines)
    phys = []
    for e, r in want:
        hits = [i for i, v in enumerate(eff) if v == e and lo <= i + 1 <= hi]
        if len(hits) != 1:
            sys.exit('%s: effective line %d found at %s; pass --from/--to' % (path, e, [h + 1 for h in hits]))
        phys.append((hits[0], e, r))
    runs = []
    for i, e, r in phys:
        if runs and r - e == runs[-1][-1][2] - runs[-1][-1][1] and i - runs[-1][-1][0] <= 12 \
                and all(eff[k] is not None for k in range(runs[-1][-1][0], i + 1)):
            runs[-1].append((i, e, r))
        else:
            runs.append([(i, e, r)])
    cr = '\r' if lines and lines[0].endswith('\r') else ''
    for run in reversed(runs):
        first, last = run[0], run[-1]
        lines[last[0] + 1:last[0] + 1] = ['#line %d%s' % (eff[last[0]] + 1, cr)]
        lines[first[0]:first[0]] = ['#line %d%s' % (first[2], cr)]
        print('%s: lines %d..%d (now %d..%d) -> %d..%d' % (path, first[0] + 1, last[0] + 1, first[1], last[1], first[2], last[2]))
    if '--dry' not in sys.argv:
        open(path, 'w', newline='').write('\n'.join(lines))


if __name__ == '__main__':
    main()
