# bsppack (bsplib.o, zip_utils.o)

Retail: depot 223 `bsppack.dylib`, 368 functions. 347/368 after this lane
(341 before), 359/368 with the byteswap.h change listed below.

| function | status | note |
|---|---|---|
| AddLump<T> (9 instantiations), WriteBSPFile, WritePakFileLump | edited, partly differs | `AddLumpInternal` clears SDK 2013's `lump_t::fourCC` bytewise (through a local struct view; our bspfile.h has `uncompressedSize` there). That flips the inliner to retail's split: `AddLumpInternal` inlined into out-of-line `AddLump<T>`. Five instantiations match. `AddLump<dleafambientindex_t>`, `<dleafambientlighting_t>`, `<char>` (vector) and `<unsigned char>` (pointer) still get inlined into their callers in ours, so WriteBSPFile and WritePakFileLump stay larger. Not explained |
| CDatadescGeneratedNameHolder::~CDatadescGeneratedNameHolder | edited | public/datamap.h scalar `delete` |
| CopyLumpInternal<T> (6), LoadBSPFile, AddOcclusionLump, SwapVHV, SwapPakfileLumpToDisk, GetPakFileLump | differs | all match with public/tier1/byteswap.h `SwapBufferToTargetEndian` skipping the `memcpy` when `outputBuffer == inputBuffer` (headers lane) |
| BuildStaticPropNameTable, CGameLump::SwapGameLump | differs | retail `StaticPropLump_t` is SDK 2013 v6 (64 bytes, `GAMELUMP_STATIC_PROPS_VERSION` 6); ours is the fork's v10 struct (84 bytes). public/gamebspfile.h, used by engine/staticpropmgr.cpp |
| __GLOBAL__I_a | differs | `texinfo` grow size: retail `MAX_MAP_TEXINFO` is 16384 (also in WriteBSPFile's error check); ours and SDK 2013 have 12288. public/bspfile.h |
| CByteswap::SwapFieldToTargetEndian (libtier1 byteswap.o) | differs | tier1/byteswap.cpp: retail has no `FIELD_INTEGER64` case (fork addition); same miss in engine |

## Lane residuals3 (368/368)

public/tier1/byteswap.h `SwapBufferToTargetEndian` skips the memcpy when the buffers are
the same (`if ( outputBuffer != inputBuffer ) memcpy`, line-neutral; verified over all
modules and the three games). Retail tier1 does swap `FIELD_INTEGER64` (case value 29);
datamap.h keeps it out of the retail enum, so byteswap.cpp uses `FIELD_VECTOR2D + 1`.
bsplib.cpp has SDK 2013's `GetPlatformMapPath` back (dead-stripped; it fixes the
GCC_except_table numbering and the `.bsp` literal position).

Identity flow: same size; left are the `.` literal's position (retail places it
before `rb`; ours comes from strtools.o's `V_SetExtension`) and the
`___gxx_personality_v0` non-lazy pointer one slot earlier.
