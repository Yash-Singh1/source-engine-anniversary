#!/usr/bin/env python
# encoding: utf-8
'''
--retail-linux32: build like Valve's retail Linux i386 game modules (depots 340/380/420).

Compiles and links with the Steam Runtime scout SDK's gcc-9 inside a pinned
container (devtools/bytematch/toolchain/linux/scout.py), the tree mounted at
/data/src, so __FILE__ strings take retail's /data/src/... forms. Flags are
Valve's VPC makefile flow as recovered by the Linux survey
(byte-match-survey/linux-elf.md).

File sets come from the macOS manifest (retail-osx32.json, through
retail_manifest's RETAIL_SELECT): retail Linux and macOS are the same
2024 build of the same projects. devtools/bytematch/manifests/retail-linux32.json
(toolchain/linux/make-manifest.py) supplies the Linux defines and the few
files Valve's VPCs compile only on Linux. Builds the static libraries the
game modules link and game/client + game/server for --build-games
(episodic, hl2 = episodic, lostcoast). libtier0/libvstdlib/libsteam_api are
linked from the retail depot (user-provided, never rebuilt).
'''

import json, os, shutil, subprocess, sys
from waflib import Logs, Task
from waflib.Configure import conf
from waflib.TaskGen import feature, before_method, after_method

MANIFEST = 'devtools/bytematch/manifests/retail-linux32.json'
OSX_MANIFEST = 'devtools/bytematch/manifests/retail-osx32.json'
TOOLCHAIN_DIR = 'devtools/bytematch/toolchain/linux'

LIB_MODULES = ['tier1', 'mathlib', 'tier2', 'tier3', 'particles', 'dmxloader', 'choreoobjects',
	'vgui2/vgui_controls', 'vgui2/matsys_controls', 'bitmap', 'vtf']
GAME_MODULES = ['game/client', 'game/server']
# --retail-linux32-engine: depot 224's engine-side modules with source (make-manifest.py's
# ENGINE_MODULES) and the static libraries only they link.
ENGINE_MODULES = ['tier0', 'vstdlib', 'appframework', 'vpklib', 'utils/bzip2', 'replay/common',
	'materialsystem/shaderlib', 'vgui2/vgui_surfacelib', 'ivp/havana', 'ivp/havana/havok/hk_base',
	'ivp/havana/havok/hk_math', 'ivp/ivp_compact_builder', 'ivp/ivp_physics',
	'scenefilecache', 'soundemittersystem', 'datacache', 'video', 'inputsystem', 'filesystem',
	'vgui2/src', 'studiorender', 'serverbrowser', 'utils/vtex', 'materialsystem/shaderapiempty',
	'engine/voice_codecs/speex', 'launcher_main', 'vphysics', 'materialsystem/stdshaders', 'replay',
	'utils/bsppack', 'utils/bugreporter_filequeue', 'utils/bugreporter_public', 'vguimatsurface',
	'gameui', 'engine', 'togl', 'materialsystem', 'launcher', 'materialsystem/shaderapidx9']
# Valve's projects that compile their C sources as C++ (bzip2.vpc; retail's BZ2_compressBlock
# matches only as g++ output, as on macOS).
COMPILE_C_AS_CXX = ['bzip2']
# Retail Linux file names where they aren't the macOS manifest key's basename + .so.
RETAIL_NAMES = {'hl2_osx': 'hl2_linux'}
# makefile_base_posix.mak's PATHWRAP, in LIB_START_SHLIB/LIB_START_EXE: file APIs go through
# tier1/pathmatch.cpp's __wrap_* (retail libtier0 calls __wrap_fopen64; ServerBrowser imports
# libsteam_api's __wrap___xstat64; client.so imports what pathmatch's __real_* calls resolve to:
# mount, mkfifo, lchown, ...).
PATHWRAP = ['fopen', 'freopen', 'open', 'creat', 'access', '__xstat', 'stat', 'lstat', 'fopen64', 'open64',
	'opendir', '__lxstat', 'chmod', 'chown', 'lchown', 'symlink', 'link', '__lxstat64', 'mknod', 'utimes',
	'unlink', 'rename', 'utime', '__xstat64', 'mount', 'mkfifo', 'mkdir', 'rmdir', 'scandir', 'realpath']

