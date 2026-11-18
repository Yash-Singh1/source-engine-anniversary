# scenefilecache

Retail: depot 223 `bin/scenefilecache.dylib`, object `SceneFileCache.o` plus
tier1 members (checksum_crc, interface, lzmaDecoder, strtools, utlbuffer; owned
by the tier1 lane).

Verification: clang-600.0.57 with the TARGET.md Release flags, compared per
N_OSO object (symdiff normalization, bytecheck rule). "hdrfix" means compiled
with SDK 2013 `public/tier1/lzmaDecoder.h` (non-static `CLZMA` API), which the
tier1 lane owns.

Before: 14/16 byte-identical. After: 14/16 with the current tier1 header,
16/16 with hdrfix.

| function | status | note |
|---|---|---|
| CSceneFileCache::Connect | identical | byte |
| CSceneFileCache::Disconnect | identical | byte |
| CSceneFileCache::Init | identical | byte |
| CSceneFileCache::Shutdown | identical | byte |
| CSceneFileCache::Reload | identical | byte |
| CSceneFileCache::GetSceneBufferSize | edited | inlines GetSceneDataFromImage; byte with hdrfix |
| CSceneFileCache::GetSceneDataFromImage | edited | local `CLZMA` instance (retail passes `this`); `MIN()` macro instead of `std::min`; byte with hdrfix |
| CSceneFileCache::FindSceneInImage | identical | byte |
| CSceneFileCache::GetSceneData | identical | byte |
| CSceneFileCache::GetSceneCachedData | identical | byte |
| CSceneFileCache::GetSceneCachedSound | identical | byte |
| CSceneFileCache::GetSceneString | identical | byte |
| CSceneFileCache::~CSceneFileCache | identical | byte (implicit) |
| CBaseAppSystem<ISceneFileCache>::QueryInterface | identical | byte (header template) |
| __CreateCSceneFileCacheISceneFileCache_interface | identical | byte |
| __GLOBAL__I_a | identical | byte |
| IsBufferBinaryVCD | uncertain | not in retail; unused global, a dead-stripped copy would look the same, so kept |

Link composition: retail has no `memoverride.o` (NO_MALLOC_OVERRIDE); our
wscript adds `public/tier0/memoverride.cpp`. Retail load commands: libtier0,
libvstdlib, libiconv, libstdc++, libSystem; our wscript does not list vstdlib.
