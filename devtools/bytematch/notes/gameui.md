# GameUI (bin/GameUI) per-function status

Measured with `--retail-osx32` (clang-600.0.57) and `devtools/bytematch` against depot 223
build 19307283, own objects only (`gameui.build/*.o`). Lane `bytematch-gameui`.

Own objects: 1736 / 1825 after lane `bytematch-gameui2` (1638 at its start on `bytematch` 39261259; 1367 / 1809 before lane `bytematch-gameui`). The tables below are from the gameui2 final run.
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

## Lane gameui2 (anniversary code from the retail disassembly)

| object | change |
|---|---|
| BasePanel | expansion switcher: `SwitchExpansion`, `OnThink` state machine (fade, `disconnect`, `launchmod`), `OnDisconnectFromServer`, `OnLevelLoadingFinished` (current expansion), `UpdateTitleText` (episode titles, Workshop campaign title through `IWorkshop`), `UpdateBackgroundImages` split from `ApplySchemeSettings`, Steam Deck background/render-target paths, `BACKGROUND_SWITCHMOD`; `OnOpen*Dialog( ..., pszMod )`; `RunMenuCommand` retail command set (`OpenMainMenuDialog`, `ReturnToMainMenu`, `SwitchCategory`, `SwitchExpansion`, `OpenWorkshopDialog`, mod suffixes); `CreateGameMenu` startup/current game; `RecursiveLoadGameMenu` `Caps` and default category; `RefreshDialogs` + `refresh_gameui_dialogs_modeset`. 217/230 |
| GameUI_Interface | `MakeBasePanel`, `deck_%language%.txt`, VR without `-vr`, Steam Deck startup progress, `OnDisconnectFromServer` forwards to the base panel. 68/68 |
| NewGameDialog | `ReloadChapters` (per-mod chapter scan, `BASE_PATH`, 64 chapters), `GetUnlockedChapter` (`sv_unlockedchapters_<mod>`), `SetOverrideMod`, commentary check button (`OnCheckButtonChecked`, `IsCommentaryEnabled`), `SetSelectedChapterIndex( int, bool )`, `launchmod` start command, per-mod chapter images, caps/proportional chapter panels, base-code keys. 61/63 |
| achievementsdialog | override mod (icons `vgui/<mod>/achievements`, item panel mod), list and group counts rebuilt per update, filtering through anniversary `IAchievementMgr` slots 11/12 (retail build only, see report), hide-achieved relayout in `OnThink`, proportional bounds. 43/45 |
| BonusMapsDialog | `CBonusMapPanel` keeps the .res image size and scales proportionally; base-code keys with Steam Controller pads; unbounded `strcpy` in retail mode. 37/38 |
| LoadingDialog | Steam Deck loading screen (`LoadingDialogDeck.res`, full-screen layout, faded edges, progress strip, spinner and logo), `flOpenTime`, proportional error layout. 31/32 |
| LoadGameDialog | Workshop campaign save loading (UGC item state, `IWorkshop::GetItem`/`LoadCampaign`, `viewonworkshop`). 11/11 |
| OptionsSubVideo | retail `COptionsSubVideo` member layout only; the anniversary video page and advanced dialog are not written. 47/70 |

### OptionsSubVideo findings (not written yet)

Retail `COptionsSubVideo` (0x20c bytes): `m_nSelectedHudAspect` at 0x1c4 (index from `hud_aspect`, compared in
`OnApplyChanges`), `HudAspectRatio` combo at 0x1dc (FitDisplay / 4:3 / 16:9 / 16:10), `HDR` combo at 0x1f0
(moved from the advanced dialog, enabled for `mat_dxlevel` >= 80), `DisplayModeCombo` at 0x1d0 built from
`SDL_GetNumVideoDisplays()` (`#GameUI_Fullscreen` per display, `#GameUI_Windowed`, `#GameUI_NoWindowBorder`),
HD-content and HDR availability bools at 0x208/0x209, `maps/test_hardware.bsp` hides the benchmark button.
`COptionsSubVideoAdvancedDlg` is 0x318 bytes (ours 0x2fc) and gains `OnSliderChanged`.

## Not matching yet

### OptionsSubVideo (23 left)