# makefile_base_posix.mak (CFG=release, linux32) with what the survey recovered from retail
# codegen: -march=core2 after pentium4, -fno-finite-math-only, -fno-delete-null-pointer-checks.
ARCH_FLAGS = ['-m32', '-march=pentium4', '-mtune=core2', '-msse2', '-mfpmath=sse', '-march=core2']
OPT_FLAGS = ['-fvisibility=hidden', '-gdwarf-2', '-g2', '-O2', '-fno-strict-aliasing', '-ffast-math',
	'-fno-finite-math-only', '-fno-delete-null-pointer-checks', '-fno-omit-frame-pointer',
	'-ftree-vectorize', '-fpredictive-commoning', '-funswitch-loops', '-pipe']
# Valve's WARN_FLAGS don't change code; -w keeps logs readable. gcc-9 rejects some
# C++11 narrowing in this code that -fpermissive doesn't cover.
WARN_FLAGS = ['-w', '-Wno-narrowing']
UNDEFS = ['-Usprintf', '-Ustrncpy', '-UPROTECTED_THINGS_ENABLE']
CXX_ONLY = ['-std=gnu++0x', '-fpermissive']

# Not Valve's. BYTEMATCH_RETAIL: this fork's runtime additions stay out (shared with
# the macOS and Windows modes); BYTEMATCH_RETAIL_LINUX: Linux-retail-only source.
RETAIL_DEFINES = ['BYTEMATCH_RETAIL', 'BYTEMATCH_RETAIL_LINUX']

# Defines from this tree's wscripts and global env that Valve's build doesn't have.
TREE_ONLY_DEFINES = ['PLATFORM_POSIX', 'NO_MEMOVERRIDE_NEW_DELETE', 'GIT_COMMIT_HASH',
	'DX_TO_GL_ABSTRACTION_DISABLED', 'NO_STEAM', 'DISABLE_STEAM', 'WAF_CFLAGS', 'WAF_LDFLAGS',
	'_OSX', 'OSX', 'OSXALL', '_DARWIN_UNLIMITED_SELECT', 'FD_SETSIZE', 'OVERRIDE_V_DEFINES',
	'QUICKTIME_VIDEO', 'FORCE_QUICKTIME', 'TOGL_OSX', 'CPP11_NO_LIBCXX', 'PNG_NO_PEDANTIC_WARNINGS']

# Retail shared libraries the modules link (depot 224 bin/), and the system libraries
# after them (makefile LIB_END_SHLIB; client adds librt, NEEDED in retail client.so).
DEPOT_LIBS = {'tier0': 'libtier0.so', 'vstdlib': 'libvstdlib.so', 'steam_api': 'libsteam_api.so'}
# VPC $SystemLibraries on Linux (engine's openal and curl-gnutls are gone from retail's NEEDED).
SYSTEM_LIBS = {'client': ['-lrt'], 'engine': ['-lrt'], 'libtier0': ['-lrt'], 'replay': ['-lrt'], 'GameUI': ['-lrt']}
# Runtime libraries from the scout image, named as retail's NEEDED (the image has no
# i386 development symlinks): uselib -> link names.
IMAGE_LIBS = {'SDL2': [':libSDL2-2.0.so.0'], 'FT2': [':libfreetype.so.6'], 'FC': [':libfontconfig.so.1']}
IMAGE_INCLUDES = {'FT2': ['/usr/include/freetype2']}
# SDK 2013 lib/common/linux32 prebuilts (thirdparty-static.md): libcurl 7.21.2 with
# OpenSSL 1.0.1a, statically in engine.so.
SDK2013_COMMON = {'CURL': ['curlssl', 'ssl', 'crypto']}

_manifests = {}

