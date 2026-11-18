"""Header-only vtable layouts via clang, shared by inventory.py and check_headers.py.

A probe translation unit includes a header, derives an empty class from the
target class and asks clang (-fdump-vtable-layouts, i386-apple-macos10.9) for
the primary vtable. Slot signatures are canonicalised through mangled names
from clang's JSON AST and demangled, giving the same spelling as the retail
symbols ('GetScreenSize(int&, int&)').
"""
import hashlib
import json
import os
import re
import subprocess

DEFINES = ['DX_TO_GL_ABSTRACTION', 'GL_GLEXT_PROTOTYPES', 'USE_SDL=1', 'OSX=1', '_OSX=1', 'POSIX=1', '_POSIX=1',
           'PLATFORM_POSIX=1', 'GNUC', 'NO_HOOK_MALLOC', '_DLL_EXT=.dylib', 'NO_MEMOVERRIDE_NEW_DELETE=1', 'NDEBUG',
           'VERSION_SAFE_STEAM_API_INTERFACES']
INCLUDES = ['public', 'public/tier0', 'public/tier1', 'common', 'game/shared', 'game/client', 'game/server']
PRELUDE = ['tier0/platform.h', 'tier0/dbg.h', 'tier1/interface.h', 'tier1/utlvector.h', 'tier1/utlstring.h',
           'mathlib/vector.h', 'mathlib/mathlib.h', 'tier1/KeyValues.h']


def clang_base(root):
    cmd = ['clang++', '-std=c++11', '-fsigned-char', '-fpermissive', '-target', 'i386-apple-macos10.9', '-w',
           '-Wno-everything', '-ferror-limit=5', '-fno-delayed-template-parsing']
    cmd += [f'-I{os.path.join(root, i)}' for i in INCLUDES] + [f'-I{root}']
    cmd += ['-idirafter', os.path.join(os.path.dirname(os.path.abspath(__file__)), 'shim')]
    cmd += [f'-D{d}' for d in DEFINES]
    return cmd


def header_relpath(h):
    for inc in INCLUDES:
        if h.startswith(inc + '/'):
            return h[len(inc) + 1:]
    return h


def probe_source(name, header, extra):
    lines = [f'#include "{h}"' for h in PRELUDE + list(extra)]
    lines.append(f'#include "{header_relpath(header)}"')
    # an out-of-line key function emits the vtable without needing a base constructor
    lines.append(f'struct __bm_probe : {name} {{ virtual void __bm_key(); }};')
    lines.append('void __bm_probe::__bm_key() {}')
    return '\n'.join(lines) + '\n'


SLOT = re.compile(r'^\s*(\d+) \| (.*)$')


def parse_layout(text):
    """Primary vtable of '__bm_probe' -> slot strings (function entries only)."""
    out = []
    active = started = False
    for line in text.splitlines():
        if line.startswith("Vtable for '"):
            active = line.startswith("Vtable for '__bm_probe'")
            started = False
            continue
        if not active:
            continue
        m = SLOT.match(line)
        if not m:
            if line.strip() == '' and out:
                break
            continue
        body = m.group(2)
        if body.startswith('offset_to_top'):
            if started:
                break
            started = True
            continue
        if body.startswith(('vbase_offset', 'vcall_offset')):
            if started:
                break
            continue
        if body.endswith(' RTTI') or '__bm_probe::__bm_key()' in body:
            continue
        out.append(body)
    return out


def _scan(s, opener):
    """Index of the first `opener` outside <...>, or None."""
    depth = 0
    for i, ch in enumerate(s):
        if ch == '<':
            depth += 1
        elif ch == '>':
            depth -= 1
        elif ch == opener and depth == 0:
            if opener == '(' and s[i + 1:i + 2] == '*':
                continue
            return i
    return None


def split_owner_name(body):
    """'void *IAppSystem::QueryInterface(const char *) [pure]' -> (owner, name, params, cv)."""
    body = re.sub(r'\s*\[(pure|complete|deleting|deleted|this adjustment[^\]]*|return adjustment[^\]]*)\]', '', body)
    # the parameter list opens right after the method name; return types may hold
    # parentheses of their own ('CBitVec<(1 << 11)> *')
    paren = None
    for m in re.finditer(r'(::|^|[\s*&])(operator\(\)|operator[^\w\s(][^(]*|operator \w[\w ]*?[*&]*|~?[A-Za-z_]\w*)\(', body):
        paren = m.end() - 1
        break
    if paren is None:
        paren = _scan(body, '(')
    head, rest = body[:paren], body[paren:]
    depth, close = 0, len(rest) - 1
    for i, ch in enumerate(rest):
        if ch == '(':
            depth += 1
        elif ch == ')':
            depth -= 1
            if depth == 0:
                close = i
                break
    params, cv = rest[1:close], rest[close + 1:].strip()
    cut = head.rfind('::')
    if cut < 0:
        return '', head.split()[-1].lstrip('*&'), params, cv
    name = head[cut + 2:]
    # walk left from '::' over the owner, which may be a template-id with spaces
    depth, i = 0, cut - 1
    while i >= 0:
        ch = head[i]
        if ch == '>':
            depth += 1
        elif ch == '<':
            depth -= 1
        elif ch in ' *&' and depth == 0:
            break
        i -= 1
    return head[i + 1:cut], name, params, cv


