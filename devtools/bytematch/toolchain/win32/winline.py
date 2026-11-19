#!/usr/bin/env python3
"""winline.py FILE OURS:RETAIL[,OURS:RETAIL...] [--from N --to M] [--dry]
winline.py --check [FILE...]

Give the lines whose effective __LINE__ is OURS the number RETAIL in MSVC retail builds only.
Each run of consecutive targets with the same offset gets, before its first line,

    #if defined( _MSC_VER ) && defined( BYTEMATCH_RETAIL )
    #line RETAIL-3
    #else
    #line OURS-1
    #endif

and after its last line an unconditional `#line` with the next line's current number, so no
other line's number changes on any platform. --from/--to limit the physical range searched
(an effective number can repeat after a #line reset).

Both `#line OURS-1` and the trailing `#line` are absolute, so they go stale when lines above
them are added or removed later (a merge). --check verifies every such block in the given
files (default: every tracked source containing one): in the non-MSVC view the `#else` branch's
`#line` must equal the `#if` line's own number minus one, and the trailing `#line` must equal
the number the line after the run would have had anyway. Exits 1 on a stale block."""
import os, re, subprocess, sys

LINE = re.compile(r'^\s*#\s*line\s+(\d+)')
MSVC_IF = re.compile(r'^\s*#\s*if\s+defined\(\s*_MSC_VER\s*\)\s*&&\s*defined\(\s*BYTEMATCH_RETAIL\s*\)\s*$')
COND = re.compile(r'^\s*#\s*(if|ifdef|ifndef|elif|else|endif)\b')


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


def check(path):
    lines = open(path, newline='').read().split('\n')
    num = other_numbers(lines)
    bad = 0
    for i in range(len(lines) - 5):
        s = [l.rstrip('\r') for l in lines[i:i + 5]]
        if not (MSVC_IF.match(s[0]) and LINE.match(s[1]) and re.match(r'^\s*#\s*else\s*$', s[2])
                and LINE.match(s[3]) and re.match(r'^\s*#\s*endif\s*$', s[4])):
            continue
        # winline.py's blocks end in an unconditional restore before any other conditional;
        # hand-written blocks without one shift the numbering on purpose and are left alone
        end = None
        for j in range(i + 5, min(len(lines), i + 60)):
            t = lines[j].rstrip('\r')
            if COND.match(t):
                break
            if LINE.match(t):
                end = j
                break
        if end is None:
            continue
        b = int(LINE.match(s[3]).group(1))
        if b != num[i] - 1:
            print('%s:%d: stale #line %d in the #else branch, want %d' % (path, i + 4, b, num[i] - 1))
            bad += 1
        c = int(LINE.match(lines[end]).group(1))
        if c != num[end - 1] + 1:
            print('%s:%d: stale trailing #line %d, want %d' % (path, end + 1, c, num[end - 1] + 1))
            bad += 1
    return bad


def check_main(files):
    if not files:
        top = subprocess.run(['git', 'rev-parse', '--show-toplevel'], capture_output=True, text=True).stdout.strip()
        r = subprocess.run(['git', '-C', top, 'grep', '-l', '-E', r'^#if defined\( _MSC_VER \) && defined\( BYTEMATCH_RETAIL \)',
                            '--', '*.cpp', '*.h', '*.c', '*.inl'], capture_output=True, text=True)
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
    nl = '\r\n' if lines and lines[0].endswith('\r') else '\n'
    cr = '\r' if nl == '\r\n' else ''
    for run in reversed(runs):
        first, last = run[0], run[-1]
        after = eff[last[0]] + 1
        lines[last[0] + 1:last[0] + 1] = ['#line %d%s' % (after, cr)]
        # skipped lines count: #else, the other #line and #endif follow the first #line, #endif the second
        lines[first[0]:first[0]] = [s + cr for s in ('#if defined( _MSC_VER ) && defined( BYTEMATCH_RETAIL )',
                                                    '#line %d' % (first[2] - 3), '#else', '#line %d' % (first[1] - 1), '#endif')]
        print('%s: lines %d..%d (now %d..%d) -> %d..%d' % (path, first[0] + 1, last[0] + 1, first[1], last[1], first[2], last[2]))
    if '--dry' not in sys.argv:
        open(path, 'w', newline='').write('\n'.join(lines))


if __name__ == '__main__':
    main()
