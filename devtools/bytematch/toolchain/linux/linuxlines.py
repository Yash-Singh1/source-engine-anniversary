#!/usr/bin/env python3
"""List retail Linux line-number evidence from report-linux.py results.

Retail Linux modules have a Telemetry zone at every VPROF, which carries its __LINE__, and the
engine-side modules (depot 224, May 2025) are a later build than the macOS/Windows ones (Nov
2024), so a function can differ from retail Linux only in those constants. This collects
(our line, retail line) pairs from the reports' "line constants only" functions, per source
file and offset. The tree keeps one numbering for every platform, so where the offsets come
from a source revision after the macOS/Windows build they are evidence of that revision, not
something to renumber for Linux alone.

usage: linuxlines.py REPORT_DIR [...] [--file SRC ...]
"""
import argparse, collections, glob, json, os, re

HERE = os.path.dirname(os.path.abspath(__file__))
TREE = os.path.abspath(os.path.join(HERE, '..', '..', '..', '..'))


def source_of(obj):
    m = re.search(r'/objs-[^/]+/(.+)\.\d+\.o$', obj)
    return m.group(1) if m else None


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('reports', nargs='+')
    ap.add_argument('--file', action='append')
    a = ap.parse_args()
    pairs = collections.defaultdict(lambda: collections.defaultdict(set))
    funcs = collections.defaultdict(dict)
    for d in a.reports:
        for p in glob.glob(os.path.join(d, '*.json')):
            if os.path.basename(p).startswith('retail-linux32'):
                continue
            for f in json.load(open(p)).get('functions', []):
                if f.get('status') != 'differs' or not f.get('line_constants') or \
                        not (f.get('reason') or '').startswith('line constants only'):
                    continue
                src = source_of(f['ours_object'])
                if not src or (a.file and src not in a.file):
                    continue
                funcs[src][f['name']] = f['ours_size']
                for lc in f['line_constants']:
                    pairs[src][lc['ours']].add(lc['retail'])
    total = [0, 0]
    for src in sorted(pairs, key=lambda s: -sum(funcs[s].values())):
        offs = collections.Counter(r - o for o, rs in pairs[src].items() for r in rs)
        n, b = len(funcs[src]), sum(funcs[src].values())
        total[0] += n
        total[1] += b
        print('%-50s %3d functions %7d bytes  offsets %s' % (src, n, b,
              ', '.join('%+d x%d' % kv for kv in sorted(offs.items()))))
    print('total: %d functions, %d bytes' % tuple(total))


if __name__ == '__main__':
    main()
