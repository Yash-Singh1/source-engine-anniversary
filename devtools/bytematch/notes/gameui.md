# GameUI (bin/GameUI) per-function status

Measured with `--retail-osx32` (clang-600.0.57) and `devtools/bytematch` against depot 223
build 19307283, own objects only (`gameui.build/*.o`). Lane `bytematch-gameui`.

Own objects: 1582 / 1825 functions matched (exact or after masking), up from 1367 / 1809.
The denominator grew by retail's `OptionsSubGamepad.o` (16 functions), which this lane wrote.

## Edited or written to the retail form (all now match unless listed below)

| object | change |
|---|---|
| BonusMapsDatabase | 24 methods virtual (retail vtable order); `V_strcpy_safe` compiles as `Q_strcpy` in retail mode. 43/43 |
| GameUI_Interface | `IGameUI`/`CGameUI::GetBonusMapsDatabase()`; `Shutdown` without mutex release or `steamapicontext->Clear()` |
| language | Latam Spanish, Vietnamese, Indonesian table entries and `ELanguage` values |
| BaseSaveGameDialog | `SaveGameDescription_t::pAppendix`, `GetSaveAppendix`, override-mod save directory (`SetOverrideMod`, `BASE_PATH`), destructor, base-code keys with Steam Controller codes, `SizeToContents` on the empty-list label, `CSaveGamePanel` path ID and proportional image panels |
| SaveGameDialog | root-parented overwrite `QueryBox`. 11/11 |
| LoadGameDialog | Workshop item member; override-mod `launchmod` load path (Workshop branch not written) |
| SaveGameBrowserDialog, *_Xbox | layout via `SaveGameDescription_t`; `OnKeyCodePressed` on `GetBaseButtonCode` without Steam Controller cases |
| TGAImagePanel | `SetTGA( file, pathID )`. 11/11 |
| BasePanel | `CBasePanel` retail layout (logo `DHANDLE`, title fonts, switch-mod state/string, demo-player handle, current/startup game strings, draw-state bool, `m_flSwitchModTransition`); `CGameMenu`/`CGameMenuItem` menu categories (`SetMenuCategory`, `MenuMod`, `MenuCategory`, `ExcludeFromMenuCategory`, `NotInGame`); `workshop_playing_campaign` ConVar; Steam Deck menu-button handling; `CreateGameLogo` |
| OptionsSubGamepad | new file from the disassembly. 14/16 |
| OptionsDialog | gamepad page, `PROPORTIONAL_VALUE` bounds and tab width, `STEAMCONTROLLER_B`, `GetOptionsSubVideo` |
| cvarslider (common/GameUI) | per-slider `scalefactor`, tick-caption precision, no `use_convar_minmax` var. 34/34 |
| ModInfo | `hl2_complete` flag, bounded title copies, `ROOT_MOD` gameinfo. 21/21 |
| LoadingDialog | Steam Deck spinner/loading-bar texture members, destructor |
| imageutils (common) | `V_strcpy_safe` as `Q_strcpy` in retail mode |

## Not matching yet

### BasePanel (38 left) — anniversary expansion switcher / workshop (engine IWorkshop missing), new Steamworks

