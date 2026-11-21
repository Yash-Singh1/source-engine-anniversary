# Source Engine - HL2 Anniversary Edition

This branch is a decomp of HL2 Anniversary Edition built off the TF2 2018 Source Engine leak and `nillerusr`'s build system ported to MacOS.

## Building & Installing

```sh
brew install pkg-config python3 sdl2 freetype fontconfig jpeg-turbo libpng
python3 ./waf configure -T release --prefix='' --build-games=hl2_complete
python3 ./waf build
python3 ./waf install --destdir="$HOME/Library/Application Support/Steam/steamapps/common/Half-Life 2" # replace destdir with whereever your steam installation directory for HL2 is stored if you want to install it there
```

## Launching

Press **Play** in Steam, or run from the game directory:

```sh
./hl2_launcher -game hl2_complete
```
