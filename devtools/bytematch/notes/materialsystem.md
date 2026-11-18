# materialsystem (retail macOS)

Retail: depot 223 `bin/materialsystem.dylib`; own objects listed below (the debug map's
materialsystem.build objects). tier1/tier2/mathlib/bitmap/vtf/shaderlib members belong to
their lanes. Measured with `bytematch diff` against `build-retail/materialsystem`
(clang-600.0.57, `--retail-osx32`); "matched" = exact or matches-after-masking.
"At lane start" is bytematch b1debe16 (start of lane mod-matsys3; earlier lanes:
lane-mod-matsys.md, lane-mod-matsys2.md).

Changes in lane mod-matsys3 (see lane-mod-matsys3.md), all under `BYTEMATCH_RETAIL`:
- `HushAsserts()` is `CommandLine()->FindParm( "-hushasserts" )` (macro in
  materialsystem_global.h; retail tier0 exports no HushAsserts).
- CMaterialSystem: constructor picks the render backend (`-noshaderapi`, `IsSteamDeck`,
  `-opengl`/`-vulkan`) and `SetShaderAPI` maps it to the DLL name; error material cleaned up in
  `CleanUpDebugMaterials` (no deferred `CleanUpErrorMaterial`); no `ForceSingleThreaded` in the
  reload paths; lock/acquire functions re-read `mat_queue_report`; `EndFrame` without
  `OnRenderingAsyncComplete`; `OverrideConfig` treats config bit 18 (`NO_WINDOW_BORDER`) as a
  video mode change; DX-support convar list adds `r_lightmap_bicubic`/`mat_specular_scalar`.
- CMaterial: `Precache()` in the framebuffer queries, render-state flag cleared in
  `CleanUpStateSnapshots`, 24-byte `CMaterial_QueueFriendly`, leak HDR/LDR fallback blocks,
  no self-fallback check, `?hdr` evaluated.
- cmaterialvar.o is SDK 2012-style: `#pragma pack(1)`, inline temp-var indexing, no texture
  ref tracking, env-cubemap sentinel checks.
- Smaller items: CMaterialSubRect flag-only `Uncache`, morph materials hold a reference,
  queued mesh index staging, mat_stub `SupportsNormalMapCompression` stubs, three lightmap
  writers (`nMaxT` local).

Still differing: three lightmap writers (register allocation), `DetermineProxyReplacements`
and `DecideShouldReloadFromWhitelist` (scheduling), `CMaterialSubRect::ParseMaterialVars`,
`cmaterial`'s static initializer (needs `DEFINE_FIXEDSIZE_ALLOCATOR` without `alignof`, a
public/tier1/mempool.h change), and other lanes' code (tier2 renderutils, bitmap, vtf).

| object | functions | matched now | matched at lane start | % bytes now |
|---|---:|---:|---:|---:|
| CColorCorrection | 38 | 38 | 37 | 100.0 |
| CMaterialSubRect | 107 | 106 | 101 | 71.3 |
| cmaterial | 139 | 138 | 122 | 98.4 |
| cmaterial_queuefriendly | 124 | 124 | 124 | 100.0 |
| cmaterialdict | 24 | 24 | 24 | 100.0 |
| cmaterialsystem | 319 | 318 | 286 | 98.0 |
| cmaterialvar | 122 | 122 | 109 | 100.0 |
| cmatlightmaps | 40 | 37 | 34 | 64.6 |
| cmatnullrendercontext | 186 | 186 | 186 | 100.0 |
| cmatqueuedrendercontext | 707 | 707 | 705 | 100.0 |
| cmatrendercontext | 274 | 274 | 272 | 100.0 |
| colorspace | 2 | 2 | 2 | 100.0 |
| ctexture | 142 | 142 | 142 | 100.0 |
| imagepacker | 3 | 3 | 3 | 100.0 |
| mat_stub | 787 | 787 | 784 | 100.0 |
| morph | 59 | 59 | 58 | 100.0 |
| occlusionquerymgr | 15 | 15 | 15 | 100.0 |
| shadersystem | 68 | 68 | 67 | 100.0 |
| texturemanager | 95 | 95 | 95 | 100.0 |
| **total** | 3251 | 3245 | 3166 | 96.8 |

Status column: matched = exact or matches-after-masking; "fixed" = differed or was missing at lane start.

