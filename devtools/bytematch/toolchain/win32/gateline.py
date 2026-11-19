#!/usr/bin/env python3
"""gateline.py FILE LINE[,LINE...] [--dry]
   gateline.py FILE LINE --retail-line N --retail-text TEXT [--dry]

First form: leave the given physical lines out of retail builds without changing any other
line's __LINE__. Each line becomes

    #ifndef BYTEMATCH_RETAIL
    <line>
    #endif
    #line M

where M is the next line's current number. Second form: MSVC retail builds compile TEXT as
line N instead of the given line (Windows retail evidence, e.g. a telemetry zone's format):

    #if defined( _MSC_VER ) && defined( BYTEMATCH_RETAIL )
    #line N
    TEXT
    #else
    <line>
    #endif
    #line M

Inside a winline.py run, where MSVC retail builds number lines differently, the restoring
#line is itself split on _MSC_VER && BYTEMATCH_RETAIL."""
import re, sys

LINE = re.compile(r'^\s*#\s*line\s+(\d+)')
MSVC_IF = '#if defined( _MSC_VER ) && defined( BYTEMATCH_RETAIL )'
MSVC_IF_RE = re.compile(r'^\s*#\s*if\s+defined\(\s*_MSC_VER\s*\)\s*&&\s*defined\(\s*BYTEMATCH_RETAIL\s*\)\s*$')
ELSE = re.compile(r'^\s*#\s*else\b')
ENDIF = re.compile(r'^\s*#\s*endif\b')


def effective(lines):
    """Per physical line: (number in MSVC retail builds, number elsewhere), None for #line lines."""
    out, cur, branch = [], [1, 1], None   # branch: None, 'msvc' or 'other' inside an MSVC_IF block
    for l in lines:
        s = l.rstrip('\r')
        if branch is None and MSVC_IF_RE.match(s):
            branch = 'msvc'
        elif branch == 'msvc' and ELSE.match(s):
            branch = 'other'
        elif branch is not None and ENDIF.match(s):
            branch = None
        m = LINE.match(s)
        out.append(None if m else tuple(cur))
        if m:
            # a #line in the branch a build skips is an ordinary line for that build
            n = int(m.group(1))
            cur[0] = n if branch in (None, 'msvc') else cur[0] + 1
            cur[1] = n if branch in (None, 'other') else cur[1] + 1
        else:
            cur = [cur[0] + 1, cur[1] + 1]
    return out


def arg(name):
    return sys.argv[sys.argv.index(name) + 1] if name in sys.argv else None


def main():
    path = sys.argv[1]
    want = sorted({int(x) for x in sys.argv[2].split(',')})
    rline, rtext = arg('--retail-line'), arg('--retail-text')
    if (rline is None) != (rtext is None) or (rline and len(want) != 1):
        sys.exit('--retail-line and --retail-text go together, with one LINE')
    lines = open(path, newline='').read().split('\n')
    eff = effective(lines)
    cr = '\r' if lines and lines[0].endswith('\r') else ''
    for n in reversed(want):
        i = n - 1
        nxt = eff[i + 1] if i + 1 < len(eff) else None
        if eff[i] is None or nxt is None:
            sys.exit('%s:%d: next to a #line directive, edit by hand' % (path, n))
        if nxt[0] == nxt[1]:
            restore = ['#line %d' % nxt[0]]
        else:
            restore = [MSVC_IF, '#line %d' % nxt[0], '#else', '#line %d' % nxt[1], '#endif']
        if rline:
            new = [MSVC_IF, '#line %s' % rline, rtext, '#else', lines[i].rstrip('\r'), '#endif']
        else:
            new = ['#ifndef BYTEMATCH_RETAIL', lines[i].rstrip('\r'), '#endif']
        lines[i:i + 1] = [s + cr for s in new + restore]
        print('%s:%d: %s' % (path, n, lines[i + (4 if rline else 1)].strip()[:100]))
    if '--dry' not in sys.argv:
        open(path, 'w', newline='').write('\n'.join(lines))


if __name__ == '__main__':
    main()
