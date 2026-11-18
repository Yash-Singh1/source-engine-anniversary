# togl (retail macOS libtogl)

Retail `bin/libtogl.dylib` was built from `togl/osx/*` against `public/togl/osx/*`
(debug map), the ToGL generation Source SDK 2013 ships headers for. Retail mode
(`TOGL_OSX`, defined by `--retail-osx32`) selects those headers in `rendermechanism.h`,
`glfuncs.inl` and `ilaunchermgr.h`; the default build stays on `linuxwin`.

Layouts checked against retail: `GLMContext` 0x2F90, `CGLMTex` 0x84,
`GLMRendererInfo::m_display` at 248, `COpenGLEntryPoints` offsets, `ILauncherMgr`
slots (+80/+100/+108/+112). `public/togl/dx_iunknown.h` is retail's `IUnknown`
(no `m_mark`, typeinfo not exported).

| file | base | matched |
|---|---|---:|
| cglmbuffer, cglmfbo, cglmquery, cglmprogram | written from the retail disassembly, linuxwin as reference | 7/7, 4/4, 11/11, 16/18 |
| glmgrbasics, glentrypoints, glmgrcocoa.mm | ToGL 2014 linuxwin, cut back to what retail has | 12/12, 6/6, 1/1 |
| glmgr, cglmtex, dxabstract, dx9asmtogl2 | Source SDK 2012 `materialsystem/glmgr` and `shaderapidx9` (the direct ancestors), moved to the 2013 SDL/gGL model, then fixed function by function against retail | 81/81, 28/29, 158/158\*, 35/35\* |

\* with `DXABSTRACT_VS_PARAM_SLOTS` 219 (see below); with the tree's 228,
`IDirect3D9::GetDeviceCaps`, `D3DToGL::Handle_UnaryOp`, `PrintParameterToString` and
`TranslateShader` differ only in that constant. Whole dylib: 495/502 (499/502 with 219).

Retail is older than the 2012 SDK code in several places (the Mac SP branch is from
2010): `dxabstract` follows the Steamworks SDK `glmgr` copy (`FlushSamplers`,
`EnumAdapterModes`/`GetAdapterModeCount` without the 800x500 mode, centroid and
shadow-sampler name tables, `MaxAnisotropy` 8), `GetSurfaceLevel` still AddRefs the
surface ("trying to turn this off - 26Oct2010" in later code), `SetRenderTarget` uses the
public refcount and `SetDepthStencilSurface` does no refcounting. Elsewhere it has later
pieces: ToGL's `m_bOutputColorRegister[4]`, `targetname = "?"`, the previous-TMU-0 restore
in `CGLMTex` (ctor, `ReadTexels`, `WriteTexels`), `m_nCurOwnerThreadId` set in the
`GLMContext` ctor, no `MakeCurrent()` in the `New*`/`Del*` helpers (except `DelProgram`)
or `BeginFrame`, and no `DebuggerBreakIfDebugging()` anywhere in `dx9asmtogl2`.
Retail-only details: `m_texClientStorage` needs `m_osComboVersion >= 0x000A0603`
(10.6.3), `GL_APPLE_texture_range`/`GL_APPLE_client_storage` guards, `gl_enabletexsubimage`
default 0, the four resolve convars constructed with a null default.

Known differences:

- `DXABSTRACT_VS_PARAM_SLOTS` lives in `public/materialsystem/IShader.h` (shared header):
  retail and SDK 2013 use 219, the 2018 leak and our tree 228. Only togl uses it.
- cglmprogram `SelectShaderPair`, `LogSlow` and cglmtex `WriteTexels`: register allocation,
  stack slots and tail merging only; the logic matches retail.
- libtogl exports match retail except tier1's `cvar`, which retail exports.

shaderapidx9 links since the matsys lane: retail keeps the D3DX helpers out of libtogl and
inlines them from `public/togl/d3dx_impl.h` (named by shaderapidx9's debug map), so the osx
`dxabstract.h` no longer declares them. `imageformat.h` gives `D3DFORMAT` its D3D9 values
under `TOGL_OSX`, as retail's format name table shows; `glmgrbasics.cpp` still lists the
old names `D3DFMT_R5G6R5`/`D3DFMT_A` (retail: `D3DFMT_R5G6B5`/`D3DFMT_A8`), kept as aliases.
