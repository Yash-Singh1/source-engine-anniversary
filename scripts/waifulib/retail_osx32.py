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

Only the modules in RETAIL_MODULES (or --retail-modules; an empty value
builds none) are configured. Passing --build-games (hl2, episodic or
lostcoast) adds game/client and game/server. --build-games=hl2mp adds
sdk2013mp instead: depot 223's hl2mp modules are a May 2014 build of Source
SDK 2013's mp tree, built here from that tree in its own env (HL2MP_ENV) with
the 2014 toolchain (see sdk2013mp/wscript). The default build is untouched
when the option is off.
'''

import json, os, shutil, subprocess, sys
from waflib import Build, Logs, Utils
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
	'engine/voice_codecs/miles', 'vgui2/chromehtml',
]

GAME_MODULES = ['game/client', 'game/server']

# hl2mp: env name, subproject, and the libraries it links, taken from Source
# SDK 2013's lib/public/osx32 at the 2014 revision: Valve's link dylibs and its
# prebuilt protobuf 2.3.0 (retail client has its code, without stabs).
HL2MP_ENV = 'hl2mp2014'
# chromehtml's prebuilt CEF wrapper: Chromium 18's clang and Release flags (retailflags.CEF_WRAPPER_CLANG).
CEF_WRAPPER_ENV = 'cefwrapper'
HL2MP_PROJECTS = ['sdk2013mp']
SDK2013_LIBS = ['libtier0.dylib', 'libvstdlib.dylib', 'libsteam_api.dylib', 'libprotobuf.a']

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

# Extra compiler flags per source file. Valve builds coroutine.cpp into libcoroutine_osx.a
# (vstdlib/coroutine_osx.vpc) without the stack protector, since it switches stacks.
EXTRA_FILE_CXXFLAGS = {'vstdlib/coroutine.cpp': ['-fno-stack-protector']}

# $GCC_ExtraLinkerFlags [$OSXALL] in the module's VPC.
EXTRA_LINKFLAGS = {'bin/libvstdlib': ['-all_load']}

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
	path = ctx.srcnode.find_node(TOOLCHAIN_DIR).abspath()
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

def macro_prefix_flags(conf, rf, cc, valve, subdir = None):
	'''__FILE__ as Xcode spelled it on Valve's buildbot: tree paths, relative to the build
	directory as waf passes them or absolute, under Valve's source root `valve`. `subdir`
	is a directory of the tree, and of the build directory for generated sources, that
	Valve had as the root itself (sdk2013mp).'''
	stamp = os.path.join(os.path.dirname(os.path.dirname(cc)), 'PATCHES')
	have = open(stamp).read().split() if os.path.exists(stamp) else []
	if rf.MACRO_PREFIX_MAP_PATCH not in have:
		conf.fatal('%s lacks %s (-fmacro-prefix-map); rebuild it with %s/build-clang.sh'
			% (cc, rf.MACRO_PREFIX_MAP_PATCH, TOOLCHAIN_DIR))
	roots = [conf.srcnode.path_from(conf.bldnode), conf.srcnode.abspath()]
	pairs = [(r + '/', valve) for r in roots]
	if subdir:
		pairs += [(r + '/' + subdir + '/', valve) for r in roots + [conf.bldnode.abspath()]]
		pairs.append((subdir + '/', valve))
	return rf.macro_prefix_map(pairs)

def is_macro_prefix_flag(f):
	return f.startswith('-fmacro-prefix-map=')

@conf
def retail_macro_prefix_map(bld, ours, valve):
	'''Compiler flags giving the files under `ours` (a directory relative to the calling
	wscript) the __FILE__ of Valve's directory `valve` (relative to its source root),
	for module-local copies of files Valve had elsewhere. Add them to cxxflags.'''
	node = bld.path.find_dir(ours)
	if not node:
		bld.fatal('retail_macro_prefix_map: no directory %s in %s' % (ours, bld.path.abspath()))
	root = bld.env.RETAIL_VALVE_SRC + valve.strip('/') + '/'
	return retailflags(bld).macro_prefix_map([(node.path_from(bld.bldnode) + '/', root), (node.abspath() + '/', root)])

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
	prefix_map = macro_prefix_flags(conf, rf, p['cc'], rf.VALVE_SRC)
	v.RETAIL_VALVE_SRC = rf.VALVE_SRC
	v.CFLAGS = rf.cflags(sdk) + prefix_map
	v.CXXFLAGS = rf.cxxflags(sdk) + prefix_map
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
	# SDK 2013's osx32 prebuilt; the others come from build-thirdparty.py.
	sdk2013 = os.environ.get('SOURCE_SDK_2013', '/Volumes/Storage/sco/source-sdk-2013/src')
	pb = os.path.join(sdk2013, 'lib', 'public', 'osx32', 'libprotobuf.a')
	if os.path.exists(pb):
		v.STLIB_PROTOBUF = ['protobuf']
		v.STLIBPATH_PROTOBUF = [os.path.dirname(pb)]
		# Valve's include paths: thirdparty (for "protobuf-2.3.0/src/...") and the protobuf src dir.
		tp_sdk = os.path.join(sdk2013, 'thirdparty')
		v.INCLUDES_PROTOBUF = [tp_sdk, os.path.join(tp_sdk, 'protobuf-2.3.0', 'src')]
	tp = os.path.join(os.path.dirname(os.path.dirname(p['sdk'])), rf.THIRDPARTY_DIR)
	if not os.path.isdir(tp):
		Logs.warn('retail-osx32: %s missing; run %s/build-thirdparty.py' % (tp, TOOLCHAIN_DIR))
	for name, lib in (('JPEG', 'jpeg'), ('PNG', 'png'), ('CRYPTOPP', 'cryptopp'), ('SPEEX', 'speex')):
		if os.path.exists(os.path.join(tp, 'lib', 'lib%s.a' % lib)):
			v['STLIB_' + name] = [lib]
			v['STLIBPATH_' + name] = [os.path.join(tp, 'lib')]
			v['INCLUDES_' + name] = [os.path.join(tp, 'include')]
			# Every retail module that links libjpeg/libpng was built with HAVE_JPEG/HAVE_PNG
			# (e.g. engine's TakeSnapshotJPEGToBuffer and imageutils' jpeg_UtlBuffer_dest).
			if name in ('JPEG', 'PNG'):
				v['DEFINES_' + name] = ['HAVE_' + name]

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
	if o.RETAIL_MODULES is None:
		v.RETAIL_PROJECTS = list(RETAIL_MODULES)
	else:
		v.RETAIL_PROJECTS = [m for m in o.RETAIL_MODULES.split(',') if m]
	if any(a == '--build-games' or a.startswith('--build-games=') for a in sys.argv):
		if o.GAMES == 'hl2mp':
			hl2mp_configure(conf, rf)
			games = HL2MP_PROJECTS
		else:
			games = GAME_MODULES
		v.RETAIL_PROJECTS += [g for g in games if g not in v.RETAIL_PROJECTS]
	Logs.info('retail-osx32: clang %s, ld64 %s, sdk %s' % (p['cc'], p['ld_dir'], sdk))
	if 'vgui2/chromehtml' in v.RETAIL_PROJECTS:
		cef_wrapper_configure(conf, rf)
	conf.add_subproject(v.RETAIL_PROJECTS)

def cef_wrapper_configure(conf, rf):
	'''CEF_WRAPPER_ENV: the retail env with libcef_dll_wrapper.a's compiler and flags.'''
	p = rf.paths(conf.options.RETAIL_TOOLCHAIN, rf.CEF_WRAPPER_CLANG)
	if not os.path.exists(p['cxx']):
		Logs.warn('retail-osx32: %s missing, cef_dll_wrapper is built with the module compiler; build it with %s/build-clang.sh %s'
			% (p['cxx'], TOOLCHAIN_DIR, rf.CEF_WRAPPER_CLANG))
		return
	conf.setenv(CEF_WRAPPER_ENV, conf.env)
	v = conf.env
	v.CXX = [p['cxx']]
	# Keep what configure added after the retail flags (gccdeps' -MMD); Valve's defines and -U don't apply,
	# and the upstream wrapper compiler has no -fmacro-prefix-map.
	v.CXXFLAGS = rf.cef_wrapper_cxxflags(p['sdk']) + [f for f in v.CXXFLAGS
		if f not in rf.cxxflags(p['sdk']) and not is_macro_prefix_flag(f)]
	v.CPPFLAGS = []
	v.DEFINES = []
	v.INCLUDES = []
	conf.setenv('')
	Logs.info('retail-osx32: cef_dll_wrapper with clang %s' % p['cxx'])

