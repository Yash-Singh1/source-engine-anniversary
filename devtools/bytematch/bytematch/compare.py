"""Pair functions across modules and classify each pair."""
import difflib
from dataclasses import dataclass, field

from . import x86
from .winmap import EQUIV, FOLDED

EXACT = 'exact'
MASKED = 'matches-after-masking'
DIFFERS = 'differs'
MISSING = 'missing'                  # retail function with no counterpart in ours
EXTRA = 'missing-in-retail'          # ours function with no counterpart in retail

_PAD = (0x90, 0xcc)


@dataclass
class Result:
    status: str
    key: str
    retail: object = None
    ours: object = None
    reason: str = ''
    first_diff: int = None
    masked_bytes: int = 0
    target_mismatches: list = field(default_factory=list)
    line_constants: list = None      # [(offset, retail, ours)] when only __LINE__-like immediates differ
    data_mismatches: list = field(default_factory=list)   # [(offset, retail identity, ours identity)] (--data-check)

    @property
    def name(self):
        return (self.retail or self.ours).name

    @property
    def obj(self):
        return (self.ours.obj if self.ours else None) or (self.retail.obj if self.retail else None)

    @property
    def size(self):
        """Weight for byte percentages: retail size when known, else ours."""
        if self.retail and self.retail.size_known:
            return self.retail.size
        return (self.ours or self.retail).size


class Index:
    """Lookup of functions by pairing key across one or more modules."""

    def __init__(self, funcs):
        self.by_key = {}
        for f in funcs:
            for k in [f.key] + [a for a in _alias_keys(f)]:
                self.by_key.setdefault(k, []).append(f)

    def find(self, f, used=None):
        cands = []
        for k in [f.key] + _alias_keys(f):
            for c in self.by_key.get(k, ()):
                if c not in cands:
                    cands.append(c)
        if used is not None:
            cands = [c for c in cands if c not in used] or cands
        if not cands:
            return None
        same = [c for c in cands if f.obj and c.obj == f.obj]
        if same:
            return same[0]
        if f.local and f.obj and any(c.local and c.obj for c in cands):
            # both sides know the object: a static in a different TU is a different function
            others = [c for c in cands if not (c.local and c.obj)]
            return others[0] if others else None
        return cands[0]


def _alias_keys(f):
    from .model import canonical
    return [canonical(f.module.fmt, a) for a in f.aliases] if f.module else []


def side_mask(f, data, both_linked):
    """Byte offsets of `f` holding link-time values, and names for some of them.

    With an object on one side its relocations are authoritative; only
    branches leaving the function (resolved by the assembler without a
    relocation for same-section targets) are added. Two linked images have no
    relocations for PIC/absolute operands, so heuristics fill in."""
    n = len(data)
    mask, labels = set(), {}
    for rel in f.relocs:
        mask.update(range(rel.offset, min(rel.offset + rel.size, n)))
        if rel.target:
            labels.setdefault(rel.offset, rel.target)
    mod = f.module
    if both_linked:
        image = None if mod.pic else (mod.image_base, mod.image_end)
        for off, w, kind in x86.address_fields(data, f.addr, image):
            mask.update(range(off, off + w))
    for off, w, tgt, is_call in x86.branch_fields(data, f.addr):
        mask.update(range(off, off + w))
        if mod.linked:
            names = mod.resolve(tgt)
            if names:
                labels.setdefault(off, names[0])
    return mask, labels


def build_mask(r, o, rdata, odata):
    """Union of both sides' masks: valid while the two byte streams line up."""
    both_linked = r.module.linked and o.module.linked
    rm, rl = side_mask(r, rdata, both_linked)
    om, ol = side_mask(o, odata, both_linked)
    return rm | om, {**rl, **ol}


def check_targets(r, o, rdata, odata):
    """Relocation targets in ours that the retail field demonstrably points elsewhere."""
    out = []
    rmod = r.module
    if not rmod.linked:
        return out
    for rel in o.relocs:
        if not rel.target or rel.size != 4 or rel.offset + 4 > len(rdata):
            continue
        k = rel.offset
        if rel.kind == 'pcrel':
            if not (k >= 1 and rdata[k - 1] in (0xe8, 0xe9)) and not (k >= 2 and rdata[k - 2] == 0x0f and 0x80 <= rdata[k - 1] <= 0x8f):
                continue
            va = (r.addr + k + 4 + x86.s32(rdata, k)) & 0xffffffff
        elif rel.kind == 'abs' and not rmod.pic:
            va = (x86.u32(rdata, k) - rel.addend) & 0xffffffff
        else:
            continue
        names = rmod.resolve(va)
        if names and rel.target not in names and EQUIV.get(rel.target) not in names \
                and not FOLDED.get(rel.target, FOLDED.get(EQUIV.get(rel.target), set())) & set(names):
            out.append((k, rel.target, names[0]))
    if o.module.linked:
        # linked vs linked: compare branch targets symbolically on both sides
        ob = {off: tgt for off, w, tgt, c in x86.branch_fields(odata, o.addr) if w == 4}
        for off, w, tgt, c in x86.branch_fields(rdata, r.addr):
            if w != 4 or off not in ob:
                continue
            rn, on = rmod.resolve(tgt), o.module.resolve(ob[off])
            if rn and on and not set(rn) & set(on):
                out.append((off, on[0], rn[0]))
    return out


