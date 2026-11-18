# vgui2

Retail: depot 223 `bin/vgui2.dylib`, own objects (499 functions) in this
order: filesystem_init, UnicodeFileHelpers, Bitmap, Border, ImageBorder,
InputWin32, keyrepeat, LocalizedStringTable, ScalableImageBorder, Scheme,
system_posix, vgui, vgui_internal, vgui_key_translation, VPanel,
VPanelWrapper; then tier1 members, tier2.o and tier3.o (other lanes).
Verification as in launcher.md with `DLLNAME=vgui2` and
`DONT_PROTECT_FILEIO_FUNCTIONS`.

Our vgui2/src is the 2018 leak's (the fork changed only system_posix and
vgui.cpp includes); retail is the SP branch plus anniversary additions.

| object | functions | before, today's headers | after, today's headers | after, with layer |
|---|---:|---:|---:|---:|
| Bitmap | 16 | 16 | 16 | 16 |
| Border | 16 | 13 | 16 | 16 |
| ImageBorder | 15 | 14 | 14 | 15 |
| InputWin32 | 102 | 40 | 40 | 102 |
| keyrepeat | 4 | 2 | 2 | 4 |
| LocalizedStringTable | 51 | 14 | 14 | 51 |
| ScalableImageBorder | 14 | 13 | 14 | 14 |
| Scheme | 61 | 21 | 40 | 61 |
| system_posix | 43 | 37 | 39 | 43 |
| VPanel | 50 | 15 | 50 | 50 |
| VPanelWrapper | 66 | 66 | 66 | 66 |
| vgui | 54 | 17 | 18 | 54 |
| filesystem_init, UnicodeFileHelpers, vgui_internal, vgui_key_translation | 7 | 6 | 6 | 7 |
| total | 499 | 274 | 335 | 499 |

Counts are bytematch (exact or matches-after-masking); "before" is the
bytematch branch tip f29128cc.

| function | status | note |
|---|---|---|
| VPanel (layout), VPanel::Solve, Init, ~VPanel, ctor | edited | int position/size/inset/clip/z (124-byte object), no ±32767 clamps, no pinned-sibling re-solve; CSGO-era `m_nThinkTraverseLevel`, `_clientPanelHandle` removed |
| VPanel::TraverseLevel | removed | CSGO-era, used only inside VPanel.cpp, not in the retail vtable |
| all other VPanel functions | identical | byte once the layout is fixed |
| CVGui::RunFrame, RemoveTickSignal, CreateNewTick, Tick_t | edited | no deferred tick removal (`bMarkDeleted`, `m_bCanRemoveTickSignal`); retail Tick_t is 76 bytes |
| other CVGui functions, __GLOBAL__I_a | identical | need SDK 2013 utlqueue.h (CUtlQueue on CUtlVector, 24 bytes) for the CUtlHandleTable layout |
| CInputSystem (all 102) | identical | need retail ButtonCode_t: Novint codes kept, Steam Controller 30 buttons and 16 axis codes per controller (BUTTON_CODE_COUNT 540), and IsJoystickCode covering joystick, Steam Controller and Novint codes |
| GetIndexForCode (keyrepeat), CKeyRepeatHandler::KeyUp/KeyDown | edited | Steam Controller d-pad and pad directions mapped to repeat aliases |
| CScheme fonts: GetFont, GetFontName, GetFontCount, GetFontAtIndex, SetFontRange, GetFontRange, SpewFonts, FindFontInAliasList | edited | aliases and ranges are vectors searched by name (`fontalias_t::_fontName`, `fontrange_t::_fontName`) |
| CScheme::LoadFonts | edited | creates aliases only once; `"isproportional" "only"` skips the non-proportional font |
| CScheme::ReloadFontGlyphs | edited | skips the `isproportional` key (static symbol); rescaled proportional fonts get FONTFLAG_ANTIALIAS; tall clamped to 256; weight/name read once |
| CScheme::LoadBorders | edited | reapplies settings to borders that already exist |
| CScheme::GetBorder, GetBorder2 | edited / written new | GetBorder2 returns NULL for missing borders; GetBorder falls back to the base border. Slot 12 of the CScheme vtable |
| CSchemeManager::UpdateBorders | written new | reloads borders of every scheme; slot 17 |
| CSchemeManager::ReloadSchemes, LoadSchemeFromFileEx, ReloadFonts, CSchemeManager() | edited | reload in place via a reload flag and index; cached scheme reused only for the same sizing panel; resolution key always processed; ReloadFonts calls LoadFonts |
| CSchemeManager::GetImage | edited | no `vgui/` prefix when the name already contains `vgui/` |
| Border::Paint (both), Paint2 | edited / written new | Paint(VPANEL) scales line thickness by `GetProportionalScaledValueEx(scheme, 1)`; slot 12 |
| IBorder::Paint2 | retail-only | inline default in the interface (forwards to the 7-argument Paint); needs IBorder.h |
| ScalableImageBorder::Paint | edited | SP-era `MAX( 1.0 - 2 * percent, 0 )` and integer `MAX` for the middle sizes |
| CLocalizedStringTable::AddFile | edited | always prepends search paths; no zeroing of the search-path buffer; needs SDK 2013 `V_UCS2ToUTF8` |
| CLocalizedStringTable (rest) | identical | most need the tier1 layer (CUtlRBTree/CUtlString/CUtlSymbol) |
| CSystem::ShellExecute, ShellExecuteEx | edited | OSX: `V_snprintf("open %s")` + `system()`; Linux keeps fork/execlp |
| CSystem::SaveUserConfigFile | identical | needs SDK 2013 `KeyValues::SaveToFile` (no `bCacheResult`) |
| fileimage, filesystem_helpers, MemoryBitmap, MessageListener | not in retail | our wscript compiles them; retail links no object for them |

Link composition: retail links no `memoverride.o` and none of the four
files above; retail frameworks Carbon (our wscript links
ApplicationServices); source order differs (manifest lane).
