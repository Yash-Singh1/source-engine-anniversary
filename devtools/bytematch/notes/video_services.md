# video_services

Retail: depot 223 `bin/video_services.dylib`, object `videoservices.o` plus
`libtier2.a(tier2.o)`, `libtier3.a(tier3.o)` and tier1 members (convar,
interface, strtools, tier1, utlbuffer), which other lanes own.

Built with `USE_SDL` (retail references `SDLMgrInterface001`).

Verification: clang-600.0.57, TARGET.md Release flags, per N_OSO object.
"hdrfix" means compiled with lane-local copies of headers owned by other lanes:
- `tier1/utlmemory.h` without the TF2-branch "exact size if larger" clamp in
  `UtlMemory_CalcNewAllocationCount` (SDK 2013 behaviour);
- `mathlib/vector4d.h` without the fork's `alignas(16)` on `Vector4D`;
- `inputsystem/iinputsystem.h` with the retail anniversary methods appended
  (slots 41-55, ending in `SteamControllerSkipVideo(bool*, bool*)`).

Before: 40/44 byte-identical. After: 40/44 with current headers, 44/44 with
hdrfix.

| function | status | note |
|---|---|---|
| CVideoCommonServices::ProcessFullScreenInput | edited | added `g_pInputSystem->SteamControllerSkipVideo()` (first out-param acts as space, second as escape), after the platform block; Linux build has the same call. Guarded by `INPUTSYSTEM_HAS_STEAMCONTROLLERSKIPVIDEO` until the header gains the method. Byte with hdrfix |
| CUtlMemory<VideoFileExtensionInfo_t,int>::Grow | identical | byte with hdrfix (utlmemory.h) |
| CUtlMemory<CActiveVideoObjectRecord_t,int>::Grow | identical | byte with hdrfix (utlmemory.h) |
| __GLOBAL__I_a | identical | byte with hdrfix (vector4d.h alignment changes `vec4_origin` init codegen) |
| all 40 other CValveVideoServices / CVideoCommonServices functions | identical | byte |
| CValveVideoServices::GetFileExtension, GetIndexForSystem, GetSystemForIndex, IsMatchAnyExtension, DestroyAllVideoInterfaces, SetResult, constructors, CVideoCommonServices::ResetInputHandlerState | identical | inlined everywhere; out-of-line copies are dead-stripped in retail, not ours-only |

Link composition: retail objects are videoservices.o, tier2.o, tier3.o and tier1
members; no `memoverride.o`. Retail links Carbon and iconv; our wscript links
CoreGraphics.
