#!/usr/bin/env python3
"""Match a --retail-linux32 build against the retail Linux game or engine modules.

usage: report-linux.py --build DIR [--game episodic|lostcoast] [--module server|client ...] [--out DIR]
       report-linux.py --build DIR --engine [--module engine ...] [--out DIR]

Per game module, our objects are the module's own (compile_commands.json entries
with --bm-proj=game/<module>) plus the static-library members the retail macOS
module of the same build pulls (retail-osx32.json; the Linux link line is the
same) and the Linux-only library files (retail-linux32.json). Per engine module
(depot 224, --engine) they are what ld linked: the loose objects and the archive
members its link map lists. Retail is stripped, so bytematch names its .eh_frame
functions by masked search (bytematch/elfmatch.py). Writes bytematch's
JSON/markdown per module and a summary, OUT/retail-linux32-GAME.md (or
retail-linux32.md), with totals for the module's own objects (the survey's
metric), each static library, and everything together.
"""
import argparse, collections, json, os, re, shutil, sys

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
DEPOT224 = os.path.join(DEPOTS, '224', '19307283')
LINUX_MANIFEST = os.path.join(REPO, 'devtools', 'bytematch', 'manifests', 'retail-linux32.json')
ENGINE_NAMES = {'hl2_osx': 'hl2_linux'}
# third-party archives build-thirdparty.sh rebuilds (measured from the link map); the other
# prebuilts (SDK 2013 zlib, curl, OpenSSL) are Valve's own files and not counted
REBUILT_ARCHIVES = ('libjpeg.a', 'libpng.a')


def engine_modules():
    '''(manifest key, file name, our output relative to the build dir, retail path), in manifest order.'''
    lin = json.load(open(LINUX_MANIFEST))
    out = []
    for key, e in sorted(lin['modules'].items(), key=lambda kv: kv[0].lower()):
        if not (key.startswith('bin/') or key in ENGINE_NAMES):
            continue
        name = ENGINE_NAMES.get(key, os.path.basename(key) + '.so')
        retail = os.path.join(DEPOT224, name) if key in ENGINE_NAMES else os.path.join(DEPOT224, 'bin', name)
        out.append((key, name, os.path.join(e['project_dir'], name), retail))
    return out


def map_groups(mapfile, build):
    '''{group: [object paths]} from a GNU ld map: 'module' for loose objects, the archive's
    file name for archive members ld pulled. Container paths become host paths.'''
    host = lambda p: os.path.join(build, p[len('/data/build/'):]) if p.startswith('/data/build/') else p
    groups = collections.OrderedDict([('module', [])])
    text = open(mapfile, errors='replace').read()
    head, _, rest = text.partition('\nMemory Configuration')
    for m in re.finditer(r'^(\S+\.a)\(([^)]+\.o)\)$', head, re.M):
        groups.setdefault(os.path.basename(m.group(1)), [])
    for m in re.finditer(r'^LOAD (\S+\.o)$', rest, re.M):
        groups['module'].append(host(m.group(1)))
    members = collections.defaultdict(set)
    for m in re.finditer(r'^(\S+\.a)\(([^)]+\.o)\)$', head, re.M):
        members[m.group(1)].add(m.group(2))
    idx = object_index(build)
    for ar, objs in members.items():
        if os.path.basename(ar) in REBUILT_ARCHIVES:
            groups[os.path.basename(ar)] += extract_members(host(ar), objs, build)
            continue
        # archive members are object basenames; waf's per-task-generator suffix (x.cpp.N.o)
        # keeps them unique, prefer the archive's own directory anyway
        d = os.path.dirname(host(ar))
        for o in sorted(objs):
            cands = idx.get(o, [])
            near = [c for c in cands if c.startswith(d + os.sep)]
            pick = (near or cands or [None])[0]
            if pick:
                groups[os.path.basename(ar)].append(pick)
    return groups


def extract_members(archive, names, build):
    '''Archive members ld pulled from a prebuilt archive, written under BUILD/objs-thirdparty.'''
    from bytematch import loader
    dst = os.path.join(build, 'objs-thirdparty', os.path.basename(archive))
    os.makedirs(dst, exist_ok=True)
    out = []
    for name, body in loader.ar_members(open(archive, 'rb').read()):
        if name in names:
            p = os.path.join(dst, name)
            with open(p, 'wb') as f:
                f.write(body)
            out.append(p)
    return out


