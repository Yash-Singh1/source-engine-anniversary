"""Pair MSVC objects with a stripped retail PE through the symbol-port maps.

Retail Windows modules export few names. byte-match-survey/symbols/out/
<module>.win.json names most functions, either with the MSVC decorated name
(when an object match proved it) or only with the macOS Itanium name plus the
macOS object it came from. Our COFF objects carry decorated names. Both sides
get a 'sig:' alias, a normalised demangled signature (scope::name(params)
qualifiers, no return type, calling convention or class/struct keywords), so
a macOS-only name still pairs with our decorated one. The map's macOS object
gives each retail function an object stem, which enables per-object scoping
like the Mach-O debug map.
"""
import json
import os
import re
import shutil
import subprocess

from .model import object_stem

UNDNAME_CANDIDATES = ('llvm-undname', '/opt/homebrew/opt/llvm/bin/llvm-undname', '/usr/local/opt/llvm/bin/llvm-undname')

# decorated (canonical COFF key) -> 'sig:...' for every name seen; compare.check_targets
# consults it so a call to ?Foo@@... matches a retail target known only as sig:Foo(...).
EQUIV = {}

# canonical name -> names our own link (/OPT:ICF) folded onto the same address, from
# link /MAP files; compare.check_targets accepts a retail target in the same group.
FOLDED = {}

_CC = re.compile(r'\b__(?:thiscall|cdecl|stdcall|fastcall|vectorcall|clrcall|ptr32|ptr64)\b')
_ACCESS = re.compile(r'^(?:public|private|protected): ')
_KW = re.compile(r'\b(?:class|struct|union|enum) ')


def is_symbol_port(doc):
    return isinstance(doc, list) and doc and isinstance(doc[0], dict) and 'address' in doc[0]


def _undname_tool():
    for c in UNDNAME_CANDIDATES:
        p = shutil.which(c) or (c if os.path.exists(c) else None)
        if p:
            return p
    return None


def undname(names):
    """{decorated: demangled} via llvm-undname (stdin mode)."""
    names = sorted({n for n in names if n.startswith('?')})
    tool = _undname_tool()
    if not names or not tool:
        return {}
    out = subprocess.run([tool], input='\n'.join(names) + '\n', capture_output=True, text=True).stdout
    lines = [l for l in out.split('\n') if l]
    want, res = set(names), {}
    for i, l in enumerate(lines):
        if l in want and l not in res and i + 1 < len(lines):
            nxt = lines[i + 1]
            if nxt not in want and not nxt.startswith('error') and not nxt.startswith('invalid'):
                res[l] = nxt
    return res


def cxxfilt(names):
    """{itanium mangled (with or without Mach-O '_'): demangled} via c++filt."""
    names = sorted({n for n in names if n.startswith(('_Z', '__Z'))})
    tool = shutil.which('c++filt') or shutil.which('llvm-cxxfilt')
    if not names or not tool:
        return {}
    keys = [n[1:] if n.startswith('__Z') else n for n in names]
    out = subprocess.run([tool], input='\n'.join(keys) + '\n', capture_output=True, text=True).stdout.split('\n')
    return {n: out[i] for i, n in enumerate(names) if i < len(out) and out[i] and out[i] != keys[i]}


def _top_level_paren(s):
    """Index of the '(' opening the parameter list (last top-level one), or -1."""
    depth, last = 0, -1
    for i, c in enumerate(s):
        if c in '<[':
            depth += 1
        elif c in '>]':
            depth -= 1
        elif c == '(':
            if depth == 0:
                last = i
            depth += 1
        elif c == ')':
            depth -= 1
    return last


def _drop_return_type(head):
    depth, cut = 0, -1
    for i, c in enumerate(head):
        if c in '<(':
            depth += 1
        elif c in '>)':
            depth -= 1
        elif c == ' ' and depth == 0:
            cut = i
    if cut >= 0 and not head[:cut].rstrip().endswith('operator'):
        return head[cut + 1:]
    return head


