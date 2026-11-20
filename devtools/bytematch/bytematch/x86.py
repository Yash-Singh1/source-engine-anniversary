"""x86-32 helpers: padding trim, address-field detection and listing.

Capstone is optional. Without it, padding is trimmed with a byte-pattern
table, only relocation-derived masks are available, and diffs are hex rows."""
import bisect
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


_REG32 = {'eax', 'ebx', 'ecx', 'edx', 'esi', 'edi', 'ebp', 'esp'}
_SUBREG = {'al': 'eax', 'ah': 'eax', 'ax': 'eax', 'bl': 'ebx', 'bh': 'ebx', 'bx': 'ebx',
           'cl': 'ecx', 'ch': 'ecx', 'cx': 'ecx', 'dl': 'edx', 'dh': 'edx', 'dx': 'edx',
           'si': 'esi', 'di': 'edi', 'bp': 'ebp', 'sp': 'esp'}
_CALLEE_SAVED = {'ebx', 'esi', 'edi'}
_NOWRITE = re.compile(r'^(cmp|test|push|j\w+|call|ret\w*|u?comis[sd]|bt|nop|cmps\w*|scas\w*|fst\w*|fist\w*|fcom\w*|'
                      r'movs[bwd]|stos\w*|lods\w*|leave|int3|ud2|out|prefetch\w*)$')
_MEM = re.compile(r'(?:(byte|word|dword|qword|xmmword|tbyte|ymmword) ptr )?(?:[a-z]s:)?\[([^\]]+)\]')
_REG_IDS = {r: getattr(csx86, 'X86_REG_' + r.upper()) for r in _REG32} if cs else {}
PIC_GOT = object()
_SLOT_CLOB = object()


def parse_mem(expr):
    """'ebx + ecx*4 + 0x10' -> (base, index, scale, disp)."""
    base = index = None
    scale, disp = 1, 0
    for sign, term in re.findall(r'([+-]?)\s*([^+\-\s]+)', expr.replace(' - ', ' + -').replace('+ -', '-')):
        neg = sign == '-'
        if '*' in term:
            index, sc = term.split('*')
            scale = int(sc)
        elif term in _REG32:
            if base is None:
                base = term
            else:
                index = term
        else:
            try:
                v = int(term, 0)
            except ValueError:
                return None
            disp += -v if neg else v
    return base, index, scale, disp