def hl2mp_configure(conf, rf):
	'''The HL2MP_ENV env: the retail env with the 2014 toolchain, deployment target and link dylibs.'''
	h = rf.HL2MP_2014
	p = rf.paths(conf.options.RETAIL_TOOLCHAIN, h['clang'], h['ld64'])
	if not os.path.exists(p['cc']) or not os.path.exists(os.path.join(p['ld_dir'], 'ld')):
		conf.fatal('hl2mp needs clang-%s and ld64-%s; build them with %s/build-clang.sh %s and build-ld64.sh %s'
			% (h['clang'], h['ld64'], TOOLCHAIN_DIR, h['clang'], h['ld64']))
	patches = conf.path.find_node(TOOLCHAIN_DIR).ant_glob('patches/clang-%s-*.patch' % h['clang'])
	stamp = os.path.join(os.path.dirname(os.path.dirname(p['cc'])), 'PATCHES')
	have = open(stamp).read().split() if os.path.exists(stamp) else []
	for n in patches:
		if n.name not in have:
			conf.fatal('%s lacks %s; rebuild it with %s/build-clang.sh %s' % (p['cc'], n.name, TOOLCHAIN_DIR, h['clang']))
	protoc = os.path.join(os.path.dirname(os.path.dirname(p['sdk'])), 'protoc-2.3.0', 'bin', 'protoc')
	if not os.path.exists(protoc):
		conf.fatal('hl2mp needs %s; build it with %s/build-protoc.sh' % (protoc, TOOLCHAIN_DIR))

	libdir = conf.bldnode.make_node(HL2MP_ENV + '-libs')
	libdir.mkdir()
	for name in SDK2013_LIBS:
		spec = '%s:mp/src/lib/public/osx32/%s' % (rf.SDK2013_COMMIT, name)
		try:
			data = subprocess.check_output(['git', '-C', rf.SDK2013, 'show', spec], stderr = subprocess.DEVNULL)
		except (OSError, subprocess.CalledProcessError):
			conf.fatal('hl2mp: cannot read %s from the Source SDK 2013 checkout %s (set SOURCE_SDK_2013)' % (spec, rf.SDK2013))
		tmp = os.path.join(libdir.abspath(), name + '.fat')
		with open(tmp, 'wb') as f:
			f.write(data)
		if not thin_copy(tmp, os.path.join(libdir.abspath(), name)):
			conf.fatal('hl2mp: %s has no i386 slice' % spec)
		os.remove(tmp)

	conf.setenv(HL2MP_ENV, conf.env)
	v = conf.env
	v.CC = v.LINK_CC = [p['cc']]
	v.CXX = v.LINK_CXX = [p['cxx']]
	# Keep what configure added after the retail flags (gccdeps' -MMD).
	prefix_map = macro_prefix_flags(conf, rf, p['cc'], rf.HL2MP_VALVE_SRC, HL2MP_PROJECTS[0])
	v.RETAIL_VALVE_SRC = rf.HL2MP_VALVE_SRC
	v.CFLAGS = rf.cflags(p['sdk'], h['min_osx']) + prefix_map + [f for f in v.CFLAGS
		if f not in rf.cflags(p['sdk']) and not is_macro_prefix_flag(f)]
	v.CXXFLAGS = rf.cxxflags(p['sdk'], h['min_osx']) + prefix_map + [f for f in v.CXXFLAGS
		if f not in rf.cxxflags(p['sdk']) and not is_macro_prefix_flag(f)]
	v.LINKFLAGS = rf.linkflags(p, h['min_osx'])
	v.DEFINES = list(rf.DEFINES)
	# The 2014 tree has its own common/; ours must not shadow missing headers.
	common = conf.path.find_node('common').abspath()
	v.INCLUDES = [i for i in v.INCLUDES if i != common]
	v.RETAIL_LIBDIR = libdir.abspath()
	v.RETAIL_PREBUILT_DYLIBS = True
	v.STLIB_PROTOBUF = ['protobuf']
	v.STLIBPATH_PROTOBUF = [libdir.abspath()]
	v.PROTOC = protoc
	conf.setenv('')
	Logs.info('retail-osx32: hl2mp with clang %s, ld64 %s, deployment target %s' % (p['cc'], p['ld_dir'], h['min_osx']))