| function | status | note |
|---|---|---|
| `SetUseHDContent(bool)` | missing |  |
| `COptionsSubVideo::PrepareResolutionList()` | differs | size retail 1449 vs ours 1384 |
| `COptionsSubVideo::EnableOrDisableWindowedForVR()` | differs | size retail 246 vs ours 224 |
| `COptionsSubVideo::COptionsSubVideo(vgui::Panel*)` | differs | size retail 4477 vs ours 3393 |
| `COptionsSubVideo::OnResetData()` | differs | size retail 605 vs ours 427 |
| `COptionsSubVideo::SetCurrentResolutionComboItem()` | differs | size retail 483 vs ours 436 |
| `COptionsSubVideo::OnApplyChanges()` | differs | size retail 1798 vs ours 1301 |
| `COptionsSubVideo::PerformLayout()` | differs | size retail 9 vs ours 89 |
| `COptionsSubVideo::OnTextChanged(vgui::Panel*, char const*)` | differs | size retail 412 vs ours 388 |
| `COptionsSubVideo::OpenAdvanced()` | differs | line constants only: 792->764 |
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

### BasePanel (13 left)

| function | status | note |
|---|---|---|
| `CGameMenuItem::ApplySchemeSettings(vgui::IScheme*)` | differs | size retail 780 vs ours 775 |
| `CBasePanel::OnThink()` | differs | line constants only: 428->420, 400->392 |
| `CBasePanel::UpdateBackgroundImages()` | differs | size retail 644 vs ours 666 |
| `CBasePanel::UpdateTitleText(bool)` | differs | size retail 1230 vs ours 1238 |
| `CBasePanel::OnLevelLoadingFinished()` | differs | size retail 344 vs ours 339 |
| `CBasePanel::PerformLayout()` | differs | size retail 488 vs ours 425 |
| `CBasePanel::OnGameUIActivated()` | differs | size retail 544 vs ours 528 |
| `CBasePanel::SystemNotification(int)` | differs | size retail 1172 vs ours 1232 |
| `CBasePanel::CloseBaseDialogs()` | differs | size retail 388 vs ours 351 |
| `CBasePanel::RefreshDialogs()` | differs | size retail 2107 vs ours 2111 |
| `UtlStringLessFunc(CUtlString const&, CUtlString const&)` | missing |  |
| `CGameMenu::OnKeyCodePressed(ButtonCode_t)` | differs | size retail 1483 vs ours 1254 |
| `CSaveBeforeQuitQueryDialog::OnKeyCodePressed(ButtonCode_t)` | differs | size retail 359 vs ours 142 |

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

### CommentaryDialog (3 left)

| function | status | note |
|---|---|---|
| `CCommentaryDialog::OnKeyCodePressed(ButtonCode_t)` | differs | size retail 317 vs ours 111 |
| `OpenCommentaryDialog()` | differs | size retail 244 vs ours 130 |
| `CPostCommentaryDialog::OnKeyCodePressed(ButtonCode_t)` | differs | size retail 304 vs ours 98 |

### imageutils (3 left)

| function | status | note |
|---|---|---|
| `ImgUtl_ConvertJPEGToTGA(char const*, char const*, bool)` | missing |  |
| `ValveJpegErrorHandler(jpeg_common_struct*)` | missing |  |
| `ImgUtl_ConvertToVTFAndDumpVMT(char const*, char const*, int, int)` | differs | size retail 1629 vs ours 1531 |

### OptionsSubAudio (3 left)

| function | status | note |
|---|---|---|
| `COptionsSubAudio::OnResetData()` | differs | size retail 1842 vs ours 1826 |
| `COptionsSubAudio::OnCommand(char const*)` | differs | size retail 375 vs ours 340 |
| `COptionsSubAudioThirdPartyCreditsDlg::COptionsSubAudioThirdPartyCreditsDlg(unsigned int)` | differs | size retail 634 vs ours 455 |

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

### achievementsdialog (2 left)

| function | status | note |
|---|---|---|
| `CAchievementsDialog::UpdateAchievementList()` | differs | size retail 1621 vs ours 1605 |
| `CAchievementsDialog::UpdateAchievementDialogInfo()` | differs | bytes |

### CreateMultiplayerGameDialog (2 left)

| function | status | note |
|---|---|---|
| `CCreateMultiplayerGameDialog::CCreateMultiplayerGameDialog(vgui::Panel*)` | differs | size retail 997 vs ours 855 |
| `CCreateMultiplayerGameDialog::OnKeyCodePressed(ButtonCode_t)` | differs | size retail 550 vs ours 580 |

