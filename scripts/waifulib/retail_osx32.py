#!/usr/bin/env python
# encoding: utf-8
'''
--retail-osx32: build like Valve's retail macOS i386 Xcode build (depot 223).

Uses the toolchain from devtools/bytematch/toolchain (Apple clang-600.0.57,
ld64-242, MacOSX10.9 stub SDK; default prefix ~/.cache/source-bytematch/toolchain)
with the xcconfig/VPC flags in devtools/bytematch/toolchain/retailflags.py.
Per module it applies Valve's defines, output name, install name and
LC_LOAD_DYLIB order from devtools/bytematch/manifests/retail-osx32.json and
implies --retail-manifest. Third-party dylibs (SDL2, steam_api, Miles, CEF)
are linked from the retail depot, thinned to i386.

Only the modules in RETAIL_MODULES (or --retail-modules) are configured.
Passing --build-games (hl2, episodic or lostcoast) adds game/client and
game/server. The default build is untouched when the option is off.
'''

import json, os, shutil, subprocess, sys
from waflib import Logs, Utils
from waflib.Configure import conf
from waflib.TaskGen import feature, before_method, after_method

MANIFEST = 'devtools/bytematch/manifests/retail-osx32.json'
TOOLCHAIN_DIR = 'devtools/bytematch/toolchain'

# Modules that compile and link in this mode (game modules are in GAME_MODULES).
RETAIL_MODULES = [
	'tier0', 'tier1', 'mathlib', 'tier2', 'tier3', 'vstdlib',
	'appframework', 'bitmap', 'vtf', 'choreoobjects', 'dmxloader', 'particles', 'vpklib',
	'materialsystem/shaderlib', 'vgui2/vgui_controls', 'vgui2/matsys_controls',
	'scenefilecache', 'soundemittersystem', 'datacache', 'video', 'inputsystem',
	'filesystem', 'vgui2/src', 'studiorender', 'serverbrowser', 'utils/vtex',
	'materialsystem/shaderapiempty', 'engine/voice_codecs/speex', 'launcher_main',
	'ivp/havana', 'ivp/havana/havok/hk_base', 'ivp/havana/havok/hk_math',
	'ivp/ivp_compact_builder', 'ivp/ivp_physics', 'vphysics', 'materialsystem/stdshaders',
	'utils/bzip2', 'replay/common', 'replay', 'sourcevr', 'video/video_quicktime',
	'utils/bsppack', 'utils/bugreporter_filequeue', 'utils/bugreporter_public', 'utils/vpk',
	'vgui2/vgui_surfacelib', 'vguimatsurface', 'gameui', 'engine',
	'togl', 'materialsystem', 'launcher', 'materialsystem/shaderapidx9',
	'engine/voice_codecs/miles',
]

GAME_MODULES = ['game/client', 'game/server']

# Defines our top-level wscript adds that Valve's build doesn't have.
TREE_ONLY_DEFINES = ['PLATFORM_POSIX', 'NO_MEMOVERRIDE_NEW_DELETE', 'GIT_COMMIT_HASH',
	'DX_TO_GL_ABSTRACTION_DISABLED', 'NO_STEAM', 'DISABLE_STEAM']

BUILD_PATH_DEFINES = ['WAF_CFLAGS', 'WAF_LDFLAGS']

# Extra compiler flags per task generator name. ivp includes <malloc/malloc.h> when
# OSX is defined, which doesn't declare alloca on the 10.6 headers.
# osxfont.cpp brace-initialises a CGRect from ints, an error under clang-600's C++11.
EXTRA_CXXFLAGS = dict((t, ['-include', 'alloca.h']) for t in
	('ivp_physics', 'ivp_compactbuilder', 'havana_constraints', 'hk_base', 'hk_math'))
EXTRA_CXXFLAGS['vgui_surfacelib'] = ['-Wno-c++11-narrowing']

# Modules whose VPC the manifest couldn't evaluate: take a sibling's base defines.
DEFINES_FROM = {'bin/video_quicktime': 'bin/video_services'}

# Valve's Xcode projects compile these C sources as C++ (N_SO names *.ii).
COMPILE_C_AS_CXX = ['bzip2']

# Retail third-party dylibs, linked from the depot (user-provided, never rebuilt).
DEPOT_DYLIBS = ['libSDL2-2.0.0.dylib', 'libsteam_api.dylib', 'libMilesX86.dylib', 'libcef.dylib']

