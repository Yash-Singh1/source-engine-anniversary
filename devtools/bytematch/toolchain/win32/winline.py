#!/usr/bin/env python3
"""winline.py FILE OURS:RETAIL[,OURS:RETAIL...] [--from N --to M] [--dry]

Give the lines whose effective __LINE__ is OURS the number RETAIL in MSVC retail builds only.
Each run of consecutive targets with the same offset gets, before its first line,

    #if defined( _MSC_VER ) && defined( BYTEMATCH_RETAIL )
    #line RETAIL-3
    #else
    #line OURS-1
    #endif

and after its last line an unconditional `#line` with the next line's current number, so no
other line's number changes on any platform. --from/--to limit the physical range searched
(an effective number can repeat after a #line reset)."""
import re, sys

LINE = re.compile(r'^\s*#\s*line\s+(\d+)')


def effective(lines):
    out, cur = [], 1
    for l in lines:
        m = LINE.match(l)
        out.append(None if m else cur)
        cur = int(m.group(1)) if m else cur + 1
    return out


def main():
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
