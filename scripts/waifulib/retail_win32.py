#!/usr/bin/env python
# encoding: utf-8
'''
--retail-win32: build like Valve's retail Windows i386 build (depot 222).

Runs MSVC 2013 Update 5 under wine (devtools/bytematch/toolchain/win32; tools
in $BYTEMATCH_TOOLCHAIN/win32) and takes everything else from
devtools/bytematch/manifests/retail-win32.json, which make-manifest.py
generates from Valve's VPC scripts: per module the sources in project order,
defines, include paths, cl/link switches, precompiled headers, per-file
overrides and link libraries. The module wscripts are not used in this mode;
their source lists are this fork's, not Valve's.

Third-party DLLs (steam_api, SDL2) get import libraries generated from the
retail depot's export tables. libz.lib comes from Source SDK 2013's
lib/public (the same VS2013 build retail links). Inputs that exist nowhere
(Valve's ftol3.obj, RAD Telemetry's telemetry32.link) are skipped with a warning.
'''

import json, os, struct, subprocess, sys
from waflib import Logs, Task, Utils, Errors
from waflib.Configure import conf
from waflib.TaskGen import feature, after_method, before_method, extension

MANIFEST = 'devtools/bytematch/manifests/retail-win32.json'
TOOLCHAIN_DIR = 'devtools/bytematch/toolchain/win32'

RETAIL_MODULES = ['tier0', 'vstdlib', 'scenefilecache', 'soundemittersystem', 'datacache',
	'studiorender', 'inputsystem', 'engine']

# Not Valve's. BYTEMATCH_RETAIL excludes this fork's runtime additions (as --retail-osx32);
# NO_X360_XDK because Valve's tree had the Xbox 360 XDK's win32 headers (bitmap/imagebyteswap.cpp).
RETAIL_DEFINES = ['BYTEMATCH_RETAIL', 'NO_X360_XDK']

# Retail third-party DLLs whose import libraries are generated from their exports.
DEPOT_IMPLIBS = ['steam_api', 'SDL2', 'binkw32']

def options(opt):
	grp = opt.add_option_group('Byte-match options')
	grp.add_option('--retail-win32', action = 'store_true', dest = 'RETAIL_WIN32', default = False,
		help = 'build with the retail Windows i386 toolchain (MSVC 2013 Update 5 under wine) [default: %default]')
	grp.add_option('--retail-win32-modules', action = 'store', dest = 'RETAIL_WIN32_MODULES', default = None,
		help = 'comma-separated modules from %s [default: %s]' % (MANIFEST, ','.join(RETAIL_MODULES)))
	grp.add_option('--retail-depot222', action = 'store', dest = 'RETAIL_DEPOT222',
		default = '/Volumes/Expansion/mac-ports/hl2-anniversary-depot/depots/222/19307283',
		help = 'retail depot 222 build root, for third-party import libraries [default: %default]')
	grp.add_option('--sdk2013', action = 'store', dest = 'SDK2013',
		default = os.environ.get('SOURCE_SDK_2013', '/Volumes/Storage/sco/source-sdk-2013/src'),
		help = 'Source SDK 2013 src/ directory, for lib/public/libz.lib [default: %default]')

def toolchain(ctx):
	path = ctx.path.find_node(TOOLCHAIN_DIR).abspath()
	if path not in sys.path:
		sys.path.insert(0, path)
	import versions
	return path, versions

def load_manifest(path):
	with open(path) as f:
		return json.load(f)

