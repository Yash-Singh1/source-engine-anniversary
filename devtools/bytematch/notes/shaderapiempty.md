# shaderapiempty

Retail: depot 223 `bin/shaderapiempty.dylib` (one own object).

| state | matched / in scope |
|---|---:|
| lane start | 585 / 588 |
| this lane | 587 / 588 |
| + SDK 2013 `LightState_t` | 588 / 588 |

- `CShaderAPIEmpty::SupportsNormalMapCompression` (and its `IHardwareConfigInternal`
  thunk): retail overrides it, returning false. SDK 2013 declares it pure in both
  `IShaderAPI` and `IMaterialSystemHardwareConfig`; our headers give inline
  defaults, so the override is added in the class (on a blank line, no shift).
- `CShaderAPIEmpty::GetDX9LightState`: retail writes 6 bytes, the SDK 2013
  `LightState_t { int m_nNumLights; bool m_bAmbientLight; bool m_bStaticLight; }`.
  Ours has the fork's `m_bStaticLightVertex`/`m_bStaticLightTexel` pair
  (`public/shaderapi/ishaderdynamic.h`). With the SDK struct the body becomes
  `m_bStaticLight = false;` and the function matches.
