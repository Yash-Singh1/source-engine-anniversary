"""Name the functions of a stripped ELF image (retail Linux .so) after our objects.

Retail Linux modules export only CreateInterface; their function extents come
from .eh_frame (elf.py). Pairing then works in two passes over our objects:

1. Search: each function's bytes, masked at its relocations and at branches
   leaving it, are compared with every retail function of the same size.
   A unique hit names that retail function.
2. Position: GNU ld lays out an object's .text section contiguously and in
   the object's order (no -ffunction-sections), so within one object section
   the hits must ascend. The longest ascending chain of hits are anchors;
   ambiguous hits are resolved between them, and functions with no hit are
   paired with the unclaimed retail functions between the same anchors
   (in order when the counts agree, else by an alignment on size).

COMDAT sections (inline and template functions) are laid out where the first
object that has them is linked, so they only take part through unique hits.
"""
import collections

from . import compare

SEARCH, AMBIGUOUS, POSITION = 'search', 'search-ambiguous', 'position'


def stripped(mod):
    """A linked ELF with unnamed functions (extents from .eh_frame only)."""
    return mod.fmt == 'elf' and mod.linked and any(f.name.startswith('sub_') for f in mod.functions)


def _runs(data, mask):
    runs, i, n = [], 0, len(data)
    while i < n:
        if i in mask:
            i += 1
            continue
        j = i
        while j < n and j not in mask:
            j += 1
        runs.append((i, j))
        i = j
    return runs


def _comdat(f):
    s = f.section or ''
    return s.startswith('.text.') and not s.startswith(('.text.unlikely', '.text.startup', '.text.hot', '.text.exit'))


def locate(retail, ours):
    """{our function: (retail function, how)} for one retail module and our object functions."""
    by_size = collections.defaultdict(list)
    for r in retail.functions:
        by_size[len(r.data)].append(r)
    hits = {}
    cache = {}
    for o in ours:
        if not o.data:
            continue
        mask, _ = compare.build_mask(o, o, o.data, o.data)
        key = (o.data, frozenset(mask))
        if key not in cache:
            runs = _runs(o.data, mask)
            data = o.data
            cache[key] = [r for r in by_size.get(len(data), ())
                          if all(r.data[a:b] == data[a:b] for a, b in runs)]
        hits[o] = cache[key]

    out = {}
    claimed = set()

    def take(o, r, how):
        out[o] = (r, how)
        claimed.add(r)

    # unique hits first; a retail function hit uniquely by several of ours (identical
    # code in different objects) goes to none of them in this pass
    uniq = collections.defaultdict(list)
    for o, hs in hits.items():
        if len(hs) == 1:
            uniq[hs[0]].append(o)
    for r, os_ in uniq.items():
        if len(os_) == 1:
            take(os_[0], r, SEARCH)

    order = sorted(retail.functions, key=lambda f: f.addr)
    index = {f: i for i, f in enumerate(order)}
    runs = collections.defaultdict(list)
    for o in ours:
        if not _comdat(o):
            runs[(o.module.label, o.section)].append(o)
    for run in runs.values():
        run.sort(key=lambda f: f.addr)
        _place_run(run, hits, out, claimed, order, index, take)

    # what's left: identical code with several candidates and no anchors around it
    for o, hs in hits.items():
        if o in out or not hs:
            continue
        free = [r for r in hs if r not in claimed]
        take(o, (free or hs)[0], AMBIGUOUS)
    _by_callees(out, hits, claimed)
    return out, hits


def _call_targets(o, r):
    """(our target key, retail target VA) for each call/jump relocation of ours."""
    for rel in o.relocs:
        k = rel.offset
        if rel.kind != 'pcrel' or not rel.target or rel.size != 4 or k + 4 > len(r.data):
            continue
        if (k >= 1 and r.data[k - 1] in (0xe8, 0xe9)) or (k >= 2 and r.data[k - 2] == 0x0f and 0x80 <= r.data[k - 1] <= 0x8f):
            yield rel.target, (r.addr + k + 4 + int.from_bytes(r.data[k:k + 4], 'little', signed=True)) & 0xffffffff


