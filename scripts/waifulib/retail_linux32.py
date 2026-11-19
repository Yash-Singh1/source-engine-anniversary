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
from waflib import Logs
from waflib.Configure import conf
from waflib.TaskGen import feature, before_method, after_method

MANIFEST = 'devtools/bytematch/manifests/retail-linux32.json'
OSX_MANIFEST = 'devtools/bytematch/manifests/retail-osx32.json'
TOOLCHAIN_DIR = 'devtools/bytematch/toolchain/linux'

LIB_MODULES = ['tier1', 'mathlib', 'tier2', 'tier3', 'particles', 'dmxloader', 'choreoobjects',
	'vgui2/vgui_controls', 'vgui2/matsys_controls', 'bitmap', 'vtf']
GAME_MODULES = ['game/client', 'game/server']

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
# Retail Linux game modules import no Telemetry symbols, though SDK 2013's
# vprof_telemetry.h enables it for _LINUX: Valve's 2015 linux VPCs define this.
RETAIL_DEFINES = ['BYTEMATCH_RETAIL', 'BYTEMATCH_RETAIL_LINUX', 'RAD_TELEMETRY_DISABLED']

# Defines from this tree's wscripts and global env that Valve's build doesn't have.
TREE_ONLY_DEFINES = ['PLATFORM_POSIX', 'NO_MEMOVERRIDE_NEW_DELETE', 'GIT_COMMIT_HASH',
	'DX_TO_GL_ABSTRACTION_DISABLED', 'NO_STEAM', 'DISABLE_STEAM', 'WAF_CFLAGS', 'WAF_LDFLAGS',
	'_OSX', 'OSX', 'OSXALL', '_DARWIN_UNLIMITED_SELECT', 'FD_SETSIZE', 'OVERRIDE_V_DEFINES',
	'QUICKTIME_VIDEO', 'FORCE_QUICKTIME', 'TOGL_OSX', 'CPP11_NO_LIBCXX', 'PNG_NO_PEDANTIC_WARNINGS']

# Retail shared libraries the modules link (depot 224 bin/), and the system libraries
# after them (makefile LIB_END_SHLIB; client adds librt, NEEDED in retail client.so).
DEPOT_LIBS = {'tier0': 'libtier0.so', 'vstdlib': 'libvstdlib.so', 'steam_api': 'libsteam_api.so'}
SYSTEM_LIBS = {'client': ['-lrt']}

_manifests = {}

def options(opt):
	grp = opt.add_option_group('Byte-match options')
	grp.add_option('--retail-linux32', action = 'store_true', dest = 'RETAIL_LINUX32', default = False,
		help = 'build the game modules like retail Linux i386 (scout gcc-9 in Docker) [default: %default]')
	grp.add_option('--retail-depot224', action = 'store', dest = 'RETAIL_DEPOT224',
		default = '/Volumes/Expansion/mac-ports/hl2-anniversary-depot/depots/224/19307283',
		help = 'retail depot 224 build root, for libtier0/libvstdlib/libsteam_api [default: %default]')
	grp.add_option('--retail-linux32-modules', action = 'store', dest = 'RETAIL_LINUX32_MODULES', default = None,
		help = 'comma-separated subprojects [default: the game modules\' static libraries, plus game/client and game/server with --build-games]')

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
	v.RETAIL_LINUX32 = True
	v.RETAIL_SCOUT = scout
	v.COMPILER_CC = 'gcc'
	v.COMPILER_CXX = 'g++'
	v.DEST_OS = 'linux'
	v.DEST_CPU = 'x86'
	v.DEST_BINFMT = 'elf'
	v.DEST_SIZEOF_VOID_P = 4
	v.SDL = 1
	v.GL = True
	v.CFLAGS = ARCH_FLAGS + WARN_FLAGS + OPT_FLAGS
	v.CXXFLAGS = ARCH_FLAGS + WARN_FLAGS + OPT_FLAGS + CXX_ONLY
	v.CPPFLAGS = list(UNDEFS)
	# default PIE, as the makefile (no -fPIC); same .text as -fPIC with hidden visibility
	v.CFLAGS_cshlib = v.CXXFLAGS_cxxshlib = []
	v.LINKFLAGS = ['-m32', '-Wl,--build-id', '-static-libgcc']
	v.LINKFLAGS_cshlib = v.LINKFLAGS_cxxshlib = ['-shared']
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
	sdl = conf.path.find_node('thirdparty/SDL')
	if sdl:
		v.append_value('INCLUDES', [sdl.abspath()])
	v.LIBDIR = v.PREFIX + '/bin/'
	v.BINDIR = v.PREFIX

	v.RETAIL_MANIFEST = conf.path.find_node(OSX_MANIFEST).abspath()
	v.RETAIL_MANIFEST_LINUX = conf.path.find_node(MANIFEST).abspath()
	v.RETAIL_SELECT = True
	if o.RETAIL_LINUX32_MODULES is not None:
		v.RETAIL_PROJECTS = [m for m in o.RETAIL_LINUX32_MODULES.split(',') if m]
	else:
		v.RETAIL_PROJECTS = list(LIB_MODULES)
		if any(a == '--build-games' or a.startswith('--build-games=') for a in sys.argv):
			v.RETAIL_PROJECTS += GAME_MODULES
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
	for d in valve + RETAIL_DEFINES + ours:
		if d not in defs:
			defs.append(d)
	self.env.DEFINES = defs
	proj = e['project_dir'] if e else self.path.path_from(self.bld.srcnode)
	tag = ['--bm-proj=' + proj]
	self.env.append_value('CFLAGS', tag)
	self.env.append_value('CXXFLAGS', tag)
	for t in getattr(self, 'compiled_tasks', []):
		t.env.DEFINES = defs
		t.env.CFLAGS = self.env.CFLAGS
		t.env.CXXFLAGS = self.env.CXXFLAGS

@feature('cshlib', 'cxxshlib')
@after_method('propagate_uselib_vars')
def retail_linux32_link(self):
	'''makefile LIB_END_SHLIB: system libraries and the version script. The g++-9 driver
	links gcc-9's libstdc++, which is static only (--disable-shared).'''
	if not self.env.RETAIL_LINUX32 or not getattr(self, 'link_task', None):
		return
	env = self.link_task.env
	vs = self.bld.srcnode.find_node('devtools/version_script.linux.txt')
	env.LDFLAGS = list(env.LDFLAGS) + SYSTEM_LIBS.get(os.path.basename(str(self.target)), []) + \
		['-lm', '-ldl', '-lpthread', '-l:ld-linux.so.2', '-Wl,--version-script=' + vs.abspath()]
