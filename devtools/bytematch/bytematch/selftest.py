"""Self tests. Each check prints PASS/FAIL/SKIP; exit status 1 on any FAIL.

1. Retail modules compared against themselves must be 100% exact.
2. tests/sample.cpp built by each available toolchain: object vs its linked
   module must match fully; an -O1 object vs the -O2 module must differ.
3. clang-600.0.57 workflow: tier1/mathlib files from this tree compiled with
   macos-cc.sh must fully match retail engine.dylib.
4. The survey pilot objects (clang-600) must reach the pilot's 311 matches.
5. The same comparisons without capstone (byte-level fallback).
6. Data audit: a retail module against itself is clean; a copy with one
   referenced literal edited shows that literal and its functions."""
import json
import os
import shutil
import subprocess
import sys

from . import compare, loader, report

HERE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
TESTS = os.path.join(HERE, 'tests')
BUILD = os.path.join(TESTS, 'build')
REPO = os.path.dirname(os.path.dirname(HERE))
MS = '/Volumes/Storage/sco/byte-match-survey/mac-scratch'
CLANG600 = os.environ.get('BM_CLANG', f'{MS}/build-600/bin/clang')
SDK106 = f'{MS}/x/MacOSX10.6/root/SDKs/MacOSX10.6.sdk'
LD64 = f'{MS}/cctools-242/cctools/ld64/src/ld/ld'
LINKSDK = f'{MS}/linktest/sdk'
SCOUT = 'registry.gitlab.steamos.cloud/steamrt/scout/sdk:0.20221019.0'
PILOT = f'{MS}/pilot/a600'
PILOT_MATCHED = 311


class Runner:
    def __init__(self):
        self.fails = 0
        self.notes = []

    def check(self, name, ok, detail=''):
        print(f'{"PASS" if ok else "FAIL"}  {name}  {detail}')
        self.fails += not ok

    def skip(self, name, why):
        print(f'SKIP  {name}  ({why})')

    def note(self, text):
        print(f'NOTE  {text}')
        self.notes.append(text)


def run_compare(retail, ours, **kw):
    R = loader.load_one(retail)
    funcs = [f for m in loader.load_path(ours, quiet=True) for f in m.functions]
    data = report.run(R, funcs, **kw)
    return report.summarize(data['results'])['functions'], data, R


def sh(cmd, **kw):
    return subprocess.run(cmd, capture_output=True, text=True, **kw)


def build_samples(t, rebuild):
    os.makedirs(BUILD, exist_ok=True)
    src = os.path.join(TESTS, 'sample.cpp')
    out = {}

    def fresh(*names):
        paths = [os.path.join(BUILD, n) for n in names]
        if not rebuild and all(os.path.exists(p) and os.path.getmtime(p) >= os.path.getmtime(src) for p in paths):
            return paths, True
        return paths, False

    # clang-600.0.57 (retail compiler) + ld64-242
    if os.path.exists(CLANG600) and os.path.isdir(SDK106):
        (o2, o1, dylib), ok = fresh('s600.o', 's600_O1.o', 's600.dylib')
        if not ok:
            base = [CLANG600, '-x', 'c++', '-arch', 'i386', '-mmacosx-version-min=10.6', '-isysroot', SDK106,
                    '-fvisibility=hidden', '-c', src]
            sh(base + ['-O2', '-o', o2])
            sh(base + ['-O1', '-o', o1])
            if os.path.exists(LD64):
                sh([LD64, '-arch', 'i386', '-dylib', '-macosx_version_min', '10.6', '-syslibroot', LINKSDK, '-o', dylib,
                    o2, f'{LINKSDK}/usr/lib/libstdc++.6.dylib', '-lSystem'])
        out['clang600'] = (o2, o1, dylib)
    else:
        t.skip('clang-600 sample build', f'{CLANG600} or {SDK106} missing')

    # current Xcode clang: i386 objects compile, linking does not
    if shutil.which('xcrun'):
        (xo,), ok = fresh('sx.o')
        if not ok:
            sh(['xcrun', 'clang++', '-arch', 'i386', '-O2', '-c', src, '-o', xo])
        r = sh(['xcrun', 'clang++', '-arch', 'i386', '-dynamiclib', xo, '-o', os.path.join(BUILD, 'sx.dylib')])
        out['xcode'] = (xo, r.returncode, (r.stderr.strip().splitlines() or [''])[-1])

    # mingw-w64 i686: COFF objects and PE DLLs (with and without a symbol table)
    gxx = shutil.which('i686-w64-mingw32-g++')
    if gxx:
        (mo, mo1, dll, dlls), ok = fresh('smg.o', 'smg_O1.o', 'smg.dll', 'smg_s.dll')
        if not ok:
            sh([gxx, '-O2', '-c', src, '-o', mo])
            sh([gxx, '-O1', '-c', src, '-o', mo1])
            link = [gxx, '-shared', '-static-libgcc', '-static-libstdc++', mo]
            sh(link + ['-o', dll])
            sh(link + ['-s', '-o', dlls])
        out['mingw'] = (mo, mo1, dll, dlls)
    else:
        t.skip('mingw sample build', 'i686-w64-mingw32-g++ not found')

    # Steam Runtime scout gcc-9 (retail Linux compiler) in Docker
    docker = shutil.which('docker')
    if docker and sh([docker, 'image', 'inspect', SCOUT]).returncode == 0:
        (eo, so, sos), ok = fresh('se.o', 'libse.so', 'libse_s.so')
        if not ok:
            r = sh([docker, 'run', '--rm', '--platform', 'linux/amd64', '-v', f'{TESTS}:/w', '-w', '/w', SCOUT, 'sh', '-c',
                    'g++-9 -m32 -O2 -fPIC -c sample.cpp -o build/se.o && g++-9 -m32 -shared build/se.o -o build/libse.so'
                    ' && strip -s build/libse.so -o build/libse_s.so'])
            if r.returncode:
                t.note('scout build failed: ' + r.stderr.strip()[-200:])
        if os.path.exists(eo):
            out['scout'] = (eo, so, sos)
    else:
        t.skip('scout gcc-9 sample build', 'docker or image missing')
    return out


