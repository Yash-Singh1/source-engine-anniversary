#!/bin/sh
# Build the bytematch-wine10 image (see Dockerfile) and, with --fetch, assemble
# MSVC 2013 Update 5 inside it into $BYTEMATCH_TOOLCHAIN (default
# ~/.cache/source-bytematch/toolchain), mounted at /toolchain.
set -e
here=$(cd "$(dirname "$0")" && pwd)
docker build --platform linux/amd64 -t bytematch-wine10 "$here"
if [ "$1" = "--fetch" ]; then
	tc=${BYTEMATCH_TOOLCHAIN:-$HOME/.cache/source-bytematch/toolchain}
	mkdir -p "$tc"
	docker run --rm --platform linux/amd64 -v "$tc:/toolchain" -v "$here/..:/scripts:ro" \
		bytematch-wine10 python3 /scripts/fetch-msvc2013.py
fi