def options(opt):
	grp = opt.add_option_group('Byte-match options')
	grp.add_option('--retail-linux32', action = 'store_true', dest = 'RETAIL_LINUX32', default = False,
		help = 'build the game modules like retail Linux i386 (scout gcc-9 in Docker) [default: %default]')
	grp.add_option('--retail-depot224', action = 'store', dest = 'RETAIL_DEPOT224',
		default = '/Volumes/Expansion/mac-ports/hl2-anniversary-depot/depots/224/19307283',
		help = 'retail depot 224 build root, for libtier0/libvstdlib/libsteam_api [default: %default]')
	grp.add_option('--retail-linux32-hl2src', action = 'store', dest = 'RETAIL_LINUX32_HL2SRC',
		default = os.environ.get('HL2SRC', os.path.expanduser('~/p/sco/source-engine-2018-hl2_src')),
		help = '2018 hl2_src tree, for RAD Telemetry\'s headers (thirdparty/telemetry/include) [default: %default]')
	grp.add_option('--retail-linux32-thirdparty', action = 'store', dest = 'RETAIL_LINUX32_THIRDPARTY',
		default = os.path.expanduser('~/.cache/source-bytematch/toolchain/thirdparty-linux32'),
		help = 'libjpeg/libpng archives from %s/build-thirdparty.sh [default: %%default]' % TOOLCHAIN_DIR)
	grp.add_option('--retail-linux32-modules', action = 'store', dest = 'RETAIL_LINUX32_MODULES', default = None,
		help = 'comma-separated subprojects [default: the game modules\' static libraries, plus game/client and game/server with --build-games]')
	grp.add_option('--retail-linux32-engine', action = 'store_true', dest = 'RETAIL_LINUX32_ENGINE', default = False,
		help = 'also build the engine-side modules of depot 224 [default: %default]')

def load(path):
	if path not in _manifests:
		with open(path) as f:
			_manifests[path] = json.load(f)
	return _manifests[path]

def _name(d):
	return d.split('=')[0]

