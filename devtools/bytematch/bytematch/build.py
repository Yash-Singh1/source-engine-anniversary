"""Turn (section bytes, symbol starts, relocation fields) into Function records."""
import bisect
from dataclasses import dataclass, field

from .model import Function, Reloc, canonical, pick_primary
from .x86 import trim_padding


@dataclass
class Entry:
    addr: int
    names: list = field(default_factory=list)   # [(is_global, raw name)]
    size: int = None                             # symbol-provided size, if any
    exact: bool = False                          # size excludes padding (ELF st_size, COFF COMDAT)
    obj: str = None
    comdat: bool = False
    bounded: bool = True                         # False: end is only a guess (stripped PE)
    obj_path: str = None


def merge_entries(entries):
    by = {}
    for e in entries:
        cur = by.get(e.addr)
        if cur is None:
            by[e.addr] = Entry(e.addr, list(e.names), e.size, e.exact, e.obj, e.comdat, e.bounded, e.obj_path)
            continue
        for n in e.names:
            if n not in cur.names:
                cur.names.append(n)
        if e.size and (not cur.size or (e.exact and not cur.exact)):
            cur.size, cur.exact = e.size, e.exact
        cur.obj = cur.obj or e.obj
        cur.obj_path = cur.obj_path or e.obj_path
        cur.comdat |= e.comdat
        cur.bounded |= e.bounded
    return [by[a] for a in sorted(by)]


def rebuild(mod, extra):
    """Rebuild all functions with extra starts, e.g. from a user map.

    Items are (va, name or [names], size) or (va, names, size, object stem)."""
    inputs, mod.build_inputs = mod.build_inputs, []
    mod.functions, mod._starts = [], None
    norm = []
    for item in extra:
        a, n, s = item[:3]
        names = [n] if isinstance(n, str) else list(n)
        norm.append((a, names, s, item[3] if len(item) > 3 else None))
    for secname, va, data, entries, fields in inputs:
        ex = [Entry(a, [(True, n) for n in ns], s, bounded=bool(s), obj=obj)
              for a, ns, s, obj in norm if va <= a < va + len(data)]
        build_functions(mod, secname, va, data, entries + ex, fields)
    for f in mod.functions:
        f.obj_path = mod.obj_paths.get(f.addr, f.obj_path)


def build_functions(mod, secname, va, data, entries, fields=(), max_guess=0x10000):
    """`fields`: iterable of (va, size, kind, target, addend) relocated fields in this section."""
    fields = sorted(fields)
    mod.build_inputs.append((secname, va, data, list(entries), fields))
    entries = merge_entries(entries)
    fstarts = [f[0] for f in fields]
    out = []
    end_of_sec = va + len(data)
    for i, e in enumerate(entries):
        if not va <= e.addr < end_of_sec:
            continue
        nxt = entries[i + 1].addr if i + 1 < len(entries) else end_of_sec
        if e.size and (e.exact or e.addr + e.size <= nxt):
            end = e.addr + e.size
        else:
            end = nxt
        if not e.bounded:
            end = min(end, e.addr + max_guess)
        end = min(end, end_of_sec)
        raw = data[e.addr - va:end - va]
        body = raw if e.exact else raw[:trim_padding(raw)]
        names = e.names or [(True, f'sub_{e.addr:x}')]
        primary = pick_primary(names)
        is_global = any(g for g, _ in names)
        relocs = []
        j = bisect.bisect_left(fstarts, e.addr - 3)
        while j < len(fields) and fields[j][0] < e.addr + len(body):
            fva, fsz, kind, target, addend = fields[j]
            if fva >= e.addr:
                relocs.append(Reloc(fva - e.addr, min(fsz, e.addr + len(body) - fva), kind, target, addend))
            elif fva + fsz > e.addr:
                relocs.append(Reloc(0, fva + fsz - e.addr, kind, target, addend))
            j += 1
        f = Function(name=primary, key=canonical(mod.fmt, primary), addr=e.addr, data=body,
                     section=secname, obj=e.obj, obj_path=e.obj_path or (None if mod.linked else mod.label),
                     local=not is_global, size_known=e.bounded,
                     raw_size=len(raw), comdat=e.comdat, relocs=relocs,
                     aliases=[n for _, n in names if n != primary], module=mod)
        out.append(f)
    mod.functions.extend(out)
    return out