def compare(r, o, target_check=True):
    odata = o.data
    rdata = r.data
    tail_note = ''
    if not r.size_known and len(rdata) > len(odata):
        # retail extent unknown (stripped PE): compare ours' length, then require padding or a known start
        nxt = r.data[len(odata):len(odata) + 1]
        boundary = r.module.function_at(r.addr + len(odata))
        if nxt and nxt[0] not in _PAD and (boundary is None or boundary is r):
            tail_note = 'retail continues past ours'
        rdata = rdata[:len(odata)]
    res = Result(EXACT, r.key, r, o)
    if rdata == odata and not tail_note:
        return res
    if len(rdata) != len(odata):
        res.status = DIFFERS
        res.reason = f'size retail {len(rdata)} vs ours {len(odata)}'
        res.first_diff = next((i for i, (a, b) in enumerate(zip(rdata, odata)) if a != b), min(len(rdata), len(odata)))
        return res
    mask, _ = build_mask(r, o, rdata, odata)
    res.masked_bytes = len(mask)
    n = min(len(rdata), len(odata))
    first = next((i for i in range(n) if i not in mask and rdata[i] != odata[i]), None)
    if first is None and len(rdata) == len(odata) and not tail_note:
        res.status = MASKED
        if target_check:
            res.target_mismatches = check_targets(r, o, rdata, odata)
            if res.target_mismatches:
                res.status = DIFFERS
                k, ours_t, ret_t = res.target_mismatches[0]
                res.reason = f'reloc target at +{k:#x}: ours {ours_t}, retail {ret_t}'
                res.first_diff = k
        return res
    res.status = DIFFERS
    res.first_diff = first if first is not None else n
    if len(rdata) == len(odata) and not tail_note:
        rz = bytes(0 if i in mask else c for i, c in enumerate(rdata))
        oz = bytes(0 if i in mask else c for i, c in enumerate(odata))
        lc = x86.line_constant_diffs(rz, oz)
        if lc:
            res.line_constants = lc
            res.reason = 'line constants only: ' + ', '.join(f'{a}->{b}' for _, a, b in lc[:6])
            return res
    if len(rdata) != len(odata):
        res.reason = f'size retail {len(rdata)} vs ours {len(odata)}'
    elif tail_note:
        res.reason = tail_note
    else:
        res.reason = 'bytes'
    return res


def instruction_diff(res, context=3, limit=None):
    """Unified diff of normalised listings ('-' retail, '+' ours)."""
    r, o = res.retail, res.ours
    rdata = r.data if r.size_known else r.data[:max(len(o.data), 1)]
    odata = o.data
    both_linked = r.module.linked and o.module.linked
    rmask, rlab = side_mask(r, rdata, both_linked)
    omask, olab = side_mask(o, odata, both_linked)
    if len(rdata) == len(odata):
        rmask = omask = rmask | omask          # streams line up: mask a field on both sides
    rl = x86.listing(rdata, r.addr, rmask, rlab)
    ol = x86.listing(odata, o.addr, omask, olab)
    a = [row[2] for row in rl]
    b = [row[2] for row in ol]
    out = []
    sm = difflib.SequenceMatcher(None, a, b, autojunk=False)
    if all(tag == 'equal' for tag, *_ in sm.get_opcodes()):
        out = [f'  {off:5x}  {hx:<20s}  {disp}' for (off, hx, _, _), (_, _, _, disp) in zip(rl, ol)]
        out += [f'  +{k:#x}: ours -> {t}, retail -> {rt}' for k, t, rt in res.target_mismatches]
        return out[:limit] + ['  ...'] if limit and len(out) > limit else out
    for group in sm.get_grouped_opcodes(context):
        i1, j1 = group[0][1], group[0][3]
        out.append(f'@@ retail +{rl[i1][0] if i1 < len(rl) else len(rdata):#x} '
                   f'ours +{ol[j1][0] if j1 < len(ol) else len(odata):#x} @@')
        for tag, a1, a2, b1, b2 in group:
            if tag == 'equal':
                out += [f'  {rl[i][0]:5x}  {ol[j][3]}' for i, j in zip(range(a1, a2), range(b1, b2))]
                continue
            out += [f'- {rl[i][0]:5x}  {rl[i][3]}' for i in range(a1, a2)]
            out += [f'+ {ol[j][0]:5x}  {ol[j][3]}' for j in range(b1, b2)]
        if limit and len(out) >= limit:
            return out[:limit] + ['  ...']
    return out


def pair_all(retail_funcs, ours_funcs, target_check=True, include=None):
    """Compare every retail function against ours; returns (results, unmatched_ours)."""
    idx = Index(ours_funcs)
    used = set()
    results = []
    for r in retail_funcs:
        if include and not include(r):
            continue
        o = idx.find(r, used)
        if o is None:
            results.append(Result(MISSING, r.key, r, None))
            continue
        used.add(o)
        results.append(compare(r, o, target_check))
    extra = []
    seen = set()
    for o in ours_funcs:
        if o in used or (include and not include(o)) or o.pair_key in seen:
            continue
        seen.add(o.pair_key)
        extra.append(Result(EXTRA, o.key, None, o))
    return results, extra


def dedupe_ours(funcs):
    """COMDAT/inline functions appear in many objects; keep one per pairing key."""
    out, seen = [], set()
    for f in funcs:
        k = f.pair_key
        if k in seen:
            continue
        seen.add(k)
        out.append(f)
    return out
