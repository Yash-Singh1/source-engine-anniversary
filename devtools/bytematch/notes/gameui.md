# GameUI (bin/GameUI) per-function status

Measured with `--retail-osx32` (clang-600.0.57) and `devtools/bytematch` against depot 223
build 19307283, own objects only (`gameui.build/*.o`). Lane `bytematch-gameui`.

Own objects: **1818 / 1825** after lane `bytematch-gameui3` (1738 at its start on `bytematch` 1b6e8971; whole dylib 4948 / 4970); 1736 after `bytematch-gameui2`, 1367 / 1809 before lane `bytematch-gameui`.
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
| OptionsSubVideo | retail `COptionsSubVideo` member layout only (gameui3 wrote the rest). 47/70 |

## Lane gameui3

| object | change |
|---|---|
| OptionsSubVideo (main page) | anniversary layout written from the disassembly: display-mode combo with one `#GameUI_Fullscreen (%d)` entry per SDL display plus windowed and `%ls (%ls)` borderless entries (borderless only in retail mode: the native config and `mat_setvideomode` lack it), `HudAspectRatio` combo driven by `hud_aspect` (fit / 4:3 / 16:9 / 16:10, thresholds 1.4 and 1.7), HDR combo moved here (hidden until the .res shows it), HD content box shown only when the .res enables it and the mod has HD content, wide resolution names with localized `#GameUI_Native`, `mat_setvideomode w h windowed noborder`, `mat_hdr_level` command, `PerformLayout` is the base call. `SetUseHDContent` is a free function, `BUseHDContent`/`FOpenGameHDFile`/`GetNameForDXLevel`/`GetScreenAspectMode` are static. VR uses the anniversary `ISourceVirtualReality::IsHmdAvailable()` (slot 27). 70/70 |
| OptionsSubVideo (advanced dialog) | retail member layout (0x318 bytes): `D3D9Ex` combo (`mat_supports_d3d9ex`, `mat_disable_d3d9ex`, relaunch message), hidden HD content box, HDR and Classic Effects combos (`r_classic_fire`/`r_classic_blood`) each with a visibility flag set from the .res, `#gameui_ultra` shader level (`r_lightmap_bicubic`), `OnSliderChanged` and the FOV value label (`L"%.0f"`, 7 ticks), `ApplyButtonEnable` on OK, triple-parent query boxes |
| CGammaDialog, *ThirdPartyCreditsDlg, COptionsSubKeyboardAdvancedDlg, CMultiplayerAdvancedDialog | parented to the GameUI panel, proportional sizes |
| OptionsSubMouse | joystick options hidden, autoaim combo / scale slider (`m_autoaim_mode`, `m_autoaim_scale`), visibility flags from the .res. 14/14 |
| OptionsSubDifficulty | hidden `MigrateButton` and `OnCommand` (`migrate_episodic_data` for HL2 complete). 12/12 |
| OptionsSubAudio, OptionsSubKeyboard | query boxes parented three levels up; zeroed language buffers; proportional key-list columns |
| GameConsole | the dialog is created in `SetParent` with the `BaseUI` panel as parent and laid out once; `Initialize` deletes an old one. 11/11 |
| URLButton | proportional text inset, underline offset and padding. 48/48 |
| scriptobject | `CDescription::ReadFromBuffer`/`InitFromFile` without `bAllowNewObject` |
| key handlers | `GetBaseButtonCode` and the retail button sets in BenchmarkDialog, CommentaryDialog, CommentaryExplanationDialog, LoadCommentaryDialog, CreateMultiplayerGameDialog, leaderboard, session options/lobby, dialogmenu, OptionsDialog_Xbox, BonusMapsDialog, CGameMenu, CSaveBeforeQuitQueryDialog (some retail sets omit Steam pad down or Steam A/B; reproduced) |
| BasePanel | `UtlStringLessFunc` is file-static and case-insensitive (utlstring.h no longer has the inline one); `PerformLayout` shows the title buttons and hides the logo in levels; `SystemNotification` no longer hides the buttons; `CGameMenuItem::ApplySchemeSettings` order; `OnThink` calls the base only while switching; Steam Deck skips the loading icon; `CloseBaseDialogs` also closes the demo player; `RefreshDialogs` reopen order. 228/230 |
| others | Steam Deck early-outs for the commentary dialogs, `clamp<float>` for the loading fade, `V_FixDoubleSlashes` on the player model command, `V_strcat_safe` default count in imageutils |

## Not matching yet (7)

| function | status | note |
|---|---|---|
| `CBasePanel::UpdateTitleText(bool)` | differs | 1230 vs 1238; same calls, block layout of the `CUtlVector` cleanup |
| `CBasePanel::OnLevelLoadingFinished()` | differs | 344 vs 339; scheduling around the inlined `IsExpansionMenuCategory` |
| `COptionsDialogXbox::ApplySchemeSettings(vgui::IScheme*)` | differs | 1942 vs 1923; register allocation, retail frame 16 bytes larger |
| `CNewGameDialog::ReloadChapters()` | differs | 2726 vs 2790; block placement of the bonus-unlock branch |
| `CNewGameDialog::OnCheckButtonChecked(vgui::Panel*)` | differs | 499 vs 483; how the inlined `clamp` folds |
| `CAchievementsDialog::UpdateAchievementList()` | differs | 1621 vs 1605; scheduling |
| `CAchievementsDialog::UpdateAchievementDialogInfo()` | differs | same size; scheduling |