| function | status | note |
|---|---|---|
| `CGameMenuItem::ApplySchemeSettings(vgui::IScheme*)` | differs | size retail 780 vs ours 775 |
| `CBasePanel::CreateGameMenu()` | differs | size retail 538 vs ours 226 |
| `CBasePanel::SetMenuAlpha(int)` | differs | size retail 173 vs ours 157 |
| `CBasePanel::CBasePanel()` | differs | size retail 3572 vs ours 3540 |
| `CBasePanel::OnThink()` | missing |  |
| `CBasePanel::PaintBackground()` | differs | size retail 273 vs ours 207 |
| `CBasePanel::DrawBackgroundImage()` | differs | size retail 1113 vs ours 670 |
| `CBasePanel::UpdateBackgroundImages()` | missing |  |
| `CBasePanel::UpdateBackgroundState()` | differs | size retail 2010 vs ours 1367 |
| `CBasePanel::SetBackgroundRenderState(CBasePanel::EBackgroundState)` | differs | size retail 568 vs ours 624 |
| `CBasePanel::SwitchExpansion(char const*)` | missing |  |
| `CBasePanel::OnLevelLoadingStarted()` | differs | size retail 147 vs ours 113 |
| `CBasePanel::OnDisconnectFromServer(unsigned char)` | missing |  |
| `CBasePanel::UpdateTitleText(bool)` | missing |  |
| `CBasePanel::OnLevelLoadingFinished()` | differs | size retail 344 vs ours 78 |
| `CBasePanel::RecursiveLoadGameMenu(KeyValues*)` | differs | size retail 428 vs ours 317 |
| `CBasePanel::PerformLayout()` | differs | size retail 488 vs ours 425 |
| `CBasePanel::ApplySchemeSettings(vgui::IScheme*)` | differs | size retail 1922 vs ours 2355 |
| `CBasePanel::OnGameUIActivated()` | differs | size retail 544 vs ours 528 |
| `CBasePanel::RunMenuCommand(char const*)` | differs | size retail 4113 vs ours 2787 |
| `CBasePanel::OnOpenNewGameDialog(int, char const*)` | missing |  |
| `CBasePanel::OnOpenLoadGameDialog(char const*)` | missing |  |
| `CBasePanel::OpenLoadSingleplayerCommentaryDialog(char const*)` | missing |  |
| `CBasePanel::OnOpenAchievementsDialog(char const*)` | missing |  |
| `CBasePanel::OnOpenMatchmakingBasePanel()` | differs | bytes |
| `CBasePanel::OnOpenCSAchievementsDialog()` | differs | size retail 171 vs ours 166 |
| `CBasePanel::SystemNotification(int)` | differs | size retail 1172 vs ours 1232 |
| `CBasePanel::CloseBaseDialogs()` | differs | size retail 388 vs ours 351 |
| `CBasePanel::RefreshDialogs()` | missing |  |
| `RefreshOptionsDialogModeset(CCommand const&)` | missing |  |
| `UtlStringLessFunc(CUtlString const&, CUtlString const&)` | missing |  |
| `CGameMenu::SetMenuCategory(char const*)` | missing |  |
| `CGameMenu::OnKeyCodePressed(ButtonCode_t)` | differs | size retail 1483 vs ours 1254 |
| `SteamInternal_Init_SteamFriends(ISteamFriends**)` | missing |  |
| `CSaveBeforeQuitQueryDialog::OnKeyCodePressed(ButtonCode_t)` | differs | size retail 359 vs ours 142 |
| `SteamInternal_Init_SteamUser(ISteamUser**)` | missing |  |
| `SteamInternal_Init_SteamClient(ISteamClient**)` | missing |  |
| `__GLOBAL__I_a` | differs | size retail 714 vs ours 634 |

### NewGameDialog (34 left) — anniversary chapter/expansion rewrite (ReloadChapters, SetOverrideMod) not yet written

