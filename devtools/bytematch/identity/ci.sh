#!/bin/sh
# Rebuild every module listed in identical.json and cmp it against retail.
# Exit status 0 only if all of them are byte-identical.
#
#   devtools/bytematch/identity/ci.sh [BUILD_DIR]     (default: build-retail)
#
# BUILD_DIR must be the directory of the last `waf configure`. If it has never
# been configured, it is configured with --retail-osx32 and only the modules the
# identical ones need.
set -e
cd "$(dirname "$0")/../../.."
OUT=${1:-build-retail}
if [ ! -f "$OUT/c4che/_cache.py" ]; then
	python3 waf configure --retail-osx32 -T release --prefix='' -o "$OUT" \
		--retail-modules=tier0,tier1,mathlib,tier2,tier3,vstdlib,appframework,bitmap,vtf,utils/vtex,inputsystem,video,video/video_quicktime,soundemittersystem,scenefilecache,utils/bugreporter_public,utils/bugreporter_filequeue,materialsystem/shaderlib,materialsystem/stdshaders,engine/voice_codecs/miles
fi
python3 waf build
python3 waf retail_linkdb
exec devtools/bytematch/identity/identity.py --identical --build-dir "$OUT"
