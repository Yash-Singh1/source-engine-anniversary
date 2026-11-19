#!/usr/bin/env python3
"""Match a --retail-linux32 build against the retail Linux game modules.

usage: report-linux.py --build DIR [--game episodic|lostcoast] [--module server|client ...] [--out DIR]

Per module, our objects are the module's own (compile_commands.json entries
with --bm-proj=game/<module>) plus the static-library members the retail macOS
module of the same build pulls (retail-osx32.json; the Linux link line is the
same) and the Linux-only library files (retail-linux32.json). Retail is
stripped, so bytematch names its .eh_frame functions by masked search
(bytematch/elfmatch.py). Writes bytematch's JSON/markdown per module and a
summary, OUT/retail-linux32-GAME.md, with totals for the module's own objects
(the survey's metric), each static library, and everything together.
"""
import argparse, collections, json, os, shutil, sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, '..', '..', '..', '..'))
sys.path.insert(0, os.path.join(REPO, 'devtools', 'bytematch'))

DEPOTS = '/Volumes/Expansion/mac-ports/hl2-anniversary-depot/depots'
RETAIL = {
    'episodic': os.path.join(DEPOTS, '380', '19307283', 'episodic', 'bin', '%s.so'),
    'hl2': os.path.join(DEPOTS, '380', '19307283', 'episodic', 'bin', '%s.so'),
    'lostcoast': os.path.join(DEPOTS, '340', '19307283', 'lostcoast', 'bin', '%s.so'),
}
MATCHED = ('exact', 'matches-after-masking')


def compile_entries(build):
    # retail_compile_commands.json (`waf retail_linkdb`) has one entry per object;
    # compile_commands.json one per source, so files both client and server compile collapse
    db = os.path.join(build, 'retail_compile_commands.json')
    if not os.path.exists(db):
        sys.exit('%s missing: run `waf retail_linkdb` after the build' % db)
    out = []
    for e in json.load(open(db)):
        args = e['arguments']
        proj = next((a.split('=', 1)[1] for a in args if a.startswith('--bm-proj=')), None)
        obj = e.get('output') or next((a[2:] for a in args if a.startswith('-o') and len(a) > 2), None)
        if obj and not os.path.isabs(obj):
            obj = os.path.join(e['directory'], obj)
        out.append(dict(file=os.path.relpath(e['file'], REPO), proj=proj, obj=obj,
                        lostcoast='-DHL2_LOSTCOAST' in args))
    return out


def objects_for(game, module, entries):
    osx = json.load(open(os.path.join(REPO, 'devtools', 'bytematch', 'manifests', 'retail-osx32.json')))
    lin = json.load(open(os.path.join(REPO, 'devtools', 'bytematch', 'manifests', 'retail-linux32.json')))
    key = '%s/%s' % ('episodic' if game == 'hl2' else game, module)
    lc = game == 'lostcoast'
    groups = collections.OrderedDict()
    groups['module'] = [e for e in entries if e['proj'] == 'game/' + module and e['lostcoast'] == lc]
    by_proj_file = {}
    for e in entries:
        if e['lostcoast'] == lc or e['proj'].startswith('game/'):
            by_proj_file.setdefault((e['proj'], e['file'].lower()), e)
    for o in osx['modules'][key]['objects']:
        if not o['archive']:
            continue
        a = lin['archives'].get(o['archive'])
        src = o['source'] or o['retail_source']
        if not a or not src:
            continue
        e = by_proj_file.get((a['project_dir'], src.lower()))
        if e:
            groups.setdefault(o['archive'], [])
            if e not in groups[o['archive']]:
                groups[o['archive']].append(e)
    for name, a in lin['archives'].items():
        if name in groups:
            for src in a.get('add', []):
                e = by_proj_file.get((a['project_dir'], src.lower()))
                if e and e not in groups[name]:
                    e = dict(e, linux_only=True)
                    groups[name].append(e)
    return groups


