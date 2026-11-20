#!/bin/sh
# Rebuild the prebuilt archives retail Linux client.so, engine.so, GameUI.so, replay.so and
# vguimatsurface.so link statically: libjpeg 8b and libpng 1.5.2. Their .comment is
# "GCC: (crosstool-NG-1.8.1) 4.5.1"; this builds a stock gcc 4.5.1 (C only) inside the scout
# SDK's i386 image, then the two libraries with it against glibc 2.3.6's headers. zlib 1.2.5
# is SDK 2013's libz.a, not rebuilt (its headers are libpng's only use of it here).
#
#   devtools/bytematch/toolchain/linux/build-thirdparty.sh
#
# What the retail code shows (client.so, depot 420; byte-match-survey/lane-linux3.md):
# - both libraries: -march=pentium4 (the toolchain's default arch; generic, i686, core2 or
#   prescott tuning change constant-multiply and scheduling choices in most functions);
# - libpng: autoconf's -O2, PIC (__i686.get_pc_thunk.bx), exported; atof() becomes
#   __strtod_internal, the inline glibc dropped in 2.7, so the toolchain's sysroot is
#   older: Debian etch's glibc 2.3.6 headers reproduce it;
# - libjpeg: README.valve's CFLAGS=-fvisibility=hidden replaces autoconf's -g -O2, so -O0,
#   hidden, not PIC (retail client.so has DT_TEXTREL); upstream 8b sources, not
#   utils/jpeglib (Valve's copy drops `register`, which -O0 code keeps), with Valve's
#   configure output public/jpeglib/jconfig.h.
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
	[ -f "$f" ] || { curl -fsSL -o "$f.part" "$1" && mv "$f.part" "$f"; }
	echo "$2  $f" | shasum -a 256 -c - >/dev/null || { echo "checksum mismatch: $f" >&2; exit 1; }
}

mkdir -p "$WORK" "$PREFIX/lib"
fetch https://ftp.gnu.org/gnu/gcc/gcc-4.5.1/gcc-core-4.5.1.tar.bz2 b57b37a83840b134572b042f170d7a9429863ba0308b56b403b35de6f0e0526a
fetch https://gcc.gnu.org/pub/gcc/infrastructure/gmp-4.3.2.tar.bz2 936162c0312886c21581002b79932829aa048cfaf9937c6265aeaa14f1cd1775
fetch https://gcc.gnu.org/pub/gcc/infrastructure/mpfr-2.4.2.tar.bz2 c7e75a08a8d49d2082e4caee1591a05d11b9d5627514e678f02d66a124bcf2ba
fetch https://gcc.gnu.org/pub/gcc/infrastructure/mpc-0.8.1.tar.gz e664603757251fd8a352848276497a4c79b7f8b21fd8aedd5cc0598a38fee3e4
fetch http://archive.debian.org/debian/pool/main/g/glibc/libc6-dev_2.3.6.ds1-13etch10+b1_i386.deb c77d709135162eb7e47b29ca6a1499bc55f5e3820054f2843850a2d3fcbacd70
fetch https://download.sourceforge.net/libpng/libpng-1.5.2.tar.gz 1ed281810348dbb61d02465c0dff854c17b826f5c5d76cce32bc0e6adca02156
fetch https://zlib.net/fossils/zlib-1.2.5.tar.gz 6064e52e513facb0fbb7998c6413406cf253cfb986063d68f4771c2bf7a3f958
fetch https://www.ijg.org/files/jpegsrc.v8b.tar.gz 64be2429346b1aa84a014fc47264661ca2e6c786aea25afecb363d5973b4e6a9

