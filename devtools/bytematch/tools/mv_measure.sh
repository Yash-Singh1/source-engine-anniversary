#!/bin/bash
# Compile the tier0/vstdlib retail object list with macos-cc.sh and write
# bytematch reports against retail libtier0/libvstdlib.
# usage: mv_measure.sh OUTDIR [SRCROOT]
#   HDRFIX=patch  apply a header patch (e.g. lane-mod-small hdrfix.patch) in a
#                 throwaway overlay of public/ before compiling
set -u
HERE=$(cd "$(dirname "$0")" && pwd)
BM=$(cd "$HERE/.." && pwd)
OUT=$1
SRC=${2:-$(cd "$HERE/../../.." && pwd)}
DEPOT=/Volumes/Expansion/mac-ports/hl2-anniversary-depot/depots/223/19307283/bin
MS=/Volumes/Storage/sco/byte-match-survey/mac-scratch
rm -rf "$OUT"; mkdir -p "$OUT/tier0" "$OUT/vstdlib" "$OUT/ov"

# Quoted sibling includes inside public/tier1 bypass -I overrides, so the
# override dir carries the whole of public/tier1 plus the survey overrides.
cp -R "$SRC/public/tier1" "$OUT/ov/tier1"
if [ -n "${HDRFIX:-}" ]; then
  mkdir -p "$OUT/hp/public"
  cp -R "$SRC/public/tier0" "$SRC/public/vstdlib" "$SRC/public/tier1" "$SRC/public/mathlib" \
    "$SRC/public/inputsystem" "$OUT/hp/public/"
  for p in $HDRFIX; do
    (cd "$OUT/hp" && patch -s -p1 < "$p") || echo "HDRFIX: $p failed"
  done
  cp -R "$OUT/hp/public/tier1/." "$OUT/ov/tier1/"
  cp -R "$OUT/hp/public/tier0" "$OUT/hp/public/vstdlib" "$OUT/hp/public/mathlib" "$OUT/ov/"
  cp "$MS/override/mathlib/mathlib.h" "$OUT/ov/mathlib/"
else
  cp -R "$MS/override/." "$OUT/ov/"
fi

T0="tier0/assert_dialog tier0/commandline tier0/cpu tier0/cpu_posix tier0/cpu_usage tier0/dbg
 tier0/dynfunction tier0/fasttimer tier0/mem tier0/minidump tier0/platform_posix tier0/pme_posix
 tier0/progressbar tier0/security tier0/systeminformation tier0/thread tier0/threadtools
 tier0/tier0_strtools tier0/tslist tier0/vcrmode_posix tier0/vprof"
VS="vstdlib/cvar vstdlib/jobthread vstdlib/KeyValuesSystem vstdlib/osversion vstdlib/random vstdlib/vcover
 vstdlib/coroutine"

for f in $T0 $VS; do
  mod=${f%%/*}; name=${f##*/}
  case $f in
    tier0/*) extra=(-DTIER0_DLL_EXPORT -DDLLNAME=tier0 -DUSE_SDL -idirafter "${SDL2_INC:-$SRC/thirdparty/SDL}") ;;
    vstdlib/coroutine) extra=(-DVSTDLIB_DLL_EXPORT -fno-stack-protector -I"$SRC/public/vstdlib") ;;
    vstdlib/*) extra=(-DVSTDLIB_DLL_EXPORT -DDLLNAME=vstdlib -I"$SRC/public/vstdlib") ;;
  esac
  [ -f "$SRC/$f.cpp" ] || { echo "MISSING SOURCE $f"; continue; }
  BM_SRC=$SRC BM_OVERRIDE=$OUT/ov "$BM/macos-cc.sh" -o "$OUT/$mod/$name.o" "$SRC/$f.cpp" -- "${extra[@]}" \
    -DWAF_CFLAGS='""' -DWAF_LDFLAGS='""' > /dev/null 2> "$OUT/$mod/$name.err" \
    || echo "COMPILE FAIL $f: $(grep -m1 error: "$OUT/$mod/$name.err")"
done

for mod in tier0 vstdlib; do
  "$BM/bin/bytematch" report --retail "$DEPOT/lib$mod.dylib" --ours "$OUT/$mod" --out "$OUT/report_$mod" > /dev/null
  python3 - "$OUT/report_$mod.json" "$mod" <<'EOF'
import json, sys
f = json.load(open(sys.argv[1]))["summary"]["functions"]
print(f"{sys.argv[2]:8s} matched {f['matched']}/{f['total']} ({f['pct']}%) exact={f['exact']} "
      f"masked={f['matches_after_masking']} differs={f['differs']} missing={f['missing']}")
EOF
done