def pic_states(code, base, allow_got=True):
    """CFG-aware PIC base register tracking for x86-32 functions.

    Returns (insns, states, was_pic) where `insns` is `list(_lite.disasm_lite(code, base))`,
    `states[i]` is `{reg_name: pic_base_va_or_PIC_GOT}` valid immediately before `insns[i]`,
    and `was_pic` is the set of registers that ever held a PIC base in this function.

    Uses intersection meet for registers so a non-PIC register on any reachable
    path is never masked as PIC, while backward edges and out-of-order basic blocks
    (e.g. a loop body placed before its header) propagate PIC registers cleanly."""
    if not cs:
        return [], [], set()
    insns = list(_lite.disasm_lite(code, base))
    n = len(insns)
    if not n:
        return insns, [], set()
    end = base + len(code)

    # Fast path: functions without `call +5` or an external call have no PIC base.
    has_seed = False
    for addr, size, mnem, ops in insns:
        if mnem == 'call':
            off = addr - base
            b = code[off:off + size]
            w = rel_branch_width(b)
            if w:
                tgt = (addr + size + _sval(b[-w:])) & 0xffffffff
                if tgt == addr + size or (allow_got and not (base <= tgt < end)):
                    has_seed = True
                    break
    if not has_seed:
        return insns, [{} for _ in range(n)], set()

    addr_to_idx = {insns[i][0]: i for i in range(n)}
    leaders = {0}
    branch_targets = set()
    for i, (addr, size, mnem, ops) in enumerate(insns):
        off = addr - base
        if mnem.startswith(('j', 'loop')):
            w = rel_branch_width(code[off:off + size])
            if w:
                tgt = (addr + size + _sval(code[off + size - w:off + size])) & 0xffffffff
                ti = addr_to_idx.get(tgt)
                if ti is not None:
                    leaders.add(ti)
                    branch_targets.add(ti)
            if i + 1 < n:
                leaders.add(i + 1)
        elif mnem in ('ret', 'retf', 'ud2', 'int3', 'hlt'):
            if i + 1 < n:
                leaders.add(i + 1)
        elif mnem == 'call' and i + 1 < n and (
                (i > 0 and insns[i - 1][2] in ('ret', 'retf')) or
                (i > 1 and insns[i - 1][2] == 'fstp' and insns[i - 2][2] in ('ret', 'retf'))):
            w = rel_branch_width(code[off:off + size])
            if w:
                tgt = (addr + size + _sval(code[off + size - w:off + size])) & 0xffffffff
                if not (base <= tgt < end):
                    leaders.add(i + 1)
    sleaders = sorted(leaders)
    blocks = [(sleaders[k], sleaders[k + 1] if k + 1 < len(sleaders) else n) for k in range(len(sleaders))]
    idx_to_blk = {lo: bi for bi, (lo, hi) in enumerate(blocks)}
    nb = len(blocks)
    is_pad_blk = [all(_is_pad_insn(insns[i][2], insns[i][3]) for i in range(lo, hi)) for lo, hi in blocks]

    succs = [[] for _ in range(nb)]
    preds = [[] for _ in range(nb)]
    ind_jmps = []
    for bi, (lo, hi) in enumerate(blocks):
        addr, size, mnem, ops = insns[hi - 1]
        off = addr - base
        if mnem == 'jmp':
            w = rel_branch_width(code[off:off + size])
            if w:
                tgt = (addr + size + _sval(code[off + size - w:off + size])) & 0xffffffff
                ti = addr_to_idx.get(tgt)
                if ti is not None:
                    succs[bi].append(idx_to_blk[ti])
            else:
                ind_jmps.append(bi)
        elif mnem.startswith(('j', 'loop')):
            w = rel_branch_width(code[off:off + size])
            if w:
                tgt = (addr + size + _sval(code[off + size - w:off + size])) & 0xffffffff
                ti = addr_to_idx.get(tgt)
                if ti is not None:
                    succs[bi].append(idx_to_blk[ti])
            if bi + 1 < nb:
                succs[bi].append(bi + 1)
        elif mnem in ('ret', 'retf', 'ud2', 'int3', 'hlt'):
            pass
        else:
            # A noreturn epilogue stub right after `ret` (`call ___stack_chk_fail`, optionally
            # preceded by `fstp st(0)` in float-returning functions) does not fall through.
            w = rel_branch_width(code[off:off + size]) if mnem == 'call' else 0
            tgt = (addr + size + _sval(code[off + size - w:off + size])) & 0xffffffff if w else None
            noreturn_stub = (
                mnem == 'call' and bi > 0
                and (hi - lo == 1 or (hi - lo == 2 and insns[lo][2] == 'fstp'))
                and tgt is not None and not (base <= tgt < end)
                and insns[blocks[bi - 1][1] - 1][2] in ('ret', 'retf')
            )
            if bi + 1 < nb and not noreturn_stub:
                succs[bi].append(bi + 1)
    for bi, ss in enumerate(succs):
        for s in ss:
            preds[s].append(bi)

    was_pic = set()

    def run_block(bi, in_pic, in_slots, record=None):
        lo, hi = blocks[bi]
        pic, slots = dict(in_pic), dict(in_slots)
        pending = None
        after_ext = False
        call_states = []
        for i in range(lo, hi):
            addr, size, mnem, ops = insns[i]
            if record is not None:
                record[i] = dict(pic)
            off = addr - base
            nxt = addr + size
            if mnem == 'call':
                b = code[off:off + size]
                w = rel_branch_width(b)
                tgt = (nxt + _sval(b[-w:])) & 0xffffffff if w else None
                if tgt == nxt:
                    pending = nxt
                    after_ext = False
                else:
                    call_states.append(({k: v for k, v in pic.items() if k in _CALLEE_SAVED}, dict(slots)))
                    for r in ('eax', 'ecx', 'edx'):
                        pic.pop(r, None)
                    pending = None
                    after_ext = bool(w and not (base <= tgt < end))
                continue
            if mnem == 'pop' and pending is not None and ops in _REG32:
                pic[ops] = pending
                was_pic.add(ops)
                pending = None
                after_ext = False
                continue
            if allow_got and mnem == 'add' and after_ext and size >= 5:
                parts = ops.split(', ', 1)
                if len(parts) == 2 and parts[0] in _REG32:
                    try:
                        int(parts[1], 0)
                        pic[parts[0]] = PIC_GOT
                        was_pic.add(parts[0])
                        pending = None
                        after_ext = False
                        continue
                    except ValueError:
                        pass
            pending = None
            after_ext = False
            if not ops:
                if mnem == 'cdq':
                    pic.pop('edx', None)
                elif mnem == 'leave':
                    pic.pop('ebp', None)
                    pic.pop('esp', None)
                continue
            parts = ops.split(', ', 1)
            dst = parts[0]
            src = parts[1] if len(parts) > 1 else ''
            if mnem == 'mov' and '[' in dst:
                m = _MEM.search(dst)
                pm = parse_mem(m.group(2)) if m else None
                if pm and pm[1] is None and pm[0] in _REG32:
                    key = (pm[0], pm[3])
                    slots[key] = pic[src] if src in pic else _SLOT_CLOB
                continue
            dreg = _SUBREG.get(dst, dst)
            if dreg in _REG32:
                if mnem == 'mov':
                    if src in pic:
                        pic[dreg] = pic[src]
                        was_pic.add(dreg)
                        continue
                    if '[' in src:
                        m = _MEM.search(src)
                        pm = parse_mem(m.group(2)) if m else None
                        if pm and pm[1] is None:
                            sv = slots.get((pm[0], pm[3]))
                            if sv is not None and sv is not _SLOT_CLOB:
                                pic[dreg] = sv
                                was_pic.add(dreg)
                                continue
                elif mnem == 'xchg' and src in _REG32:
                    vd, vs = pic.get(dreg), pic.get(src)
                    if vs is not None:
                        pic[dreg] = vs
                        was_pic.add(dreg)
                    else:
                        pic.pop(dreg, None)
                    if vd is not None:
                        pic[src] = vd
                        was_pic.add(src)
                    else:
                        pic.pop(src, None)
                    continue
                if not _NOWRITE.match(mnem):
                    pic.pop(dreg, None)
            if mnem in ('mul', 'div', 'idiv') or (mnem == 'imul' and ',' not in ops):
                pic.pop('eax', None)
                pic.pop('edx', None)
        return pic, slots, call_states

    def meet(a, b):
        if a is None:
            return (dict(b[0]), dict(b[1]))
        p1, s1 = a
        p2, s2 = b
        np = {k: v for k, v in p1.items() if p2.get(k) == v}
        ns = dict(s1)
        for k, v2 in s2.items():
            if k not in ns:
                ns[k] = v2
            elif ns[k] != v2:
                ns[k] = _SLOT_CLOB
        return (np, ns)

    in_state = [None] * nb
    out_state = [None] * nb
    all_calls = {}
    in_state[0] = ({}, {})

    def solve(work, allowed=None):
        in_q = set(work)
        while work:
            bi = work.pop(0)
            in_q.discard(bi)
            op, os_, cs_ = run_block(bi, *in_state[bi])
            out_state[bi] = (op, os_)
            all_calls[bi] = cs_
            for s in succs[bi]:
                if allowed is not None and s not in allowed:
                    continue
                ns = meet(in_state[s], (op, os_))
                if in_state[s] != ns:
                    in_state[s] = ns
                    if s not in in_q:
                        work.append(s)
                        in_q.add(s)

    solve([0])
    # Indirect jumps (switch tables): first decode exact in-function PIC jump tables
    # (`mov/add reg, [pic + idx*4 + tbl_disp]; ...; jmp reg`) so multi-switch functions
    # route each switch's state to its exact cases and never mis-seed landing pads.
    def _jt_info(bi):
        lo, hi = blocks[bi]
        bpic = {}
        run_block(bi, *in_state[bi], record=bpic)
        for i in range(hi - 2, max(lo - 1, hi - 6), -1):
            _, _, mnem, ops = insns[i]
            if mnem not in ('mov', 'add') or '[' not in ops:
                continue
            m = _MEM.search(ops)
            pm = parse_mem(m.group(2)) if m else None
            if not pm or pm[2] != 4:
                continue
            pv = bpic.get(i, {}).get(pm[0])
            if not isinstance(pv, int):
                continue
            tbl_va = (pv + pm[3]) & 0xffffffff
            if base <= tbl_va < end:
                return tbl_va - base, pv
        return None

    resolved_ind = set()
    jt_cache = {}
    while True:
        # Discover all currently reached in-function jump tables first so adjacent
        # tables at the end of the function bound each other cleanly.
        for bi in ind_jmps:
            if bi not in jt_cache and out_state[bi] is not None:
                info = _jt_info(bi)
                if info is not None:
                    jt_cache[bi] = info
        tbl_starts = sorted({off_t for off_t, _ in jt_cache.values()})
        added = []
        for bi, (off_t, pv) in jt_cache.items():
            if bi in resolved_ind:
                continue
            nxt_idx = bisect.bisect_right(tbl_starts, off_t)
            limit = tbl_starts[nxt_idx] if nxt_idx < len(tbl_starts) else len(code)
            tgts = []
            pos = off_t
            while pos + 4 <= limit:
                tva = (pv + s32(code, pos)) & 0xffffffff
                ti = addr_to_idx.get(tva)
                if ti is None:
                    break
                tgts.append(bisect.bisect_right(sleaders, ti) - 1)
                pos += 4
            if tgts:
                resolved_ind.add(bi)
                for tb in sorted(set(tgts)):
                    if tb not in succs[bi]:
                        succs[bi].append(tb)
                        preds[tb].append(bi)
                added.append(bi)
        if not added:
            break
        solve(added)

    # Fallback for indirect jumps whose jump table lives outside the function (e.g. ELF .rodata).
    unresolved_ind = [bi for bi in ind_jmps if out_state[bi] is not None and bi not in resolved_ind]
    if unresolved_ind:
        unreached = {bi for bi in range(nb) if in_state[bi] is None and not is_pad_blk[bi]}
        while unreached:
            roots = [u for u in sorted(unreached) if not any(not is_pad_blk[p] for p in preds[u])]
            if not roots:
                roots = [min(unreached)]
            for u in roots:
                prec = [j for j in unresolved_ind if j < u]
                src_j = prec[-1] if prec else unresolved_ind[0]
                in_state[u] = (dict(out_state[src_j][0]), dict(out_state[src_j][1]))
            solve(list(roots), allowed=unreached)
            unresolved_ind = [bi for bi in ind_jmps if out_state[bi] is not None and bi not in resolved_ind]
            unreached = {bi for bi in unreached if in_state[bi] is None}
    # Remaining unreached non-padding blocks (e.g. C++ exception landing pads): seed
    # root blocks from consistent callee-saved PIC regs and stack slots of preceding
    # call sites (else all call sites), propagating only among unreached blocks so
    # noreturn landing pads never poison normal code.
    unreached = {bi for bi in range(nb) if in_state[bi] is None and not is_pad_blk[bi]}
    if unreached and all_calls:
        def lp_seed(before_bi):
            csets = [st for bi, cs_ in all_calls.items() if bi < before_bi for st in cs_]
            if not csets:
                csets = [st for cs_ in all_calls.values() for st in cs_]
            lp_pic, lp_slots = {}, {}
            for cp, csl in csets:
                for k, v in cp.items():
                    if k not in lp_pic:
                        lp_pic[k] = v
                    elif lp_pic[k] != v:
                        lp_pic[k] = _SLOT_CLOB
                for k, v in csl.items():
                    if k not in lp_slots:
                        lp_slots[k] = v
                    elif lp_slots[k] != v:
                        lp_slots[k] = _SLOT_CLOB
            return {k: v for k, v in lp_pic.items() if v is not _SLOT_CLOB}, lp_slots

        while unreached:
            roots = [u for u in sorted(unreached) if not any(not is_pad_blk[p] for p in preds[u])]
            if not roots:
                roots = [min(unreached)]
            seeded = []
            for u in roots:
                lp_pic, lp_slots = lp_seed(u)
                if lp_pic or lp_slots:
                    in_state[u] = (lp_pic, lp_slots)
                    seeded.append(u)
            if not seeded:
                break
            # Do not propagate back into already-seeded landing-pad roots: a noreturn
            # `call __clang_call_terminate` before another pad's `jmp root` is not a loop.
            solve(seeded, allowed=unreached - set(seeded))
            unreached = {bi for bi in unreached if in_state[bi] is None}

    states = [{} for _ in range(n)]
    for bi in range(nb):
        if in_state[bi] is not None:
            run_block(bi, *in_state[bi], record=states)
    return insns, states, was_pic


