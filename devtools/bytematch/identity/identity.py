#!/usr/bin/env python3
"""Rebuild a --retail-osx32 module so the whole file can match retail.

The --retail-osx32 build already reproduces code and data. What is left is the
debug map ld64 writes into the symbol table: N_SO (the compiled file and its
directory), N_SOL (headers with code, from the DWARF line tables), N_OSO (the
object or archive member path and its mtime). This tool reproduces those
inputs the way Valve's buildbot produced them:

1. ccache-style compile. Each source is preprocessed with our flags (absolute,
   normalized include paths), our tree / SDK / clang resource paths in the .ii's
   line markers are rewritten to Valve's (/Users/buildbot/.../src, Xcode's
   MacOSX10.9.sdk, Xcode's lib/clang/6.0; __FILE__ already is Valve's through
   the retail flags' -fmacro-prefix-map), and the .ii is compiled under its retail ccache temp
   name (e.g. inputsyste.tmp.hl2-osx-builder.2104.ii) with Valve's compilation
   directory (/Users/buildbot/buildslave/../.ccache/tmp).
2. Objects are written under a scratch root at Valve's Xcode paths
   (<root>/Users/buildbot/.../Objects-normal/i386/inputsystem.o) with the
   retail N_OSO mtimes. Static libraries are re-archived with retail member
   names (convar.o) and member dates, in our archive order.
3. The waf link command (build-retail/link_commands.json, from
   `waf retail_linkdb`) is re-run with Valve's object and archive paths. The
   buildhost.dylib shim maps /Users/buildbot/ onto the scratch root for the
   clang driver and ld64, so ld64 records Valve's paths verbatim, and gives
   ld64 the OS X 10.9/10.10 qsort (section order ties).
4. The result is compared with the retail file (machocmp.py).

No post-link patching is done. LC_UUID follows by itself: ld64 hashes the file
without the stab nlists and stab strings (see lduuid.py).

  identity.py inputsystem                  rebuild and compare one module
  identity.py --all                        every dylib in link_commands.json
  identity.py --identical                  the modules in identical.json; fails unless each
                                           output equals retail byte for byte (see ci.sh)

The scratch tree (<build-dir>/identity/roots/<module>) is kept for inspection;
compiled objects are cached in <build-dir>/identity/cache.
"""
import argparse
import concurrent.futures
import hashlib
import json
import os
import re
import shlex
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
TREE = os.path.normpath(os.path.join(HERE, '..', '..', '..'))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, '..', 'toolchain'))
import machocmp  # noqa: E402

MANIFEST = os.path.join(TREE, 'devtools', 'bytematch', 'manifests', 'retail-osx32.json')
MODULE_DIR = os.path.join(TREE, 'devtools', 'bytematch', 'manifests', 'modules')
ALIASES = os.path.join(HERE, 'aliases.json')
IDENTICAL = os.path.join(HERE, 'identical.json')

XCODE = '/Applications/Xcode.app/Contents/Developer/'
VALVE_SDK = XCODE + 'Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk/'
VALVE_RESOURCE = XCODE + 'Toolchains/XcodeDefault.xctoolchain/usr/bin/../lib/clang/6.0/'
VALVE_TOP = '/Users/buildbot/'
DEFAULT_CCACHE_DIR = '/Users/buildbot/buildslave/../.ccache/tmp/'