def pe_exports(path):
	'''Exported names of a PE32 DLL.'''
	with open(path, 'rb') as f:
		d = f.read()
	pe = struct.unpack_from('<I', d, 0x3c)[0]
	nsec = struct.unpack_from('<H', d, pe + 6)[0]
	opt = pe + 24
	secs = opt + struct.unpack_from('<H', d, pe + 20)[0]
	exp_rva = struct.unpack_from('<I', d, opt + 96)[0]
	def off(rva):
		for i in range(nsec):
			s = secs + 40 * i
			va, vsz, raw = struct.unpack_from('<III', d, s + 12)[0], struct.unpack_from('<I', d, s + 8)[0], struct.unpack_from('<I', d, s + 20)[0]
			if va <= rva < va + max(vsz, 1):
				return rva - va + raw
		raise ValueError('rva %x' % rva)
	if not exp_rva:
		return []
	e = off(exp_rva)
	n, names = struct.unpack_from('<I', d, e + 24)[0], struct.unpack_from('<I', d, e + 32)[0]
	out = []
	for i in range(n):
		p = off(struct.unpack_from('<I', d, off(names) + 4 * i)[0])
		out.append(d[p:d.index(b'\0', p)].decode())
	return out

@conf
def retail_win32_configure(conf):
	tdir, V = toolchain(conf)
	for p, what in ((os.path.join(V.VS, 'VC', 'bin', 'cl.exe'), 'fetch-msvc2013.py'),
			(os.path.join(V.WINE_BIN, 'wine'), 'setup-wine.py'),
			(os.path.join(V.PREFIX, 'system.reg'), 'setup-wine.py')):
		if not os.path.exists(p):
			conf.fatal('%s not found; run %s/%s (see %s/README.md)' % (p, TOOLCHAIN_DIR, what, TOOLCHAIN_DIR))

	v = conf.env
	shim = lambda t: ['/usr/bin/env', 'BYTEMATCH_MSVC2013=' + V.VS, sys.executable, os.path.join(tdir, 'msvc.py'), t]
	v.NO_MSVC_DETECT = 1
	v.MSVC_COMPILER = 'msvc'
	v.MSVC_VERSION = 12.0
	v.CC = v.CXX = shim('cl')
	v.LINK_CC = v.LINK_CXX = shim('link')
	v.AR = shim('lib')
	v.WINRC = shim('rc')
	v.RETAIL_ML = shim('ml')
	v.RETAIL_MC = shim('mc')
	v.RETAIL_WINMETA = os.path.join(V.VS, 'sdk71a', 'Include', 'winmeta.xml')
	v.RETAIL_COMPAT_H = os.path.join(tdir, 'msvc2013_compat.h')
	v.MT = ['true']  # waf's msvc tool insists on mt.exe; Valve's VPC sets $GenerateManifest "No"
	subprocess.call(shim('--start'))
	conf.load('msvc')
	# msvc.py supplies the toolchain's INCLUDE/LIB; the caller's must not leak in.
	v.INCLUDES = []
	v.LIBPATH = []
	v.MSVC_MANIFEST = False
	v.LINKFLAGS = [f for f in v.LINKFLAGS if f.upper() != '/MANIFEST']
	v.DEST_OS = 'win32'
	v.DEST_CPU = 'x86'
	v.DEST_BINFMT = 'pe'
	v.DEST_SIZEOF_VOID_P = 4
	v.COMPILER_CC = 'msvc'
	v.RETAIL_WIN32 = True
	v.WINRC_TGT_F = '/fo'
	v.WINRC_SRC_F = ''
	v.WINRCFLAGS = ['/nologo', '/l', '0x409']

	o = conf.options
	v.RETAIL_MANIFEST_WIN32 = conf.path.find_node(MANIFEST).abspath()
	m = load_manifest(v.RETAIL_MANIFEST_WIN32)['modules']
	want = o.RETAIL_WIN32_MODULES.split(',') if o.RETAIL_WIN32_MODULES else RETAIL_MODULES
	for w in want:
		if w not in m:
			conf.fatal('%s is not in %s (regenerate it with make-manifest.py %s)' % (w, MANIFEST, w))
	v.RETAIL_WIN32_MODULES = want

	# Import libraries for retail third-party DLLs, from their export tables.
	libdir = conf.bldnode.make_node('retail-win32-libs')
	libdir.mkdir()
	for name in DEPOT_IMPLIBS:
		dll = os.path.join(o.RETAIL_DEPOT222, 'bin', name + '.dll')
		if not os.path.exists(dll):
			Logs.warn('retail-win32: %s not found, no import library' % dll)
			continue
		deffile = libdir.make_node(name + '.def')
		deffile.write('LIBRARY %s.dll\nEXPORTS\n%s\n' % (name, '\n'.join('\t' + e for e in pe_exports(dll))))
		lib = libdir.make_node(name + '.lib')
		if conf.exec_command(v.AR + ['/nologo', '/MACHINE:X86', '/DEF:' + deffile.abspath(), '/OUT:' + lib.abspath()]):
			conf.fatal('lib /DEF failed for %s' % name)
	v.RETAIL_LIBPATH = [libdir.abspath()]
	for d in ('lib/public', 'lib/common'):
		p = os.path.join(o.SDK2013, d)
		if os.path.isdir(p):
			v.append_value('RETAIL_LIBPATH', [p])
	Logs.info('retail-win32: MSVC %s, wine %s, modules %s' % (V.VS, V.WINE_BIN, ','.join(want)))

