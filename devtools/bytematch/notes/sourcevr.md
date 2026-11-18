# sourcevr

Retail: depot 223 `bin/sourcevr.dylib`, LC_VERSION_MIN 10.5, links
`libsteam_api.dylib` and `libgcc_s.1.dylib`.

| state | matched / in scope |
|---|---:|
| lane start | 130 / 141 |
| this lane | 138 / 141 (`sourcevirtualreality.o` 100%) |

Retail is the SteamVR build (`steam/steamvr.h`, `vr::IHmd` / `IHmd_005`, `VR_Init`
from steam_api), not the leak's OpenVR port. The source is the same apart from the
API calls: `ZeroTracker` (zero pose), `GetTrackerFromHeadPose( 0.f, ... )`
(`SampleTrackingState`), `GetHeadFromEyePose`, `IHmd::WillDriftInYaw`, and a
trailing `virtual bool IsHmdAvailable()` after the interface methods
(`m_pHmd || VR_IsHmdPresent()`). `public/steam/steamvr.h` is SDK 2013's file.

Retail mode builds the module at `-mmacosx-version-min=10.5`, which gives retail's
load commands. The three remaining functions are statically linked library code:

- `libtier1.a(utlbuffer.o)`: `CUtlCStringConversion` ctor and `_GLOBAL__I_a`. Retail
  built sourcevr's tier1 for 10.5 too, so zero `memset` stays `memset` (10.6+
  lowers it to `__bzero`). Needs a 10.5 build of the tier1 members sourcevr pulls.
- `libmathlib.a(mathlib_base.o)`: `SinCos` is the leak's POSIX `fsincos` asm in this
  10.5 build, but `sin`/`cos` calls in the 10.6 modules (studiorender, replay), so
  retail's `public/mathlib/mathlib.h` picks the asm only below 10.6.

Verified: `mathlib_base.cpp` and `utlbuffer.cpp` compiled with the retail command
line at 10.5 (plus that `SinCos` condition) match all of sourcevr's retail
`utlbuffer.o` functions and `SinCos`, i.e. 141/141.
