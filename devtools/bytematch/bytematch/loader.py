"""Detect binary formats and load modules, archives and directories."""
import os
import struct
import sys

from . import coff, elf, macho

OBJ_EXTS = ('.o', '.obj', '.a', '.lib', '.dylib', '.so', '.dll', '.exe', '.bundle')


def load_bytes(path, data, member=None):
    if macho.is_macho(data):
        return macho.load(path, data, member)
    if elf.is_elf(data):
        return elf.load(path, data, member)
    if coff.is_pe(data):
        return coff.load_pe(path, data, member)
    if coff.is_coff_obj(data):
        return coff.load_obj(path, data, member)
    raise ValueError('unrecognised format')


def ar_members(data):
    """Yield (name, bytes) for GNU, BSD and MSVC-style ar archives."""
    pos = 8
    longnames = b''
    while pos + 60 <= len(data):
        hdr = data[pos:pos + 60]
        name = hdr[:16].decode('latin-1').rstrip()
        size = int(hdr[48:58].decode().strip() or 0)
        body = data[pos + 60:pos + 60 + size]
        pos += 60 + size + (size & 1)
        if name.startswith('#1/'):
            n = int(name[3:])
            name, body = body[:n].rstrip(b'\0').decode('latin-1'), body[n:]
        elif name == '//':
            longnames = body
            continue
        elif name in ('/', '/SYM64/') or name.startswith('__.SYMDEF'):
            continue
        elif name.startswith('/') and name[1:].isdigit():
            o = int(name[1:])
            end = longnames.find(b'/\n', o)
            if end < 0:
                end = longnames.find(b'\0', o)
            name = longnames[o:end].decode('latin-1')
        name = name.rstrip('/')
        yield name, body


def load_path(path, quiet=False):
    """Load a file, archive or directory tree into a list of modules."""
    mods = []
    paths = []
    if os.path.isdir(path):
        for root, dirs, files in os.walk(path):
            dirs.sort()
            for f in sorted(files):
                if f.lower().endswith(OBJ_EXTS) or '.so.' in f:
                    paths.append(os.path.join(root, f))
    else:
        paths.append(path)
    errors = 0
    for p in paths:
        try:
            with open(p, 'rb') as fh:
                data = fh.read()
        except OSError as e:
            errors += 1
            if not quiet:
                print(f'warning: {p}: {e}', file=sys.stderr)
            continue
        if data[:8] == b'!<arch>\n':
            for name, body in ar_members(data):
                try:
                    mods.append(load_bytes(p, body, name))
                except (ValueError, struct.error, IndexError):
                    pass
            continue
        try:
            mods.append(load_bytes(p, data))
        except (ValueError, struct.error, IndexError) as e:
            errors += 1
            if not quiet or len(paths) == 1:
                print(f'warning: {p}: {e}', file=sys.stderr)
    if len(paths) == 1 and not mods:
        raise SystemExit(f'error: could not load {path}')
    return mods


def load_one(path):
    mods = load_path(path)
    if len(mods) != 1:
        raise SystemExit(f'error: {path} is not a single module ({len(mods)} loaded)')
    return mods[0]
