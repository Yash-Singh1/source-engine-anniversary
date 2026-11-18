# inputsystem

Retail: depot 223 `bin/inputsystem.dylib`, objects `inputsystem.o`,
`joystick_sdl.o`, `key_translation.o`, `steamcontroller.o`, plus `libtier2.a(tier2.o)`
and tier1 members (convar, interface, KeyValues, strtools, tier1, utlbuffer,
utlstring), which other lanes own.

Retail is the anniversary Steam Input rewrite: Steamworks SDK headers with the
1.60 interface set (`SteamInput006`, `SteamController008`, and a
`SteamAPI_InitEx` interface list with `STEAMUGC_INTERFACE_VERSION020` and
`STEAMUSERSTATS_INTERFACE_VERSION012`; 1.61 has UserStats 013),
`STEAM_CALLBACK` for `SteamInputConfigurationLoaded_t`, an on-disc action
manifest (`steam_input\action_manifest_hl2.vdf`), and active-input-type
tracking (`EActiveInputType_t`). The retail debug map shows Valve kept these in
`public/steam/`. Here the 1.60 headers `steam_api.h` pulls in are vendored at
`inputsystem/steam/` (found through the module's `.` include path before the
tree's old `public/steam`); `stub_steam` exports the three Steamworks entry
points they need, so the native build gets a NULL `SteamInput()`.

Verification: `--retail-osx32` build, bytematch per N_OSO object. All 193 retail
functions in scope match (159 exact, 34 after masking); the four inputsystem
objects are 110/110 and 21,473/21,473 bytes. The linked `inputsystem.dylib` has
every section byte-identical to retail (`__text`, stubs, `__const`,
`__cstring`, unwind/EH, all of `__DATA`); only the symbol table (debug-map
paths, one extra stab) and the UUID differ. The mod-small strict checker
passes 105/110; the other 5 reference `__literal4`/`__const` constants that it
resolves to a neighbouring retail symbol, and the section comparison shows
those bytes are identical. Before: 113/192 (34% of bytes).

## CInputSystem layout (retail offsets)

| offset | member |
|---|---|
| 0x008 | `STEAM_CALLBACK( CInputSystem, OnSteamInputConfigurationLoaded, SteamInputConfigurationLoaded_t )` |
| 0x014 | `steampad_t m_Device[MAX_STEAM_CONTROLLERS]` (0x128 each) |
| 0x954 | `m_unNumConnected`, 0x958 `InputHandle_t m_hActiveSteamInput`, 0x960 unused int, 0x964 `m_nControllerType[9]`, 0x988 `m_eActiveInputType` |
| 0x98c | `m_pLauncherMgr`, `m_ChainedWndProc`, `m_hAttachedHWnd`, enabled/pump/polling bools |
| 0x99c | `InputState_t m_InputState[2]` (0x1184 each; 539 button codes, 10 analog codes) |
| 0x2ca4 | ticks, poll count, mouse wheel, `m_JoysticksEnabled`, `m_nJoystickCount`, `m_nJoystickBaseline`, joystick/xcontroller/steam bools |
| 0x2cc8 | `m_flLastSteamControllerInput`, 0x2ccc `m_pRadialMenuStickVal[16][2]`, 0x2d4c `m_pJoystickInfo[1]` |
| 0x2d94 | `m_appXKeys`, `m_XDevices`, `m_PrimaryUserId`, raw mouse, `m_hEvent`, XInput/RawInput modules |
| 0x2f08 | last configuration device handle and major/minor revision, 0x2f18 next device check time, 0x2f1c `m_bConsoleTextMode`, `m_bOnDiscManifestLoaded` |

Fork touch members sit after these under `#ifndef BYTEMATCH_RETAIL`.

## Functions

"layout" = the source didn't change, only `CInputSystem` member offsets did.

| function | status | note |
|---|---|---|
| QueryInterface, Disconnect, GetHapticsInterfaceAddress, SetNovintPure, PostEvent, AnalogCodeToString, StringToAnalogCode, VirtualKeyToButtonCode, ButtonCodeToVirtualKey, ScanCodeToButtonCode, Shutdown, ~CInputSystem (D1, D0), CUtlMemory<InputEvent_t>::Grow, interface factory, ___clang_call_terminate | identical | |
| CInputSystem::CInputSystem | edited | retail member order; no `-nosteamcontroller`/CSteamAPIContext; adds `m_nJoystickBaseline = 0`; Steam bools not initialised |
| CInputSystem::~CInputSystem (D2) | edited | callback member unregisters; no CSteamAPIContext |
| CInputSystem::Init | edited | no SteamAPI_InitSafe/context; clears the configuration-revision members; in the joystick block calls `InitializeSteamControllers()` and arms `FindNewDevices` (+5 s) |
| CInputSystem::PostButtonPressedEvent | edited | sets `m_eActiveInputType` from the virtual code: key → mouse/keyboard, Steam Controller → Steam Input, joystick range → XInput |
| CInputSystem::UpdateMouseButtonState | edited | press sets mouse/keyboard input type; moved after PollInputState_Platform (retail order) |
| CInputSystem::PollInputState | edited | calls virtual `FindNewDevices()` before SampleDevices |
| CInputSystem::SetRumble | edited | Steam Input rumble when `IsSteamControllerActive()`, else SDL |
| CInputSystem::StopRumble | edited | calls SetXDeviceRumble directly |
| `__GLOBAL__I_a` (inputsystem.o) | edited | `joy_xcontroller_found` is FCVAR_NONE, not FCVAR_HIDDEN |
| IsSteamControllerConnected, GetControllerTypeForSlot, SetActiveInputType, GetActiveSteamInputHandle | edited | defined in the class (retail emits them with the inline group) |
| AttachToWindow, Connect, DetachFromWindow, EnableInput, EnableMessagePump, EnableJoystickInput, EnableJoystickDiagonalPOV, Get* accessors, IsButtonDown, PollInputState_Platform, PostButtonReleasedEvent, PostUserEvent, ResetInputState, SampleDevices, SetConsoleTextMode, SetCursorPosition, SetPrimaryUserId, SleepUntilInput, GetRawMouseAccumulators, ButtonCodeToString, StringToButtonCode, UpdateMousePositionState | edited | layout |
| CInputSystem::FindNewDevices | written new | virtual slot 58; every 5 s re-runs InitializeSteamControllers |
| CInputSystem::SteamControllerSkipVideo | written new | was a stub; polls Steam Input, A/B/X/Y skip, Start/Select escape |
| CCallbackImpl<32> (4), CCallbackInternal_OnSteamInputConfigurationLoaded (3) | written new | from `STEAM_CALLBACK` (Steamworks `steam_api_internal.h`) |
| CInputSystem::InitializeJoysticks | edited | returns on Steam Deck unless `-forcejoy` (tier1 `IsSteamDeck`), under `#ifdef BYTEMATCH_RETAIL` because stub_steam has no Steam Input to replace SDL there; "no joysticks" is `DevMsg( 2, ... )` |
| JoystickHotplugAdded/Removed, JoystickAxisMotion, JoystickSDLWatcher, ShutdownJoysticks, SetXDeviceRumble, joy_gamecontroller_config_changed_f | edited | layout; the config callback moved after ShutdownJoysticks (retail function order) |
| PollJoystick, SearchForDevice, SetJoyXControllerFound, joy_active_changed_f, `__GLOBAL__I_a` (joystick_sdl.o) | identical | |
| ButtonCode_ButtonCodeToString | edited | Steam Controller codes look up the new `s_pSControllerButtonCodeNames` (one controller's names) |
| ButtonCode_InitKeyTranslationTable | edited | Steam key table filled before the reverse virtual-key table |
| other key_translation functions (9) | identical | |
| GetNumSteamControllersConnected, GetRadialMenuStickValues, GetSteamControllerIndexForSlot, GetActionSetHandle, `__GLOBAL__I_a` (steamcontroller.o) | identical / layout | |
| IsSteamControllerActive, IsXinputControllerActive, IsMouseKeyboardActive | edited | compare `m_eActiveInputType` (2 / 1 / 0) |
| InitDigitalMenuActions (static) | written new | action set, digital and analog handles via `SteamInput()`; the analog handle comes from `GetDigitalActionHandle` in retail too; true if GameControls and MenuControls handles exist |
| CInputSystem::OnSteamInputConfigurationLoaded | written new | re-fetches handles when device/revision changes |
| CInputSystem::PollSteamControllers | edited | ISteamInput rewrite: drops a vanished active handle, one-time menu action-set activation (`g_bInitialized`), 12 digital menu actions, RadialMenuStick analog |
| CInputSystem::InitializeSteamControllers | edited | `SteamAPI_Init()`, `SteamInput()->Init( true )`, manifest once, false when nothing is connected |
| CInputSystem::BLoadOnDiscManifest (static member) | written new | `<cwd>\steam_input\action_manifest_hl2.vdf` via `SetInputActionManifestFilePath` (called twice, as retail does); branch nesting and the manifest-name local follow retail's string order |
| CInputSystem::ActivateSteamControllerActionSetForSlot | edited | ISteamInput; per-slot activation only when the set changed, otherwise all controllers |
| CInputSystem::SetSteamInputRumble | written new | TriggerVibration on the active handle, or slot/all controllers |
| CInputSystem::GetActionOriginForSlotAndCode | written new | was a stub; button code → menu action (indices as in retail: LB → menu_rb, Select → pause_menu, Start → menu_lb) → GetDigitalActionOrigins |
| CInputSystem::PulseHapticOnSteamController | edited | `Legacy_TriggerHapticPulse` |
| SteamInternal_Init_SteamInput | written new | Steamworks header accessor |

Removed (old ISteamController API, not in retail): SteamControllerInterface,
SKeyToButtonCode, GetActionSetHandle(const char*), both
GetSteamControllerActionOrigin overloads, the font-character/description origin
maps, InitializeSteamControllerActionSets, GetControllerStateForSlot,
ReadSteamController, PostKeyEvent, GetSteamPadDeadZone, the CSteamAPIContext
member and `-nosteamcontroller`. Functions retail inlines everywhere
(ClearInputState, CopyInputState, ReleaseAllButtons, ZeroAnalogState,
ComputeSampleTick, JoystickButtonPress/Release, MapCocoaVirtualKeyToButtonCode,
ButtonCode_SKeyToButtonCode...) stay; their out-of-line copies are dead-stripped.

Fork-only: touch input (`touch_sdl.cpp`, members, Init call) and NULL-`SteamInput()`
guards in SetSteamInputRumble/GetActionOriginForSlotAndCode for stub_steam are
under `#ifndef BYTEMATCH_RETAIL` (padded).

Shared-header shim: retail's `GameActionSet_t` is GameControls 0,
VehicleControls 1, MenuControls 2. Until `public/inputsystem/InputEnums.h` has
those values, `inputsystem.h` defines `GAME_ACTION_SET_GAMECONTROLS`,
`GAME_ACTION_SET_VEHICLECONTROLS` and `GAME_ACTION_SET_MENUCONTROLS` as macros.
