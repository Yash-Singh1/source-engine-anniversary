"""Minimal 32-bit Mach-O reader (thin or fat, i386 slice) for the SDK tools."""
import struct

FAT_MAGIC = 0xCAFEBABE
MH_MAGIC = 0xFEEDFACE
CPU_TYPE_I386 = 7

LC_SEGMENT = 0x1
LC_SYMTAB = 0x2
LC_LOAD_DYLIB = 0xC
LC_ID_DYLIB = 0xD
LC_LOAD_WEAK_DYLIB = 0x80000018
LC_REEXPORT_DYLIB = 0x8000001F
LC_LAZY_LOAD_DYLIB = 0x20
LC_LOAD_UPWARD_DYLIB = 0x80000023
DYLIB_ORDINAL_CMDS = (LC_LOAD_DYLIB, LC_LOAD_WEAK_DYLIB, LC_REEXPORT_DYLIB,
                      LC_LAZY_LOAD_DYLIB, LC_LOAD_UPWARD_DYLIB)

N_STAB = 0xE0
N_PEXT = 0x10
N_TYPE = 0x0E
N_EXT = 0x01
N_UNDF = 0x0
N_ABS = 0x2
N_SECT = 0xE
N_INDR = 0xA
N_WEAK_REF = 0x40
N_WEAK_DEF = 0x80


def fmt_version(v):
    return f"{v >> 16}.{(v >> 8) & 0xFF}.{v & 0xFF}"


class Dylib:
    def __init__(self, cmd, name, current, compat):
        self.cmd = cmd
        self.name = name
        self.current = current
        self.compat = compat


class Symbol:
    def __init__(self, name, ntype, sect, desc, value, segname=None, indirect=None):
        self.name = name
        self.ntype = ntype
        self.sect = sect
        self.desc = desc
        self.value = value
        self.segname = segname
        self.indirect = indirect

    @property
    def kind(self):
        return self.ntype & N_TYPE

    @property
    def external(self):
        return bool(self.ntype & N_EXT) and not (self.ntype & N_PEXT) and not (self.ntype & N_STAB)

    @property
    def weak_def(self):
        return bool(self.desc & N_WEAK_DEF)

    @property
    def library_ordinal(self):
        return (self.desc >> 8) & 0xFF