| function | status | note |
|---|---|---|
| `COM_GetModDirectory()` | differs | size retail 315 vs ours 223 |
| `CNewGameDialog::OnCheckButtonChecked(vgui::Panel*)` | missing |  |
| `CNewGameDialog::GetUnlockedChapter()` | missing |  |
| `CNewGameDialog::SetSelectedChapterIndex(int, bool)` | missing |  |
| `CNewGameDialog::ReloadChapters()` | missing |  |
| `CNewGameDialog::CNewGameDialog(vgui::Panel*, bool)` | differs | size retail 2305 vs ours 4247 |
| `CNewGameDialog::~CNewGameDialog()` | missing |  |
| `CNewGameDialog::~CNewGameDialog()` | differs | size retail 38 vs ours 197 |
| `CNewGameDialog::~CNewGameDialog()` | differs | size retail 42 vs ours 201 |
| `CNewGameDialog::SetOverrideMod(char const*)` | missing |  |
| `CNewGameDialog::UpdateMenuComponents(CNewGameDialog::EScrollDirection)` | differs | bytes |
| `CNewGameDialog::Activate()` | differs | size retail 337 vs ours 405 |
| `CNewGameDialog::ApplySettings(KeyValues*)` | differs | bytes |
| `CNewGameDialog::ApplySchemeSettings(vgui::IScheme*)` | differs | size retail 530 vs ours 427 |
| `CNewGameDialog::ScrollSelectionPanels(CNewGameDialog::EScrollDirection)` | differs | bytes |
| `CNewGameDialog::UpdateBonusSelection()` | differs | bytes |
| `CNewGameDialog::InitPanelIndexForDisplay(int)` | differs | bytes |
| `CNewGameDialog::SetSelectedChapter(char const*)` | differs | size retail 182 vs ours 173 |
| `CNewGameDialog::PostScroll(CNewGameDialog::EScrollDirection)` | differs | bytes |
| `CNewGameDialog::AnimateSelectionPanels()` | differs | bytes |
| `CNewGameDialog::ShiftPanelIndices(int)` | differs | bytes |
| `CNewGameDialog::ContinueScrolling()` | differs | bytes |
| `CNewGameDialog::FinishScroll()` | differs | bytes |
| `CNewGameDialog::StartGame()` | differs | size retail 639 vs ours 494 |
| `CNewGameDialog::OnClose()` | differs | bytes |
| `CNewGameDialog::OnCommand(char const*)` | differs | size retail 884 vs ours 857 |
| `CNewGameDialog::PaintBackground()` | differs | bytes |
| `CNewGameDialog::OnKeyCodePressed(ButtonCode_t)` | differs | size retail 864 vs ours 608 |
| `CNewGameDialog::OnKeyCodeReleased(ButtonCode_t)` | differs | bytes |
| `CNewGameDialog::OnThink()` | differs | bytes |
| `CGameChapterPanel::CGameChapterPanel(CNewGameDialog*, char const*, char const*, int, char const*, char const*,` | missing |  |
| `CGameChapterPanel::ApplySchemeSettings(vgui::IScheme*)` | differs | size retail 624 vs ours 387 |
| `CSelectionOverlayPanel::OnMousePressed(ButtonCode_t)` | differs | size retail 74 vs ours 66 |
| `CNewGamePlayButton::OnKeyCodePressed(ButtonCode_t)` | differs | size retail 397 vs ours 192 |

### OptionsSubMultiplayer (30 left) — vgui::ImagePanel is 4 bytes larger in retail (vgui_controls lane)

| function | status | note |
|---|---|---|
| `CrosshairImagePanelSimple::InitCrosshairColorEntries()` | differs | bytes |
| `CrosshairImagePanelSimple::CrosshairImagePanelSimple(vgui::Panel*, char const*, COptionsSubMultiplayer*)` | differs | bytes |
| `CrosshairImagePanelSimple::Paint()` | differs | bytes |
| `CrosshairImagePanelSimple::UpdateCrosshair()` | differs | bytes |
| `CrosshairImagePanelSimple::OnTextChanged(vgui::Panel*)` | differs | bytes |
| `CrosshairImagePanelSimple::ResetData()` | differs | bytes |
| `CrosshairImagePanelSimple::ApplyChanges()` | differs | bytes |
| `CrosshairImagePanelCS::InitCrosshairColorEntries()` | differs | bytes |
| `CrosshairImagePanelCS::CrosshairImagePanelCS(vgui::Panel*, char const*, COptionsSubMultiplayer*)` | differs | bytes |
| `CrosshairImagePanelCS::Paint()` | differs | bytes |
| `CrosshairImagePanelCS::UpdateCrosshair()` | differs | bytes |
| `CrosshairImagePanelCS::OnSliderMoved(KeyValues*)` | differs | bytes |
| `CrosshairImagePanelCS::OnTextChanged(vgui::Panel*)` | differs | bytes |
| `CrosshairImagePanelCS::OnCheckButtonChecked()` | differs | bytes |
| `CrosshairImagePanelCS::ResetData()` | differs | bytes |
| `CrosshairImagePanelCS::ApplyChanges()` | differs | bytes |
| `CrosshairImagePanelAdvanced::InitAdvCrosshairStyleList()` | differs | bytes |
| `CrosshairImagePanelAdvanced::SetCrosshairTexture(char const*)` | differs | bytes |
| `CrosshairImagePanelAdvanced::UpdateCrosshair()` | differs | bytes |
| `CrosshairImagePanelAdvanced::CrosshairImagePanelAdvanced(vgui::Panel*, char const*, COptionsSubMultiplayer*)` | differs | bytes |
| `CrosshairImagePanelAdvanced::~CrosshairImagePanelAdvanced()` | differs | bytes |
| `CrosshairImagePanelAdvanced::~CrosshairImagePanelAdvanced()` | differs | bytes |
| `CrosshairImagePanelAdvanced::Paint()` | differs | bytes |
| `CrosshairImagePanelAdvanced::OnTextChanged(vgui::Panel*)` | differs | bytes |
| `CrosshairImagePanelAdvanced::OnSliderMoved(KeyValues*)` | differs | bytes |
| `CrosshairImagePanelAdvanced::ResetData()` | differs | bytes |
| `CrosshairImagePanelAdvanced::ApplyChanges()` | differs | bytes |
| `CrosshairImagePanelAdvanced::UpdateVisibility()` | differs | bytes |
| `COptionsSubMultiplayer::COptionsSubMultiplayer(vgui::Panel*)` | differs | bytes |
| `COptionsSubMultiplayer::OnApplyChanges()` | differs | size retail 736 vs ours 718 |