def sig(demangled):
    """Normalised signature of a demangled MSVC or Itanium function name, or None."""
    s = demangled.strip()
    # this-adjusting thunks: MSVC `adjustor{N}', Itanium "non-virtual thunk to" (_ZThnN_, see
    # read_rows); keyed apart from the method they enter, with the offset when known
    a = re.search(r"`adjustor\{(\d+)\}'\s*", s)
    if a:
        k = sig(s.replace(a.group(0), '').replace('[thunk]:', ''))
        return k and k + '#thunk' + a.group(1)
    if s.startswith('non-virtual thunk to '):
        k = sig(s[len('non-virtual thunk to '):])
        return k and k + '#thunk'
    m = re.search(r"::`(?:scalar|vector) deleting (?:destructor|dtor)'", s)
    if m:
        # MSVC's deleting destructor; Itanium's D0 gets the same key (read_rows)
        k = sig(s[:m.start()] + '::__dtor()')
        if k:
            cls = re.sub(r'<.*>', '', k[4:-len('::__dtor()')]).split('::')[-1]
            return k[:-len('__dtor()')] + '~' + cls + '()#D0'
        return None
    if not s or '`' in s.replace("`anonymous namespace'", ''):
        return None          # vftables, dynamic initializers, RTTI, string literals ...
    s = s.replace('(anonymous namespace)', "`anonymous namespace'")
    s = _ACCESS.sub('', s)
    s = re.sub(r'^(?:virtual |static )+', '', s)
    s = _CC.sub('', s)
    p = _top_level_paren(s)
    if p < 0:
        return None
    head, tail = s[:p].strip(), s[p:]
    head = _drop_return_type(head)
    tail = tail.replace('(void)', '()')
    out = head + tail
    out = _KW.sub('', out)
    out = out.replace('__int64', 'long long').replace('signed char', 'signed char')
    out = re.sub(r'\s+', '', out)
    out = out.replace('(void)', '()')
    # MSVC keeps __restrict and a parameter's top-level const (array parameters decay to
    # T *const); Itanium drops both. undname spells bool template arguments 0/1.
    out = out.replace('__restrict', '').replace('__ptr64', '')
    out = re.sub(r'\b(?:__sFILE|_iobuf)\b', 'FILE', out)   # Darwin's and MSVC's FILE
    out = re.sub(r'\*const(?=[,)])', '*', out)
    out = re.sub(r'(?<=[<,])false(?=[,>])', '0', out)
    out = re.sub(r'(?<=[<,])true(?=[,>])', '1', out)
    return 'sig:' + out


def add_sig_aliases(funcs):
    """Give COFF functions a 'sig:' alias from their decorated names."""
    dem = undname({n for f in funcs for n in [f.name] + list(f.aliases)})
    for f in funcs:
        for n in [f.name] + list(f.aliases):
            d = dem.get(n)
            k = sig(d) if d else None
            if k:
                EQUIV[f.key] = k
                if k not in f.aliases:
                    f.aliases.append(k)
                break
    names = {r.target for f in funcs for r in f.relocs if r.target and r.target.startswith('?')}
    for n, d in undname(names).items():
        k = sig(d)
        if k:
            EQUIV.setdefault(n, k)


_MAP_PUBLIC = re.compile(r'^\s*[0-9a-fA-F]{4}:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\b')


def load_link_map(path):
    """Fill FOLDED from an MSVC link /MAP file: code symbols that share an address."""
    from .model import canonical
    at = {}
    with open(path, errors='replace') as f:
        for line in f:
            m = _MAP_PUBLIC.match(line)
            if m:
                at.setdefault(int(m.group(2), 16), []).append(m.group(1))
    groups = [g for g in at.values() if len(g) > 1]
    dem = undname({n for g in groups for n in g})
    for g in groups:
        keys = {canonical('coff', n) for n in g}
        keys |= {k for k in (sig(dem[n]) for n in g if n in dem) if k}
        for k in keys:
            FOLDED.setdefault(k, set()).update(keys)
    return len(groups)


def itanium_key(mangled, demangled):
    """sig() of an Itanium name, with the deleting-destructor and thunk-offset suffixes the
    MSVC side gets ("#D0", "#thunkN")."""
    k = sig(demangled)
    if not k:
        return k
    k = k.replace('#thunk', '')
    if re.search(r'D0Ev$', mangled):
        k += '#D0'
    t = re.match(r'_?_ZThn(\d+)_', mangled)
    if t:
        k += '#thunk' + t.group(1)
    return k


def read_rows(doc, min_conf=0.0):
    """symbol-port rows -> [(addr, [names], size, obj_stem, confidence)] for named functions."""
    rows = [r for r in doc if r.get('name') and float(r.get('confidence') or 0) >= min_conf]
    dem_it = cxxfilt([r['mangled'] for r in rows if r.get('mangled')])
    out = []
    for r in rows:
        names = []
        if r.get('msvc_name'):
            names.append(r['msvc_name'])
        k = None
        if r.get('msvc_demangled'):
            k = sig(r['msvc_demangled'])
        if not k and r.get('mangled'):
            if r['mangled'] in dem_it:
                k = itanium_key(r['mangled'], dem_it[r['mangled']])
            elif not r['mangled'].startswith('__Z'):
                names.append(r['mangled'])      # C name, '_Foo' on Mach-O
        if not k and not names:
            k = sig(r['name'])
        if k:
            names.append(k)
        if not names:
            continue
        stem = object_stem(r['mac_obj']) if r.get('mac_obj') else None
        if not stem:
            m = re.search(r'msvc-[^:]+:([^ ]+\.obj)', ' '.join(r.get('evidence') or []))
            stem = object_stem(m.group(1)) if m else None
        out.append((int(r['address'], 16), names, r.get('size'), stem, float(r.get('confidence') or 0)))
    return out


def load(path):
    with open(path) as f:
        return json.load(f)
