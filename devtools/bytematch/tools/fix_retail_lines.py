#!/usr/bin/env python3
"""Recompute the `#line` directive after each BYTEMATCH_RETAIL gate.

A gate is `#ifdef BYTEMATCH_RETAIL` or `#ifndef BYTEMATCH_RETAIL`, an optional
`#else`, and `#endif`, followed by `#line N`. N is rewritten so that the lines
after the gate are numbered as if only the retail branch were in the file.
Gates without a following `#line` are left alone.

Usage: fix_retail_lines.py FILE...
"""
import re
import sys


def fix(path):
	raw = open(path, 'rb').read().decode('latin-1')
	nl = '\r\n' if '\r\n' in raw else '\n'
	src = raw.split(nl)
	out = []
	presumed = 1  # number of the line about to be appended
	i = 0
	changed = False
	while i < len(src):
		line = src[i]
		m = re.match(r'\s*#\s*(ifdef|ifndef)\s+BYTEMATCH_RETAIL\b', line)
		if m:
			start = presumed
			j = i + 1
			depth = 0
			branch = [[], []]  # lines under #ifdef / #else
			cur = 0
			while j < len(src):
				t = src[j]
				if re.match(r'\s*#\s*if', t):
					depth += 1
				elif re.match(r'\s*#\s*endif', t):
					if depth == 0:
						break
					depth -= 1
				elif depth == 0 and re.match(r'\s*#\s*else', t):
					cur = 1
					j += 1
					continue
				branch[cur].append(t)
				j += 1
			retail = branch[0] if m[1] == 'ifdef' else branch[1]
			out.extend(src[i:j + 1])
			presumed += j + 1 - i
			i = j + 1
			if i < len(src) and re.match(r'\s*#\s*line\s+\d+\s*$', src[i]):
				new = '#line %d' % (start + len(retail))
				changed |= src[i] != new
				out.append(new)
				presumed = start + len(retail)
				i += 1
			continue
		lm = re.match(r'\s*#\s*line\s+(\d+)', line)
		out.append(line)
		presumed = int(lm[1]) if lm else presumed + 1
		i += 1
	if changed:
		open(path, 'wb').write(nl.join(out).encode('latin-1'))
	return changed


if __name__ == '__main__':
	for p in sys.argv[1:]:
		if fix(p):
			print('fixed', p)