@conf
def retail_linux32_configure(conf):
	o = conf.options
	tree = conf.path.abspath()
	build = conf.bldnode.abspath()
	scout = [sys.executable, os.path.join(conf.path.find_node(TOOLCHAIN_DIR).abspath(), 'scout.py'),
		'--tree', tree, '--build', build]
	if subprocess.call(scout + ['--start']):
		conf.fatal('retail-linux32: could not start the scout container (see %s/scout.py)' % TOOLCHAIN_DIR)
	conf.environ['CC'] = ' '.join(scout + ['gcc-9'])
	conf.environ['CXX'] = ' '.join(scout + ['g++-9'])
	conf.environ['AR'] = ' '.join(scout + ['ar'])
	conf.load('gcc gxx')
	v = conf.env
	# makefile_base_posix.mak links with $(CC) and names libstdc++ itself (retail_linux32_link)
	v.LINK_CXX = scout + ['gcc-9']
	v.RETAIL_LINUX32 = True
	v.RETAIL_SCOUT = scout
	v.COMPILER_CC = 'gcc'
	v.COMPILER_CXX = 'g++'
	v.DEST_OS = 'linux'
	v.DEST_CPU = 'x86'
	v.DEST_BINFMT = 'elf'
	v.DEST_SIZEOF_VOID_P = 4
	v.SDL = 1
	v.GL = False  # retail Linux renders through DXVK: no DX_TO_GL_ABSTRACTION (make-manifest.py)
	v.CFLAGS = ARCH_FLAGS + WARN_FLAGS + OPT_FLAGS
	v.CXXFLAGS = ARCH_FLAGS + WARN_FLAGS + OPT_FLAGS + CXX_ONLY
	v.CPPFLAGS = list(UNDEFS)
	# default PIE, as the makefile (no -fPIC); same .text as -fPIC with hidden visibility
	v.CFLAGS_cshlib = v.CXXFLAGS_cxxshlib = []
	v.LINKFLAGS = ['-m32', '-Wl,--build-id', '-static-libgcc']
	v.LINKFLAGS_cshlib = v.LINKFLAGS_cxxshlib = ['-shared']
	v.cshlib_PATTERN = v.cxxshlib_PATTERN = '%s.so'
	v.DEFINES = []
	v.INCLUDES = [conf.path.find_node('common').abspath()]
	# objects first, then the static libraries in one group, as LIB_START_SHLIB/LIB_END_SHLIB
	v.STLIB_MARKER = ['-Wl,--start-group', '-Wl,-Bstatic']
	v.SHLIB_MARKER = ['-Wl,--end-group', '-Wl,-Bdynamic']

	libdir = conf.bldnode.make_node('retail-linux32-libs')
	libdir.mkdir()
	for use, name in DEPOT_LIBS.items():
		src = os.path.join(o.RETAIL_DEPOT224, 'bin', name)
		if os.path.exists(src):
			shutil.copy2(src, libdir.make_node(name).abspath())
			v['LIB_' + use] = [':' + name]
			v['LIBPATH_' + use] = [libdir.abspath()]
		else:
			Logs.warn('retail-linux32: %s not found, modules will not link' % src)
	src = os.path.join(o.RETAIL_DEPOT224, 'bin', 'libmimalloc.so')
	if os.path.exists(src):
		shutil.copy2(src, libdir.make_node('libmimalloc.so').abspath())
	v.RETAIL_LIBDIR = libdir.abspath()
	sdk2013 = os.environ.get('SOURCE_SDK_2013', '/Volumes/Storage/sco/source-sdk-2013/src')
	# zlib: every member retail client.so links is byte-identical to SDK 2013's libz.a (survey)
	z = os.path.join(sdk2013, 'lib', 'public', 'linux32', 'libz.a')
	if os.path.exists(z):
		shutil.copy2(z, libdir.make_node('libz.a').abspath())
		v.STLIB_ZLIB = ['z']
		v.STLIBPATH_ZLIB = [libdir.abspath()]
	# htmlprotobuf.cpp (libvgui_controls.a) includes SDK 2013's protobuf 2.3.0 headers,
	# copied under the build dir: the container sees only the tree and the build dir.
	pbsrc = os.path.join(sdk2013, 'thirdparty', 'protobuf-2.3.0', 'src')
	if os.path.isdir(pbsrc):
		tp = os.path.join(conf.bldnode.abspath(), 'retail-linux32-include', 'thirdparty')
		if not os.path.isdir(os.path.join(tp, 'protobuf-2.3.0', 'src')):
			shutil.copytree(pbsrc, os.path.join(tp, 'protobuf-2.3.0', 'src'))
		v.INCLUDES_PROTOBUF = [tp, os.path.join(tp, 'protobuf-2.3.0', 'src')]
		shutil.copy2(os.path.join(sdk2013, 'lib', 'public', 'linux32', 'libprotobuf.a'), libdir.make_node('libprotobuf.a').abspath())
		v.STLIB_PROTOBUF = ['protobuf']
		v.STLIBPATH_PROTOBUF = [libdir.abspath()]
	for use, libs in SDK2013_COMMON.items():
		srcs = [os.path.join(sdk2013, 'lib', 'common', 'linux32', 'lib%s.a' % l) for l in libs]
		if all(os.path.exists(s) for s in srcs):
			for s in srcs:
				shutil.copy2(s, libdir.make_node(os.path.basename(s)).abspath())
			v['STLIB_' + use] = list(libs)
			v['STLIBPATH_' + use] = [libdir.abspath()]
	for use, libs in IMAGE_LIBS.items():
		v['LIB_' + use] = list(libs)
		v['INCLUDES_' + use] = list(IMAGE_INCLUDES.get(use, []))
	# Retail's engine, GameUI, replay and clients link libjpeg 8b and libpng 1.5.2 (old
	# crosstool-NG gcc 4.5.1 prebuilts, which build-thirdparty.sh rebuilds), so their code has
	# HAVE_JPEG/HAVE_PNG; the tree's public/jpeglib and public/libpng are those versions' headers.
	for use, d, lib in (('JPEG', 'public/jpeglib', 'jpeg'), ('PNG', 'public/libpng', 'png')):
		v['INCLUDES_' + use] = [conf.path.find_node(d).abspath()]
		v['DEFINES_' + use] = ['HAVE_' + use]
		a = os.path.join(o.RETAIL_LINUX32_THIRDPARTY, 'lib', 'lib%s.a' % lib)
		if not os.path.exists(a):
			Logs.warn('retail-linux32: %s missing; run %s/build-thirdparty.sh' % (a, TOOLCHAIN_DIR))
			continue
		shutil.copy2(a, libdir.make_node(os.path.basename(a)).abspath())
		v['STLIB_' + use] = [lib]
		v['STLIBPATH_' + use] = [libdir.abspath()]
	inc = os.path.join(conf.bldnode.abspath(), 'retail-linux32-include')
	# Valve's curl 7.21.2 headers (common/curl in hl2_src), as the prebuilt libcurl's
	curl = os.path.join(o.RETAIL_LINUX32_HL2SRC, 'common', 'curl')
	if os.path.isfile(os.path.join(curl, 'curl.h')):
		shutil.rmtree(os.path.join(inc, 'curl'), ignore_errors=True)
		shutil.copytree(curl, os.path.join(inc, 'curl'))
		v.INCLUDES_CURL = [inc]
	# Crypto++ 5.6.1 headers for common/crypto.cpp (filesystem_stdio, vpklib), from the
	# --retail-osx32 third-party build; retail's Linux archive (SteamRT gcc 4.8) isn't available,
	# so nothing is linked.
	cryptopp = os.path.join(os.environ.get('BYTEMATCH_TOOLCHAIN', os.path.expanduser('~/.cache/source-bytematch/toolchain')),
		'thirdparty-osx32-5', 'include', 'cryptopp')
	if os.path.isfile(os.path.join(cryptopp, 'cryptlib.h')):
		shutil.rmtree(os.path.join(inc, 'cryptopp'), ignore_errors=True)
		shutil.copytree(cryptopp, os.path.join(inc, 'cryptopp'))
		v.INCLUDES_CRYPTOPP = [inc]
	sdl = conf.path.find_node('thirdparty/SDL')
	if sdl:
		v.append_value('INCLUDES', [sdl.abspath()])
	# Retail Linux modules have a Telemetry zone at every VPROF (they import g_Telemetry), as
	# vprof_telemetry.h enables for _LINUX. Its "../../thirdparty/telemetry/include/telemetry.h"
	# resolves through an include directory two levels below a copy of hl2_src's headers
	# (inside the build dir, which the container can see).
	tm = os.path.join(o.RETAIL_LINUX32_HL2SRC, 'thirdparty', 'telemetry', 'include')
	if os.path.isfile(os.path.join(tm, 'telemetry.h')):
		root = conf.bldnode.make_node('retail-linux32-telemetry')
		dst = os.path.join(root.abspath(), 'thirdparty', 'telemetry', 'include')
		shutil.rmtree(dst, ignore_errors=True)
		shutil.copytree(tm, dst)
		inc = root.make_node('include/tier0')
		inc.mkdir()
		v.append_value('INCLUDES', [inc.abspath()])
	else:
		Logs.warn('retail-linux32: %s not found; Telemetry zones compiled out (RAD_TELEMETRY_DISABLED)' % tm)
		v.DEFINES = ['RAD_TELEMETRY_DISABLED']
	v.LIBDIR = v.PREFIX + '/bin/'
	v.BINDIR = v.PREFIX

	v.RETAIL_MANIFEST = conf.path.find_node(OSX_MANIFEST).abspath()
	v.RETAIL_MANIFEST_LINUX = conf.path.find_node(MANIFEST).abspath()
	v.RETAIL_SELECT = True
	if o.RETAIL_LINUX32_MODULES is not None:
		v.RETAIL_PROJECTS = [m for m in o.RETAIL_LINUX32_MODULES.split(',') if m]
	else:
		v.RETAIL_PROJECTS = list(LIB_MODULES)
		if o.RETAIL_LINUX32_ENGINE:
			v.RETAIL_PROJECTS += ENGINE_MODULES
		if any(a == '--build-games' or a.startswith('--build-games=') for a in sys.argv):
			v.RETAIL_PROJECTS += GAME_MODULES
	# The define makefile_base_posix.mak (2018) left commented out and the 2025 SDK's
	# source_posix_ninja.vpc sets: retail libtier0/libvstdlib import readdir64 and no readdir,
	# Plat_IsInDebugSession calls fopen64's wrapper, and client.so imports mmap64 (mumble.cpp)
	# and readdir64 (pathmatch.cpp), never mmap or readdir.
	v.RETAIL_LINUX32_DEFINES = ['_FILE_OFFSET_BITS=64']
	Logs.info('retail-linux32: container %s, projects %s' % (subprocess.check_output(scout + ['--name']).decode().strip(),
		','.join(v.RETAIL_PROJECTS)))
	conf.add_subproject(v.RETAIL_PROJECTS)

