"""x86-32 helpers: padding trim, address-field detection and listing.

Capstone is optional. Without it, padding is trimmed with a byte-pattern
table, only relocation-derived masks are available, and diffs are hex rows."""
import os
import re
import struct

cs = None
if not os.environ.get('BYTEMATCH_NO_CAPSTONE'):
    try:
        import capstone as cs
        from capstone import x86 as csx86
    except ImportError:
        cs = None

_lite = _det = None
if cs:
    _lite = cs.Cs(cs.CS_ARCH_X86, cs.CS_MODE_32)
    _lite.skipdata = False
    _det = cs.Cs(cs.CS_ARCH_X86, cs.CS_MODE_32)
    _det.detail = True


def have_capstone():
    return cs is not None


# Encodings compilers and linkers use to fill alignment gaps (gas legacy lea/mov
# forms, Intel multi-byte NOPs, MSVC int3).
_PAD_SEQS = sorted({bytes.fromhex(h) for h in (
    '90', 'cc', '6690', '0f1f00', '0f1f4000', '0f1f440000', '660f1f440000',
    '0f1f8000000000', '0f1f840000000000', '660f1f840000000000',
    '662e0f1f840000000000', '66662e0f1f840000000000', '6666662e0f1f840000000000',
    '666666662e0f1f840000000000', '6666666666662e0f1f840000000000',
    '89f6', '8d7600', '8d742600', '8db600000000', '8db42600000000',
    '8dbf00000000', '8dbc2700000000', '89ff', '8d7f00')}, key=len, reverse=True)

_PAD_MNEM = re.compile(r'^(nop|int3)$')
_SELF_LEA = re.compile(r'^(\w+), (?:dword ptr )?\[\1(?: \+ eiz(?:\*1)?)?(?: \+ 0)?\]$')
_SELF_MOV = re.compile(r'^(\w+), \1$')


def _is_pad_insn(mnem, ops):
    if _PAD_MNEM.match(mnem):
        return True
    if mnem == 'lea' and _SELF_LEA.match(ops):
        return True
    if mnem in ('mov', 'xchg') and _SELF_MOV.match(ops):
        return True
    return False


def trim_padding(code):
    """Length of `code` without trailing alignment padding."""
    if not code:
        return 0
    if cs:
        end = pos = 0
        for addr, size, mnem, ops in _lite.disasm_lite(code, 0):
            pos = addr + size
            if not _is_pad_insn(mnem, ops):
                end = pos
        if pos < len(code):
            # undecodable tail (data or truncated code): keep it unless it is all padding bytes
            tail = code[pos:]
            if tail.strip(b'\x90\xcc'):
                end = len(code)
        return end
    n = len(code)
    while n:
        for p in _PAD_SEQS:
            if n >= len(p) and code[n - len(p):n] == p:
                # a lone trailing 0x90/0xcc may be an operand byte; only trust
                # single-byte pads when the byte before is also padding-like
                if len(p) == 1 and n >= 2 and code[n - 2] not in (0x90, 0xcc, 0xc3, 0x00):
                    return n
                n -= len(p)
                break
        else:
            return n
    return n


def skip_padding(code):
    """Length of the alignment padding at the start of `code`."""
    n = 0
    while n < len(code):
        if cs:
            ins = next(_lite.disasm_lite(code[n:n + 16], 0), None)
            if ins is None or not _is_pad_insn(ins[2], ins[3]):
                return n
            n += ins[1]
            continue
        p = next((p for p in _PAD_SEQS if code[n:n + len(p)] == p), None)
        if p is None:
            return n
        n += len(p)
    return n


_PREFIXES = {0x2e, 0x3e, 0xf2, 0xf3, 0x66, 0x26, 0x36, 0x64, 0x65}


