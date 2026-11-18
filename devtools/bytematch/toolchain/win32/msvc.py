#!/usr/bin/env python3
"""Run an MSVC 2013 Update 5 tool under wine with POSIX paths translated.

usage: msvc.py TOOL [ARGS...]      TOOL: cl link lib ml rc mc cvtres dumpbin undname
       msvc.py --start | --stop    keep a wineserver running / shut it down

Absolute POSIX paths in arguments (bare, or attached to an option such as
/Fo/abs/x.obj or /LIBPATH:/abs) become Z:\\ paths; wine maps the working
directory itself. INCLUDE and LIB are always the toolchain's VC and 7.1A SDK
directories, never the caller's. Command lines longer than Windows allows go through a response
file. The tools run on Microsoft's own msvcr120/msvcp120.
"""
import os, re, subprocess, sys, tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import versions as V

TOOLS = {
    "cl": "VC/bin/cl.exe", "link": "VC/bin/link.exe", "lib": "VC/bin/lib.exe",
    "ml": "VC/bin/ml.exe", "cvtres": "VC/bin/cvtres.exe", "dumpbin": "VC/bin/dumpbin.exe",
    "undname": "VC/bin/undname.exe", "rc": "sdk71a/Bin/RC.Exe", "mc": "sdk71a/Bin/MC.Exe",
}

MAX_CMDLINE = 30000


def winpath(p):
    return "Z:" + p.replace("/", "\\")


def is_host_path(p):
    if not p.startswith("/") or len(p) < 3 or "/" not in p[1:]:
        return False
    first = p[1:].split("/", 1)[0]
    return os.path.exists("/" + first)


def translate(arg):
    if is_host_path(arg):
        return winpath(arg)
    m = re.match(r"^([/-][A-Za-z_?]+:?)(/.+)$", arg)
    if m and is_host_path(m.group(2)):
        return m.group(1) + winpath(m.group(2))
    if arg.startswith("@") and is_host_path(arg[1:]):
        return "@" + winpath(arg[1:])
    return arg


def quote(a):
    return '"%s"' % a if (" " in a or "\t" in a) and not a.startswith('"') else a


def env():
    e = dict(os.environ)
    e.update(WINEPREFIX=V.PREFIX, WINEDEBUG="-all", MVK_CONFIG_LOG_LEVEL="0",
             WINEDLLOVERRIDES="msvcr120,msvcp120=n;msxml6=n,b;mscoree,mshtml=;winemenubuilder.exe=d")
    vs = winpath(V.VS)
    e["INCLUDE"] = "%s\\VC\\include;%s\\sdk71a\\Include" % (vs, vs)
    e["LIB"] = "%s\\VC\\lib;%s\\sdk71a\\Lib" % (vs, vs)
    # MSVC reads these as extra options; never inherit them from the host.
    for k in ("CL", "_CL_", "LINK", "_LINK_"):
        e.pop(k, None)
    return e


def wine(name):
    return os.path.join(V.WINE_BIN, name)


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    if sys.argv[1] == "--start":
        return subprocess.call([wine("wineserver"), "-p"], env=env())
    if sys.argv[1] == "--stop":
        return subprocess.call([wine("wineserver"), "-k"], env=env())
    tool = TOOLS.get(sys.argv[1].lower())
    if not tool:
        sys.exit("unknown tool %s" % sys.argv[1])
    exe = os.path.join(V.VS, tool)
    if not os.path.exists(exe):
        sys.exit("%s missing; run devtools/bytematch/toolchain/win32/fetch-msvc2013.py" % exe)
    args = [translate(a) for a in sys.argv[2:]]
    rsp = None
    if sum(len(a) + 3 for a in args) > MAX_CMDLINE:
        fd, rsp = tempfile.mkstemp(suffix=".rsp", prefix="msvc-")
        with os.fdopen(fd, "w") as f:
            f.write("\n".join(quote(a) for a in args) + "\n")
        args = ["@" + winpath(rsp)]
    try:
        # A file, not a pipe: wine services started by this run inherit stderr and outlive it.
        with tempfile.TemporaryFile() as errf:
            rc = subprocess.call([wine("wine"), exe] + args, env=env(), stderr=errf)
            errf.seek(0)
            err = errf.read().decode(errors="replace")
        noise = ("Wine cannot find the FreeType", "use TrueType fonts please install", "or equal to 2.0.5",
                 "http://www.freetype.org")
        sys.stderr.write("".join(l for l in err.splitlines(True) if not l.startswith(noise)))
        return rc
    finally:
        if rsp:
            os.unlink(rsp)


if __name__ == "__main__":
    sys.exit(main())
