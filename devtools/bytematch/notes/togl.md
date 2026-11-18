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
| cglmbuffer, cglmfbo, cglmquery, cglmprogram | written from the retail disassembly, linuxwin as reference | 6/7, 4/4, 11/11, 16/18 |
| glmgrbasics, glentrypoints, glmgrcocoa.mm | ToGL 2014 linuxwin, cut back to what retail has | 12/12, 6/6, 1/1 |
| glmgr, cglmtex, dxabstract, dx9asmtogl2 | Source SDK 2012 `materialsystem/glmgr` and `shaderapidx9` (the direct ancestors), moved to the 2013 SDL/gGL model | 69/81, 23/29, 140/158, 24/35 |

Known differences:

- cglmtex: the remaining functions are the 2012 code (retail adds e.g. the 10.6.3 client-storage check).
- dx9asmtogl2: retail's translator sits between the 2012 and 2014 code (it has `m_dwTempBoolUsageMask`); the handlers need a function-by-function backport.
- cglmprogram `SelectShaderPair`, `LogSlow`: stack layout and register allocation only.
- libtogl exports match retail except tier1's `cvar`, which retail exports.

shaderapidx9 compiles against these headers but does not link: our source calls
`D3DXCreateMatrixStack`, `ID3DXMatrixStack`, `D3DXMatrixTranspose`,
`D3DXPlaneTransform`, `D3DXCompileShader` and other D3DX helpers, which retail's
libtogl does not export (retail shaderapidx9 keeps a local `D3DXMatrixTranspose`).