### OptionsSubVideo (27 left) — anniversary advanced-video rewrite not yet written

| function | status | note |
|---|---|---|
| `SetUseHDContent(bool)` | missing |  |
| `COptionsSubVideo::PrepareResolutionList()` | differs | size retail 1449 vs ours 1384 |
| `COptionsSubVideo::EnableOrDisableWindowedForVR()` | differs | size retail 246 vs ours 224 |
| `COptionsSubVideo::COptionsSubVideo(vgui::Panel*)` | differs | size retail 4477 vs ours 3345 |
| `COptionsSubVideo::~COptionsSubVideo()` | differs | bytes |
| `COptionsSubVideo::OnResetData()` | differs | size retail 605 vs ours 427 |
| `COptionsSubVideo::SetCurrentResolutionComboItem()` | differs | size retail 483 vs ours 436 |
| `COptionsSubVideo::OnApplyChanges()` | differs | size retail 1798 vs ours 1301 |
| `COptionsSubVideo::PerformLayout()` | differs | size retail 9 vs ours 89 |
| `COptionsSubVideo::OnTextChanged(vgui::Panel*, char const*)` | differs | size retail 412 vs ours 388 |
| `COptionsSubVideo::RequiresRestart()` | differs | bytes |
| `COptionsSubVideo::OpenAdvanced()` | differs | bytes |
| `COptionsSubVideo::OpenGammaDialog()` | differs | bytes |
| `COptionsSubVideo::OpenThirdPartyVideoCreditsDialog()` | differs | bytes |
| `COptionsSubVideoAdvancedDlg::RequiresRestart()` | differs | size retail 248 vs ours 239 |
| `COptionsSubVideoAdvancedDlg::COptionsSubVideoAdvancedDlg(vgui::Panel*)` | differs | size retail 6203 vs ours 4904 |
| `CGammaDialog::CGammaDialog(unsigned int)` | differs | size retail 1446 vs ours 1253 |
| `COptionsSubVideoThirdPartyCreditsDlg::COptionsSubVideoThirdPartyCreditsDlg(unsigned int)` | differs | size retail 634 vs ours 455 |
| `COptionsSubVideoAdvancedDlg::MarkDefaultSettingsAsRecommended()` | differs | size retail 3871 vs ours 3438 |
| `COptionsSubVideoAdvancedDlg::OnCommand(char const*)` | differs | size retail 430 vs ours 324 |
| `COptionsSubVideoAdvancedDlg::Activate()` | differs | bytes |
| `COptionsSubVideoAdvancedDlg::OnSliderChanged(vgui::Panel*)` | missing |  |
| `COptionsSubVideoAdvancedDlg::OnTextChanged(vgui::Panel*)` | differs | size retail 457 vs ours 221 |
| `COptionsSubVideoAdvancedDlg::ResetDXLevelCombo()` | differs | size retail 273 vs ours 264 |
| `COptionsSubVideoAdvancedDlg::OK_Confirmed()` | differs | size retail 113 vs ours 24 |
| `COptionsSubVideoAdvancedDlg::ApplyChanges()` | differs | size retail 2912 vs ours 2244 |
| `COptionsSubVideoAdvancedDlg::OnResetData()` | differs | size retail 2117 vs ours 1742 |

