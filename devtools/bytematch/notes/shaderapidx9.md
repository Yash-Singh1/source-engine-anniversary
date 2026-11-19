# shaderapidx9 (retail macOS)

Retail: depot 223 `bin/shaderapidx9.dylib`; own objects listed below. Measured with
`bytematch diff` against `build-retail/materialsystem/shaderapidx9`; "matched" = exact or
matches-after-masking. "At lane start" is bytematch b1debe16 (start of lane mod-matsys3).

Earlier lanes: D3DX helpers inlined from `public/togl/d3dx_impl.h`, `nativeGetClientRect`,
retail `DynamicState_t`/`SamplerState_t`/`Texture_t`/`HardwareCaps_t` layouts, no streaming,
`IsOpenGL()` as a render backend query, D3D9 `D3DFORMAT` values, retail `ImageFormat`.

Lane mod-matsys3 (under `BYTEMATCH_RETAIL`):
- meshdx8: `CIndexBuffer` is not refcounted (no `m_nReferenceCount`, public destructor);
  meshes delete their own index buffers and the manager deletes the dynamic IB (SDK 2012
  lifetime). meshdx8.o went from 225 to 247 of 250.
- texturedx8: pool from `bManaged` only, direct release in `DestroyD3DTexture`.
- shaderdevicedx8: `Connect` fetches `g_pLauncherMgr`; `gGL` comes from sdlmgr.o;
  `GetDisplayDeviceName` without the Win10 guard.
- vertexshaderdx8: combo index range checks in `SetVertexShader`/`SetPixelShader`.
- Convar flags without `FCVAR_ALLOWED_IN_COMPETITIVE`.

Still differing: `ComputeCapsFromD3D` (HardwareCaps_t bitfield codegen), `OnDeviceInit`
(retail inlines `CreateMatrixStacks`; the inlined `ID3DXMatrixStack` setup differs in
CUtlVector constructor stores), `GetRecommendedConfigurationInfo`, `ForceCapsToDXLevel`,
`SetupHardwareCaps`, three meshdx8 lock functions, shaderdevicebase caps loading,
`GetPixelFogCombo1`.

| object | functions | matched now | matched at lane start | % bytes now |
|---|---:|---:|---:|---:|
| TransitionTable | 73 | 73 | 73 | 100.0 |
| colorformatdx8 | 7 | 7 | 7 | 100.0 |
| cvballoctracker | 5 | 5 | 5 | 100.0 |
| hardwareconfig | 92 | 90 | 90 | 71.4 |
| meshbase | 22 | 22 | 22 | 100.0 |
| meshdx8 | 249 | 246 | 225 | 93.5 |
| shaderapibase | 5 | 5 | 5 | 100.0 |
| shaderapidx8 | 642 | 634 | 633 | 97.3 |
| shaderdevicebase | 46 | 43 | 43 | 65.1 |
| shaderdevicedx8 | 67 | 66 | 64 | 73.7 |
| shadershadowdx8 | 65 | 65 | 65 | 100.0 |
| texturedx8 | 11 | 11 | 9 | 100.0 |
| vertexdecl | 17 | 16 | 16 | 98.8 |
| vertexshaderdx8 | 49 | 49 | 47 | 100.0 |
| winutils | 3 | 3 | 3 | 100.0 |
| **total** | 1353 | 1335 | 1307 | 93.8 |

Status column: matched = exact or matches-after-masking; "fixed" = differed or was missing at lane start.

