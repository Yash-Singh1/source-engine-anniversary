# stdshader_dx9 (retail macOS)

Retail: depot 223 `bin/stdshader_dx9.dylib`. Measured with `bytematch report` against
`build-retail/materialsystem/stdshaders/stdshader_dx9.dylib`; "matched" = exact or
matches-after-masking.

| state | functions | bytes |
|---|---:|---:|
| lane start (bytematch 64c10856) | 1925/1963 | 72.8% |
| this branch | 1956/1963 | 87.3% |
| this branch + `LightState_t` overlay (see below) | 1963/1963 exact | 100.0% |

With the overlay every section of the linked dylib (`__text`, `__cstring`, `__const`,
`__data`, `__eh_frame`, `__unwind_info`, stubs) is byte-identical to retail. Only the
link-edit string table (Valve's buildbot paths in the debug map) and the UUID differ.

## Combo headers

Retail compiles Valve's regenerated combo headers from `materialsystem/stdshaders/include/`
(N_SOL). Most are the bitfield format (`unsigned int m_nX : 2`, `int GetIndex() const`),
which changes codegen in every DrawElements that sets a non-bool combo. The public copy
is the TF2 branch of Source SDK 2013 (`origin/master` 0759e2e8, Feb 2025).
`devtools/bytematch/tools/stdshader_include.py` regenerates `include/` from it with the HL2
differences: TF2-only axes that retail never sets are dropped (`TREESWAY`,
`PHONG_USE_EXPONENT_FACTOR`, `STATIC_LIGHT_LIGHTMAP`, `DEBUG_LUXELS`), the vertexlit vertex
shaders keep the single `STATIC_LIGHT` axis, and three old-format headers fall back to
`fxctmp9/`. Evidence: the stringified `SET_*_COMBO` names in the retail dylib, and the
retail VCS combo counts (`devtools/anniversary/shader_layouts.json` on `anniversary-macos`;
all 283 layouts agree). Only `--retail-osx32` puts `include/` on the include path; the
native build keeps `fxctmp9/`/`vshtmp9/`, which match the fork's compiled shaders.

## Source changes

| object | status | note |
|---|---|---|
| all fog-using shaders | edited | `GetPixelFogCombo1( true )` (radial fog, TF2 branch) where the 2024 header has `PIXELFOGTYPE` 0..2; retail only |
| lightmappedgeneric_dx9_helper | edited, written new | SDK 2013 normal decode modes; `r_lightmap_bicubic` + `BICUBIC_LIGHTMAP` (retail only); anniversary `mat_ldr_workaround` and `mat_specular_scalar` (from disassembly) |
| refract_dx9_helper | edited, written new | SDK 2013 normal decode modes; `mat_specular_scalar` (retail writes the tint back unscaled) |
| vertexlitgeneric_dx9_helper | edited, written new | `PIXELFOGTYPE` combos; `mat_specular_scalar`; SDK double-precision `flResScale` (retail only) |
| water | edited | SDK 2013 normal decode modes; TF2 eye position constants (retail only) |
| eye_refract_helper | edited | TF2 `PIXELFOGTYPE` combos (retail only) |
| teeth | edited | TF2 spec exponent in `vEyePos_SpecExponent` |
| colorcorrection | edited | no `IsOpenGL()` assert block (TF2 branch) |
| flesh_interior_blended_pass_helper | edited | SDK constant counts (retail only; native keeps the fork's out-of-bounds fix) |
| pyro_vision | edited | uninitialized stripe-scale `Vector4D` (retail only) |
| commandbuilder.h | edited | leak/SDK direct store and `PutInt` (fork `memcpy`/`PutIntPtr` native only) |

## Remaining (needs `public/shaderapi/ishaderdynamic.h`)

Retail `LightState_t` is SDK 2013's: `m_nNumLights`, `m_bAmbientLight`, `m_bStaticLight`
(our `m_bStaticLightTexel` moves the vertex-light flag to offset 6). Seven functions read it:
`DrawVertexLitGeneric_DX9_Internal`, `Draw_Eyes_Refract_Internal`, `DrawEyes_DX8_DX9_Internal`,
`Teeth_DX9::DrawUsingVertexShader`, `DrawFleshInteriorBlendedPass`,
`TreeLeaf::OnDrawElements`, `pyro_vision::OnDrawElements`. Retail strings also have
`lightState.m_bStaticLight ? 1 : 0`. With the header change, the stdshaders sources need
`m_bStaticLightVertex` renamed to `m_bStaticLight` (7 files), and shaderapidx9/shaderapiempty
drop their `m_bStaticLightTexel` stores.

| object | functions | matched at start | matched now | with overlay | % bytes start | % bytes now |
|---|---:|---:|---:|---:|---:|---:|
| BaseShader | 46 | 45 | 46 | 46 | 96.9 | 100.0 |
| cable_dx9 | 26 | 25 | 26 | 26 | 63.7 | 100.0 |
| colorcorrection | 15 | 14 | 15 | 15 | 73.0 | 100.0 |
| convar | 47 | 44 | 47 | 47 | 90.2 | 100.0 |
| DecalBaseTimesLightmapAlphaBlendSelfIllum_dx9 | 28 | 27 | 28 | 28 | 50.9 | 100.0 |
| DecalModulate_dx9 | 26 | 25 | 26 | 26 | 30.8 | 100.0 |
| eye_refract_helper | 5 | 4 | 4 | 5 | 11.4 | 11.4 |
| eyes_dx8_dx9_helper | 6 | 5 | 5 | 6 | 15.4 | 15.4 |
| flesh_interior_blended_pass_helper | 3 | 2 | 2 | 3 | 20.6 | 20.6 |
| lightmappedgeneric_dx9_helper | 9 | 4 | 9 | 9 | 4.7 | 100.0 |
| pyro_vision | 17 | 16 | 16 | 17 | 46.3 | 46.3 |
| refract_dx9_helper | 3 | 1 | 3 | 3 | 3.9 | 100.0 |
| skin_dx9_helper | 7 | 6 | 7 | 7 | 14.1 | 100.0 |
| sprite_dx9 | 29 | 26 | 29 | 29 | 39.7 | 100.0 |
| teeth | 29 | 27 | 28 | 29 | 24.5 | 54.9 |
| TreeLeaf | 15 | 14 | 14 | 15 | 59.2 | 59.2 |
| vertexlitgeneric_dx9_helper | 10 | 5 | 9 | 10 | 4.1 | 14.1 |
| vr_distort_hud | 17 | 16 | 17 | 17 | 58.5 | 100.0 |
| vr_distort_texture | 15 | 14 | 15 | 15 | 55.1 | 100.0 |
| warp | 17 | 16 | 17 | 17 | 38.3 | 100.0 |
| water | 32 | 30 | 32 | 32 | 53.1 | 100.0 |
| windowimposter_dx90 | 27 | 26 | 27 | 27 | 69.2 | 100.0 |
| worldtwotextureblend | 16 | 15 | 16 | 16 | 41.3 | 100.0 |

All other objects were already fully matched and are unchanged.
