# materialsystem (retail macOS)

Retail: depot 223 `bin/materialsystem.dylib`; own objects listed below (the debug map's
materialsystem.build objects). tier1/tier2/mathlib/bitmap/vtf/shaderlib members belong to
their lanes. Measured with `bytematch diff` against `build-retail/materialsystem`
(clang-600.0.57, `--retail-osx32`); "matched" = exact or matches-after-masking.
"At lane start" is bytematch 1f7fd619.

Main changes (see lane-mod-matsys.md): fork texture streaming, async texture operations and
the TF2 texture compositor are out of the retail path (internal interfaces, CMaterialSystem,
CTextureManager, CTexture entry points); retail layouts for CMaterialSystem
(`m_eRenderBackend` after `m_ThreadOwnershipID`), CMatQueuedRenderContext and CMaterial
(proxy count + array); SDK 2013 `pixelwriter.h` in retail mode; SDK 2012 / leak bodies where
retail matches them exactly.

Largest remaining gap: `ctexture` is still TF2's streaming-era CTexture with retail entry
points; retail's class is the 2013 one (separate mapping/actual width/height/depth shorts,
no streaming members, handles at +0x40, `TextureLodOverride`, `Init(..., bool)`,
`LoadTextureBitsFromFile(char*, char**, bool)`). SDK 2012's member list matches retail's
offsets; it needs a function-by-function rewrite.

| object | functions | matched now | matched at lane start | % bytes now |
|---|---:|---:|---:|---:|
| CColorCorrection | 38 | 33 | 32 | 75.8 |
| CMaterialSubRect | 107 | 101 | 101 | 54.3 |
| cmaterial | 139 | 121 | 66 | 64.6 |
| cmaterial_queuefriendly | 124 | 124 | 119 | 100.0 |
| cmaterialdict | 24 | 24 | 22 | 100.0 |
| cmaterialsystem | 319 | 282 | 215 | 59.6 |
| cmaterialvar | 122 | 107 | 104 | 69.4 |
| cmatlightmaps | 40 | 31 | 30 | 26.6 |
| cmatnullrendercontext | 186 | 186 | 182 | 100.0 |
| cmatqueuedrendercontext | 707 | 705 | 686 | 97.7 |
| cmatrendercontext | 274 | 263 | 246 | 91.4 |
| colorspace | 2 | 2 | 2 | 100.0 |
| ctexture | 142 | 89 | 82 | 22.5 |
| imagepacker | 3 | 3 | 3 | 100.0 |
| mat_stub | 787 | 784 | 784 | 99.7 |
| morph | 59 | 55 | 52 | 94.3 |
| occlusionquerymgr | 15 | 15 | 15 | 100.0 |
| shadersystem | 68 | 61 | 58 | 95.2 |
| texturemanager | 95 | 78 | 57 | 74.2 |
| **total** | 3251 | 3064 | 2856 | 73.7 |

Status column: matched = exact or matches-after-masking; "fixed" = differed or was missing at lane start.

