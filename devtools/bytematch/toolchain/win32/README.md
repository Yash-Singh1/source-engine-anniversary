# Retail Windows i386 toolchain

Rebuilds the toolchain Valve used for the HL2 Anniversary Windows modules
(depot 222, build 19307283) from Microsoft's public downloads, runs it on an
Apple Silicon Mac under wine, and drives the `--retail-win32` waf mode.
Everything installs under `$BYTEMATCH_TOOLCHAIN/win32` (default
`~/.cache/source-bytematch/toolchain/win32`). No Microsoft binary enters the repo.

| piece | version | source |
|---|---|---|
| compiler | cl/c1/c1xx/c2 18.00.40629 (VS2013 Update 5) | Express 2013 for Windows Desktop Update 5 ISO: RTM MSIs + `kb2829760vc.msp` |
| linker, ml, mspdb | 12.00.40629 | same patch |
| static CRT | libcmt/libcpmt objects at build 21005 (Update 2 layer, as retail's Rich headers) | `kb2829760v2.msp` |
| SDK | Windows 7.1A (XP support, `v120_xp`) | `XPSupport` MSI on the same ISO |
| wine | 11.0 (Gcenx macOS build, WoW64) | github.com/Gcenx/macOS_Wine_builds, pinned by sha256 |
| msxml6 | 6.0 SP2 (KB2957482), 32-bit DLLs only | Microsoft download, as winetricks; needed by `mc.exe` |

## Setup

Needs `msiextract`, `cabextract` and `7zz`/`7z` (`brew install msitools cabextract sevenzip`).

```sh
T=devtools/bytematch/toolchain/win32
$T/fetch-msvc2013.py        # ~400 MB of HTTP range reads from the 3.5 GB ISO; every file sha256-checked
$T/setup-wine.py            # wine, prefix, native msxml6
$T/build-thirdparty.py      # stand-in cryptlib.lib (Crypto++ 5.6.1) and libpng.lib (1.5.2)
$T/make-manifest.py         # optional: regenerate devtools/bytematch/manifests/retail-win32.json (needs hl2_src)
git -c protocol.file.allow=always submodule update --init

WAFLOCK=.lock-waf-win python3 waf configure --retail-win32 -o build-win32   # --retail-win32-modules=tier0,vstdlib,...
WAFLOCK=.lock-waf-win python3 waf build -j12
$T/report-modules.py --build build-win32 [--mac-build build-retail --mac-game-build hl2=DIR ...]
$T/win32-cc.py MODULE FILE.cpp --retail [--against engine] [--symbol NAME]   # one file vs retail
```

The default module set is every first-party depot 222 module the macOS build
covers: the eight engine-side DLLs above plus filesystem_stdio, materialsystem,
shaderapidx9 (Valve's Win32 file set: real D3D9, no ToGL), stdshader_dx9,
vgui2, vguimatsurface, GameUI, ServerBrowser, vphysics, replay, launcher,
hl2.exe (`launcher_main`, linked as `default.exe`), and the hl2 and episodic
`client`/`server` (`client_hl2`, ..., in per-module directories since both
games build `game/client/client.dll`). A full build is about 400 cl batches
and several hours under heavy load.

`WAFLOCK` keeps each build directory's lock file apart, so a `--retail-osx32`
build (`WAFLOCK=.lock-waf-mac ... -o build-retail`) can be configured and run
alongside. The output directory may live outside the tree (`-o /abs/path`);
wine sees every host path as `Z:\...`.

`fetch-msvc2013.py` verifies 19 downloaded packages and then 20 key files of
the assembled tree (`versions.EXPECTED`); a fresh run reproduced the survey's
tree exactly. `BYTEMATCH_MSVC2013=DIR` points everything at another copy.

## How it runs

- `msvc.py TOOL ARGS` runs `cl`, `link`, `lib`, `ml`, `rc`, `mc`, ... under wine.
  Absolute POSIX paths in arguments become `Z:\` paths, long command lines go
  through a response file, `INCLUDE`/`LIB` are always the toolchain's, and the
  tools run on Microsoft's own `msvcr120`/`msvcp120`.
- The macOS wine build is WoW64: 32-bit `cl.exe` runs inside a 64-bit macOS
  process, which Rosetta 2 translates. Linux containers on Apple Silicon can't
  do that (Rosetta for Linux has no 32-bit code segments), so 32-bit wine there
  falls back to qemu-i386.
- `setup-wine.py` disables the bundle's FreeType. Without it wine skips
  enumerating every macOS font through CoreText in each process, which cost
  about 3 s per tool run and serialised parallel builds on the wineserver.
- Rosetta re-translates `c1xx.dll`/`c2.dll` in every new process, so the waf
  mode compiles sources with identical switches in one `cl` run (12 files a
  batch), as MSBuild did.

## `link /DEBUG` (LNK1101 in the survey)

| environment | cl | link without /DEBUG | link /DEBUG |
|---|---|---|---|
| Debian 12 wine 8.0, win32 prefix, Docker (qemu-i386) | yes | `LNK1000` assert | no |
| Debian trixie wine 10.0, Docker (qemu-i386) | yes, ~4.5 s/TU | yes | `LNK1101`; with tracing it hangs after loading `mspdb120.dll` |
| Arch wine 11.22 (WoW64), Docker under Rosetta for Linux | 64-bit only | n/a | prefix gets no `syswow64`: 32-bit code can't run |
| CrossOver 26.3 wine, macOS | yes | yes | **yes** (PDB written) |
| Gcenx wine-stable 11.0, macOS (this setup) | yes | yes | **yes** (PDB written) |

The failure follows the qemu-i386 user-mode emulation of the 32-bit wine
loader, not the MSVC files: the same `link.exe`/`mspdb120.dll` link with
`/DEBUG` under both macOS WoW64 wines. `/Z7`, `mspdbsrv -start` and native
msvcr120 weren't needed. A real x86_64 Linux host (no qemu) with `docker/`
was not tested.

Every `--retail-win32` DLL links with `/DEBUG`. `llvm-pdbutil dump --summary`
reads the PDBs, and their GUID and age equal the DLL's CodeView record. Like
retail's (ages 7 to 43), the age goes up on each relink into an existing PDB.
GUID, age and the PE/debug timestamps still differ from retail on every link;
MSVC 2013 has no `/Brepro`.

## `--retail-win32`

`scripts/waifulib/retail_win32.py` builds the modules in
`manifests/retail-win32.json`, which `make-manifest.py` generates from Valve's
2018 hl2_src VPC scripts (`vpc.py` evaluates them for `WIN32 WINDOWS VS2013
RETAIL`, Release). The module wscripts aren't used: their file lists are this
fork's. Per module:

- sources and their order: VPC's vcxproj generator order (files sorted by name
  within each folder, then subfolders; `external/vpc/.../projectgenerator_vcproj.cpp`),
  which is MSBuild's compile and link order. `link-order.py MODULE` checks it
  against the order objects appear in retail `.text` (studiorender: 0
  inversions with generator order, 8 with script order).
- `cl` switches as MSBuild derives them from the VPC properties (`/O2 /Ob2 /Oi
  /Ot /GF /MT /Gy /arch:SSE /fp:fast /Zc:forScope /GR /Gd /TP /FC /Zi
  /W4 /Zc:wchar_t /GS- /wd4316 /Zm200 /Zo /Oy- /Gw /Zc:inline`; libraries `/Z7`),
  defines including MSBuild's implicit `_WINDLL`, `_MBCS` and `v120_xp`'s
  `_USING_V110_SDK71_`, per-file overrides, `/Yc`/`/Yu` precompiled headers,
  `pointeroverride.asm` (`ml /safeseh`), `mc.exe` for tier0's ETW manifest.
- link: `/INCREMENTAL:NO /DEBUG /SUBSYSTEM:WINDOWS,5.01 /OPT:REF /OPT:ICF
  /MACHINE:X86 /MANIFEST:NO /DYNAMICBASE /NXCOMPAT /SAFESEH:NO`,
  `/PDBALTPATH:` the buildslave path retail's CodeView records name (so the
  record has retail's size in `.rdata`), Valve's
  `/NODEFAULTLIB` list and system libraries, static libraries built in the
  same run, import libraries for retail third-party DLLs (steam_api, SDL2,
  binkw32) generated from their export tables, `libz.lib` from Source SDK 2013.

Not Valve's, and why:

| item | reason |
|---|---|
| `BYTEMATCH_RETAIL` | excludes this fork's runtime additions, as `--retail-osx32` |
| `NO_X360_XDK` | Valve's tree had the Xbox 360 XDK's win32 headers (`bitmap/imagebyteswap.cpp`) |
| `/FI msvc2013_compat.h` | maps `alignas`/`alignof`, which this tree uses, to MSVC 2013 spellings; no code |
| `/FS`, per-target `/Fd` | parallel `cl` runs share a PDB; debug info only |
| no `lib/public/ftol3.obj` | Valve's object isn't public (retail Rich headers show it: one MASM 12.00.21210 object) |
| RAD Telemetry declarations in `telemetry/include/telemetry.h` | RAD's header is proprietary and not in this tree. Retail Windows modules carry Telemetry zones at every `VPROF`/`tmZone` site, so the build resolves `vprof_telemetry.h`'s `../../thirdparty/telemetry/include/telemetry.h` through `build-win32/retail-win32-telemetry`, a link to `telemetry/`. The declarations were inferred from the retail binaries: tier0 loads Telemetry 1.1 build 25, the context's function table has the 2.0 offsets, and `tmCoreMessage` (no thread id), `tmCorePlot*` (no timestamp) and `tmCoreSendCallStack` (no frame skip) have the older argument lists |
| `nvtc.lib` from `nvtc_stub.cpp` | NVIDIA's S3TC encoder library (bitmap, vguimatsurface) isn't available; the stub encodes nothing |
| `telemetry32.lib` from `telemetry_stub.c` instead of `telemetry32.link` | RAD's loader library isn't available; the stub's `tmStartup` etc. report failure, so tier0 never opens a context. Only tier0 links it |
| `jpeglib` built here | retail links an older VS2010 SP1 libjpeg prebuilt |
| no `socketlib` | not in hl2_src |
| `cryptlib.lib`, `libpng.lib` from `build-thirdparty.py` | Valve's prebuilts aren't available; same pinned Crypto++ 5.6.1 / libpng 1.5.2 sources as the macOS stand-ins |
| `libprotobuf.lib`, `libcurl.lib`, `libz.lib` from Source SDK 2013 | the period prebuilts (thirdparty-static.md) |
| ivp libraries from the ivp submodule's VPCs | hl2_src has no ivp tree; `hk_math` also gets `ivp/ivp_utility` (the fork's `vector_fpu.inl` includes `ivu_types.hxx`) |
| `/MAP` | side file only; tells the matcher which call targets `/OPT:ICF` folded |

## Docker

`docker/build-image.sh [--fetch]` builds `bytematch-wine10` (Debian trixie
pinned by digest, wine 10.0~repack-6, msitools/cabextract/7zip) and can run
`fetch-msvc2013.py` inside it. On non-macOS hosts `versions.py` uses the
system wine and `$WINEPREFIX`.

## Files

- `versions.py`: pinned URLs, sizes, sha256s, overlay lists, paths
- `fetch-msvc2013.py`, `setup-wine.py`, `msvc.py`: toolchain and runner
- `vpc.py`, `make-manifest.py`: VPC evaluation and the module manifest
- `msvc2013_compat.h`: force-included C++11 spellings
- `telemetry/include/telemetry.h`: the Telemetry 1.1 API as retail calls it
- `telemetry_stub.c`: stand-ins for RAD Telemetry's loader functions (tier0)
- `nvtc_stub.cpp`: stand-ins for NVIDIA's S3TC encoder (bitmap)
- `build-thirdparty.py`: stand-in Crypto++ and libpng archives
- `win32-cc.py`: compile one file with its module's switches and diff it against retail
- `winline.py FILE OURS:RETAIL,...`: give lines retail Windows' `__LINE__` numbers (telemetry
  zones) in MSVC retail builds only, without moving any other line on either platform
- `gateline.py FILE LINE,...`: leave lines out of retail builds (`#ifndef BYTEMATCH_RETAIL`), or give MSVC retail
  builds other text at a retail line number (`--retail-line N --retail-text TEXT`), without moving any other line
- `report-modules.py`, `link-order.py`: match summary with macOS cross-check, link order check.
  Symbol-port maps for the 16 modules beyond the survey's eight are in
  /Volumes/Storage/sco/byte-match-survey/win32-maps (the survey's `symbols/` pipeline run with
  the new modules and this build's objects as exact-byte anchors); both tools search it before
  the survey's `symbols/out` (`--maps DIR`, repeatable, or `BM_WIN32_MAPS` overrides).
- `docker/`: Linux image

## Licenses

- Visual Studio Express 2013 for Windows Desktop with Update 5, including the
  KB2829760 payloads: Microsoft Software License Terms for Visual Studio Express
  2013. Free of charge for developing and testing your own programs; only the
  listed redistributables may be redistributed, never the compiler or tools.
  Setup normally shows these terms; the scripts unpack the packages without
  running setup, so read and accept the terms before using them.
- Windows SDK 7.1A (XP support): Windows SDK license terms.
- MSXML 6.0 SP2: Microsoft redistributable terms.
- Wine: LGPL-2.1 (Gcenx's builds bundle other open-source libraries under their own licenses).
- Don't redistribute `$BYTEMATCH_TOOLCHAIN/win32`.
