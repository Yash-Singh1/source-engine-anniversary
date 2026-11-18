#!/usr/bin/env bash
# Compile every retail tier1/mathlib archive member with macos-cc.sh and report against
# the retail module that links it (engine, or hl2 client/server/scenefilecache for members
# engine lacks).
# Usage: tier1_measure.sh SRCROOT OUTDIR [members...]
#   The survey header overrides are disabled unless BM_OVERRIDE is set.
set -u
HERE=$(cd "$(dirname "$0")" && pwd)
BM=$(cd "$HERE/.." && pwd)
if [ -z "${BM_OVERRIDE:-}" ]; then export BM_OVERRIDE=$(mktemp -d); fi
D=/Volumes/Expansion/mac-ports/hl2-anniversary-depot/depots/223/19307283
SDK=/Volumes/Storage/sco/byte-match-survey/mac-scratch/x/MacOSX10.6/root/SDKs/MacOSX10.6.sdk
SRC=$1; OUT=$2; shift 2
ALL="mathlib/bumpvects mathlib/color_conversion mathlib/IceKey mathlib/mathlib_base mathlib/sse
 mathlib/vmatrix mathlib/powsse mathlib/lightdesc mathlib/randsse mathlib/ssenoise
 tier1/bitbuf tier1/byteswap tier1/characterset tier1/checksum_crc tier1/checksum_md5 tier1/commandbuffer
 tier1/convar tier1/datamanager tier1/generichash tier1/ilocalize tier1/interface tier1/KeyValues tier1/lzss
 tier1/mempool tier1/memstack tier1/NetAdr tier1/snappy-sinksource tier1/snappy tier1/strtools tier1/tier1
 tier1/uniqueid tier1/utlbuffer tier1/utlbufferutil tier1/utlstring tier1/utlsymbol tier1/rangecheckedvar
 tier1/stringpool tier1/lzmaDecoder"
declare -A WHERE=( [lightdesc]=hl2/bin/client [powsse]=hl2/bin/client [randsse]=hl2/bin/client
  [ssenoise]=hl2/bin/client [rangecheckedvar]=hl2/bin/client [stringpool]=hl2/bin/server
  [lzmaDecoder]=bin/scenefilecache )
for f in ${*:-$ALL}; do
  n=${f##*/}; m=${WHERE[$n]:-bin/engine}
  mkdir -p "$OUT/${m//\//_}"
  [ -f "$SRC/$f.cpp" ] || { echo "MISSING $f"; continue; }
  BM_SRC=$SRC "$BM/macos-cc.sh" -o "$OUT/${m//\//_}/$n.o" "$SRC/$f.cpp" -- \
    -DBYTEMATCH_RETAIL -Wno-c++11-narrowing -I"$SDK/usr/include/malloc" > /dev/null 2> "$OUT/$n.err" \
    || echo "COMPILE FAIL $f: $(grep -m1 error: "$OUT/$n.err")"
done
for d in "$OUT"/*/; do
  m=$(basename "$d"); m=${m//_//}
  "$BM/bin/bytematch" report --retail "$D/$m.dylib" --ours "$d" --out "$OUT/report_$(basename "$d")" > /dev/null
done
python3 - "$OUT" <<'EOF'
import glob, json, sys, collections
tot = collections.Counter()
for p in sorted(glob.glob(sys.argv[1] + "/report_*.json")):
    r = json.load(open(p))
    print(p.rsplit("/", 1)[-1], json.dumps(r.get("summary", r.get("counts", {})))[:300])
EOF
