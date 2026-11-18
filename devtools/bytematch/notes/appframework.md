# appframework (launcher's libappframework.a members)

Retail: `libappframework.a(AppSystemGroup.o, glmrendererinfo_osx.o,
posixapp.o, sdlmgr.o)` inside depot 223 `bin/launcher.dylib` (116
functions). AppSystemGroup is also linked into engine, sdlmgr into
shaderapidx9; glmrendererinfo_osx and posixapp only into launcher.
Verification as in launcher.md (appframework VPC defines: `USE_SDL`,
`DEV_BUILD`, GL defines, no `DLLNAME`).

| object | functions | before, today's headers | after, today's headers | after, with layer |
|---|---:|---:|---:|---:|
| AppSystemGroup | 32 | 15 | 15 | 32 |
| posixapp | 9 | 9 | 9 | 9 |
| sdlmgr | 56 | 32 | 37 | 45 |
| glmrendererinfo_osx | 19 | does not compile | does not compile | 10 |

glmrendererinfo_osx.mm only compiles against the overlay headers in the
measurement setup (pilot override order); the waf build is unaffected.

| function | status | note |
|---|---|---|
| CSDLMgr::handleKeyInput | edited | retail has neither the leak's `-exclusivefs` command-tab minimize nor the `-nonqwerty` key-name remap |
| CSDLMgr::OnFrameRendered, __GLOBAL__I_a | edited | no `osx_rawinput_set_one_time` on Mac in retail; the fork's Android `rawinput_set_one_time` stays under `ANDROID` |
| CSDLMgr::SetWindowFullScreen | edited | leak mouse scaling per mode branch under `VPC`; fork drawable-size (camera notch) scaling under `!VPC`. Matches with layer (`ThreadSleep` exported from tier0, now on bytematch) |
| CSDLMgr::CSDLMgr, PumpWindowsMessageLoop | edited | Android vsync-skip members and their uses under `ANDROID`; PumpWindowsMessageLoop needs SDK 2013 `V_UTF8ToUnicode` |
| CSDLMgr::Init | edited, differs | 640x480 hidden window under `VPC`. Retail adds an inlined renderer choice (`IsSteamDeck()`, `-opengl`, `-vulkan`, `-noshaderapi`) that needs tier0's anniversary `IsSteamDeck` |
| CSDLMgr::CreateHiddenGameWindow | differs | same renderer choice; static `glGetString`/`glGetIntegerv` lookups with a "Could not find required OpenGL entry point" error; GL compatibility-profile warning; `-gl_debug`; `GL_RENDERER`/`GL_VENDOR`/`GL_VERSION`/`GL_EXTENSIONS` dump; "This game does not support this type of graphics processor". Not written: depends on togl's `gGL` table layout |
| CSDLMgr::ShowPixels, SizeWindow, DecWindowRefCount, Shutdown | differs | `gGL` function-table offsets differ (retail togl/osx headers); retail Shutdown no longer deletes `m_readFBO`; SizeWindow calls an extra interface method that returns 2 |
| CSDLMgr::GetDisplayDB, GetDesiredPixelFormatAttribsAndRendererInfo | differs | `GLMRendererInfoFields` is 16 bytes smaller in retail |
| GLMDisplayInfo::PopulateModes/Dump/GLMDisplayInfo(), _DisplayModeSortFunction, CUtlMemory<GLMDisplayMode*>::Grow | retail-only / differs | retail compiles the SDL display-mode code (glmdisplaydb_linuxwin.inl) on Mac too |
| GLMDisplayDB::GetDisplayCount/GetDisplayInfo/GetModeCount/GetModeInfo/GetRendererInfo/Dump/PopulateRenderers, GLMRendererInfo::GLMRendererInfo, GLMDetectScaledResolveMode | differs | retail `GLMRendererInfo` holds one `GLMDisplayInfo *m_display` (offset 248) instead of a display vector; needs public/togl changes |
| CheesyRendererDecode, _DisplayInfoSortFunction, GLMDisplayInfo::PopulateModes (Mac variant) | ours-only | belong to the Mac multi-display variant retail doesn't use |
| AppSystemGroup.o, posixapp.o | identical | AppSystemGroup needs the tier1 layer (CUtlString/CUtlRBTree) |

## filesystem_init (public/filesystem_init.cpp)

Retail engine compiles its own `filesystem_init.o` (engine.build, `ENGINE_DLL`),
as do launcher and vtex_dll. After the leftovers lane: engine 8/8, vtex_dll
25/25, launcher 20/20. The `libappframework.a (vs engine)` report row pairs
appframework's non-engine copy with engine's object, so it keeps showing
`LoadExpansionInfo` missing and `FileSystem_LoadSearchPaths` different.

| function | status | note |
|---|---|---|
| FileSystem_LoadSearchPaths | written new (retail mode) | anniversary form: `-game`/`-defaultgamedir` (default `hl2`), `game_hd.txt` probe (inlined static helper), then `SetUseLowViolancePaths`, `SetUseHDMount`, `SetAudioLanguageOverride`, `AddGameAndModSearchPathsFromGameInfo` on the filesystem; `hl2_complete` picks the expansion from `LoadExpansionInfo` (engine only) or `hl2`; adds `exp` and `root_mod` paths; marks by-request IDs; `PrintSearchPaths`; `UpdateSearchPathsForCurrentGameExpansion`. The leak/fork body stays under `#ifndef BYTEMATCH_RETAIL` |
| LoadExpansionInfo | written new (`ENGINE_DLL`) | reads `<mod>/cfg/config.cfg`, finds `hl2_expansion`, takes the second `COM_Parse` token into the output (passing `com_token` as the format string, as retail does). Leaks the read buffer and, on an empty file, the FILE, as retail does |
| CFSSearchPathsInit::CFSSearchPathsInit | edited | retail struct has one bool (`m_bLowViolence`) at +0xc and `m_ModPath` at +0xd. `m_bMountHDContent` is a static dummy in retail mode so engine/common.cpp still compiles; the engine lane should drop that assignment |
| FileSystem_Init_Normal (utils/common/filesystem_tools.cpp) | edited | calls `FileSystem_SetupSteamInstallPath()` after `FileSystem_GetFileSystemDLLName` (SDK 2013), which also brings that function back into vtex_dll |
