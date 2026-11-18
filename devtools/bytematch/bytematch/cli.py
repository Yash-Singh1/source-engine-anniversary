"""bytematch command line."""
import argparse
import collections
import json
import os
import re
import sys

from . import compare, loader, mapfile, report, winmap, x86
from .model import object_stem


def _load_retail(args):
    mod = loader.load_one(args.retail)
    for m in args.retail_map or ():
        n = mapfile.apply_map(mod, m)
        print(f'applied {n} names from {m}', file=sys.stderr)
    return mod


def _load_ours(args):
    mods = loader.load_path(args.ours, quiet=True)
    for m in getattr(args, 'ours_map', None) or ():
        for mod in mods:
            mapfile.apply_map(mod, m)
    funcs = [f for m in mods for f in m.functions]
    if not funcs:
        raise SystemExit(f'error: no functions found in {args.ours}')
    if any(m.fmt == 'coff' for m in mods):
        winmap.add_sig_aliases([f for f in funcs if f.module.fmt == 'coff'])
    for m in getattr(args, 'ours_link_map', None) or ():
        n = winmap.load_link_map(m)
        print(f'{n} folded groups from {m}', file=sys.stderr)
    return mods, funcs


def _data_check(args, retail, mods):
    if not getattr(args, 'data_check', False):
        return None
    from . import data
    if retail.fmt != 'macho':
        raise SystemExit('error: --data-check supports Mach-O only')
    rimg = data.Image(retail.path)
    oimgs = {}
    for m in mods:
        if m.fmt == 'macho' and m.member is None and m.path not in oimgs:
            oimgs[m.path] = data.Image(m.path)
    return rimg, oimgs


def _include(args):
    if not getattr(args, 'include', None):
        return None
    rx = re.compile(args.include)
    return lambda f: bool(rx.search(f.key) or rx.search(f.name))


def _find_symbol(funcs, sym):
    exact = [f for f in funcs if sym in (f.name, f.key) or sym in f.aliases]
    if exact:
        return exact
    pretty = report.demangle([f.key for f in funcs])
    sub = [f for f in funcs if sym in f.key or sym in pretty.get(f.key, '')]
    return sub


