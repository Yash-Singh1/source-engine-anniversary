"""Scoping, summaries and JSON/markdown report output."""
import collections
import datetime
import json
import shutil
import subprocess
import sys

from . import compare, x86
from .compare import DIFFERS, EXACT, EXTRA, MASKED, MISSING

MATCHED = (EXACT, MASKED)


def demangle(names):
    """Best-effort Itanium demangling via c++filt; returns {name: pretty}."""
    names = [n for n in set(names) if n]
    tool = shutil.which('c++filt') or shutil.which('llvm-cxxfilt')
    if not tool or not names:
        return {n: n for n in names}
    keys = [n[1:] if n.startswith('__Z') else n for n in names]
    try:
        out = subprocess.run([tool], input='\n'.join(keys) + '\n', capture_output=True, text=True, timeout=120).stdout
        lines = out.split('\n')
        return {n: (lines[i] if i < len(lines) and lines[i] else n) for i, n in enumerate(names)}
    except (OSError, subprocess.SubprocessError):
        return {n: n for n in names}


def scope(retail, ours_funcs, objects=None, all_retail=False):
    """Pick the retail functions a comparison is judged against.

    Returns (functions, mode, notes). Modes:
      objects - retail functions the debug map attributes to our object stems
                (or to --retail-object patterns)
      all     - every retail function
      paired  - formats without object info: retail functions that have a
                counterpart in ours (so 'missing' cannot be measured)"""
    notes = []
    rf = retail.functions
    if objects:
        sel = [f for f in rf if any(p == f.obj or p in (f.obj_path or '') for p in objects)]
        paths = sorted({f.obj_path for f in sel if f.obj_path})
        if len(paths) > 1:
            notes.append('retail objects selected: ' + ', '.join(paths))
        return sel, 'objects', notes
    if all_retail:
        return list(rf), 'all', notes
    if any(f.obj for f in rf) and retail.fmt == 'macho':
        stems = {f.obj for f in ours_funcs if f.obj}
        sel = [f for f in rf if f.obj in stems]
        by_stem = collections.defaultdict(set)
        for f in sel:
            by_stem[f.obj].add(f.obj_path)
        for stem, paths in sorted(by_stem.items()):
            if len(paths) > 1:
                notes.append(f'object stem {stem!r} is ambiguous in the retail debug map '
                             f'({len(paths)} objects); use --retail-object to pick one: ' + ', '.join(sorted(paths)))
        missing = sorted(stems - set(by_stem))
        if missing:
            notes.append(f'{len(missing)} of our objects have no retail counterpart in the debug map: '
                         + ', '.join(missing[:20]) + (' ...' if len(missing) > 20 else ''))
        return sel, 'objects', notes
    idx = compare.Index(ours_funcs)
    return [f for f in rf if idx.find(f)], 'paired', notes


def run(retail, ours_funcs, objects=None, all_retail=False, target_check=True, include=None):
    rf, mode, notes = scope(retail, ours_funcs, objects, all_retail)
    results, extra = compare.pair_all(rf, ours_funcs, target_check, include)
    # ours functions outside the scope that retail has elsewhere (inline/COMDAT copies
    # the debug map attributes to another object): compared, but reported separately
    shared, only_ours = [], []
    if mode == 'objects':
        in_scope = set(rf)
        others = compare.Index([f for f in retail.functions if f not in in_scope])
        for e in extra:
            r = others.find(e.ours)
            if r is not None:
                shared.append(compare.compare(r, e.ours, target_check))
            else:
                only_ours.append(e)
    else:
        only_ours = extra
    return dict(results=results, shared=shared, extra=only_ours, mode=mode, notes=notes)


def summarize(results):
    c = collections.Counter(r.status for r in results)
    b = collections.Counter()
    for r in results:
        b[r.status] += r.size
    nf = len(results)
    nb = sum(b.values())
    matched_f = c[EXACT] + c[MASKED]
    matched_b = b[EXACT] + b[MASKED]
    return dict(
        functions=dict(total=nf, exact=c[EXACT], matches_after_masking=c[MASKED], differs=c[DIFFERS],
                       missing=c[MISSING], matched=matched_f, pct=round(100 * matched_f / nf, 2) if nf else 0.0),
        bytes=dict(total=nb, exact=b[EXACT], matches_after_masking=b[MASKED], differs=b[DIFFERS],
                   missing=b[MISSING], matched=matched_b, pct=round(100 * matched_b / nb, 2) if nb else 0.0))


def per_object(results):
    groups = collections.defaultdict(list)
    for r in results:
        groups[(r.retail.obj_path if r.retail and r.retail.obj_path else None) or r.obj or '?'].append(r)
    rows = []
    for path, rs in sorted(groups.items(), key=lambda kv: kv[0].lower()):
        s = summarize(rs)
        rows.append(dict(object=rs[0].obj or path, path=path, **{k: v for k, v in s['functions'].items()},
                         bytes=s['bytes']['total'], matched_bytes=s['bytes']['matched'], pct_bytes=s['bytes']['pct']))
    return rows


def result_json(r, diff_lines=0):
    d = dict(name=r.name, key=r.key, status=r.status, object=r.obj)
    if r.retail:
        d.update(retail_addr=hex(r.retail.addr), retail_size=r.retail.size, retail_object=r.retail.obj_path)
        if not r.retail.size_known:
            d['retail_size_known'] = False
    if r.ours:
        d.update(ours_size=r.ours.size, ours_object=r.ours.obj_path or r.ours.module.label)
    if r.reason:
        d['reason'] = r.reason
    if r.first_diff is not None:
        d['first_diff'] = r.first_diff
    if r.target_mismatches:
        d['target_mismatches'] = [dict(offset=k, ours=t, retail=rt) for k, t, rt in r.target_mismatches]
    if diff_lines and r.status == DIFFERS and r.retail and r.ours:
        d['diff'] = compare.instruction_diff(r, context=1, limit=diff_lines)
    return d


