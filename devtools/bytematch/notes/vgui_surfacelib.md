# vgui_surfacelib

Retail: `libvgui_surfacelib.a` as linked into depot 223 `bin/vguimatsurface.dylib`
(BitmapFont, FontAmalgam, FontEffects, FontManager, osxfont; 92 functions).

osxfont.cpp is rewritten from the retail disassembly: retail's COSXFont is an
older ATSU implementation than the 2018 leak's (per-glyph text layouts created
with ATSUCreateTextLayoutWithTextPtr, two styles with and without underline,
rounded ABC widths, a space-width tweak). Neither SDK 2013 nor SDK 2012 has it
(SDK 2012's osxfont.cpp is the later CoreText version). font_t is the
anniversary IVGuiFontImpl interface; COSXFont does not implement it, so retail
OSX only ever creates bitmap fonts (CreateOrFindWin32Font finds or reserves a
slot and returns NULL).

The remaining function, CDatadescGeneratedNameHolder::~CDatadescGeneratedNameHolder,
needs `delete` instead of `delete[]` in public/datamap.h (headers lane).

| object | functions | matched now | matched at lane start | % bytes now |
|---|---:|---:|---:|---:|
| BitmapFont | 15 | 14 | 6 | 93.7 |
| FontAmalgam | 15 | 15 | 6 | 100.0 |
| FontEffects | 5 | 5 | 4 | 100.0 |
| FontManager | 30 | 30 | 15 | 100.0 |
| osxfont | 26 | 26 | 21 | 100.0 |
| **total** | 91 | 90 | 52 | 99.3 |

Status column: matched = exact or matches-after-masking; "fixed" = differed or was missing at lane start.

| object | function | status | note |
|---|---|---|---|
| BitmapFont | `CBitmapFont::CBitmapFont()` | matched | fixed (was differs) |
| BitmapFont | `CBitmapFont::Create_Bitmap(char const*, float, float, int)` | matched | fixed (was missing) |
| BitmapFont | `CBitmapFont::GetCharABCWidths(int, int&, int&, int&, int*)` | matched | fixed (was missing) |
| BitmapFont | `CBitmapFont::GetCharCoords(int, float*, float*, float*, float*)` | matched | fixed (was differs) |
| BitmapFont | `CBitmapFont::GetKernedCharWidth(wchar_t, wchar_t, wchar_t, float&, float&, float&)` | matched | fixed (was differs) |
| BitmapFont | `CBitmapFont::GetTexturePage()` | matched | fixed (was differs) |
| BitmapFont | `CBitmapFont::IsEqualTo_Bitmap(char const*, float, float, int)` | matched | fixed (was missing) |
| BitmapFont | `CBitmapFont::SetScale(float, float)` | matched | fixed (was differs) |
| BitmapFont | `CDatadescGeneratedNameHolder::~CDatadescGeneratedNameHolder()` | differs | reloc target at +0x2e: ours _ZdaPv, retail _ZdlPv |
| FontAmalgam | `CFontAmalgam::AddFont(IVGuiFontImpl*, int, int)` | matched | fixed (was missing) |
| FontAmalgam | `CFontAmalgam::GetFlags(int)` | matched | fixed (was differs) |
| FontAmalgam | `CFontAmalgam::GetFontFamilyName(int)` | matched | fixed (was differs) |
| FontAmalgam | `CFontAmalgam::GetFontForChar(int)` | matched | fixed (was differs) |
| FontAmalgam | `CFontAmalgam::GetFontHeight()` | matched | fixed (was differs) |
| FontAmalgam | `CFontAmalgam::GetFontHeightRequested()` | matched | fixed (was differs) |
| FontAmalgam | `CFontAmalgam::GetFontName(int)` | matched | fixed (was differs) |
| FontAmalgam | `CFontAmalgam::GetUnderlined()` | matched | fixed (was differs) |
| FontAmalgam | `CFontAmalgam::SetFontScale(float, float)` | matched | fixed (was differs) |
| FontEffects | `ApplyGaussianBlurToTexture(int, int, unsigned char*, int, bool)` | matched | fixed (was missing) |
| FontManager | `CFontManager::CFontManager()` | matched | fixed (was differs) |
| FontManager | `CFontManager::ClearAllFonts()` | matched | fixed (was differs) |
| FontManager | `CFontManager::CreateOrFindBitmapFont(char const*, float, float, int)` | matched | fixed (was differs) |
| FontManager | `CFontManager::CreateOrFindWin32Font(char const*, int, int, int, int, int)` | matched | fixed (was differs) |
| FontManager | `CFontManager::GetCharABCwide(unsigned long, int, int&, int&, int&)` | matched | fixed (was differs) |
| FontManager | `CFontManager::GetCharacterWidth(unsigned long, int)` | matched | fixed (was differs) |
| FontManager | `CFontManager::GetFontAscent(unsigned long, wchar_t)` | matched | fixed (was differs) |
| FontManager | `CFontManager::GetKernedCharWidth(unsigned long, wchar_t, wchar_t, wchar_t, float&, float&, float&)` | matched | fixed (was differs) |
| FontManager | `CFontManager::GetTextSize(unsigned long, wchar_t const*, int&, int&)` | matched | fixed (was differs) |
| FontManager | `CFontManager::InitFreetype()` | matched | fixed (was missing) |
| FontManager | `CFontManager::SetBitmapFontGlyphSet(unsigned long, char const*, float, float, int)` | matched | fixed (was differs) |
| FontManager | `CFontManager::SetFontGlyphSet(unsigned long, char const*, int, int, int, int, int, int, int)` | matched | fixed (was differs) |
| FontManager | `CFontManager::SetLanguage(char const*)` | matched | fixed (was differs) |
| FontManager | `CFontManager::~CFontManager()` | matched | fixed (was differs) |
| FontManager | `CUtlMemory<IVGuiFontImpl*, int>::Grow(int)` | matched | fixed (was missing) |
| osxfont | `COSXFont::COSXFont()` | matched | fixed (was differs) |
| osxfont | `COSXFont::Create(char const*, int, int, int, int, int)` | matched | fixed (was differs) |
| osxfont | `COSXFont::GetCharABCWidths(int, int&, int&, int&, int*)` | matched | fixed (was missing) |
| osxfont | `COSXFont::GetCharRGBA(wchar_t, int, int, unsigned char*)` | matched | fixed (was differs) |
| osxfont | `COSXFont::~COSXFont()` | matched | fixed (was differs) |

## Whole-file identity (vguimatsurface)

- `osxfont.o`: retail's `COSXFont::GetFlags` atom is 0x10 bytes ending in a
  5-byte nop, so a dead `__text` function followed it. The 2018 leak defines
  `SetAsActiveFont` right after the two less-funcs (which clang creates
  earlier, at the constructor's reference), and it is back in `osxfont.h`
  and `osxfont.cpp`.
- `FontManager.o` is what's left in `identity.py vguimatsurface` (6 string-table
  bytes). Except-table labels 3/6/17 are 4/7/18 in retail, so retail creates
  one more function before the `CFontManager` base constructor. It isn't in
  `__text` between `FontManager()` and the constructor: `FontManager()`'s atom
  ends at the constructor. It is a dead coalesced function created early (the
  first weak atom leaves no trace) or a coalesced copy that an earlier object's
  copy replaced. Its name is not recovered.
