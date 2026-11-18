# Retail macOS i386 toolchain

Rebuilds the toolchain Valve used for the HL2 Anniversary macOS modules (depot
223, build 19307283) from public sources, on an arm64 or x86_64 Mac, and drives
the `--retail-osx32` waf mode. Everything installs under
`$BYTEMATCH_TOOLCHAIN` (default `~/.cache/source-bytematch/toolchain`).

| piece | version | why |
|---|---|---|
| compiler | Apple clang-600.0.57 (Xcode 6.2) | static-init naming and the pilot match rate pin clang-600; .54 gives identical code on every object we build, .51 differs in one object retail doesn't link |
| linker | ld64-242 (Xcode 6.2) via cctools-port | 236.3 writes `__unwind_info` with 2^0 alignment, retail has 2^2; 241.9 and 242 give byte-identical dylibs |
| SDK | stub MacOSX10.9.sdk | 10.6 headers plus i386 stub libraries carrying retail's install names, versions and exports |
| hl2mp (2014) | clang-503.0.38, ld64-236.3 (Xcode 5.1), protoc 2.3.0 | depot 223's hl2mp modules are a May 2014 Source SDK 2013 build: `LC_VERSION_MIN` 10.5, `__unwind_info` 2^0; `--build-games=hl2mp` (see `sdk2013mp/wscript`). clang-503 carries `patches/clang-503.0.38-maskandbranch.patch` (an uninitialized x86 flag made Xcode 5.1's codegen vary per compile) |

## Setup

Needs cmake, ninja, git, Xcode command line tools, and the Snow Leopard retail
install image (`Mac_OS_X_10.6_Snow_Leopard_Retail.iso`) for the 10.6 SDK.

```sh
T=devtools/bytematch/toolchain
$T/build-clang.sh 600.0.57          # optional: 600.0.54 600.0.51 503.0.38
$T/build-ld64.sh 242                # optional: 241.9 236.3
$T/extract-sdk106.sh /path/to/Mac_OS_X_10.6_Snow_Leopard_Retail.iso
$T/make-sdk109.py                   # reads retail imports from depot 223/340/380/420
$T/build-thirdparty.py              # optional: libjpeg 8b, libpng 1.5.2, Crypto++ 5.6.1, speex 1.0.4
$T/build-protoc.sh                  # hl2mp only, with clang 503.0.38 and ld64 236.3
git submodule update --init thirdparty

python3 waf configure --retail-osx32 -T release --prefix='' -o build-retail
python3 waf build
# hl2mp alone (needs a Source SDK 2013 git checkout with history, $SOURCE_SDK_2013):
#   python3 waf configure --retail-osx32 -T release --prefix='' -o build-hl2mp --build-games=hl2mp --retail-modules=
$T/report-modules.py                # per-module function match vs retail (devtools/bytematch matcher)
```

Each clang build takes a few minutes. `--from-build DIR` on the build scripts
installs from an existing build tree instead of rebuilding. Sources are pinned
by tag/commit and sha256 in `versions.sh` and `build-thirdparty.py`; the only
source changes are the files in `patches/`.

Native arm64 ld64 maps input files with 16 KB pages and fails with EINVAL on fat
slices at 4 KB-aligned offsets, so every SDK and depot dylib it reads is thinned
to i386 first (`thin-i386.py`, and the waf mode's `retail-depot-libs/`).

## Files

- `versions.sh`: pinned clang tags and cctools-port commits
- `build-clang.sh`, `build-ld64.sh`: fetch, patch, build, install
- `extract-sdk106.sh`, `thin-i386.py`: Snow Leopard SDK extraction
- `make-sdk109.py`, `macho.py`: stub 10.9 SDK generator
- `retailflags.py`: compiler and linker flags from Valve's xcconfig/VPC, shared by everything here and by `scripts/waifulib/retail_osx32.py`
- `ld64-pin.py`: links scenefilecache with each ld64 and compares structure with retail
- `build-thirdparty.py`: period third-party archives (known non-matching inputs)
- `build-protoc.sh`: host protoc 2.3.0 from `sdk2013mp/thirdparty/protobuf-2.3.0`, for hl2mp's `htmlmessages.pb.cc`
- `report-modules.py`: match summary for a `--retail-osx32` build directory

## Stub SDK: what it does and doesn't reproduce

For each of the 24 system libraries retail modules load, `make-sdk109.py`
builds a flat i386 stub with the retail `LC_LOAD_DYLIB` install name and
versions (libSystem 1197.1.1, CoreFoundation 855.17, AppKit 1265.21,
libstdc++ 60.0.0, ...). Its exports are the 10.6 library's own, with
privately re-exported sublibraries flattened in and every retail import added,
and weak definitions follow 10.6 or retail's weak-binding entries. Re-exports at
public locations stay `LC_REEXPORT_DYLIB` (Carbon -> ApplicationServices),
because ld64 links those implicitly and that decides the client's dylib list.
Every other file is the thinned 10.6 SDK. The directory must be named
`MacOSX10.9.sdk`: ld64 takes the `LC_VERSION_MIN` sdk field from that path.

Header-difference risk. The headers are 10.6's, not 10.9's:

- Prototypes. 10.8 moved `scandir`/`alphasort` to POSIX-2008 const signatures;
  `patches/sdk109-dirent.patch` applies that. 10.7+ no longer declares the
  QuickDraw GWorld calls video_quicktime redeclares as function pointers;
  `patches/sdk109-qdoffscreen.patch` removes them. 10.8 also marked `longjmp` and
  friends `__dead2` (`patches/sdk109-setjmp.patch`); that one compiles either way but
  changes codegen around functions that end in `longjmp`. Others may surface as modules are
  added. Errors like these are SDK gaps, not source drift: patch the SDK, not the tree.
- Inline functions and macros in libc, libm and the frameworks (e.g. `__sincosf`
  exists only in 10.9's math.h), plus `_FORTIFY_SOURCE` wrappers and the
  CarbonCore/CoreServices declarations. Any of these can change codegen without
  failing to compile. libstdc++ is 4.2.1 in both SDKs, and Apple barely touched
  those headers between 10.6 and 10.9.
- Availability macros report 10.6 as the maximum allowed version, so code that
  checks `MAC_OS_X_VERSION_MAX_ALLOWED` may take different branches than Valve's.
- Stub libraries are only correct for symbols retail binds to. Our sources can
  bind elsewhere, e.g. to a symbol that moved libraries between 10.6 and 10.9.

A real MacOSX10.9.sdk (Xcode 6.1/6.2) removes all of these. Drop it in as
`sdk/MacOSX10.9.sdk` after thinning it with `thin-i386.py`.