def write(out_prefix, run_info, data, diff_lines=12, top=40):
    results, shared, extra = data['results'], data['shared'], data['extra']
    summary = summarize(results)
    objs = per_object(results)
    doc = dict(run_info, scope=data['mode'], notes=data['notes'], capstone=x86.have_capstone(),
               generated=datetime.datetime.now().isoformat(timespec='seconds'),
               summary=summary, shared_summary=summarize(shared), objects=objs,
               functions=[result_json(r, diff_lines) for r in results],
               shared=[result_json(r, diff_lines) for r in shared],
               extra=[dict(name=e.name, key=e.key, object=e.obj, size=e.ours.size) for e in extra])
    with open(out_prefix + '.json', 'w') as fh:
        json.dump(doc, fh, indent=1)
    with open(out_prefix + '.md', 'w') as fh:
        fh.write(markdown(doc, results, top, diff_lines))
    return doc


def _pct(a, b):
    return f'{100 * a / b:.1f}%' if b else '-'


def markdown(doc, results, top, diff_lines):
    s, ss = doc['summary'], doc['shared_summary']
    f, b = s['functions'], s['bytes']
    pretty = demangle([r.key for r in results if r.status in (DIFFERS, MISSING)])
    L = [f'# bytematch report', '',
         f'- retail: `{doc["retail"]}`', f'- ours: `{doc["ours"]}`',
         f'- scope: {doc["scope"]}; capstone: {"yes" if doc["capstone"] else "no (byte-level fallback)"}; '
         f'generated {doc["generated"]}', '']
    for n in doc['notes']:
        L.append(f'> note: {n}')
    if doc['notes']:
        L.append('')
    L += ['| | functions | bytes |', '|---|---:|---:|',
          f'| in scope | {f["total"]} | {b["total"]} |',
          f'| exact | {f["exact"]} | {b["exact"]} |',
          f'| matches after masking | {f["matches_after_masking"]} | {b["matches_after_masking"]} |',
          f'| differs | {f["differs"]} | {b["differs"]} |',
          f'| missing in ours | {f["missing"]} | {b["missing"]} |',
          f'| **matched** | **{f["matched"]} ({f["pct"]}%)** | **{b["matched"]} ({b["pct"]}%)** |', '']
    if ss['functions']['total']:
        sf = ss['functions']
        L += [f'Out of scope but present in retail under another object (inline/COMDAT copies): '
              f'{sf["matched"]}/{sf["total"]} matched ({sf["pct"]}%).', '']
    L += [f'Functions only in ours (inlined, renamed or new): {len(doc["extra"])}.', '']
    if len(doc['objects']) > 1:
        L += ['## Per object', '', '| object | funcs | matched | % funcs | % bytes | differs | missing |',
              '|---|---:|---:|---:|---:|---:|---:|']
        for o in doc['objects']:
            L.append(f'| {o["object"]} | {o["total"]} | {o["matched"]} | {o["pct"]}% | {o["pct_bytes"]}% | '
                     f'{o["differs"]} | {o["missing"]} |')
        L.append('')
    diffs = sorted((r for r in results if r.status == DIFFERS), key=lambda r: -r.size)
    if diffs:
        L += [f'## Largest differing functions (top {min(top, len(diffs))} of {len(diffs)})', '']
        for r in diffs[:top]:
            L.append(f'### `{pretty.get(r.key, r.key)}`')
            L.append(f'{r.obj} · retail {r.retail.size} B @ {r.retail.addr:#x}, ours {r.ours.size} B · {r.reason}')
            lines = compare.instruction_diff(r, context=1, limit=diff_lines) if diff_lines else []
            if lines:
                L += ['```diff'] + lines + ['```']
            L.append('')
    missing = [r for r in results if r.status == MISSING]
    if missing:
        L += [f'## Missing in ours ({len(missing)})', '']
        for r in sorted(missing, key=lambda r: (r.obj or '', r.key))[:200]:
            L.append(f'- {r.obj}: `{pretty.get(r.key, r.key)}` ({r.retail.size} B)')
        if len(missing) > 200:
            L.append(f'- ... {len(missing) - 200} more in the JSON')
        L.append('')
    return '\n'.join(L)


def print_table(results, out=sys.stdout, show_all=True, names=None):
    order = {DIFFERS: 0, MISSING: 1, MASKED: 2, EXACT: 3, EXTRA: 4}
    tag = {EXACT: 'exact', MASKED: 'masked', DIFFERS: 'DIFFERS', MISSING: 'missing', EXTRA: 'only-ours'}
    for r in sorted(results, key=lambda r: (order[r.status], r.obj or '', r.key)):
        if not show_all and r.status in MATCHED:
            continue
        rs = r.retail.size if r.retail else '-'
        os_ = r.ours.size if r.ours else '-'
        nm = names.get(r.key, r.key) if names else r.key
        extra = f'  [{r.reason}]' if r.reason else ''
        print(f'{tag[r.status]:9s} {str(rs):>6s} {str(os_):>6s}  {r.obj or "?"}: {nm}{extra}', file=out)
