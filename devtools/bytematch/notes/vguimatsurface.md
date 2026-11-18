# vguimatsurface

Retail: depot 223 `bin/vguimatsurface.dylib`. Own objects: vgui_controls.o (the
surface's stub), Clip2D, Cursor, FontTextureCache, Input, MatSystemSurface,
TextureDictionary (341 functions); then libvgui_controls.a, tier1/2/3, bitmap,
mathlib (other lanes) and libvgui_surfacelib.a (see vgui_surfacelib.md).
Verification: `--retail-osx32` build, `bytematch report` against the dylib.

"now" is without the shared-header change this lane needs (USE_GETKERNEDCHARWIDTH
for OSX in public/vgui/VGUI.h, see the lane report); with it as a local overlay
MatSystemSurface reaches 240/240 (DrawPrintText is the difference).

The table keys functions by symbol name, so the three per-object `__GLOBAL__I_a` static initialisers (vgui_controls, MatSystemSurface, TextureDictionary) count once; by object, `run.sh` gives 340/341 now and 207/341 at lane start.

| object | functions | matched now | matched at lane start | % bytes now |
|---|---:|---:|---:|---:|
| Clip2D | 13 | 13 | 13 | 100.0 |
| Cursor | 14 | 14 | 14 | 100.0 |
| FontTextureCache | 34 | 34 | 17 | 100.0 |
| Input | 5 | 5 | 5 | 100.0 |
| MatSystemSurface | 239 | 238 | 151 | 97.8 |
| TextureDictionary | 33 | 33 | 5 | 100.0 |
| **total** | 338 | 337 | 205 | 98.6 |

Status column: matched = exact or matches-after-masking; "fixed" = differed or was missing at lane start.

| object | function | status | note |
|---|---|---|---|
| FontTextureCache | `CFontTextureCache::AllocatePageForChar(int, int, int&, int&, int&, int&, int&)` | matched | fixed (was differs) |
| FontTextureCache | `CFontTextureCache::CFontTextureCache()` | matched | fixed (was differs) |
| FontTextureCache | `CFontTextureCache::Clear()` | matched | fixed (was differs) |
| FontTextureCache | `CFontTextureCache::GetTextureForChar(unsigned long, vgui::FontDrawType_t, wchar_t, int*, float**, int*)` | matched | fixed (was missing) |
| FontTextureCache | `CFontTextureCache::GetTextureForChars(unsigned long, vgui::FontDrawType_t, wchar_t const*, int*, float**, int, int*)` | matched | fixed (was missing) |
| FontTextureCache | `CFontTextureCache::~CFontTextureCache()` | matched | fixed (was differs) |
| FontTextureCache | `CUtlMemory<UtlRBTreeNode_t<CFontTextureCache::CacheEntry_t, unsigned short>, unsigned short>::Grow(int)` | matched | fixed (was differs) |
| FontTextureCache | `CUtlRBTree<CFontTextureCache::CacheEntry_t, unsigned short, bool (*)(CFontTextureCache::CacheEntry_t const&, CFontTextureCache::CacheEntry_t const&), CUtlMemory<UtlRBTreeNode_t<CFontTextureCache::CacheEntry_t, unsigned short>, unsigned short>>::Find(CFontTextureCache::CacheEntry_t const&) const` | matched | fixed (was differs) |
| FontTextureCache | `CUtlRBTree<CFontTextureCache::CacheEntry_t, unsigned short, bool (*)(CFontTextureCache::CacheEntry_t const&, CFontTextureCache::CacheEntry_t const&), CUtlMemory<UtlRBTreeNode_t<CFontTextureCache::CacheEntry_t, unsigned short>, unsigned short>>::FindInsertionPosition(CFontTextureCache::CacheEntry_t const&, unsigned short&, bool&)` | matched | fixed (was differs) |
| FontTextureCache | `CUtlRBTree<CFontTextureCache::CacheEntry_t, unsigned short, bool (*)(CFontTextureCache::CacheEntry_t const&, CFontTextureCache::CacheEntry_t const&), CUtlMemory<UtlRBTreeNode_t<CFontTextureCache::CacheEntry_t, unsigned short>, unsigned short>>::Insert(CFontTextureCache::CacheEntry_t const&)` | matched | fixed (was differs) |
| FontTextureCache | `CUtlRBTree<CFontTextureCache::CacheEntry_t, unsigned short, bool (*)(CFontTextureCache::CacheEntry_t const&, CFontTextureCache::CacheEntry_t const&), CUtlMemory<UtlRBTreeNode_t<CFontTextureCache::CacheEntry_t, unsigned short>, unsigned short>>::InsertRebalance(unsigned short)` | matched | fixed (was differs) |
| FontTextureCache | `CUtlRBTree<CFontTextureCache::CacheEntry_t, unsigned short, bool (*)(CFontTextureCache::CacheEntry_t const&, CFontTextureCache::CacheEntry_t const&), CUtlMemory<UtlRBTreeNode_t<CFontTextureCache::CacheEntry_t, unsigned short>, unsigned short>>::IsLeftChild(unsigned short) const` | matched | fixed (was differs) |
| FontTextureCache | `CUtlRBTree<CFontTextureCache::CacheEntry_t, unsigned short, bool (*)(CFontTextureCache::CacheEntry_t const&, CFontTextureCache::CacheEntry_t const&), CUtlMemory<UtlRBTreeNode_t<CFontTextureCache::CacheEntry_t, unsigned short>, unsigned short>>::IsRightChild(unsigned short) const` | matched | fixed (was differs) |
| FontTextureCache | `CUtlRBTree<CFontTextureCache::CacheEntry_t, unsigned short, bool (*)(CFontTextureCache::CacheEntry_t const&, CFontTextureCache::CacheEntry_t const&), CUtlMemory<UtlRBTreeNode_t<CFontTextureCache::CacheEntry_t, unsigned short>, unsigned short>>::NewNode()` | matched | fixed (was differs) |
| FontTextureCache | `CUtlRBTree<CFontTextureCache::CacheEntry_t, unsigned short, bool (*)(CFontTextureCache::CacheEntry_t const&, CFontTextureCache::CacheEntry_t const&), CUtlMemory<UtlRBTreeNode_t<CFontTextureCache::CacheEntry_t, unsigned short>, unsigned short>>::RemoveAll()` | matched | fixed (was differs) |
| FontTextureCache | `CUtlRBTree<CFontTextureCache::CacheEntry_t, unsigned short, bool (*)(CFontTextureCache::CacheEntry_t const&, CFontTextureCache::CacheEntry_t const&), CUtlMemory<UtlRBTreeNode_t<CFontTextureCache::CacheEntry_t, unsigned short>, unsigned short>>::RotateLeft(unsigned short)` | matched | fixed (was differs) |
| FontTextureCache | `CUtlRBTree<CFontTextureCache::CacheEntry_t, unsigned short, bool (*)(CFontTextureCache::CacheEntry_t const&, CFontTextureCache::CacheEntry_t const&), CUtlMemory<UtlRBTreeNode_t<CFontTextureCache::CacheEntry_t, unsigned short>, unsigned short>>::RotateRight(unsigned short)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::AccessChromeHTMLController()` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::AddCustomFontFile(char const*, char const*)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::AttachToWindow(void*, bool)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::Begin3DPaint(int, int, int, int, bool)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::CMatSystemSurface()` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::CalculateMouseVisible()` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::DeleteTextureByID(int)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::DestroyTextureID(int)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::DrawFilledRectArray(vgui::IntRect*, int)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::DrawFilledRectFade(int, int, int, int, unsigned int, unsigned int, bool)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::DrawFlushText()` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::DrawGetTextureFile(int, char*, int)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::DrawGetTextureId(char const*)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::DrawGetTextureMatInfoFactory(int)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::DrawGetTextureMaterial(int)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::DrawGetTextureSize(int, int&, int&)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::DrawGetUnicodeCharRenderInfo(wchar_t, vgui::CharRenderInfo&)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::DrawOutlinedCircle(int, int, int, int)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::DrawPanelIn3DSpace(unsigned int, VMatrix const&, int, int, float, float)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::DrawPolyLine(int*, int*, int)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::DrawPrintText(wchar_t const*, int, vgui::FontDrawType_t)` | differs | size retail 1293 vs ours 1196 |
| MatSystemSurface | `CMatSystemSurface::DrawQuad(vgui::Vertex_t const&, vgui::Vertex_t const&, unsigned char*)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::DrawQuadArray(int, vgui::Vertex_t*, unsigned char*, bool)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::DrawRenderCharFromInfo(vgui::CharRenderInfo const&)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::DrawSetSubTextureRGBA(int, int, int, unsigned char const*, int, int)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::DrawSetTextFont(unsigned long)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::DrawSetTextScale(float, float)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::DrawSetTexture(int)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::DrawSetTextureFile(int, char const*, int, bool)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::DrawSetTextureFrame(int, int, unsigned int*)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::DrawSetTextureMaterial(int, IMaterial*)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::DrawSetTextureRGBA(int, unsigned char const*, int, int, int, bool)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::DrawSetTextureRGBAEx(int, unsigned char const*, int, int, ImageFormat)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::DrawSetTextureRGBAEx2(int, unsigned char const*, int, int, ImageFormat, bool, bool)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::DrawTexturedLine(vgui::Vertex_t const&, vgui::Vertex_t const&)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::DrawTexturedLineInternal(vgui::Vertex_t const&, vgui::Vertex_t const&)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::DrawTexturedPolygon(int, vgui::Vertex_t*, bool)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::DrawTexturedRect(int, int, int, int)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::DrawTexturedSubRect(int, int, int, int, float, float, float, float)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::DrawUnicodeString(wchar_t const*, vgui::FontDrawType_t)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::DrawUpdateRegionTextureRGBA(int, int, int, unsigned char const*, int, int, ImageFormat)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::End3DPaint()` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::ForceScreenPosOffset(bool, int, int)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::ForceScreenSizeOverride(bool, int, int)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::GetBitmapFontName(char const*)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::GetFullscreenViewportAndRenderTarget(int&, int&, int&, int&, ITexture**)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::GetScreenSize(int&, int&)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::GetTextureNumFrames(int)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::GetTitle(unsigned int)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::GetWorkspaceBounds(int&, int&, int&, int&)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::GetZPos() const` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::HandleInputEvent(InputEvent_t const&)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::Init()` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::InstallPlaySoundFunc(void (*)(char const*))` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::InternalSchemeSettingsTraverse(unsigned int, bool)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::InternalSolveTraverse(unsigned int)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::InternalThinkTraverse(unsigned int)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::IsCursorVisible()` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::IsInThink(unsigned int)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::IsScreenPosOverrideActive()` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::IsScreenSizeOverrideActive()` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::NeedKBInput()` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::OffsetAbsPos(int&, int&)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::OnScreenSizeChanged(int, int)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::PaintSoftwareCursor()` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::PaintTraverseEx(unsigned int, bool)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::PlaySound(char const*)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::PopMakeCurrent(unsigned int)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::PrecacheFontCharacters(unsigned long, wchar_t const*)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::ReferenceProceduralMaterial(int, int, IMaterial*)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::ReleasePanel(unsigned int)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::ResetFontCaches()` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::RunFrame()` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::SetAllowHTMLJavaScript(bool)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::SetBitmapFontName(char const*, char const*)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::SetCursor(unsigned long)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::SetCursorAlwaysVisible(bool)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::SetFullscreenViewportAndRenderTarget(int, int, int, int, ITexture*)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::SetPanelForInput(unsigned int)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::SetTitle(unsigned int, wchar_t const*)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::SetWorkspaceInsets(int, int, int, int)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::Shutdown()` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::StartDrawing()` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::StartDrawingIn3DSpace(VMatrix const&, int, int, float, float)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::SurfaceGetCursorPos(int&, int&)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::SurfaceSetCursorPos(int, int)` | matched | fixed (was differs) |
| MatSystemSurface | `CMatSystemSurface::~CMatSystemSurface()` | matched | fixed (was differs) |
| MatSystemSurface | `vgui_font_reload(CCommand const&)` | matched | fixed (was missing) |
| TextureDictionary | `CMatSystemTexture::CleanUpMaterial()` | matched | fixed (was differs) |
| TextureDictionary | `CMatSystemTexture::CreateRegen(int, int, ImageFormat)` | matched | fixed (was differs) |
| TextureDictionary | `CMatSystemTexture::ReferenceOtherProcedural(CMatSystemTexture*, IMaterial*)` | matched | fixed (was differs) |
| TextureDictionary | `CMatSystemTexture::SetMaterial(IMaterial*)` | matched | fixed (was differs) |
| TextureDictionary | `CMatSystemTexture::SetTextureRGBA(char const*, int, int, ImageFormat, bool, bool, bool)` | matched | fixed (was missing) |
| TextureDictionary | `CTextureDictionary::BindTextureToFile(int, char const*)` | matched | fixed (was differs) |
| TextureDictionary | `CTextureDictionary::BindTextureToMaterial(int, IMaterial*)` | matched | fixed (was differs) |
| TextureDictionary | `CTextureDictionary::BindTextureToMaterialReference(int, int, IMaterial*)` | matched | fixed (was differs) |
| TextureDictionary | `CTextureDictionary::CTextureDictionary()` | matched | fixed (was differs) |
| TextureDictionary | `CTextureDictionary::CreateTexture(bool)` | matched | fixed (was differs) |
| TextureDictionary | `CTextureDictionary::DestroyAllTextures()` | matched | fixed (was differs) |
| TextureDictionary | `CTextureDictionary::FindTextureIdForTextureFile(char const*)` | matched | fixed (was differs) |
| TextureDictionary | `CTextureDictionary::GetTextureMaterial(int)` | matched | fixed (was differs) |
| TextureDictionary | `CTextureDictionary::GetTextureSize(int, int&, int&)` | matched | fixed (was differs) |
| TextureDictionary | `CTextureDictionary::GetTextureTexCoords(int, float&, float&, float&, float&)` | matched | fixed (was differs) |
| TextureDictionary | `CTextureDictionary::IsValidId(int) const` | matched | fixed (was differs) |
| TextureDictionary | `CTextureDictionary::SetSubTextureRGBA(int, int, int, unsigned char const*, int, int)` | matched | fixed (was differs) |
| TextureDictionary | `CTextureDictionary::SetSubTextureRGBAEx(int, int, int, unsigned char const*, int, int, ImageFormat)` | matched | fixed (was differs) |
| TextureDictionary | `CTextureDictionary::SetTextureRGBA(int, char const*, int, int)` | matched | fixed (was differs) |
| TextureDictionary | `CTextureDictionary::SetTextureRGBAEx(int, char const*, int, int, ImageFormat, bool)` | matched | fixed (was differs) |
| TextureDictionary | `CTextureDictionary::SetTextureRGBAEx2(int, char const*, int, int, ImageFormat, bool, bool, bool)` | matched | fixed (was missing) |
| TextureDictionary | `CTextureDictionary::UpdateSubTextureRGBA(int, int, int, unsigned char const*, int, int, ImageFormat)` | matched | fixed (was differs) |
| TextureDictionary | `CTextureDictionary::~CTextureDictionary()` | matched | fixed (was differs) |
| TextureDictionary | `CUtlLinkedList<CMatSystemTexture, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<CMatSystemTexture, unsigned short>, unsigned short>>::AllocInternal(bool)` | matched | fixed (was differs) |
| TextureDictionary | `CUtlLinkedList<CMatSystemTexture, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<CMatSystemTexture, unsigned short>, unsigned short>>::Free(unsigned short)` | matched | fixed (was differs) |
| TextureDictionary | `CUtlLinkedList<CMatSystemTexture, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<CMatSystemTexture, unsigned short>, unsigned short>>::LinkBefore(unsigned short, unsigned short)` | matched | fixed (was differs) |
| TextureDictionary | `CUtlLinkedList<CMatSystemTexture, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<CMatSystemTexture, unsigned short>, unsigned short>>::~CUtlLinkedList()` | matched | fixed (was missing) |
| TextureDictionary | `CUtlMemory<UtlLinkedListElem_t<CMatSystemTexture, unsigned short>, unsigned short>::Grow(int)` | matched | fixed (was differs) |
