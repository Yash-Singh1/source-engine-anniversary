# vtf (libvtf.a)

Retail: `libvtf.a(vtf.o)` in depot 223 `engine.dylib` (82 functions),
`materialsystem.dylib` (84) and vtex_dll. 81/82 vs engine after this lane
(71/82 before). The pristine-misc lane already gave vtf.cpp the leak's 7.4
header handling; retail predates the leak in the places below.

| function | status | note |
|---|---|---|
| GenericAllocateReusableData (inlined into Init, InitLowResImage, SetResourceData, ResourceMemorySection::LoadData, ConvertImageFormat) | edited | no "free if 16x too big" shrink clause |
| FileSize, ImageFileInfo | edited | no null check on the image resource entry |
| Unserialize | edited | retail has the full body (no `UnserializeEx` wrapper, no `nForceFlags`, no 32 MB size guard) |
| LoadImageData | edited | the pre-streaming loader: reads `nFacesToRead` faces per frame, no stream settings or mip clamps. The fork body stays under `#ifndef BYTEMATCH_RETAIL`; the leak's streaming body is gone |
| ConvertImageFormat | edited | `MemAllocScratch` for up to 200000 bytes, `new[]` above; no DUDV `Assert` |
| GenerateSpheremap | edited | `MemAllocScratch`/`MemFreeScratch` |
| PostProcess | differs | calls CVTFTexture's first own virtual at slot 55 (0xdc); ours is at 57 because our `IVTFTexture` has `UnserializeEx` and `GetMipmapRange`. Headers lane: retail IVTFTexture is the SDK 2013 55-slot layout |
| CDatadescGeneratedNameHolder::~CDatadescGeneratedNameHolder (vtex/materialsystem copies) | edited | public/datamap.h: scalar `delete` as in the leak and SDK 2013 |
