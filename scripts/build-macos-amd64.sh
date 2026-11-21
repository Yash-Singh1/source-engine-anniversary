#!/bin/sh
set -e

git submodule update --init

brew install pkg-config sdl2 freetype fontconfig jpeg-turbo libpng

python3 ./waf configure -T release --prefix='' --build-games=hl2_complete --disable-warns "$@"
python3 ./waf build
