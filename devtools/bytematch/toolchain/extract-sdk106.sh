#!/bin/bash
# Extract MacOSX10.6.sdk from a Mac OS X 10.6 Snow Leopard retail install image
# (Xcode 3.2 lives in "Optional Installs/Xcode.mpkg"), then make an i386-only copy.
#
# Usage: extract-sdk106.sh [--prefix DIR] <Snow_Leopard.iso | MacOSX10.6.pkg>
#
# Produces:
#   <prefix>/sdk/MacOSX10.6.sdk        pristine (fat i386/x86_64/ppc)
#   <prefix>/sdk/MacOSX10.6-i386.sdk   Mach-O files thinned to i386
# The thinned copy exists because a native arm64 ld64 mmaps whole files with
# 16 KB pages and fails (EINVAL) on fat slices at 4 KB-aligned offsets.
set -euo pipefail
HERE=$(cd "$(dirname "$0")" && pwd)
. "$HERE/versions.sh"

PREFIX=$BYTEMATCH_TOOLCHAIN SRC=
while [ $# -gt 0 ]; do
	case $1 in
	--prefix) PREFIX=$2; shift 2 ;;
	-h|--help) sed -n '2,11p' "$0"; exit 0 ;;
	*) SRC=$1; shift ;;
	esac
done
[ -n "$SRC" ] || { sed -n '5p' "$0"; exit 1; }
OUT=$PREFIX/sdk
mkdir -p "$OUT"
TMP=$(mktemp -d "${TMPDIR:-/tmp}/sdk106.XXXXXX")
MNT=
cleanup() {
	[ -n "$MNT" ] && hdiutil detach -quiet "$MNT" 2>/dev/null || true
	rm -rf "$TMP"
}
trap cleanup EXIT

case $SRC in
*.pkg) PKG=$SRC ;;
*)
	MNT=$TMP/mnt
	mkdir -p "$MNT"
	hdiutil attach -readonly -nobrowse -noverify -mountpoint "$MNT" "$SRC" >/dev/null
	PKG=$(find "$MNT"/Optional* -maxdepth 3 -name MacOSX10.6.pkg -print -quit 2>/dev/null || true)
	[ -n "$PKG" ] || { echo "MacOSX10.6.pkg not found in image" >&2; exit 1; }
	;;
esac

mkdir -p "$TMP/pkg" "$TMP/root"
(cd "$TMP/pkg" && xar -xf "$PKG")
(cd "$TMP/root" && { gzip -dc ../pkg/Payload 2>/dev/null || bzip2 -dc ../pkg/Payload; } | cpio -id --quiet 2>/dev/null) || true
SDK=$TMP/root/SDKs/MacOSX10.6.sdk
[ -f "$SDK/usr/lib/libSystem.B.dylib" ] || { echo "payload did not contain MacOSX10.6.sdk" >&2; exit 1; }

rm -rf "$OUT/MacOSX10.6.sdk" "$OUT/MacOSX10.6-i386.sdk"
mv "$SDK" "$OUT/MacOSX10.6.sdk"
cp -R "$OUT/MacOSX10.6.sdk" "$OUT/MacOSX10.6-i386.sdk"
python3 "$HERE/thin-i386.py" "$OUT/MacOSX10.6-i386.sdk"
echo "SDK: $OUT/MacOSX10.6.sdk"
echo "SDK (i386): $OUT/MacOSX10.6-i386.sdk"