@conf
def retail_stlib_variant(bld, name, suffix, flags):
	'''Copy of static library `name` built as `name + suffix` with extra compiler flags, for
	modules whose retail build links its own build of the library (e.g. sourcevr at 10.5).
	Returns the new target name; pass it in the module's `use` and `retail_archives`.
	Modules asking for the same variant share one copy.'''
	try:
		bld.get_tgen_by_name(name + suffix)
		return name + suffix
	except Exception:
		pass
	try:
		tg = bld.get_tgen_by_name(name)
	except Exception:
		bld.fatal('retail-osx32: %s variant: %s must be configured before the module using it' % (suffix, name))
	import retail_manifest
	kind, key, _ = retail_manifest.lookup(tg)
	v = tg.clone(tg.env)
	v.name = v.target = name + suffix
	v.idx = bld.get_taskgen_count()
	if kind == 'archive':
		v.retail_archive = key
	v.cflags = tg.to_list(getattr(tg, 'cflags', [])) + flags
	v.cxxflags = tg.to_list(getattr(tg, 'cxxflags', [])) + flags
	return v.name

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
	if getattr(self, 'retail_defines', None) is not None:
		valve = [d for d in self.retail_defines if _name(d) not in undef]
	elif e and e.get('defines'):
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
	# retail_file_cxxflags: {tree-relative source: [flags]} for single objects.
	per_file = getattr(self, 'retail_file_cxxflags', None) or {}
	for t in getattr(self, 'compiled_tasks', []):
		t.env.DEFINES = defs
		if extra:
			t.env.CXXFLAGS = self.env.CXXFLAGS
		if as_cxx and t.__class__.__name__ == 'c':
			t.env.CC = self.env.CXX
			t.env.CFLAGS = list(self.env.CXXFLAGS) + ['-x', 'c++']
		src = t.inputs[0].path_from(self.bld.srcnode).replace(os.sep, '/')
		flags = EXTRA_FILE_CXXFLAGS.get(src, []) + per_file.get(src, [])
		if flags:
			t.env = t.env.derive()
			t.env.CXXFLAGS = list(t.env.CXXFLAGS) + flags

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
		'-compatibility_version', '1', '-current_version', '1'] + EXTRA_LINKFLAGS.get(e[0], []))

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
			target = None if self.env.RETAIL_PREBUILT_DYLIBS else by_install.get(path)
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

