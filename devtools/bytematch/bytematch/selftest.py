"""Self tests. Each check prints PASS/FAIL/SKIP; exit status 1 on any FAIL.

1. Retail modules compared against themselves must be 100% exact.
2. tests/sample.cpp built by each available toolchain: object vs its linked
   module must match fully; an -O1 object vs the -O2 module must differ.
3. clang-600.0.57 workflow: tier1/mathlib files from this tree compiled with
   macos-cc.sh must fully match retail engine.dylib.
4. The survey pilot objects (clang-600) must reach the pilot's 311 matches.
5. The same comparisons without capstone (byte-level fallback)."""
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
        (eo, so), ok = fresh('se.o', 'libse.so')
        if not ok:
            r = sh([docker, 'run', '--rm', '--platform', 'linux/amd64', '-v', f'{TESTS}:/w', '-w', '/w', SCOUT, 'sh', '-c',
                    'g++-9 -m32 -O2 -fPIC -c sample.cpp -o build/se.o && g++-9 -m32 -shared build/se.o -o build/libse.so'])
            if r.returncode:
                t.note('scout build failed: ' + r.stderr.strip()[-200:])
        if os.path.exists(eo):
            out['scout'] = (eo, so)
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
        eo, so = b['scout']
        f, _, _ = run_compare(so, eo)
        t.check('ELF scout gcc-9 object vs .so', f['total'] >= 10 and f['differs'] == 0, f'{f["matched"]}/{f["total"]}')


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


def main(args):
    t = Runner()
    from . import x86
    print(f'capstone: {"yes" if x86.have_capstone() else "no"}')
    test_retail_self(t, args.depots, args.full)
    b = build_samples(t, rebuild=args.rebuild)
    test_samples(t, b)
    test_workflow(t, args.depots)
    test_pilot(t, args.depots)
    test_no_capstone(t, args.depots, b)
    print(f'\n{"FAILED" if t.fails else "OK"}: {t.fails} failure(s)')
    return 1 if t.fails else 0
