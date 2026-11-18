# bitmap (libbitmap.a)

Retail: `libbitmap.a` members in depot 223 `engine.dylib` (102 functions),
`materialsystem.dylib` (99) and `shaderapidx9.dylib` (4). All match after
this lane (before: 83/102 vs engine).

| function | status | note |
|---|---|---|
| ConvertFromDXT1/DXT5/DXT5IgnoreAlpha (14 instantiations) | edited | `GetColorBlockColorsBGRA8888` expands 565 with the old shifts (`r << 3`, `g << 2`, `b << 3`), not the leak's `round()` float math. With the precomputed colours small enough, clang unrolls `DecodeColorBlock` the way retail does |
| ConvertImageFormat_RGB323232F_To_RGBA8888/BGRA8888 | edited | `PixRGBAF_to_8` (float_bm.h) clamps in double (`255.0`) |
| ImageLoader::ConvertImageFormat | edited | no `DXT*_RUNTIME` fast-copy or DXT-target cases; `ConvertToDXT` is the legacy path only (the stb compressor `CompressSTB`/`ConvertToDXTRuntime` isn't in retail) |
| ImageLoader::GetMemRequired | edited | tests DXT1/DXT3/DXT5/ATI1N/ATI2N directly instead of `IsCompressed()`; no runtime formats in the block switch. `IsCompressed` itself stays the table lookup (FlipImage*/RotateImageLeft need it) |
| ImageLoader::ImageFormatToD3DFormat | edited | no fork `RGB888`/`RGBA8888` cases, no runtime cases (shaderapidx9 copy) |
| FloatBitMap_t::CompressTo8Bits | edited | `IScaleValue` is `min(255, ceil(f*255.0))`, clamped in double |
| ImageFormat enum, g_ImageFormatInfo | edited | retail has no `IMAGE_FORMAT_DXT1_RUNTIME`/`DXT5_RUNTIME`: `NUM_IMAGE_FORMATS` is 39 and the table has 40 entries (retail symbol size 0x460). In retail mode the two enumerators sit after `NUM_IMAGE_FORMATS` so fork code in materialsystem/shaderapidx9 still compiles |