_index = {}


def object_index(build):
    if build not in _index:
        idx = collections.defaultdict(list)
        for root, dirs, files in os.walk(build):
            dirs[:] = [x for x in dirs if not x.startswith(('report', 'objs-', 'stripped', 'c4che'))]
            for f in files:
                if f.endswith('.o'):
                    idx[f].append(os.path.join(root, f))
        _index[build] = idx
    return _index[build]


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
    entries = compile_entries(build)
    groups = objects_for(game, module, entries)
    mapfile = os.path.join(build, 'game', module, module + '.so.map')
    if os.path.exists(mapfile):
        for g, objs in map_groups(mapfile, build).items():
            if g in REBUILT_ARCHIVES:
                groups[g] = [dict(obj=o) for o in objs]
    return measure(groups, RETAIL[game] % module, build, out, '%s-%s' % (game, module), extra)


def run_engine(key, name, retail, build, out, extra=()):
    mapfile = os.path.join(build, [m[2] for m in engine_modules() if m[0] == key][0] + '.map')
    if not os.path.exists(mapfile):
        return None, None
    groups = collections.OrderedDict((g, [dict(obj=o) for o in objs]) for g, objs in map_groups(mapfile, build).items())
    return measure(groups, retail, build, out, os.path.splitext(name)[0], extra)


def measure(groups, retail, build, out, stem, extra=()):
    from bytematch import cli
    tmp = os.path.join(out, 'objs-' + stem)
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
    prefix = os.path.join(out, stem)
    cli.main(['report', '--retail', retail, '--ours', tmp, '--out', prefix] + list(extra))
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
        if e.get('identical_in_retail'):
            c['mf'] += 1
            c['mb'] += e['size']
            c['loc'] += 1
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
    doc['objects_not_located'] = sorted(os.path.relpath(p, build) for p, c in per_obj.items() if c['f'] and not c['loc'])
    return doc, rows


def table(rows):
    lines = ['| objects | functions | matched | % | bytes | matched | % | located |',
             '|---|---:|---:|---:|---:|---:|---:|---:|']
    for g, c in rows.items():
        if not c['f']:
            continue
        lines.append('| %s | %d | %d | %.1f%% | %d | %d | %.1f%% | %d |' % (
            '**total**' if g == 'total' else g, c['f'], c['mf'], 100.0 * c['mf'] / c['f'], c['b'], c['mb'],
            100.0 * c['mb'] / c['b'] if c['b'] else 0, c['loc']))
    return lines


GCC_SUFFIX = re.compile(r'(\.(part|cold|isra|constprop)(\.\d+)?)+$')


_mac_any = {}


def mac_statuses(path):
    out = {}
    for f in json.load(open(path))['functions']:
        out[f.get('key') or f['name'][1:]] = f['status']
    return out


def mac_any(mac_dir):
    '''Function -> macOS status over every module report in mac_dir, matched if any module
    matches it: the macOS modules dead-strip static-library functions they don't use.'''
    if mac_dir not in _mac_any:
        out = {}
        for fn in sorted(os.listdir(mac_dir)):
            if fn.endswith('.json'):
                for k, st in mac_statuses(os.path.join(mac_dir, fn)).items():
                    if out.get(k) not in MATCHED:
                        out[k] = st
        _mac_any[mac_dir] = out
    return _mac_any[mac_dir]


def classify(doc, mac_json):
    '''Unmatched functions of ours by the macOS status of the same function (bytematch report
    of the --retail-osx32 module, else of any macOS module): differs on macOS too (shared
    source drift), matches on macOS (a Linux-only cause), or no such function there.
    {class: [(bytes, name, object)]}.'''
    mac = dict(mac_any(os.path.dirname(mac_json)))
    mac.update(mac_statuses(mac_json))
    out = collections.OrderedDict((k, []) for k in ('shared', 'linux', 'nomac'))
    for f in doc['functions']:
        if f['status'] in MATCHED:
            continue
        name = GCC_SUFFIX.sub('', f['name'])
        st = mac.get(name)
        k = 'nomac' if st is None or st == 'missing' else 'linux' if st in MATCHED else 'shared'
        out[k].append((f['ours_size'], f['name'], os.path.basename(f['ours_object'])))
    for e in doc['extra']:
        if not e.get('identical_in_retail'):
            out['nomac' if mac.get(GCC_SUFFIX.sub('', e['name'])) is None else 'shared'].append(
                (e['size'], e['name'], os.path.basename(e['ours_object'])))
    return out


