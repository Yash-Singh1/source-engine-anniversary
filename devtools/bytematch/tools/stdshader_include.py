#!/usr/bin/env python3
"""Regenerate materialsystem/stdshaders/include/ for the --retail-osx32 build.

Retail stdshader_dx9 (build 19307283) compiles Valve's 2024 combo headers from
materialsystem/stdshaders/include/ (debug map N_SOL). The closest public copy is
the Team Fortress 2 branch of Source SDK 2013 (origin/master, 0759e2e8, Feb 2025),
which shares the TF/HL2 code branch. This copies those headers and applies the
HL2 differences:

- TF2-only axes that retail never sets (no stringified SET_*_COMBO name in the
  dylib, and absent from the HL2 VCS combo counts) are removed;
- HL2's vertexlit vertex shaders keep the single STATIC_LIGHT axis where TF2
  has STATIC_LIGHT_VERTEX and STATIC_LIGHT_LIGHTMAP;
- old-format headers whose only difference is such an axis are omitted, so the
  build falls back to fxctmp9/, which already has the HL2 layout.

Usage: stdshader_include.py [SDK2013_CHECKOUT] (default ../source-sdk-2013)
"""
import os
import re
import subprocess
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..'))
OUT = os.path.join(ROOT, 'materialsystem', 'stdshaders', 'include')
REV = '0759e2e8'
SRC = 'src/materialsystem/stdshaders/include'

# Old-format TF2 headers that only add an axis HL2 lacks: fxctmp9 has the HL2 form.
OMIT = ['depthwrite_vs20', 'depthwrite_vs30', 'vertexlit_and_unlit_generic_ps20']

# Bitfield-format headers: axes to drop, axes to rename.
EDITS = {
	'skin_ps20b': ({'PHONG_USE_EXPONENT_FACTOR'}, {}),
	'skin_ps30': ({'PHONG_USE_EXPONENT_FACTOR'}, {}),
	'vertexlit_and_unlit_generic_ps20b': ({'STATIC_LIGHT_LIGHTMAP', 'DEBUG_LUXELS'}, {}),
	'vertexlit_and_unlit_generic_ps30': ({'STATIC_LIGHT_LIGHTMAP', 'DEBUG_LUXELS'}, {}),
	'VertexLit_and_unlit_Generic_vs20': ({'TREESWAY', 'STATIC_LIGHT_LIGHTMAP'}, {'STATIC_LIGHT_VERTEX': 'STATIC_LIGHT'}),
	'vertexlit_and_unlit_generic_vs30': ({'TREESWAY', 'STATIC_LIGHT_LIGHTMAP'}, {'STATIC_LIGHT_VERTEX': 'STATIC_LIGHT'}),
}


def mentions(line, names):
	return any(re.search(r'\b(m_[nb]|\$|Set|_)?%s\b' % n, line) for n in names)


def edit_class(body, drop, rename, base):
	"""Edit one bitfield-format *_Index class; returns (body, combo count)."""
	for old, new in rename.items():
		body = re.sub(r'\b(m_[nb]|Set)%s\b' % old, r'\g<1>' + new, body)
	# setter blocks
	for n in drop:
		body = re.sub(r'\tvoid Set%s\( int i \)\n\t\{\n.*?\n\t\}\n\n' % n, '', body, flags=re.S)
	lines = [l for l in body.split('\n')
		if not (re.match(r'\s*(unsigned int|bool) m_[nb]\w+ : \d+;', l) and mentions(l, drop))
		and not (re.match(r'\s*m_[nb]\w+ = ', l) and mentions(l, drop))
		and not (l.lstrip().startswith('AssertMsg') and mentions(l, drop))]
	body = '\n'.join(lines)
	body = re.sub(r'Assert\( (m_b\w+(?: && m_b\w+)*) \);',
		lambda m: 'Assert( %s );' % ' && '.join(x for x in m[1].split(' && ') if x[3:] not in drop), body)
	ranges = [(a, int(lo), int(hi)) for a, lo, hi in
		re.findall(r'void Set(\w+)\( int i \)\n\t\{\n\t\tAssert\( i >= (-?\d+) && i <= (-?\d+) \);', body)]
	order = re.findall(r'\* m_n(\w+) \)', re.search(r'return (.*?);', body)[1])
	order = [rename.get(a, a) for a in order if a not in drop]
	rng = dict((a, (lo, hi)) for a, lo, hi in ranges)
	mul, terms = base, []
	for a in order:
		terms.append('( %d * m_n%s )' % (mul, a))
		mul *= rng[a][1] - rng[a][0] + 1
	body = re.sub(r'return .*?;', 'return ' + ' + '.join(terms + ['0']) + ';', body, count=1)
	return body, mul // base


def edit(text, name, drop, rename):
	lines = text.split('\n')
	text = '\n'.join(l for l in lines if not (l.startswith('// ') and mentions(l, drop)))
	for old, new in rename.items():
		text = re.sub(r'\b(psh|vsh)_forgot_to_set_(static|dynamic)_%s\b' % old, r'\1_forgot_to_set_\2_' + new, text)
	for n in drop:
		text = re.sub(r' \+ (psh|vsh)_forgot_to_set_(static|dynamic)_%s\b' % n, '', text)
	dyn = re.search(r'(class %s_Dynamic_Index\n\{.*?\n\};)' % name, text, re.S)
	body, count = edit_class(dyn[1], drop, rename, 1)
	text = text.replace(dyn[1], body)
	sta = re.search(r'(class %s_Static_Index\n\{.*?\n\};)' % name, text, re.S)
	body, _ = edit_class(sta[1], drop, rename, count)
	return text.replace(sta[1], body)


def main():
	sdk = sys.argv[1] if len(sys.argv) > 1 else os.path.join(ROOT, '..', 'source-sdk-2013')
	names = subprocess.check_output(['git', '-C', sdk, 'ls-tree', '--name-only', REV, SRC + '/'], text=True).split()
	os.makedirs(OUT, exist_ok=True)
	for old in os.listdir(OUT):
		os.unlink(os.path.join(OUT, old))
	count = 0
	for path in names:
		base = os.path.basename(path)
		name = base[:-len('.inc')]
		if name in OMIT:
			continue
		text = subprocess.check_output(['git', '-C', sdk, 'show', '%s:%s' % (REV, path)]).decode('latin-1').replace('\r\n', '\n')
		if name in EDITS:
			text = edit(text, name.lower() if name.startswith('VertexLit') else name, *EDITS[name])
		with open(os.path.join(OUT, base), 'w', encoding='latin-1', newline='\n') as f:
			f.write(text)
		count += 1
	print('wrote %d headers to %s' % (count, os.path.relpath(OUT, ROOT)))


if __name__ == '__main__':
	main()
