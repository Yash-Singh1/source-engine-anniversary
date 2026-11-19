"""Scoping, summaries and JSON/markdown report output."""
import collections
import re
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
    if any(f.obj for f in rf) and retail.fmt in ('macho', 'pe'):
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


def run(retail, ours_funcs, objects=None, all_retail=False, target_check=True, include=None, data_check=None):
    """`data_check`: (retail data.Image, {ours module path: data.Image}) to also resolve the
    data references of matched functions (strings, globals, constants) on both sides."""
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
    if target_check:
        n = accept_equivalent_targets(results + shared, retail, ours_funcs)
        if n:
            notes.append(f'{n} functions differed only in call targets whose code is identical on both '
                         f'sides (identical-code folding named them differently); counted as matching')
        n = accept_folded_instantiations(results + shared, retail)
        if n:
            notes.append(f'{n} functions match a retail function of the same name with other template '
                         f'arguments (the instantiations are folded in retail, and the map named the copy '
                         f'after another one); counted as matching')
        n = accept_misnamed(results + shared, retail, only_ours, ours_funcs)
        if n:
            notes.append(f'{n} functions match exactly one retail function or one of ours that the name map '
                         f'paired with something else (the map swapped names between look-alike functions); '
                         f'counted as matching')
    if data_check:
        apply_data_check(results + shared, *data_check)
    return dict(results=results, shared=shared, extra=only_ours, mode=mode, notes=notes)


def _untemplated(name):
    prev = None
    while prev != name:
        prev, name = name, re.sub(r'<[^<>]*>', '', name)
    return name


def accept_folded_instantiations(results, retail):
    """Pass a template function that matches another retail instantiation of the same name.

    Identical-code folding keeps one copy of instantiations that compile to the same code,
    and the name map can only call it after one of them."""
    by = {}
    for f in retail.functions:
        for n in [f.name] + list(f.aliases):
            if '<' in n:
                by.setdefault(_untemplated(n), set()).add(f)
    moved = 0
    for res in results:
        if res.status != compare.DIFFERS or not res.ours or not res.retail or '<' not in res.key:
            continue
        for r in by.get(_untemplated(res.key), ()):
            if r is res.retail or not r.data:
                continue
            c = compare.compare(r, res.ours, True)
            if c.status in (compare.EXACT, compare.MASKED):
                res.status, res.reason, res.first_diff = c.status, '', None
                res.line_constants, res.target_mismatches = None, []
                res.matched_at = r.addr
                moved += 1
                break
    return moved


MISNAMED_MIN_SIZE = 8


def _head(f):
    """Leading bytes no relocation of f covers (up to 4): the bucket key for a quick pre-filter."""
    return f.data[:min([rel.offset for rel in f.relocs] + [4])]


def _unique_match(cands, key, accept, tiebreak):
    hits = [c for c in cands.get(key, ()) if accept(c)]
    if len(hits) > 1:
        hits = [c for c in hits if tiebreak(c)]
    return hits[0] if len(hits) == 1 else None


def _targets_agree(r, o, oby):
    """False when one of o's relocations names a function of ours whose code differs from the
    retail code the same field points at (vcall thunks for member pointers, callees)."""
    rmod = r.module
    if not rmod.linked or rmod.pic:
        return True
    for rel in o.relocs:
        of = oby.get(rel.target)
        if of is None or of is o or rel.size != 4 or rel.offset + 4 > len(r.data):
            continue
        k = rel.offset
        if rel.kind == 'pcrel':
            if not (k >= 1 and r.data[k - 1] in (0xe8, 0xe9)):
                continue
            va = (r.addr + k + 4 + x86.s32(r.data, k)) & 0xffffffff
        elif rel.kind == 'abs':
            va = (x86.u32(r.data, k) - rel.addend) & 0xffffffff
        else:
            continue
        if not of.relocs:
            if rmod.read(va, len(of.data)) != of.data:
                return False
            continue
        rf = rmod.function_at(va)
        if rf is not None and rf.addr == va and compare.compare(rf, of, False).status not in MATCHED:
            return False
    return True


def _literals_agree(r, o):
    """False when one of o's absolute references to an MSVC string literal points at other
    bytes in the retail image (look-alike ConVar/ConCommand/shader-parameter initializers
    differ only in the strings they pass)."""
    lits, rmod = o.module.literals if o.module else None, r.module
    if not lits or not rmod.regions:
        return True
    for rel in o.relocs:
        lit = lits.get(rel.target)
        if lit is None or rel.kind != 'abs' or rel.size != 4 or rel.offset + 4 > len(r.data):
            continue
        if rmod.read_data((x86.u32(r.data, rel.offset) - rel.addend) & 0xffffffff, len(lit)) != lit:
            return False
    return True


