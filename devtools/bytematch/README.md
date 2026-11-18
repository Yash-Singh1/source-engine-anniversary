# bytematch

Per-function byte matching of x86-32 code against the retail Half-Life 2
Anniversary modules (Steam build 19307283), in the style of objdiff. The primary
target is macOS i386: retail `.dylib` (depot 223) versus our Mach-O `.o`
compiled with Apple clang-600.0.57. ELF (depot 224) and PE (depot 222) are
supported as secondary paths.

## Setup

Python 3.9+ standard library only. [capstone](https://www.capstone-engine.org/)
is optional but recommended: it enables padding trimming by instruction,
masking of branches that leave the function, the linked-vs-linked PIC
heuristics, and instruction-level diffs.

```sh
devtools/bytematch/setup.sh        # creates devtools/bytematch/.venv with capstone
devtools/bytematch/bin/bytematch --help
```

`bin/bytematch` re-executes itself inside `.venv` when it exists. Without the
venv it uses `python3` (and capstone from the user site, if installed).
`BYTEMATCH_NO_CAPSTONE=1` forces the byte-level fallback.

## Typical macOS workflow

```sh
R=/Volumes/Expansion/mac-ports/hl2-anniversary-depot/depots/223/19307283/bin/engine.dylib
BM=devtools/bytematch

# compile one file like the retail Xcode build, then compare it with its retail object
$BM/macos-cc.sh -o /tmp/convar.o --retail $R tier1/convar.cpp

# drill into one function (raw, canonical or demangled-substring name)
$BM/bin/bytematch diff --retail $R --ours /tmp/convar.o --symbol 'ConVar::InternalSetValue(char const'

# a whole build directory: JSON + markdown report
$BM/bin/bytematch report --retail $R --ours build/tier1 --out /tmp/tier1
```

`macos-cc.sh` uses the survey's clang-600.0.57 build, the 10.6 SDK and the header
overrides under `byte-match-survey/mac-scratch/` (overridable with `BM_CLANG`,
`BM_SDK`, `BM_OVERRIDE`). It adds `MATHLIB_LIB`/`TIER1_STATIC_LIB` for those
directories; other modules need their defines and include paths after `--`.

### Per-object scoping (Mach-O)

Retail Mach-O modules carry a stabs debug map: `N_OSO` names every object in
link order and `N_FUN` gives each function's object and size. When ours are
objects, the comparison is judged only against the retail functions the debug
map attributes to the same object stem, so a single `.o` can be checked in
isolation. Stems are normalised (`libtier1.a(convar.o)`, `convar.o`,
`convar.cpp.1.o` all become `convar`). Ambiguous stems are reported. Pick one
with `--retail-object 'libtier1.a(convar.o)'` (stem or path substring,
repeatable). `--all-retail` judges against everything.

Inline and template functions are emitted into many objects but the debug map
attributes each one to a single retail object. Copies outside the scope are
still compared, then reported separately as "out of scope" so they don't
distort the per-object numbers.

`bytematch objects MODULE` lists the debug map (link order, function count and
bytes per object).

## Commands

| command | purpose |
|---|---|
| `diff --retail M --ours X` | per-function status table plus summary; `--differs-only`, `--demangle`, `--show N` (diffs of the first N differing functions), `--json` |
| `diff ... --symbol S` | one function: addresses, objects, status, unified instruction diff (`--listing` for the full listing even when it matches) |
| `report --retail M --ours DIR --out P` | writes `P.json` and `P.md` (counts, % by function and by bytes, per-object table, largest differing functions with diffs, missing list) |
| `funcs MODULE` | functions extracted from any supported file (`--object`, `--symbol`, `--json`) |
| `objects MODULE` | Mach-O debug-map objects |
| `locate --retail M --ours X -o map.txt` | find our functions in an unnamed image (stripped PE) by masked pattern search, and write an address map |
| `selftest [--full]` | built-in tests (see below) |

`--ours` accepts an object, an `ar` archive (`.a`/`.lib`), a linked module, or
a directory, which is searched recursively for `.o .obj .a .lib .dylib .so .dll`.
`--retail-map` / `--ours-map` add names from a text map (`ADDR NAME [SIZE]`),
JSON (`{name: addr}` or `[{name, addr, size}]`), or an MSVC `/MAP` file.
Addresses below the image base are taken as RVAs. `locate` writes the text
format.

Exit status of `diff --symbol`: 0 matched, 2 differs, 1 not found or ambiguous.

## Statuses

- `exact`: identical bytes, including relocated fields.
- `matches-after-masking`: identical once relocated fields are masked on both
  sides, and every call/jump relocation in ours resolves to the same symbol
  that the retail branch lands on.
- `differs`: different size, different unmasked bytes, or a relocation that
  demonstrably targets another symbol (`reason` says which). The first
  differing offset and a short instruction diff are reported.
- `missing`: an in-scope retail function with no counterpart in ours.
  `missing-in-retail` / "only in ours" is the reverse.

Percentages are matched / in-scope, by function count and by retail bytes.

## How matching works

1. **Function extents.** Retail Mach-O: symbols (local and global) plus
   `LC_FUNCTION_STARTS` plus `N_FUN` sizes. Objects: section symbols, extent to
   the next symbol. ELF: `STT_FUNC` with `st_size` (`.symtab`, then `.dynsym`).
   COFF: function symbols per section (COMDAT = one function). PE: exports,
   COFF symbols if present (mingw), and user maps. Trailing alignment padding
   (NOP forms, `int3`) is trimmed by decoding, or by a byte-pattern table
   without capstone.
2. **Pairing** by canonical name: Mach-O drops the leading `_`, COFF drops
   `_`/`@N` C decoration and `__imp_`, and aliases (C1/C2 constructors) are
   tried too. File-local functions pair within the same object stem.
3. **Masks.** Our object's relocations are authoritative: Mach-O scattered and
   normal relocs including `SECTDIFF` PIC pairs, ELF `REL`/`RELA` (GOT32/GOT32X
   also mask the two opcode bytes that `ld` may relax), and COFF i386 types.
   Added to those are branches that leave the function, which assemblers
   resolve without a relocation for same-section targets. On the retail side:
   PE base relocations and Mach-O rebase opcodes or local relocations in text.
   When both sides are linked, capstone heuristics mask PIC-base-relative disp32
   (call/pop and `get_pc_thunk`+`add`, following register copies and stack
   spills), and in non-PIC images absolute disp32/imm32 inside the image.
4. **Target check.** For each call/jump (and, in PE, absolute) relocation in
   ours, the retail field is resolved to a symbol through the symbol table,
   i386 `__symbol_stub` indirect symbols, ELF PLT entries, or the PE
   import/export tables. A disagreement makes the pair `differs`
   (`--no-target-check` turns this off).

## Formats

| | objects | linked |
|---|---|---|
| Mach-O i386 (thin or fat) | yes (clang 3.x-21, `__text`/`__textcoal_nt`) | dylib/bundle/exe: symbols, stabs debug map, function starts, stubs, rebase info |
| ELF32 i386 | yes (REL and RELA) | `.so`/exe: `.symtab` + `.dynsym`, PLT names, text relocations |
| COFF i386 | yes, incl. `/bigobj`; `/GL` LTCG objects are rejected (they contain IL) | PE32: exports, imports + delay imports, base relocs, COFF symbols, user maps |
| `ar` archives | GNU, BSD (`#1/`) and MSVC `.lib` members | |

## Self-test

`bin/bytematch selftest [--full] [--rebuild]` checks:

- retail modules against themselves (Mach-O, ELF, PE; `--full` adds engine and
  server) at 100% exact, and the Mach-O debug-map attribution;
- `tests/sample.cpp` built with clang-600.0.57 + ld64-242, current Xcode clang,
  mingw-w64 i686 and Steam Runtime scout gcc-9 (Docker): each object fully
  matches its own linked module; `-O1` objects against `-O2` modules must
  differ; a forged call target must be flagged; `locate` + map names more of a
  stripped DLL;
- `tier1/checksum_md5.cpp`, `tier1/checksum_crc.cpp`, `mathlib/IceKey.cpp`
  from this tree compiled by `macos-cc.sh` fully match retail `engine.dylib`;
- the survey pilot objects (`mac-scratch/pilot/a600`) reach at least the
  pilot's 311 matched functions;
- the same comparisons without capstone.

Toolchains or depots that are absent are skipped. Build outputs go to
`tests/build/` (ignored).

### Current Xcode and i386

Current Xcode (Apple clang 21) `clang++ -arch i386 -c` still
emits i386 Mach-O objects, even with libc++ headers, so it can exercise the
Mach-O object parser. However, the SDK has no libstdc++ headers (retail uses
libstdc++), `ld` cannot link i386 (the SDK `.tbd` stubs have no i386 slice), and
its code generation is unrelated to Xcode 6. Use clang-600.0.57 for anything
that is meant to match.

## Whole-file identity (`identity/`)

Function matching ignores the symbol table. `identity/identity.py` rebuilds
`--retail-osx32` modules so the whole file can equal retail, debug map and
LC_UUID included:

```sh
python3 waf configure --retail-osx32 -T release --prefix='' -o build-retail
python3 waf build && python3 waf retail_linkdb      # writes link_commands.json, retail_compile_commands.json
devtools/bytematch/identity/identity.py --all       # or: identity.py inputsystem vgui2 ...
devtools/bytematch/identity/machocmp.py -s OURS RETAIL   # region-by-region and symbol diff
```

Regression check: `devtools/bytematch/identity/ci.sh [BUILD_DIR]` builds, writes
the link database and runs `identity.py --identical`, which rebuilds every
module in `identity/identical.json` and `cmp`s it against retail (the retail
file's sha256 is checked too). It exits non-zero if any of them differ. If
`BUILD_DIR` (default `build-retail`) was never configured, it is configured
with just the modules those need. Add a module to `identical.json` once it
becomes identical.

Outputs go to `build-retail/identity/out/`. For each object the retail debug
map lists, the tool reproduces what ld64 records:

- **N_SO**: the source is preprocessed with absolute include paths, our tree,
  SDK and clang resource prefixes in the `.ii` are rewritten to Valve's
  (`/Users/buildbot/buildslave/rel_singleplayer_osx/build/src/`, Xcode's
  `MacOSX10.9.sdk`, Xcode's `lib/clang/6.0`), and the `.ii` is compiled under
  its retail ccache temp name with `-fdebug-compilation-dir
  /Users/buildbot/buildslave/../.ccache/tmp`, as ccache did.
- **N_SOL**: follows from the rewritten line markers. Files that sit elsewhere
  in our tree are mapped by `identity/aliases.json`.
- **N_OSO**: objects are written under a scratch root at Valve's Xcode paths
  with the retail mtimes. Archives are rebuilt with retail member names and
  member dates, in our member order.
- **Link**: the waf link command runs with Valve's object and archive paths.
  `identity/buildhost.c` is injected into the clang driver and ld64 with
  `DYLD_INSERT_LIBRARIES`. It maps `/Users/buildbot/` onto the scratch root
  (ld64 records input paths verbatim). It also replaces `qsort` with the OS X
  10.9/10.10 one (`identity/qsort109.c`): ld64-242 qsorts output sections,
  and `__DATA,__bss` and `__DATA,__common` tie.
- **LC_UUID**: no patching. ld64 hashes the file without stab nlists and stab
  strings, so the UUID follows from everything else (`identity/lduuid.py`
  recomputes it).

Code and data sections never change in this flow. If `machocmp.py` reports a
section difference, it is source drift (see the lane report). Things that
show up only here:

- the `GCC_except_tableN` labels, numbered by clang's per-TU function index,
  which counts functions that were dead-stripped;
- function order inside an object, which follows clang's first reference to
  each function, not definition order;
- literal placement, where the first-loaded archive member's copy wins, even
  when the function using it is dead-stripped.

## Known gaps

- Data references are masked but not checked symbolically (only call/jump
  targets, and absolute fields in PE). Two functions that load different
  globals at the same offsets still count as matching.
- Linked-vs-linked comparisons rely on heuristics. PIC registers are tracked
  linearly and never un-tracked, so a register reused for a large struct offset
  after holding the PIC base can be over-masked.
- Stripped PE modules: function extents are unknown until a map supplies them.
  A paired function is compared over our length and must be followed by
  padding, so byte totals use our sizes.
- Retail functions with no debug-map entry (about 4% of `engine.dylib`, mostly
  assembly or prebuilt code without stabs) are outside the per-object scope.
- Inline jump tables in `__text` are compared as bytes (retail
  `LC_DATA_IN_CODE` is empty, so clang-600 places them in `__const`).
- Only x86-32. 64-bit Mach-O and ELF are rejected.
