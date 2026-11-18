#!/usr/bin/env python
# encoding: utf-8
'''
Order a module's sources and static libraries like Valve's retail macOS build.

devtools/bytematch/manifests/retail-osx32.json records, per retail module, the
link order of its objects and archives (from the binaries' N_OSO debug map).
With --retail-manifest, wscripts that call bld.retail_manifest() get their
source list reordered to that link order, files retail doesn't link dropped,
files retail takes from an archive moved after the loose objects, and
static-library `use` entries put in retail archive order. Without the option
the call returns its arguments unchanged.

Usage in a wscript:
	source, libs = bld.retail_manifest('bin/scenefilecache', source, libs)

--retail-osx32 sets env.RETAIL_SELECT, which applies the manifest to every
task generator that has an entry, without wscript changes:
  - a module's sources become retail's loose objects, in link order, mapped to
    our tree (files retail has under another path, e.g. engine/audio/private,
    resolve to ours); files retail doesn't link are dropped and files our
    wscript lacks are added;
  - an archive's sources are put in retail member order (ld64 lays out archive
    members by their position in the archive), with members retail never
    pulled kept after them;
  - static libraries retail links are added to `use`, and the link line's
    static libraries are put in retail link order.
A task generator is matched by its `retail_module` attribute, else by the
manifest's waf target, else by bin/<target>. Per-module exceptions live in
RETAIL_KEEP / RETAIL_SKIP below.
'''

import json, os
from waflib import Logs
from waflib.Configure import conf
from waflib.TaskGen import feature, before_method, after_method

MANIFEST = 'devtools/bytematch/manifests/retail-osx32.json'
_cache = {}

# Archives without a waf target in the manifest, and the target that builds them.
ARCHIVE_TARGETS = {
	'libreplay_common.a': 'replay_common',
	'libcryptopp.a': 'CRYPTOPP',
	'libspeex.a': 'SPEEX',
}

# Retail links objects for these, but our copy can't be compiled for this target;
# the module keeps building without them. module or archive -> files (tree-relative).
RETAIL_SKIP = {
	# Our copies lost the tag filter and friends/spectator pages they need from BaseGamesPage.
	'bin/ServerBrowser': ['serverbrowser/unused/BlacklistedServers.cpp', 'serverbrowser/unused/CustomGames.cpp',
		'serverbrowser/unused/FriendsGames.cpp', 'serverbrowser/unused/SpectateGames.cpp'],
	# Written against the linuxwin GLMDisplayDB; only launcher pulls it (ToGL lane).
	'libappframework.a': ['appframework/glmrendererinfo_osx.mm'],
}

# Files retail doesn't link that our tree still needs to link the module
# (code that retail has in a file we lack). module -> files (tree-relative).
RETAIL_KEEP = {
	# CInputSystem still declares the touch API.
	'bin/inputsystem': ['inputsystem/touch_sdl.cpp'],
	# Our CBaseFileSystem uses the fork's CPackFile classes.
	'bin/filesystem_stdio': ['filesystem/packfile.cpp'],
	# Our vtex.cpp loads PNG sources through ImgUtl.
	'bin/vtex_dll': ['common/imageutils.cpp'],
	# Callers in our engine that retail's engine doesn't have.
	'bin/engine': ['engine/sv_uploaddata.cpp', 'common/language.cpp', 'engine/audio/MPAHeader.cpp',
		'engine/audio/VBRHeader.cpp', 'engine/masterserver.cpp'],
}

def options(opt):
	grp = opt.add_option_group('Byte-match options')
	grp.add_option('--retail-manifest', action = 'store_true', dest = 'RETAIL_MANIFEST', default = False,
		help = 'order sources and static libraries per %s [default: %%default]' % MANIFEST)

def configure(conf):
	if conf.options.RETAIL_MANIFEST:
		conf.env.RETAIL_MANIFEST = conf.srcnode.make_node(MANIFEST).abspath()

def _load(path):
	if path not in _cache:
		with open(path) as f:
			_cache[path] = json.load(f)
	return _cache[path]

@conf
def retail_manifest(ctx, module, source, use):
	if not ctx.env.RETAIL_MANIFEST or ctx.env.RETAIL_SELECT:
		return source, use

	mod = _load(ctx.env.RETAIL_MANIFEST)['modules'][module]
	here = ctx.path.path_from(ctx.srcnode)
	to_root = lambda s: os.path.normpath(os.path.join(here, s))
	ours = dict((to_root(s).lower(), s) for s in source)

	ordered = [ours[s.lower()] for s in mod['sources'] if s.lower() in ours]
	for obj in mod['objects']:
		src = (obj['source'] or '').lower()
		if obj['archive'] and src in ours and ours[src] not in ordered:
			ordered.append(ours[src])

	for s in source:
		if s not in ordered:
			Logs.warn('%s: retail links no object for %s, dropped' % (module, to_root(s)))
	for s in mod['missing_sources']:
		Logs.warn('%s: retail compiles %s, which is not in this tree' % (module, s))

	retail_libs = [a['waf_target'] for a in mod['archives'] if a['waf_target']]
	slots = iter([u for u in retail_libs if u in use])
	use = [next(slots) if u in retail_libs else u for u in use]

	return ordered, use

