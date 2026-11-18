#!/bin/bash
# Fetch and build ld64 from cctools-port as a native host linker with i386 support.
#
# Usage: build-ld64.sh [options] <version>      (242, 241.9, 236.3)
#   --prefix DIR        toolchain root (default $BYTEMATCH_TOOLCHAIN)
#   --work DIR          scratch (default <prefix>/work)
#   --from-build DIR    skip building; install from an existing cctools-port
#                       checkout whose ld64 was already built
#
# Installs <prefix>/ld64-<version>/bin/{ld,dyldinfo,machocheck}.
# Builds unmodified; cctools-port's own configure handles arm64 hosts.
set -euo pipefail
HERE=$(cd "$(dirname "$0")" && pwd)
. "$HERE/versions.sh"

PREFIX=$BYTEMATCH_TOOLCHAIN WORK= FROM_BUILD= V=
while [ $# -gt 0 ]; do
	case $1 in
	--prefix) PREFIX=$2; shift 2 ;;
	--work) WORK=$2; shift 2 ;;
	--from-build) FROM_BUILD=$2; shift 2 ;;
	-h|--help) sed -n '2,11p' "$0"; exit 0 ;;
	*) V=$1; shift ;;
	esac
done
V=${V:-$DEFAULT_LD64}
BRANCH=$(ld64_branch "$V") || { echo "unknown ld64 version $V" >&2; exit 1; }
COMMIT=$(ld64_commit "$V")
WORK=${WORK:-$PREFIX/work}
DEST=$PREFIX/ld64-$V
mkdir -p "$WORK"

install_from() {
	local c=$1/cctools/ld64/src
	[ -x "$c/ld/ld" ] || { echo "no ld in $c/ld" >&2; exit 1; }
	rm -rf "$DEST"
	mkdir -p "$DEST/bin"
	cp "$c/ld/ld" "$DEST/bin/ld"
	for t in dyldinfo machocheck unwinddump ObjectDump; do
		[ -x "$c/other/$t" ] && cp "$c/other/$t" "$DEST/bin/" || true
	done
	echo "$V $COMMIT" > "$DEST/VERSION"
	"$DEST/bin/ld" -v 2>&1 | head -1
	echo "installed $DEST"
}

if [ -n "$FROM_BUILD" ]; then
	install_from "$FROM_BUILD"
	exit 0
fi

SRC=$WORK/cctools-$V
rm -rf "$SRC"
git init -q "$SRC"
git -C "$SRC" remote add origin "$CCTOOLS_REPO"
git -C "$SRC" fetch -q --depth 1 origin "$COMMIT"
git -C "$SRC" checkout -q FETCH_HEAD
test "$(git -C "$SRC" rev-parse HEAD)" = "$COMMIT"

cd "$SRC/cctools"
[ -x configure ] || ./autogen.sh
./configure --prefix="$SRC/inst" --target=i386-apple-darwin13 \
	CC=/usr/bin/clang CXX=/usr/bin/clang++ \
	CFLAGS="-w -O2" CXXFLAGS="-w -O2 -std=gnu++11" > "$SRC/configure.log" 2>&1
# ld64 only; the rest of cctools (as, ar, ...) is not needed.
make -C ld64 -j "$BYTEMATCH_JOBS" > "$SRC/make.log" 2>&1 || { tail -30 "$SRC/make.log"; exit 1; }
install_from "$SRC"
