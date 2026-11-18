#!/bin/bash
# Create devtools/bytematch/.venv with the optional capstone decoder.
# bin/bytematch uses this venv automatically when it exists; without it the
# tool runs on any Python 3.9+ (using capstone from the user site if present).
set -euo pipefail
HERE=$(cd "$(dirname "$0")" && pwd)
python3 -m venv "$HERE/.venv"
"$HERE/.venv/bin/pip" install --quiet --upgrade pip
"$HERE/.venv/bin/pip" install --quiet -r "$HERE/requirements.txt"
"$HERE/.venv/bin/python3" -c 'import capstone; print("capstone", capstone.__version__)'