### achievementsdialog (17 left) — anniversary SetOverrideMod/OnThink + layout not yet written

| function | status | note |
|---|---|---|
| `LoadAchievementIcon(vgui::ImagePanel*, IAchievement*, char const*)` | differs | size retail 560 vs ours 373 |
| `CAchievementsDialog::UpdateAchievementList()` | differs | size retail 1621 vs ours 1097 |
| `CAchievementsDialog::CreateOrUpdateComboItems(bool)` | differs | bytes |
| `CAchievementsDialog::CAchievementsDialog(vgui::Panel*)` | differs | size retail 1575 vs ours 1573 |
| `CAchievementsDialog::~CAchievementsDialog()` | differs | size retail 279 vs ours 185 |
| `CAchievementsDialog::SetOverrideMod(char const*)` | missing |  |
| `CAchievementsDialog::ApplySettings(KeyValues*)` | differs | bytes |
| `CAchievementsDialog::OnSizeChanged(int, int)` | differs | bytes |
| `CAchievementDialogItemPanel::CAchievementDialogItemPanel(vgui::PanelListPanel*, char const*, int, char const*)` | missing |  |
| `CAchievementsDialog::OnThink()` | missing |  |
| `CAchievementsDialog::ApplySchemeSettings(vgui::IScheme*)` | differs | size retail 1184 vs ours 1175 |
| `CAchievementsDialog::ScrollToItem(int)` | differs | bytes |
| `CAchievementsDialog::OnKeyCodePressed(ButtonCode_t)` | differs | size retail 960 vs ours 896 |
| `CAchievementsDialog::UpdateAchievementDialogInfo()` | differs | size retail 667 vs ours 567 |
| `CAchievementDialogItemPanel::UpdateAchievementInfo(vgui::IScheme*)` | differs | size retail 1245 vs ours 1271 |
| `CAchievementDialogItemPanel::~CAchievementDialogItemPanel()` | differs | size retail 97 vs ours 38 |
| `CAchievementDialogItemPanel::~CAchievementDialogItemPanel()` | differs | size retail 101 vs ours 42 |

### BonusMapsDialog (12 left) — layout (+8) and base-code key handling not yet written

| function | status | note |
|---|---|---|
| `CBonusMapsDialog::BuildMapsList()` | differs | bytes |
| `CBonusMapsDialog::CBonusMapsDialog(vgui::Panel*)` | differs | bytes |
| `CBonusMapsDialog::SetSelectedBooleanStatus(char const*, bool)` | differs | bytes |
| `CBonusMapsDialog::RefreshData()` | differs | bytes |
| `CBonusMapsDialog::OnCommand(char const*)` | differs | bytes |
| `CBonusMapsDialog::OnKeyCodeTyped(ButtonCode_t)` | differs | size retail 267 vs ours 56 |
| `CBonusMapsDialog::OnKeyCodePressed(ButtonCode_t)` | differs | size retail 1302 vs ours 1024 |
| `CBonusMapsDialog::OnPanelSelected()` | differs | bytes |
| `CBonusMapsDialog::OnControlModified()` | differs | bytes |
| `CBonusMapPanel::CBonusMapPanel(vgui::PanelListPanel*, char const*, int)` | differs | size retail 962 vs ours 868 |
| `CBonusMapPanel::SetBonusMapInfo(char const*, BonusMapDescription_t&)` | differs | size retail 890 vs ours 859 |
| `CBonusMapPanel::ApplySchemeSettings(vgui::IScheme*)` | differs | size retail 356 vs ours 121 |

### matchmakingbasepanel (9 left) — public/const.h MAX_MAP_NAME must be 32 (headers lane)

