"""Per-module data audit: string literals, constant data, and the data behind
masked references of matched functions, with source locations.

Both sides are Mach-O i386; ours is usually the linked module of a
--retail-osx32 build (one object also works, with less attribution)."""
import collections
import json
import os
import re
import subprocess

from . import compare, data, loader, report
from .model import object_stem

VALVE_SRC = re.compile(r'^/Users/buildbot/buildslave/[^/]+/build/src/')
SOURCE_GLOBS = ('*.c', '*.cc', '*.cpp', '*.cxx', '*.h', '*.hpp', '*.inc', '*.mm', '*.m')
_FMT = re.compile(rb'%[-+ #0]*(?:\d+|\*)?(?:\.(?:\d+|\*)?)?(?:hh|h|ll|l|L|q|j|z|t|I64|I32)?[diouxXeEfFgGaAcspn%]')


def norm_path_literal(b):
    """__FILE__-style literal with the build location removed, or None."""
    try:
        s = b.decode('latin-1')
    except Exception:
        return None
    if not re.search(r'\.(cpp|cxx|cc|c|h|hpp|inl|mm)$', s, re.I) or ' ' in s:
        return None
    s = VALVE_SRC.sub('', s)
    s = re.sub(r'^(\.\./)+', '', s)
    s = re.sub(r'^.*/(?:se-bm-[^/]+|source-engine[^/]*|sdk2013mp)/', '', s)
    s = re.sub(r'^sdk2013mp/', '', s)
    return s.lower()


_STAMP = re.compile(rb'^(?:[A-Z][a-z]{2} [ \d]\d \d{4}|\d\d:\d\d:\d\d)$')
BUILD_SIDE = ('path', 'build-stamp')
NAME_ONLY = ('renamed',)                 # same bytes; only the symbol table differs


def _same_text(a, b):
    """A literal on one side and anonymous constant bytes holding the same C string on the other."""
    s, l = (a, b) if a[0] == 'str' else (b, a)
    return s[0] == 'str' and l[0] == 'lit' and s[1] is not None and l[1][:len(s[1]) + 1] == s[1] + b'\0'


def classify(rt, ot):
    """Category of a retail/ours literal pair."""
    if rt is None or ot is None:
        return 'text'
    if _STAMP.match(rt) and _STAMP.match(ot):
        return 'build-stamp'
    pr, po = norm_path_literal(rt), norm_path_literal(ot)
    if pr and pr == po:
        return 'path'
    if _FMT.sub(b'%', rt) == _FMT.sub(b'%', ot):
        return 'format'
    if rt.strip() == ot.strip() or rt.lower() == ot.lower():
        return 'whitespace/case'
    return 'text'