def run(game, module, build, out, extra=()):
    from bytematch import cli
    entries = compile_entries(build)
    groups = objects_for(game, module, entries)
    tmp = os.path.join(out, 'objs-%s-%s' % (game, module))
    shutil.rmtree(tmp, ignore_errors=True)
    group_of, linux_only = {}, set()
    for g, es in groups.items():
        for e in es:
            if not e['obj'] or not os.path.exists(e['obj']):
                continue
            dst = os.path.join(tmp, os.path.relpath(e['obj'], build))
            os.makedirs(os.path.dirname(dst), exist_ok=True)
            if not os.path.lexists(dst):
                os.symlink(e['obj'], dst)
            group_of[os.path.realpath(dst)] = g
            if e.get('linux_only'):
                linux_only.add(os.path.realpath(dst))
    prefix = os.path.join(out, '%s-%s' % (game, module))
    cli.main(['report', '--retail', RETAIL[game] % module, '--ours', tmp, '--out', prefix] + list(extra))
    doc = json.load(open(prefix + '.json'))
    per_obj = collections.defaultdict(collections.Counter)
    for f in doc['functions']:
        c = per_obj[os.path.realpath(f['ours_object'])]
        c['f'] += 1
        c['b'] += f['ours_size']
        c['loc'] += 1
        if f['status'] in MATCHED:
            c['mf'] += 1
            c['mb'] += f['ours_size']
    for e in doc['extra']:
        c = per_obj[os.path.realpath(e['ours_object'])]
        c['f'] += 1
        c['b'] += e['size']
    # Linux-only library files nothing in the module pulls (pathmatch.cpp is reached only through
    # -Wl,--wrap, which game modules don't use): retail has none of their functions
    dropped = sorted(os.path.basename(p) for p in linux_only if not per_obj[p]['loc'])
    rows = collections.OrderedDict((g, collections.Counter()) for g in groups)
    rows['total'] = collections.Counter()
    for p, c in per_obj.items():
        if os.path.basename(p) in dropped:
            continue
        for k in (group_of.get(p, 'module'), 'total'):
            rows[k].update(c)
    doc['linux_only_not_linked'] = dropped
    return doc, rows


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--build', required=True)
    ap.add_argument('--game', default='episodic', choices=sorted(RETAIL))
    ap.add_argument('--module', action='append', choices=['server', 'client'])
    ap.add_argument('--out', default=None)
    ap.add_argument('--no-target-check', action='store_true', help='as the survey pilot: no call-target check')
    args = ap.parse_args()
    out = args.out or os.path.join(args.build, 'report')
    os.makedirs(out, exist_ok=True)
    lines = ['# --retail-linux32 vs retail %s (build 19307283)' % args.game, '',
             'Our functions (deduplicated), our sizes; a function counts as matched when its bytes equal a '
             'retail function\'s once relocated fields are masked (bytematch, stripped-ELF pairing). '
             'Static libraries: the members the retail macOS module of the same build pulls.', '']
    for module in args.module or ['server', 'client']:
        doc, rows = run(args.game, module, args.build, out, ['--no-target-check'] if args.no_target_check else [])
        s = doc['summary']['functions']
        lines += ['## %s.so' % module, '', '| objects | functions | matched | % | bytes | matched | % | located |',
                  '|---|---:|---:|---:|---:|---:|---:|---:|']
        for g, c in rows.items():
            if not c['f']:
                continue
            lines.append('| %s | %d | %d | %.1f%% | %d | %d | %.1f%% | %d |' % (
                '**total**' if g == 'total' else g, c['f'], c['mf'], 100.0 * c['mf'] / c['f'], c['b'], c['mb'],
                100.0 * c['mb'] / c['b'] if c['b'] else 0, c['loc']))
        lines += ['', 'Paired with a retail function: %d (exact %d, after masking %d, differs %d); report: `%s-%s.md`.' % (
            s['total'], s['exact'], s['matches_after_masking'], s['differs'], args.game, module)]
        if doc['linux_only_not_linked']:
            lines.append('Linux-only library files with no function in retail (not counted): %s.' % ', '.join(doc['linux_only_not_linked']))
        lines.append('')
    path = os.path.join(out, 'retail-linux32-%s.md' % args.game)
    open(path, 'w').write('\n'.join(lines))
    print(path)


if __name__ == '__main__':
    main()
