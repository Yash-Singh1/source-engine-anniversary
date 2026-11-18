#!/usr/bin/env python3
"""Evaluate the tree's wscripts against a mock waf context and list task generators.

Imports each subproject wscript, runs configure() and build() with stand-ins
for the few waf APIs they touch, and records every shlib/stlib/program call
(target, sources relative to the tree root, use, defines). Nothing is compiled.

Usage: waf_sources.py [--root DIR] [--os darwin] [--cpu x86] [--games hl2] [subproject ...]
"""
import argparse, contextlib, importlib.machinery, importlib.util, io, json, os, sys, tempfile

sys.dont_write_bytecode = True

PLATFORM_DEFINES = {
    "darwin": ["OSX=1", "_OSX=1", "POSIX=1", "_POSIX=1", "PLATFORM_POSIX=1", "GNUC",
               "NO_HOOK_MALLOC", "_DLL_EXT=.dylib"],
    "linux": ["LINUX=1", "_LINUX=1", "POSIX=1", "_POSIX=1", "PLATFORM_POSIX=1", "GNUC",
              "NO_HOOK_MALLOC", "_DLL_EXT=.so"],
}


class Env(dict):
    def __getattr__(self, k):
        return self.get(k, [])

    def __setattr__(self, k, v):
        self[k] = v

    def append_unique(self, k, v):
        cur = self.setdefault(k, [])
        for x in v if isinstance(v, list) else [v]:
            if x not in cur:
                cur.append(x)

    def append_value(self, k, v):
        self.setdefault(k, []).extend(v if isinstance(v, list) else [v])


class Options:
    def __init__(self, **kw):
        self.__dict__.update(kw)

    def __getattr__(self, k):
        return False


class Conf:
    def __init__(self, env, options):
        self.env, self.options = env, options

    def define(self, k, v=1, quote=True):
        self.env.append_unique("DEFINES", f"{k}={v}")

    def fatal(self, msg, *a):
        raise RuntimeError(f"conf.fatal: {msg} {a}")

    def __getattr__(self, k):
        return lambda *a, **kw: True


class Bld:
    def __init__(self, env, root, subdir):
        self.env, self.root, self.subdir = env, root, subdir
        self.taskgens = []

    def _tg(self, kind, **kw):
        base = os.path.join(self.root, self.subdir)
        srcs = kw.get("source", [])
        srcs = srcs.split() if isinstance(srcs, str) else srcs
        rel = [os.path.relpath(os.path.normpath(os.path.join(base, s)), self.root) for s in srcs]
        use = kw.get("use", [])
        self.taskgens.append(dict(
            subproject=self.subdir, kind=kind, target=kw.get("target"), name=kw.get("name"),
            features=kw.get("features"), sources=rel,
            use=use.split() if isinstance(use, str) else list(use),
            defines=list(kw.get("defines", [])), linkflags=list(kw.get("linkflags", []))))

    def shlib(self, **kw):
        self._tg("shlib", **kw)

    def stlib(self, **kw):
        self._tg("stlib", **kw)

    def program(self, **kw):
        self._tg("program", **kw)

    def __call__(self, **kw):
        self._tg("generic", **kw)

    def get_taskgen_count(self):
        return 0

    def retail_manifest(self, module, source, use):
        return source, use

    def __getattr__(self, k):
        return lambda *a, **kw: None


def load_wscript(root, subdir):
    path = os.path.join(root, subdir, "wscript")
    name = "wscript_" + subdir.replace("/", "_")
    spec = importlib.util.spec_from_loader(name, importlib.machinery.SourceFileLoader(name, path))
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


def evaluate(root, subdirs, dest_os="darwin", cpu="x86", games="hl2", gl=True, sdl=True):
    ensure_waflib_stub()
    sys.path.insert(0, os.path.join(root, "scripts", "waifulib"))
    out, errors = [], {}
    for sub in subdirs:
        defines = list(PLATFORM_DEFINES.get(dest_os, []))
        defines += ["NDEBUG", "NO_MEMOVERRIDE_NEW_DELETE=1"]
        if gl:
            defines += ["DX_TO_GL_ABSTRACTION", "GL_GLEXT_PROTOTYPES", "BINK_VIDEO"]
        if sdl:
            defines += ["USE_SDL=1"]
        env = Env(DEST_OS=dest_os, DEST_CPU=cpu, DEFINES=defines, GAMES=games, GL=gl, SDL=sdl,
                  DEDICATED=False, LIBDIR="lib", BINDIR="bin", PREFIX="", TESTDIR="tests",
                  SUBPROJECT_PATH=[os.path.join(root, sub)], MSVC_SUBSYSTEM=None, BUILD_SDK=False)
        opts = Options(GAMES=games, DEDICATED=False)
        cwd = os.getcwd()
        try:
            os.chdir(os.path.join(root, sub))
            mod = load_wscript(root, sub)
            with contextlib.redirect_stdout(io.StringIO()):
                if hasattr(mod, "configure"):
                    mod.configure(Conf(env, opts))
                bld = Bld(env, root, sub)
                mod.build(bld)
            out += bld.taskgens
        except Exception as e:
            errors[sub] = f"{type(e).__name__}: {e}"
        finally:
            os.chdir(cwd)
    return out, errors


def all_subprojects(root):
    subs = []
    for d, dirs, files in os.walk(root):
        dirs[:] = [x for x in dirs if not x.startswith(".") and x not in ("thirdparty", "lib")]
        if "wscript" in files and d != root:
            subs.append(os.path.relpath(d, root))
    return sorted(subs)


def ensure_waflib_stub():
    """wscripts import waflib modules at top level but don't call them during build()."""
    try:
        import waflib  # noqa: F401
        return
    except ImportError:
        pass
    base = os.path.join(tempfile.gettempdir(), "bytematch-waflib-stub")
    stub = os.path.join(base, "waflib")
    os.makedirs(stub, exist_ok=True)
    for name in ("__init__", "Utils", "Logs", "Options", "Configure", "Context", "Build", "TaskGen", "Task"):
        open(os.path.join(stub, name + ".py"), "a").close()
    sys.path.insert(0, base)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--root", default=os.path.normpath(os.path.join(os.path.dirname(__file__), "..", "..", "..", "..")))
    ap.add_argument("--os", default="darwin")
    ap.add_argument("--cpu", default="x86")
    ap.add_argument("--games", default="hl2")
    ap.add_argument("subprojects", nargs="*")
    a = ap.parse_args()
    tgs, errors = evaluate(a.root, a.subprojects or all_subprojects(a.root), a.os, a.cpu, a.games)
    json.dump({"taskgens": tgs, "errors": errors}, sys.stdout, indent=1)
    print()


if __name__ == "__main__":
    main()