| object | function | status | note |
|---|---|---|---|
| hardwareconfig | `CHardwareConfig::ForceCapsToDXLevel(HardwareCaps_t*, int, HardwareCaps_t const&)` | differs | size retail 1844 vs ours 1833 |
| hardwareconfig | `CHardwareConfig::SetupHardwareCaps(int, HardwareCaps_t const&)` | differs | size retail 549 vs ours 503 |
| meshdx8 | `CDynamicMeshDX8::SetVertexFormat(unsigned long long)` | matched | fixed (was differs) |
| meshdx8 | `CIndexBuffer::Lock(bool, int, int&, int)` | differs | bytes |
| meshdx8 | `CMeshDX8::Lock(bool, int, int, IndexDesc_t&)` | matched | fixed (was differs) |
| meshdx8 | `CMeshDX8::Lock(int, bool, VertexDesc_t&)` | differs | size retail 1495 vs ours 1460 |
| meshdx8 | `CMeshDX8::SetRenderState(int, int, unsigned long long)` | matched | fixed (was differs) |
| meshdx8 | `CMeshDX8::Unlock(int, IndexDesc_t&)` | matched | fixed (was differs) |
| meshdx8 | `CMeshDX8::~CMeshDX8()` | matched | fixed (was differs) |
| meshdx8 | `CMeshMgr::CopyStaticMeshIndexBufferToTempMeshIndexBuffer(CTempMeshDX8*, CMeshDX8*)` | matched | fixed (was differs) |
| meshdx8 | `CMeshMgr::CreateDynamicIndexBuffer()` | matched | fixed (was differs) |
| meshdx8 | `CMeshMgr::CreateVertexIDBuffer()` | differs | size retail 638 vs ours 603 |
| meshdx8 | `CMeshMgr::DestroyDynamicIndexBuffer()` | matched | fixed (was missing) |
| meshdx8 | `CMeshMgr::DestroyVertexBuffers()` | matched | fixed (was differs) |
| meshdx8 | `CMeshMgr::DiscardVertexBuffers()` | matched | fixed (was differs) |
| meshdx8 | `CMeshMgr::GetDynamicMesh(IMaterial*, unsigned long long, int, bool, IMesh*, IMesh*)` | matched | fixed (was differs) |
| meshdx8 | `CMeshMgr::GetFlexMesh()` | matched | fixed (was differs) |
| meshdx8 | `CMeshMgr::GetMaxVerticesToRender(IMaterial*)` | matched | fixed (was differs) |
| meshdx8 | `CMeshMgr::ReleaseBuffers()` | matched | fixed (was differs) |
| meshdx8 | `CMeshMgr::SetRenderState(int, int, unsigned long long, int)` | matched | fixed (was differs) |
| meshdx8 | `CMeshMgr::Shutdown()` | matched | fixed (was differs) |
| meshdx8 | `CTempMeshDX8::BeginPass()` | matched | fixed (was differs) |
| meshdx8 | `CTempMeshDX8::GetDynamicMesh()` | matched | fixed (was missing) |
| meshdx8 | `CTempMeshDX8::RenderPass()` | matched | fixed (was differs) |
| meshdx8 | `CVertexBufferDx8::CVertexBufferDx8(ShaderBufferType_t, unsigned long long, int, char const*)` | matched | fixed (was differs) |
| meshdx8 | `non-virtual thunk to CMeshDX8::Unlock(int, IndexDesc_t&)` | matched | fixed (was differs) |
| shaderapidx8 | `CShaderAPIDx8::ComputeGammaCorrectedFogColor(unsigned char, unsigned char, unsigned char, bool)` | differs | size retail 449 vs ours 422 |
| shaderapidx8 | `CShaderAPIDx8::CreateTextureHandles(int*, int)` | differs | size retail 289 vs ours 296 |
| shaderapidx8 | `CShaderAPIDx8::CreateTextures(int*, int, int, int, int, ImageFormat, int, int, int, char const*, char const*)` | differs | size retail 759 vs ours 715 |
| shaderapidx8 | `CShaderAPIDx8::GetDX9LightState(LightState_t*) const` | matched | fixed (was differs) |
| shaderapidx8 | `CShaderAPIDx8::GetPixelFogCombo1(bool)` | differs | size retail 86 vs ours 83 |
| shaderapidx8 | `CShaderAPIDx8::OnDeviceInit()` | differs | size retail 664 vs ours 434 |
| shaderapidx8 | `CShaderAPIDx8::UserClipTransform(VMatrix const&)` | differs | size retail 201 vs ours 196 |
| shaderapidx8 | `non-virtual thunk to CShaderAPIDx8::GetPixelFogCombo1(bool)` | differs | size retail 96 vs ours 93 |
| shaderapidx8 | `non-virtual thunk to CShaderAPIDx8::UserClipTransform(VMatrix const&)` | differs | size retail 211 vs ours 206 |
| shaderdevicebase | `CShaderDeviceMgrBase::GetRecommendedConfigurationInfo(int, int, int, int, KeyValues*)` | differs | size retail 2281 vs ours 2222 |
| shaderdevicebase | `CShaderDeviceMgrBase::LoadHardwareCaps(KeyValues*, HardwareCaps_t&)` | differs | size retail 462 vs ours 410 |
| shaderdevicebase | `CShaderDeviceMgrBase::ReadDXSupportLevels(HardwareCaps_t&)` | differs | size retail 240 vs ours 206 |
| shaderdevicedx8 | `CShaderDeviceDx8::GetDisplayDeviceName()` | matched | fixed (was differs) |
| shaderdevicedx8 | `CShaderDeviceMgrDx8::ComputeCapsFromD3D(HardwareCaps_t*, int)` | differs | size retail 4731 vs ours 4774 |
| shaderdevicedx8 | `CShaderDeviceMgrDx8::Connect(void* (*)(char const*, int*))` | matched | fixed (was differs) |
| texturedx8 | `CreateD3DTexture(int, int, int, ImageFormat, int, int, char*)` | matched | fixed (was differs) |
| texturedx8 | `DestroyD3DTexture(IDirect3DBaseTexture9*)` | matched | fixed (was differs) |
| vertexdecl | `ReleaseAllVertexDecl()` | differs | bytes |
| vertexshaderdx8 | `CShaderManager::SetPixelShader(int)` | matched | fixed (was differs) |
| vertexshaderdx8 | `CShaderManager::SetVertexShader(int)` | matched | fixed (was differs) |