def _by_callees(out, hits, claimed):
    """Re-pick among identical-code candidates by what they call: "store the vtable, call
    the base destructor" masks alike for every class, but the callee tells them apart."""
    from .model import canonical
    addrs, sure = {}, {}
    for o, hs in hits.items():
        keys = [o.key] + [canonical(o.module.fmt, a) for a in o.aliases]
        for k in keys:
            addrs.setdefault(k, set()).update(r.addr for r in hs)
        if len(hs) == 1:
            sure[hs[0].addr] = set(keys)

    def score(o, r):
        good = bad = 0
        for t, va in _call_targets(o, r):
            if va in addrs.get(t, ()):
                good += 1
            elif va in sure:
                bad += 1
        return (bad == 0, good)

    # functions sharing one candidate set are assigned together, best-agreeing pairs first
    groups = collections.defaultdict(list)
    for o, hs in hits.items():
        if len(hs) >= 2 and o in out and out[o][1] != POSITION and len(hs) <= 400:
            groups[tuple(id(r) for r in hs)].append(o)
    for os_ in groups.values():
        cands = hits[os_[0]]
        if not any(o.relocs for o in os_):
            continue
        for o in os_:
            claimed.discard(out[o][0])
        free = [r for r in cands if r not in claimed]
        pairs = sorted(((score(o, r), -i, -j, o, r) for i, o in enumerate(os_) for j, r in enumerate(free)),
                       key=lambda p: p[:3], reverse=True)
        done, used = set(), set()
        for s, _, _, o, r in pairs:
            if o in done or r in used:
                continue
            out[o] = (r, out[o][1])
            done.add(o)
            used.add(r)
        for o in os_:
            claimed.add(out[o][0])


def _lis(pairs):
    """Longest chain of (position in run, retail index) ascending in both."""
    if not pairs:
        return []
    n = len(pairs)
    best = [1] * n
    prev = [-1] * n
    for i in range(n):
        for j in range(max(0, i - 200), i):
            if pairs[j][1] < pairs[i][1] and best[j] + 1 > best[i]:
                best[i], prev[i] = best[j] + 1, j
    i = max(range(n), key=lambda k: best[k])
    chain = []
    while i >= 0:
        chain.append(pairs[i])
        i = prev[i]
    return chain[::-1]


def _place_run(run, hits, out, claimed, order, index, take):
    pairs = [(k, index[out[o][0]]) for k, o in enumerate(run) if o in out]
    anchors = _lis(pairs)
    keep = set(k for k, _ in anchors)
    for k, o in enumerate(run):
        if o in out and k not in keep:
            # a unique hit out of order: identical code elsewhere (e.g. an inlined copy)
            claimed.discard(out.pop(o)[0])
    bounds = [(-1, None)] + anchors + [(len(run), None)]
    for (ka, ra), (kb, rb) in zip(bounds, bounds[1:]):
        lo = ra + 1 if ra is not None else None
        hi = rb if rb is not None else None
        gap = run[ka + 1:kb]
        if not gap:
            continue
        # ambiguous hits inside the window
        for o in gap:
            if o in out or len(hits.get(o, ())) < 2:
                continue
            inside = [r for r in hits[o] if r not in claimed and (lo is None or index[r] >= lo) and (hi is None or index[r] < hi)]
            if inside:
                take(o, inside[0], SEARCH)
                lo = index[inside[0]] + 1 if lo is not None else lo
        edge = lo is None or hi is None
        if lo is None and hi is None:
            continue
        # before the first anchor or after the last: up to the nearest function another object claimed
        if lo is None:
            lo = hi
            while lo > 0 and order[lo - 1] not in claimed:
                lo -= 1
        if hi is None:
            hi = lo
            while hi < len(order) and order[hi] not in claimed:
                hi += 1
        # identical code found elsewhere isn't this slot's function; gap code without an FDE
        # (ud2 after noreturn calls, CRT code) isn't a compiled function
        rest = [o for o in gap if o not in out and not hits.get(o)]
        fde = getattr(order[0].module, 'fde_starts', None) if order else None
        free = [r for r in order[lo:hi] if r not in claimed and (fde is None or r.addr in fde)]
        if not rest or not free:
            continue
        if len(rest) == len(free) and not edge:
            for o, r in zip(rest, free):
                take(o, r, POSITION)
        else:
            for o, r in _align(rest, free):
                take(o, r, POSITION)


