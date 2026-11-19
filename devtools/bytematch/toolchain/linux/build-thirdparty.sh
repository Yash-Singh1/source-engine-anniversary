#!/bin/sh
# Rebuild the prebuilt archives retail Linux client.so (and engine.so, GameUI.so, replay.so) link
# statically: libjpeg 8b and libpng 1.5.2. Their .comment is "GCC: (crosstool-NG-1.8.1) 4.5.1";
# this builds a stock gcc 4.5.1 (C and C++, cc1/cc1plus only) inside the scout SDK's i386 image,
# then the two libraries with it. zlib 1.2.5 is SDK 2013's libz.a, not rebuilt.
#
#   devtools/bytematch/toolchain/linux/build-thirdparty.sh
#
# Output: $PREFIX/lib/{libjpeg,libpng}.a (retail_linux32.py's --retail-linux32-thirdparty).
set -e
HERE=$(cd "$(dirname "$0")" && pwd)
TREE=$(cd "$HERE/../../../.." && pwd)
CACHE=${BYTEMATCH_TOOLCHAIN:-$HOME/.cache/source-bytematch/toolchain}
PREFIX=${PREFIX:-$CACHE/thirdparty-linux32}
WORK=${WORK:-$CACHE/work/linux32}
IMAGE=registry.gitlab.steamos.cloud/steamrt/scout/sdk/i386@sha256:88e7e1f74723ed7be3e73a1c4decc250b978fa62789a902b17c6ea5266ef3cce

fetch() {  # url sha256
	f=$WORK/$(basename "$1")
	[ -f "$f" ] || curl -fsSL -o "$f.part" "$1" && { [ -f "$f" ] || mv "$f.part" "$f"; }
	echo "$2  $f" | shasum -a 256 -c - >/dev/null || { echo "checksum mismatch: $f" >&2; exit 1; }
}

mkdir -p "$WORK" "$PREFIX/lib"
fetch https://ftp.gnu.org/gnu/gcc/gcc-4.5.1/gcc-core-4.5.1.tar.bz2 b57b37a83840b134572b042f170d7a9429863ba0308b56b403b35de6f0e0526a
fetch https://ftp.gnu.org/gnu/gcc/gcc-4.5.1/gcc-g++-4.5.1.tar.bz2 a5349ea8d531aa0a248484f426513e5d05b12915e5412d332c278f65d272ce48
fetch https://gcc.gnu.org/pub/gcc/infrastructure/gmp-4.3.2.tar.bz2 936162c0312886c21581002b79932829aa048cfaf9937c6265aeaa14f1cd1775
fetch https://gcc.gnu.org/pub/gcc/infrastructure/mpfr-2.4.2.tar.bz2 c7e75a08a8d49d2082e4caee1591a05d11b9d5627514e678f02d66a124bcf2ba
fetch https://gcc.gnu.org/pub/gcc/infrastructure/mpc-0.8.1.tar.gz e664603757251fd8a352848276497a4c79b7f8b21fd8aedd5cc0598a38fee3e4
fetch https://download.sourceforge.net/libpng/libpng-1.5.2.tar.gz 1ed281810348dbb61d02465c0dff854c17b826f5c5d76cce32bc0e6adca02156

cat > "$WORK/inner.sh" <<'EOF'
#!/bin/sh
set -e
if [ ! -x /w/gcc451/bin/gcc ]; then
	rm -rf /tmp/g && mkdir -p /tmp/g && cd /tmp/g
	tar xjf /w/gcc-core-4.5.1.tar.bz2 && tar xjf /w/gcc-g++-4.5.1.tar.bz2
	cd gcc-4.5.1
	tar xjf /w/gmp-4.3.2.tar.bz2 && mv gmp-4.3.2 gmp
	tar xjf /w/mpfr-2.4.2.tar.bz2 && mv mpfr-2.4.2 mpfr
	tar xzf /w/mpc-0.8.1.tar.gz && mv mpc-0.8.1 mpc
	mkdir ../build && cd ../build
	CC=gcc-4.6 CFLAGS="-O2 -fgnu89-inline" ../gcc-4.5.1/configure --prefix=/w/gcc451 \
		--enable-languages=c,c++ --disable-bootstrap --disable-multilib --disable-nls \
		--disable-libmudflap --disable-libgomp --disable-libssp \
		--with-pkgversion=crosstool-NG-1.8.1 MAKEINFO=true
	make -j4 all-gcc MAKEINFO=true
	make install-gcc MAKEINFO=true
fi
@LIBS@
EOF
docker run --rm --platform linux/amd64 -v "$WORK:/w" -v "$TREE:/t:ro" -v "$PREFIX:/out" "$IMAGE" sh /w/inner.sh
ls -l "$PREFIX/lib"
