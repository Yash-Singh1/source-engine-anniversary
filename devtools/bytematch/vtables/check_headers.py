#!/usr/bin/env python3
"""Compare our headers' vtable layouts against the retail layouts in interfaces/*.json.

For each interface, a probe translation unit includes the declaring header,
derives an empty class from the interface and lets clang print the vtable
layout (-fdump-vtable-layouts) for an i386-apple-macos10.9 target. Slot
signatures are canonicalised through the mangled names from clang's JSON AST,
so they are compared in the same demangled form as the retail symbols.

A slot matches when the method name, parameter types and const qualifier
agree. Destructor slots match any destructor. Exit status is 1 when any
checked interface mismatches; `--only` limits the run, `--json` writes the
full result.

Probe settings per interface (extra includes, header to use, skip reason) live
in overrides.json under the interface name: {"includes": [...], "header": ...,
"skip": "why"}.

Usage: check_headers.py [--only NAME ...] [--json OUT] [--verbose]
"""
import argparse
import concurrent.futures
import glob
import json
import os
import re
import sys
import tempfile

import vtlayout

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, '..', '..', '..'))
IFACE_DIR = os.path.join(HERE, 'interfaces')


def norm(sig):
    return re.sub(r'\s+', '', sig or '')


def compare(retail, ours):
    """-> list of mismatch dicts (empty when identical)."""
    issues = []
    n = max(len(retail), len(ours))
    for i in range(n):
        r = retail[i] if i < len(retail) else None
        o = ours[i] if i < len(ours) else None
        if r is None:
            issues.append({'index': i, 'kind': 'extra-in-ours', 'ours': o['sig']})
            continue
        if o is None:
            issues.append({'index': i, 'kind': 'missing-in-ours', 'retail': r['sig']})
            continue
        rs = r['sig'] or ''
        if rs == '__cxa_pure_virtual':
            continue
        if o.get('dtor') or rs.startswith('~'):
            if not (o.get('dtor') and rs.startswith('~')):
                issues.append({'index': i, 'kind': 'differs', 'retail': rs, 'ours': o['sig']})
            continue
        if norm(rs) != norm(o['sig']):
            kind = 'signature' if rs.split('(')[0] == o['sig'].split('(')[0] else 'differs'
            issues.append({'index': i, 'kind': kind, 'retail': rs, 'ours': o['sig']})
    return issues


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--only', nargs='*')
    ap.add_argument('--json')
    ap.add_argument('--verbose', '-v', action='store_true')
    ap.add_argument('-j', type=int, default=os.cpu_count())
    a = ap.parse_args()
    overrides = {}
    op = os.path.join(HERE, 'overrides.json')
    if os.path.exists(op):
        overrides = json.load(open(op))

    recs = []
    for f in sorted(glob.glob(os.path.join(IFACE_DIR, '*.json'))):
        rec = json.load(open(f))
        if a.only and rec['interface'] not in a.only:
            continue
        recs.append(rec)

    workdir = tempfile.mkdtemp(prefix='bm-vtcheck-')

    def run_one(rec):
        name = rec['interface']
        ov = overrides.get(name, {})
        res = {'interface': name}
        if ov.get('skip'):
            res['status'] = 'skipped'
            res['why'] = ov['skip']
            return res
        if 'slots' not in rec:
            res['status'] = 'no-retail'
            return res
        headers = [ov['header']] if ov.get('header') else rec['headers']
        if not headers:
            res['status'] = 'no-header'
            return res
        err = None
        for h in headers:
            ours, err = vtlayout.layout(ROOT, name, h, ov.get('includes', []), workdir)
            if ours is not None:
                res['header'] = h
                break
        if ours is None:
            res['status'] = 'probe-failed'
            res['error'] = err
            return res
        issues = compare(rec['slots'], ours)
        res['retail_len'] = len(rec['slots'])
        res['ours_len'] = len(ours)
        res['issues'] = issues
        res['status'] = 'ok' if not issues else 'mismatch'
        return res

    with concurrent.futures.ThreadPoolExecutor(a.j) as ex:
        results = list(ex.map(run_one, recs))

    bad = 0
    for res in results:
        st = res['status']
        if st == 'mismatch':
            bad += 1
            first = res['issues'][0]
            print(f"MISMATCH {res['interface']}: retail {res['retail_len']} slots, ours {res['ours_len']}; "
                  f"{len(res['issues'])} differing slots, first at {first['index']}: "
                  f"retail={first.get('retail')} ours={first.get('ours')}")
            if a.verbose:
                for it in res['issues']:
                    print(f"    [{it['index']}] {it['kind']}: retail={it.get('retail')} ours={it.get('ours')}")
        elif st == 'probe-failed':
            bad += 1
            print(f"PROBE-FAILED {res['interface']}: {res.get('error', '').splitlines()[0] if res.get('error') else ''}")
        elif a.verbose:
            print(f"{st.upper()} {res['interface']}" + (f" ({res.get('why')})" if res.get('why') else ''))
    ok = sum(1 for r in results if r['status'] == 'ok')
    print(f'\n{ok} ok, {bad} failing, {len(results) - ok - bad} skipped/no data, of {len(results)}')
    if a.json:
        json.dump(results, open(a.json, 'w'), indent=1)
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main())
