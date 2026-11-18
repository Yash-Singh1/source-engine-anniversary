#!/bin/bash
# Compile one source file to an i386 Mach-O object the way the retail Xcode
# Release build did (Apple clang-600.0.57), then optionally diff it against
# a retail module with bytematch.
#
# usage: macos-cc.sh [-o OUT.o] [--retail MODULE] [--symbol NAME] [--define LIBDEF]
#                    SOURCE.cpp [-- extra clang args (-I, -D, ...)]
#
# Environment overrides:
#   BM_CLANG     clang binary   (default: survey clang-600.0.57 build)
#   BM_SDK       sysroot        (default: MacOSX10.6.sdk extracted by the survey)
#   BM_OVERRIDE  header override dir searched first (default: survey mac-scratch/override)
#   BM_SRC       source root    (default: git toplevel of SOURCE)
set -euo pipefail
HERE=$(cd "$(dirname "$0")" && pwd)
MS=/Volumes/Storage/sco/byte-match-survey/mac-scratch
CLANG=${BM_CLANG:-$MS/build-600/bin/clang}
SDK=${BM_SDK:-$MS/x/MacOSX10.6/root/SDKs/MacOSX10.6.sdk}
OVERRIDE=${BM_OVERRIDE:-$MS/override}

OUT= RETAIL= SYMBOL= LIBDEF= SRC_FILE=
while [ $# -gt 0 ]; do
  case $1 in
    -o) OUT=$2; shift 2;;
    --retail) RETAIL=$2; shift 2;;
    --symbol) SYMBOL=$2; shift 2;;
    --define) LIBDEF=$2; shift 2;;
    --) shift; break;;
    -*) echo "unknown option $1" >&2; exit 2;;
    *) SRC_FILE=$1; shift;;
  esac
done
[ -n "$SRC_FILE" ] || { sed -n '2,15p' "$0"; exit 2; }
SRC_FILE=$(cd "$(dirname "$SRC_FILE")" && pwd)/$(basename "$SRC_FILE")
ROOT=${BM_SRC:-$(git -C "$(dirname "$SRC_FILE")" rev-parse --show-toplevel)}
stem=$(basename "${SRC_FILE%.*}")
OUT=${OUT:-$PWD/$stem.o}

if [ -z "$LIBDEF" ]; then
  case ${SRC_FILE#"$ROOT"/} in
    mathlib/*) LIBDEF=-DMATHLIB_LIB;;
    tier1/*) LIBDEF=-DTIER1_STATIC_LIB;;
  esac
fi

DEFS="-DVPC -DNDEBUG -DGNUC -DPOSIX -D_POSIX -DCOMPILER_GCC -D_DLL_EXT=.dylib -D_OSX -DOSX
 -D_DARWIN_UNLIMITED_SELECT -DFD_SETSIZE=10240 -DOVERRIDE_V_DEFINES -DFRAME_POINTER_OMISSION_DISABLED
 -DNO_MALLOC_OVERRIDE=1 -DVPROF_LEVEL=1 -DNO_HOOK_MALLOC=1 -DPNG_NO_PEDANTIC_WARNINGS -DCPP11_NO_LIBCXX
 -Usprintf -Ustrncpy -UPROTECTED_THINGS_ENABLE"
INC="-I$OVERRIDE -I$ROOT/common -I$ROOT/public -I$ROOT/public/tier0 -I$ROOT/public/tier1 -I$ROOT/public/mathlib
 -I$(dirname "$SRC_FILE")"
BASE="-x c++ -arch i386 -mmacosx-version-min=10.6 -isysroot $SDK -std=gnu++11 -stdlib=libstdc++
 -fmessage-length=0 -fpascal-strings -fasm-blocks -fstrict-aliasing -O2 -g
 -fvisibility=hidden -fvisibility-inlines-hidden -ffast-math -ftree-vectorize -ftemplate-depth=512
 -Qunused-arguments -w"

# shellcheck disable=SC2086
"$CLANG" $BASE $DEFS $LIBDEF $INC "$@" -c "$SRC_FILE" -o "$OUT"
echo "$OUT"
if [ -n "$RETAIL" ]; then
  args=(diff --retail "$RETAIL" --ours "$OUT")
  [ -n "$SYMBOL" ] && args+=(--symbol "$SYMBOL")
  "$HERE/bin/bytematch" "${args[@]}"
fi
