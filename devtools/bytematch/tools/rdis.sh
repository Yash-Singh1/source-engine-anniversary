#!/bin/bash
# Print the retail disassembly of one function: rdis.sh tier0|vstdlib MANGLED_OR_REGEX
DEPOT=/Volumes/Expansion/mac-ports/hl2-anniversary-depot/depots/223/19307283/bin
BIN=$DEPOT/lib$1.dylib
sym=$(nm "$BIN" | awk '{print $3}' | grep -E -m1 "^_?$2$" || nm "$BIN" | awk '{print $3}' | grep -E -m1 "$2")
[ -z "$sym" ] && { echo "no symbol matching $2" >&2; exit 1; }
otool -tV -p "$sym" "$BIN" | c++filt | awk 'NR<=3 {print; next} /^[^0-9\t ].*:$/ {exit} {print}'