| function | status | note |
|---|---|---|
| `CMatchmakingBasePanel::CMatchmakingBasePanel(vgui::Panel*)` | differs | bytes |
| `CMatchmakingBasePanel::~CMatchmakingBasePanel()` | differs | bytes |
| `CMatchmakingBasePanel::~CMatchmakingBasePanel()` | differs | bytes |
| `CMatchmakingBasePanel::SetFooterButtons(CBaseDialog*, KeyValues*, int)` | differs | bytes |
| `CMatchmakingBasePanel::ShowFooter(bool)` | differs | bytes |
| `CMatchmakingBasePanel::SetFooterButtonVisible(char const*, bool)` | differs | bytes |
| `CMatchmakingBasePanel::LoadSessionProperties()` | differs | bytes |
| `CMatchmakingBasePanel::OnOpenSessionBrowserDialog(char const*)` | differs | bytes |
| `CMatchmakingBasePanel::OnOpenSessionLobbyDialog(char const*)` | differs | bytes |

### LoadingDialog (7 left) — Steam Deck loading bar/spinner PaintBackground not yet written

| function | status | note |
|---|---|---|
| `CLoadingDialog::SetupControlSettings(bool)` | differs | size retail 558 vs ours 323 |
| `CLoadingDialog::CLoadingDialog(vgui::Panel*)` | differs | size retail 1782 vs ours 1557 |
| `CLoadingDialog::PaintBackground()` | differs | size retail 1849 vs ours 624 |
| `CLoadingDialog::Open()` | differs | size retail 275 vs ours 247 |
| `CLoadingDialog::DisplayGenericError(char const*, char const*)` | differs | size retail 760 vs ours 584 |
| `CLoadingDialog::PerformLayout()` | differs | size retail 291 vs ours 380 |
| `CLoadingDialog::OnKeyCodePressed(ButtonCode_t)` | differs | size retail 290 vs ours 293 |

### GameUI_Interface (6 left) — MakeBasePanel/Initialize/RunFrame anniversary changes, OnOpenNewGameDialog(int,const char*)

| function | status | note |
|---|---|---|
| `MakeBasePanel()` | missing |  |
| `CGameUI::Initialize(void* (*)(char const*, int*))` | differs | size retail 642 vs ours 934 |
| `CGameUI::RunFrame()` | differs | size retail 589 vs ours 552 |
| `CGameUI::OnDisconnectFromServer(unsigned char)` | differs | size retail 260 vs ours 242 |
| `CGameUI::ShowNewGameDialog(int)` | differs | size retail 46 vs ours 100 |
| `__GLOBAL__I_a` | differs | size retail 281 vs ours 471 |

### OptionsSubMouse (5 left)

| function | status | note |
|---|---|---|
| `COptionsSubMouse::COptionsSubMouse(vgui::Panel*)` | differs | size retail 3038 vs ours 2067 |
| `COptionsSubMouse::OnResetData()` | differs | size retail 371 vs ours 245 |
| `COptionsSubMouse::OnApplyChanges()` | differs | size retail 374 vs ours 278 |
| `COptionsSubMouse::OnControlModified(vgui::Panel*)` | differs | size retail 582 vs ours 569 |
| `COptionsSubMouse::OnTextChanged(vgui::Panel*)` | differs | size retail 597 vs ours 415 |

### GameConsole (4 left)

| function | status | note |
|---|---|---|
| `CGameConsole::~CGameConsole()` | differs | size retail 5 vs ours 26 |
| `CGameConsole::Initialize()` | differs | size retail 56 vs ours 211 |
| `CGameConsole::SetParent(int)` | differs | size retail 302 vs ours 41 |
| `__GLOBAL__I_a` | differs | size retail 219 vs ours 232 |

### LoadGameDialog (4 left) — Workshop save loading needs Steamworks UGC 020 + engine IWorkshop

| function | status | note |
|---|---|---|
| `CLoadGameDialog::OnCommand(char const*)` | differs | size retail 1567 vs ours 672 |
| `int V_sprintf_safe<2048ul>(char (&) [2048ul], char const*, ...)` | missing |  |
| `SteamInternal_Init_SteamUtils(ISteamUtils**)` | missing |  |
| `SteamInternal_Init_SteamUGC(ISteamUGC**)` | missing |  |

### OptionsSubAudio (4 left)

