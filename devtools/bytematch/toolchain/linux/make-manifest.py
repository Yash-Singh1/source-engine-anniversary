#!/usr/bin/env python3
"""Generate devtools/bytematch/manifests/retail-linux32.json.

--retail-linux32 takes each module's file set from the macOS manifest
(retail-osx32.json: retail's own object lists, which the game lanes aligned
with retail) and this file supplies what differs on Linux, per macOS manifest
module or archive:

- defines: the module's VPC (this tree's, as the macOS manifest's) evaluated
  for POSIX/LINUX/LINUX32/LINUXALL Release, then what Valve's VPC makefile
  generator and makefile_base_posix.mak append (seen in the Source SDK 2013
  linux32 makefiles);
- add/remove: files Valve's 2018 VPCs (hl2_src) compile for Linux but not
  macOS, and the reverse.

usage: make-manifest.py [--hl2src DIR] [--out FILE]
"""
import argparse, json, os, sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, '..', '..', '..', '..'))
sys.path.insert(0, os.path.join(HERE, '..', 'win32'))
import vpc as VPC

OSX_MANIFEST = os.path.join(REPO, 'devtools', 'bytematch', 'manifests', 'retail-osx32.json')
OUT = os.path.join(REPO, 'devtools', 'bytematch', 'manifests', 'retail-linux32.json')
HL2SRC = os.environ.get('HL2SRC', os.path.expanduser('~/p/sco/source-engine-2018-hl2_src'))

LINUX = ('POSIX', 'LINUX', 'LINUX32', 'LINUXALL', 'GL', 'SDL')
OSX = ('POSIX', 'OSX', 'OSX32', 'OSXALL', 'GL', 'SDL')

# Game modules --retail-linux32 builds; their archives come along.
MODULES = ['episodic/server', 'episodic/client', 'lostcoast/server', 'lostcoast/client']

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


def entry(vpc_path, hl2src):
    s = evaluate(REPO, vpc_path, LINUX, '.so')
    defines = []
    for d in s['settings'].get('compiler.preprocessordefinitions', '').split(';'):
        if d and d not in defines:
            defines.append(d)
    project_dir = os.path.dirname(vpc_path)
    out = {'vpc': vpc_path, 'project_dir': project_dir, 'defines': defines + generator_defines(project_dir)}
    hp = HL2SRC_VPC.get(vpc_path, vpc_path)
    if hl2src and os.path.exists(os.path.join(hl2src, hp)):
        lin = sources(evaluate(hl2src, hp, LINUX, '.so'))
        osx = sources(evaluate(hl2src, hp, OSX, '.dylib'))
        out['add'] = sorted(lin - osx)
        out['remove'] = sorted(osx - lin)
    return out


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--hl2src', default=HL2SRC)
    ap.add_argument('--out', default=OUT)
    args = ap.parse_args()
    if not os.path.isdir(args.hl2src):
        sys.exit('hl2_src not found at %s (--hl2src)' % args.hl2src)
    osx = json.load(open(OSX_MANIFEST))
    mods, archives = {}, {}
    for key in MODULES:
        m = osx['modules'][key]
        mods[key] = entry(m['defines']['vpc_files'][0].split(':', 1)[1], args.hl2src)
        for a in m['archives']:
            name = a['path'].split('/')[-1]
            if name in archives:
                continue
            files = (osx['archives'][name].get('defines') or {}).get('vpc_files')
            if files:
                archives[name] = entry(files[0].split(':', 1)[1], args.hl2src)
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