def test_retail_self(t, depots, full):
    mods = [('223/19307283/bin/libtier0.dylib', 'macho'), ('223/19307283/bin/libvstdlib.dylib', 'macho'),
            ('224/19307283/bin/libtier0.so', 'elf'), ('222/19307283/bin/tier0.dll', 'pe')]
    if full:
        mods += [('223/19307283/bin/engine.dylib', 'macho'), ('223/19307283/hl2/bin/server.dylib', 'macho'),
                 ('224/19307283/bin/engine.so', 'elf'), ('222/19307283/bin/engine.dll', 'pe')]
    for rel, fmt in mods:
        p = os.path.join(depots, rel)
        if not os.path.exists(p):
            t.skip(f'self-compare {rel}', 'not present')
            continue
        f, data, R = run_compare(p, p, all_retail=True)
        t.check(f'self-compare {rel}', f['total'] > 0 and f['exact'] == f['total'],
                f'{f["exact"]}/{f["total"]} exact')
        if fmt == 'macho':
            with_obj = sum(1 for fn in R.functions if fn.obj)
            t.check(f'debug map {rel}', len(R.debug_map) > 0 and with_obj >= 0.9 * len(R.functions),
                    f'{len(R.debug_map)} objects, {with_obj}/{len(R.functions)} functions attributed')


def test_data(t, depots):
    from . import audit, data
    p = os.path.join(depots, '223/19307283/bin/libvstdlib.dylib')
    if not os.path.exists(p) or not data.x86.have_capstone():
        t.skip('data audit', 'libvstdlib.dylib or capstone missing')
        return
    doc = audit.audit(p, p, sources=False)
    s = doc['summary']
    clean = (s['differing'] == s['missing'] == s['extra'] == s['moved'] == 0 and not doc['functions']
             and not doc['data_symbols'])
    t.check('data audit of a retail module against itself is clean', clean and s['matched_functions'] > 0,
            f'{s["retail_strings"]} literals, {s["matched_functions"]} functions')
    # edit one character of a literal that code references, in a copy
    img = data.Image(p)
    R = loader.load_one(p)
    target = None
    for f in R.functions:
        for k, (va, _) in data.code_refs(img, f).items():
            txt = img.cstring_at(va)
            if txt and len(txt) > 8 and img.string_start(va) == va and txt[0:1].isalpha():
                target = (va, txt)
                break
        if target:
            break
    raw = bytearray(img.raw)
    sec = img.section_at(target[0])
    fo = sec.offset + target[0] - sec.addr
    raw[fo] ^= 0x20                                  # flip case of the first letter
    os.makedirs(BUILD, exist_ok=True)
    q = os.path.join(BUILD, 'libvstdlib-edited.dylib')
    with open(q, 'wb') as fh:
        fh.write(raw)
    doc = audit.audit(p, q, sources=False)
    hit = [m for m in doc['functions'] if m['kind'] == 'string']
    t.check('an edited literal is reported with its functions',
            doc['summary']['differing'] == 1 and hit and all(target[1].decode('latin-1')[1:] in m['retail'] for m in hit),
            f'{target[1][:30]!r}: {len(hit)} function(s)')


