#!/usr/bin/env python3
"""Build the cross-module interface inventory from extract_retail.py output.

An interface is any class that retail registers through CreateInterface, or
that retail uses as a (transitive) base class and that our tree declares in a
public header (public/, common/). For each one this writes
interfaces/<Name>.json with the retail vtable order recovered from the macOS
i386 dylibs, plus the interface version strings seen in retail, in our tree
and in Source SDK 2013 SP. inventory.json summarises all of them.

Retail vtables mostly belong to implementing classes, so an interface's own
slot count is taken from, in order:
  1. the interface's own vtable when retail emits one,
  2. an entry in overrides.json ("retail_len"),
  3. the smallest of these bounds on the reference implementer's subtable:
     - the shortest subtable for that base over every retail class deriving
       from it,
     - the first destructor slot the interface itself doesn't declare (per
       the SDK 2013 header, else ours),
     - the first own method of the next class in the retail base chain, as
       declared in SDK 2013 headers.
Our own headers never decide the retail order; at most they hint whether the
interface declares a virtual destructor when SDK 2013 lacks the class.

Usage: inventory.py [--cache DIR] [--sdk DIR]
"""
import argparse
import collections
import concurrent.futures
import glob
import json
import os
import re
import subprocess
import tempfile

import vtlayout

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, '..', '..', '..'))
DEFAULT_CACHE = os.path.join(HERE, '.cache', 'retail')
DEFAULT_SDK = '/Volumes/Storage/sco/source-sdk-2013/src'
OUT_DIR = os.path.join(HERE, 'interfaces')
HEADER_DIRS = ('public', 'common')

DECL = re.compile(r'^\s*(?:abstract_class|class|struct)\s+(?:DLL_CLASS_EXPORT\s+|PLATFORM_CLASS\s+|VGUI_EXPORT\s+|ALIGN16\s+)?'
                  r'([A-Za-z_]\w*)\s*(?::[^;]*)?(?:\{.*)?$')
DEFINE = re.compile(r'^\s*#\s*define\s+(\w+)\s+"([^"]+)"')


def method_sig(symbol):
    """'CEngineClient::GetScreenSize(int&, int&) const' -> 'GetScreenSize(int&, int&) const'."""
    if symbol is None:
        return None
    if symbol == '__cxa_pure_virtual':
        return symbol
    s = symbol.split('/', 1)[1] if re.match(r'^lib\w+/', symbol) else symbol
    depth = 0
    paren = None
    for i, ch in enumerate(s):
        if ch == '<':
            depth += 1
        elif ch == '>':
            depth -= 1
        elif ch == '(' and depth == 0:
            paren = i
            break
    head = s[:paren] if paren is not None else s
    depth, cut = 0, 0
    for i, ch in enumerate(head):
        if ch == '<':
            depth += 1
        elif ch == '>':
            depth -= 1
        elif ch == ':' and depth == 0 and head[i:i + 2] == '::':
            cut = i + 2
    return s[cut:]


def method_name(sig):
    if sig is None:
        return None
    return sig.split('(')[0]


def scan_decls(root, dirs):
    decl = collections.defaultdict(set)
    for d in dirs:
        for dp, _, fs in os.walk(os.path.join(root, d)):
            for f in fs:
                if not f.endswith('.h'):
                    continue
                p = os.path.join(dp, f)
                try:
                    lines = open(p, encoding='latin-1').read().splitlines()
                except OSError:
                    continue
                for line in lines:
                    m = DECL.match(line)
                    if m:
                        decl[m.group(1)].add(os.path.relpath(p, root))
    return decl


def scan_defines(root, dirs):
    """version string -> sorted [(macro, file)] for quoted #defines."""
    out = collections.defaultdict(set)
    cmd = ['rg', '-n', '--no-heading', '-g', '*.h', '-g', '*.cpp', r'^\s*#\s*define\s+\w+\s+"[A-Za-z_][\w]*\d\d\d"'] + \
          [os.path.join(root, d) for d in dirs if os.path.isdir(os.path.join(root, d))]
    for line in subprocess.run(cmd, capture_output=True, text=True, errors='replace').stdout.splitlines():
        f, _, rest = line.split(':', 2)
        m = DEFINE.match(rest)
        if m:
            out[m.group(2)].add((m.group(1), os.path.relpath(f, root)))
    return out


def version_family(v):
    m = re.match(r'^(.*?)(\d{3})$', v or '')
    return m.group(1) if m else None


