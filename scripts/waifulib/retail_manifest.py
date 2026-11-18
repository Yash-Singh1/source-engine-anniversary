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
'''

import json, os
from waflib import Logs
from waflib.Configure import conf

MANIFEST = 'devtools/bytematch/manifests/retail-osx32.json'
_cache = {}

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
	if not ctx.env.RETAIL_MANIFEST:
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

@conf
def retail_manifest_sources(ctx, module):
	'''The module's retail loose objects as sources relative to this wscript, in link order.'''
	mod = _load(ctx.env.RETAIL_MANIFEST)['modules'][module]
	here = ctx.path.path_from(ctx.srcnode)
	missing = set(mod['missing_sources'])
	return [os.path.relpath(s, here) for s in mod['sources'] if s not in missing]