def test_samples(t, b):
    if 'clang600' in b:
        o2, o1, dylib = b['clang600']
        if os.path.exists(dylib):
            f, _, _ = run_compare(dylib, o2)
            t.check('Mach-O clang-600 object vs ld64-242 dylib', f['total'] >= 10 and f['differs'] == 0,
                    f'{f["matched"]}/{f["total"]} matched')
            f, _, _ = run_compare(dylib, o1)
            t.check('Mach-O -O1 object vs -O2 dylib reports differences', f['differs'] > 0, f'{f["differs"]} differ')
            f, _, _ = run_compare(dylib, dylib)
            t.check('Mach-O linked dylib vs itself', f['exact'] == f['total'] > 0, f'{f["exact"]}/{f["total"]}')
            R = loader.load_one(dylib)
            o = loader.load_one(o2)
            idx = compare.Index(R.functions)
            fn, r = next((fn, idx.find(fn)) for fn in o.functions
                         if any(rel.kind == 'pcrel' and rel.target for rel in fn.relocs)
                         and idx.find(fn) and compare.compare(idx.find(fn), fn).status == compare.MASKED)
            next(rel for rel in fn.relocs if rel.kind == 'pcrel' and rel.target).target = '_Z9not_a_funcv'
            res = compare.compare(r, fn)
            t.check('call to a different symbol is flagged', res.status == compare.DIFFERS and 'reloc target' in res.reason,
                    res.reason)
        else:
            t.skip('Mach-O object vs dylib', 'ld64-242 link unavailable')
    if 'xcode' in b:
        xo, rc, msg = b['xcode']
        f, _, _ = run_compare(xo, xo)
        t.check('Mach-O object from current Xcode clang parses (self-compare)', f['exact'] == f['total'] > 0,
                f'{f["total"]} functions')
        t.check('current Xcode cannot link i386', rc != 0, msg[:120])
    if 'mingw' in b:
        mo, mo1, dll, dlls = b['mingw']
        f, _, _ = run_compare(dll, mo)
        t.check('COFF object vs PE DLL with symbols', f['total'] >= 10 and f['differs'] == 0, f'{f["matched"]}/{f["total"]}')
        f, _, _ = run_compare(dll, mo1)
        t.check('COFF -O1 object vs -O2 DLL reports differences', f['differs'] > 0, f'{f["differs"]} differ')
        f, _, _ = run_compare(dlls, mo)
        t.check('COFF object vs stripped DLL (exports only)', f['total'] >= 6 and f['differs'] == 0,
                f'{f["matched"]}/{f["total"]}')
        mp = os.path.join(BUILD, 'smg_s.map')
        from .cli import main as cli_main
        cli_main(['locate', '--retail', dlls, '--ours', mo, '-o', mp])
        R = loader.load_one(dlls)
        from .mapfile import apply_map
        apply_map(R, mp)
        funcs = loader.load_one(mo).functions
        data = report.run(R, funcs)
        g = report.summarize(data['results'])['functions']
        t.check('locate + --retail-map names more of the stripped DLL', g['total'] > f['total'] and g['differs'] == 0,
                f'{g["matched"]}/{g["total"]} (exports only: {f["total"]})')
    if 'scout' in b:
        eo, so, sos = b['scout']
        f, _, _ = run_compare(so, eo)
        t.check('ELF scout gcc-9 object vs .so', f['total'] >= 10 and f['differs'] == 0, f'{f["matched"]}/{f["total"]}')
        if os.path.exists(sos):
            from . import elfmatch
            R = loader.load_one(sos)
            funcs = loader.load_one(eo).functions
            was = elfmatch.stripped(R)
            # functions without an FDE inside FDE-less CRT code (glibc's crti.o get_pc_thunk.bx) can't be told apart
            starts = {fn.addr for fn in R.functions}
            unstripped = compare.Index(loader.load_one(so).functions)
            want = sum(1 for o in funcs if unstripped.find(o) and unstripped.find(o).addr in starts)
            elfmatch.name_retail(R, funcs)
            g = report.summarize(report.run(R, funcs)['results'])['functions']
            t.check('ELF object vs stripped .so (.eh_frame extents, located by search)',
                    was and g['matched'] == want >= 10 and g['differs'] == 0, f'{g["matched"]}/{want}')


def test_workflow(t, depots):
    engine = os.path.join(depots, '223/19307283/bin/engine.dylib')
    cc = os.path.join(HERE, 'macos-cc.sh')
    if not (os.path.exists(engine) and os.path.exists(CLANG600)):
        t.skip('clang-600 workflow', 'engine.dylib or clang-600 missing')
        return
    for src in ('tier1/checksum_md5.cpp', 'tier1/checksum_crc.cpp', 'mathlib/IceKey.cpp'):
        out = os.path.join(BUILD, os.path.basename(src).replace('.cpp', '.o'))
        r = sh([cc, '-o', out, os.path.join(REPO, src)])
        if r.returncode:
            t.check(f'compile {src}', False, r.stderr.strip()[-200:])
            continue
        f, data, _ = run_compare(engine, out)
        t.check(f'{src} (clang-600) vs retail engine.dylib', f['total'] > 0 and f['matched'] == f['total'],
                f'{f["matched"]}/{f["total"]} matched, scope {data["mode"]}')


def test_pilot(t, depots):
    engine = os.path.join(depots, '223/19307283/bin/engine.dylib')
    if not (os.path.isdir(PILOT) and os.path.exists(engine)):
        t.skip('pilot regression', 'pilot objects or engine.dylib missing')
        return
    f, data, _ = run_compare(engine, PILOT)
    t.check('survey pilot a600 vs engine.dylib', f['matched'] >= PILOT_MATCHED,
            f'{f["matched"]}/{f["total"]} matched (pilot script: {PILOT_MATCHED}/399)')


