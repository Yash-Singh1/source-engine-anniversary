#!/usr/bin/env python3
"""Generate devtools/bytematch/manifests/retail-linux32.json.

--retail-linux32 takes each module's file set from the macOS manifest
(retail-osx32.json: retail's own object lists, which the game lanes aligned
with retail) and this file supplies what differs on Linux, per macOS manifest
module or archive:

- defines: the module's VPC (this tree's, as the macOS manifest's) evaluated
  for POSIX/LINUX/LINUX32/LINUXALL/SDL Release (no GL: retail Linux uses
  DXVK, not ToGL), then what Valve's VPC makefile
  generator and makefile_base_posix.mak append (seen in the Source SDK 2013
  linux32 makefiles);
- add/remove: files Valve's 2018 VPCs (hl2_src) compile for Linux but not
  macOS, and the reverse.

usage: make-manifest.py [--hl2src DIR] [--out FILE]
"""
import argparse, json, os, re, sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, '..', '..', '..', '..'))
sys.path.insert(0, os.path.join(HERE, '..', 'win32'))
import vpc as VPC

OSX_MANIFEST = os.path.join(REPO, 'devtools', 'bytematch', 'manifests', 'retail-osx32.json')
OUT = os.path.join(REPO, 'devtools', 'bytematch', 'manifests', 'retail-linux32.json')
HL2SRC = os.environ.get('HL2SRC', os.path.expanduser('~/p/sco/source-engine-2018-hl2_src'))

# Retail Linux renders through DXVK (depot 224 ships libdxvk_d3d9.so and shaderapivk.so), so
# Valve ran VPC without /gl: no $GL, hence no DX_TO_GL_ABSTRACTION or GL_GLEXT_PROTOTYPES
# (source_video_base.vpc). imesh.h's OPENGL_SWAP_COLORS follows DX_TO_GL_ABSTRACTION and
# shows in every vertex color the client writes. macOS still uses ToGL.
LINUX = ('POSIX', 'LINUX', 'LINUX32', 'LINUXALL', 'SDL')
OSX = ('POSIX', 'OSX', 'OSX32', 'OSXALL', 'GL', 'SDL')

# Game modules --retail-linux32 builds; their archives come along.
MODULES = ['episodic/server', 'episodic/client', 'lostcoast/server', 'lostcoast/client']

# Engine-side modules (depot 224 bin/, May 2025 build), by macOS manifest key. Not here:
# shaderapivk (DXVK; no source), chromehtml, vaudio_miles, video_bink/video_webm (deferred,
# scope.md), sourcevr (an older SteamRT gcc-4.8 build), video_quicktime (macOS only).
ENGINE_MODULES = ['bin/libtier0', 'bin/libvstdlib', 'bin/engine', 'bin/filesystem_stdio',
                  'bin/materialsystem', 'bin/shaderapidx9', 'bin/shaderapiempty', 'bin/stdshader_dx9',
                  'bin/libtogl', 'bin/studiorender', 'bin/datacache', 'bin/vphysics', 'bin/inputsystem',
                  'bin/soundemittersystem', 'bin/scenefilecache', 'bin/vgui2', 'bin/vguimatsurface',
                  'bin/GameUI', 'bin/ServerBrowser', 'bin/replay', 'bin/launcher', 'hl2_osx',
                  'bin/video_services', 'bin/vaudio_speex', 'bin/vtex_dll', 'bin/bsppack',
                  'bin/bugreporter_filequeue', 'bin/bugreporter_public']

# Modules built with $GL on Linux: libtogl and those linking it (retail NEEDED), and the engine,
# whose SDL window and video-mode code compiles on Linux only with DX_TO_GL_ABSTRACTION in this
# tree. The others have no DX_TO_GL_ABSTRACTION (see NOT_LINUX).
GL_MODULES = {'bin/libtogl', 'bin/shaderapidx9', 'bin/launcher', 'bin/engine'}