def main_engine(args, out, extra):
    mods = [m for m in engine_modules() if not args.module or os.path.splitext(m[1])[0] in args.module]
    lines = ['# --retail-linux32 --retail-linux32-engine vs retail depot 224 (build 19307283)', '',
             'Our functions (deduplicated), our sizes; a function counts as matched when its bytes equal a '
             'retail function\'s once relocated fields are masked (bytematch, stripped-ELF pairing). '
             'Objects: what ld linked into our module (its map): loose objects ("module") and the archive '
             'members it pulled.', '',
             '| module | functions | matched | % | bytes | matched | % | own objects % funcs / bytes |',
             '|---|---:|---:|---:|---:|---:|---:|---:|']
    detail, summary, tot = [], collections.OrderedDict(), collections.Counter()
    for key, name, rel, retail in mods:
        doc, rows = run_engine(key, name, retail, args.build, out, extra)
        if doc is None:
            lines.append('| %s | not built | | | | | | |' % name)
            continue
        t, own = rows['total'], rows['module']
        tot.update(t)
        summary[name] = {'total': dict(t), 'module': dict(own),
                         'groups': dict((g, dict(c)) for g, c in rows.items()),
                         'objects_not_located': doc['objects_not_located']}
        lines.append('| %s | %d | %d | %.1f%% | %d | %d | %.1f%% | %.1f%% / %.1f%% |' % (
            name, t['f'], t['mf'], 100.0 * t['mf'] / max(t['f'], 1), t['b'], t['mb'], 100.0 * t['mb'] / max(t['b'], 1),
            100.0 * own['mf'] / max(own['f'], 1), 100.0 * own['mb'] / max(own['b'], 1)))
        s = doc['summary']['functions']
        detail += ['## %s' % name, ''] + table(rows) + [
            '', 'Paired with a retail function: %d (exact %d, after masking %d, differs %d); report: `%s.md`.' % (
                s['total'], s['exact'], s['matches_after_masking'], s['differs'], os.path.splitext(name)[0])]
        if doc['objects_not_located']:
            detail.append('Objects with no function located in retail: %s.' % ', '.join(
                '`%s`' % os.path.basename(p) for p in doc['objects_not_located']))
        mj = os.path.join(args.mac, os.path.splitext(name)[0] + '.json') if args.mac else None
        if mj and os.path.exists(mj):
            cl = classify(doc, mj)
            summary[name]['classes'] = dict((k, sorted(v, reverse=True)) for k, v in cl.items())
            detail.append('Unmatched by macOS status: differs on macOS too %d (%d bytes), matches on macOS %d '
                          '(%d bytes), not on macOS %d (%d bytes).' % tuple(
                              x for k in cl for x in (len(cl[k]), sum(s for s, _, _ in cl[k]))))
        detail.append('')
    lines.append('| **total** | %d | %d | %.1f%% | %d | %d | %.1f%% | |' % (
        tot['f'], tot['mf'], 100.0 * tot['mf'] / max(tot['f'], 1), tot['b'], tot['mb'], 100.0 * tot['mb'] / max(tot['b'], 1)))
    path = os.path.join(out, 'retail-linux32.md')
    open(path, 'w').write('\n'.join(lines + [''] + detail))
    json.dump(summary, open(os.path.join(out, 'retail-linux32.json'), 'w'), indent=1)
    print(path)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--build', required=True)
    ap.add_argument('--game', default='episodic', choices=sorted(RETAIL))
    ap.add_argument('--module', action='append', help='server, client; with --engine a file stem (engine, libtier0, ...)')
    ap.add_argument('--engine', action='store_true', help='depot 224 engine-side modules (--retail-linux32-engine)')
    ap.add_argument('--mac', help='with --engine: directory of bytematch reports (STEM.json) of the --retail-osx32 '
                                  'modules, to classify unmatched functions by their macOS status')
    ap.add_argument('--out', default=None)
    ap.add_argument('--no-target-check', action='store_true', help='as the survey pilot: no call-target check')
    args = ap.parse_args()
    out = args.out or os.path.join(args.build, 'report')
    os.makedirs(out, exist_ok=True)
    if args.engine:
        return main_engine(args, out, ['--no-target-check'] if args.no_target_check else [])
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