cat > "$WORK/inner.sh" <<'EOF'
#!/bin/sh
set -e
if [ ! -x /w/gcc451/bin/gcc ]; then
	rm -rf /tmp/g && mkdir -p /tmp/g && cd /tmp/g
	tar xjf /w/gcc-core-4.5.1.tar.bz2
	cd gcc-4.5.1
	tar xjf /w/gmp-4.3.2.tar.bz2 && mv gmp-4.3.2 gmp
	tar xjf /w/mpfr-2.4.2.tar.bz2 && mv mpfr-2.4.2 mpfr
	tar xzf /w/mpc-0.8.1.tar.gz && mv mpc-0.8.1 mpc
	mkdir ../build && cd ../build
	CC=gcc-4.6 CFLAGS="-O2 -fgnu89-inline" ../gcc-4.5.1/configure --prefix=/w/gcc451 \
		--enable-languages=c --disable-bootstrap --disable-multilib --disable-nls \
		--disable-libmudflap --disable-libgomp --disable-libssp \
		--with-pkgversion=crosstool-NG-1.8.1 MAKEINFO=true
	make -j4 all-gcc MAKEINFO=true
	make install-gcc MAKEINFO=true
fi
rm -rf /tmp/s && mkdir -p /tmp/s/glibc /tmp/o/png /tmp/o/jpeg && cd /tmp/s
(cd glibc && ar x /w/libc6-dev_2.3.6.ds1-13etch10+b1_i386.deb && tar xzf data.tar.gz ./usr/include)
tar xzf /w/libpng-1.5.2.tar.gz && tar xzf /w/zlib-1.2.5.tar.gz && tar xzf /w/jpegsrc.v8b.tar.gz
GCC=/w/gcc451/bin/gcc
LIBGCC=/w/gcc451/lib/gcc/i686-pc-linux-gnu/4.5.1
# glibc 2.3.6 first; the image's kernel headers (linux/, asm/) after it
SYS="-nostdinc -isystem $LIBGCC/include -isystem $LIBGCC/include-fixed -isystem /tmp/s/glibc/usr/include \
	-idirafter /usr/include -idirafter /usr/include/i386-linux-gnu"
P=/tmp/s/libpng-1.5.2
cp $P/scripts/pnglibconf.h.prebuilt $P/pnglibconf.h
for f in png pngerror pngget pngmem pngpread pngread pngrio pngrtran pngrutil pngset pngtrans pngwio \
		pngwrite pngwtran pngwutil; do
	$GCC -O2 -fPIC -march=pentium4 $SYS -I$P -I/tmp/s/zlib-1.2.5 -c $P/$f.c -o /tmp/o/png/$f.o
done
J=/tmp/s/jpeg-8b
for f in jaricom jcapimin jcapistd jcarith jccoefct jccolor jcdctmgr jchuff jcinit jcmainct jcmarker \
		jcmaster jcomapi jcparam jcprepct jcsample jctrans jdapimin jdapistd jdarith jdatadst jdatasrc \
		jdcoefct jdcolor jddctmgr jdhuff jdinput jdmainct jdmarker jdmaster jdmerge jdpostct jdsample \
		jdtrans jerror jfdctflt jfdctfst jfdctint jidctflt jidctfst jidctint jquant1 jquant2 jutils \
		jmemmgr jmemnobs; do
	$GCC -fvisibility=hidden -march=pentium4 $SYS -I/t/public/jpeglib -I$J -c $J/$f.c -o /tmp/o/jpeg/$f.o
done
rm -f /out/lib/libpng.a /out/lib/libjpeg.a
(cd /tmp/o/png && ar rcs /out/lib/libpng.a png.o pngerror.o pngget.o pngmem.o pngpread.o pngread.o pngrio.o \
	pngrtran.o pngrutil.o pngset.o pngtrans.o pngwio.o pngwrite.o pngwtran.o pngwutil.o)
(cd /tmp/o/jpeg && ar rcs /out/lib/libjpeg.a *.o)
EOF
docker run --rm --platform linux/amd64 -v "$WORK:/w" -v "$TREE:/t:ro" -v "$PREFIX:/out" "$IMAGE" sh /w/inner.sh
ls -l "$PREFIX/lib"