def rel_branch_width(b):
    """Width of the relative displacement if `b` encodes a direct branch/call."""
    i = 0
    opsize16 = False
    while i < len(b) and b[i] in _PREFIXES:
        opsize16 |= b[i] == 0x66
        i += 1
    if i >= len(b):
        return 0
    op = b[i]
    if op in (0xe8, 0xe9):
        return 2 if opsize16 else 4
    if op == 0xeb or 0x70 <= op <= 0x7f or 0xe0 <= op <= 0xe3:
        return 1
    if op == 0x0f and i + 1 < len(b) and 0x80 <= b[i + 1] <= 0x8f:
        return 2 if opsize16 else 4
    return 0


def _sval(b):
    return int.from_bytes(b, 'little', signed=True)


def decode(code, base):
    if not cs:
        return []
    return list(_lite.disasm_lite(code, base))


def branch_fields(code, base):
    """[(field_offset, width, target_va, is_call)] for direct branches leaving [base, base+len)."""
    out = []
    if not cs:
        return out
    end = base + len(code)
    for addr, size, mnem, ops in _lite.disasm_lite(code, base):
        off = addr - base
        b = code[off:off + size]
        w = rel_branch_width(b)
        if not w:
            continue
        tgt = (addr + size + _sval(b[size - w:])) & 0xffffffff
        if not base <= tgt < end:
            out.append((off + size - w, w, tgt, mnem.startswith('call')))
    return out


def address_fields(code, base, image=None):
    """Heuristic address-bearing fields in linked code with no relocation info.

    Masks: displacements of branches leaving the function, disp32 relative to a
    PIC base register (call/pop or get_pc_thunk + add), and when `image`
    (lo, hi) is given, absolute disp32/imm32 values inside the image."""
    out = []
    if not cs:
        return out
    end = base + len(code)
    pic = set()
    pic_slots = set()                        # (base reg, disp) stack slots holding a PIC base
    after_call_next = False
    after_ext_call = False
    for ins in _det.disasm(code, base):
        off = ins.address - base
        b = bytes(ins.bytes)
        w = rel_branch_width(b)
        call_next = False
        ext_call = False
        if w:
            tgt = (ins.address + ins.size + _sval(b[-w:])) & 0xffffffff
            if not base <= tgt < end:
                out.append((off + ins.size - w, w, 'branch'))
                ext_call = ins.mnemonic.startswith('call')
            call_next = ins.mnemonic.startswith('call') and tgt == ins.address + ins.size
        else:
            ops = ins.operands
            if (ins.mnemonic == 'add' and after_ext_call and len(ops) == 2 and ops[0].type == csx86.X86_OP_REG
                    and ops[1].type == csx86.X86_OP_IMM and ins.imm_size == 4):
                pic.add(ops[0].reg)                  # gcc: call __x86.get_pc_thunk.REG; add REG, GOT-.
                out.append((off + ins.imm_offset, 4, 'pic'))
                after_call_next = after_ext_call = False
                continue
            for op in ops:
                if op.type == csx86.X86_OP_MEM and ins.disp_size == 4:
                    if op.mem.base in pic or op.mem.index in pic:
                        out.append((off + ins.disp_offset, 4, 'pic'))
                    elif image and image[0] <= (op.mem.disp & 0xffffffff) < image[1]:
                        out.append((off + ins.disp_offset, 4, 'heur'))
                elif op.type == csx86.X86_OP_IMM and ins.imm_size == 4 and image:
                    if image[0] <= (op.imm & 0xffffffff) < image[1]:
                        out.append((off + ins.imm_offset, 4, 'heur'))
            # Track registers / stack slots that hold the PIC base. Decoding is
            # linear with no control flow, so a register is never un-tracked:
            # an overwrite on one path says nothing about the others.
            if ins.mnemonic == 'pop' and after_call_next and ops and ops[0].type == csx86.X86_OP_REG:
                pic.add(ops[0].reg)
            elif ins.mnemonic == 'mov' and len(ops) == 2:
                dst, src = ops
                if dst.type == csx86.X86_OP_MEM and src.type == csx86.X86_OP_REG and src.reg in pic:
                    pic_slots.add((dst.mem.base, dst.mem.disp))
                elif dst.type == csx86.X86_OP_REG and (
                        (src.type == csx86.X86_OP_REG and src.reg in pic) or
                        (src.type == csx86.X86_OP_MEM and (src.mem.base, src.mem.disp) in pic_slots)):
                    pic.add(dst.reg)
        after_call_next = call_next
        after_ext_call = ext_call
    return out