def _entry(tg):
	'''(osx manifest key, linux manifest entry) for a module or archive task generator.'''
	import retail_manifest
	kind, key, _ = retail_manifest.lookup(tg)
	if not kind:
		return None, None
	m = load(tg.env.RETAIL_MANIFEST_LINUX)
	return key, (m['modules'] if kind == 'module' else m['archives']).get(key)

@feature('c', 'cxx')
@before_method('process_source')
def retail_linux32_own_env(self):
	if self.env.RETAIL_LINUX32:
		self.env = self.env.derive()

@feature('c', 'cxx')
@after_method('retail_select_sources')
@before_method('process_source')
def retail_linux32_sources(self):
	'''Files Valve's VPCs compile only on Linux (or only on macOS).'''
	if not self.env.RETAIL_LINUX32 or getattr(self, 'retail_linux32_selected', False):
		return
	self.retail_linux32_selected = True
	key, e = _entry(self)
	if not e:
		return
	src = self.bld.srcnode
	drop = set(p.lower() for p in e.get('remove', []))
	out = [n for n in self.to_list(self.source)
		if not (hasattr(n, 'abspath') and n.path_from(src).lower() in drop)]
	for p in e.get('add', []):
		n = src.find_resource(p)
		if n is None:
			Logs.warn('%s: Linux retail compiles %s, which is not in this tree' % (key, p))
		elif n not in out:
			out.append(n)
	self.source = out

