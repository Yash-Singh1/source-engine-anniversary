#!/bin/sh
set -e

git submodule update --init
sudo apt-get update
sudo apt-get install -y libbz2-dev

python3 ./waf configure -T release --sanitize=address,undefined --disable-warns --tests --prefix=out/ "$@"
python3 ./waf install
cd out
LD_LIBRARY_PATH=bin/ ./unittest
