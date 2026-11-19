# Retail Linux i386 game modules

Rebuilds the HL2 Anniversary Linux game modules (`episodic/bin/server.so` and
`client.so` in depots 380/420, `lostcoast/bin/*.so` in depot 340, build 19307283)
the way Valve built them, and measures them per function against retail.

| piece | version | source |
|---|---|---|
| image | `registry.gitlab.steamos.cloud/steamrt/scout/sdk:0.20221019.0`, pinned by digest `sha256:005e25de…` (`scout.py`) | Steam Runtime; any snapshot 0.20210317.0 to 0.20221019.0 has this gcc |
| compiler | `g++-9 (SteamRT 9.3.0-17+steamrt1.3+srt1) 9.3.1 20210309` | retail `.comment` |
| as/ld | binutils 2.30 (the `g++-9` wrapper puts `/usr/lib/binutils-2.30/bin` first) | retail `.plt.got`, GOT32X relaxation |
| ar/strip | system binutils 2.22 | |

Survey: `byte-match-survey/linux-elf.md`.

## Use

```sh
docker pull registry.gitlab.steamos.cloud/steamrt/scout/sdk@sha256:005e25dee9751524db33233550a17157af9f0ea1680ff52c26fb5b4b2fd799fb
git -c protocol.file.allow=always submodule update --init

WAFLOCK=.lock-waf-linux python3 waf configure --retail-linux32 --build-games=episodic --prefix='' -o /abs/build-linux
WAFLOCK=.lock-waf-linux python3 waf build -j6
WAFLOCK=.lock-waf-linux python3 waf retail_linkdb      # per-object compile database for the report
devtools/bytematch/toolchain/linux/report-linux.py --build /abs/build-linux --game episodic
```

`--build-games=lostcoast` builds Lost Coast (the hl2 project with `HL2_LOSTCOAST`
and its own static-library variants, as `--retail-osx32`); `hl2` is the episodic
build. Without `--build-games` only the static libraries are built.
`--retail-linux32-modules=tier1,game/server` picks subprojects.

## How it runs

- `scout.py` keeps one container per tree and build directory (`docker run`
  takes ~20 s on a loaded host, `docker exec` ~1 s), the tree mounted at
  `/data/src` and the build directory at `/data/build`. waf runs on the host;
  the compiler, `ar` and the linker run in the container on translated paths.
- Each compile runs in `/data/src/<project dir>` and names its source as
  Valve's VPC makefiles did: `$(PWD)/file.cpp` for files under the project,
  else `$(PWD)/../../<tree path>`. Include directories are absolute
  (`$(abspath $(INCLUDEDIRS))`). This reproduces retail's 130 `__FILE__`
  strings (`/data/src/game/server/../../game/shared/gamemovement.cpp`,
  `/data/src/public/tier1/UtlSortVector.h`).
- Flags: `makefile_base_posix.mak` (CFG=release, linux32) plus what the survey
  recovered from retail code: `-m32 -march=pentium4 -mtune=core2 -msse2
  -mfpmath=sse -march=core2 -fvisibility=hidden -gdwarf-2 -g2 -O2
  -fno-strict-aliasing -ffast-math -fno-finite-math-only
  -fno-delete-null-pointer-checks -fno-omit-frame-pointer -ftree-vectorize
  -fpredictive-commoning -funswitch-loops -pipe -std=gnu++0x -fpermissive`,
  `-Usprintf -Ustrncpy -UPROTECTED_THINGS_ENABLE`, gcc-9's default PIE (no
  `-fPIC`). `-w -Wno-narrowing` only silence diagnostics.
- Files: the macOS manifest's retail object lists (retail-osx32.json, applied
  by `retail_manifest`), plus `manifests/retail-linux32.json` from
  `make-manifest.py`: per module the Linux defines (this tree's VPCs evaluated
  for LINUX32 Release, then the makefile generator's and
  `makefile_base_posix.mak`'s additions) and the files Valve's 2018 VPCs
  compile only on Linux (`tier1/pathmatch.cpp`, `tier1/qsort_s.cpp`,
  `mathlib/3dnow.cpp`; game modules pull none of `pathmatch.cpp`).
- No `DX_TO_GL_ABSTRACTION`/`GL_GLEXT_PROTOTYPES`: retail Linux renders through
  DXVK (depot 224 ships `libdxvk_d3d9.so`, `shaderapivk.so`), and its client
  writes vertex colors unswapped, i.e. without `imesh.h`'s `OPENGL_SWAP_COLORS`.
  Our VPCs (and hl2_src's) force `$GL` on for every non-Windows client, so
  `make-manifest.py` drops the two defines.
- Telemetry on: retail imports `g_Telemetry` and has a zone at every VPROF, as
  `vprof_telemetry.h` enables for `_LINUX`. RAD's headers come from hl2_src
  (`--retail-linux32-hl2src`), copied under the build directory.
- Mode defines (not Valve's): `BYTEMATCH_RETAIL` (shared with the macOS and
  Windows modes) and `BYTEMATCH_RETAIL_LINUX` (Linux-retail-only source:
  `public/vstdlib/random.h` drops `CDefaultUniformRandomStream`, whose inline
  methods let gcc speculatively devirtualize every `random->` call;
  `public/tier0/memalloc.h` takes SDK 2013's POSIX allocators, without the
  fork's `MemAllocOOMError` checks).
- Link, as `LIB_START_SHLIB`/`LIB_END_SHLIB` with `$(LINK)` = gcc-9:
  `-shared -m32 -Wl,--build-id -static-libgcc`, objects, the static libraries
  in one `--start-group`, then `libtier0.so libvstdlib.so libsteam_api.so`
  copied from depot 224, `-lm -ldl -l:libstdc++.a -lpthread -l:ld-linux.so.2`
  (`-lrt` for the client) and `devtools/version_script.linux.txt`. NEEDED
  equals retail's. gcc-9's libstdc++ is static only. zlib is SDK 2013's
  `libz.a` (byte-identical to retail's), protobuf SDK 2013's prebuilt. Not
  linked yet: libpng and libjpeg (retail's client exports libpng and builds
  `imageutils.cpp` with `HAVE_PNG`/`HAVE_JPEG`).
- After the link, `devtools/gendbg.sh`'s steps and `strip -x -S` with the
  image's binutils 2.22 write `stripped/NAME.so` and `NAME.so.dbg` (retail's
  trailing section order).

## Measuring

Retail Linux modules are stripped (`strip -x -S`): only `CreateInterface` and
libstdc++ helpers keep symbols. bytematch takes function extents from
`.eh_frame` (code between FDEs becomes unnamed functions) and names retail
functions after ours (`bytematch/elfmatch.py`):

- a masked search against every retail function of the same size;
- object order (ld keeps each object's `.text` contiguous): unique hits that
  ascend are anchors, ambiguous hits are resolved between them, and functions
  with no hit pair with the unclaimed FDE functions between anchors, so they
  get instruction diffs;
- groups of ours with identical masked code (vtable-setting destructors,
  template instantiations) are assigned to their candidates by call targets;
- a call target counts as wrong only when retail's callee was found by a unique
  hit under another name and isn't an identical-code copy of our callee;
- functions whose code equals a retail function another of ours took (more
  identical instantiations in ours than retail keeps) count as matched.

`report-linux.py` builds each module's object set (its own objects plus the
static-library members the macOS module pulls) from `waf retail_linkdb`'s
per-object compile database, and writes per-module reports and
`retail-linux32-GAME.md`. `--no-target-check` gives the survey pilot's metric.

One file: `bytematch diff --retail server.so --ours build/game/shared/x.cpp.N.o --symbol NAME`.