### LoadCommentaryDialog (2 left)

| function | status | note |
|---|---|---|
| `OpenLoadCommentaryDialog()` | differs | size retail 146 vs ours 130 |
| `CLoadCommentaryDialog::OnKeyCodePressed(ButtonCode_t)` | differs | size retail 305 vs ours 81 |

### MultiplayerAdvancedDialog (2 left)

| function | status | note |
|---|---|---|
| `CMultiplayerAdvancedDialog::CreateControls()` | differs | size retail 2112 vs ours 1872 |
| `CMultiplayerAdvancedDialog::CMultiplayerAdvancedDialog(vgui::Panel*)` | differs | size retail 1015 vs ours 858 |

### NewGameDialog (2 left)

| function | status | note |
|---|---|---|
| `CNewGameDialog::OnCheckButtonChecked(vgui::Panel*)` | differs | size retail 499 vs ours 483 |
| `CNewGameDialog::ReloadChapters()` | differs | size retail 2726 vs ours 2790 |

### OptionsDialog_Xbox (2 left)

| function | status | note |
|---|---|---|
| `COptionsDialogXbox::ApplySchemeSettings(vgui::IScheme*)` | differs | size retail 1942 vs ours 1923 |
| `COptionsDialogXbox::HandleInactiveKeyCodePressed(ButtonCode_t)` | differs | size retail 1664 vs ours 1424 |

### scriptobject (2 left)

| function | status | note |
|---|---|---|
| `CDescription::ReadFromBuffer(char const**)` | missing |  |
| `CDescription::InitFromFile(char const*)` | missing |  |

### BenchmarkDialog (1 left)

| function | status | note |
|---|---|---|
| `CBenchmarkDialog::OnKeyCodePressed(ButtonCode_t)` | differs | size retail 275 vs ours 73 |

### BonusMapsDialog (1 left)

| function | status | note |
|---|---|---|
| `CBonusMapsDialog::OnKeyCodePressed(ButtonCode_t)` | differs | size retail 1302 vs ours 1312 |

### CommentaryExplanationDialog (1 left)

| function | status | note |
|---|---|---|
| `CCommentaryExplanationDialog::OnKeyCodePressed(ButtonCode_t)` | differs | size retail 506 vs ours 977 |

### CreateMultiplayerGameGameplayPage (1 left)

| function | status | note |
|---|---|---|
| `CCreateMultiplayerGameGameplayPage::CCreateMultiplayerGameGameplayPage(vgui::Panel*, char const*)` | differs | size retail 497 vs ours 389 |

### dialogmenu (1 left)

| function | status | note |
|---|---|---|
| `CDialogMenu::HandleKeyCode(ButtonCode_t)` | differs | size retail 812 vs ours 560 |

### GameConsoleDialog (1 left)

| function | status | note |
|---|---|---|
| `CGameConsoleDialog::CGameConsoleDialog(vgui::Panel*)` | missing |  |

### leaderboarddialog (1 left)

| function | status | note |
|---|---|---|
| `CLeaderboardDialog::OnKeyCodePressed(ButtonCode_t)` | differs | size retail 612 vs ours 320 |

### LoadingDialog (1 left)

| function | status | note |
|---|---|---|
| `CLoadingDialog::PaintBackground()` | differs | size retail 1849 vs ours 1874 |

### OptionsDialog (1 left)

| function | status | note |
|---|---|---|
| `COptionsDialog::COptionsDialog(vgui::Panel*)` | differs | line constants only: 460->456, 528->512 |

### OptionsSubMultiplayer (1 left)

| function | status | note |
|---|---|---|
| `COptionsSubMultiplayer::OnApplyChanges()` | differs | size retail 736 vs ours 718 |

### sessionlobbydialog (1 left)

| function | status | note |
|---|---|---|
| `CSessionLobbyDialog::OnKeyCodePressed(ButtonCode_t)` | differs | size retail 1360 vs ours 1080 |

### sessionoptionsdialog (1 left)

| function | status | note |
|---|---|---|
| `CSessionOptionsDialog::OnKeyCodePressed(ButtonCode_t)` | differs | size retail 401 vs ours 193 |