# Symbols whose definition is source drift with no retail counterpart: left for
# dynamic lookup instead of linking libraries retail doesn't link.
UNDEFINED_OK = {
	'bin/engine': ['_GetCPUFrequencyResults'],  # tier0 cpumonitoring, absent from retail tier0
}

# Exports retail modules bind from an older libsteam_api than the depot's.
# module -> (depot dylib, extra symbols); a stub with the depot exports plus these is linked.
DEPOT_STUBS = {
	'bin/sourcevr': ('libsteam_api.dylib', ['_VR_Init', '_VR_IsHmdPresent', '_VR_Shutdown']),
}

_manifest = {}

def options(opt):
	grp = opt.add_option_group('Byte-match options')
	grp.add_option('--retail-osx32', action = 'store_true', dest = 'RETAIL_OSX32', default = False,
		help = 'build with the retail macOS i386 toolchain and flags [default: %default]')
	grp.add_option('--retail-toolchain', action = 'store', dest = 'RETAIL_TOOLCHAIN', default = None,
		help = 'toolchain prefix [default: $BYTEMATCH_TOOLCHAIN or ~/.cache/source-bytematch/toolchain]')
	grp.add_option('--retail-depot', action = 'store', dest = 'RETAIL_DEPOT', default = None,
		help = 'retail depot 223 build root, for third-party dylibs [default: see retailflags.py]')
	grp.add_option('--retail-clang', action = 'store', dest = 'RETAIL_CLANG', default = None,
		help = 'clang version under the toolchain prefix [default: 600.0.57]')
	grp.add_option('--retail-ld64', action = 'store', dest = 'RETAIL_LD64', default = None,
		help = 'ld64 version under the toolchain prefix [default: 242]')
	grp.add_option('--retail-modules', action = 'store', dest = 'RETAIL_MODULES', default = None,
		help = 'comma-separated subprojects to build [default: %s]' % ','.join(RETAIL_MODULES))

def retailflags(ctx):
	path = ctx.path.find_node(TOOLCHAIN_DIR).abspath()
	if path not in sys.path:
		sys.path.insert(0, path)
	import retailflags
	return retailflags

def load_manifest(path):
	if path not in _manifest:
		with open(path) as f:
			m = json.load(f)
		by_target = {}
		for name, mod in m['modules'].items():
			if mod.get('waf_target') and mod['retail'].get('filetype') == 'MH_DYLIB':
				by_target.setdefault(mod['waf_target'], (name, mod))
		archives = dict((a['waf_target'], a) for a in m['archives'].values() if a.get('waf_target'))
		_manifest[path] = (m, by_target, archives)
	return _manifest[path]

def thin_copy(src, dst):
	if subprocess.call(['lipo', src, '-verify_arch', 'i386'], stderr = subprocess.DEVNULL) != 0:
		return False
	if subprocess.call(['lipo', src, '-thin', 'i386', '-output', dst], stderr = subprocess.DEVNULL) != 0:
		shutil.copy2(src, dst)  # already thin
	return True

def make_depot_stub(conf, p, src, outdir, extra):
	'''i386 dylib with src's install name, versions and exports plus `extra`.'''
	if not os.path.exists(src):
		return
	if not os.path.isdir(outdir):
		os.makedirs(outdir)
	syms = subprocess.check_output(['nm', '-gUj', src]).decode().split()
	names = sorted(set(syms + extra))
	cfile = os.path.join(outdir, 'stub.c')
	with open(cfile, 'w') as f:
		for n in names:
			f.write('void %s(void) {}\n' % n[1:])
	ident = subprocess.check_output(['otool', '-D', src]).decode().split('\n')[1].strip()
	ver = subprocess.check_output(['otool', '-L', src]).decode().split('\n')[1]
	compat = ver.split('compatibility version ')[1].split(',')[0]
	current = ver.split('current version ')[1].split(')')[0]
	out = os.path.join(outdir, os.path.basename(src))
	cmd = [p['cc'], '-arch', 'i386', '-isysroot', p['sdk'], '-mmacosx-version-min=10.6', '-dynamiclib',
		'-nostdlib', '-B', p['ld_dir'], '-mlinker-version=' + p['ld_version'], '-Wl,-undefined,dynamic_lookup',
		'-install_name', ident, '-compatibility_version', compat, '-current_version', current, cfile, '-o', out]
	if subprocess.call(cmd) != 0:
		conf.fatal('retail-osx32: could not build stub %s' % out)