## Lane residuals3 (1592/1592 with `--data-check`)

All under `BYTEMATCH_RETAIL` unless noted. The anniversary code asks for the render
backend at runtime: `ReadDXSupportLevels` caps the max dxlevel at 95 on Vulkan,
`LoadHardwareCaps` drops the ATI centroid hack on Vulkan, `SetupHardwareCaps` skips the
dxlevel fallbacks on Vulkan, `CreateTextures` marks textures lockable on ToGL (not
`IsPosix()`), `GetRecommendedConfigurationInfo` sets `ConVar.r_lightmap_bicubic` from the
dxlevel (if/else, two `SetInt` calls) and `GetRam` prints the 32-bit `dwTotalPhys`.
Retail never touches `m_bCanStretchRectFromTextures` (not in `ComputeCapsFromD3D` on GL,
not in `ForceCapsToDXLevel`) and clears `m_bSupportsNormalMapCompression` in
`ForceCapsToDXLevel` (dx60-dx92, next to the border-color clear) and after
`CheckBorderColorSupport`. New convar `r_fog_force_gamma_override` (`FCVAR_CHEAT`, after
`r_pixelfog`) overrides the fog color space. `Texture_t()` doesn't null the texture
pointer. `ReleaseAllVertexDecl` has no null check (SDK 2012). `GetPixelFogCombo1` tests
`bSupportsRadial` first. dynamicvb.h counter names use `V_strcpy`/`V_strcat`; dynamicib.h's
refcount gates now restore retail's numbering (retail never had those 13 lines; `Msg`
lines 862-868). ToGL headers: `D3DXMATRIX::operator!=` compares `sizeof(this)` bytes (as
linuxwin's dxabstract.cpp), inline in d3dx_impl.h; `ID3DXMatrixStack::Create` is
`FORCEINLINE` so `CreateMatrixStacks` inlines into `OnDeviceInit` with its loop kept.

## Identity lane round 6: byte-identical (`identical.json`)

- `shaderdevicedx8.o`'s except-table labels were 25/51/54 where retail has 26/52/55:
  retail emits one more function before the `CShaderDeviceDx8` constructor, dead in
  the dylib like `CheckBorderColorSupport`, the vendor checks and
  `ComputeDXSupportLevel`. Retail mode now has
  `CShaderDeviceMgrDx8::CheckNormalCompressionSupport`, the Orange Box-era
  normal-map (ATI2N) check, reduced to the `m_bSupportsNormalMapCompression = false`
  store that residuals3 found after `CheckBorderColorSupport`. It is inlined into
  `ComputeCapsFromD3D` with unchanged code and dead out of line. Any dead extern
  member in that range gives the same file; the name is a reconstruction.
- `dynamicib.h`: the win32-5 merge put the non-MSVC `CIndexBuffer::Lock` `Msg`
  lines back at 875-881 (its `#line 874`/`#line 882` predate residuals3's
  renumbering). Retail has 862/865/868: now `#line 861` and `#line 869`. Only
  Asserts follow, so the MSVC build's later lines don't matter.