@feature('c', 'cxx')
@after_method('propagate_uselib_vars')
def retail_linux32_defines(self):
	if not self.env.RETAIL_LINUX32:
		return
	key, e = _entry(self)
	valve = list(e['defines']) if e else []
	if not e:
		Logs.warn('%s: no entry in %s; tree defines only' % (getattr(self, 'name', self), MANIFEST))
	names = set(_name(d) for d in valve)
	ours = [d for d in self.env.DEFINES if _name(d) not in TREE_ONLY_DEFINES and _name(d) not in names]
	defs = []
	for d in valve + RETAIL_DEFINES + list(self.env.RETAIL_LINUX32_DEFINES or []) + ours:
		if d not in defs:
			defs.append(d)
	self.env.DEFINES = defs
	proj = e['project_dir'] if e else self.path.path_from(self.bld.srcnode)
	tag = ['--bm-proj=' + proj]
	self.env.append_value('CFLAGS', tag)
	self.env.append_value('CXXFLAGS', tag)
	per_file = e.get('file_defines', {}) if e else {}
	as_cxx = getattr(self, 'name', None) in COMPILE_C_AS_CXX
	for t in getattr(self, 'compiled_tasks', []):
		extra = per_file.get(t.inputs[0].path_from(self.bld.srcnode).replace(os.sep, '/'))
		if extra:
			t.env = t.env.derive()
		t.env.DEFINES = defs + [d for d in (extra or []) if d not in defs]
		t.env.CFLAGS = self.env.CFLAGS
		t.env.CXXFLAGS = self.env.CXXFLAGS
		if as_cxx and t.__class__.__name__ == 'c':
			t.env.CC = self.env.CXX
			t.env.CFLAGS = list(self.env.CXXFLAGS) + ['-xc++']