class Sources:
    """Finds where a literal is written in the tree (and in the pristine leak)."""

    def __init__(self, tree, pristine=None):
        self.tree = os.path.abspath(tree) if tree else None
        self.pristine = pristine if pristine and os.path.isdir(pristine) else None
        self._lines = {}

    def lines(self, path):
        if path not in self._lines:
            try:
                with open(path, 'rb') as fh:
                    self._lines[path] = fh.read().decode('latin-1').split('\n')
            except OSError:
                self._lines[path] = None
        return self._lines[path]

    def rel(self, path):
        for root, tag in ((self.tree, ''), (self.pristine, 'pristine:')):
            if root and path.startswith(root + '/'):
                return tag + path[len(root) + 1:]
        return path

    def object_files(self, img, obj_path):
        """Source and header files the debug map lists for one object, in our tree."""
        for o in img.objects:
            if o['path'] != obj_path:
                continue
            files = [o['source']] + o['headers']
            out = []
            for f in files:
                if not f:
                    continue
                if VALVE_SRC.match(f):
                    f = os.path.join(self.tree, VALVE_SRC.sub('', f)) if self.tree else f
                f = os.path.normpath(f)
                if f not in out and os.path.isfile(f):
                    out.append(f)
            return out
        return []

    @staticmethod
    def needles(text):
        """Escaped forms to search for: the whole literal, then its longest plain piece."""
        full = data.c_escape(text)
        out = []
        if len(full) >= 4:
            out.append((full, True))
        pieces = re.split(r'\\[ntr"\\]|\\x[0-9a-f]{2}|%[-+ #0-9.*]*[a-zA-Z%]', full)
        best = max(pieces, key=len) if pieces else ''
        if len(best) >= 6 and best != full:
            out.append((best, False))
        return out

    def find_in_files(self, files, text, limit=3):
        hits = []
        for needle, exact in self.needles(text):
            q = '"' + needle + '"' if exact else needle
            for f in files:
                ls = self.lines(f)
                if not ls:
                    continue
                for i, line in enumerate(ls):
                    if q in line or (exact and needle in line):
                        hits.append(dict(file=self.rel(f), line=i + 1, exact=exact))
                        if len(hits) >= limit:
                            return hits
            if hits:
                return hits
        return hits

    def grep(self, root, texts, limit=40):
        """{text: [hits]} with one ripgrep pass over root for many literals."""
        if not root or not texts:
            return {}
        want = {}
        for t in texts:
            for needle, exact in self.needles(t):
                want.setdefault(needle, []).append((t, exact))
                break
        if not want:
            return {}
        pat = os.path.join('/tmp', f'bytematch-needles-{os.getpid()}.txt')
        with open(pat, 'w', encoding='latin-1') as fh:
            fh.write('\n'.join(want) + '\n')
        cmd = ['rg', '--json', '-F', '-f', pat, '--no-ignore', '-g', '!build*', '-g', '!.git']
        for g in SOURCE_GLOBS:
            cmd += ['-g', g]
        cmd.append(root)
        out = collections.defaultdict(list)
        try:
            p = subprocess.run(cmd, capture_output=True, timeout=600)
        except (OSError, subprocess.SubprocessError):
            return {}
        finally:
            os.unlink(pat)
        for line in p.stdout.split(b'\n'):
            if not line.startswith(b'{"type":"match"'):
                continue
            m = json.loads(line)['data']
            path = m['path'].get('text')
            for sm in m['submatches']:
                needle = sm['match'].get('text')
                for t, exact in want.get(needle, ()):
                    if len(out[t]) < limit:
                        out[t].append(dict(file=self.rel(path), line=m['line_number'], exact=exact))
        return out


def _prefer(img, objs):
    return objs[0] if objs else None


def _context(img, func, off):
    """The longer literal referenced nearest to `off` in the same function."""
    best = None
    for k, (t, _) in data.code_refs(img, func).items():
        txt = img.cstring_at(t)
        if k != off and txt and len(txt) >= 4 and (best is None or abs(k - off) < best[0]):
            best = (abs(k - off), txt)
    return best[1] if best else None


def rank_hits(hits, objects, limit=3):
    """Order source hits: the owning object's file first, then its directory, then non-thirdparty."""
    stems = {object_stem(o).lower() for o in objects if o}

    def score(h):
        f = h['file'].split(':', 1)[-1]
        stem = object_stem(f).lower()
        third = f.startswith(('thirdparty/', 'external/', 'lib/')) or '/test' in f or '/examples/' in f
        return (0 if stem in stems else 2 if not third else 3, not h.get('exact'), f, h['line'])
    out = sorted(hits, key=score)[:limit]
    for h in out:
        h['owner'] = object_stem(h['file'].split(':', 1)[-1]).lower() in stems
    return out