def cmd_diff(args):
    retail = _load_retail(args)
    mods, ours = _load_ours(args)
    if args.symbol:
        cands = _find_symbol(ours, args.symbol)
        if not cands:
            cands_r = _find_symbol(retail.functions, args.symbol)
            if cands_r:
                for f in cands_r[:20]:
                    print(f'retail only: {f.obj or "?"}: {f.name} @ {f.addr:#x} ({f.size} B)')
                return 1
            raise SystemExit(f'error: no function matching {args.symbol!r} in ours or retail')
        keys = sorted({f.pair_key for f in cands})
        if len(keys) > 1:
            pretty = report.demangle([f.key for f in cands])
            print(f'{len(keys)} functions match {args.symbol!r}; be more specific:')
            for f in cands[:40]:
                print(f'  {f.obj}: {f.name}   ({pretty.get(f.key, f.key)})')
            return 1
        o = cands[0]
        rscope, _, _ = report.scope(retail, ours, args.retail_object, all_retail=True)
        r = compare.Index(rscope).find(o)
        if r is None:
            print(f'{o.name}: not found in retail ({retail.path})')
            return 1
        res = compare.compare(r, o, not args.no_target_check)
        dc = _data_check(args, retail, [o.module] if o.module.member is None else [])
        if dc:
            report.apply_data_check([res], *dc)
        pretty = report.demangle([o.key]).get(o.key, o.key)
        print(f'function: {pretty}\n  symbol: {o.name}')
        print(f'  retail: {r.addr:#x} size {r.size}{"" if r.size_known else " (extent unknown)"}'
              f'  object {r.obj_path or r.obj or "?"}')
        print(f'  ours:   size {o.size}  object {o.obj_path or o.module.label}')
        print(f'  status: {res.status}' + (f'  ({res.reason})' if res.reason else '')
              + (f'  masked bytes {res.masked_bytes}' if res.masked_bytes else ''))
        for k, a, b in res.data_mismatches:
            from . import data as _d
            print(f'  data ref +{k:#x} ({_d.kind_of(a, b)}): retail {_d.show(a)}  ours {_d.show(b)}')
        if res.status == compare.DIFFERS or args.listing:
            if not x86.have_capstone():
                print('  (capstone not installed: showing masked hex rows)')
            lines = compare.instruction_diff(res, context=args.context if res.status == compare.DIFFERS else 10 ** 6)
            print('\n'.join(lines))
        return 0 if res.status != compare.DIFFERS else 2

    data = report.run(retail, ours, args.retail_object, args.all_retail, not args.no_target_check, _include(args),
                      _data_check(args, retail, mods))
    for n in data['notes']:
        print(f'note: {n}', file=sys.stderr)
    results = data['results']
    if args.json:
        json.dump(dict(scope=data['mode'], summary=report.summarize(results),
                       functions=[report.result_json(r, args.show and 40) for r in results],
                       shared=[report.result_json(r) for r in data['shared']],
                       extra=[dict(name=e.name, object=e.obj, size=e.ours.size) for e in data['extra']]),
                  sys.stdout, indent=1)
        print()
        return 0
    names = report.demangle([r.key for r in results + data['extra']]) if args.demangle else None
    report.print_table(results + (data['extra'] if args.verbose else []), show_all=not args.differs_only, names=names)
    if args.show:
        for r in [r for r in results if r.status == compare.DIFFERS][:args.show]:
            print(f'\n=== {r.key}  [{r.reason}]')
            print('\n'.join(compare.instruction_diff(r, context=args.context, limit=80)))
    s = report.summarize(results)
    f, b = s['functions'], s['bytes']
    print(f'\nscope={data["mode"]} functions={f["total"]} exact={f["exact"]} masked={f["matches_after_masking"]} '
          f'differs={f["differs"]} missing={f["missing"]} matched={f["matched"]} ({f["pct"]}%) '
          f'bytes {b["matched"]}/{b["total"]} ({b["pct"]}%)')
    if data['shared']:
        ss = report.summarize(data['shared'])['functions']
        print(f'out-of-scope copies (attributed to other retail objects): {ss["matched"]}/{ss["total"]} matched')
    if s['line_constants_only']['functions']:
        print(f'differing only in __LINE__-like immediates: {s["line_constants_only"]["functions"]} functions')
    print(f'only in ours: {len(data["extra"])}')
    return 0


def cmd_report(args):
    retail = _load_retail(args)
    mods, ours = _load_ours(args)
    data = report.run(retail, ours, args.retail_object, args.all_retail, not args.no_target_check, _include(args),
                      _data_check(args, retail, mods))
    out = args.out or os.path.join(os.getcwd(), 'bytematch-report')
    doc = report.write(out, dict(retail=os.path.abspath(args.retail), ours=os.path.abspath(args.ours),
                                 ours_modules=len(mods)), data, diff_lines=args.diff_lines, top=args.top)
    f, b = doc['summary']['functions'], doc['summary']['bytes']
    print(f'{out}.json\n{out}.md')
    print(f'scope={doc["scope"]} functions {f["matched"]}/{f["total"]} ({f["pct"]}%)  bytes {b["matched"]}/{b["total"]} ({b["pct"]}%)')
    return 0


def cmd_data(args):
    from . import audit
    tree = args.tree or os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..'))
    log = (lambda m: print(f'[data] {m}', file=sys.stderr)) if args.verbose else None
    doc = audit.audit(args.retail, args.ours, tree=tree, pristine=args.pristine, sources=not args.no_sources, log=log)
    out = args.out or os.path.join(os.getcwd(), 'bytematch-data')
    audit.write(out, doc, args.title)
    s = doc['summary']
    print(f'{out}.json\n{out}.md')
    print(f'literals retail {s["retail_strings"]} ours {s["ours_strings"]}: differing {s["differing"]}, '
          f'retail-only {s["missing"]}, ours-only {s["extra"]}, moved {s["moved"]}; '
          f'functions not truly matching {s["functions_not_truly_matching"]}/{s["matched_functions"]} '
          f'(strings {s["functions_string_mismatch"]}); data symbols differing {s["data_symbols_differing"]}')
    return 0


