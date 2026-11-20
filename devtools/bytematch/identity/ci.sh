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
# waf build acts on the out_dir of its lock file ($WAFLOCK, default
# .lock-waf_<platform>_build); when unset, use the lock configured for $OUT.
if [ -z "$WAFLOCK" ] && [ -d "$OUT" ]; then
	abs=$(cd "$OUT" && pwd)
	for f in .lock-waf*; do
		if grep -qxF "out_dir = '$abs'" "$f" 2>/dev/null; then
			export WAFLOCK=$f
			break
		fi
	done
fi
if [ ! -f "$OUT/c4che/_cache.py" ] || ! grep -q "sdk2013mp" "$OUT/c4che/_cache.py" 2>/dev/null; then
	python3 waf configure --retail-osx32 -T release --prefix='' -o "$OUT" \
		--build-games=hl2mp \
		--retail-modules=tier0,tier1,mathlib,tier2,tier3,vstdlib,appframework,bitmap,vtf,utils/vtex,inputsystem,video,video/video_quicktime,soundemittersystem,scenefilecache,utils/bugreporter_public,utils/bugreporter_filequeue,materialsystem/shaderlib,materialsystem/stdshaders,engine/voice_codecs/miles,launcher,launcher_main,togl,vpklib,utils/vpk,filesystem,sourcevr,utils/bsppack,vgui2/src,vgui2/vgui_controls,serverbrowser,materialsystem/shaderapiempty,vgui2/chromehtml,utils/bzip2,replay/common,replay,vgui2/vgui_surfacelib,vguimatsurface,materialsystem/shaderapidx9,datacache
fi
python3 devtools/bytematch/toolchain/win32/winline.py --check
python3 waf build
python3 waf retail_linkdb
exec devtools/bytematch/identity/identity.py --identical --build-dir "$OUT"
