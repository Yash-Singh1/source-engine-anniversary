# dmxloader (libdmxloader.a)

Engine has 99/99, but it dead-strips the unserialize path. The anniversary hl2
client (depot 223 `hl2/bin/client.dylib`, 83 functions in scope) and the 2014
hl2mp client (85) keep it; both match after the leftovers lane. dmxloader.cpp
has no fork changes, so these functions are the leak being newer than retail.

| function | status | note |
|---|---|---|
| ReadDMXHeader | edited | `sscanf` straight into the caller's buffers (no temporaries or `V_strncpy`), `header[]` not zero-filled |
| CDmxSerializer::Unserialize | edited | string table read with no `nStrings > 0` test (two `stackalloc`s always); element loop keyed on `bReadStringTable` (encoding version) rather than `stringTable`; no `nElementCount < 0` check; `GetString` with explicit sizes |
| CDmxSerializer::UnserializeAttributes | edited | `GetString( nameBuf, sizeof( nameBuf ) )`: retail calls `GetString(char*, int)` with 1024, while ours resolved to the defaulted non-template overload with 0 |
| UnserializeDMX | edited | followed from ReadDMXHeader's frame |