def _align(a, b):
    """Order-preserving pairing of two function lists by size similarity (gaps allowed)."""
    if len(a) * len(b) > 40000:
        return []
    def score(x, y):
        sx, sy = len(x.data), len(y.data)
        return 1.0 - abs(sx - sy) / max(sx, sy, 1) * 2
    n, m = len(a), len(b)
    gap = -0.3
    dp = [[0.0] * (m + 1) for _ in range(n + 1)]
    bt = [[0] * (m + 1) for _ in range(n + 1)]
    for i in range(1, n + 1):
        dp[i][0], bt[i][0] = dp[i - 1][0] + gap, 1
    for j in range(1, m + 1):
        dp[0][j], bt[0][j] = dp[0][j - 1] + gap, 2
    for i in range(1, n + 1):
        for j in range(1, m + 1):
            c = [dp[i - 1][j - 1] + score(a[i - 1], b[j - 1]), dp[i - 1][j] + gap, dp[i][j - 1] + gap]
            k = max(range(3), key=lambda t: c[t])
            dp[i][j], bt[i][j] = c[k], k
    i, j, pairs = n, m, []
    while i > 0 and j > 0:
        k = bt[i][j]
        if k == 0:
            if score(a[i - 1], b[j - 1]) > 0:
                pairs.append((a[i - 1], b[j - 1]))
            i, j = i - 1, j - 1
        elif k == 1:
            i -= 1
        else:
            j -= 1
    return pairs[::-1]


def name_retail(retail, ours):
    """Locate our functions in a stripped retail ELF and give the retail functions our names.

    Returns {how: count}. Our functions are deduplicated by pairing key first."""
    from .model import canonical
    funcs = compare.dedupe_ours(ours)
    found, hits = locate(retail, funcs)
    taken = {}
    for o, (r, how) in found.items():
        if r in taken or not r.name.startswith('sub_'):
            continue
        taken[r] = o
        r.name, r.key, r.local, r.obj = o.name, o.key, o.local, o.obj
        r.aliases = list(o.aliases)
        r.obj_path = o.obj_path
        r.located = how
    for r, o in taken.items():
        for k in [o.key] + [canonical(o.module.fmt, a) for a in o.aliases]:
            retail.add_name(r.addr, k)
    # Identical code in more of ours than retail has copies (template instantiations alike
    # once relocations are masked): every copy is taken, but the code is retail's.
    paired = set(taken.values())
    for o, hs in hits.items():
        if hs and o not in paired:
            o.identical_in_retail = True
    # A call target only disagrees when the retail callee was identified by a unique search
    # hit under another name and isn't one of our callee's identical-code hits: ld keeps
    # CUtlVector<int>::GrowVector and an identical CUtlVector<T*> copy, and hundreds of
    # vtable-setting destructors look alike once relocations are masked.
    hit_addrs = {}
    for o, hs in hits.items():
        for k in [o.key] + [canonical(o.module.fmt, a) for a in o.aliases]:
            hit_addrs.setdefault(k, set()).update(r.addr for r in hs)
    sure = {r.addr for r in taken if getattr(r, 'located', None) == SEARCH and len(hits.get(taken[r], ())) == 1}

    def target_ok(key, va):
        return va in hit_addrs.get(key, ()) or va not in sure
    retail.target_ok = target_ok
    return collections.Counter(how for o, (r, how) in found.items() if taken.get(r) is o)