| object | function | status | note |
|---|---|---|---|
| CColorCorrection | `CColorCorrectionSystem::AddLookup(char const*)` | differs | size retail 506 vs ours 514 |
| CColorCorrection | `CColorCorrectionSystem::ReleaseTextures()` | differs | bytes |
| CColorCorrection | `CColorCorrectionSystem::RemoveLookup(unsigned int)` | differs | size retail 299 vs ours 157 |
| CColorCorrection | `CColorCorrectionSystem::RestoreTextures()` | differs | bytes |
| CColorCorrection | `CColorCorrectionSystem::Shutdown()` | differs | size retail 271 vs ours 129 |
| CColorCorrection | `CColorCorrectionSystem::SortLookups()` | matched | fixed (was differs) |
| CMaterialSubRect | `CMaterialSubRect::CMaterialSubRect(char const*, char const*, KeyValues*, KeyValues*, bool)` | differs | size retail 651 vs ours 658 |
| CMaterialSubRect | `CMaterialSubRect::GetLowResColorSample(float, float, float*) const` | differs | size retail 54 vs ours 80 |
| CMaterialSubRect | `CMaterialSubRect::ParseMaterialVars(KeyValues&)` | differs | size retail 2147 vs ours 2163 |
| CMaterialSubRect | `CMaterialSubRect::Uncache(bool)` | differs | size retail 63 vs ours 195 |
| CMaterialSubRect | `CMaterialSubRect::~CMaterialSubRect()` | differs | size retail 375 vs ours 326 |
| CMaterialSubRect | `IMaterialInternal::CreateMaterialSubRect(char const*, char const*, KeyValues*, KeyValues*, bool)` | differs | bytes |
| cmaterial | `AccumulateRecursiveVmtPatches(KeyValues&, KeyValues**, KeyValues const&, char const*, CUtlVector<void*, CUtlMemory<void*, int>>*)` | differs | size retail 734 vs ours 739 |
| cmaterial | `CMaterial::AddMaterialVar(IMaterialVar*)` | matched | fixed (was differs) |
| cmaterial | `CMaterial::AlphaModulate(float)` | matched | fixed (was differs) |
| cmaterial | `CMaterial::CMaterial(char const*, char const*, KeyValues*)` | differs | size retail 754 vs ours 789 |
| cmaterial | `CMaterial::CallBindProxy(void*)` | matched | fixed (was differs) |
| cmaterial | `CMaterial::CheckProxyReplacement(void*)` | matched | fixed (was differs) |
| cmaterial | `CMaterial::ClearContextData()` | matched | fixed (was differs) |
| cmaterial | `CMaterial::ColorModulate(float, float, float)` | matched | fixed (was differs) |
| cmaterial | `CMaterial::DecideShouldReloadFromWhitelist(IFileList*)` | differs | size retail 397 vs ours 402 |
| cmaterial | `CMaterial::DeleteIfUnreferenced()` | matched | fixed (was differs) |
| cmaterial | `CMaterial::DrawMesh(VertexCompressionType_t)` | matched | fixed (was differs) |
| cmaterial | `CMaterial::FindRepresentativeTexture()` | differs | bytes |
| cmaterial | `CMaterial::FindVar(char const*, bool*, bool)` | matched | fixed (was differs) |
| cmaterial | `CMaterial::FindVarFast(char const*, unsigned int*)` | matched | fixed (was differs) |
| cmaterial | `CMaterial::GetAlphaModulation()` | matched | fixed (was differs) |
| cmaterial | `CMaterial::GetChangeID() const` | matched | fixed (was differs) |
| cmaterial | `CMaterial::GetColorModulation(float*, float*, float*)` | matched | fixed (was differs) |
| cmaterial | `CMaterial::GetLowResColorSample(float, float, float*) const` | matched | fixed (was differs) |
| cmaterial | `CMaterial::GetMaterialVarFlag(MaterialVarFlags_t) const` | matched | fixed (was differs) |
| cmaterial | `CMaterial::GetMorphFormat() const` | matched | fixed (was differs) |
| cmaterial | `CMaterial::GetNumAnimationFrames()` | matched | fixed (was differs) |
| cmaterial | `CMaterial::GetNumPasses()` | matched | fixed (was differs) |
| cmaterial | `CMaterial::GetPreviewImage(unsigned char*, int, int, ImageFormat) const` | differs | size retail 809 vs ours 846 |
| cmaterial | `CMaterial::GetPreviewImageProperties(int*, int*, ImageFormat*, bool*) const` | differs | size retail 609 vs ours 634 |
| cmaterial | `CMaterial::GetPropertyFlag(MaterialPropertyTypes_t)` | matched | fixed (was differs) |
| cmaterial | `CMaterial::GetQueueFriendlyVersion()` | matched | fixed (was differs) |
| cmaterial | `CMaterial::GetReflectivity(Vector&)` | matched | fixed (was differs) |
| cmaterial | `CMaterial::GetRenderState()` | matched | fixed (was differs) |
| cmaterial | `CMaterial::GetShaderParams()` | matched | fixed (was differs) |
| cmaterial | `CMaterial::GetTextureMemoryBytes()` | matched | fixed (was differs) |
| cmaterial | `CMaterial::GetVertexFormat() const` | matched | fixed (was differs) |
| cmaterial | `CMaterial::GetVertexUsage() const` | matched | fixed (was differs) |
| cmaterial | `CMaterial::HasProxy() const` | differs | size retail 18 vs ours 69 |
| cmaterial | `CMaterial::InitializeMaterialProxy(KeyValues*)` | matched | fixed (was differs) |
| cmaterial | `CMaterial::InitializeShader(KeyValues&, KeyValues&, int)` | differs | size retail 3120 vs ours 3276 |
| cmaterial | `CMaterial::InitializeStateSnapshots()` | differs | size retail 241 vs ours 223 |
| cmaterial | `CMaterial::IsAlphaTested()` | matched | fixed (was differs) |
| cmaterial | `CMaterial::IsSpriteCard()` | matched | fixed (was differs) |
| cmaterial | `CMaterial::IsSuppressed() const` | matched | fixed (was differs) |
| cmaterial | `CMaterial::IsTranslucent()` | matched | fixed (was differs) |
| cmaterial | `CMaterial::IsTranslucentInternal(float) const` | matched | fixed (was differs) |
| cmaterial | `CMaterial::IsUsingVertexID() const` | matched | fixed (was differs) |
| cmaterial | `CMaterial::IsVertexLit()` | matched | fixed (was differs) |
| cmaterial | `CMaterial::NeedsFixedFunctionFlashlight() const` | matched | fixed (was differs) |
| cmaterial | `CMaterial::NeedsFullFrameBufferTexture(bool)` | differs | size retail 95 vs ours 101 |
| cmaterial | `CMaterial::NeedsLightmapBlendAlpha()` | matched | fixed (was differs) |
| cmaterial | `CMaterial::NeedsPowerOfTwoFrameBufferTexture(bool)` | differs | size retail 95 vs ours 101 |
| cmaterial | `CMaterial::NeedsSoftwareLighting()` | matched | fixed (was differs) |
| cmaterial | `CMaterial::NeedsSoftwareSkinning()` | matched | fixed (was differs) |
| cmaterial | `CMaterial::NeedsTangentSpace()` | differs | size retail 85 vs ours 57 |
| cmaterial | `CMaterial::NoDebugOverride() const` | matched | fixed (was differs) |
| cmaterial | `CMaterial::PerformDebugTrace() const` | matched | fixed (was differs) |
| cmaterial | `CMaterial::Precache()` | matched | fixed (was differs) |
| cmaterial | `CMaterial::PrecacheMappingDimensions()` | matched | fixed (was differs) |
| cmaterial | `CMaterial::PrecacheVars(KeyValues*, KeyValues*, CUtlVector<void*, CUtlMemory<void*, int>>*, int)` | matched | fixed (was differs) |
| cmaterial | `CMaterial::ReloadFromWhitelistIfMarked()` | matched | fixed (was differs) |
| cmaterial | `CMaterial::ReloadTextures()` | differs | size retail 141 vs ours 128 |
| cmaterial | `CMaterial::ReportVarChanged(IMaterialVar*)` | matched | fixed (was differs) |
| cmaterial | `CMaterial::SetMaterialVarFlag(MaterialVarFlags_t, bool)` | matched | fixed (was differs) |
| cmaterial | `CMaterial::SetShader(char const*)` | matched | fixed (was differs) |
| cmaterial | `CMaterial::SetShaderAndParams(KeyValues*)` | matched | fixed (was differs) |
| cmaterial | `CMaterial::SetUseFixedFunctionBakedLighting(bool)` | matched | fixed (was differs) |
| cmaterial | `CMaterial::SetupErrorShader()` | matched | fixed (was differs) |
| cmaterial | `CMaterial::ShouldSkipVar(KeyValues*, bool*)` | differs | size retail 479 vs ours 485 |
| cmaterial | `CMaterial::ToggleDebugTrace()` | matched | fixed (was differs) |
| cmaterial | `CMaterial::ToggleSuppression()` | matched | fixed (was differs) |
| cmaterial | `CMaterial::Uncache(bool)` | differs | size retail 494 vs ours 478 |
| cmaterial | `CMaterial::UseFog() const` | matched | fixed (was differs) |
| cmaterial | `CMaterial::UsesEnvCubemap()` | differs | size retail 85 vs ours 57 |
| cmaterial | `CMaterial::WasReloadedFromWhitelist()` | matched | fixed (was differs) |
| cmaterial | `CMaterial::~CMaterial()` | matched | fixed (was differs) |
| cmaterial | `IMaterialInternal::CreateMaterial(char const*, char const*, KeyValues*)` | differs | bytes |
| cmaterial | `IMaterialInternal::DestroyMaterial(IMaterialInternal*)` | differs | size retail 79 vs ours 94 |
| cmaterial_queuefriendly | `CMaterial_QueueFriendly::DecrementReferenceCount()` | matched | fixed (was differs) |
| cmaterial_queuefriendly | `CMaterial_QueueFriendly::DeleteIfUnreferenced()` | matched | fixed (was differs) |
| cmaterial_queuefriendly | `CMaterial_QueueFriendly::GetLowResColorSample(float, float, float*) const` | matched | fixed (was differs) |
| cmaterial_queuefriendly | `CMaterial_QueueFriendly::IncrementReferenceCount()` | matched | fixed (was differs) |
| cmaterial_queuefriendly | `CMaterial_QueueFriendly::UpdateToRealTime()` | matched | fixed (was differs) |
| cmaterialdict | `CMaterialDict::RemoveMaterial(IMaterialInternal*)` | matched | fixed (was differs) |
| cmaterialdict | `CMaterialDict::Shutdown()` | matched | fixed (was differs) |
| cmaterialsystem | `CMatQueuedRenderContext::CCallQueueExternal::QueueFunctorInternal(CFunctor*)` | matched | fixed (was differs) |
| cmaterialsystem | `CMatQueuedRenderContext::CMatQueuedRenderContext()` | differs | bytes |
| cmaterialsystem | `CMaterialSystem::AddReleaseFunc(void (*)())` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::AddRestoreFunc(void (*)(int))` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::AddTextureAlias(char const*, char const*)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::AllocateStandardTextures()` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::AllowThreading(bool, int)` | differs | size retail 210 vs ours 256 |
| cmaterialsystem | `CMaterialSystem::BeginFrame(float)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::BeginRenderTargetAllocation()` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::BindStandardVertexTexture(VertexTextureSampler_t, StandardTextureId_t)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::CMaterialSystem()` | differs | size retail 5130 vs ours 5018 |
| cmaterialsystem | `CMaterialSystem::CacheUsedMaterials()` | differs | size retail 198 vs ours 204 |
| cmaterialsystem | `CMaterialSystem::CanUseEditorMaterials() const` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::CleanUpDebugMaterials()` | differs | size retail 503 vs ours 453 |
| cmaterialsystem | `CMaterialSystem::CompactMemory()` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::Connect(void* (*)(char const*, int*))` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::CreateDebugMaterials()` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::CreateNamedRenderTargetTextureEx(char const*, int, int, RenderTargetSizeMode_t, ImageFormat, MaterialRenderTargetDepth_t, unsigned int, unsigned int)` | differs | size retail 410 vs ours 381 |
| cmaterialsystem | `CMaterialSystem::CreateNamedRenderTargetTextureEx2(char const*, int, int, RenderTargetSizeMode_t, ImageFormat, MaterialRenderTargetDepth_t, unsigned int, unsigned int)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::CreateProceduralTexture(char const*, char const*, int, int, ImageFormat, int)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::CreateRenderContext(MaterialContextType_t)` | differs | size retail 141 vs ours 149 |
| cmaterialsystem | `CMaterialSystem::DebugPrintUsedMaterials(char const*, bool)` | differs | bytes |
| cmaterialsystem | `CMaterialSystem::DebugPrintUsedTextures()` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::DetermineProxyReplacements(IMaterial*, KeyValues*)` | differs | size retail 1349 vs ours 1341 |
| cmaterialsystem | `CMaterialSystem::Disconnect()` | differs | size retail 199 vs ours 223 |
| cmaterialsystem | `CMaterialSystem::DoStartupShaderPreloading()` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::EnableEditorMaterials()` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::EndFrame()` | differs | size retail 2530 vs ours 2722 |
| cmaterialsystem | `CMaterialSystem::EndRenderTargetAllocation()` | differs | size retail 91 vs ours 66 |
| cmaterialsystem | `CMaterialSystem::FindMaterialEx(char const*, char const*, int, bool, char const*)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::FindTexture(char const*, char const*, bool, int)` | differs | size retail 410 vs ours 440 |
| cmaterialsystem | `CMaterialSystem::ForceSingleThreaded()` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::GenerateConfigFromConfigKeyValues(MaterialSystem_Config_t*, bool)` | differs | size retail 1159 vs ours 1207 |
| cmaterialsystem | `CMaterialSystem::GetDisplayMode(MaterialVideoMode_t&) const` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::GetForcedTextureLoadPathID()` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::GetHardwareConfig(char const*, int*)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::GetMaterialProxyFactory()` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::GetRecommendedConfigurationInfo(int, KeyValues*)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::GetRenderBackend() const` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::GetRenderTargetFrameBufferDimensions(int&, int&)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::GetRenderThreadId() const` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::GetStandardTextureDimensions(int*, int*, StandardTextureId_t)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::GetThreadMode()` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::Init()` | differs | size retail 465 vs ours 477 |
| cmaterialsystem | `CMaterialSystem::Init(char const*, IMaterialProxyFactory*, void* (*)(char const*, int*), void* (*)(char const*, int*))` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::IsInFrame() const` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::IsInStubMode()` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::IsRenderThreadSafe()` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::IsTextureLoaded(char const*) const` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::Lock()` | differs | size retail 474 vs ours 480 |
| cmaterialsystem | `CMaterialSystem::ModShutdown()` | differs | size retail 33 vs ours 46 |
| cmaterialsystem | `CMaterialSystem::OnDrawMesh(IMesh*, CPrimList*, int)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::OnDrawMesh(IMesh*, int, int)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::OnThreadEvent(unsigned int)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::OverrideConfig(MaterialSystem_Config_t const&, bool)` | differs | size retail 3709 vs ours 3741 |
| cmaterialsystem | `CMaterialSystem::PreloadReplacements()` | differs | size retail 244 vs ours 319 |
| cmaterialsystem | `CMaterialSystem::QueryInterface(char const*)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::ReadConfigFromConVars(MaterialSystem_Config_t*)` | differs | size retail 1009 vs ours 997 |
| cmaterialsystem | `CMaterialSystem::ReleaseShaderObjects()` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::ReleaseStandardTextures()` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::ReloadFilesInList(IFileList*)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::ReloadMaterials(char const*)` | differs | size retail 1007 vs ours 1109 |
| cmaterialsystem | `CMaterialSystem::ReloadTextures()` | differs | size retail 43 vs ours 67 |
| cmaterialsystem | `CMaterialSystem::RemoveReleaseFunc(void (*)())` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::RemoveRestoreFunc(void (*)(int))` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::RemoveTextureAlias(char const*)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::ResetTempHWMemory(bool)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::RestoreShaderObjects(void* (*)(char const*, int*), int)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::ScanDirForReplacements(char const*)` | differs | size retail 671 vs ours 639 |
| cmaterialsystem | `CMaterialSystem::SetAdapter(int, int)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::SetAsyncTextureLoadCache(void*)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::SetExcludedTextures(char const*)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::SetInStubMode(bool)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::SetMaterialProxyFactory(IMaterialProxyFactory*)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::SetMode(void*, MaterialSystem_Config_t const&)` | differs | bytes |
| cmaterialsystem | `CMaterialSystem::SetRenderTargetFrameBufferSizeOverrides(int, int)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::SetShaderAPI(char const*)` | differs | size retail 354 vs ours 421 |
| cmaterialsystem | `CMaterialSystem::SetThreadMode(MaterialThreadMode_t, int)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::Shutdown()` | differs | size retail 248 vs ours 290 |
| cmaterialsystem | `CMaterialSystem::SpewDriverInfo() const` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::ThreadAcquire(bool)` | differs | size retail 443 vs ours 447 |
| cmaterialsystem | `CMaterialSystem::ThreadExecuteQueuedContext(CMatQueuedRenderContext*)` | differs | size retail 100 vs ours 106 |
| cmaterialsystem | `CMaterialSystem::ThreadRelease()` | differs | size retail 613 vs ours 609 |
| cmaterialsystem | `CMaterialSystem::UncacheAllMaterials()` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::UncacheUnusedMaterials(bool)` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::Unlock(MaterialLock_t__*)` | differs | size retail 270 vs ours 250 |
| cmaterialsystem | `CMaterialSystem::UpdateExcludedTextures()` | matched | fixed (was differs) |
| cmaterialsystem | `CMaterialSystem::WriteConfigIntoConVars(MaterialSystem_Config_t const&)` | differs | size retail 1192 vs ours 1165 |
| cmaterialsystem | `CMaterialSystem::WriteConfigurationInfoToConVars(bool)` | differs | bytes |
| cmaterialsystem | `CMaterialSystem::~CMaterialSystem()` | matched | fixed (was differs) |
| cmaterialsystem | `CReplacementProxy::Init(IMaterial*, KeyValues*)` | differs | size retail 139 vs ours 131 |
| cmaterialsystem | `CReplacementProxy::Release()` | differs | size retail 25 vs ours 46 |
| cmaterialsystem | `MatProxyCallback(IConVar*, char const*, float)` | differs | size retail 139 vs ours 77 |
| cmaterialsystem | `__CreateCReplacementProxy_interface()` | differs | size retail 37 vs ours 44 |
| cmaterialsystem | `non-virtual thunk to CMaterialSystem::BindStandardVertexTexture(VertexTextureSampler_t, StandardTextureId_t)` | matched | fixed (was differs) |
| cmaterialsystem | `non-virtual thunk to CMaterialSystem::Disconnect()` | differs | size retail 199 vs ours 25 |
| cmaterialsystem | `non-virtual thunk to CMaterialSystem::GetStandardTextureDimensions(int*, int*, StandardTextureId_t)` | matched | fixed (was differs) |
| cmaterialsystem | `non-virtual thunk to CMaterialSystem::GetThreadMode()` | matched | fixed (was differs) |
| cmaterialsystem | `non-virtual thunk to CMaterialSystem::IsInStubMode()` | matched | fixed (was differs) |
| cmaterialsystem | `non-virtual thunk to CMaterialSystem::IsRenderThreadSafe()` | matched | fixed (was differs) |
| cmaterialsystem | `non-virtual thunk to CMaterialSystem::OnDrawMesh(IMesh*, CPrimList*, int)` | matched | fixed (was differs) |
| cmaterialsystem | `non-virtual thunk to CMaterialSystem::OnDrawMesh(IMesh*, int, int)` | matched | fixed (was differs) |
| cmaterialsystem | `non-virtual thunk to CMaterialSystem::OnThreadEvent(unsigned int)` | matched | fixed (was differs) |
| cmaterialsystem | `non-virtual thunk to CMaterialSystem::QueryInterface(char const*)` | matched | fixed (was differs) |
| cmaterialvar | `CMaterialVar::CMaterialVar(IMaterial*, char const*, VMatrix const&)` | differs | bytes |
| cmaterialvar | `CMaterialVar::CleanUpData()` | differs | size retail 192 vs ours 180 |
| cmaterialvar | `CMaterialVar::CopyFrom(IMaterialVar*)` | matched | fixed (was differs) |
| cmaterialvar | `CMaterialVar::GetStringValue() const` | matched | fixed (was differs) |
| cmaterialvar | `CMaterialVar::GetTextureValue()` | matched | fixed (was differs) |
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
| cmatlightmaps | `CMatLightmaps::AllocateLightmap(int, int, int*, IMaterial*)` | differs | bytes |
| cmatlightmaps | `CMatLightmaps::BeginLightmapAllocation()` | differs | bytes |
| cmatlightmaps | `CMatLightmaps::BumpedLightmapBitsToPixelWriter_HDRF(float*, float*, float*, float*, int*, int*, FloatBitMap_t*)` | differs | size retail 1957 vs ours 1935 |
| cmatlightmaps | `CMatLightmaps::BumpedLightmapBitsToPixelWriter_HDRI(float*, float*, float*, float*, int*, int*, FloatBitMap_t*)` | differs | size retail 5328 vs ours 5654 |
| cmatlightmaps | `CMatLightmaps::BumpedLightmapBitsToPixelWriter_LDR(float*, float*, float*, float*, int*, int*, FloatBitMap_t*)` | differs | size retail 6584 vs ours 6784 |
| cmatlightmaps | `CMatLightmaps::EndLightmapAllocation()` | differs | size retail 453 vs ours 502 |
| cmatlightmaps | `CMatLightmaps::InitLightmapBits(int)` | matched | fixed (was differs) |
| cmatlightmaps | `CMatLightmaps::LightmapBitsToPixelWriter_HDRF(float*, int*, int*, FloatBitMap_t*)` | differs | size retail 651 vs ours 622 |
| cmatlightmaps | `CMatLightmaps::LightmapBitsToPixelWriter_HDRI(float*, int*, int*, FloatBitMap_t*)` | differs | size retail 1584 vs ours 1408 |
| cmatlightmaps | `CMatLightmaps::LightmapBitsToPixelWriter_LDR(float*, int*, int*, FloatBitMap_t*)` | differs | size retail 1046 vs ours 944 |
| cmatnullrendercontext | `CMatNullRenderContext::~CMatNullRenderContext()` | matched | fixed (was differs) |
| cmatnullrendercontext | `CMatNullRenderContext::~CMatNullRenderContext()` | matched | fixed (was differs) |
| cmatnullrendercontext | `non-virtual thunk to CMatNullRenderContext::~CMatNullRenderContext()` | matched | fixed (was differs) |
| cmatnullrendercontext | `non-virtual thunk to CMatNullRenderContext::~CMatNullRenderContext()` | matched | fixed (was differs) |
| cmatqueuedrendercontext | `CMatQueuedMesh::ExecuteDefferredBuild(unsigned char*, int, int, unsigned short*, int)` | differs | size retail 1035 vs ours 1103 |
| cmatqueuedrendercontext | `CMatQueuedMesh::LockMesh(int, int, MeshDesc_t&)` | matched | fixed (was differs) |
| cmatqueuedrendercontext | `CMatQueuedMesh::UnlockMesh(int, int, MeshDesc_t&)` | matched | fixed (was differs) |
| cmatqueuedrendercontext | `CMatQueuedRenderContext::BeginQueue(CMatRenderContextBase*)` | matched | fixed (was differs) |
| cmatqueuedrendercontext | `CMatQueuedRenderContext::CallQueued(bool)` | differs | size retail 372 vs ours 388 |
| cmatqueuedrendercontext | `CMatQueuedRenderContext::CompactMemory()` | matched | fixed (was differs) |
| cmatqueuedrendercontext | `CMatQueuedRenderContext::FogColor3ub(unsigned char, unsigned char, unsigned char)` | matched | fixed (was differs) |
| cmatqueuedrendercontext | `CMatQueuedRenderContext::FogEnd(float)` | matched | fixed (was differs) |
| cmatqueuedrendercontext | `CMatQueuedRenderContext::FogMaxDensity(float)` | matched | fixed (was differs) |
| cmatqueuedrendercontext | `CMatQueuedRenderContext::FogRadial(bool)` | matched | fixed (was differs) |
| cmatqueuedrendercontext | `CMatQueuedRenderContext::FogStart(float)` | matched | fixed (was differs) |
| cmatqueuedrendercontext | `CMatQueuedRenderContext::GetCallQueue()` | matched | fixed (was differs) |
| cmatqueuedrendercontext | `CMatQueuedRenderContext::GetFogColor(unsigned char*)` | matched | fixed (was differs) |
| cmatqueuedrendercontext | `CMatQueuedRenderContext::GetFogDistances(float*, float*, float*)` | matched | fixed (was differs) |
| cmatqueuedrendercontext | `CMatQueuedRenderContext::GetFogRadial()` | matched | fixed (was differs) |
| cmatqueuedrendercontext | `CMatQueuedRenderContext::Init(CMaterialSystem*, CMatRenderContextBase*)` | matched | fixed (was differs) |
| cmatqueuedrendercontext | `CMatQueuedRenderContext::SetFogZ(float)` | matched | fixed (was differs) |
| cmatqueuedrendercontext | `CMatQueuedRenderContext::~CMatQueuedRenderContext()` | matched | fixed (was differs) |
| cmatqueuedrendercontext | `CMatQueuedRenderContext::~CMatQueuedRenderContext()` | matched | fixed (was differs) |
| cmatqueuedrendercontext | `non-virtual thunk to CMatQueuedRenderContext::~CMatQueuedRenderContext()` | matched | fixed (was differs) |
| cmatqueuedrendercontext | `non-virtual thunk to CMatQueuedRenderContext::~CMatQueuedRenderContext()` | matched | fixed (was differs) |
| cmatrendercontext | `CMatRenderContext::BindBumpLightmap(Sampler_t)` | differs | bytes |
| cmatrendercontext | `CMatRenderContext::BindLightmap(Sampler_t)` | differs | bytes |
| cmatrendercontext | `CMatRenderContext::BindLocalCubemap(ITexture*)` | matched | fixed (was differs) |
| cmatrendercontext | `CMatRenderContext::BindMorph(IMorph*)` | differs | size retail 101 vs ours 108 |
| cmatrendercontext | `CMatRenderContext::BindStandardTexture(Sampler_t, StandardTextureId_t)` | differs | bytes |
| cmatrendercontext | `CMatRenderContext::BindStandardVertexTexture(VertexTextureSampler_t, StandardTextureId_t)` | differs | bytes |
| cmatrendercontext | `CMatRenderContext::CommitRenderTargetAndViewport()` | differs | bytes |
| cmatrendercontext | `CMatRenderContext::CopyRenderTargetToTextureEx(ITexture*, int, Rect_t*, Rect_t*)` | differs | bytes |
| cmatrendercontext | `CMatRenderContext::CopyTextureToRenderTargetEx(int, ITexture*, Rect_t*, Rect_t*)` | differs | bytes |
| cmatrendercontext | `CMatRenderContext::DrawClearBufferQuad(unsigned char, unsigned char, unsigned char, unsigned char, bool, bool, bool)` | matched | fixed (was differs) |
| cmatrendercontext | `CMatRenderContext::DrawScreenSpaceQuad(IMaterial*)` | matched | fixed (was differs) |
| cmatrendercontext | `CMatRenderContext::EnableNonInteractiveMode(MaterialNonInteractiveMode_t)` | differs | bytes |
| cmatrendercontext | `CMatRenderContext::GetStandardTextureDimensions(int*, int*, StandardTextureId_t)` | matched | fixed (was differs) |
| cmatrendercontext | `CMatRenderContext::Init(CMaterialSystem*)` | matched | fixed (was differs) |
| cmatrendercontext | `CMatRenderContext::PopCustomClipPlane()` | matched | fixed (was differs) |
| cmatrendercontext | `CMatRenderContext::PushCustomClipPlane(float const*)` | matched | fixed (was differs) |
| cmatrendercontext | `CMatRenderContext::SetRenderTargetEx(int, ITexture*)` | differs | bytes |
| cmatrendercontext | `CMatRenderContext::UpdateHeightClipUserClipPlane()` | matched | fixed (was differs) |
| cmatrendercontext | `CMatRenderContextBase::BindLocalCubemap(ITexture*)` | matched | fixed (was differs) |
| cmatrendercontext | `CMatRenderContextBase::EnableClipping(bool)` | matched | fixed (was differs) |
| cmatrendercontext | `CMatRenderContextBase::PopRenderTargetAndViewport()` | differs | size retail 30 vs ours 56 |
| cmatrendercontext | `CMatRenderContextBase::PushRenderTargetAndViewport()` | matched | fixed (was differs) |
| cmatrendercontext | `CMatRenderContextBase::PushRenderTargetAndViewport(ITexture*)` | matched | fixed (was differs) |
| cmatrendercontext | `CMatRenderContextBase::PushRenderTargetAndViewport(ITexture*, ITexture*, int, int, int, int)` | matched | fixed (was differs) |
| cmatrendercontext | `CMatRenderContextBase::PushRenderTargetAndViewport(ITexture*, int, int, int, int)` | matched | fixed (was differs) |
| cmatrendercontext | `CMatRenderContextBase::ReleaseRenderData()` | matched | fixed (was differs) |
| cmatrendercontext | `CMatRenderContextBase::SetHeightClipMode(MaterialHeightClipMode_t)` | matched | fixed (was differs) |
| cmatrendercontext | `CMatRenderContextBase::SetHeightClipZ(float)` | matched | fixed (was differs) |
| ctexture | `CReferenceToHandleTexture::DeleteIfUnreferenced()` | matched | fixed (was differs) |
| ctexture | `CReferenceToHandleTexture::Release()` | matched | fixed (was missing) |
| ctexture | `CReferenceToHandleTexture::SetFilteringAndClampingMode()` | matched | fixed (was missing) |
| ctexture | `CReferenceToHandleTexture::SetRenderTarget(int)` | differs | bytes |
| ctexture | `CTexture::AllocateShaderAPITextures()` | differs | size retail 660 vs ours 681 |
| ctexture | `CTexture::ApplyRenderTargetSizeMode(int&, int&, ImageFormat)` | differs | size retail 1136 vs ours 1120 |
| ctexture | `CTexture::Bind(Sampler_t)` | differs | size retail 45 vs ours 48 |
| ctexture | `CTexture::Bind(Sampler_t, int, Sampler_t)` | differs | size retail 97 vs ours 183 |
| ctexture | `CTexture::BindVertexTexture(VertexTextureSampler_t, int)` | differs | bytes |
| ctexture | `CTexture::CTexture()` | differs | size retail 342 vs ours 498 |
| ctexture | `CTexture::ComputeActualFormat(ImageFormat)` | differs | size retail 283 vs ours 277 |
| ctexture | `CTexture::ComputeActualSize(bool, IVTFTexture*)` | missing |  |
| ctexture | `CTexture::ConvertToActualFormat(IVTFTexture*, bool)` | missing |  |
| ctexture | `CTexture::CopyFrameBufferToMe(int, Rect_t*, Rect_t*)` | differs | bytes |
| ctexture | `CTexture::CopyLowResImageToTexture(IVTFTexture*)` | differs | size retail 1760 vs ours 1776 |
| ctexture | `CTexture::CopyMeToFrameBuffer(int, Rect_t*, Rect_t*)` | differs | bytes |
| ctexture | `CTexture::DecrementReferenceCount()` | matched | fixed (was differs) |
| ctexture | `CTexture::DeleteIfUnreferenced()` | matched | fixed (was differs) |
| ctexture | `CTexture::Download(Rect_t*, int)` | differs | size retail 190 vs ours 198 |
| ctexture | `CTexture::GenerateShowMipLevelsTextures(IVTFTexture*)` | matched | fixed (was differs) |
| ctexture | `CTexture::GetActualDepth() const` | differs | bytes |
| ctexture | `CTexture::GetActualHeight() const` | differs | bytes |
| ctexture | `CTexture::GetActualWidth() const` | differs | bytes |
| ctexture | `CTexture::GetLowResColorSample(float, float, float*) const` | differs | size retail 638 vs ours 641 |
| ctexture | `CTexture::GetMappingDepth() const` | differs | bytes |
| ctexture | `CTexture::GetNumAnimationFrames() const` | differs | bytes |
| ctexture | `CTexture::GetOptimalReadBuffer(void*, int, CUtlBuffer&)` | missing |  |
| ctexture | `CTexture::GetReflectivity(Vector&)` | differs | bytes |
| ctexture | `CTexture::GetResourceData(unsigned int, unsigned long*) const` | differs | size retail 107 vs ours 123 |
| ctexture | `CTexture::GetTextureHandle(int, int)` | differs | size retail 49 vs ours 69 |
| ctexture | `CTexture::HandleFileLoadFailedTexture(IVTFTexture*)` | differs | size retail 317 vs ours 339 |
| ctexture | `CTexture::Init(int, int, int, ImageFormat, int, int, bool)` | missing |  |
| ctexture | `CTexture::InitRenderTarget(char const*, int, int, RenderTargetSizeMode_t, ImageFormat, RenderTargetType_t, unsigned int, unsigned int)` | differs | size retail 557 vs ours 555 |
| ctexture | `CTexture::IsVolumeTexture() const` | differs | bytes |
| ctexture | `CTexture::LoadLowResTexture(IVTFTexture*)` | differs | size retail 211 vs ours 223 |
| ctexture | `CTexture::LoadTextureBitsFromFile(char*, char**, bool)` | missing |  |
| ctexture | `CTexture::MarkAsExcluded(bool, int)` | differs | bytes |
| ctexture | `CTexture::OnRestore()` | differs | size retail 269 vs ours 264 |
| ctexture | `CTexture::Precache()` | differs | size retail 963 vs ours 1260 |
| ctexture | `CTexture::ReconstructPartialProceduralBits(Rect_t const*, Rect_t*)` | differs | size retail 470 vs ours 660 |
| ctexture | `CTexture::ReconstructPartialTexture(Rect_t const*)` | differs | bytes |
| ctexture | `CTexture::ReconstructProceduralBits()` | differs | size retail 256 vs ours 438 |
| ctexture | `CTexture::ReconstructTexture()` | missing |  |
| ctexture | `CTexture::Release()` | differs | size retail 129 vs ours 26 |
| ctexture | `CTexture::SaveToFile(char const*)` | differs | size retail 4192 vs ours 4344 |
| ctexture | `CTexture::SetFilterState()` | matched | fixed (was differs) |
| ctexture | `CTexture::SetFilteringAndClampingMode()` | differs | size retail 103 vs ours 104 |
| ctexture | `CTexture::SetRenderTarget(int)` | differs | bytes |
| ctexture | `CTexture::SetRenderTarget(int, ITexture*)` | differs | bytes |
| ctexture | `CTexture::SetTextureRegenerator(ITextureRegenerator*)` | differs | size retail 39 vs ours 45 |
| ctexture | `CTexture::SetupDebuggingTextures(IVTFTexture*)` | differs | bytes |
| ctexture | `CTexture::Shutdown()` | differs | size retail 289 vs ours 201 |
| ctexture | `CTexture::SwapContents(ITexture*)` | differs | size retail 177 vs ours 339 |
| ctexture | `CTexture::UpdateExcludedState()` | differs | bytes |
| ctexture | `CTexture::~CTexture()` | differs | size retail 228 vs ours 294 |
| ctexture | `ITextureInternal::ChangeRenderTarget(ITextureInternal*, int, int, RenderTargetSizeMode_t, ImageFormat, RenderTargetType_t, unsigned int, unsigned int)` | differs | bytes |
| ctexture | `ITextureInternal::CreateFileTexture(char const*, char const*)` | differs | bytes |
| ctexture | `ITextureInternal::CreateProceduralTexture(char const*, char const*, int, int, int, ImageFormat, int)` | missing |  |
| ctexture | `ITextureInternal::CreateRenderTarget(char const*, int, int, RenderTargetSizeMode_t, ImageFormat, RenderTargetType_t, unsigned int, unsigned int)` | differs | bytes |
| ctexture | `ITextureInternal::Destroy(ITextureInternal*)` | missing |  |
| mat_stub | `CDummyHardwareConfig::SupportsNormalMapCompression() const` | missing |  |
| mat_stub | `CDummyMaterialSystem::GetRenderBackend() const` | differs | size retail 10 vs ours 39 |
| mat_stub | `CDummyMaterialSystem::SupportsNormalMapCompression()` | missing |  |
| morph | `CMorph::RegenerateTextureBits(ITexture*, IVTFTexture*, Rect_t*)` | matched | fixed (was differs) |
| morph | `CMorph::WriteDeltaPositionNormalToTexture(CPixelWriter&, int, int, MorphVertexInfo_t const&)` | matched | fixed (was differs) |
| morph | `CMorph::WriteSideSpeedToTexture(CPixelWriter&, int, int, MorphVertexInfo_t const&)` | matched | fixed (was differs) |
| morph | `CMorphMgr::AllocateMaterials()` | differs | size retail 519 vs ours 495 |
| morph | `CUtlSortVector<CVertexMorphDict::MorphVertexList_t, CVertexMorphDict::VertexMorphDictLess, CUtlVector<CVertexMorphDict::MorphVertexList_t, CUtlMemory<CVertexMorphDict::MorphVertexList_t, int>>>::Insert(CVertexMorphDict::MorphVertexList_t const&)` | differs | bytes |
| morph | `int CUtlSortVector<CVertexMorphDict::MorphVertexList_t, CVertexMorphDict::VertexMorphDictLess, CUtlVector<CVertexMorphDict::MorphVertexList_t, CUtlMemory<CVertexMorphDict::MorphVertexList_t, int>>>::Find<CVertexMorphDict::MorphVertexList_t>(CVertexMorphDict::MorphVertexList_t const&) const` | differs | bytes |
| morph | `int CUtlSortVector<CVertexMorphDict::MorphVertexList_t, CVertexMorphDict::VertexMorphDictLess, CUtlVector<CVertexMorphDict::MorphVertexList_t, CUtlMemory<CVertexMorphDict::MorphVertexList_t, int>>>::FindLessOrEqual<CVertexMorphDict::MorphVertexList_t>(CVertexMorphDict::MorphVertexList_t const&, bool*) const` | differs | bytes |
| shadersystem | `CShaderSystem::BindTexture(Sampler_t, ITexture*, int)` | differs | size retail 50 vs ours 53 |
| shadersystem | `CShaderSystem::BindTexture(Sampler_t, Sampler_t, ITexture*, int)` | differs | size retail 79 vs ours 82 |
| shadersystem | `CShaderSystem::GetShaderAPITextureBindHandle(ITexture*, int, int)` | differs | bytes |
| shadersystem | `CShaderSystem::InitShaderParameters(IShader*, IMaterialVar**, char const*)` | differs | size retail 624 vs ours 640 |
| shadersystem | `CShaderSystem::LoadBumpMap(IMaterialVar*, char const*)` | matched | fixed (was differs) |
| shadersystem | `CShaderSystem::LoadCubeMap(IMaterialVar**, IMaterialVar*, int)` | matched | fixed (was differs) |
| shadersystem | `CShaderSystem::LoadTexture(IMaterialVar*, char const*, int)` | matched | fixed (was differs) |
| shadersystem | `non-virtual thunk to CShaderSystem::BindTexture(Sampler_t, ITexture*, int)` | differs | size retail 50 vs ours 53 |
| shadersystem | `non-virtual thunk to CShaderSystem::BindTexture(Sampler_t, Sampler_t, ITexture*, int)` | differs | size retail 79 vs ours 82 |
| shadersystem | `non-virtual thunk to CShaderSystem::GetShaderAPITextureBindHandle(ITexture*, int, int)` | differs | bytes |
| texturemanager | `CCheckerboardTexture::RegenerateTextureBits(ITexture*, IVTFTexture*, Rect_t*)` | matched | fixed (was differs) |
| texturemanager | `CCheckerboardTexture::Release()` | differs | size retail 18 vs ours 20 |
| texturemanager | `CColorCorrectionTexture::RegenerateTextureBits(ITexture*, IVTFTexture*, Rect_t*)` | matched | fixed (was differs) |
| texturemanager | `CColorCorrectionTexture::Release()` | differs | size retail 18 vs ours 20 |
| texturemanager | `CNormalizationCubemap::RegenerateTextureBits(ITexture*, IVTFTexture*, Rect_t*)` | differs | size retail 4256 vs ours 3142 |
| texturemanager | `CSignedNormalizationCubemap::RegenerateTextureBits(ITexture*, IVTFTexture*, Rect_t*)` | differs | size retail 6768 vs ours 3840 |
| texturemanager | `CSolidTexture::RegenerateTextureBits(ITexture*, IVTFTexture*, Rect_t*)` | matched | fixed (was differs) |
| texturemanager | `CSolidTexture::Release()` | differs | size retail 18 vs ours 20 |
| texturemanager | `CTextureManager::AddTextureAlias(char const*, char const*)` | matched | fixed (was differs) |
| texturemanager | `CTextureManager::AllocateStandardRenderTargets()` | matched | fixed (was differs) |
| texturemanager | `CTextureManager::CacheExternalStandardRenderTargets()` | matched | fixed (was differs) |
| texturemanager | `CTextureManager::ColorCorrectionTexture(int)` | matched | fixed (was differs) |
| texturemanager | `CTextureManager::CreateProceduralTexture(char const*, char const*, int, int, int, ImageFormat, int)` | differs | size retail 146 vs ours 154 |
| texturemanager | `CTextureManager::DebugPrintUsedTextures()` | matched | fixed (was differs) |
| texturemanager | `CTextureManager::ErrorTexture()` | matched | fixed (was differs) |
| texturemanager | `CTextureManager::FindOrLoadTexture(char const*, char const*, int)` | matched | fixed (was differs) |
| texturemanager | `CTextureManager::FreeStandardRenderTargets()` | matched | fixed (was differs) |
| texturemanager | `CTextureManager::FullFrameDepthTexture()` | matched | fixed (was differs) |
| texturemanager | `CTextureManager::GenerateErrorTexture(ITexture*, IVTFTexture*)` | matched | fixed (was differs) |
| texturemanager | `CTextureManager::IdentityLightWarp()` | matched | fixed (was differs) |
| texturemanager | `CTextureManager::Init(int)` | differs | bytes |
| texturemanager | `CTextureManager::LoadTexture(char const*, char const*, int)` | differs | bytes |
| texturemanager | `CTextureManager::NormalizationCubemap()` | matched | fixed (was differs) |
| texturemanager | `CTextureManager::ReleaseTextures()` | differs | bytes |
| texturemanager | `CTextureManager::ReloadFilesInList(IFileList*)` | differs | bytes |
| texturemanager | `CTextureManager::RemoveTexture(ITextureInternal*)` | differs | size retail 151 vs ours 159 |
| texturemanager | `CTextureManager::RemoveUnusedTextures()` | differs | size retail 188 vs ours 196 |
| texturemanager | `CTextureManager::RequestNextTextureID()` | matched | fixed (was differs) |
| texturemanager | `CTextureManager::ResetTextureFilteringState()` | differs | bytes |
| texturemanager | `CTextureManager::RestoreNonRenderTargetTextures()` | differs | bytes |
| texturemanager | `CTextureManager::RestoreRenderTargets()` | matched | fixed (was differs) |
| texturemanager | `CTextureManager::SetColorCorrectionTexture(int, ITextureInternal*)` | matched | fixed (was differs) |
| texturemanager | `CTextureManager::SetExcludedTextures(char const*)` | differs | bytes |
| texturemanager | `CTextureManager::ShadowNoise2D()` | matched | fixed (was differs) |
| texturemanager | `CTextureManager::Shutdown()` | differs | bytes |
| texturemanager | `CTextureManager::SignedNormalizationCubemap()` | matched | fixed (was differs) |
| texturemanager | `CTextureManager::UpdateExcludedTextures()` | differs | bytes |
| texturemanager | `CTextureManager::~CTextureManager()` | matched | fixed (was differs) |