# Archive members to leave out on Linux, beyond the VPC differences. mathlib/vector.cpp isn't
# among retail's libmathlib.a members (retail-osx32.json) and defines vec3_origin again (and a
# global __x86.get_pc_thunk.ax), so ld pulled it next to mathlib_base.o into vguimatsurface.
LINUX_REMOVE = {'libmathlib.a': ['mathlib/vector.cpp']}

# Files hl2_src's Linux VPCs add that the retail module lacks: GameUI has no Crypto++ code
# (filesystem_stdio has 556 CryptoPP strings), materialsystem and stdshader_dx9 no texture
# compositor (no r_texcomp_* convars).
NOT_IN_RETAIL = {'bin/GameUI': ['common/crypto.cpp'],
                 'bin/materialsystem': ['materialsystem/ctexturecompositor.cpp'],
                 'bin/stdshader_dx9': ['materialsystem/stdshaders/compositor.cpp']}

# hl2_src names some projects' VPCs differently from this tree.
HL2SRC_VPC = {'game/server/server_hl2.vpc': 'game/server/server_lostcoast.vpc',
              'game/client/client_hl2.vpc': 'game/client/client_lostcoast.vpc'}


def generator_defines(project_dir):
    # VPC's makefile generator, then makefile_base_posix.mak's DEFINES +=
    return ['_EXTERNAL_DLL_EXT=.so', 'VPCGAMECAPS=VALVE', 'PROJECTDIR=/data/src/' + project_dir,
            '_DLL_EXT=.so', 'SOURCE1=1', 'VPCGAME=valve', '_LINUX=1', '_POSIX=1', 'LINUX=1', 'POSIX=1',
            'VPROF_LEVEL=1', 'GNUC', 'NO_HOOK_MALLOC', 'NO_MALLOC_OVERRIDE']


def evaluate(root, path, conds, ext):
    return VPC.Project(root, os.path.join(root, path), conds, 'Release',
                       macros={'_DLL_EXT': ext, '_STATICLIB_EXT': '.a'}).parse().summary()


def sources(summary):
    return set(f['path'] for f in summary['files']
               if f['kind'] == '$file' and not f.get('excluded') and f['path'].endswith(('.c', '.cc', '.cpp', '.mm')))


# source_video_base.vpc forces $Conditional GL on for every non-Windows client build;
# retail Linux game code has no OPENGL_SWAP_COLORS (vertex colors unswapped, e.g. tier2
# RenderLine), so these come off.
NOT_LINUX = {'GL_GLEXT_PROTOTYPES', 'DX_TO_GL_ABSTRACTION'}


def entry(vpc_path, hl2src, gl=False):
    conds = LINUX + (('GL',) if gl else ())
    s = evaluate(REPO, vpc_path, conds, '.so')
    defines = []
    for d in s['settings'].get('compiler.preprocessordefinitions', '').split(';'):
        if d and d not in defines and (gl or d not in NOT_LINUX):
            defines.append(d)
    project_dir = os.path.dirname(vpc_path)
    out = {'vpc': vpc_path, 'project_dir': project_dir, 'defines': defines + generator_defines(project_dir)}
    # $GCC_ExtraCompilerFlags isn't applied: the only Linux one here, vstdlib's -U_FORTIFY_SOURCE,
    # isn't in retail (libvstdlib.so calls __longjmp_chk and __vsnprintf_chk)
    if gl:
        out['gl'] = True
    hp = HL2SRC_VPC.get(vpc_path, vpc_path)
    if hl2src and os.path.exists(os.path.join(hl2src, hp)):
        lin = sources(evaluate(hl2src, hp, conds, '.so'))
        osx = sources(evaluate(hl2src, hp, OSX, '.dylib'))
        out['add'] = sorted(lin - osx)
        out['remove'] = sorted(osx - lin)
    return out


def aliases(osx):
    '''retail tree path -> our tree path, from the macOS manifest's object lists.'''
    out = {}
    objs = [o for m in osx['modules'].values() for o in m['objects']]
    objs += [o for a in osx['archives'].values() for o in a['members']]
    for o in objs:
        if o.get('source') and o.get('retail_source') and o['source'] != o['retail_source']:
            out[o['retail_source'].lower()] = o['source']
    return out