def address_fields(code, base, image=None):
    """Heuristic address-bearing fields in linked code with no relocation info.

    Masks: displacements of branches leaving the function, disp32 relative to a
    PIC base register (CFG-tracked from call/pop or get_pc_thunk + add), and when
    `image` (lo, hi) is given, absolute disp32/imm32 values inside the image."""
    out = []
    if not cs:
        return out
    end = base + len(code)
    insns, states, _ = pic_states(code, base, allow_got=True)
    pic_by_addr = {ins[0]: {_REG_IDS[r] for r in st} for ins, st in zip(insns, states) if st}
    after_ext_call = False
    for ins in _det.disasm(code, base):
        off = ins.address - base
        b = bytes(ins.bytes)
        w = rel_branch_width(b)
        ext_call = False
        if w:
            tgt = (ins.address + ins.size + _sval(b[-w:])) & 0xffffffff
            if not base <= tgt < end:
                out.append((off + ins.size - w, w, 'branch'))
                ext_call = ins.mnemonic.startswith('call')
        else:
            ops = ins.operands
            if (ins.mnemonic == 'add' and after_ext_call and len(ops) == 2 and ops[0].type == csx86.X86_OP_REG
                    and ops[1].type == csx86.X86_OP_IMM and ins.imm_size == 4):
                out.append((off + ins.imm_offset, 4, 'pic'))
                after_ext_call = False
                continue
            pic = pic_by_addr.get(ins.address, ())
            for op in ops:
                if op.type == csx86.X86_OP_MEM and ins.disp_size == 4:
                    if op.mem.base in pic or (op.mem.index in pic and op.mem.scale == 1):
                        out.append((off + ins.disp_offset, 4, 'pic'))
                    elif image and image[0] <= (op.mem.disp & 0xffffffff) < image[1]:
                        out.append((off + ins.disp_offset, 4, 'heur'))
                elif op.type == csx86.X86_OP_IMM and ins.imm_size == 4 and image:
                    if image[0] <= (op.imm & 0xffffffff) < image[1]:
                        out.append((off + ins.imm_offset, 4, 'heur'))
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