| object | function | status | note |
|---|---|---|---|
| CColorCorrection | `CColorCorrectionSystem::AddLookup(char const*)` | matched | fixed (was differs) |
| CMaterialSubRect | `CMaterialSubRect::CMaterialSubRect(char const*, char const*, KeyValues*, KeyValues*, bool)` | matched | fixed (was differs) |
| CMaterialSubRect | `CMaterialSubRect::GetLowResColorSample(float, float, float*) const` | matched | fixed (was differs) |
| CMaterialSubRect | `CMaterialSubRect::ParseMaterialVars(KeyValues&)` | differs | size retail 2147 vs ours 2163 |
| CMaterialSubRect | `CMaterialSubRect::Uncache(bool)` | matched | fixed (was differs) |
| CMaterialSubRect | `CMaterialSubRect::~CMaterialSubRect()` | matched | fixed (was differs) |
| CMaterialSubRect | `IMaterialInternal::CreateMaterialSubRect(char const*, char const*, KeyValues*, KeyValues*, bool)` | matched | fixed (was differs) |
| cmaterial | `AccumulateRecursiveVmtPatches(KeyValues&, KeyValues**, KeyValues const&, char const*, CUtlVector<void*, CUtlMemory<void*, int>>*)` | matched | fixed (was differs) |
| cmaterial | `CMaterial::CMaterial(char const*, char const*, KeyValues*)` | matched | fixed (was differs) |
| cmaterial | `CMaterial::DecideShouldReloadFromWhitelist(IFileList*)` | differs | size retail 397 vs ours 402 |
| cmaterial | `CMaterial::GetPreviewImage(unsigned char*, int, int, ImageFormat) const` | matched | fixed (was differs) |
| cmaterial | `CMaterial::GetPreviewImageProperties(int*, int*, ImageFormat*, bool*) const` | matched | fixed (was differs) |
| cmaterial | `CMaterial::HasProxy() const` | matched | fixed (was differs) |
| cmaterial | `CMaterial::InitializeShader(KeyValues&, KeyValues&, int)` | matched | fixed (was differs) |
| cmaterial | `CMaterial::InitializeStateSnapshots()` | matched | fixed (was differs) |
| cmaterial | `CMaterial::NeedsFullFrameBufferTexture(bool)` | matched | fixed (was differs) |
| cmaterial | `CMaterial::NeedsPowerOfTwoFrameBufferTexture(bool)` | matched | fixed (was differs) |
| cmaterial | `CMaterial::NeedsTangentSpace()` | matched | fixed (was differs) |
| cmaterial | `CMaterial::ReloadTextures()` | matched | fixed (was differs) |
| cmaterial | `CMaterial::ShouldSkipVar(KeyValues*, bool*)` | matched | fixed (was differs) |
| cmaterial | `CMaterial::Uncache(bool)` | matched | fixed (was differs) |
| cmaterial | `CMaterial::UsesEnvCubemap()` | matched | fixed (was differs) |
| cmaterial | `IMaterialInternal::CreateMaterial(char const*, char const*, KeyValues*)` | matched | fixed (was differs) |
| cmaterial | `IMaterialInternal::DestroyMaterial(IMaterialInternal*)` | matched | fixed (was differs) |
| cmaterialsystem | `CMatQueuedRenderContext::CMatQueuedRenderContext()` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::AllowThreading(bool, int)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::CMaterialSystem()` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::CacheUsedMaterials()` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::CleanUpDebugMaterials()` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::CreateNamedRenderTargetTextureEx(char const*, int, int, RenderTargetSizeMode_t, ImageFormat, MaterialRenderTargetDepth_t, unsigned int, unsigned int)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::CreateRenderContext(MaterialContextType_t)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::DetermineProxyReplacements(IMaterial*, KeyValues*)` | differs | bytes |
| cmaterialsystem | `CMaterialSystem::Disconnect()` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::EndFrame()` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::EndRenderTargetAllocation()` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::FindTexture(char const*, char const*, bool, int)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::Init()` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::Lock()` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::ModShutdown()` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::OverrideConfig(MaterialSystem_Config_t const&, bool)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::PreloadReplacements()` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::ReloadMaterials(char const*)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::ReloadTextures()` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::ScanDirForReplacements(char const*)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::SetMode(void*, MaterialSystem_Config_t const&)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::SetShaderAPI(char const*)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::Shutdown()` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::ThreadAcquire(bool)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::ThreadExecuteQueuedContext(CMatQueuedRenderContext*)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::ThreadRelease()` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::Unlock(MaterialLock_t__*)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::WriteConfigurationInfoToConVars(bool)` | matched | fixed (was differs) |
| cmaterialsystem | `CReplacementProxy::Init(IMaterial*, KeyValues*)` | matched | fixed (was differs) |
| cmaterialsystem | `CReplacementProxy::Release()` | matched | fixed (was differs) |
| cmaterialsystem | `MatProxyCallback(IConVar*, char const*, float)` | matched | fixed (was differs) |
| cmaterialsystem | `__CreateCReplacementProxy_interface()` | matched | fixed (was differs) |
| cmaterialsystem | `non-virtual thunk to CMaterialSystem::Disconnect()` | matched | fixed (was differs) |
| cmaterialvar | `CMaterialVar::CMaterialVar(IMaterial*, char const*, VMatrix const&)` | matched | fixed (was differs) |
| cmaterialvar | `CMaterialVar::CleanUpData()` | matched | fixed (was differs) |
| cmaterialvar | `CMaterialVar::SetFloatValue(float)` | matched | fixed (was differs) |
| cmaterialvar | `CMaterialVar::SetFourCCValue(unsigned long, void*)` | matched | fixed (was differs) |
| cmaterialvar | `CMaterialVar::SetIntValue(int)` | matched | fixed (was differs) |
| cmaterialvar | `CMaterialVar::SetMaterialValue(IMaterial*)` | matched | fixed (was differs) |
| cmaterialvar | `CMaterialVar::SetMatrixValue(VMatrix const&)` | matched | fixed (was differs) |
| cmaterialvar | `CMaterialVar::SetStringValue(char const*)` | matched | fixed (was differs) |
| cmaterialvar | `CMaterialVar::SetTextureValue(ITexture*)` | matched | fixed (was differs) |
| cmaterialvar | `CMaterialVar::SetUndefined()` | matched | fixed (was differs) |
| cmaterialvar | `CMaterialVar::SetVecComponentValue(float, int)` | matched | fixed (was differs) |
| cmaterialvar | `CMaterialVar::SetVecValueInternal(Vector4D const&, int)` | matched | fixed (was differs) |
| cmaterialvar | `EnableThreadedMaterialVarAccess(bool, IMaterialVar**, int)` | matched | fixed (was differs) |
| cmatlightmaps | `CMatLightmaps::BumpedLightmapBitsToPixelWriter_HDRF(float*, float*, float*, float*, int*, int*, FloatBitMap_t*)` | matched | fixed (was differs) |
| cmatlightmaps | `CMatLightmaps::BumpedLightmapBitsToPixelWriter_HDRI(float*, float*, float*, float*, int*, int*, FloatBitMap_t*)` | matched | fixed (was differs) |
| cmatlightmaps | `CMatLightmaps::BumpedLightmapBitsToPixelWriter_LDR(float*, float*, float*, float*, int*, int*, FloatBitMap_t*)` | differs | size retail 6584 vs ours 6572 |
| cmatlightmaps | `CMatLightmaps::LightmapBitsToPixelWriter_HDRF(float*, int*, int*, FloatBitMap_t*)` | matched | fixed (was differs) |
| cmatlightmaps | `CMatLightmaps::LightmapBitsToPixelWriter_HDRI(float*, int*, int*, FloatBitMap_t*)` | differs | size retail 1584 vs ours 1600 |
| cmatlightmaps | `CMatLightmaps::LightmapBitsToPixelWriter_LDR(float*, int*, int*, FloatBitMap_t*)` | differs | bytes |
| cmatqueuedrendercontext | `CMatQueuedMesh::ExecuteDefferredBuild(unsigned char*, int, int, unsigned short*, int)` | matched | fixed (was differs) |
| cmatqueuedrendercontext | `CMatQueuedRenderContext::CallQueued(bool)` | matched | fixed (was differs) |
| cmatrendercontext | `CMatRenderContext::BindMorph(IMorph*)` | matched | fixed (was differs) |
| cmatrendercontext | `CMatRenderContextBase::PopRenderTargetAndViewport()` | matched | fixed (was differs) |
| mat_stub | `CDummyHardwareConfig::SupportsNormalMapCompression() const` | matched | fixed (was missing) |
| mat_stub | `CDummyMaterialSystem::GetRenderBackend() const` | matched | fixed (was differs) |
| mat_stub | `CDummyMaterialSystem::SupportsNormalMapCompression()` | matched | fixed (was missing) |
| morph | `CMorphMgr::AllocateMaterials()` | matched | fixed (was differs) |
| shadersystem | `CShaderSystem::InitShaderParameters(IShader*, IMaterialVar**, char const*)` | matched | fixed (was differs) |