def _replace_field(ops, code, fo, ph):
    val = int.from_bytes(code[fo:fo + 4], 'little')
    sv = val - (1 << 32) if val & 0x80000000 else val
    new = re.sub(rf'0x0*{val:x}\b', ph, ops, count=1)
    if new == ops and sv < 0:
        new = re.sub(rf'- 0x0*{-sv:x}\b', '+ ' + ph, ops, count=1)
    return new if new != ops else re.sub(r'0x[0-9a-f]{3,}', ph, ops)


def listing(code, base, masked, labels=None):
    """[(offset, bytes_hex, normalised_text, display_text)] for diffs.

    Masked operands become '<reloc>' / '<ext>' in the normalised text (used
    for matching) and the target name, when known, in the display text.
    Branches inside the function are shown as function-relative offsets."""
    labels = labels or {}
    rows = []
    if not cs:
        for off in range(0, len(code), 8):
            chunk = code[off:off + 8]
            txt = ' '.join('..' if (off + i) in masked else f'{c:02x}' for i, c in enumerate(chunk))
            rows.append((off, chunk.hex(), txt, txt))
        return rows
    pos = 0
    for addr, size, mnem, ops in _lite.disasm_lite(code, base):
        off = addr - base
        b = code[off:off + size]
        hit = [k for k in range(off, off + size) if k in masked]
        w = rel_branch_width(b)
        disp = ops
        if w:
            tgt = (addr + size + _sval(b[-w:])) & 0xffffffff
            if base <= tgt < base + len(code):
                ops = disp = f'.+{tgt - base:#x}'
            else:
                name = labels.get(off + size - w)
                ops, disp = '<ext>', f'<{name}>' if name else f'<ext {tgt:#x}>'
        elif hit:
            fo = hit[0]
            name = labels.get(fo)
            if fo + 4 <= off + size:
                disp = _replace_field(ops, code, fo, f'<{name}>' if name else '<reloc>')
                ops = _replace_field(ops, code, fo, '<reloc>')
            else:
                ops = disp = re.sub(r'0x[0-9a-f]+', '<reloc>', ops)
        rows.append((off, b.hex(), f'{mnem} {ops}'.strip(), f'{mnem} {disp}'.strip()))
        pos = off + size
    if pos < len(code):
        for off in range(pos, len(code), 8):
            t = '.byte ' + code[off:off + 8].hex(' ')
            rows.append((off, code[off:off + 8].hex(), t, t))
    return rows


def line_constant_diffs(a, b, base=0, lo=40, hi=50000):
    """For same-length code differing only in immediates that look like source
    line numbers (Assert's __LINE__), [(offset, value in a, value in b)]; else None."""
    if not cs or len(a) != len(b) or a == b:
        return None
    ia, ib = list(_det.disasm(a, base)), list(_det.disasm(b, base))
    if len(ia) != len(ib) or sum(i.size for i in ia) != len(a):
        return None
    out = []
    for x, y in zip(ia, ib):
        if x.bytes == y.bytes:
            continue
        if x.size != y.size or x.mnemonic != y.mnemonic or not x.imm_size or x.imm_offset != y.imm_offset:
            return None
        o, n = x.imm_offset, x.imm_size
        if x.bytes[:o] != y.bytes[:o] or x.bytes[o + n:] != y.bytes[o + n:]:
            return None
        va = int.from_bytes(bytes(x.bytes[o:o + n]), 'little')
        vb = int.from_bytes(bytes(y.bytes[o:o + n]), 'little')
        if not (lo <= va <= hi and lo <= vb <= hi):
            return None
        out.append((x.address - base + o, va, vb))
    return out or None


def u32(b, off=0):
    return struct.unpack_from('<I', b, off)[0]


def s32(b, off=0):
    return struct.unpack_from('<i', b, off)[0]