class MachO:
    def __init__(self, path):
        self.path = path
        with open(path, "rb") as f:
            data = f.read()
        off = 0
        if struct.unpack_from(">I", data, 0)[0] == FAT_MAGIC:
            n = struct.unpack_from(">I", data, 4)[0]
            for i in range(n):
                cpu, _, o, _, _ = struct.unpack_from(">iiIII", data, 8 + 20 * i)
                if cpu == CPU_TYPE_I386:
                    off = o
                    break
            else:
                raise ValueError(f"{path}: no i386 slice")
        magic, cpu, _, self.filetype, ncmds, _, self.flags = struct.unpack_from("<7I", data, off)
        if magic != MH_MAGIC or cpu != CPU_TYPE_I386:
            raise ValueError(f"{path}: not i386 Mach-O")
        self.data = data
        self.base = off
        self.ncmds = ncmds
        self.commands = []        # (cmd, offset of command, size)
        self.id = None
        self.dylibs = []          # all ordinal-bearing dylib commands, in order
        self.sections = []        # (segname, sectname)
        self._section_hdr = []
        symoff = None
        p = off + 28
        for _ in range(ncmds):
            cmd, size = struct.unpack_from("<II", data, p)
            self.commands.append((cmd, p, size))
            if cmd == LC_SEGMENT:
                nsects = struct.unpack_from("<I", data, p + 48)[0]
                for s in range(nsects):
                    q = p + 56 + 68 * s
                    sect = data[q:q + 16].rstrip(b"\0").decode()
                    seg = data[q + 16:q + 32].rstrip(b"\0").decode()
                    self.sections.append((seg, sect))
                    self._section_hdr.append(q)
            elif cmd == LC_SYMTAB:
                symoff, nsyms, stroff, strsize = struct.unpack_from("<4I", data, p + 8)
            elif cmd == LC_ID_DYLIB or cmd in DYLIB_ORDINAL_CMDS:
                noff, _, cur, compat = struct.unpack_from("<4I", data, p + 8)
                name = data[p + noff:p + size].split(b"\0")[0].decode()
                d = Dylib(cmd, name, cur, compat)
                if cmd == LC_ID_DYLIB:
                    self.id = d
                else:
                    self.dylibs.append(d)
            p += size
        self.symbols = []
        if symoff is not None:
            strtab = data[off + stroff:off + stroff + strsize]

            def s(i):
                return strtab[i:strtab.index(b"\0", i)].decode("latin-1")

            for i in range(nsyms):
                strx, ntype, sect, desc, value = struct.unpack_from("<IBBHI", data, off + symoff + 12 * i)
                seg = None
                if (ntype & N_TYPE) == N_SECT and 0 < sect <= len(self.sections):
                    seg = self.sections[sect - 1][0]
                ind = s(value) if (ntype & N_TYPE) == N_INDR and value < strsize else None
                self.symbols.append(Symbol(s(strx), ntype, sect, desc, value, seg, ind))

    def exports(self):
        """Defined external symbols: name -> (kind, weak) with kind text/data/abs."""
        out = {}
        for sym in self.symbols:
            if not sym.external or sym.kind == N_UNDF:
                continue
            if sym.kind == N_ABS:
                kind = "abs"
            elif sym.segname and sym.segname != "__TEXT":
                kind = "data"
            else:
                kind = "text"
            out[sym.name] = (kind, sym.weak_def)
        return out

    def imports(self):
        """Undefined external symbols: name -> install name of the binding dylib."""
        out = {}
        for sym in self.symbols:
            if sym.ntype & N_STAB or sym.kind != N_UNDF or not (sym.ntype & N_EXT):
                continue
            o = sym.library_ordinal
            if 1 <= o <= len(self.dylibs):
                out[sym.name] = self.dylibs[o - 1].name
            else:
                out[sym.name] = None
        return out

    def reexports(self):
        return [d.name for d in self.dylibs if d.cmd == LC_REEXPORT_DYLIB]

    def section_align(self, i):
        return struct.unpack_from("<I", self.data, self._section_hdr[i] + 44)[0]

    def linkedit_layout(self):
        """[(name, file offset, size)] of LINKEDIT blobs, sorted by offset."""
        out = []
        for cmd, p, _ in self.commands:
            u = lambda i, n=1: struct.unpack_from("<%dI" % n, self.data, p + i)
            if cmd in (0x22, 0x80000022):  # LC_DYLD_INFO(_ONLY)
                v = u(8, 10)
                for i, name in enumerate(("rebase", "bind", "weak_bind", "lazy_bind", "export")):
                    out.append((name, v[2 * i], v[2 * i + 1]))
            elif cmd == LC_SYMTAB:
                symoff, nsyms, stroff, strsize = u(8, 4)
                out += [("symtab", symoff, nsyms * 12), ("strtab", stroff, strsize)]
            elif cmd == 0xB:  # LC_DYSYMTAB
                v = u(8, 18)
                out += [("indirect_syms", v[12], v[13] * 4), ("ext_rel", v[14], v[15] * 8),
                        ("loc_rel", v[16], v[17] * 8)]
            elif cmd in (0x26, 0x29, 0x1D):  # FUNCTION_STARTS, DATA_IN_CODE, CODE_SIGNATURE
                name = {0x26: "function_starts", 0x29: "data_in_code", 0x1D: "code_signature"}[cmd]
                off, size = u(8, 2)
                out.append((name, off, size))
        return sorted((o for o in out if o[2]), key=lambda o: o[1])

    def load_command_names(self):
        names = {0x1: "SEGMENT", 0x2: "SYMTAB", 0xB: "DYSYMTAB", 0xC: "LOAD_DYLIB", 0xD: "ID_DYLIB",
                 0x1B: "UUID", 0x22: "DYLD_INFO", 0x80000022: "DYLD_INFO_ONLY", 0x24: "VERSION_MIN_MACOSX",
                 0x26: "FUNCTION_STARTS", 0x29: "DATA_IN_CODE", 0x2A: "SOURCE_VERSION",
                 0x80000018: "LOAD_WEAK_DYLIB", 0x8000001F: "REEXPORT_DYLIB"}
        return [names.get(c, hex(c)) for c, _, _ in self.commands]


def is_public_location(path):
    """ld64's rule for implicitly linking a re-exported dylib (InputFiles.cpp)."""
    if path.startswith("/usr/lib/") and "/" not in path[9:]:
        return True
    pre = "/System/Library/Frameworks/"
    if path.startswith(pre):
        rest = path[len(pre):]
        dot = rest.find(".")
        if dot > 0:
            name = rest[:dot]
            return path.endswith("/" + name) and "/Frameworks/" not in rest
    return False