def test_no_capstone(t, depots, b):
    env = dict(os.environ, BYTEMATCH_NO_CAPSTONE='1')
    runs = []
    if 'clang600' in b and os.path.exists(b['clang600'][2]):
        runs.append(('sample dylib vs object', b['clang600'][2], b['clang600'][0], 'all'))
    engine = os.path.join(depots, '223/19307283/bin/engine.dylib')
    if os.path.isdir(PILOT) and os.path.exists(engine):
        runs.append(('pilot a600 vs engine.dylib', engine, PILOT, 'pilot'))
    for name, retail, ours, kind in runs:
        r = sh([sys.executable, '-m', 'bytematch', 'diff', '--retail', retail, '--ours', ours, '--json'], env=env, cwd=HERE)
        if r.returncode:
            t.check(f'no-capstone {name}', False, r.stderr.strip()[-200:])
            continue
        f = json.loads(r.stdout)['summary']['functions']
        ok = f['differs'] == 0 if kind == 'all' else f['matched'] >= 0.95 * PILOT_MATCHED
        t.check(f'no-capstone {name}', ok, f'{f["matched"]}/{f["total"]} matched')


def test_pe_matcher(t):
    import struct
    from . import model, winmap

    # 1. sig() normalization: function-pointer return types and repeated template args
    cases = [
        ('void * (__cdecl * __cdecl ClientDLL_GetFactory(void))(char const *, int *)',
         'sig:ClientDLL_GetFactory()'),
        ('public: virtual void * (__cdecl * __thiscall CEngineVGui::GetGameUIFactory(void))(char const *, int *)',
         'sig:CEngineVGui::GetGameUIFactory()'),
        ('public: virtual void (__cdecl * __thiscall CNetworkStringTable::GetCallback(void))(void *, class INetworkStringTable *, int)',
         'sig:CNetworkStringTable::GetCallback()'),
        ('public: __thiscall CUtlLinkedList<int, unsigned short>::~CUtlLinkedList<int, unsigned short>(void)',
         'sig:CUtlLinkedList<int,unsignedshort>::~CUtlLinkedList()'),
        ('public: int __thiscall Foo::operator()(int)',
         'sig:Foo::operator()(int)'),
        ('public: int __thiscall Foo<int>::Bar<int>(void)',
         'sig:Foo<int>::Bar<int>()'),
    ]
    bad = [(inp, winmap.sig(inp), want) for inp, want in cases if winmap.sig(inp) != want]
    t.check('PE sig() fn-ptr return & repeated template args', not bad, str(bad[:1]) if bad else f'{len(cases)}/{len(cases)}')

    # Build a synthetic linked PE image at 0x10001000 for stripped-PE rules
    base = 0x10001000
    R = model.Module('<synth-retail>', 'pe', True)
    R.image_base, R.image_end = base, base + 0x10000
    O = model.Module('<synth-ours>', 'coff', False)

    # Layout in .text (16-byte aligned functions):
    # +0x00: dtor (7 B: mov [ecx], 0x10009000; ret) + 9 B int3
    # +0x10: atexit stub (10 B: mov ecx, 0x10008000; jmp +0x00) + 6 B int3
    # +0x20: dynamic initializer (18 B: mov ecx, 0x10008000; push +0x10; call +0x50; pop ecx; ret) + 14 B int3
    # +0x40: merged preceding func (12 B) + 4 B int3; +0x50: real target func (16 B, exact mult of 16, ends in ret)
    # +0x60: frameless successor func (8 B: push edi; call +0x50; pop edi; ret)
    # +0x70: thunk (7 B: mov eax, [ecx+0xc0]; ret) + 9 B int3
    # +0x80: dtor-array-1 (10 B) + 6 B int3; +0x90: dtor-array-2 (10 B, matches ours) + 6 B int3
    # +0xa0: caller referencing +0x90 (12 B) + 4 B int3
    # +0xb0: jmp +0x60 (tail call proving +0x60 is a function start)
    buf = bytearray(b'\xcc' * 0xc0)
    dtor_code = b'\xc7\x01' + struct.pack('<I', base + 0x9000) + b'\xc3'
    buf[0x00:0x07] = dtor_code
    stub_code = b'\xb9' + struct.pack('<I', base + 0x8000) + b'\xe9' + struct.pack('<i', 0x00 - 0x1a)
    buf[0x10:0x1a] = stub_code
    init_code = (b'\xb9' + struct.pack('<I', base + 0x8000) +
                 b'\x68' + struct.pack('<I', base + 0x10) +
                 b'\xe8' + struct.pack('<i', 0x50 - 0x2f) + b'\x59\xc3')
    buf[0x20:0x31] = init_code
    prev_code = b'\x55\x8b\xec\x83\xc0\x01\x83\xc0\x02\x5d\xc3'
    buf[0x40:0x4b] = prev_code
    real_code = b'\x55\x8b\xec\x8b\x45\x08\x83\xc0\x07\x03\xc0\x83\xc0\x01\x5d\xc3'  # 16 B exact!
    buf[0x50:0x60] = real_code
    succ_code = b'\x57\xe8' + struct.pack('<i', 0x50 - 0x66) + b'\x5f\xc3'
    buf[0x60:0x68] = succ_code
    thunk_code = b'\x8b\x81\xc0\x00\x00\x00\xc3'
    buf[0x70:0x77] = thunk_code
    arr1_code = b'\x56\x57\xbe' + struct.pack('<I', base + 0x8100) + b'\x5f\x5e\xc3'  # 10 B (different)
    arr2_code = b'\x56\x57\xbf' + struct.pack('<I', base + 0x8200) + b'\x5f\x5e\xc3'  # 10 B (matches ours)
    buf[0x80:0x8a] = arr1_code
    buf[0x90:0x9a] = arr2_code
    caller_code = b'\x68' + struct.pack('<I', base + 0x90) + b'\xe8' + struct.pack('<i', 0x50 - 0xaa) + b'\x59\xc3'
    buf[0xa0:0xac] = caller_code
    buf[0xb0:0xb5] = b'\xe9' + struct.pack('<i', 0x60 - 0xb5)
    R.code.append((base, bytes(buf), '.text'))

    r_stub = model.Function('sig:WrongDtor()', 'sig:WrongDtor()', base + 0x10, bytes(buf[0x10:0x1a]),
                            obj='foo', size_known=False, raw_size=16,
                            relocs=[model.Reloc(1, 4, 'abs')], module=R)
    r_init = model.Function('__GLOBAL__I_a', '__GLOBAL__I_a', base + 0x20, bytes(buf[0x20:0x31]),
                            obj='foo', size_known=False, raw_size=32,
                            relocs=[model.Reloc(1, 4, 'abs'), model.Reloc(6, 4, 'abs')], module=R)
    # Merged extent: covers prev_code (+0x40), real_code (+0x50), AND succ_code (+0x60) with no boundary!
    r_merged = model.Function('sig:RealFn()', 'sig:RealFn()', base + 0x40, bytes(buf[0x40:0x68]),
                              obj='foo', size_known=False, raw_size=40, module=R)
    r_thunk = model.Function('sig:CBase::Get()#thunk4', 'sig:CBase::Get()#thunk4', base + 0x70,
                             bytes(buf[0x70:0x77]), obj='foo', size_known=False, raw_size=16, module=R)
    r_arr1 = model.Function('___cxx_global_array_dtor', '__cxx_global_array_dtor', base + 0x80,
                            bytes(buf[0x80:0x8a]), obj='bar', size_known=False, raw_size=16, module=R)
    r_arr2 = model.Function('___cxx_global_array_dtor', '__cxx_global_array_dtor', base + 0x90,
                            bytes(buf[0x90:0x9a]), obj='foo', size_known=False, raw_size=16, module=R)
    r_caller = model.Function('sig:Caller()', 'sig:Caller()', base + 0xa0, bytes(buf[0xa0:0xac]),
                              obj='foo', size_known=False, raw_size=16, module=R)
    R.functions = [r_stub, r_init, r_merged, r_thunk, r_arr1, r_arr2, r_caller]
    for fn in (r_init, r_merged, r_thunk, r_arr1, r_arr2, r_caller):
        R.add_name(fn.addr, fn.key)

    o_dtor = model.Function('??1CDtor@@UAE@XZ', '??1CDtor@@UAE@XZ', 0,
                            b'\xc7\x01\x00\x00\x00\x00\xc3', obj='foo',
                            relocs=[model.Reloc(2, 4, 'abs', 'vft')], module=O)
    o_stub = model.Function('??__Fg_x@@YAXXZ', '??__Fg_x@@YAXXZ', 0x10,
                            b'\xb9\x00\x00\x00\x00\xe9\x00\x00\x00\x00', obj='foo',
                            relocs=[model.Reloc(1, 4, 'abs', 'g_x'), model.Reloc(6, 4, 'pcrel', '??1CDtor@@UAE@XZ')],
                            module=O)
    o_init = model.Function('??__Eg_x@@YAXXZ', '??__Eg_x@@YAXXZ', 0x20,
                            b'\xb9\x00\x00\x00\x00\x68\x00\x00\x00\x00\xe8\x00\x00\x00\x00\x59\xc3', obj='foo',
                            relocs=[model.Reloc(1, 4, 'abs', 'g_x'), model.Reloc(6, 4, 'abs', '??__Fg_x@@YAXXZ'),
                                    model.Reloc(11, 4, 'pcrel', 'sig:RealFn()')], module=O)
    o_real = model.Function('?RealFn@@YAXXZ', '?RealFn@@YAXXZ', 0x50, real_code, obj='foo',
                            aliases=['sig:RealFn()'], module=O)
    o_thunk = model.Function('?Get@CBase@@UAEHXZ', '?Get@CBase@@UAEHXZ', 0x70, thunk_code, obj='foo',
                             aliases=['sig:CBase::Get()'], module=O)
    o_arr2 = model.Function('??__Farr@@YAXXZ', '??__Farr@@YAXXZ', 0x90,
                            b'\x56\x57\xbf\x00\x00\x00\x00\x5f\x5e\xc3', obj='foo',
                            relocs=[model.Reloc(3, 4, 'abs', 'g_arr')], module=O)
    o_caller = model.Function('?Caller@@YAXXZ', '?Caller@@YAXXZ', 0xa0,
                              b'\x68\x00\x00\x00\x00\xe8\x00\x00\x00\x00\x59\xc3', obj='foo',
                              relocs=[model.Reloc(1, 4, 'abs', '??__Farr@@YAXXZ'),
                                      model.Reloc(6, 4, 'pcrel', 'sig:RealFn()')],
                              aliases=['sig:Caller()'], module=O)
    # Put WrongDtor in ours with wrong bytes so r_stub starts as DIFFERS (just like engine.dll)
    o_wrong = model.Function('?WrongDtor@@YAXXZ', '?WrongDtor@@YAXXZ', 0x100, b'\x90' * 30, obj='foo',
                             aliases=['sig:WrongDtor()'], module=O)
    R.add_name(base + 0x50, 'sig:RealFn()')

    # 2. _find_late_start + _ends_at (exact multiple of 16 followed by tail-called frameless func)
    res_m = compare.compare(r_merged, o_real, True)
    t.check('PE stripped late-start & ret+call_target extent',
            res_m.status == compare.EXACT and getattr(res_m, 'matched_at', None) == base + 0x50,
            f'{res_m.status} @ {hex(getattr(res_m, "matched_at", 0))}')

    # 3. Full report.run(): verifies #thunk, ref_map atexit stub, and VA-disambiguated equivalent target
    data = report.run(R, [o_dtor, o_stub, o_init, o_real, o_thunk, o_arr2, o_caller, o_wrong], objects=['foo'])
    by_addr = {r.retail.addr: r for r in data['results']}
    t.check('PE virtual adjustor #thunk pairing',
            by_addr[base + 0x70].status == compare.EXACT and getattr(by_addr[base + 0x70], 'matched_as', None) == o_thunk.name,
            f'{by_addr[base + 0x70].status} as {getattr(by_addr[base + 0x70], "matched_as", None)}')
    t.check('PE matched-reloc atexit stub resolution',
            by_addr[base + 0x10].status == compare.MASKED and getattr(by_addr[base + 0x10], 'matched_as', None) == o_stub.name,
            f'{by_addr[base + 0x10].status} as {getattr(by_addr[base + 0x10], "matched_as", None)}')
    t.check('PE VA-based equivalent target disambiguates duplicate names',
            by_addr[base + 0xa0].status == compare.MASKED and bool(getattr(by_addr[base + 0xa0], 'equivalent_targets', None)),
            f'{by_addr[base + 0xa0].status}')
    # Negative check: mismatched bytes on #thunk must NOT match
    o_bad_thunk = model.Function('?Get@CBase@@UAEHXZ', '?Get@CBase@@UAEHXZ', 0x70,
                                 b'\x8b\x81\xc4\x00\x00\x00\xc3', obj='foo', aliases=['sig:CBase::Get()'], module=O)
    bad_data = report.run(R, [o_bad_thunk], objects=['foo'])
    bad_thunk_res = [r for r in bad_data['results'] if r.retail.addr == base + 0x70][0]
    t.check('PE mismatched #thunk bytes are rejected', bad_thunk_res.status == compare.MISSING, bad_thunk_res.status)

    # 4. _find_early_start: map entry started mid-function right after an inlined DebuggerBreak int3
    R2 = model.Module('<synth-early>', 'pe', True)
    R2.image_base, R2.image_end = base, base + 0x10000
    early_code = b'\x55\x8b\xec\x83\xec\x10\x53\x8b\xd9\x33\xc0\x85\xc0\x74\x01\xcc\x5b\x8b\xe5\x5d\xc3'  # 21 B, int3 at +15
    buf2 = bytearray(b'\xcc' * 0x40)
    buf2[0x00:len(early_code)] = early_code
    R2.code.append((base, bytes(buf2), '.text'))
    R2.port_starts = {base}
    r_mid = model.Function('sig:EarlyFn()', 'sig:EarlyFn()', base + 16, bytes(buf2[16:len(early_code)]),
                           obj='foo', size_known=False, raw_size=len(early_code) - 16, module=R2)
    R2.functions = [r_mid]
    o_early = model.Function('?EarlyFn@@YAXXZ', '?EarlyFn@@YAXXZ', 0, early_code, obj='foo',
                             aliases=['sig:EarlyFn()'], module=O)
    res_e = compare.compare(r_mid, o_early, True)
    t.check('PE stripped early-start across inlined int3',
            res_e.status == compare.EXACT and getattr(res_e, 'matched_at', None) == base,
            f'{res_e.status} @ {hex(getattr(res_e, "matched_at", 0))}')