@feature('cshlib', 'cxxshlib', 'cprogram', 'cxxprogram')
@before_method('apply_link')
def retail_linux32_name(self):
	'''Retail's file name: the macOS manifest key's basename (bin/libtier0 -> libtier0.so).'''
	if not self.env.RETAIL_LINUX32:
		return
	import retail_manifest
	kind, key, _ = retail_manifest.lookup(self)
	if kind != 'module' or not (key.startswith('bin/') or key in RETAIL_NAMES):
		return
	name = RETAIL_NAMES.get(key, os.path.basename(key))
	self.target = os.path.join(os.path.dirname(str(self.target)), name)
	self.env = self.env.derive()
	self.env.cshlib_PATTERN = self.env.cxxshlib_PATTERN = '%s.so'

@feature('cshlib', 'cxxshlib', 'cprogram', 'cxxprogram')
@after_method('propagate_uselib_vars')
def retail_linux32_link(self):
	'''makefile LIB_END_SHLIB: system libraries, libstdc++ and the version script, linked by
	gcc-9 as $(LINK). gcc-9's libstdc++ is static only (--disable-shared); the makefile's
	-print-file-name=libstdc++.so names nothing, so it's named directly.'''
	if not self.env.RETAIL_LINUX32 or not getattr(self, 'link_task', None):
		return
	env = self.link_task.env
	# shared libraries of this build by file name (libtier0.so, not -llibtier0)
	ours = {}
	for u in self.to_list(getattr(self, 'use', [])):
		try:
			tg = self.bld.get_tgen_by_name(u)
		except Exception:
			continue
		lt = getattr(tg, 'link_task', None)
		if lt and lt.__class__.__name__ in ('cshlib', 'cxxshlib'):
			ours[os.path.basename(str(tg.target))] = ':' + lt.outputs[0].name
	env.LIB = [ours.get(l, l) for l in env.LIB]
	import retail_manifest
	kind, key, _ = retail_manifest.lookup(self)
	if kind == 'module':
		env.LINKFLAGS = list(env.LINKFLAGS) + ['-Wl,--wrap=' + f for f in PATHWRAP]
	vs = self.bld.srcnode.find_node('devtools/version_script.linux.txt')
	if self.link_task.__class__.__name__ in ('cprogram', 'cxxprogram'):
		# source_exe_posix_base.vpc's $GCC_ExtraLinkerFlags (tcmalloc_minimal in 2018, mimalloc in
		# retail's NEEDED), then LIB_END_EXE
		env.LDFLAGS = list(env.LDFLAGS) + ['-L' + env.RETAIL_LIBDIR, '-Wl,--no-as-needed', '-l:libmimalloc.so',
			'-Wl,--as-needed', '-lm', '-ldl', '-l:libstdc++.a', '-lpthread']
	else:
		env.LDFLAGS = list(env.LDFLAGS) + SYSTEM_LIBS.get(os.path.basename(str(self.target)), []) + \
			['-lm', '-ldl', '-l:libstdc++.a', '-lpthread', '-l:ld-linux.so.2', '-Wl,--version-script=' + vs.abspath()]
	out = self.link_task.outputs[0]
	# not Valve's: the map tells report-linux.py which archive members ld pulled in
	env.LDFLAGS.append('-Wl,-Map,' + out.abspath() + '.map')
	# makefile GEN_SYM (devtools/gendbg.sh) then `strip -x -S`, with the image's own binutils
	# 2.22 (outside the g++-9 wrapper's PATH): stripped/NAME.so plus NAME.so.dbg
	stripped = out.parent.make_node('stripped').make_node(out.name)
	self.create_task('retail_linux32_strip', out, [stripped, stripped.parent.make_node(out.name + '.dbg')])

class retail_linux32_strip(Task.Task):
	color = 'BLUE'
	def run(self):
		so, dbg = self.outputs[0].abspath(), self.outputs[1].abspath()
		os.makedirs(os.path.dirname(so), exist_ok=True)
		shutil.copy2(self.inputs[0].abspath(), so)
		scout = self.env.RETAIL_SCOUT
		cwd = os.path.dirname(so)
		so, dbg = os.path.basename(so), os.path.basename(dbg)
		for cmd in (['/usr/bin/objcopy', so, dbg], ['/usr/bin/objcopy', '--add-gnu-debuglink=' + dbg, so],
				['/usr/bin/strip', '-x', '-S', so]):
			if self.exec_command(scout + cmd, cwd=cwd):
				return 1
		return 0