def lookup(tg):
	'''(kind, key, entry) for a task generator: kind is 'module', 'archive' or None.'''
	path = tg.env.RETAIL_MANIFEST
	if not path:
		return None, None, None
	m = _load(path)
	key = getattr(tg, 'retail_module', None)
	if key:
		return 'module', key, m['modules'][key]
	names = [n for n in (getattr(tg, 'name', None), os.path.basename(str(tg.target))) if n]
	for name in names:
		for k, mod in m['modules'].items():
			if mod.get('waf_target') in (name, name.lower()) and mod['retail'].get('filetype') in ('MH_DYLIB', 'MH_EXECUTE'):
				return 'module', k, mod
	for name in names:
		if 'bin/' + name in m['modules']:
			return 'module', 'bin/' + name, m['modules']['bin/' + name]
	for name in names:
		for k, a in m['archives'].items():
			if a.get('waf_target') in (name, name.lower()) or ARCHIVE_TARGETS.get(k) == name:
				return 'archive', k, a
	return None, None, None

def archive_target(name, archive):
	return archive.get('waf_target') or ARCHIVE_TARGETS.get(name)

def _nodes(tg):
	nodes = []
	for x in tg.to_list(tg.source):
		n = x if hasattr(x, 'abspath') else tg.path.find_resource(x)
		if n:
			nodes.append(n)
		else:
			nodes.append(x)  # let process_source report it
	return nodes

def _rel(tg, n):
	return n.path_from(tg.bld.srcnode) if hasattr(n, 'abspath') else str(n)

@feature('c', 'cxx')
@before_method('process_source')
def retail_select_sources(self):
	if not self.env.RETAIL_SELECT or getattr(self, 'retail_selected', False):
		return
	self.retail_selected = True
	kind, key, e = lookup(self)
	if not kind:
		return
	m = _load(self.env.RETAIL_MANIFEST)
	srcnode = self.bld.srcnode
	ours = _nodes(self)
	ours_by_path = dict((_rel(self, n).lower(), n) for n in ours)

	def resolve(path):
		n = ours_by_path.get(path.lower())
		if n is None:
			n = srcnode.find_resource(path)
		return n

	skip = set(s.lower() for s in RETAIL_SKIP.get(key, []))
	if kind == 'archive':
		ours = [n for n in ours if _rel(self, n).lower() not in skip]
		out = []
		for mem in e['members']:
			if mem['source'] and mem['source'].lower() not in skip:
				n = ours_by_path.get(mem['source'].lower())
				if n is not None and n not in out:
					out.append(n)
		self.source = out + [n for n in ours if n not in out]
		return

	keep = set(s.lower() for s in RETAIL_KEEP.get(key, []))
	built = set(k for k, a in m['archives'].items() if archive_target(k, a))
	out, missing = [], []
	for obj in e['objects']:
		if obj['archive'] and obj['archive'] in built:
			continue
		src = obj['source'] or obj['retail_source']
		if not src:
			missing.append(obj.get('member'))
			continue
		if src.lower() in skip:
			continue
		n = resolve(src)
		if n is None:
			missing.append(src)
		elif n not in out:
			out.append(n)
	added = [n for n in out if _rel(self, n).lower() not in ours_by_path]
	dropped = [n for n in ours if n not in out]
	out += [n for n in dropped if _rel(self, n).lower() in keep]
	dropped = [n for n in dropped if _rel(self, n).lower() not in keep]
	if missing:
		Logs.warn('%s: retail compiles %d file(s) not in this tree: %s' % (key, len(missing), ', '.join(missing)))
	Logs.debug('retail: %s: %d sources, %d added, %d dropped (%s)' % (key, len(out), len(added),
		len(dropped), ', '.join(_rel(self, n) for n in dropped)))
	self.source = out
	self.retail_key = key

	use = self.to_list(getattr(self, 'use', []))
	for k in [a['path'].split('/')[-1] for a in e['archives']]:
		t = archive_target(k, m['archives'][k])
		if not t or t in use:
			continue
		if t.isupper():
			use.append(t)
			continue
		try:
			self.bld.get_tgen_by_name(t)
		except Exception:
			Logs.warn('%s: retail links %s, which this build has no target for' % (key, k))
			continue
		use.append(t)
	self.use = use

@feature('cshlib', 'cxxshlib', 'cprogram', 'cxxprogram')
@after_method('propagate_uselib_vars')
def retail_stlib_order(self):
	'''Put the link line's static libraries in retail link order.'''
	if not self.env.RETAIL_SELECT or not getattr(self, 'link_task', None):
		return
	kind, key, e = lookup(self)
	if kind != 'module':
		return
	m = _load(self.env.RETAIL_MANIFEST)
	order = []
	for a in e['archives']:
		k = a['path'].split('/')[-1]
		order.append(k[3:-2])
		t = archive_target(k, m['archives'][k])
		if t and not t.isupper():
			try:
				tg = self.bld.get_tgen_by_name(t)
				order.append(os.path.basename(str(tg.target)))
			except Exception:
				pass
	env = self.link_task.env
	libs = list(env.STLIB)
	rank = lambda l: order.index(l) if l in order else len(order)
	env.STLIB = sorted(libs, key = rank)