def test_macho_pic_and_data(t):
    import struct
    from . import data, model, x86

    if not x86.have_capstone():
        t.skip('Mach-O CFG PIC & negative-index tests', 'capstone missing')
        return

    base = 0x10000

    # 1. Backward jump after register copy (RebalanceThinks pattern) + non-PIC overwrite check:
    # +00: call +5 (e8 00 00 00 00) -> +05: pop ebx (5b)
    # +06: jmp +0x15 (eb 0d) -> L_hdr (+15)
    # +08: L_body: mov eax, [edi + 0x123456] (8b 87 56 34 12 00)   <- disp at +0a (PIC!)
    # +0e: mov ecx, [ebx + 0x234567]         (8b 8b 67 45 23 00)   <- disp at +10 (NOT PIC: ebx clobbered!)
    # +14: ret (c3)
    # +15: L_hdr: mov edi, ebx (89 df)
    # +17: xor ebx, ebx (31 db)
    # +19: jmp L_body (eb ed)
    code1 = (
        b'\xe8\x00\x00\x00\x00\x5b'
        b'\xeb\x0d'
        b'\x8b\x87' + struct.pack('<i', 0x123456) +
        b'\x8b\x8b' + struct.pack('<i', 0x234567) +
        b'\xc3'
        b'\x89\xdf\x31\xdb\xeb\xed'
    )
    af1 = {(off, kind) for off, w, kind in x86.address_fields(code1, base)}
    t.check('CFG PIC tracks reg copy across backward jump without masking clobbered reg',
            (0x0a, 'pic') in af1 and (0x10, 'pic') not in af1,
            f'fields={sorted(af1)}')

    # 2. Backward jump after stack-spill reload across alignment nop (ForceDynamicLinkNeighbors pattern):
    # +00: call +5 (5) ; +05: pop edx (5a) ; +06: mov [ebp-0x10], edx (89 55 f0)
    # +09: jmp L_hdr (eb 08 -> +13) ; +0b: nop (90)
    # +0c: L_body: mov eax, [ebx + 0x123456] (8b 83 56 34 12 00)  <- disp at +0e (PIC!)
    # +12: ret (c3)
    # +13: L_hdr: mov ebx, [ebp-0x10] (8b 5d f0) ; +16: jne L_body (75 f4 -> +0c) ; +18: ret (c3)
    code2 = (
        b'\xe8\x00\x00\x00\x00\x5a\x89\x55\xf0'
        b'\xeb\x08\x90'
        b'\x8b\x83' + struct.pack('<i', 0x123456) + b'\xc3'
        b'\x8b\x5d\xf0\x75\xf4\xc3'
    )
    af2 = {(off, kind) for off, w, kind in x86.address_fields(code2, base)}
    t.check('CFG PIC tracks stack-spill reload across jmp + alignment nop',
            (0x0e, 'pic') in af2, f'fields={sorted(af2)}')

    # 3. Clobbered PIC reg on one path into a join must NOT be masked:
    # +00: call +5 (5) ; +05: pop ebx (5b) ; +06: test eax, eax (85 c0) ; +08: je +0c (74 02)
    # +0a: xor ebx, ebx (31 db)
    # +0c: mov eax, [ebx + 0x123456] (8b 83 56 34 12 00)  <- disp at +0e (NOT PIC!) ; +12: ret (c3)
    code3 = b'\xe8\x00\x00\x00\x00\x5b\x85\xc0\x74\x02\x31\xdb\x8b\x83' + struct.pack('<i', 0x123456) + b'\xc3'
    af3 = {(off, kind) for off, w, kind in x86.address_fields(code3, base)}
    t.check('CFG PIC rejects register clobbered on one incoming path',
            (0x0e, 'pic') not in af3, f'fields={sorted(af3)}')

    # 3b. Noreturn `ret; fstp st(0); call ext` epilogue stub must not fall through and
    # poison the following block with the early-return path's clobbered stack slot:
    # +00: call +5 (5) ; +05: pop edx (5a) ; +06: test eax, eax (85 c0) ; +08: je +0f (74 05)
    # +0a: mov [ebp-0x10], edx (89 55 f0) ; +0d: jmp +1c (eb 0d)
    # +0f: mov [ebp-0x10], eax (89 45 f0) ; +12: jne +15 (75 01) ; +14: ret (c3)
    # +15: fstp st(0) (dd d8) ; +17: call ext (e8 ...)
    # +1c: mov eax, [ebp-0x10] (8b 45 f0) ; +1f: mov eax, [eax+0x123456] (8b 80 <disp32>) ; +25: ret
    code3b = (
        b'\xe8\x00\x00\x00\x00\x5a\x85\xc0\x74\x05'
        b'\x89\x55\xf0\xeb\x0d'
        b'\x89\x45\xf0\x75\x01\xc3\xdd\xd8\xe8' + struct.pack('<i', 0x10000) +
        b'\x8b\x45\xf0\x8b\x80' + struct.pack('<i', 0x123456) + b'\xc3'
    )
    af3b = {(off, kind) for off, w, kind in x86.address_fields(code3b, base)}
    t.check('CFG PIC does not fall through ret + fstp + external noreturn call',
            (0x21, 'pic') in af3b, f'fields={sorted(af3b)}')

    # Helper to build minimal synthetic data.Image for --data-check tests
    def synth_img(const_addr, const_bytes, data_addr, data_syms):
        img = object.__new__(data.Image)
        img.path = '<synth>'
        img.linked = True
        img.absolute = False
        s_const = data.Section(seg='__TEXT', name='__const', addr=const_addr, size=len(const_bytes),
                               offset=0, data=const_bytes, type=0, index=1, reloff=0, nreloc=0, r1=0, r2=0)
        s_bss = data.Section(seg='__DATA', name='__common', addr=data_addr, size=0x1000,
                             offset=0, data=None, type=1, index=2, reloff=0, nreloc=0, r1=0, r2=0)
        img.sections = [s_const, s_bss]
        img._sec_starts = [(s_const.addr, 0), (s_bss.addr, 1)]
        img.names = {a: [nm] for a, nm in data_syms}
        img.sec_syms = {2: sorted(a for a, _ in data_syms)}
        img.slots, img.indirect, img.code_fields = {}, {}, {}
        img.objects, img.addr_obj, img.name_obj, img._cstr_cache = [], {}, {}, {}
        return img

    # 4. Negative-indexed constant table (`cmp ecx, -4; jb; mov edx, [ebx + ecx*4 + table_end]`):
    # +00: call +5 (5) ; +05: pop ebx (5b) ; +06: cmp ecx, -4 (83 f9 fc) ; +09: jb +12 (72 07)
    # +0b: mov edx, [ebx + ecx*4 + disp] (8b 94 8b <disp32>) ; +12: ret (c3)
    tbl_ok = struct.pack('<4I', 1, 3, 4, 2)
    tbl_bad = struct.pack('<4I', 1, 3, 5, 2)
    r_const = tbl_ok + struct.pack('<f', -0.75)
    o_const = tbl_ok + struct.pack('<f', 250000.0)
    o_const_bad = tbl_bad + struct.pack('<f', 250000.0)
    r_base_va, o_base_va = 0x20010, 0x30010      # table_end on each side
    code_neg_r = (b'\xe8\x00\x00\x00\x00\x5b\x83\xf9\xfc\x72\x07\x8b\x94\x8b'
                  + struct.pack('<i', r_base_va - (base + 5)) + b'\xc3')
    code_neg_o = (b'\xe8\x00\x00\x00\x00\x5b\x83\xf9\xfc\x72\x07\x8b\x94\x8b'
                  + struct.pack('<i', o_base_va - (base + 5)) + b'\xc3')
    fr = model.Function('f', 'f', base, code_neg_r)
    fo = model.Function('f', 'f', base, code_neg_o)
    rimg = synth_img(0x20000, r_const, 0x40000, [(0x40010, '_g_OtherA'), (0x40040, '_g_Arr')])
    oimg = synth_img(0x30000, o_const, 0x50000, [(0x50010, '_g_OtherB'), (0x50040, '_g_Arr')])
    oimg_bad = synth_img(0x30000, o_const_bad, 0x50000, [(0x50010, '_g_OtherB'), (0x50040, '_g_Arr')])
    m_ok = data.check_function(rimg, oimg, fr, fo)
    m_bad = data.check_function(rimg, oimg_bad, fr, fo)
    t.check('data-check verifies negative-indexed table contents and ignores trailing constant',
            not m_ok and len(m_bad) == 1,
            f'same={len(m_ok)} diff={len(m_bad)}')

    # 5. Negative-biased indexed symbol base (`lea ebx, [eax + esi + (&g_Arr[-1] - pic)]`):
    # +00: call +5 (5) ; +05: pop esi (5e) ; +06: lea ebx, [eax + esi + disp] (8d 9c 30 <disp32>) ; +0d: ret (c3)
    code_sym_r = b'\xe8\x00\x00\x00\x00\x5e\x8d\x9c\x30' + struct.pack('<i', 0x40020 - (base + 5)) + b'\xc3'
    code_sym_o = b'\xe8\x00\x00\x00\x00\x5e\x8d\x9c\x30' + struct.pack('<i', 0x50020 - (base + 5)) + b'\xc3'
    oimg_wrong_sym = synth_img(0x30000, o_const, 0x50000, [(0x50010, '_g_OtherB'), (0x50040, '_g_Wrong')])
    oimg_wrong_off = synth_img(0x30000, o_const, 0x50000, [(0x50010, '_g_OtherB'), (0x50060, '_g_Arr')])
    fsr = model.Function('g', 'g', base, code_sym_r)
    fso = model.Function('g', 'g', base, code_sym_o)
    s_ok = data.check_function(rimg, oimg, fsr, fso)
    s_wsym = data.check_function(rimg, oimg_wrong_sym, fsr, fso)
    s_woff = data.check_function(rimg, oimg_wrong_off, fsr, fso)
    t.check('data-check matches negative-biased indexed symbol base and rejects wrong target/stride',
            not s_ok and len(s_wsym) == 1 and len(s_woff) == 1,
            f'same={len(s_ok)} wrong_sym={len(s_wsym)} wrong_stride={len(s_woff)}')


def main(args):
    t = Runner()
    from . import x86
    print(f'capstone: {"yes" if x86.have_capstone() else "no"}')
    test_pe_matcher(t)
    test_macho_pic_and_data(t)
    test_retail_self(t, args.depots, args.full)
    test_data(t, args.depots)
    b = build_samples(t, rebuild=args.rebuild)
    test_samples(t, b)
    test_workflow(t, args.depots)
    test_pilot(t, args.depots)
    test_no_capstone(t, args.depots, b)
    print(f'\n{"FAILED" if t.fails else "OK"}: {t.fails} failure(s)')
    return 1 if t.fails else 0
