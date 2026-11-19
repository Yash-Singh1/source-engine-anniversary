#!/bin/bash
# Fetch, patch and build an Apple open-source gcc (cc1, cc1plus and the driver)
# as an x86_64 macOS host compiler that targets i686-apple-darwin, plus the
# cctools i386 assembler it drives. On arm64 Macs the gcc binaries run under
# Rosetta (old gcc has no arm64 host support); the assembler is native.
#
# Usage: build-gcc.sh [options] <version>     (5666.3, 5646, 5564, 5484)
#   --prefix DIR        toolchain root (default $BYTEMATCH_TOOLCHAIN)
#   --work DIR          download/build scratch (default <prefix>/work)
#   --keep              keep the build tree after installing
#
# Installs <prefix>/gcc-<version>/bin/{gcc,g++} (no -arch: they target i386
# only), libexec/gcc/<target>/<basever>/{cc1,cc1plus,as} and the compiler's own
# headers. patches/gcc-<version>-*.patch apply to that version only.
# Pass -isysroot to compile; the C++ headers come from the SDK.
set -euo pipefail
HERE=$(cd "$(dirname "$0")" && pwd)
. "$HERE/versions.sh"

PREFIX=$BYTEMATCH_TOOLCHAIN WORK= KEEP= V=
while [ $# -gt 0 ]; do
	case $1 in
	--prefix) PREFIX=$2; shift 2 ;;
	--work) WORK=$2; shift 2 ;;
	--keep) KEEP=1; shift ;;
	-h|--help) sed -n '2,15p' "$0"; exit 0 ;;
	*) V=$1; shift ;;
	esac
done
[ -n "$V" ] || { sed -n '7p' "$0"; exit 1; }
REPO=$(gcc_repo "$V") || { echo "unknown gcc version $V" >&2; exit 1; }
SHA=$(gcc_sha256 "$V")
TARGET=$(gcc_target "$V")
WORK=${WORK:-$PREFIX/work}
DEST=$PREFIX/gcc-$V
mkdir -p "$WORK"
VERSION_PATCHES=$(ls "$HERE/patches/gcc-$V-"*.patch 2>/dev/null || true)

TB=$WORK/$REPO.tar.gz
if [ ! -f "$TB" ]; then
	curl -fL --retry 3 -o "$TB.part" "$GCC_URL_BASE/${REPO%%-*}/archive/refs/tags/$REPO.tar.gz"
	mv "$TB.part" "$TB"
fi
echo "$SHA  $TB" | shasum -a 256 -c -

SRC=$WORK/$REPO
rm -rf "$SRC"
mkdir -p "$SRC"
tar -xzf "$TB" -C "$SRC" --strip-components=1
for p in $VERSION_PATCHES; do
	(cd "$SRC" && patch -p1 --forward < "$p")
done
BASEVER=$(cat "$SRC/gcc/BASE-VER" 2>/dev/null ||
	sed -n 's/.*version_string\[\] = "\([0-9.]*\).*/\1/p' "$SRC/gcc/version.c")
MAJ=${BASEVER%.*}
case $MAJ in 4.0) CXXINC=4.0.0 ;; *) CXXINC=$BASEVER ;; esac

# cctools i386 assembler (GNU as 1.38 derived), from the cctools-port commit
# build-ld64.sh pins for ld64-242. Built natively.
AS_SRC=$WORK/cctools-as-$(ld64_commit "$GCC_AS_LD64")
if [ ! -x "$AS_SRC/cctools/as/i386/i386-as" ]; then
	rm -rf "$AS_SRC"
	git init -q "$AS_SRC"
	git -C "$AS_SRC" remote add origin "$CCTOOLS_REPO"
	git -C "$AS_SRC" fetch -q --depth 1 origin "$(ld64_commit "$GCC_AS_LD64")"
	git -C "$AS_SRC" checkout -q FETCH_HEAD
	(cd "$AS_SRC/cctools" && { [ -x configure ] || ./autogen.sh; } &&
		./configure --target=i386-apple-darwin13 CC=/usr/bin/clang CXX=/usr/bin/clang++ \
			CFLAGS="-w -O2" CXXFLAGS="-w -O2 -std=gnu++11" &&
		make -C libstuff -j "$BYTEMATCH_JOBS" && make -C as -j "$BYTEMATCH_JOBS") \
		> "$AS_SRC/build.log" 2>&1 || { tail -30 "$AS_SRC/build.log"; exit 1; }