def cmd_funcs(args):
    mod = loader.load_one(args.module)
    for m in args.map or ():
        mapfile.apply_map(mod, m)
    funcs = mod.functions
    if args.object:
        funcs = [f for f in funcs if args.object == f.obj or args.object in (f.obj_path or '')]
    if args.symbol:
        funcs = [f for f in funcs if re.search(args.symbol, f.name)]
    if args.json:
        json.dump([dict(name=f.name, key=f.key, addr=f.addr, size=f.size, raw_size=f.raw_size, object=f.obj_path or f.obj,
                        local=f.local, relocs=len(f.relocs)) for f in funcs], sys.stdout, indent=1)
        print()
        return 0
    for f in sorted(funcs, key=lambda f: f.addr):
        print(f'{f.addr:08x} {f.size:6d} {"l" if f.local else "g"} {f.obj or "-":24s} {f.name}')
    print(f'{len(funcs)} functions, {sum(f.size for f in funcs)} bytes ({mod.fmt}, {"linked" if mod.linked else "object"})',
          file=sys.stderr)
    return 0


def cmd_objects(args):
    mod = loader.load_one(args.module)
    if not mod.debug_map:
        raise SystemExit(f'{args.module}: no N_OSO debug map (only Mach-O images carry one)')
    count = collections.Counter(f.obj_path for f in mod.functions)
    size = collections.Counter()
    for f in mod.functions:
        size[f.obj_path] += f.size
    rows = [dict(index=i, path=p, stem=object_stem(p), mtime=t, functions=count[p], bytes=size[p])
            for i, (p, t) in enumerate(mod.debug_map)]
    if args.json:
        json.dump(rows, sys.stdout, indent=1)
        print()
        return 0
    for r in rows:
        print(f'{r["index"]:4d} {r["functions"]:5d} {r["bytes"]:8d}  {r["stem"]:28s} {r["path"]}')
    return 0


def cmd_locate(args):
    retail = _load_retail(args)
    mods, ours = _load_ours(args)
    ours = compare.dedupe_ours(ours)
    found = amb = 0
    lines = []
    for o in ours:
        if o.size < args.min_size:
            continue
        mask, _ = compare.build_mask(o, o, o.data, o.data)
        hits = _search(retail, o.data, mask)
        if len(hits) == 1:
            found += 1
            lines.append(f'{hits[0]:#010x} {o.name} {o.size}')
        elif hits:
            amb += 1
            lines.append(f'# ambiguous ({len(hits)} hits): {o.name} ' + ' '.join(hex(h) for h in hits[:4]))
    text = '\n'.join(lines) + '\n'
    if args.output:
        open(args.output, 'w').write(text)
    else:
        sys.stdout.write(text)
    print(f'located {found} uniquely, {amb} ambiguous, of {sum(1 for o in ours if o.size >= args.min_size)} '
          f'functions >= {args.min_size} bytes', file=sys.stderr)
    return 0


def _search(mod, code, mask, limit=4):
    runs, i, n = [], 0, len(code)
    while i < n:
        if i in mask:
            i += 1
            continue
        j = i
        while j < n and j not in mask:
            j += 1
        runs.append((i, j))
        i = j
    if not runs:
        return []
    a0, a1 = max(runs, key=lambda r: r[1] - r[0])
    anchor = code[a0:a1]
    hits = []
    for va, text, _ in mod.code:
        pos = text.find(anchor)
        while pos != -1 and len(hits) < limit:
            base = pos - a0
            if 0 <= base and base + n <= len(text) and all(text[base + s:base + e] == code[s:e] for s, e in runs):
                hits.append(va + base)
            pos = text.find(anchor, pos + 1)
    return hits


def cmd_selftest(args):
    from . import selftest
    return selftest.main(args)


