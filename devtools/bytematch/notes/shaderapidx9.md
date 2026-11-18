# shaderapidx9 (retail macOS)

Retail: depot 223 `bin/shaderapidx9.dylib`; own objects listed below. Did not link in
`--retail-osx32` at lane start (no per-function baseline; the ToGL lane measured 943/1361 of
the compiled objects). Measured with `bytematch diff` against
`build-retail/materialsystem/shaderapidx9`; "matched" = exact or matches-after-masking.

Main changes: D3DX helpers inlined from `public/togl/d3dx_impl.h` (retail header named by the
debug map; vtable-less matrix stack), `nativeGetClientRect`; retail `DynamicState_t`,
`SamplerState_t`, `Texture_t` and `HardwareCaps_t` layouts; streaming hooks and
ShaderApiStreaming001 out of the retail path; `IsOpenGL()` as a render backend query; D3D9
`D3DFORMAT` values; leak bodies for constant uploads and caps.

Known causes of the rest: `colorformatdx8` needs SDK 2013's 39-entry `ImageFormat` enum
(cross-module header); the six `CTransitionTable` sort-vector functions differ only in
`AssertFatal` line numbers from `public/tier1/utlsortvector.h` (ours is one line short of
SDK 2013 before line 179); meshdx8/texturedx8 are TF2-era dynamic buffer and texture code.

| object | functions | matched now | matched at lane start | % bytes now |
|---|---:|---:|---:|---:|
| TransitionTable | 73 | 67 | - | 90.9 |
| colorformatdx8 | 7 | 5 | - | 36.7 |
| cvballoctracker | 5 | 5 | - | 100.0 |
| hardwareconfig | 92 | 90 | - | 71.4 |
| meshbase | 22 | 22 | - | 100.0 |
| meshdx8 | 249 | 225 | - | 71.5 |
| shaderapibase | 5 | 5 | - | 100.0 |
| shaderapidx8 | 642 | 632 | - | 95.9 |
| shaderdevicebase | 46 | 43 | - | 65.1 |
| shaderdevicedx8 | 67 | 64 | - | 72.2 |
| shadershadowdx8 | 65 | 65 | - | 100.0 |
| texturedx8 | 11 | 8 | - | 74.8 |
| vertexdecl | 17 | 16 | - | 98.8 |
| vertexshaderdx8 | 49 | 47 | - | 95.5 |
| winutils | 3 | 3 | - | 100.0 |
| **total** | 1353 | 1297 | - | 85.6 |

Status column: matched = exact or matches-after-masking; "fixed" = differed or was missing at lane start.