def load_retail(cache):
    mods = {}
    for f in sorted(glob.glob(os.path.join(cache, '*.json'))):
        d = json.load(open(f))
        mods[d['module']] = d
    return mods


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--cache', default=DEFAULT_CACHE)
    ap.add_argument('--sdk', default=DEFAULT_SDK)
    ap.add_argument('-j', type=int, default=os.cpu_count())
    a = ap.parse_args()

    mods = load_retail(a.cache)
    op = os.path.join(HERE, 'overrides.json')
    overrides = json.load(open(op)) if os.path.exists(op) else {}

    bases = {}
    for m, d in mods.items():
        for c, v in d['classes'].items():
            if 'bases' in v and c not in bases:
                bases[c] = v['bases']

    def ancestors(c, off=0, path=(), seen=None):
        seen = seen if seen is not None else []
        path = path + (c,)
        seen.append((c, off, path))
        for b in bases.get(c, []):
            if b[0] and len(b) == 2:
                ancestors(b[0], off + b[1], path, seen)
        return seen

    # derived[N] = [(module, class, offset, path from class down to N)]
    derived = collections.defaultdict(list)
    for m, d in mods.items():
        for c, v in d['classes'].items():
            if not v['tables']:
                continue
            first = {}
            for anc, off, path in ancestors(c):
                first.setdefault(anc, (off, path))
            for anc, (off, path) in first.items():
                derived[anc].append((m, c, off, path))

    regs = collections.defaultdict(list)
    for m, d in mods.items():
        for r in d['interfaces']:
            if r['interface']:
                iface = r['interface']
                # create-function names drop namespaces (vgui2 registers vgui::IPanel as "IPanel")
                if any(x[1] == r['class'] for x in derived.get('vgui::' + iface, ())):
                    iface = 'vgui::' + iface
                regs[iface].append({'module': m, 'version': r['version'], 'class': r['class']})

    decl = scan_decls(ROOT, HEADER_DIRS)
    sdk_decl = scan_decls(a.sdk, HEADER_DIRS)
    ours_defs = scan_defines(ROOT, ('public', 'common', 'engine', 'materialsystem', 'vgui2', 'filesystem', 'inputsystem',
                                    'datacache', 'studiorender', 'vphysics', 'gameui', 'serverbrowser', 'game'))
    sdk_defs = scan_defines(a.sdk, ('public', 'common', 'game'))
    namespaces = set()
    for f in {f for fs in decl.values() for f in fs}:
        for line in open(os.path.join(ROOT, f), encoding='latin-1'):
            m = re.match(r'^\s*namespace\s+(\w+)', line)
            if m:
                namespaces.add(m.group(1))

    def public_headers(n, table):
        parts = n.split('::')
        if len(parts) > 2 or (len(parts) == 2 and parts[0] not in namespaces):
            return []
        return sorted(f for f in table.get(parts[-1], ()) if f.startswith(HEADER_DIRS))

    names = set(regs)
    for n in derived:
        if '<' not in n and public_headers(n, decl):
            names.add(n)

    def subtable(m, c, off):
        for t in mods[m]['classes'][c]['tables']:
            if t['offset_to_top'] == -off:
                return t['slots']
        return None

    # SDK 2013 (and, for destructor hints only, our) layouts of every interface
    # and of the class following it in each retail base chain
    want = set(names)
    for n in names:
        for m, c, off, path in derived.get(n, []):
            want.update(p for p in path[:-1] if '<' not in p)
    # game entity classes need the whole game include context; their chains rely on the destructor bound
    sdk_chain_decl = scan_decls(a.sdk, HEADER_DIRS + ('materialsystem', 'filesystem', 'vgui2'))
    want = {n for n in want if n in names or n.split('::')[-1] in sdk_chain_decl}
    workdir = tempfile.mkdtemp(prefix='bm-inventory-')

    def probe(root, table, n):
        ov = overrides.get(n, {})
        if table is sdk_decl and n not in names:
            hs = sorted(sdk_chain_decl.get(n.split('::')[-1], ()))
        else:
            hs = public_headers(n, table)
        for h in hs:
            slots, _ = vtlayout.layout(root, n, h, ov.get('includes', []), workdir)
            if slots is not None:
                return slots
        return None

    with concurrent.futures.ThreadPoolExecutor(a.j) as ex:
        sdk_l = dict(zip(sorted(want), ex.map(lambda n: probe(a.sdk, sdk_decl, n), sorted(want))))
        ours_l = dict(zip(sorted(names), ex.map(lambda n: probe(ROOT, decl, n), sorted(names))))

    os.makedirs(OUT_DIR, exist_ok=True)
    for f in glob.glob(os.path.join(OUT_DIR, '*.json')):
        os.remove(f)
    inventory = {}
    for n in sorted(names):
        headers = public_headers(n, decl)
        rec = {'interface': n, 'headers': headers}

        rver = sorted({r['version'] for r in regs.get(n, []) if r['version']})
        fams = {version_family(v) for v in rver} - {None}
        rec['versions'] = {
            'retail': rver,
            'ours': sorted({v for v in ours_defs if version_family(v) in fams}),
            'sdk2013': sorted({v for v in sdk_defs if version_family(v) in fams}),
        }
        rec['version_macros'] = sorted({mac for v in rec['versions']['ours'] for mac, _ in ours_defs[v]})
        rec['registrations'] = regs.get(n, [])
        sdk = sdk_l.get(n)
        rec['sdk2013_len'] = len(sdk) if sdk is not None else None
        if sdk is not None:
            rec['sdk2013_slots'] = [x['sig'] for x in sdk]

        cands = []
        for m, c, off, path in derived.get(n, []):
            st = subtable(m, c, off)
            if st is not None:
                cands.append((m, c, off, st, path))
        own = [x for x in cands if x[1] == n]
        reg_classes = {(r['module'], r['class']) for r in regs.get(n, [])}
        if not cands:
            rec['status'] = 'no retail vtable'
            inventory[n] = {k: rec[k] for k in ('headers', 'versions', 'status')}
            json.dump(rec, open(os.path.join(OUT_DIR, n.replace('::', '__') + '.json'), 'w'), indent=1)
            continue
        shortest = min(len(x[3]) for x in cands)

        def pure_count(x):
            return sum(1 for s in x[3][:shortest] if s == '__cxa_pure_virtual')
        concrete = [x for x in cands if (x[0], x[1]) in reg_classes]
        ref = own[0] if own else min(concrete or cands, key=lambda x: (pure_count(x), len(x[3]), x[0] != 'bin/engine'))
        slots = ref[3]
        sigs = [method_sig(s) for s in slots]

        bounds = {}
        bounds['shortest deriving subtable'] = min(len(x[3]) for x in cands)
        hint = sdk if sdk is not None else ours_l.get(n)
        if hint is not None:
            own_dtors = sum(1 for s in hint if s.get('dtor'))
            seen = 0
            for i, s in enumerate(sigs):
                if s and s.startswith('~'):
                    seen += 1
                    if seen > own_dtors:
                        bounds['first implementer destructor'] = i
                        break
        if sdk is not None:
            n_sigs = {s['sig'] for s in sdk}
            for nxt in reversed(ref[4][:-1]):
                if '<' in nxt:
                    continue
                if not sdk_l.get(nxt):
                    break
                first_own = next((s['sig'] for s in sdk_l[nxt][len(sdk):] if not s.get('dtor') and s['sig'] not in n_sigs), None)
                if first_own:
                    hit = next((i for i, s in enumerate(sigs) if s == first_own), None)
                    if hit is not None:
                        bounds[f'first own method of {nxt}'] = hit
                    break

        if n in overrides and 'retail_len' in overrides[n]:
            length, basis = overrides[n]['retail_len'], 'override: ' + overrides[n].get('why', '')
        elif own:
            length, basis = len(own[0][3]), 'own vtable'
        else:
            basis, length = min(bounds.items(), key=lambda kv: kv[1])
        rec['retail_reference'] = {'module': ref[0], 'class': ref[1], 'offset': ref[2], 'chain': list(ref[4])}
        rec['retail_len'] = length
        rec['retail_len_basis'] = basis
        rec['retail_len_bounds'] = bounds
        rec['slots'] = [{'index': i, 'name': method_name(sigs[i]), 'sig': sigs[i], 'symbol': s}
                        for i, s in enumerate(slots[:length])]
        if len(slots) > length:
            rec['implementer_tail'] = sigs[length:]
        disagree = []
        for m, c, off, st, _ in cands:
            got = [method_sig(s) for s in st[:length]]
            for i, (x, y) in enumerate(zip(sigs[:length], got)):
                if x != y and '__cxa_pure_virtual' not in (x, y) and not (x and y and x.startswith('~') and y.startswith('~')):
                    disagree.append({'module': m, 'class': c, 'index': i, 'ref': x, 'got': y})
                    break
        if disagree:
            rec['derived_disagreements'] = disagree[:20]
        rec['derived'] = sorted({f'{m}:{c}' for m, c, *_ in cands})[:40]
        rec['derived_count'] = len({(m, c) for m, c, *_ in cands})
        inventory[n] = {'headers': headers, 'versions': rec['versions'], 'retail_len': length,
                        'sdk2013_len': rec['sdk2013_len'], 'retail_len_basis': basis,
                        'reference': f'{ref[0]}:{ref[1]}'}
        json.dump(rec, open(os.path.join(OUT_DIR, n.replace('::', '__') + '.json'), 'w'), indent=1)

    json.dump(inventory, open(os.path.join(HERE, 'inventory.json'), 'w'), indent=1, sort_keys=True)
    print(f'{len(inventory)} interfaces -> {OUT_DIR}')


if __name__ == '__main__':
    main()
