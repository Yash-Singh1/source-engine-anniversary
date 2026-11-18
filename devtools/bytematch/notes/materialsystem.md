# materialsystem (retail macOS)

# materialsystem (retail macOS)

Retail: depot 223 `bin/materialsystem.dylib`; own objects listed below (the debug map's
materialsystem.build objects). tier1/tier2/mathlib/bitmap/vtf/shaderlib members belong to
their lanes. Measured with `bytematch diff` against `build-retail/materialsystem`
(clang-600.0.57, `--retail-osx32`); "matched" = exact or matches-after-masking.
"At lane start" is bytematch d276d8ee (start of lane mod-matsys2; lane mod-matsys started
from 1f7fd619, see lane-mod-matsys.md).

Changes in lane mod-matsys2 (see lane-mod-matsys2.md):
- CTexture is the 2013 (pre-streaming) class in retail mode: `materialsystem/ctexture_retail.inl`,
  included by ctexture.cpp under BYTEMATCH_RETAIL. ctexture.o 143/143.
- texturemanager.o 96/96: run-time `IsOpenGL()` choice in the signed normalization cubemap,
  the leak's integer normalization cubemap, no virtual destructors on the simple regenerators,
  the leak's normalize-cubemap flags.
- cmatlightmaps: anniversary 1024x512 pages and one-texel lightmap borders (allocation functions
  match; the six writers have retail's logic but still differ in register allocation).
- cmaterialsystem: retail convar set/flags plus `r_lightmap_bicubic` (config flag bit 19), the
  anniversary desktop-resolution default, leak EndFrame job submission.
- `ImageFormat` without the two `_RUNTIME` formats and `IVTFTexture` without the streaming methods
  in retail mode (also measured in engine, studiorender, shaderapidx9).

Largest remaining gaps: the lightmap writers (register allocation / loop strength reduction),
`CMaterialSystem::CMaterialSystem()`, `OverrideConfig` and `EndFrame` (inlined helpers),
`CMaterial::InitializeShader`, and tier2 renderutils / bitmap colorconversion (other lanes).

| object | functions | matched now | matched at lane start | % bytes now |
|---|---:|---:|---:|---:|
| CColorCorrection | 38 | 37 | 35 | 90.1 |
| CMaterialSubRect | 107 | 101 | 101 | 54.3 |
| cmaterial | 139 | 122 | 122 | 66.3 |
| cmaterial_queuefriendly | 124 | 124 | 124 | 100.0 |
| cmaterialdict | 24 | 24 | 24 | 100.0 |
| cmaterialsystem | 319 | 286 | 283 | 67.0 |
| cmaterialvar | 122 | 107 | 107 | 69.4 |
| cmatlightmaps | 40 | 34 | 31 | 34.0 |
| cmatnullrendercontext | 186 | 186 | 186 | 100.0 |
| cmatqueuedrendercontext | 707 | 705 | 705 | 97.7 |
| cmatrendercontext | 274 | 272 | 272 | 99.7 |
| colorspace | 2 | 2 | 2 | 100.0 |
| ctexture | 142 | 142 | 94 | 100.0 |
| imagepacker | 3 | 3 | 3 | 100.0 |
| mat_stub | 787 | 784 | 784 | 99.7 |
| morph | 59 | 58 | 58 | 98.0 |
| occlusionquerymgr | 15 | 15 | 15 | 100.0 |
| shadersystem | 68 | 67 | 67 | 97.0 |
| texturemanager | 95 | 95 | 85 | 100.0 |
| **total** | 3251 | 3164 | 3098 | 85.5 |

Status column: matched = exact or matches-after-masking; "fixed" = differed or was missing at lane start.

| object | function | status | note |
|---|---|---|---|
| CColorCorrection | `CColorCorrectionSystem::AddLookup(char const*)` | differs | bytes |
| CColorCorrection | `CColorCorrectionSystem::RemoveLookup(unsigned int)` | matched | fixed (was differs) |
| CColorCorrection | `CColorCorrectionSystem::Shutdown()` | matched | fixed (was differs) |
| CMaterialSubRect | `CMaterialSubRect::CMaterialSubRect(char const*, char const*, KeyValues*, KeyValues*, bool)` | differs | size retail 651 vs ours 658 |
| CMaterialSubRect | `CMaterialSubRect::GetLowResColorSample(float, float, float*) const` | differs | size retail 54 vs ours 80 |
| CMaterialSubRect | `CMaterialSubRect::ParseMaterialVars(KeyValues&)` | differs | size retail 2147 vs ours 2163 |
| CMaterialSubRect | `CMaterialSubRect::Uncache(bool)` | differs | size retail 63 vs ours 195 |
| CMaterialSubRect | `CMaterialSubRect::~CMaterialSubRect()` | differs | size retail 375 vs ours 326 |
| CMaterialSubRect | `IMaterialInternal::CreateMaterialSubRect(char const*, char const*, KeyValues*, KeyValues*, bool)` | differs | bytes |
| cmaterial | `AccumulateRecursiveVmtPatches(KeyValues&, KeyValues**, KeyValues const&, char const*, CUtlVector<void*, CUtlMemory<void*, int>>*)` | differs | size retail 734 vs ours 739 |
| cmaterial | `CMaterial::CMaterial(char const*, char const*, KeyValues*)` | differs | size retail 754 vs ours 789 |
| cmaterial | `CMaterial::DecideShouldReloadFromWhitelist(IFileList*)` | differs | size retail 397 vs ours 402 |
| cmaterial | `CMaterial::GetPreviewImage(unsigned char*, int, int, ImageFormat) const` | differs | size retail 809 vs ours 846 |
| cmaterial | `CMaterial::GetPreviewImageProperties(int*, int*, ImageFormat*, bool*) const` | differs | size retail 609 vs ours 634 |
| cmaterial | `CMaterial::HasProxy() const` | differs | size retail 18 vs ours 69 |
| cmaterial | `CMaterial::InitializeShader(KeyValues&, KeyValues&, int)` | differs | size retail 3120 vs ours 3276 |
| cmaterial | `CMaterial::InitializeStateSnapshots()` | differs | size retail 241 vs ours 223 |
| cmaterial | `CMaterial::NeedsFullFrameBufferTexture(bool)` | differs | size retail 95 vs ours 101 |
| cmaterial | `CMaterial::NeedsPowerOfTwoFrameBufferTexture(bool)` | differs | size retail 95 vs ours 101 |
| cmaterial | `CMaterial::NeedsTangentSpace()` | differs | size retail 85 vs ours 57 |
| cmaterial | `CMaterial::ReloadTextures()` | differs | size retail 141 vs ours 128 |
| cmaterial | `CMaterial::ShouldSkipVar(KeyValues*, bool*)` | differs | size retail 479 vs ours 485 |
| cmaterial | `CMaterial::Uncache(bool)` | differs | size retail 494 vs ours 478 |
| cmaterial | `CMaterial::UsesEnvCubemap()` | differs | size retail 85 vs ours 57 |
| cmaterial | `IMaterialInternal::CreateMaterial(char const*, char const*, KeyValues*)` | differs | bytes |
| cmaterial | `IMaterialInternal::DestroyMaterial(IMaterialInternal*)` | differs | size retail 79 vs ours 94 |
| cmaterialsystem | `CMatQueuedRenderContext::CMatQueuedRenderContext()` | differs | bytes |
| cmaterialsystem | `CMaterialSystem::AllowThreading(bool, int)` | differs | size retail 210 vs ours 256 |
| cmaterialsystem | `CMaterialSystem::CMaterialSystem()` | differs | size retail 5130 vs ours 5018 |
| cmaterialsystem | `CMaterialSystem::CacheUsedMaterials()` | differs | size retail 198 vs ours 204 |
| cmaterialsystem | `CMaterialSystem::CleanUpDebugMaterials()` | differs | size retail 503 vs ours 453 |
| cmaterialsystem | `CMaterialSystem::CreateNamedRenderTargetTextureEx(char const*, int, int, RenderTargetSizeMode_t, ImageFormat, MaterialRenderTargetDepth_t, unsigned int, unsigned int)` | differs | size retail 410 vs ours 381 |
| cmaterialsystem | `CMaterialSystem::CreateRenderContext(MaterialContextType_t)` | differs | size retail 141 vs ours 149 |
| cmaterialsystem | `CMaterialSystem::DetermineProxyReplacements(IMaterial*, KeyValues*)` | differs | size retail 1349 vs ours 1341 |
| cmaterialsystem | `CMaterialSystem::Disconnect()` | differs | size retail 199 vs ours 223 |
| cmaterialsystem | `CMaterialSystem::EndFrame()` | differs | size retail 2530 vs ours 2722 |
| cmaterialsystem | `CMaterialSystem::EndRenderTargetAllocation()` | differs | size retail 91 vs ours 66 |
| cmaterialsystem | `CMaterialSystem::FindTexture(char const*, char const*, bool, int)` | differs | size retail 410 vs ours 440 |
| cmaterialsystem | `CMaterialSystem::GenerateConfigFromConfigKeyValues(MaterialSystem_Config_t*, bool)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::Init()` | differs | size retail 465 vs ours 477 |
| cmaterialsystem | `CMaterialSystem::Lock()` | differs | size retail 474 vs ours 480 |
| cmaterialsystem | `CMaterialSystem::ModShutdown()` | differs | size retail 33 vs ours 46 |
| cmaterialsystem | `CMaterialSystem::OverrideConfig(MaterialSystem_Config_t const&, bool)` | differs | size retail 3709 vs ours 3741 |
| cmaterialsystem | `CMaterialSystem::PreloadReplacements()` | differs | size retail 244 vs ours 319 |
| cmaterialsystem | `CMaterialSystem::ReadConfigFromConVars(MaterialSystem_Config_t*)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::ReloadMaterials(char const*)` | differs | size retail 1007 vs ours 1093 |
| cmaterialsystem | `CMaterialSystem::ReloadTextures()` | differs | size retail 43 vs ours 54 |
| cmaterialsystem | `CMaterialSystem::ScanDirForReplacements(char const*)` | differs | size retail 671 vs ours 639 |
| cmaterialsystem | `CMaterialSystem::SetMode(void*, MaterialSystem_Config_t const&)` | differs | bytes |
| cmaterialsystem | `CMaterialSystem::SetShaderAPI(char const*)` | differs | size retail 354 vs ours 421 |
| cmaterialsystem | `CMaterialSystem::Shutdown()` | differs | size retail 248 vs ours 290 |
| cmaterialsystem | `CMaterialSystem::ThreadAcquire(bool)` | differs | size retail 443 vs ours 447 |
| cmaterialsystem | `CMaterialSystem::ThreadExecuteQueuedContext(CMatQueuedRenderContext*)` | differs | size retail 100 vs ours 106 |
| cmaterialsystem | `CMaterialSystem::ThreadRelease()` | differs | size retail 613 vs ours 609 |
| cmaterialsystem | `CMaterialSystem::Unlock(MaterialLock_t__*)` | differs | size retail 270 vs ours 250 |
| cmaterialsystem | `CMaterialSystem::WriteConfigIntoConVars(MaterialSystem_Config_t const&)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::WriteConfigurationInfoToConVars(bool)` | differs | bytes |
| cmaterialsystem | `CReplacementProxy::Init(IMaterial*, KeyValues*)` | differs | size retail 139 vs ours 131 |
| cmaterialsystem | `CReplacementProxy::Release()` | differs | size retail 25 vs ours 46 |
| cmaterialsystem | `MatProxyCallback(IConVar*, char const*, float)` | differs | size retail 139 vs ours 248 |
| cmaterialsystem | `__CreateCReplacementProxy_interface()` | differs | size retail 37 vs ours 44 |
| cmaterialsystem | `non-virtual thunk to CMaterialSystem::Disconnect()` | differs | size retail 199 vs ours 25 |
| cmaterialvar | `CMaterialVar::CMaterialVar(IMaterial*, char const*, VMatrix const&)` | differs | bytes |
| cmaterialvar | `CMaterialVar::CleanUpData()` | differs | size retail 192 vs ours 180 |
| cmaterialvar | `CMaterialVar::SetFloatValue(float)` | differs | size retail 348 vs ours 364 |
| cmaterialvar | `CMaterialVar::SetFourCCValue(unsigned long, void*)` | differs | size retail 421 vs ours 433 |
| cmaterialvar | `CMaterialVar::SetIntValue(int)` | differs | size retail 348 vs ours 364 |
| cmaterialvar | `CMaterialVar::SetMaterialValue(IMaterial*)` | differs | size retail 410 vs ours 430 |
| cmaterialvar | `CMaterialVar::SetMatrixValue(VMatrix const&)` | differs | size retail 621 vs ours 710 |
| cmaterialvar | `CMaterialVar::SetStringValue(char const*)` | differs | size retail 491 vs ours 511 |
| cmaterialvar | `CMaterialVar::SetTextureValue(ITexture*)` | differs | size retail 384 vs ours 591 |
| cmaterialvar | `CMaterialVar::SetUndefined()` | differs | size retail 284 vs ours 314 |
| cmaterialvar | `CMaterialVar::SetVecComponentValue(float, int)` | differs | size retail 503 vs ours 583 |
| cmaterialvar | `CMaterialVar::SetVecValueInternal(Vector4D const&, int)` | differs | size retail 454 vs ours 489 |
| cmaterialvar | `CUtlSymbolTableMT::Find(char const*) const` | missing |  |
| cmaterialvar | `EnableThreadedMaterialVarAccess(bool, IMaterialVar**, int)` | differs | size retail 126 vs ours 164 |
| cmaterialvar | `IMaterialVar::FindSymbol(char const*)` | differs | size retail 63 vs ours 91 |
| cmatlightmaps | `CMatLightmaps::AllocateLightmap(int, int, int*, IMaterial*)` | matched | fixed (was differs) |
| cmatlightmaps | `CMatLightmaps::BeginLightmapAllocation()` | matched | fixed (was differs) |
| cmatlightmaps | `CMatLightmaps::BumpedLightmapBitsToPixelWriter_HDRF(float*, float*, float*, float*, int*, int*, FloatBitMap_t*)` | differs | size retail 1957 vs ours 1965 |
| cmatlightmaps | `CMatLightmaps::BumpedLightmapBitsToPixelWriter_HDRI(float*, float*, float*, float*, int*, int*, FloatBitMap_t*)` | differs | size retail 5328 vs ours 5376 |
| cmatlightmaps | `CMatLightmaps::BumpedLightmapBitsToPixelWriter_LDR(float*, float*, float*, float*, int*, int*, FloatBitMap_t*)` | differs | size retail 6584 vs ours 6572 |
| cmatlightmaps | `CMatLightmaps::EndLightmapAllocation()` | matched | fixed (was differs) |
| cmatlightmaps | `CMatLightmaps::LightmapBitsToPixelWriter_HDRF(float*, int*, int*, FloatBitMap_t*)` | differs | size retail 651 vs ours 672 |
| cmatlightmaps | `CMatLightmaps::LightmapBitsToPixelWriter_HDRI(float*, int*, int*, FloatBitMap_t*)` | differs | size retail 1584 vs ours 1600 |
| cmatlightmaps | `CMatLightmaps::LightmapBitsToPixelWriter_LDR(float*, int*, int*, FloatBitMap_t*)` | differs | size retail 1046 vs ours 1056 |
| cmatqueuedrendercontext | `CMatQueuedMesh::ExecuteDefferredBuild(unsigned char*, int, int, unsigned short*, int)` | differs | size retail 1035 vs ours 1103 |
| cmatqueuedrendercontext | `CMatQueuedRenderContext::CallQueued(bool)` | differs | size retail 372 vs ours 388 |
| cmatrendercontext | `CMatRenderContext::BindMorph(IMorph*)` | differs | size retail 101 vs ours 108 |
| cmatrendercontext | `CMatRenderContextBase::PopRenderTargetAndViewport()` | differs | size retail 30 vs ours 56 |
| ctexture | `CTexture::AllocateShaderAPITextures()` | matched | fixed (was differs) |
| ctexture | `CTexture::ApplyRenderTargetSizeMode(int&, int&, ImageFormat)` | matched | fixed (was differs) |
| ctexture | `CTexture::Bind(Sampler_t, int, Sampler_t)` | matched | fixed (was differs) |
| ctexture | `CTexture::BindVertexTexture(VertexTextureSampler_t, int)` | matched | fixed (was differs) |
| ctexture | `CTexture::CTexture()` | matched | fixed (was differs) |
| ctexture | `CTexture::ComputeActualFormat(ImageFormat)` | matched | fixed (was differs) |
| ctexture | `CTexture::ComputeActualSize(bool, IVTFTexture*)` | matched | fixed (was missing) |
| ctexture | `CTexture::ConvertToActualFormat(IVTFTexture*, bool)` | matched | fixed (was missing) |
| ctexture | `CTexture::CopyFrameBufferToMe(int, Rect_t*, Rect_t*)` | matched | fixed (was differs) |
| ctexture | `CTexture::CopyLowResImageToTexture(IVTFTexture*)` | matched | fixed (was differs) |
| ctexture | `CTexture::CopyMeToFrameBuffer(int, Rect_t*, Rect_t*)` | matched | fixed (was differs) |
| ctexture | `CTexture::Download(Rect_t*, int)` | matched | fixed (was differs) |
| ctexture | `CTexture::GetActualDepth() const` | matched | fixed (was differs) |
| ctexture | `CTexture::GetActualHeight() const` | matched | fixed (was differs) |
| ctexture | `CTexture::GetActualWidth() const` | matched | fixed (was differs) |
| ctexture | `CTexture::GetLowResColorSample(float, float, float*) const` | matched | fixed (was differs) |
| ctexture | `CTexture::GetMappingDepth() const` | matched | fixed (was differs) |
| ctexture | `CTexture::GetNumAnimationFrames() const` | matched | fixed (was differs) |
| ctexture | `CTexture::GetOptimalReadBuffer(void*, int, CUtlBuffer&)` | matched | fixed (was missing) |
| ctexture | `CTexture::GetResourceData(unsigned int, unsigned long*) const` | matched | fixed (was differs) |
| ctexture | `CTexture::GetTextureHandle(int, int)` | matched | fixed (was differs) |
| ctexture | `CTexture::HandleFileLoadFailedTexture(IVTFTexture*)` | matched | fixed (was differs) |
| ctexture | `CTexture::Init(int, int, int, ImageFormat, int, int, bool)` | matched | fixed (was missing) |
| ctexture | `CTexture::InitRenderTarget(char const*, int, int, RenderTargetSizeMode_t, ImageFormat, RenderTargetType_t, unsigned int, unsigned int)` | matched | fixed (was differs) |
| ctexture | `CTexture::IsVolumeTexture() const` | matched | fixed (was differs) |
| ctexture | `CTexture::LoadLowResTexture(IVTFTexture*)` | matched | fixed (was differs) |
| ctexture | `CTexture::LoadTextureBitsFromFile(char*, char**, bool)` | matched | fixed (was missing) |
| ctexture | `CTexture::MarkAsExcluded(bool, int)` | matched | fixed (was differs) |
| ctexture | `CTexture::OnRestore()` | matched | fixed (was differs) |
| ctexture | `CTexture::Precache()` | matched | fixed (was differs) |
| ctexture | `CTexture::ReconstructPartialProceduralBits(Rect_t const*, Rect_t*)` | matched | fixed (was differs) |
| ctexture | `CTexture::ReconstructPartialTexture(Rect_t const*)` | matched | fixed (was differs) |
| ctexture | `CTexture::ReconstructProceduralBits()` | matched | fixed (was differs) |
| ctexture | `CTexture::ReconstructTexture()` | matched | fixed (was missing) |
| ctexture | `CTexture::Release()` | matched | fixed (was differs) |
| ctexture | `CTexture::SaveToFile(char const*)` | matched | fixed (was differs) |
| ctexture | `CTexture::SetFilteringAndClampingMode()` | matched | fixed (was differs) |
| ctexture | `CTexture::SetRenderTarget(int, ITexture*)` | matched | fixed (was differs) |
| ctexture | `CTexture::SetTextureRegenerator(ITextureRegenerator*)` | matched | fixed (was differs) |
| ctexture | `CTexture::SetupDebuggingTextures(IVTFTexture*)` | matched | fixed (was differs) |
| ctexture | `CTexture::Shutdown()` | matched | fixed (was differs) |
| ctexture | `CTexture::SwapContents(ITexture*)` | matched | fixed (was differs) |
| ctexture | `CTexture::UpdateExcludedState()` | matched | fixed (was differs) |
| ctexture | `CTexture::~CTexture()` | matched | fixed (was differs) |
| ctexture | `ITextureInternal::CreateFileTexture(char const*, char const*)` | matched | fixed (was differs) |
| ctexture | `ITextureInternal::CreateProceduralTexture(char const*, char const*, int, int, int, ImageFormat, int)` | matched | fixed (was missing) |
| ctexture | `ITextureInternal::CreateRenderTarget(char const*, int, int, RenderTargetSizeMode_t, ImageFormat, RenderTargetType_t, unsigned int, unsigned int)` | matched | fixed (was differs) |
| ctexture | `ITextureInternal::Destroy(ITextureInternal*)` | matched | fixed (was missing) |
| mat_stub | `CDummyHardwareConfig::SupportsNormalMapCompression() const` | missing |  |
| mat_stub | `CDummyMaterialSystem::GetRenderBackend() const` | differs | size retail 10 vs ours 39 |
| mat_stub | `CDummyMaterialSystem::SupportsNormalMapCompression()` | missing |  |
| morph | `CMorphMgr::AllocateMaterials()` | differs | size retail 519 vs ours 495 |
| shadersystem | `CShaderSystem::InitShaderParameters(IShader*, IMaterialVar**, char const*)` | differs | size retail 624 vs ours 640 |
| texturemanager | `CCheckerboardTexture::Release()` | matched | fixed (was differs) |
| texturemanager | `CColorCorrectionTexture::Release()` | matched | fixed (was differs) |
| texturemanager | `CNormalizationCubemap::RegenerateTextureBits(ITexture*, IVTFTexture*, Rect_t*)` | matched | fixed (was differs) |
| texturemanager | `CSignedNormalizationCubemap::RegenerateTextureBits(ITexture*, IVTFTexture*, Rect_t*)` | matched | fixed (was differs) |
| texturemanager | `CSolidTexture::Release()` | matched | fixed (was differs) |
| texturemanager | `CTextureManager::CreateProceduralTexture(char const*, char const*, int, int, int, ImageFormat, int)` | matched | fixed (was differs) |
| texturemanager | `CTextureManager::Init(int)` | matched | fixed (was differs) |
| texturemanager | `CTextureManager::RemoveTexture(ITextureInternal*)` | matched | fixed (was differs) |
| texturemanager | `CTextureManager::RemoveUnusedTextures()` | matched | fixed (was differs) |
| texturemanager | `CTextureManager::Shutdown()` | matched | fixed (was differs) |
