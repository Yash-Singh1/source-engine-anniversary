"""User-supplied address -> name maps.

Accepted formats (auto-detected):
  * text:  one `ADDR NAME [SIZE]` per line, '#' comments; hex with or without 0x
  * JSON:  {"name": addr, ...}, [{"name":..., "addr":..., "size":...}, ...] or {"functions": [...]}
  * MSVC linker /MAP files (the "Publics by Value" table)
  * byte-match-survey symbol-port maps (symbols/out/<module>.win.json): MSVC and
    macOS names, signature aliases and the macOS object stem (see winmap.py)
Addresses below the image base are treated as RVAs; string addresses are hex."""
import json
import re

from . import winmap
from .build import rebuild
from .model import canonical

_MSVC = re.compile(r'^\s*[0-9a-f]{4}:[0-9a-f]{8}\s+(\S+)\s+([0-9a-f]{8})\s+(?:f\s+)?(?:i\s+)?(\S+)\s*$', re.I)


def _num(v):
    return int(v, 16) if isinstance(v, str) else int(v)


def read_map(path):
    text = open(path, encoding='utf-8', errors='replace').read()
    if text.lstrip()[:1] in ('[', '{'):
        doc = json.loads(text)
        if isinstance(doc, dict) and 'functions' in doc:
            doc = doc['functions']
        if isinstance(doc, dict):
            return [(_num(a), n, None) for n, a in doc.items()]
        return [(_num(d['addr']), d['name'], d.get('size')) for d in doc]
    out = []
    msvc = 'Publics by Value' in text
    for line in text.splitlines():
        if msvc:
            m = _MSVC.match(line)
            if m and m.group(1) != '___ImageBase':
                out.append((int(m.group(2), 16), m.group(1), None))
            continue
        line = line.split('#', 1)[0].strip()
        if not line:
            continue
        parts = line.split()
        size = int(parts[2], 0) if len(parts) > 2 else None
        out.append((int(parts[0], 16), parts[1], size))
    return out


# Symbol-port names below this confidence still scope functions but don't name relocation
# targets: 0.7 CRT object matches can't tell identical routines apart (memcpy/memmove), and
# 0.4 call-site groups are identical-function-folding guesses.
TARGET_MIN_CONF = 0.9


def apply_map(mod, path, min_conf=0.0):
    if path.endswith('.json'):
        doc = winmap.load(path)
        if winmap.is_symbol_port(doc):
            norm = []
            for addr, names, size, stem, conf in winmap.read_rows(doc, min_conf):
                if mod.image_base and addr < mod.image_base:
                    addr += mod.image_base
                if conf >= TARGET_MIN_CONF:
                    for n in names:
                        mod.add_name(addr, canonical(mod.fmt, n))
                norm.append((addr, names, size, stem))
            rebuild(mod, norm)
            return len(norm)
    norm = []
    for addr, name, size in read_map(path):
        if mod.image_base and addr < mod.image_base:
            addr += mod.image_base
        mod.add_name(addr, canonical(mod.fmt, name))
        norm.append((addr, name, size))
    rebuild(mod, norm)
    return len(norm)
