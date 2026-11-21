# Source Engine - HL2 Anniversary Edition

This branch is a decomp of HL2 Anniversary Edition built off the TF2 2018 Source Engine leak and `nillerusr`'s build system ported to MacOS.

## Building

```sh
brew install pkg-config python3 sdl2 freetype fontconfig jpeg-turbo libpng
python3 ./waf configure -T release --prefix='' --build-games=hl2_complete
python3 ./waf build
python3 ./waf install --destdir="/path/to/Half-Life 2"
```

## Launching

Go to your destination directory and run:

```sh
./hl2_launcher -game hl2_complete
```