def closure(m, names):
	out = []
	def visit(n):
		if n in out or n not in m:
			return
		for l in m[n]['libs']:
			if l.get('project'):
				visit(l['project'])
		out.append(n)
	for n in names:
		visit(n)
	return out

@conf
def retail_win32_build(bld):
	manifest = load_manifest(bld.env.RETAIL_MANIFEST_WIN32)['modules']
	for idx, name in enumerate(closure(manifest, bld.env.RETAIL_WIN32_MODULES)):
		retail_win32_module(bld, name, manifest[name], idx + 1)

def find_lib(bld, filename):
	for d in bld.env.RETAIL_LIBPATH:
		p = os.path.join(d, filename)
		if os.path.exists(p):
			return p
	return None

def retail_win32_module(bld, name, m, idx):
	src = bld.srcnode
	pdir = src.make_node(m['project_dir'])
	bdir = pdir.get_bld()
	bdir.mkdir()
	sources, files, mc = [], {}, None
	for s in m['sources']:
		if s.get('missing'):
			if s.get('pch') == 'create' and m.get('pch_header'):
				node = bdir.make_node(os.path.basename(s['path']))
				node.write('#include "%s"\n' % (s.get('pch_header') or m['pch_header']))
				Logs.warn('%s: %s is not in this tree; generated it from the PCH header' % (name, s['path']))
			else:
				Logs.warn('%s: %s is not in this tree, skipped' % (name, s['path']))
				continue
		elif s['lang'] == 'mc':
			node = src.find_node(s['path'])
			stem = os.path.splitext(node.name)[0]
			mc = bld(rule = '${RETAIL_MC} -W ${RETAIL_WINMETA} -um ${SRC[0].abspath()} -h ${TGT[0].parent.abspath()} -r ${TGT[0].parent.abspath()} -z %sEvents' % stem,
				source = node, target = [bdir.make_node(stem + 'Events.h')], shell = False, name = name + '-mc')
			continue
		else:
			node = src.find_node(s['path'])
		sources.append(node)
		files[node.abspath()] = s

	includes = [src.make_node(i) for i in m['includes']] + [bdir]
	cl = m['cl'] + ['/FS', '/Fd' + bdir.make_node('vc120.pdb').abspath()] if '/Zi' in m['cl'] else list(m['cl'])
	cl.append('/FI' + bld.env.RETAIL_COMPAT_H)
	use, ldflags = [], []
	for l in m['libs']:
		if l.get('project'):
			use.append(l['project'])
			continue
		fname = os.path.basename(l.get('prebuilt') or l.get('external'))
		if fname.endswith('.link'):
			Logs.warn('%s: %s is proprietary and not available, skipped' % (name, fname))
			continue
		p = find_lib(bld, fname)
		if p:
			ldflags.append(p)
		else:
			Logs.warn('%s: no %s to link' % (name, fname))
	linkflags = []
	if m['type'] != 'lib':
		linkflags = list(m['link']) + list(m['system_libs'])
		for obj in m['link_objects']:
			n = src.find_node(obj)
			if n:
				linkflags.append(n.abspath())
			else:
				Logs.warn('%s: %s is not available, linked without it' % (name, obj))

	features = {'dll': 'c cxx cxxshlib', 'lib': 'c cxx cxxstlib', 'exe': 'c cxx cxxprogram'}[m['type']]
	tg = bld(
		features = features + ' retail_win32',
		source   = sources,
		target   = m['output'],
		name     = name,
		path     = pdir,
		includes = includes,
		defines  = m['defines'] + RETAIL_DEFINES,
		use      = use,
		install_path = None,
		idx      = idx,
	)
	tg.retail_files = files
	tg.retail_pch = m.get('pch_header')
	tg.retail_mc = mc
	tg.env = bld.env.derive()
	tg.retail_base = list(tg.env.CXXFLAGS)
	tg.env.CFLAGS = tg.env.CFLAGS + cl
	tg.env.CXXFLAGS = tg.env.CXXFLAGS + cl
	tg.env.LINKFLAGS = tg.env.LINKFLAGS + linkflags
	tg.env.LDFLAGS = ldflags
	if m['type'] == 'lib':
		tg.env.ARFLAGS = ['/nologo', '/MACHINE:X86']