@conf
def retail_osx32_configure(conf):
	rf = retailflags(conf)
	o = conf.options
	p = rf.paths(o.RETAIL_TOOLCHAIN, o.RETAIL_CLANG, o.RETAIL_LD64)
	for k in ('cc', 'sdk'):
		if not os.path.exists(p[k]):
			conf.fatal('%s not found; build the toolchain with %s/*.sh (see %s/README.md)' % (p[k], TOOLCHAIN_DIR, TOOLCHAIN_DIR))
	if not os.path.exists(os.path.join(p['ld_dir'], 'ld')):
		conf.fatal('ld64 not found in %s' % p['ld_dir'])

	conf.environ['CC'] = p['cc']
	conf.environ['CXX'] = p['cxx']
	conf.load('compiler_c compiler_cxx mm_hook')
	if conf.env.DEST_OS != 'darwin':
		conf.fatal('--retail-osx32 needs the Apple clang toolchain (got DEST_OS=%s)' % conf.env.DEST_OS)

	v = conf.env
	v.RETAIL_OSX32 = True
	v.DEST_CPU = 'x86'
	v.DEST_SIZEOF_VOID_P = 4
	# Valve's VPC defines USE_SDL and DX_TO_GL_ABSTRACTION; these select the same sources in our wscripts.
	v.SDL = 1
	v.GL = True
	sdk = p['sdk']
	v.CFLAGS = rf.cflags(sdk)
	v.CXXFLAGS = rf.cxxflags(sdk)
	v.CPPFLAGS = list(rf.UNDEFS)
	v.LINKFLAGS = rf.linkflags(p)
	v.CFLAGS_cshlib = v.CXXFLAGS_cxxshlib = []
	# Retail modules were built against the togl/osx headers (debug map: public/togl/osx/*.h).
	v.DEFINES = list(rf.DEFINES) + ['TOGL_OSX']
	v.RETAIL_UNDEFINED = list(rf.UNDEFINED_NAMES)
	v.INCLUDES = [conf.path.find_node('common').abspath()]
	conf.load('gccdeps')

	# System libraries come from the stub SDK; zlib and curl are system dylibs on Mac.
	v.LIB_ICONV = ['iconv']
	v.LIB_ZLIB = ['z']
	v.LIB_CURL = ['curl']
	for fw in ('AppKit', 'IOKit', 'Foundation', 'CoreFoundation', 'OpenGL', 'Carbon',
			'ApplicationServices', 'CoreServices', 'CoreAudio', 'AudioToolbox',
			'SystemConfiguration', 'OpenAL', 'QuickTime', 'Cocoa', 'ForceFeedback', 'AudioUnit'):
		v['FRAMEWORK_' + fw.upper()] = fw
	v.FRAMEWORK_COREGRAPHICS = 'ApplicationServices'

	# Period third-party archives (see thirdparty-static.md). protobuf 2.3.0 is
	# SDK 2013's osx32 prebuilt; the others come from build-thirdparty.sh.
	sdk2013 = os.environ.get('SOURCE_SDK_2013', '/Volumes/Storage/sco/source-sdk-2013/src')
	pb = os.path.join(sdk2013, 'lib', 'public', 'osx32', 'libprotobuf.a')
	if os.path.exists(pb):
		v.STLIB_PROTOBUF = ['protobuf']
		v.STLIBPATH_PROTOBUF = [os.path.dirname(pb)]
	tp = os.path.join(os.path.dirname(os.path.dirname(p['sdk'])), 'thirdparty-osx32')
	for name, lib in (('JPEG', 'jpeg'), ('PNG', 'png'), ('CRYPTOPP', 'cryptopp'), ('SPEEX', 'speex')):
		if os.path.exists(os.path.join(tp, 'lib', 'lib%s.a' % lib)):
			v['STLIB_' + name] = [lib]
			v['STLIBPATH_' + name] = [os.path.join(tp, 'lib')]
			v['INCLUDES_' + name] = [os.path.join(tp, 'include')]

	# User-provided retail dylibs, thinned so the native ld64 can map them.
	depot = o.RETAIL_DEPOT or rf.RETAIL_DEPOT
	libdir = conf.bldnode.make_node('retail-depot-libs')
	libdir.mkdir()
	for name in DEPOT_DYLIBS:
		src = os.path.join(depot, 'bin', name)
		if os.path.exists(src) and thin_copy(src, os.path.join(libdir.abspath(), name)):
			continue
		Logs.warn('retail-osx32: %s not found in %s' % (name, depot))
	v.RETAIL_DEPOT = depot
	v.RETAIL_LIBDIR = libdir.abspath()
	for key, (name, extra) in DEPOT_STUBS.items():
		make_depot_stub(conf, p, os.path.join(libdir.abspath(), name),
			os.path.join(libdir.abspath(), key.replace('/', '_')), extra)
	# source_video_base.vpc puts ../thirdparty/SDL on every module's include path.
	sdl = conf.path.find_node('thirdparty/SDL')
	if not sdl or not sdl.find_node('SDL.h'):
		conf.fatal('thirdparty/SDL headers missing; run git submodule update --init thirdparty')
	v.INCLUDES_SDL2 = []
	v.append_value('INCLUDES', [sdl.abspath()])
	v.LIBPATH_SDL2 = v.LIBPATH_STEAM_API = [libdir.abspath()]
	v.LIB_SDL2 = ['SDL2-2.0.0']
	v.LIB_STEAM_API = ['steam_api']

	v.RETAIL_MANIFEST = conf.path.find_node(MANIFEST).abspath()
	v.RETAIL_SELECT = True
	v.LIBDIR = v.PREFIX + '/bin/'
	v.BINDIR = v.PREFIX
	v.RETAIL_PROJECTS = (o.RETAIL_MODULES.split(',') if o.RETAIL_MODULES else list(RETAIL_MODULES))
	if any(a == '--build-games' or a.startswith('--build-games=') for a in sys.argv):
		v.RETAIL_PROJECTS += [g for g in GAME_MODULES if g not in v.RETAIL_PROJECTS]
	Logs.info('retail-osx32: clang %s, ld64 %s, sdk %s' % (p['cc'], p['ld_dir'], sdk))
	conf.add_subproject(v.RETAIL_PROJECTS)