def engine_files(key, m, out, hl2src, alias):
    '''GNU ld links every object of an engine module, where ld64 dead-stripped whole objects
    from the macOS module: add the files hl2_src's Linux VPC compiles that the macOS object
    list lacks, in our tree's paths. report-linux.py shows which of them retail has.'''
    hp = HL2SRC_VPC.get(out['vpc'], out['vpc'])
    if not hl2src or not os.path.exists(os.path.join(hl2src, hp)):
        return
    conds = LINUX + (('GL',) if out.get('gl') else ())

    def ours(p):
        if p.lower() in alias:
            return alias[p.lower()]
        # this tree flattened engine/audio/private
        return p.replace('engine/audio/private/', 'engine/audio/')

    # vpc.py keeps one condition per logical line; a $File continuation ("a" [$WINDOWS] \ "b" ...)
    # has one per file
    proj = VPC.Project(hl2src, os.path.join(hl2src, hp), conds, 'Release', macros={'_DLL_EXT': '.so'})
    text = open(os.path.join(hl2src, hp), errors='replace').read()
    file_cond = {}
    for fm in re.finditer(r'"([^"\n]+\.(?:cpp|cc|c|mm))"[ \t]*\[([^\]\n]+)\]', text):
        rel = os.path.normpath(os.path.join(os.path.dirname(hp), fm.group(1).replace('\\', '/')))
        file_cond[rel.lower()] = fm.group(2)
    keep = lambda p: proj.cond(file_cond[p.lower()]) if p.lower() in file_cond else True
    lin = set(ours(p) for p in sources(evaluate(hl2src, hp, conds, '.so')) if keep(p))
    osx = set(ours(p) for p in sources(evaluate(hl2src, hp, OSX, '.dylib')))
    have = set((o['source'] or ours(o['retail_source'] or '')).lower() for o in m['objects'] if not o['archive'])
    add = sorted(p for p in lin if p.lower() not in have and p not in NOT_IN_RETAIL.get(key, []))
    out['add'] = [p for p in add if os.path.exists(os.path.join(REPO, p))]
    missing = [p for p in add if p not in out['add']]
    if missing:
        out['missing'] = missing
    remove = set(osx - lin)
    # macOS links the older togl/osx generation; Linux builds togl/linuxwin
    remove |= set(o['source'] for o in m['objects'] if (o['source'] or '').startswith('togl/osx/'))
    out['remove'] = sorted(remove)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--hl2src', default=HL2SRC)
    ap.add_argument('--out', default=OUT)
    args = ap.parse_args()
    if not os.path.isdir(args.hl2src):
        sys.exit('hl2_src not found at %s (--hl2src)' % args.hl2src)
    osx = json.load(open(OSX_MANIFEST))
    alias = aliases(osx)
    mods, archives = {}, {}
    for key in MODULES + ENGINE_MODULES:
        m = osx['modules'][key]
        mods[key] = entry(m['defines']['vpc_files'][0].split(':', 1)[1], args.hl2src, key in GL_MODULES)
        if key in ENGINE_MODULES:
            engine_files(key, m, mods[key], args.hl2src, alias)
        for a in m['archives']:
            name = a['path'].split('/')[-1]
            if name in archives:
                continue
            files = (osx['archives'][name].get('defines') or {}).get('vpc_files')
            if files:
                archives[name] = entry(files[0].split(':', 1)[1], args.hl2src)
                if name in LINUX_REMOVE:
                    archives[name]['remove'] = sorted(set(archives[name].get('remove', []) + LINUX_REMOVE[name]))
    doc = {
        'schema': 1,
        'generated_by': 'devtools/bytematch/toolchain/linux/make-manifest.py',
        'notes': __doc__.split('\n\n')[1].replace('\n', ' '),
        'modules': mods,
        'archives': archives,
    }
    with open(args.out, 'w') as f:
        json.dump(doc, f, indent=1, sort_keys=True)
        f.write('\n')
    print('%s: %d modules, %d archives' % (args.out, len(mods), len(archives)))


if __name__ == '__main__':
    main()