def accept_misnamed(results, retail, spare, ours_funcs):
    """Pair functions the name map gave the wrong name.

    Maps ported by call-graph and byte similarity can rotate names among look-alike functions
    (the Create_<Panel> build factories differ only in the allocation size; every
    PanelMessageFunc_<name>::InitVar has the same shape). A differing function passes when it
    matches exactly one retail function no result matches; a missing retail function passes
    when it matches exactly one of our unpaired functions (`spare`, removed from the list when
    used). The target check applies to both; look-alikes that differ only in what a relocation
    points at are told apart by comparing the code there, and on PE by comparing the string
    literals they reference (the port names every MSVC dynamic initializer __GLOBAL__I_a).
    Functions under MISNAMED_MIN_SIZE bytes stay as they are."""
    claimed = {res.retail.addr for res in results if res.status in MATCHED and res.retail}
    rpool = collections.defaultdict(list)
    for f in retail.functions:
        if f.addr not in claimed and len(f.data) >= MISNAMED_MIN_SIZE:
            for n in (1, 2, 3, 4):
                rpool[f.data[:n]].append(f)
    opool = collections.defaultdict(list)
    for e in spare:
        o = e.ours
        if len(o.data) >= MISNAMED_MIN_SIZE:
            for n in (1, 2, 3, 4):
                opool[o.data[:n]].append(o)
    oby = {}
    for f in ours_funcs:
        for n in [f.name] + list(f.aliases):
            oby.setdefault(n, f)
    used, moved = set(), 0

    def fits(r, o):
        return len(r.data) == len(o.data) if r.size_known else len(r.data) >= len(o.data)

    def same(r, o):
        return compare.compare(r, o, True).status in MATCHED and _literals_agree(r, o)

    for res in results:
        if res.status == compare.DIFFERS and res.ours and res.retail:
            o = res.ours
            if len(o.data) < MISNAMED_MIN_SIZE or not _head(o):
                continue
            r = _unique_match(rpool, _head(o), lambda r: r is not res.retail and r.addr not in claimed
                              and fits(r, o) and same(r, o), lambda r: _targets_agree(r, o, oby))
            if r is None:
                continue
        elif res.status == compare.MISSING and res.retail:
            r = res.retail
            if len(r.data) < MISNAMED_MIN_SIZE or not _head(r):
                continue
            o = _unique_match(opool, _head(r), lambda o: id(o) not in used and fits(r, o) and same(r, o),
                              lambda o: _targets_agree(r, o, oby))
            if o is None:
                continue
            used.add(id(o))
            res.ours = o
            res.matched_as = o.name
        else:
            continue
        res.status, res.reason, res.first_diff = compare.compare(r, o, True).status, '', None
        res.line_constants, res.target_mismatches = None, []
        if r is not res.retail:
            res.matched_at = r.addr
        claimed.add(r.addr)
        moved += 1
    spare[:] = [e for e in spare if id(e.ours) not in used]
    return moved


def accept_equivalent_targets(results, retail, ours_funcs):
    """Pass functions whose only differences are call targets that are the same code.

    Identical-code folding (/OPT:ICF, ld64's coalescing) leaves one copy of identical
    functions under one name, and the retail name map can only give that address one name
    (e.g. FreeCircularBuffer for the folded operator delete). A target is equivalent when our
    target function and the retail function at the target address match each other."""
    rby, oby = {}, {}
    for f in retail.functions:
        for n in [f.name] + list(f.aliases):
            rby.setdefault(n, f)
    for f in ours_funcs:
        for n in [f.name] + list(f.aliases):
            oby.setdefault(n, f)
    same, moved = {}, 0
    for res in results:
        if res.status != compare.DIFFERS or not res.target_mismatches or res.line_constants:
            continue
        ok = True
        for _, ot, rt in res.target_mismatches:
            if (ot, rt) not in same:
                o, r = oby.get(ot), rby.get(rt)
                same[(ot, rt)] = bool(o and r and o.data and r.data and
                                      compare.compare(r, o, False).status in (compare.EXACT, compare.MASKED))
            ok = ok and same[(ot, rt)]
        if ok:
            res.equivalent_targets, res.target_mismatches = res.target_mismatches, []
            res.status, res.reason, res.first_diff = compare.MASKED, '', None
            moved += 1
    return moved


def apply_data_check(results, rimg, oimgs):
    from . import data
    for r in results:
        if r.status not in MATCHED:
            continue
        oimg = oimgs.get(r.ours.module.path)
        if oimg is None:
            continue
        mism = [m for m in data.check_function(rimg, oimg, r.retail, r.ours) if not data.same_modulo_linkage(m[1], m[2])]
        if mism:
            k, ri, oi = mism[0]
            r.status, r.first_diff, r.data_mismatches = DIFFERS, k, mism
            r.reason = f'data ref at +{k:#x} ({data.kind_of(ri, oi)}): retail {data.show(ri, 60)}, ours {data.show(oi, 60)}'


def summarize(results):
    c = collections.Counter(r.status for r in results)
    b = collections.Counter()
    for r in results:
        b[r.status] += r.size
    nf = len(results)
    nb = sum(b.values())
    lc = [r for r in results if r.status == DIFFERS and r.line_constants]
    matched_f = c[EXACT] + c[MASKED]
    matched_b = b[EXACT] + b[MASKED]
    return dict(
        line_constants_only=dict(functions=len(lc), bytes=sum(r.size for r in lc)),
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
    if r.line_constants:
        d['line_constants'] = [dict(offset=o, retail=a, ours=b) for o, a, b in r.line_constants]
    if r.data_mismatches:
        from . import data
        d['data_mismatches'] = [dict(offset=k, kind=data.kind_of(a, b), retail=data.show(a), ours=data.show(b))
                                for k, a, b in r.data_mismatches]
    if r.target_mismatches:
        d['target_mismatches'] = [dict(offset=k, ours=t, retail=rt) for k, t, rt in r.target_mismatches]
    if getattr(r, 'matched_at', None):
        d['matched_at'] = hex(r.matched_at)
    if getattr(r, 'matched_as', None):
        d['matched_as'] = r.matched_as
    if getattr(r, 'equivalent_targets', None):
        d['equivalent_targets'] = [dict(offset=k, ours=t, retail=rt) for k, t, rt in r.equivalent_targets]
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
    L = ['# bytematch report', '',
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