def _entry(tg):
	if not tg.env.RETAIL_MANIFEST:
		return None, None
	import retail_manifest
	kind, key, e = retail_manifest.lookup(tg)
	if kind == 'module':
		return kind, (key, e)
	return kind, e

def _name(d):
	return d.split('=')[0]

@feature('c', 'cxx')
@before_method('process_source')
def retail_osx32_own_env(self):
	if self.env.RETAIL_OSX32:
		self.env = self.env.derive()

@feature('c', 'cxx')
@after_method('propagate_uselib_vars')
def retail_osx32_defines(self):
	if not self.env.RETAIL_OSX32:
		return
	kind, e = _entry(self)
	if kind == 'module':
		key, e = e
		if not (e.get('defines') or {}).get('vpc') and key in DEFINES_FROM:
			import retail_manifest
			src = retail_manifest._load(self.env.RETAIL_MANIFEST)['modules'][DEFINES_FROM[key]]
			dllname = 'DLLNAME=' + os.path.basename(key)
			e = {'defines': {'xcconfig': src['defines']['xcconfig'],
				'vpc': [dllname if d.startswith('DLLNAME=') else d for d in src['defines']['vpc']]}}
	undef = set(self.env.RETAIL_UNDEFINED)
	ours = [d for d in self.env.DEFINES if _name(d) not in TREE_ONLY_DEFINES]
	# tier0's log banner embeds the configure flags, i.e. local toolchain paths.
	ours = [_name(d) + '=""' if _name(d) in BUILD_PATH_DEFINES else d for d in ours]
	valve = []
	if e and e.get('defines'):
		valve = [d for d in (e['defines']['xcconfig'] or []) + (e['defines']['vpc'] or []) if _name(d) not in undef]
	names = set(_name(d) for d in valve)
	defs = []
	for d in valve + [d for d in ours if _name(d) not in names]:
		if d not in defs:
			defs.append(d)
	self.env.DEFINES = defs
	extra = EXTRA_CXXFLAGS.get(getattr(self, 'name', None), [])
	if extra:
		self.env.append_value('CXXFLAGS', extra)
	as_cxx = getattr(self, 'name', None) in COMPILE_C_AS_CXX
	for t in getattr(self, 'compiled_tasks', []):
		t.env.DEFINES = defs
		if extra:
			t.env.CXXFLAGS = self.env.CXXFLAGS
		if as_cxx and t.__class__.__name__ == 'c':
			t.env.CC = self.env.CXX
			t.env.CFLAGS = list(self.env.CXXFLAGS) + ['-x', 'c++']

