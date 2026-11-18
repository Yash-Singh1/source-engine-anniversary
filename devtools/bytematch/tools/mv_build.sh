#!/bin/bash
# Compile tier0 / vstdlib sources with the retail-era Apple clang-600 flags.
# Usage: mv_build.sh <outdir> [module/file ...]
# Default file list is the retail N_OSO object list of libtier0 + libvstdlib.
set -u
HERE=$(cd "$(dirname "$0")" && pwd)
SRC=$(cd "$HERE/../../.." && pwd)
MS=/Volumes/Storage/sco/byte-match-survey/mac-scratch
CLANG=${CLANG:-$MS/build-600/bin/clang}
SDK=${SDK:-$MS/x/MacOSX10.6/root/SDKs/MacOSX10.6.sdk}
OUT=$1; shift
mkdir -p "$OUT"

FILES=${*:-"tier0/assert_dialog tier0/commandline tier0/cpu tier0/cpu_posix tier0/cpu_usage tier0/dbg
 tier0/dynfunction tier0/fasttimer tier0/mem tier0/minidump tier0/platform_posix tier0/pme_posix
 tier0/progressbar tier0/security tier0/systeminformation tier0/thread tier0/threadtools
 tier0/tier0_strtools tier0/tslist tier0/vcrmode_posix tier0/vprof
 vstdlib/cvar vstdlib/jobthread vstdlib/KeyValuesSystem vstdlib/osversion vstdlib/random vstdlib/vcover
 vstdlib/coroutine"}

DEFS="-DVPC -DNDEBUG -DGNUC -DPOSIX -D_POSIX -DCOMPILER_GCC -D_DLL_EXT=.dylib -D_OSX -DOSX
 -D_DARWIN_UNLIMITED_SELECT -DFD_SETSIZE=10240 -DOVERRIDE_V_DEFINES -DFRAME_POINTER_OMISSION_DISABLED
 -DNO_MALLOC_OVERRIDE=1 -DVPROF_LEVEL=1 -DNO_HOOK_MALLOC=1 -DCPP11_NO_LIBCXX
 -Usprintf -Ustrncpy -UPROTECTED_THINGS_ENABLE -DWAF_CFLAGS=\"\" -DWAF_LDFLAGS=\"\""
# Quoted sibling includes inside public/tier1 bypass -I overrides, so shadow the
# whole directory with the overrides applied.
SH="$OUT/.shadow"; rm -rf "$SH"; mkdir -p "$SH"
cp -R "$SRC/public/tier1" "$SH/tier1"; cp -R "$HERE/override/." "$SH/"
INC="-I$SH -I$SRC/common -I$SRC/public -I$SRC/public/tier0 -I$SRC/public/tier1"
BASE="-x c++ -arch i386 -mmacosx-version-min=10.6 -isysroot $SDK -std=gnu++11 -stdlib=libstdc++
 -fmessage-length=0 -fpascal-strings -fasm-blocks -fstrict-aliasing -O2 -g
 -fvisibility=hidden -fvisibility-inlines-hidden -ffast-math -ftree-vectorize -ftemplate-depth=512
 -Qunused-arguments -w"

fail=0
for f in $FILES; do
  lib=${f%%/*}; name=${f##*/}
  case $lib in
    tier0) d="-DTIER0_DLL_EXPORT -DDLLNAME=tier0 -I$SRC/tier0" ;;
    vstdlib) d="-DVSTDLIB_DLL_EXPORT -DDLLNAME=vstdlib -I$SRC/vstdlib -I$SRC/public/vstdlib" ;;
  esac
  # coroutine.cpp is built as the static lib coroutine_osx (-fno-stack-protector, no DLLNAME)
  [ "$f" = vstdlib/coroutine ] && d="-DVSTDLIB_DLL_EXPORT -fno-stack-protector -I$SRC/public/vstdlib"
  "$CLANG" $BASE $DEFS $d $INC -c "$SRC/$f.cpp" -o "$OUT/$name.o" 2> "$OUT/$name.err" \
    || { echo "COMPILE FAIL $f: $(grep -m1 error: "$OUT/$name.err")"; fail=1; }
done
exit $fail