def _json_objects(txt):
    dec = json.JSONDecoder()
    i = 0
    while i < len(txt):
        while i < len(txt) and txt[i].isspace():
            i += 1
        if i >= len(txt):
            break
        o, i = dec.raw_decode(txt, i)
        yield o


def _ast_methods(root, src_path, owner):
    """{(name, qualType): mangledName} for the methods of class `owner`."""
    r = subprocess.run(clang_base(root) + ['-fsyntax-only', '-Xclang', '-ast-dump=json', '-Xclang',
                                           f'-ast-dump-filter={owner}', src_path],
                       capture_output=True, text=True, errors='replace')
    res = {}
    short = owner.split('::')[-1].split('<')[0]

    def walk(node, ns):
        k = node.get('kind')
        if k == 'NamespaceDecl':
            ns = ns + [node.get('name', '')]
        if k in ('CXXRecordDecl', 'ClassTemplateSpecializationDecl') and node.get('name') == short and node.get('inner'):
            for m in node['inner']:
                if m.get('kind') in ('CXXMethodDecl', 'CXXDestructorDecl') and m.get('mangledName'):
                    res.setdefault((m['name'], m['type']['qualType']), m['mangledName'])
            return
        for c in node.get('inner', []) or []:
            walk(c, ns)

    for o in _json_objects(r.stdout):
        walk(o, [])
    return res


def demangle(names):
    if not names:
        return []
    return subprocess.run(['c++filt'], input='\n'.join(names) + '\n', capture_output=True, text=True).stdout.splitlines()


def sig_of(demangled):
    """'IAppSystem::QueryInterface(char const*)' -> 'QueryInterface(char const*)'."""
    if demangled is None:
        return None
    if demangled == '__cxa_pure_virtual':
        return demangled
    if re.match(r'^lib\w+/', demangled):
        demangled = demangled.split('/', 1)[1]
    paren = _scan(demangled, '(')
    head = demangled[:paren] if paren is not None else demangled
    depth, cut = 0, 0
    for i, ch in enumerate(head):
        if ch == '<':
            depth += 1
        elif ch == '>':
            depth -= 1
        elif depth == 0 and head[i:i + 2] == '::':
            cut = i + 2
    return demangled[cut:]


def layout(root, name, header, extra, workdir):
    """-> (slots, error). Each slot: {'sig', 'owner', ['dtor'], ['unresolved']}."""
    src = probe_source(name, header, extra)
    path = os.path.join(workdir, hashlib.md5((root + name + header + repr(extra)).encode()).hexdigest() + '.cpp')
    with open(path, 'w') as fh:
        fh.write(src)
    r = subprocess.run(clang_base(root) + ['-Xclang', '-fdump-vtable-layouts', '-S', '-emit-llvm', '-o', '/dev/null', path],
                       capture_output=True, text=True, errors='replace')
    slots = parse_layout(r.stdout)
    if not slots:
        err = '\n'.join(l for l in r.stderr.splitlines() if 'error' in l)[:800]
        return None, err or 'no layout dumped'
    parsed = [split_owner_name(s) for s in slots]
    tables = {o: _ast_methods(root, path, o) for o in sorted({p[0] for p in parsed if p[0]})}
    keys, mangled = [], []
    for owner, mname, params, cv in parsed:
        if mname.startswith('~'):
            keys.append(('dtor', None))
            continue
        cands = tables.get(owner, {})
        hit = None
        want_const = 'const' in cv.split()
        for (n, qt), mg in cands.items():
            if n != mname:
                continue
            m = re.match(r'^.*?\((.*)\)\s*(const)?\s*(volatile)?\s*(&&?)?$', qt)
            if m and m.group(1).replace(' ', '') == params.replace(' ', '') and bool(m.group(2)) == want_const:
                hit = mg
                break
        if hit is None:
            same = [mg for (n, qt), mg in cands.items() if n == mname]
            hit = same[0] if len(same) == 1 else None
        if hit:
            keys.append(('m', len(mangled)))
            mangled.append(hit)
        else:
            keys.append(('raw', f'{mname}({params}){" const" if want_const else ""}'))
    dem = demangle(mangled)
    out = []
    for (kind, v), (owner, mname, params, cv) in zip(keys, parsed):
        if kind == 'dtor':
            out.append({'sig': f'{mname}()', 'owner': owner, 'dtor': True})
        elif kind == 'm':
            out.append({'sig': sig_of(dem[v]), 'owner': owner})
        else:
            out.append({'sig': v, 'owner': owner, 'unresolved': True})
    return out, None
