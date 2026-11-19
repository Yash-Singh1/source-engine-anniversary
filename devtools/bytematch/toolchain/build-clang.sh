#!/bin/bash
# Fetch, patch and build an Apple open-source clang, or upstream clang at an
# llvm.org svn revision, as a native host (arm64 or x86_64 macOS) compiler
# that targets i386-apple-darwin.
#
# Usage: build-clang.sh [options] <version>     (600.0.57, 600.0.54, 600.0.51, 503.0.38, r147225)
#   --prefix DIR        toolchain root (default $BYTEMATCH_TOOLCHAIN)
#   --work DIR          download/build scratch (default <prefix>/work)
#   --tarball FILE      use a local copy of the release tarball (still hash-checked)
#   --from-build DIR    skip building; install from an existing build tree that
#                       was configured from the same sources
#
# Installs <prefix>/clang-<version>/bin/{clang,clang++} and lib/clang/*/include.
# patches/clang-<version>-*.patch apply to that version only and are listed
# in <prefix>/clang-<version>/PATCHES.
set -euo pipefail
HERE=$(cd "$(dirname "$0")" && pwd)
. "$HERE/versions.sh"

PREFIX=$BYTEMATCH_TOOLCHAIN WORK= TARBALL= FROM_BUILD= V=
while [ $# -gt 0 ]; do
	case $1 in
	--prefix) PREFIX=$2; shift 2 ;;
	--work) WORK=$2; shift 2 ;;
	--tarball) TARBALL=$2; shift 2 ;;
	--from-build) FROM_BUILD=$2; shift 2 ;;
	-h|--help) sed -n '2,15p' "$0"; exit 0 ;;
	*) V=$1; shift ;;
	esac
done
V=${V:-$DEFAULT_CLANG}
SHA=$(clang_sha256 "$V") || { echo "unknown clang version $V" >&2; exit 1; }
SVN_COMMIT=$(llvm_svn_commit "$V" || true)
WORK=${WORK:-$PREFIX/work}
DEST=$PREFIX/clang-$V
mkdir -p "$WORK" "$PREFIX"
VERSION_PATCHES=$(ls "$HERE/patches/clang-$V-"*.patch 2>/dev/null || true)

install_from() {
	local b=$1
	[ -x "$b/bin/clang" ] || { echo "no clang in $b/bin" >&2; exit 1; }
	rm -rf "$DEST"
	mkdir -p "$DEST/bin" "$DEST/lib"
	cp "$b/bin/clang" "$DEST/bin/clang"
	ln -sf clang "$DEST/bin/clang++"
	cp -R "$b/lib/clang" "$DEST/lib/"
	echo "$V" > "$DEST/VERSION"
	for p in $VERSION_PATCHES; do basename "$p"; done > "$DEST/PATCHES"
	"$DEST/bin/clang" --version | head -1
	echo "installed $DEST"
}

if [ -n "$FROM_BUILD" ]; then
	install_from "$FROM_BUILD"
	exit 0
fi

TB=$WORK/clang-$V.tar.gz
if [ -n "$TARBALL" ]; then
	cp "$TARBALL" "$TB"
elif [ ! -f "$TB" ]; then
	if [ -n "$SVN_COMMIT" ]; then
		URL=$LLVM_PROJECT_URL_BASE/$SVN_COMMIT.tar.gz
	else
		URL=$CLANG_URL_BASE/clang-$V.tar.gz
	fi
	curl -fL --retry 3 -o "$TB.part" "$URL"
	mv "$TB.part" "$TB"
fi
echo "$SHA  $TB" | shasum -a 256 -c -

# Both layouts end up as Apple's: the llvm tree in src/, clang in src/tools/clang.
SRC=$WORK/clang-$V
rm -rf "$SRC"
mkdir -p "$SRC"
HOST_FLAGS=
TARGETS=clang
if [ -n "$SVN_COMMIT" ]; then
	top=llvm-project-$SVN_COMMIT
	tar -xzf "$TB" -C "$SRC" --strip-components=1 "$top/llvm" "$top/clang"
	mv "$SRC/llvm" "$SRC/src"
	mv "$SRC/clang" "$SRC/src/tools/clang"
	# 2012 llvm/clang has undefined behaviour (e.g. this == NULL tests) that a
	# current host compiler optimizes into crashes.
	HOST_FLAGS="-fno-delete-null-pointer-checks -fno-strict-aliasing -fwrapv -Wno-c++11-narrowing"
	# Resource headers (lib/clang/*/include) are a separate target before LLVM 3.3.
	TARGETS="clang clang-headers"
else
	tar -xzf "$TB" -C "$SRC" --strip-components=1
	(cd "$SRC" && patch -p1 --forward < "$HERE/patches/clang-intrusiverefcntptr-friend.patch")
fi
for p in $VERSION_PATCHES; do
	(cd "$SRC" && patch -p1 --forward < "$p")
done

# Host-compiler compatibility knobs only; none affect generated i386 code:
#  -D_XLOCALE__REGEX_H_  avoids the SDK's xlocale regex header clashing with llvm's regex
#  HAVE_SANITIZER_MSAN_INTERFACE_H=OFF  the SDK ships the header but old llvm misuses it
#  libcxx/compiler-rt are not needed (we target libstdc++ from the SDK)
BLD=$WORK/build-clang-$V
rm -rf "$BLD"
mkdir -p "$BLD"
cmake -S "$SRC/src" -B "$BLD" -G Ninja \
	-DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++ \
	-DCMAKE_C_FLAGS="-w $HOST_FLAGS" \
	-DCMAKE_CXX_FLAGS="-std=c++11 -w -Wno-error -D_XLOCALE__REGEX_H_ $HOST_FLAGS" \
	-DLLVM_TARGETS_TO_BUILD=X86 \
	-DLLVM_DEFAULT_TARGET_TRIPLE=i386-apple-darwin13 \
	-DLLVM_INCLUDE_TESTS=OFF -DLLVM_INCLUDE_EXAMPLES=OFF -DLLVM_INCLUDE_DOCS=OFF \
	-DLLVM_ENABLE_TERMINFO=OFF -DLLVM_ENABLE_ZLIB=OFF \
	-DLLVM_EXTERNAL_LIBCXX_BUILD=OFF -DLLVM_EXTERNAL_LIBCXXABI_BUILD=OFF \
	-DLLVM_EXTERNAL_COMPILER_RT_BUILD=OFF \
	-DHAVE_SANITIZER_MSAN_INTERFACE_H=OFF
ninja -C "$BLD" -j "$BYTEMATCH_JOBS" $TARGETS
install_from "$BLD"