| function | status | note |
|---|---|---|
| `COptionsSubAudio::OnResetData()` | differs | size retail 1842 vs ours 1794 |
| `COptionsSubAudio::OnCommand(char const*)` | differs | size retail 375 vs ours 340 |
| `COptionsSubAudioThirdPartyCreditsDlg::COptionsSubAudioThirdPartyCreditsDlg(unsigned int)` | differs | size retail 634 vs ours 455 |
| `SteamInternal_Init_SteamApps(ISteamApps**)` | missing |  |

### imageutils (3 left)

| function | status | note |
|---|---|---|
| `ImgUtl_ConvertJPEGToTGA(char const*, char const*, bool)` | missing |  |
| `ValveJpegErrorHandler(jpeg_common_struct*)` | missing |  |
| `ImgUtl_ConvertToVTFAndDumpVMT(char const*, char const*, int, int)` | differs | size retail 1629 vs ours 1531 |

### CommentaryDialog (3 left)

| function | status | note |
|---|---|---|
| `CCommentaryDialog::OnKeyCodePressed(ButtonCode_t)` | differs | size retail 317 vs ours 111 |
| `OpenCommentaryDialog()` | differs | size retail 244 vs ours 130 |
| `CPostCommentaryDialog::OnKeyCodePressed(ButtonCode_t)` | differs | size retail 304 vs ours 98 |

### LoadCommentaryDialog (3 left)

| function | status | note |
|---|---|---|
| `OpenLoadCommentaryDialog()` | differs | size retail 146 vs ours 130 |
| `CLoadCommentaryDialog::OnKeyCodePressed(ButtonCode_t)` | differs | size retail 305 vs ours 81 |
| `CCommentaryItemPanel::CCommentaryItemPanel(vgui::PanelListPanel*, char const*, int)` | differs | bytes |

### OptionsSubDifficulty (3 left)

| function | status | note |
|---|---|---|
| `COptionsSubDifficulty::COptionsSubDifficulty(vgui::Panel*)` | differs | size retail 786 vs ours 595 |
| `COptionsSubDifficulty::OnResetData()` | differs | size retail 114 vs ours 100 |
| `COptionsSubDifficulty::OnCommand(char const*)` | missing |  |

### OptionsSubKeyboard (3 left)

| function | status | note |
|---|---|---|
| `COptionsSubKeyboard::ParseActionDescriptions()` | differs | size retail 1238 vs ours 998 |
| `COptionsSubKeyboard::OnCommand(char const*)` | differs | size retail 521 vs ours 486 |
| `COptionsSubKeyboardAdvancedDlg::COptionsSubKeyboardAdvancedDlg(unsigned int)` | differs | size retail 634 vs ours 455 |

### URLButton (3 left)

| function | status | note |
|---|---|---|
| `vgui::URLButton::URLButton(vgui::Panel*, char const*, char const*, vgui::Panel*, char const*)` | differs | size retail 839 vs ours 778 |
| `vgui::URLButton::Paint()` | differs | size retail 263 vs ours 175 |
| `vgui::URLButton::SizeToContents()` | differs | size retail 213 vs ours 69 |

### scriptobject (2 left)

| function | status | note |
|---|---|---|
| `CDescription::ReadFromBuffer(char const**)` | missing |  |
| `CDescription::InitFromFile(char const*)` | missing |  |

### BaseSaveGameDialog (2 left) — vgui::ImagePanel size (vgui_controls lane)

| function | status | note |
|---|---|---|
| `CBaseSaveGameDialog::SetOverrideMod(char const*)` | missing |  |
| `CSaveGamePanel::CSaveGamePanel(vgui::PanelListPanel*, char const*, int)` | differs | bytes |

### CreateMultiplayerGameDialog (2 left)

| function | status | note |
|---|---|---|
| `CCreateMultiplayerGameDialog::CCreateMultiplayerGameDialog(vgui::Panel*)` | differs | size retail 997 vs ours 855 |
| `CCreateMultiplayerGameDialog::OnKeyCodePressed(ButtonCode_t)` | differs | size retail 550 vs ours 580 |

### MultiplayerAdvancedDialog (2 left)