class retail_ml(Task.Task):
	run_str = '${RETAIL_ML} /nologo /safeseh /c /Cp /Zi /Fo${TGT[0].abspath()} ${SRC[0].abspath()}'
	color = 'BLUE'

@extension('.asm')
def retail_win32_asm(self, node):
	if not self.env.RETAIL_WIN32:
		raise Errors.WafError('.asm sources are only handled in --retail-win32 mode')
	out = node.change_ext('.%d.obj' % self.idx)
	t = self.create_task('retail_ml', node, out)
	try:
		self.compiled_tasks.append(t)
	except AttributeError:
		self.compiled_tasks = [t]

@feature('retail_win32')
@after_method('process_source')
def retail_win32_per_file(self):
	'''Per-file switches and precompiled headers, as the VPC's per-file $Configuration blocks.'''
	create, users = None, []
	pch = self.path.get_bld().make_node(self.name + '.pch')
	for t in getattr(self, 'compiled_tasks', []):
		s = self.retail_files.get(t.inputs[0].abspath())
		if s is None or s['lang'] not in ('c', 'c++'):
			continue
		flags = list(s.get('cl') or [])
		if flags:
			flags.append('/FI' + self.env.RETAIL_COMPAT_H)
			if '/Zi' in flags:
				flags += ['/FS', '/Fd' + self.path.get_bld().make_node('vc120.pdb').abspath()]
			t.env = t.env.derive()
			t.env.CFLAGS = t.env.CXXFLAGS = self.retail_base + flags
		if s.get('defines'):
			t.env = t.env.derive()
			t.env.DEFINES = s['defines'] + RETAIL_DEFINES
		mode = s.get('pch')
		if mode:
			hdr = s.get('pch_header') or self.retail_pch
			t.env = t.env.derive()
			if mode == 'create':
				t.env.append_value('CXXFLAGS', ['/Yc' + hdr, '/Fp' + pch.abspath()])
				t.env.append_value('CFLAGS', ['/Yc' + hdr, '/Fp' + pch.abspath()])
				t.outputs.append(pch)
				create = t
			else:
				t.env.append_value('CXXFLAGS', ['/Yu' + hdr, '/Fp' + pch.abspath()])
				t.env.append_value('CFLAGS', ['/Yu' + hdr, '/Fp' + pch.abspath()])
				users.append(t)
	for t in users:
		if create:
			t.set_run_after(create)
			t.dep_nodes.append(pch)
	gen = self.retail_mc
	if gen:
		gen.post()
		for t in self.compiled_tasks:
			for g in gen.tasks:
				t.set_run_after(g)