N_SO, N_OSO = 0x64, 0x66
PREPROCESSOR_ONLY = ('-I', '-D', '-U', '-isystem', '-iquote', '-include', '-imacros')
DEP_FLAGS = ('-MMD', '-MD', '-MP')
LANG_EXT = {'c': '.i', 'c++': '.ii', 'objective-c++': '.mii', 'objective-c': '.mi'}
# Per-module archive variants (retail_stlib_variant: sourcevr's 10.5 libtier1_105.a) that
# retail links under the plain name (build/Release/libtier1.a).
ARCHIVE_VARIANT = r'_105\.a$'
LINE_MARKER = re.compile(rb'^(# \d+ ")([^"\n]*)(")', re.M)
# Archives Valve took prebuilt from another build tree, which we compile from source (the
# manifest has no source for their members, so they are matched by member name):
# {archive: (our root, how that build named headers under it, its SDK)}. Steam's CEF build
# ran Xcode from src/cef with -I., so the debug map has the main file's absolute path but
# ./include/... for headers, and the 10.6 SDK.
FOREIGN_ARCHIVES = {
    'libcef_dll_wrapper.a': ('external/cef/', './', XCODE + 'Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.6.sdk/'),
}
# Prebuilt third-party archives whose members carry DWARF debug info (so ld64 emits
# N_SO/N_SOL/N_OSO stabs). Rebuilt from the toolchain work tree with the period gcc
# driver and Valve's SDK/source paths before archiving with retail member mtimes.
CRYPTOPP_FLAGS = [
    '-mmacosx-version-min=10.5', '-DNDEBUG', '-g', '-O2', '-fPIC',
    '-fvisibility=hidden', '-w', '-pipe', '-DCRYPTOPP_DISABLE_ASM',
]
# Apple's 10.9 SDK crt1.10.6.o (Csu-79): defines _pvars in __DATA,__program_vars
# with .no_dead_strip (so -dead_strip keeps it) and NXArgc/NXArgv/environ/__progname
# as __DATA,__common zerofills. The 10.6 SDK stub lacks _pvars and puts them in __data.
CRT1_10_6_ASM = """\t.section __TEXT,__text,regular,pure_instructions
\t.globl start
\t.align 2
start:
\tpushl $0
\tmovl %esp, %ebp
\tandl $-16, %esp
\tsubl $16, %esp
\tmovl 4(%ebp), %ebx
\tmovl %ebx, (%esp)
\tleal 8(%ebp), %ecx
\tmovl %ecx, 4(%esp)
\taddl $1, %ebx
\tshll $2, %ebx
\taddl %ecx, %ebx
\tmovl %ebx, 8(%esp)
Lloop:
\tmovl (%ebx), %eax
\taddl $4, %ebx
\ttestl %eax, %eax
\tjne Lloop
\tmovl %ebx, 12(%esp)
\tcall _main
\tmovl %eax, (%esp)
\tcall _exit
\thlt

\t.section __DATA,__program_vars
\t.align 2
\t.no_dead_strip _pvars
_pvars:
\t.long ___dso_handle
\t.long _NXArgc
\t.long _NXArgv
\t.long _environ
\t.long ___progname

\t.globl _NXArgc
\t.zerofill __DATA,__common,_NXArgc,4,2
\t.globl _NXArgv
\t.zerofill __DATA,__common,_NXArgv,4,2
\t.globl _environ
\t.zerofill __DATA,__common,_environ,4,2
\t.globl ___progname
\t.zerofill __DATA,__common,___progname,4,2
\t.subsections_via_symbols
"""


def log(msg):
    print(msg, flush=True)


# ---------------------------------------------------------------- retail side

def retail_debug_map(path):
    """[(so_dir, so_name, oso, mtime)] in link order, from the stabs."""
    img = machocmp.Image(path)
    out, so = [], []
    for _, _, ntype, sect, desc, value, name in img.symbols():
        if ntype == N_SO:
            if name:
                so.append(name)
            else:
                so = []
        elif ntype == N_OSO:
            d, n = (so[0], so[1]) if len(so) >= 2 else ('', so[0] if so else '')
            out.append({'so_dir': d, 'so_name': n, 'oso': name, 'mtime': value})
    return out


def split_member(oso):
    m = re.match(r'^(.*\.a)\((.*)\)$', oso)
    return (m.group(1), m.group(2)) if m else (None, None)


# ---------------------------------------------------------------- our build

class Build:
    def __init__(self, bld):
        self.bld = os.path.abspath(bld)
        path = os.path.join(self.bld, 'link_commands.json')
        if not os.path.exists(path):
            raise SystemExit('%s missing: run `python3 waf retail_linkdb` first' % path)
        # compile_commands.json keeps one entry per source file, so a source compiled by
        # two targets (tier1/interface.cpp in tier1 and vaudio_speex) loses one of them.
        self.compile = {}
        for name in ('compile_commands.json', 'retail_compile_commands.json'):
            p = os.path.join(self.bld, name)
            if not os.path.exists(p):
                continue
            with open(p) as f:
                cc = json.load(f)
            for e in cc:
                args = e['arguments']
                out = next((a[2:] if a != '-o' else args[i + 1]) for i, a in enumerate(args)
                           if a.startswith('-o'))
                self.compile[os.path.normpath(os.path.join(e['directory'], out))] = e
        with open(path) as f:
            self.links = json.load(f)
        self.by_target = dict((e['target'], e) for e in self.links)
        self.by_output = dict((os.path.normpath(e['output']), e) for e in self.links)

    def source_of(self, obj):
        e = self.compile.get(os.path.normpath(obj))
        if not e:
            return None, None
        return os.path.normpath(os.path.join(e['directory'], e['file'])), e