def audit(retail_path, ours_path, tree=None, pristine=None, sources=True, log=None):
    """Audit one module pair; returns a JSON-able dict."""
    def note(msg):
        if log:
            log(msg)
    rmod = loader.load_one(retail_path)
    omods = loader.load_path(ours_path, quiet=True)
    ofuncs = [f for m in omods for f in m.functions]
    rimg = data.Image(retail_path)
    oimg = data.Image(ours_path)
    note('loaded')

    run = report.run(rmod, ofuncs)
    pairs = [r for r in run['results'] + run['shared'] if r.status in (compare.EXACT, compare.MASKED)]
    note(f'paired {len(pairs)} matched functions')

    # 1. data references of matched functions
    fmis, linkage = [], []
    for r in pairs:
        for off, ri, oi in data.check_function(rimg, oimg, r.retail, r.ours):
            if data.same_modulo_linkage(ri, oi):
                linkage.append(dict(function=r.key, object=r.retail.obj_path, offset=off,
                                    retail=data.show(ri), ours=data.show(oi)))
                continue
            kind = data.kind_of(ri, oi)
            cat = None
            if kind == 'string' and ri[0] in ('str', 'cfstr') and oi[0] in ('str', 'cfstr'):
                cat = classify(ri[1], oi[1])
            elif kind == 'string' and _same_text(ri, oi):
                cat = 'placement'
            elif data.renamed(rimg, oimg, ri, oi):
                cat = 'renamed'
            elif ri[0] == oi[0] == 'sym':
                cat = 'layout' if ri[1] == oi[1] else 'other-symbol'
            item = dict(function=r.key, object=r.retail.obj_path, ours_object=r.ours.obj_path, offset=off,
                        kind=kind, category=cat, retail=data.show(ri, 400), ours=data.show(oi, 400),
                        _r=ri, _o=oi, _oo=r.ours.obj_path)
            if kind == 'string' and len(oi[1] or b'') < 4:
                # short literals (ConVar defaults): name the closest longer literal referenced before it
                ctx = _context(oimg, r.ours, off)
                if ctx:
                    item['context'], item['_ctx'] = data.c_quote(ctx), ctx
            fmis.append(item)
    note(f'function data checks: {len(fmis)} mismatches')

    # 2. literal sets and order
    st = data.compare_strings(rimg, oimg, rmod.functions, [f for m in omods for f in m.functions])
    note(f'strings: retail {st["retail"]} ours {st["ours"]} missing {len(st["missing"])} extra {len(st["extra"])}')

    # 3. constant data symbols
    dsyms = data.compare_data(rimg, oimg) if oimg.linked else []
    note(f'data symbols differing: {len(dsyms)}')

    # differing pairs: function evidence first, then data slots, then similarity
    miss_ix = {m['text']: i for i, m in enumerate(st['missing'])}
    extra_ix = {e['text']: i for i, e in enumerate(st['extra'])}
    differing, paired_m, paired_e = [], set(), set()

    def add_pair(rt, ot, how, where, obj, ours_obj):
        i, j = miss_ix.get(rt), extra_ix.get(ot)
        key = (rt, ot)
        for d in differing:
            if (d['_rt'], d['_ot']) == key:
                if where and where not in d['used_by']:
                    d['used_by'].append(where)
                return
        if i is not None:
            paired_m.add(i)
        if j is not None:
            paired_e.add(j)
        differing.append(dict(retail=data.c_quote(rt), ours=data.c_quote(ot), category=classify(rt, ot), evidence=how,
                              object=obj, used_by=[where] if where else [], _rt=rt, _ot=ot, _oo=ours_obj))

    for m in fmis:
        if (m['kind'] == 'string' and m['_r'][0] in ('str', 'cfstr') and m['_o'][0] in ('str', 'cfstr')
                and m['_r'][1] is not None and m['_o'][1] is not None):
            add_pair(m['_r'][1], m['_o'][1], 'function', m['function'], m['object'], m['_oo'])
    for d in dsyms:
        for x in d['diffs']:
            ri, oi = x.pop('_r', None), x.pop('_o', None)
            # only literals missing on the other side: a shifted table pairs existing literals wrongly
            if (ri and oi and ri[0] in ('str', 'cfstr') and oi[0] in ('str', 'cfstr')
                    and ri[1] in miss_ix and oi[1] in extra_ix):
                add_pair(ri[1], oi[1], 'data', f'{d["name"]}+{x["offset"]}', d['object'], None)
    mi = [i for i in range(len(st['missing'])) if i not in paired_m]
    ej = [j for j in range(len(st['extra'])) if j not in paired_e]
    for a, b, ratio in data.pair_similar([st['missing'][i] for i in mi], [st['extra'][j] for j in ej]):
        m, e = st['missing'][mi[a]], st['extra'][ej[b]]
        add_pair(m['text'], e['text'], f'similar {ratio}', None, _prefer(rimg, m['objects']), _prefer(oimg, e['objects']))

    # source locations
    src = Sources(tree, pristine) if sources and tree else None
    if src:
        need_tree = []
        for d in differing:
            files = src.object_files(oimg, d['_oo']) if d['_oo'] else []
            d['source'] = src.find_in_files(files, d['_ot']) if files else []
            if not d['source']:
                need_tree.append(d['_ot'])
        unpaired_e = [e for j, e in enumerate(st['extra']) if j not in paired_e]
        for e in unpaired_e:
            files = []
            for ob in e['objects'][:2]:
                files += src.object_files(oimg, ob)
            e['source'] = src.find_in_files(files, e['text']) if files else []
            if not e['source']:
                need_tree.append(e['text'])
        unpaired_m = [m for i, m in enumerate(st['missing']) if i not in paired_m]
        g_tree = src.grep(src.tree, need_tree + [m['text'] for m in unpaired_m])
        g_pristine = src.grep(src.pristine, [m['text'] for m in unpaired_m] + [d['_rt'] for d in differing])
        for d in differing:
            objs = [d['object'], d['_oo']]
            d['source'] = d['source'] or rank_hits(g_tree.get(d['_ot'], []), objs)
            d['pristine'] = rank_hits(g_pristine.get(d['_rt'], []), objs)
        for e in unpaired_e:
            e['source'] = e['source'] or rank_hits(g_tree.get(e['text'], []), e['objects'])
        for m in unpaired_m:
            m['in_tree'] = rank_hits(g_tree.get(m['text'], []), m['objects'])
            m['pristine'] = rank_hits(g_pristine.get(m['text'], []), m['objects'])
            # in our copy of the owning file but not in our module: not compiled, or a different code path
            m['in_owner_source'] = any(h.get('owner') for h in m['in_tree'])
        for m in fmis:
            if m['kind'] == 'string':
                d = next((d for d in differing if d['_rt'] == m['_r'][1] and d['_ot'] == m['_o'][1]), None)
                m['source'] = d['source'] if d else []
                if not m['source'] and m.get('_ctx'):
                    files = src.object_files(oimg, m['_oo']) if m['_oo'] else []
                    m['source'] = src.find_in_files(files, m['_ctx']) if files else []
                    if d is not None and not d['source']:
                        d['source'], d['context'] = m['source'], m['context']
    note('sources located')

    # per-object rollup (retail object paths; ours-only literals by our object stem)
    per = collections.defaultdict(lambda: collections.Counter())
    for i, m in enumerate(st['missing']):
        if i not in paired_m:
            for ob in m['objects'][:1] or ['?']:
                per[object_stem(ob)]['missing'] += 1
    for j, e in enumerate(st['extra']):
        if j not in paired_e:
            for ob in e['objects'][:1] or ['?']:
                per[object_stem(ob)]['extra'] += 1
    for d in differing:
        per[object_stem(d['object'] or d['_oo'] or '?')]['differing'] += 1
    for m in fmis:
        per[object_stem(m['object'] or '?')]['functions_' + ('string' if m['kind'] == 'string' else 'other')] += 1
    for d in dsyms:
        per[object_stem(d['object'] or '?')]['data_' + ('string' if d['kind'] == 'string' else 'other')] += 1
    for m in st['moved']:
        for ob in m['objects'][:1] or ['?']:
            per[object_stem(ob)]['moved'] += 1

    def clean(d):
        return {k: v for k, v in d.items() if not k.startswith('_')}

    def lit(m):
        out = dict(m)
        out['text'] = data.c_quote(m['text'])
        out['category'] = 'path' if norm_path_literal(m['text']) else None
        for k in ('retail_addr', 'ours_addr'):
            if k in out:
                out[k] = hex(out[k])
        return out

    bad_all = {(m['function'], m['object']) for m in fmis}
    bad_funcs = {(m['function'], m['object']) for m in fmis if m.get('category') not in BUILD_SIDE + NAME_ONLY}
    bad_string_funcs = {(m['function'], m['object']) for m in fmis
                        if m['kind'] == 'string' and m.get('category') not in BUILD_SIDE}
    return dict(
        retail=os.path.abspath(retail_path), ours=os.path.abspath(ours_path),
        summary=dict(
            retail_strings=st['retail'], ours_strings=st['ours'], common=st['common'], in_order=st['in_order'],
            moved=len(st['moved']), missing=len(st['missing']) - len(paired_m), extra=len(st['extra']) - len(paired_e),
            differing=len(differing),
            differing_by_category=dict(collections.Counter(d['category'] for d in differing)),
            matched_functions=len(pairs), functions_not_truly_matching=len(bad_funcs),
            functions_build_side_only=len({(m['function'], m['object']) for m in fmis
                                           if m.get('category') in BUILD_SIDE} - bad_funcs),
            functions_renamed_only=len({(m['function'], m['object']) for m in fmis
                                        if m.get('category') in NAME_ONLY} - bad_funcs),
            functions_string_mismatch=len(bad_string_funcs),
            function_mismatch_kinds=dict(collections.Counter(m['kind'] for m in fmis)),
            function_mismatch_categories=dict(collections.Counter(m.get('category') or m['kind'] for m in fmis)),
            missing_in_owner_source=sum(1 for i, m in enumerate(st['missing']) if i not in paired_m and m.get('in_owner_source')),
            linkage_only=len(linkage),
            data_symbols_differing=len(dsyms), data_symbol_kinds=dict(collections.Counter(d['kind'] for d in dsyms))),
        differing=[clean(d) for d in differing],
        functions=[clean(m) for m in fmis],
        linkage=linkage,
        missing=[lit(m) for i, m in enumerate(st['missing']) if i not in paired_m],
        extra=[lit(e) for j, e in enumerate(st['extra']) if j not in paired_e],
        moved=[dict(text=data.c_quote(m['text'], 200), objects=m['objects']) for m in st['moved']],
        data_symbols=dsyms,
        objects={k: dict(v) for k, v in sorted(per.items())})


