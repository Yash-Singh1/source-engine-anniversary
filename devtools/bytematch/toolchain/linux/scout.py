#!/usr/bin/env python3
"""Run a Steam Runtime scout SDK tool on host paths, the way Valve's Linux build ran it.

usage: scout.py --tree DIR --build DIR [--mount HOST=CONTAINER ...] TOOL ARGS...
       scout.py --tree DIR --build DIR --start | --stop | --name

The tree is mounted at /data/src (retail's __FILE__ paths are /data/src/...) and
the build directory at /data/build, in one long-lived container per tree
and build directory (`docker run` costs ~20 s on a loaded host, `docker exec`
~1 s). Arguments
naming files under a mount are rewritten to container paths. For a compile,
`--bm-proj=DIR` (tree-relative project directory, stripped here) makes the
container cwd /data/src/DIR and spells the source as Valve's VPC makefiles did:
$(PWD)/file.cpp for files under the project directory, else
$(PWD)/<path to the tree root>/<tree path> (/data/src/game/server/../../game/shared/x.cpp).
Include directories become normalised absolute paths, as the makefile's
$(abspath $(INCLUDEDIRS)).
"""
import hashlib, os, subprocess, sys

# registry.gitlab.steamos.cloud/steamrt/scout/sdk:0.20221019.0 (gcc-9 9.3.0-17+steamrt1.3+srt1,
# binutils 2.30 behind the g++-9 wrapper, system binutils 2.22).
IMAGE = 'registry.gitlab.steamos.cloud/steamrt/scout/sdk@sha256:005e25dee9751524db33233550a17157af9f0ea1680ff52c26fb5b4b2fd799fb'
TREE, BUILD = '/data/src', '/data/build'
SOURCE_EXT = ('.c', '.cc', '.cpp', '.cxx')
PATH_PREFIXES = ('-I', '-L', '-o', '-MF', '-include', '')


def container_name(mounts):
    key = ';'.join('%s=%s' % (os.path.realpath(h), c) for h, c in mounts)
    return 'bm-scout-' + hashlib.sha1(key.encode()).hexdigest()[:10]


class Mapper:
    def __init__(self, mounts):
        # longest host prefix first
        self.mounts = sorted(((os.path.realpath(h), c) for h, c in mounts), key=lambda m: -len(m[0]))

    def to_container(self, path, cwd):
        p = os.path.normpath(os.path.join(cwd, path))
        rp = os.path.realpath(p)
        for host, cont in self.mounts:
            for cand in (p, rp):
                if cand == host or cand.startswith(host + os.sep):
                    return cont + cand[len(host):]
        return None

    def to_host(self, path):
        for host, cont in self.mounts:
            if path == cont or path.startswith(cont + '/'):
                return host + path[len(cont):]
        return None


def looks_like_path(s):
    return '/' in s or os.path.splitext(s)[1] in SOURCE_EXT + ('.o', '.a', '.so', '.txt')


def translate(args, mapper, cwd, proj):
    out, source = [], None
    it = iter(args)
    for a in it:
        if a in ('-o', '-MF', '-I', '-L', '-include', '-isystem') :
            nxt = next(it, None)
            out += [a, rewrite(nxt, mapper, cwd) if nxt is not None else nxt]
            continue
        if a.startswith('-Wl,'):
            parts = []
            for x in a[4:].split(','):
                k, eq, v = x.rpartition('=') if x.startswith('--') else ('', '', x)
                parts.append(k + eq + (rewrite(v, mapper, cwd) if looks_like_path(v) else v))
            out.append('-Wl,' + ','.join(parts))
            continue
        if not a.startswith('-') and a.endswith(SOURCE_EXT) and proj is not None:
            c = mapper.to_container(a, cwd)
            if c and c.startswith(TREE + '/'):
                source = c
                out.append(vpc_spelling(c[len(TREE) + 1:], proj))
                continue
        hit = False
        for pre in PATH_PREFIXES:
            if pre and not a.startswith(pre):
                continue
            rest = a[len(pre):]
            if not rest or (not pre and (rest.startswith('-') or not looks_like_path(rest))):
                continue
            c = mapper.to_container(rest, cwd)
            if c is not None:
                out.append(pre + c)
                hit = True
            break
        if not hit:
            out.append(a)
    return out, source


def rewrite(path, mapper, cwd):
    c = mapper.to_container(path, cwd)
    return c if c is not None else path


def vpc_spelling(rel, proj):
    proj = proj.strip('/')
    if rel.startswith(proj + '/'):
        return '%s/%s/%s' % (TREE, proj, rel[len(proj) + 1:])
    up = '/'.join(['..'] * len(proj.split('/')))
    return '%s/%s/%s/%s' % (TREE, proj, up, rel)


def docker(*a, **kw):
    return subprocess.call(['docker'] + list(a), **kw)


def start(name, mounts):
    st = subprocess.run(['docker', 'inspect', '-f', '{{.State.Running}} {{.Config.Image}}', name],
                        stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, universal_newlines=True)
    if st.returncode == 0:
        running, image = st.stdout.split()
        if running == 'true' and image == IMAGE:
            return 0
        docker('rm', '-f', name, stdout=subprocess.DEVNULL)
    cmd = ['run', '-d', '--name', name, '--platform', 'linux/amd64']
    for h, c in mounts:
        cmd += ['-v', '%s:%s' % (os.path.realpath(h), c)]
    return docker(*(cmd + [IMAGE, 'sleep', 'infinity']), stdout=subprocess.DEVNULL)


def main(argv):
    tree = build = None
    mounts, rest = [], []
    i = 0
    while i < len(argv):
        a = argv[i]
        if a in ('--tree', '--build', '--mount'):
            v = argv[i + 1]
            i += 2
            if a == '--tree':
                tree = v
            elif a == '--build':
                build = v
            else:
                h, c = v.split('=', 1)
                mounts.append((h, c))
            continue
        rest = argv[i:]
        break
    if not tree or not build or not rest:
        sys.exit(__doc__)
    mounts = [(tree, TREE), (build, BUILD)] + mounts
    name = container_name(mounts)
    if rest[0] == '--name':
        print(name)
        return 0
    if rest[0] == '--start':
        os.makedirs(build, exist_ok=True)
        return start(name, mounts)
    if rest[0] == '--stop':
        return docker('rm', '-f', name, stdout=subprocess.DEVNULL)

    tool, args = rest[0], rest[1:]
    proj = next((a.split('=', 1)[1] for a in args if a.startswith('--bm-proj=')), None)
    args = [a for a in args if not a.startswith('--bm-proj=')]
    mapper = Mapper(mounts)
    cwd = os.getcwd()
    targs, source = translate(args, mapper, cwd, proj)
    if source is not None:
        wd = '%s/%s' % (TREE, proj.strip('/'))
    else:
        wd = mapper.to_container('.', cwd) or BUILD
    cmd = ['docker', 'exec', '-i', '-w', wd, name, tool] + targs
    if os.environ.get('BM_SCOUT_VERBOSE'):
        print(' '.join(cmd), file=sys.stderr)
    for attempt in range(3):
        p = subprocess.run(cmd, stdin=sys.stdin if not sys.stdin.isatty() else subprocess.DEVNULL)
        # docker exec itself failing (daemon busy) exits 125/126 before the tool runs
        if p.returncode not in (125, 126) or start(name, mounts):
            break
    return p.returncode


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