@feature('cprogram', 'cxxprogram')
@before_method('apply_link')
def retail_osx32_exe_name(self):
	if not self.env.RETAIL_OSX32:
		return
	kind, e = _entry(self)
	if kind == 'module':
		self.target = os.path.join(os.path.dirname(self.target), os.path.basename(e[1]['retail']['path']))

@feature('cshlib', 'cxxshlib')
@before_method('apply_link')
def retail_osx32_dylib_name(self):
	if not self.env.RETAIL_OSX32:
		return
	kind, e = _entry(self)
	if kind != 'module':
		return
	base = os.path.basename(e[1]['retail']['path'])
	stem = os.path.basename(self.target)
	if stem in base:
		pattern = base.replace(stem, '%s', 1)
	else:
		pattern = '%s.dylib'
		self.target = os.path.join(os.path.dirname(self.target), base[:-len('.dylib')])
	self.env.cshlib_PATTERN = self.env.cxxshlib_PATTERN = pattern
	self.env.append_value('LINKFLAGS', ['-install_name', e[1]['retail']['install_name'],
		'-compatibility_version', '1', '-current_version', '1'])

@feature('cshlib', 'cxxshlib', 'cprogram', 'cxxprogram')
@after_method('propagate_uselib_vars')
def retail_osx32_link_order(self):
	'''Link dylibs in the retail LC_LOAD_DYLIB order instead of waf's frameworks-first order.'''
	if not self.env.RETAIL_OSX32 or not getattr(self, 'link_task', None):
		return
	kind, e = _entry(self)
	if kind != 'module':
		return
	m, by_target, _ = load_manifest(self.env.RETAIL_MANIFEST)
	by_install = dict((mod['retail'].get('install_name'), mod.get('waf_target'))
		for mod in m['modules'].values())
	args = []
	for d in e[1]['dylibs']:
		path = d['path']
		if 'libSystem' in path:
			break  # later entries were linked implicitly via re-exports (e.g. Carbon -> ApplicationServices)
		if path.startswith('@'):
			target = by_install.get(path)
			tg = None
			if target:
				try:
					tg = self.bld.get_tgen_by_name(target)
				except Exception:
					tg = None
			if tg and tg is not self:
				tg.post()
				node = tg.link_task.outputs[0]
				self.link_task.set_run_after(tg.link_task)
				self.link_task.dep_nodes.append(node)
				args.append(node.abspath())
			else:
				lib = os.path.join(self.env.RETAIL_LIBDIR, os.path.basename(path))
				stub = DEPOT_STUBS.get(e[0])
				if stub and stub[0] == os.path.basename(path):
					lib = os.path.join(self.env.RETAIL_LIBDIR, e[0].replace('/', '_'), stub[0])
				if not os.path.exists(lib):
					src = os.path.join(self.env.RETAIL_DEPOT, 'bin', os.path.basename(path))
					if not os.path.exists(src) or not thin_copy(src, lib):
						Logs.warn('%s: no %s to link' % (self.target, path))
						continue
				args.append(lib)
		elif path.startswith('/System/Library/Frameworks/'):
			args += ['-framework', path.split('/')[4].rsplit('.', 1)[0]]
		elif 'libstdc++' in path or 'libSystem' in path:
			continue  # added by the clang++ driver
		elif path.startswith('/usr/lib/lib'):
			args.append('-l' + os.path.basename(path)[3:].split('.')[0])
	env = self.link_task.env
	env.LIB = []
	env.FRAMEWORK = []
	flags, it = [], iter(env.LINKFLAGS)
	for f in it:
		if f == '-framework':
			next(it, None)
		else:
			flags.append(f)
	for sym in UNDEFINED_OK.get(e[0], []):
		flags.append('-Wl,-U,' + sym)
	env.LINKFLAGS = flags
	env.LDFLAGS = args