def markdown(doc, title=None, top=60):
    s = doc['summary']
    L = [f'# {title or "bytematch data audit"}', '', f'- retail: `{doc["retail"]}`', f'- ours: `{doc["ours"]}`', '',
         '| | count |', '|---|---:|',
         f'| retail literals / ours | {s["retail_strings"]} / {s["ours_strings"]} |',
         f'| common, in order | {s["common"]}, {s["in_order"]} (moved {s["moved"]}) |',
         f'| differing pairs | {s["differing"]} {s["differing_by_category"] or ""} |',
         f'| retail-only / ours-only (unpaired) | {s["missing"]} / {s["extra"]} |',
         f'| matched functions with differing data refs | {s["functions_not_truly_matching"]} of {s["matched_functions"]} '
         f'(strings: {s["functions_string_mismatch"]}; plus {s["functions_build_side_only"]} with only __FILE__/__DATE__ '
         f'and {s.get("functions_renamed_only", 0)} with only renamed symbols) |',
         f'| function mismatch categories | {s["function_mismatch_categories"] or ""} |',
         f'| data symbols differing | {s["data_symbols_differing"]} {s["data_symbol_kinds"] or ""} |', '']
    if doc['functions']:
        L += ['## Matched functions whose data references differ', '']
        for m in doc['functions'][:top]:
            loc = ', '.join(f'{h["file"]}:{h["line"]}' for h in m.get('source', [])[:2])
            L.append(f'- `{m["function"]}` +{m["offset"]:#x} ({m["kind"]}{", " + m["category"] if m.get("category") else ""}) '
                     f'{object_stem(m["object"] or "?")}: retail `{m["retail"]}` ours `{m["ours"]}`' + (f' at {loc}' if loc else ''))
        L.append('')
    if doc['differing']:
        L += ['## Differing literals', '']
        for d in doc['differing'][:top]:
            loc = ', '.join(f'{h["file"]}:{h["line"]}' for h in d.get('source', [])[:2])
            L.append(f'- [{d["category"]}; {d["evidence"]}] {object_stem(d["object"] or "?")}: retail `{d["retail"][:200]}` '
                     f'ours `{d["ours"][:200]}`' + (f' at {loc}' if loc else ''))
        L.append('')
    strd = [d for d in doc['data_symbols'] if d['kind'] == 'string']
    if strd:
        L += ['## Data symbols with differing text', '']
        for d in strd[:top]:
            x = next((x for x in d['diffs'] if x['kind'] == 'string'), d['diffs'][0] if d['diffs'] else None)
            if x is None:
                continue
            L.append(f'- `{d["name"]}` ({d["section"]}, {object_stem(d["object"] or "?")}) +{x["offset"]}: '
                     f'retail `{x["retail"][:160]}` ours `{x["ours"][:160]}`')
        L.append('')
    for key, label in (('missing', 'Retail-only literals'), ('extra', 'Ours-only literals')):
        if doc[key]:
            L += [f'## {label} ({len(doc[key])}, first {min(top, len(doc[key]))})', '']
            for m in doc[key][:top]:
                where = m.get('source') or m.get('in_tree') or m.get('pristine') or []
                loc = ', '.join(f'{h["file"]}:{h["line"]}' for h in where[:2])
                tag = ' **text is in our copy of the file**' if m.get('in_owner_source') else ''
                L.append(f'- {", ".join(object_stem(o) for o in m["objects"][:2]) or "?"}: `{m["text"][:200]}`'
                         + (f' ({loc})' if loc else '') + tag)
            L.append('')
    return '\n'.join(L) + '\n'


def write(prefix, doc, title=None):
    with open(prefix + '.json', 'w') as fh:
        json.dump(doc, fh, indent=1, default=str)
        fh.write('\n')
    with open(prefix + '.md', 'w') as fh:
        fh.write(markdown(doc, title))