| object | function | status | note |
|---|---|---|---|
| TransitionTable | `CUtlSortVector<CTransitionTable::ShadowStateDictEntry_t, CTransitionTable::ShadowStateDictLessFunc, CUtlVector<CTransitionTable::ShadowStateDictEntry_t, CUtlMemory<CTransitionTable::ShadowStateDictEntry_t, int>>>::Insert(CTransitionTable::ShadowStateDictEntry_t const&)` | differs | bytes |
| TransitionTable | `CUtlSortVector<CTransitionTable::SnapshotDictEntry_t, CTransitionTable::SnapshotDictLessFunc, CUtlVector<CTransitionTable::SnapshotDictEntry_t, CUtlMemory<CTransitionTable::SnapshotDictEntry_t, int>>>::Insert(CTransitionTable::SnapshotDictEntry_t const&)` | differs | bytes |
| TransitionTable | `CUtlSortVector<CTransitionTable::TransitionList_t, CTransitionTable::UniqueSnapshotLessFunc, CUtlVector<CTransitionTable::TransitionList_t, CUtlMemory<CTransitionTable::TransitionList_t, int>>>::Insert(CTransitionTable::TransitionList_t const&)` | differs | bytes |
| TransitionTable | `int CUtlSortVector<CTransitionTable::ShadowStateDictEntry_t, CTransitionTable::ShadowStateDictLessFunc, CUtlVector<CTransitionTable::ShadowStateDictEntry_t, CUtlMemory<CTransitionTable::ShadowStateDictEntry_t, int>>>::FindLessOrEqual<CTransitionTable::ShadowStateDictEntry_t>(CTransitionTable::ShadowStateDictEntry_t const&, bool*) const` | differs | bytes |
| TransitionTable | `int CUtlSortVector<CTransitionTable::SnapshotDictEntry_t, CTransitionTable::SnapshotDictLessFunc, CUtlVector<CTransitionTable::SnapshotDictEntry_t, CUtlMemory<CTransitionTable::SnapshotDictEntry_t, int>>>::FindLessOrEqual<CTransitionTable::SnapshotDictEntry_t>(CTransitionTable::SnapshotDictEntry_t const&, bool*) const` | differs | bytes |
| TransitionTable | `int CUtlSortVector<CTransitionTable::TransitionList_t, CTransitionTable::UniqueSnapshotLessFunc, CUtlVector<CTransitionTable::TransitionList_t, CUtlMemory<CTransitionTable::TransitionList_t, int>>>::FindLessOrEqual<CTransitionTable::TransitionList_t>(CTransitionTable::TransitionList_t const&, bool*) const` | differs | bytes |
| colorformatdx8 | `GetNearestD3DColorFormat(ImageFormat, bool, bool, bool)` | differs | size retail 4868 vs ours 4880 |
| colorformatdx8 | `InitializeColorInformation(unsigned int, _D3DDEVTYPE, ImageFormat)` | differs | bytes |
| hardwareconfig | `CHardwareConfig::ForceCapsToDXLevel(HardwareCaps_t*, int, HardwareCaps_t const&)` | differs | size retail 1844 vs ours 1833 |
| hardwareconfig | `CHardwareConfig::SetupHardwareCaps(int, HardwareCaps_t const&)` | differs | size retail 549 vs ours 503 |
| meshdx8 | `CDynamicMeshDX8::SetVertexFormat(unsigned long long)` | differs | size retail 168 vs ours 192 |
| meshdx8 | `CIndexBuffer::Lock(bool, int, int&, int)` | differs | bytes |
| meshdx8 | `CMeshDX8::Lock(bool, int, int, IndexDesc_t&)` | differs | size retail 549 vs ours 555 |
| meshdx8 | `CMeshDX8::Lock(int, bool, VertexDesc_t&)` | differs | size retail 1495 vs ours 1460 |
| meshdx8 | `CMeshDX8::SetRenderState(int, int, unsigned long long)` | differs | size retail 337 vs ours 223 |
| meshdx8 | `CMeshDX8::Unlock(int, IndexDesc_t&)` | differs | bytes |
| meshdx8 | `CMeshDX8::~CMeshDX8()` | differs | size retail 599 vs ours 362 |
| meshdx8 | `CMeshMgr::CopyStaticMeshIndexBufferToTempMeshIndexBuffer(CTempMeshDX8*, CMeshDX8*)` | differs | bytes |
| meshdx8 | `CMeshMgr::CreateDynamicIndexBuffer()` | differs | size retail 329 vs ours 363 |
| meshdx8 | `CMeshMgr::CreateVertexIDBuffer()` | differs | size retail 638 vs ours 603 |
| meshdx8 | `CMeshMgr::DestroyDynamicIndexBuffer()` | differs | size retail 306 vs ours 25 |
| meshdx8 | `CMeshMgr::DestroyVertexBuffers()` | differs | size retail 778 vs ours 887 |
| meshdx8 | `CMeshMgr::DiscardVertexBuffers()` | differs | bytes |
| meshdx8 | `CMeshMgr::GetDynamicMesh(IMaterial*, unsigned long long, int, bool, IMesh*, IMesh*)` | differs | size retail 448 vs ours 460 |
| meshdx8 | `CMeshMgr::GetFlexMesh()` | differs | size retail 325 vs ours 337 |
| meshdx8 | `CMeshMgr::GetMaxVerticesToRender(IMaterial*)` | differs | size retail 95 vs ours 124 |
| meshdx8 | `CMeshMgr::ReleaseBuffers()` | differs | size retail 375 vs ours 487 |
| meshdx8 | `CMeshMgr::SetRenderState(int, int, unsigned long long, int)` | differs | size retail 427 vs ours 435 |
| meshdx8 | `CMeshMgr::Shutdown()` | differs | size retail 77 vs ours 80 |
| meshdx8 | `CTempMeshDX8::BeginPass()` | differs | size retail 941 vs ours 973 |
| meshdx8 | `CTempMeshDX8::GetDynamicMesh()` | differs | size retail 218 vs ours 52 |
| meshdx8 | `CTempMeshDX8::RenderPass()` | differs | size retail 725 vs ours 768 |
| meshdx8 | `CVertexBufferDx8::CVertexBufferDx8(ShaderBufferType_t, unsigned long long, int, char const*)` | differs | size retail 482 vs ours 475 |
| meshdx8 | `non-virtual thunk to CMeshDX8::Unlock(int, IndexDesc_t&)` | differs | bytes |
| shaderapidx8 | `CShaderAPIDx8::CommitVertexShaderLighting()` | differs | bytes |
| shaderapidx8 | `CShaderAPIDx8::ComputeGammaCorrectedFogColor(unsigned char, unsigned char, unsigned char, bool)` | differs | size retail 449 vs ours 422 |
| shaderapidx8 | `CShaderAPIDx8::CreateTextureHandles(int*, int)` | differs | size retail 289 vs ours 296 |
| shaderapidx8 | `CShaderAPIDx8::CreateTextures(int*, int, int, int, int, ImageFormat, int, int, int, char const*, char const*)` | differs | size retail 759 vs ours 715 |
| shaderapidx8 | `CShaderAPIDx8::GetDX9LightState(LightState_t*) const` | differs | size retail 294 vs ours 298 |
| shaderapidx8 | `CShaderAPIDx8::GetPixelFogCombo1(bool)` | differs | size retail 86 vs ours 83 |
| shaderapidx8 | `CShaderAPIDx8::OnDeviceInit()` | differs | size retail 664 vs ours 434 |
| shaderapidx8 | `CShaderAPIDx8::UserClipTransform(VMatrix const&)` | differs | size retail 201 vs ours 196 |
| shaderapidx8 | `non-virtual thunk to CShaderAPIDx8::GetPixelFogCombo1(bool)` | differs | size retail 96 vs ours 93 |
| shaderapidx8 | `non-virtual thunk to CShaderAPIDx8::UserClipTransform(VMatrix const&)` | differs | size retail 211 vs ours 206 |
| shaderdevicebase | `CShaderDeviceMgrBase::GetRecommendedConfigurationInfo(int, int, int, int, KeyValues*)` | differs | size retail 2281 vs ours 2222 |
| shaderdevicebase | `CShaderDeviceMgrBase::LoadHardwareCaps(KeyValues*, HardwareCaps_t&)` | differs | size retail 462 vs ours 410 |
| shaderdevicebase | `CShaderDeviceMgrBase::ReadDXSupportLevels(HardwareCaps_t&)` | differs | size retail 240 vs ours 206 |
| shaderdevicedx8 | `CShaderDeviceDx8::GetDisplayDeviceName()` | differs | size retail 141 vs ours 156 |
| shaderdevicedx8 | `CShaderDeviceMgrDx8::ComputeCapsFromD3D(HardwareCaps_t*, int)` | differs | size retail 4731 vs ours 4774 |
| shaderdevicedx8 | `CShaderDeviceMgrDx8::Connect(void* (*)(char const*, int*))` | differs | size retail 128 vs ours 99 |
| texturedx8 | `CreateD3DTexture(int, int, int, ImageFormat, int, int, char*)` | differs | size retail 648 vs ours 682 |
| texturedx8 | `DestroyD3DTexture(IDirect3DBaseTexture9*)` | differs | size retail 49 vs ours 345 |
| texturedx8 | `LoadTextureFromVTF(TextureLoadInfo_t&, IVTFTexture*, int)` | differs | size retail 399 vs ours 479 |
| vertexdecl | `ReleaseAllVertexDecl()` | differs | bytes |
| vertexshaderdx8 | `CShaderManager::SetPixelShader(int)` | differs | size retail 359 vs ours 234 |
| vertexshaderdx8 | `CShaderManager::SetVertexShader(int)` | differs | size retail 462 vs ours 329 |