fi

# Host-compiler compatibility knobs only (modern clang rejects K&R-era C by
# default); none affect the code gcc generates. The SDK only supplies the
# system limits.h check for gcc's own limits.h.
SDK=$PREFIX/sdk/MacOSX10.6.sdk
HOSTCC="/usr/bin/clang -arch x86_64"
HOSTCFLAGS="-O2 -w -std=gnu89 -fcommon -Wno-error=implicit-function-declaration -Wno-error=implicit-int \
-Wno-error=int-conversion -Wno-error=incompatible-pointer-types -Wno-error=incompatible-function-pointer-types"
# gcc 4.0's C parser needs bison 2.x (macOS ships 2.3); newer bison drops YYLEX.
[ -x /usr/bin/bison ] && export BISON=/usr/bin/bison
# Same Darwin version on host and target, so configure treats the compiler as
# native (system headers from the -isysroot SDK) rather than as a cross compiler.
HOST=x86_64-apple-${TARGET#*-apple-}
BLD=$WORK/build-gcc-$V
rm -rf "$BLD"
mkdir -p "$BLD"
cd "$BLD"
CC="$HOSTCC" CXX="/usr/bin/clang++ -arch x86_64" CFLAGS="$HOSTCFLAGS" \
"$SRC/configure" --build="$HOST" --host="$HOST" --target="$TARGET" \
	--prefix=/usr --disable-checking --disable-werror --disable-nls --disable-bootstrap \
	--enable-languages=c,c++ --with-gxx-include-dir=/usr/include/c++/$CXXINC \
	--program-transform-name="/^[cg][^.-]*\$/s/\$/-$MAJ/" > configure.log 2>&1 ||
	{ tail -20 configure.log; exit 1; }
HOSTLIBS="all-build-libiberty all-libiberty all-libcpp configure-gcc"
[ -d "$SRC/libdecnumber" ] && HOSTLIBS="$HOSTLIBS all-libdecnumber"
make -j "$BYTEMATCH_JOBS" CC="$HOSTCC" CFLAGS="$HOSTCFLAGS" $HOSTLIBS > make-host.log 2>&1 ||
	{ tail -30 make-host.log; exit 1; }
# Apple's installed compilers carry no fixed system headers; skip fixincludes.
mkdir -p gcc/include
make -C gcc -j "$BYTEMATCH_JOBS" CC="$HOSTCC" CFLAGS="$HOSTCFLAGS" SYSTEM_HEADER_DIR="$SDK/usr/include" \
	STMP_FIXINC= cc1 cc1plus xgcc g++ stmp-int-hdrs > make-gcc.log 2>&1 || { tail -30 make-gcc.log; exit 1; }

LIBEXEC=$DEST/libexec/gcc/$TARGET/$BASEVER
INC=$DEST/lib/gcc/$TARGET/$BASEVER/include
rm -rf "$DEST"
mkdir -p "$DEST/bin" "$LIBEXEC" "$INC"
cp gcc/xgcc "$DEST/bin/gcc"
cp gcc/g++ "$DEST/bin/g++"
cp gcc/cc1 gcc/cc1plus "$LIBEXEC/"
cp "$AS_SRC/cctools/as/i386/i386-as" "$LIBEXEC/as"
cp -R gcc/include/ "$INC/"
cp "$SRC/gcc/gsyslimits.h" "$INC/syslimits.h"
# Mirror Apple's install: build_gcc adds more-hdrs/ headers gcc doesn't install
# itself (stdint.h, ppc_intrinsics.h); unwind.h and varargs.h are not shipped.
for h in "$SRC"/more-hdrs/*.h; do
	case $(basename "$h") in assert.h|inttypes.h|varargs.h) continue ;; esac
	[ -f "$INC/$(basename "$h")" ] || cp "$h" "$INC/"
done
rm -f "$INC/unwind.h" "$INC/varargs.h"
echo "$V" > "$DEST/VERSION"
for p in $VERSION_PATCHES; do basename "$p"; done > "$DEST/PATCHES"
"$DEST/bin/gcc" --version | head -1
echo "installed $DEST"
[ -n "$KEEP" ] || rm -rf "$BLD" "$SRC"