def main(argv=None):
    p = argparse.ArgumentParser(prog='bytematch', description='Per-function byte matching of x86-32 code against retail modules.')
    sub = p.add_subparsers(dest='cmd', required=True)

    def common(sp, ours=True):
        sp.add_argument('--retail', required=True, help='retail module (.dylib/.so/.dll)')
        if ours:
            sp.add_argument('--ours', required=True, help='our object, archive, linked module or build directory')
        sp.add_argument('--retail-map', action='append', help='address->name map for the retail module (repeatable)')
        sp.add_argument('--ours-map', action='append', help='address->name map for linked ours modules')
        sp.add_argument('--ours-link-map', action='append',
                        help='MSVC link /MAP of our module: names it folded together (/OPT:ICF) count as one call target')
        sp.add_argument('--retail-object', action='append',
                        help='limit retail scope to debug-map objects whose stem equals or path contains this (repeatable)')
        sp.add_argument('--all-retail', action='store_true', help='judge against every retail function, not just our objects')
        sp.add_argument('--include', help='regex on symbol names to restrict the comparison')
        sp.add_argument('--no-target-check', action='store_true', help='do not flag relocations that resolve to different symbols')
        sp.add_argument('--data-check', action='store_true',
                        help='Mach-O: matched functions whose masked data references resolve to different strings, '
                             'globals or constants count as differing')

    d = sub.add_parser('diff', help='compare functions; with --symbol show an instruction diff')
    common(d)
    d.add_argument('--symbol', help='raw, canonical or demangled (substring) name of one function')
    d.add_argument('--context', type=int, default=3, help='diff context lines')
    d.add_argument('--listing', action='store_true', help='with --symbol: print the full listing even when it matches')
    d.add_argument('--show', type=int, default=0, help='print instruction diffs for the first N differing functions')
    d.add_argument('--differs-only', action='store_true', help='hide matched functions in the table')
    d.add_argument('--demangle', action='store_true', help='demangle names in the table')
    d.add_argument('--verbose', '-v', action='store_true', help='also list functions only present in ours')
    d.add_argument('--json', action='store_true')
    d.set_defaults(fn=cmd_diff)

    r = sub.add_parser('report', help='write JSON and markdown match reports')
    common(r)
    r.add_argument('--out', help='output path prefix (default ./bytematch-report)')
    r.add_argument('--diff-lines', type=int, default=12, help='max diff lines per differing function')
    r.add_argument('--top', type=int, default=40, help='differing functions shown in markdown')
    r.set_defaults(fn=cmd_report)

    da = sub.add_parser('data', help='Mach-O data audit: literals, constant data, data refs of matched functions')
    da.add_argument('--retail', required=True)
    da.add_argument('--ours', required=True, help='our linked module (or one object)')
    da.add_argument('--out', help='output path prefix (default ./bytematch-data)')
    da.add_argument('--tree', help='source tree for locating literals (default: this checkout)')
    da.add_argument('--pristine', default='/Users/yashsingh/p/sco/source-engine-2018-hl2_src',
                    help='pristine leak to search for retail-only literals')
    da.add_argument('--no-sources', action='store_true', help='skip source location')
    da.add_argument('--title')
    da.add_argument('-v', '--verbose', action='store_true')
    da.set_defaults(fn=cmd_data)

    f = sub.add_parser('funcs', help='list the functions extracted from a module')
    f.add_argument('module')
    f.add_argument('--map', action='append')
    f.add_argument('--object', help='debug-map object stem or path substring')
    f.add_argument('--symbol', help='regex on raw names')
    f.add_argument('--json', action='store_true')
    f.set_defaults(fn=cmd_funcs)

    o = sub.add_parser('objects', help='list a Mach-O module\'s debug-map objects (N_OSO) in link order')
    o.add_argument('module')
    o.add_argument('--json', action='store_true')
    o.set_defaults(fn=cmd_objects)

    lo = sub.add_parser('locate', help='find our functions in an unnamed retail image; emits an address map')
    common(lo)
    lo.add_argument('--min-size', type=int, default=16)
    lo.add_argument('-o', '--output')
    lo.set_defaults(fn=cmd_locate)

    st = sub.add_parser('selftest', help='run the built-in self tests')
    st.add_argument('--depots', default='/Volumes/Expansion/mac-ports/hl2-anniversary-depot/depots')
    st.add_argument('--full', action='store_true', help='also self-compare large modules (engine, server)')
    st.add_argument('--rebuild', action='store_true', help='recompile sample objects even if tests/build is current')
    st.set_defaults(fn=cmd_selftest)

    args = p.parse_args(argv)
    try:
        return args.fn(args) or 0
    except BrokenPipeError:
        return 0


if __name__ == '__main__':
    sys.exit(main())
