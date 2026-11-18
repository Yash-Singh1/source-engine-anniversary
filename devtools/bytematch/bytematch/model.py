"""Format-independent model: a module is a list of functions, each with its
bytes and the byte ranges that hold link-time-relocated values."""
import bisect
import os
import re
from dataclasses import dataclass, field


@dataclass
class Reloc:
    offset: int              # field offset inside the function
    size: int                # field width in bytes
    kind: str                # 'abs', 'pcrel', 'got', 'secrel', 'rebase', 'branch', 'heur'
    target: str = None       # canonical key of the referenced symbol, when known
    addend: int = 0          # value stored in the object at this field (REL-style)


@dataclass(eq=False)
class Function:
    name: str                # raw symbol name
    key: str                 # canonical name used to pair functions across modules
    addr: int                # VA in linked images, section-relative VA in objects
    data: bytes              # function bytes with trailing alignment padding removed
    section: str = ''
    obj: str = None          # source object stem ('convar' for .../convar.o, libtier1.a(convar.o), convar.cpp.1.o)
    obj_path: str = None     # full object path (retail N_OSO entry, or our file/archive member)
    local: bool = False      # file-local symbol (pairs only with a function from the same object stem)
    size_known: bool = True  # False when the extent is a guess (PE export with no following symbol)
    raw_size: int = 0        # extent before padding trim
    comdat: bool = False
    relocs: list = field(default_factory=list)
    aliases: list = field(default_factory=list)
    module: object = field(default=None, repr=False)

    @property
    def size(self):
        return len(self.data)

    @property
    def pair_key(self):
        return f'{self.key}@{self.obj}' if self.local and self.obj else self.key


class Module:
    """A parsed binary. `linked` is False for relocatable objects."""

    def __init__(self, path, fmt, linked, member=None):
        self.path = path
        self.member = member
        self.fmt = fmt                # 'macho', 'elf', 'coff' (object), 'pe'
        self.linked = linked
        self.functions = []
        self.image_base = 0
        self.image_end = 0
        self.code = []                # [(vaddr, bytes, section name)] executable regions
        self.notes = []
        self.build_inputs = []
        self.debug_map = []           # Mach-O N_OSO: [(object path, mtime)] in link order
        self.obj_paths = {}           # function addr -> object path from the debug map
        self._names = {}              # addr -> [keys] for relocation-target resolution
        self._starts = None

    @property
    def label(self):
        return f'{self.path}({self.member})' if self.member else self.path

    @property
    def pic(self):
        """Absolute addresses cannot be told from small constants in images based at 0."""
        return self.image_base < 0x10000

    def add_name(self, addr, key):
        lst = self._names.setdefault(addr, [])
        if key not in lst:
            lst.append(key)

    def resolve(self, addr):
        return self._names.get(addr, [])

    def read(self, addr, size):
        for va, data, _ in self.code:
            if va <= addr < va + len(data):
                return data[addr - va:addr - va + size]
        return b''

    def function_at(self, addr):
        if self._starts is None:
            self.functions.sort(key=lambda f: f.addr)
            self._starts = [f.addr for f in self.functions]
        i = bisect.bisect_right(self._starts, addr) - 1
        if i >= 0:
            f = self.functions[i]
            if f.addr <= addr < f.addr + max(f.raw_size, f.size, 1):
                return f
        return None


def object_stem(path):
    """Normalise an object path to the stem shared by retail stabs and our build outputs."""
    m = re.search(r'\(([^()]+)\)$', path)
    if m:
        path = m.group(1)
    base = os.path.basename(path.replace('\\', '/'))
    base = re.sub(r'\.(o|obj)$', '', base, flags=re.I)
    base = re.sub(r'\.\d+$', '', base)                       # waf: convar.cpp.1.o
    base = re.sub(r'\.(cpp|cxx|cc|c|mm|m|s|asm)$', '', base, flags=re.I)
    return base


_STDCALL = re.compile(r'@(\d{1,3})$')


def canonical(fmt, name):
    """Map a raw symbol name to the key used for pairing and target checks.

    Mach-O and i386 COFF prefix C-level names with '_'; MSVC also decorates
    stdcall/fastcall names with '@N'. Import thunks/pointers are folded onto
    the imported name."""
    if fmt == 'macho':
        for suf in ('$non_lazy_ptr', '$lazy_ptr', '$stub'):
            if name.endswith(suf):
                name = name[:-len(suf)]
        if name.startswith('L_'):
            name = name[1:]
        return name[1:] if name.startswith('_') else name
    if fmt in ('coff', 'pe'):
        if name.startswith('__imp_'):
            name = name[6:]
        if name.startswith('?'):
            return name
        if name[:1] in ('_', '@'):
            m = _STDCALL.search(name)
            if m and int(m.group(1)) % 4 == 0 and name.count('@') == (1 if name[0] == '_' else 2):
                name = name[:m.start()]
            return name[1:]
        return name
    if fmt == 'elf':
        return name.split('@', 1)[0] if '@' in name and not name.startswith('@') else name
    return name


def pick_primary(names):
    """Choose a display name among aliases: (is_global, raw name) pairs."""
    return sorted(names, key=lambda n: (not n[0], n[1][:1] in 'lL', len(n[1]), n[1]))[0][1]