| function | status | note |
|---|---|---|
| `CMultiplayerAdvancedDialog::CreateControls()` | differs | size retail 2112 vs ours 1872 |
| `CMultiplayerAdvancedDialog::CMultiplayerAdvancedDialog(vgui::Panel*)` | differs | size retail 1015 vs ours 858 |

### OptionsDialog (2 left) — page sizes of COptionsSubDifficulty/Mouse/Video

| function | status | note |
|---|---|---|
| `COptionsDialog::COptionsDialog(vgui::Panel*)` | differs | bytes |
| `COptionsDialog::GetOptionsSubVideo()` | missing |  |

### OptionsDialog_Xbox (2 left)

| function | status | note |
|---|---|---|
| `COptionsDialogXbox::ApplySchemeSettings(vgui::IScheme*)` | differs | size retail 1942 vs ours 1923 |
| `COptionsDialogXbox::HandleInactiveKeyCodePressed(ButtonCode_t)` | differs | size retail 1664 vs ours 1424 |

### OptionsSubGamepad (2 left) — Steam Input (ISteamInput) needs newer Steamworks headers

| function | status | note |
|---|---|---|
| `COptionsSubGamepad::OnCommand(char const*)` | differs | size retail 227 vs ours 9 |
| `SteamInternal_Init_SteamInput(ISteamInput**)` | missing |  |

### OptionsSubVoice (2 left)

| function | status | note |
|---|---|---|
| `COptionsSubVoice::COptionsSubVoice(vgui::Panel*)` | differs | bytes |
| `COptionsSubVoice::OnCommand(char const*)` | differs | size retail 160 vs ours 150 |

### BenchmarkDialog (1 left)

| function | status | note |
|---|---|---|
| `CBenchmarkDialog::OnKeyCodePressed(ButtonCode_t)` | differs | size retail 275 vs ours 73 |

### CommandCheckButton (1 left)

| function | status | note |
|---|---|---|
| `vgui::CheckButton::IsCheckButtonCheckable() const` | differs | bytes |

### CommentaryExplanationDialog (1 left)

| function | status | note |
|---|---|---|
| `CCommentaryExplanationDialog::OnKeyCodePressed(ButtonCode_t)` | differs | size retail 506 vs ours 977 |

### CreateMultiplayerGameGameplayPage (1 left)

| function | status | note |
|---|---|---|
| `CCreateMultiplayerGameGameplayPage::CCreateMultiplayerGameGameplayPage(vgui::Panel*, char const*)` | differs | size retail 497 vs ours 389 |

### GameConsoleDialog (1 left)

| function | status | note |
|---|---|---|
| `CGameConsoleDialog::CGameConsoleDialog(vgui::Panel*)` | missing |  |

### basedialog (1 left)

| function | status | note |
|---|---|---|
| `CScenarioInfoPanel::CScenarioInfoPanel(vgui::Panel*, char const*)` | differs | bytes |

### dialogmenu (1 left)

| function | status | note |
|---|---|---|
| `CDialogMenu::HandleKeyCode(ButtonCode_t)` | differs | size retail 812 vs ours 560 |

### leaderboarddialog (1 left)

| function | status | note |
|---|---|---|
| `CLeaderboardDialog::OnKeyCodePressed(ButtonCode_t)` | differs | size retail 612 vs ours 320 |

### sessionbrowserdialog (1 left)

| function | status | note |
|---|---|---|
| `CSessionBrowserDialog::SessionSearchResult(int, void*, _XSESSION_SEARCHRESULT*, int)` | differs | size retail 961 vs ours 970 |

### sessionlobbydialog (1 left)

| function | status | note |
|---|---|---|
| `CSessionLobbyDialog::OnKeyCodePressed(ButtonCode_t)` | differs | size retail 1360 vs ours 1080 |

### sessionoptionsdialog (1 left)

| function | status | note |
|---|---|---|
| `CSessionOptionsDialog::OnKeyCodePressed(ButtonCode_t)` | differs | size retail 401 vs ours 193 |

### SaveGameBrowserDialog (1 left) — vgui::ImagePanel size / CBasePanel console-UI members

| function | status | note |
|---|---|---|
| `CGameSavePanel::CGameSavePanel(CSaveGameBrowserDialog*, SaveGameDescription_t*, bool)` | differs | bytes |