LINK_TASK_CLASSES = ('cshlib', 'cxxshlib', 'cprogram', 'cxxprogram', 'cstlib', 'cxxstlib')
COMPILE_TASK_CLASSES = ('c', 'cxx')

class RetailLinkDb(Build.BuildContext):
	'''writes link_commands.json (link and archive commands) and retail_compile_commands.json
	(every compile command, one per object; compile_commands.json keeps one per source file)
	for devtools/bytematch/identity'''
	cmd = 'retail_linkdb'

	def execute(self):
		self.restore()
		if not self.all_envs:
			self.load_envs()
		self.recurse([self.run_dir])
		self.pre_build()

		def exec_command(*k, **kw):
			return 0
		entries, compiles = [], []
		for g in self.groups:
			for tg in g:
				if hasattr(tg, 'post'):
					tg.post()
				for tsk in getattr(tg, 'tasks', []):
					kind = tsk.__class__.__name__
					if kind in COMPILE_TASK_CLASSES:
						tsk.keep_last_cmd = True
						old = tsk.exec_command
						tsk.exec_command = exec_command
						try:
							tsk.run()
						finally:
							tsk.exec_command = old
						cmd = getattr(tsk, 'last_cmd', None)
						if cmd:
							compiles.append({
								'directory': tsk.get_cwd().abspath(),
								'arguments': cmd if isinstance(cmd, list) else Utils.split_quoted(cmd),
								'file': tsk.inputs[0].abspath(),
								'output': tsk.outputs[0].abspath(),
							})
						continue
					if kind not in LINK_TASK_CLASSES:
						continue
					if kind.endswith('stlib'):
						# stlink_task.run deletes its output first, so spell the ar command out
						cwd = tsk.get_cwd()
						env = tsk.env
						tgt_f, src_f = (''.join(Utils.to_list(env[k])) for k in ('AR_TGT_F', 'AR_SRC_F'))
						cmd = (Utils.to_list(env.AR) + Utils.to_list(env.ARFLAGS) + [tgt_f + tsk.outputs[0].path_from(cwd)] +
							[src_f + n.path_from(cwd) for n in tsk.inputs])
					else:
						tsk.keep_last_cmd = True
						old = tsk.exec_command
						tsk.exec_command = exec_command
						try:
							tsk.run()
						finally:
							tsk.exec_command = old
						cmd = getattr(tsk, 'last_cmd', None)
					if not cmd:
						continue
					entries.append({
						'target': getattr(tg, 'name', None),
						'kind': tsk.__class__.__name__,
						'directory': tsk.get_cwd().abspath(),
						'arguments': cmd if isinstance(cmd, list) else Utils.split_quoted(cmd),
						'output': tsk.outputs[0].abspath(),
						'inputs': [n.abspath() for n in tsk.inputs],
					})
		node = self.bldnode.make_node('link_commands.json')
		node.write_json(entries)
		Logs.info('Link commands stored in %s', node.path_from(self.path))
		node = self.bldnode.make_node('retail_compile_commands.json')
		node.write_json(compiles)
		Logs.info('Compile commands stored in %s', node.path_from(self.path))