def load_manifest():
    with open(MANIFEST) as f:
        m = json.load(f)
    return m


def module_key(m, target, link=None):
    for key, mod in m['modules'].items():
        if mod.get('waf_target') == target and mod['retail'].get('filetype') in ('MH_DYLIB', 'MH_EXECUTE'):
            return key
    candidates = [target, 'bin/' + target]
    if link:
        base = os.path.basename(link['output'])
        candidates += [base, 'bin/' + base]
    for key in candidates:
        if key in m['modules']:
            return key
    return None


# ---------------------------------------------------------------- compile

class Compiler:
    def __init__(self, scratch, tree, aliases, jobs):
        self.scratch = scratch
        self.tree = tree.rstrip('/') + '/'
        self.aliases = aliases
        import retailflags
        self.sdk2013 = os.path.abspath(retailflags.SDK2013)
        self.thirdparty = os.path.abspath(os.path.join(retailflags.TOOLCHAIN, retailflags.THIRDPARTY_DIR))
        self.jobs = jobs
        self.cache = os.path.join(scratch, 'cache')
        os.makedirs(self.cache, exist_ok=True)

    def rewrite_map(self, cc_args, valve_src, foreign=None):
        cc = cc_args[0]
        resource = os.path.join(os.path.dirname(cc), '..', 'lib', 'clang')
        rmap = []
        if os.path.isdir(resource):
            for v in os.listdir(resource):
                rmap.append((os.path.join(os.path.dirname(cc), '..', 'lib', 'clang', v) + '/', VALVE_RESOURCE))
        sdk = next((cc_args[i + 1] for i, a in enumerate(cc_args) if a == '-isysroot'), None)
        if sdk:
            rmap.append((sdk.rstrip('/') + '/', foreign['sdk'] if foreign else VALVE_SDK))
        if foreign:
            rmap.append((foreign['src'], foreign['valve_path']))
            rmap.append((self.tree + foreign['root'], foreign['headers']))
        for ours, valve in self.aliases:
            rmap.append((self.tree + ours, valve_src + valve))
        # protobuf's headers come from the Source SDK 2013 checkout (Valve's src/thirdparty).
        rmap.append((os.path.join(self.sdk2013, 'src', 'thirdparty') + '/', valve_src + 'thirdparty/'))
        # Crypto++ headers come from thirdparty-osx32-*/include/cryptopp (Valve's src/external/crypto++-5.61).
        rmap.append((os.path.join(self.thirdparty, 'include', 'cryptopp') + '/', valve_src + 'external/crypto++-5.61/'))
        rmap.append((self.tree, valve_src))
        rmap.sort(key=lambda p: -len(p[0]))
        return rmap

    def compile(self, src, entry, valve_src, so_dir, so_name, out, foreign=None):
        """Preprocess, rewrite paths, compile the .ii as ccache would. Returns cache path."""
        d = entry['directory']
        args = list(entry['arguments'])
        pre, comp, i = [], [], 0
        while i < len(args):
            a = args[i]
            nxt = args[i + 1] if i + 1 < len(args) else None
            if a == '-o':
                i += 2
                continue
            if a.startswith('-o') or a == '-c' or a in DEP_FLAGS:
                i += 1
                continue
            if a in ('-MF', '-MT', '-MQ'):
                i += 2
                continue
            if os.path.normpath(os.path.join(d, a)) == src:
                pre.append(src)
                i += 1
                continue
            hit = next((p for p in PREPROCESSOR_ONLY if a == p or a.startswith(p)), None)
            if hit:
                val = a[len(hit):] if a != hit else nxt
                if hit in ('-I', '-isystem', '-iquote', '-include', '-imacros') and val and not os.path.isabs(val):
                    p = os.path.normpath(os.path.join(d, val))
                    # -include alloca.h (vphysics) names a header on the search path
                    if hit in ('-I', '-isystem', '-iquote') or os.path.exists(p):
                        val = p
                elif hit in ('-I', '-isystem', '-iquote') and val:
                    val = os.path.normpath(val)
                if hit in ('-I', '-D', '-U'):
                    pre.append(hit + val)
                else:
                    pre += [hit, val]
                i += 1 if a != hit else 2
                continue
            pre.append(a)
            comp.append(a)
            i += 1
        rmap = self.rewrite_map(args, valve_src, foreign)
        raw = subprocess.run(pre + ['-E', '-o', '-'], cwd=d, stdout=subprocess.PIPE,
                             stderr=subprocess.PIPE)
        if raw.returncode:
            raise RuntimeError('preprocess %s failed:\n%s' % (src, raw.stderr.decode(errors='replace')))
        bmap = [(o.encode(), v.encode()) for o, v in rmap]

        def marker(m):
            path = m.group(2)
            for ours, valve in bmap:
                if path.startswith(ours):
                    return m.group(1) + valve + path[len(ours):] + m.group(3)
            return m.group(0)
        text = LINE_MARKER.sub(marker, raw.stdout)
        leftover = next((o for o, _ in bmap if o in text), None)
        if leftover:
            raise RuntimeError('preprocess %s: %s outside line markers (__FILE__ without -fmacro-prefix-map?)'
                               % (src, leftover.decode()))
        comp_dir = so_dir.rstrip('/') or '/'
        key = hashlib.sha1(text + repr((comp, so_dir, so_name)).encode()).hexdigest()
        cached = os.path.join(self.cache, key + '.o')
        if not os.path.exists(cached):
            tmpdir = os.path.join(self.scratch, 'ccache-tmp', key[:8])
            os.makedirs(tmpdir, exist_ok=True)
            ii = os.path.join(tmpdir, so_name)
            with open(ii, 'wb') as f:
                f.write(text)
            cmd = comp + [so_name, '-c', '-Xclang', '-fdebug-compilation-dir', '-Xclang', comp_dir,
                          '-o', cached + '.tmp']
            r = subprocess.run(cmd, cwd=tmpdir, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
            if r.returncode:
                raise RuntimeError('compile %s failed:\n%s' % (ii, r.stdout.decode(errors='replace')))
            os.rename(cached + '.tmp', cached)
            shutil.rmtree(tmpdir, ignore_errors=True)
        return cached

    def compile_cryptopp(self, src_name, so_dir):
        """Compile a Crypto++ 5.6.1 member with period gcc 4.2.1 and rewrite DWARF paths."""
        import retailflags
        tc = retailflags.TOOLCHAIN
        gxx = os.path.join(tc, 'gcc-5666.3', 'bin', 'g++')
        sdk105 = os.path.join(tc, 'sdk', 'MacOSX10.5.sdk')
        src_dir = os.path.join(tc, 'work', 'thirdparty', 'cryptopp561')
        src = os.path.join(src_dir, src_name)
        if not os.path.exists(src):
            raise RuntimeError('%s missing; run devtools/bytematch/toolchain/build-thirdparty.py --only cryptopp' % src)
        target_dir = so_dir.rstrip('/')
        key = hashlib.sha1(repr(('cryptopp2', src_name, target_dir, os.path.getmtime(src), CRYPTOPP_FLAGS)).encode()).hexdigest()
        cached = os.path.join(self.cache, key + '.o')
        if not os.path.exists(cached):
            tmpdir = os.path.join(self.scratch, 'ccache-tmp', key[:8])
            os.makedirs(tmpdir, exist_ok=True)
            # gcc 4.2.1 emits DW_AT_comp_dir as inline DW_FORM_string with a hardcoded CU length,
            # so compile from a symlink of the exact byte length of Valve's comp_dir.
            prefix = '/tmp/bm-%d-%s-' % (os.getpid(), key[:8])
            if len(prefix) > len(target_dir):
                raise RuntimeError('target_dir too short for exact-length symlink: %s' % target_dir)
            link_path = prefix + 'x' * (len(target_dir) - len(prefix))
            if os.path.islink(link_path):
                os.unlink(link_path)
            os.symlink(src_dir, link_path)
            try:
                s_path = os.path.join(tmpdir, src_name + '.s')
                env = dict(os.environ, MACOSX_DEPLOYMENT_TARGET='10.5', PWD=link_path)
                r = subprocess.run([gxx, '-isysroot', sdk105] + CRYPTOPP_FLAGS + ['-S', src_name, '-o', s_path],
                                   cwd=link_path, env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
                if r.returncode:
                    raise RuntimeError('compile %s failed:\n%s' % (src, r.stdout.decode(errors='replace')))
                with open(s_path, 'rb') as f:
                    asm = f.read()
                asm = asm.replace((sdk105.rstrip('/') + '/').encode(), b'/Developer/SDKs/MacOSX10.5.sdk/')
                asm = asm.replace(link_path.encode(), target_dir.encode())
                with open(s_path, 'wb') as f:
                    f.write(asm)
                r = subprocess.run([gxx, '-c', s_path, '-o', cached + '.tmp'],
                                   cwd=tmpdir, env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
                if r.returncode:
                    raise RuntimeError('assemble %s failed:\n%s' % (s_path, r.stdout.decode(errors='replace')))
                os.rename(cached + '.tmp', cached)
            finally:
                if os.path.islink(link_path):
                    os.unlink(link_path)
                shutil.rmtree(tmpdir, ignore_errors=True)
        return cached


# ---------------------------------------------------------------- module

def place(cached, dest, mtime):
    os.makedirs(os.path.dirname(dest), exist_ok=True)
    shutil.copyfile(cached, dest)
    os.utime(dest, (mtime, mtime))


def lang_of(src):
    ext = os.path.splitext(src)[1].lower()
    return {'.c': 'c', '.mm': 'objective-c++', '.m': 'objective-c'}.get(ext, 'c++')


def default_so_name(src, lang):
    """ccache 3 temp name: first 10 characters of the stem, host, pid."""
    return os.path.splitext(os.path.basename(src))[0][:10] + '.tmp.identity.0' + LANG_EXT[lang]


class Module:
    def __init__(self, target, build, manifest, comp, depot, scratch, out_dir):
        self.target = target
        self.build = build
        self.comp = comp
        self.link = build.by_target.get(target)
        if not self.link:
            raise SystemExit('%s: no link command in link_commands.json' % target)
        self.key = module_key(manifest, target, self.link)
        if not self.key:
            raise SystemExit('%s: not in %s' % (target, MANIFEST))
        self.mod = manifest['modules'][self.key]
        self.retail = os.path.join(depot, self.mod['retail']['path'])
        self.root = os.path.join(scratch, 'roots', self.key.replace('/', '_'))
        self.out = os.path.join(out_dir, os.path.basename(self.link['output']))
        with open(os.path.join(MODULE_DIR, self.key.replace('/', '.') + '.json')) as f:
            self.objects = json.load(f)['objects']
        ours = self.mod.get('objects', [])
        self.dmap = retail_debug_map(self.retail)
        if not (len(self.dmap) == len(self.objects) == len(ours)):
            raise SystemExit('%s: debug map has %d objects, manifest %d/%d' % (target, len(self.dmap), len(self.objects), len(ours)))
        for o, our, d in zip(self.objects, ours, self.dmap):
            rel = o['oso']
            o.update(d)
            o['source'] = our.get('source')  # path in our tree (retail's may differ, see aliases.json)
            rs = our.get('retail_source')
            if not o['source'] and rs and os.path.exists(os.path.join(TREE, rs)):
                o['source'] = rs  # restored since the manifest was generated
            o['valve_src'] = d['oso'][:-len(rel)] if d['oso'].endswith(rel) and d['oso'] != rel else None
        self.valve_src = next((o['valve_src'] for o in self.objects if o['valve_src'] and o['kind'] == 'object'),
                              next((o['valve_src'] for o in self.objects if o['valve_src']), None))
        self.notes = []

    def vpath(self, p):
        return os.path.join(self.root, p.lstrip('/')) if p.startswith(VALVE_TOP) else None

    def plan(self):
        """Work list: (our object, source, compile entry, retail object or None, Valve path).

        A loose object that retail links as a member of an archive we don't build
        (vstdlib's coroutine.o from libcoroutine_osx.a) goes into that archive instead,
        returned as {Valve archive path: [(object, source, entry, retail member, member name)]}."""
        d = self.link['directory']
        args = self.link['arguments']
        loose = [o for o in self.objects if o['kind'] == 'object']
        by_src = dict((o.get('source'), o) for o in loose if o.get('source'))
        ours_ar = set('lib%s.a' % a[2:] for a in args if a.startswith('-l'))
        mem_src = dict((o.get('source'), o) for o in self.objects if o['kind'] == 'archive_member'
                       and o.get('source') and os.path.basename(o['archive']) not in ours_ar)
        obj_dir = os.path.dirname(loose[0]['oso']) if loose else None
        jobs, arg_map, extra = [], {}, {}
        for a in args:
            if not a.endswith('.o') or a.startswith('-'):
                continue
            path = os.path.normpath(os.path.join(d, a))
            src, entry = self.build.source_of(path)
            if not src:
                self.notes.append('no compile command for %s' % a)
                continue
            rel = os.path.relpath(src, self.comp.tree)
            r = by_src.get(rel)
            if not r and rel in mem_src:
                r = mem_src[rel]
                varchive, mname = split_member(r['oso'])
                extra.setdefault(varchive, []).append((path, src, entry, r, mname))
                arg_map[a] = None
                continue
            if r:
                vp = r['oso']
            else:
                vp = os.path.join(obj_dir or (self.valve_src + 'build'), os.path.splitext(os.path.basename(src))[0] + '.o')
                self.notes.append('%s: not in the retail debug map' % rel)
            jobs.append(('obj', path, src, entry, r, vp))
            arg_map[a] = vp
        return jobs, arg_map, extra

    def archive_plan(self):
        """For each -l that names one of our static libraries: (lib arg, archive entry, members)."""
        d = self.link['directory']
        args = self.link['arguments']
        libdirs = [os.path.normpath(os.path.join(d, a[2:])) for a in args if a.startswith('-L')]
        members = [o for o in self.objects if o['kind'] in ('archive_member', 'prebuilt_member')]
        out = []
        for a in args:
            if not a.startswith('-l'):
                continue
            name = 'lib%s.a' % a[2:]
            ar = next((os.path.join(ld, name) for ld in libdirs if os.path.exists(os.path.join(ld, name))), None)
            if not ar:
                continue
            if os.path.normpath(ar) not in self.build.by_output:
                if name == 'libcryptopp.a':
                    rmem = [o for o in members if o['oso'].split('(')[0].endswith('/' + name)]
                    if rmem:
                        varchive = split_member(rmem[0]['oso'])[0]
                        by_m = dict((split_member(o['oso'])[1], o) for o in rmem)
                        all_m = [m for m in subprocess.check_output(['/usr/bin/ar', 't', ar], text=True).splitlines()
                                 if m.endswith('.o')]
                        mems = []
                        for mn in all_m:
                            r = by_m.get(mn)
                            if r:
                                mems.append((ar, r['so_name'], None, r, mn))
                            else:
                                mems.append((ar, None, None, None, mn))
                        out.append((a, varchive, mems))
                continue
            entry = self.build.by_output[os.path.normpath(ar)]
            rname = re.sub(ARCHIVE_VARIANT, '.a', name)
            rmem = [o for o in members if o['oso'].split('(')[0].endswith('/' + rname)]
            if rmem:
                varchive = split_member(rmem[0]['oso'])[0]
            else:
                varchive = self.valve_src + 'build/Release/' + name
            by_src = dict((o.get('source'), o) for o in rmem if o.get('source'))
            by_member = dict((split_member(o['oso'])[1], o) for o in rmem) if rname in FOREIGN_ARCHIVES else {}
            mems = []
            for inp in entry['inputs']:
                src, cent = self.build.source_of(inp)
                if not src:
                    self.notes.append('no compile command for %s' % inp)
                    continue
                rel = os.path.relpath(src, self.comp.tree)
                r = by_src.get(rel) or by_member.get(os.path.splitext(os.path.basename(src))[0] + '.o')
                mname = split_member(r['oso'])[1] if r else os.path.splitext(os.path.basename(src))[0] + '.o'
                mems.append((inp, src, cent, r, mname))
            out.append((a, varchive, mems))
        return out

    def run(self, shim):
        if os.path.isdir(self.root):
            shutil.rmtree(self.root)
        jobs, arg_map, extra = self.plan()
        archives = self.archive_plan()
        for varchive, mems in extra.items():
            archives.append((varchive, varchive, mems))
        work = []
        for kind, path, src, entry, r, vp in jobs:
            work.append((src, entry, r, ('obj', vp)))
        for lib, varchive, mems in archives:
            for inp, src, cent, r, mname in mems:
                if src is not None:
                    work.append((src, cent, r, ('mem', varchive, mname)))

        def do(item):
            src, entry, r, dest = item
            if entry is None:
                return item, self.comp.compile_cryptopp(src, r['so_dir'])
            lang = lang_of(src)
            so_dir = r['so_dir'] if r and r.get('so_dir') else DEFAULT_CCACHE_DIR
            so_name = r['so_name'] if r and r.get('so_name') else default_so_name(src, lang)
            vsrc = (r or {}).get('valve_src') or self.valve_src
            foreign = None
            fa = FOREIGN_ARCHIVES.get(os.path.basename(dest[1])) if dest[0] == 'mem' else None
            if fa and r:
                foreign = {'root': fa[0], 'headers': fa[1], 'sdk': fa[2], 'src': src,
                           'valve_path': so_dir + so_name}
            return item, self.comp.compile(src, entry, vsrc, so_dir, so_name, None, foreign)

        with concurrent.futures.ThreadPoolExecutor(self.comp.jobs) as ex:
            for item, cached in ex.map(do, work):
                src, entry, r, dest = item
                mtime = r['mtime'] if r else 0
                if dest[0] == 'obj':
                    place(cached, self.vpath(dest[1]), mtime)
                else:
                    stage = os.path.join(self.root, '.ar', os.path.basename(dest[1]))
                    place(cached, os.path.join(stage, dest[2]), mtime)
        lib_map = {}
        for lib, varchive, mems in archives:
            stage = os.path.join(self.root, '.ar', os.path.basename(varchive))
            dest = self.vpath(varchive)
            os.makedirs(os.path.dirname(dest), exist_ok=True)
            if os.path.exists(dest):
                os.unlink(dest)
            # Extract any unmodified members of a prebuilt archive (e.g. dead-stripped Crypto++ objects).
            prebuilt_ar = next((m[0] for m in mems if m[1] is None and m[0]), None)
            if prebuilt_ar:
                os.makedirs(stage, exist_ok=True)
                missing = [m[4] for m in mems if m[1] is None]
                subprocess.check_call(['/usr/bin/ar', 'x', prebuilt_ar] + missing, cwd=stage,
                                      stderr=subprocess.DEVNULL)
            names = []
            for m in mems:
                if m[4] not in names:
                    names.append(m[4])
            if prebuilt_ar:
                subprocess.check_call(['/usr/bin/libtool', '-static', '-arch_only', 'i386', '-o', dest] + names,
                                      cwd=stage, stderr=subprocess.DEVNULL)
            else:
                subprocess.check_call(['/usr/bin/ar', 'rcs', dest] + names, cwd=stage,
                                      stderr=subprocess.DEVNULL)
            lib_map[lib] = varchive
        # link; archives of formerly loose objects follow the last object, ahead of the libraries
        cmd = []
        args = self.link['arguments']
        last_obj = max([i for i, a in enumerate(args) if a in arg_map] or [-1])
        for i, a in enumerate(args):
            if a in arg_map:
                if arg_map[a]:
                    cmd.append(arg_map[a])
                if i == last_obj:
                    cmd += [lib_map[v] for v in extra]
            elif a in lib_map:
                cmd.append(lib_map[a])
            elif a.startswith('-o') and len(a) > 2:
                cmd.append('-o' + self.out)
            else:
                cmd.append(a)
        if '-o' in cmd:
            cmd[cmd.index('-o') + 1] = self.out
        os.makedirs(os.path.dirname(self.out), exist_ok=True)
        env = dict(os.environ)
        env['DYLD_INSERT_LIBRARIES'] = shim
        pathmap = '%s=%s/' % (VALVE_TOP, os.path.join(self.root, VALVE_TOP[1:].rstrip('/')))
        if self.mod['retail'].get('filetype') == 'MH_EXECUTE':
            sdk = next((cmd[i + 1] for i, a in enumerate(cmd) if a == '-isysroot'), None)
            if sdk:
                crt = build_crt109(self.comp.scratch)
                pathmap += ';%s/usr/lib/crt1.10.6.o=%s' % (sdk.rstrip('/'), crt)
        env['BM_PATHMAP'] = pathmap
        r = subprocess.run(cmd, cwd=self.link['directory'], env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        if r.returncode:
            raise RuntimeError('link failed:\n%s\n%s' % (' '.join(shlex.quote(c) for c in cmd), r.stdout.decode(errors='replace')))
        with open(os.path.join(os.path.dirname(self.out), os.path.basename(self.out) + '.link.txt'), 'w') as f:
            f.write(' '.join(shlex.quote(c) for c in cmd) + '\n')
        return self.out


def build_shim(scratch):
    srcs = [os.path.join(HERE, f) for f in ('buildhost.c', 'qsort109.c')]
    out = os.path.join(scratch, 'buildhost.dylib')
    if not os.path.exists(out) or os.path.getmtime(out) < max(os.path.getmtime(s) for s in srcs):
        subprocess.check_call(['xcrun', 'clang', '-O2', '-dynamiclib', '-o', out] + srcs)
    return out


def build_crt109(scratch):
    import retailflags
    out = os.path.join(scratch, 'crt1.10.6.o')
    asm = os.path.join(scratch, 'crt1.10.6.s')
    if os.path.exists(out) and os.path.exists(asm):
        with open(asm) as f:
            if f.read() == CRT1_10_6_ASM:
                return out
    with open(asm, 'w') as f:
        f.write(CRT1_10_6_ASM)
    cc = retailflags.paths()['cc']
    subprocess.check_call([cc, '-arch', 'i386', '-c', asm, '-o', out])
    return out


def load_aliases():
    if not os.path.exists(ALIASES):
        return []
    with open(ALIASES) as f:
        data = json.load(f)
    return [(a['ours'], a['valve']) for a in data['aliases']]


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('targets', nargs='*', help='waf target names (inputsystem, tier0, vgui2, ...)')
    ap.add_argument('--all', action='store_true', help='every dylib in link_commands.json')
    ap.add_argument('--identical', action='store_true',
                    help='the modules in identical.json; exit 1 unless every output equals retail (cmp)')
    ap.add_argument('--build-dir', default=os.path.join(TREE, 'build-retail'))
    ap.add_argument('--scratch', default=None, help='scratch tree [default: <build-dir>/identity]')
    ap.add_argument('--depot', default=None, help='retail depot 223 build root')
    ap.add_argument('--no-aliases', action='store_true', help='ignore aliases.json')
    ap.add_argument('-j', '--jobs', type=int, default=os.cpu_count() or 4)
    ap.add_argument('-s', '--symbols', action='store_true', help='show the symbol-table diff')
    args = ap.parse_args()

    import retailflags
    depot = args.depot or retailflags.RETAIL_DEPOT
    build = Build(args.build_dir)
    scratch = os.path.abspath(args.scratch or os.path.join(build.bld, 'identity'))
    os.makedirs(scratch, exist_ok=True)
    manifest = load_manifest()
    targets = args.targets
    expect = {}
    if args.all:
        targets = [e['target'] for e in build.links if e['kind'].endswith('shlib') or e['kind'].endswith('program')]
    if args.identical:
        with open(IDENTICAL) as f:
            expect = dict((e['target'], e) for e in json.load(f)['modules'])
        targets = list(expect)
    if not targets:
        ap.error('no targets')
    shim = build_shim(scratch)
    comp = Compiler(scratch, TREE, [] if args.no_aliases else load_aliases(), args.jobs)
    rc = 0
    summary = []
    for t in targets:
        m = None
        try:
            m = Module(t, build, manifest, comp, depot, scratch, os.path.join(scratch, 'out'))
            out = m.run(shim)
        except (RuntimeError, SystemExit) as e:
            for n in (m.notes if m else []):
                log('%s: note: %s' % (t, n))
            log('%s: FAILED: %s' % (t, e))
            summary.append((t, 'failed', str(e).splitlines()[0]))
            rc = 1
            continue
        for n in m.notes:
            log('%s: note: %s' % (t, n))
        a, b = machocmp.Image(out), machocmp.Image(m.retail)
        cargs = argparse.Namespace(symbols=args.symbols, verbose=False, max=60)
        status, lines = machocmp.compare(a, b, cargs)
        log('\n'.join(lines))
        if t in expect:
            with open(m.retail, 'rb') as f:
                digest = hashlib.sha256(f.read()).hexdigest()
            if digest != expect[t]['sha256']:
                summary.append((t, 'failed', 'retail file %s is not the recorded one (sha256 %s)' % (m.retail, digest)))
                rc = 1
                continue
            if subprocess.run(['cmp', '-s', out, m.retail]).returncode:
                status = status or 1
        summary.append((t, 'IDENTICAL' if status == 0 else 'differs', out))
        rc |= status
    log('')
    for t, s, info in summary:
        log('%-24s %-10s %s' % (t, s, info))
    return rc


if __name__ == '__main__':
    sys.exit(main())
