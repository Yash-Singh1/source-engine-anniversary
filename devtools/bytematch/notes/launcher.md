# launcher

Retail: depot 223 `bin/launcher.dylib`, own objects `filesystem_init.o`,
`launcher.o`, `reslistgenerator.o` (84 functions), then `libtier2.a(tier2.o)`,
tier1 members, `libappframework.a` (see appframework.md) and
`libtier3.a(tier3.o)`.

Verification: clang-600.0.57 with the TARGET.md Release flags plus the VPC
defines (`VPC`, `USE_SDL`, `DEV_BUILD`, `GL_GLEXT_PROTOTYPES`,
`DX_TO_GL_ABSTRACTION`, `QUICKTIME_VIDEO`, `FORCE_QUICKTIME`,
`DLLNAME=launcher`, `LAUNCHERONLY`), compared per N_OSO object with
`devtools/bytematch` (exact or matches-after-masking) and the lane's strict
script. "Layer" means compiled with mod-small's hdrfix plus this lane's
hdrfix2 (byte-match-survey/lane-mod-launcher-scratch/hdrfix2.patch).

| object | functions | before, today's headers | after, today's headers | after, with layer |
|---|---:|---:|---:|---:|
| filesystem_init | 20 | 11 | 15 | 20 |
| launcher | 39 | 11 | 11 | 39 |
| reslistgenerator | 25 | 5 | 5 | 25 |

Fork-only behaviour that retail lacks is kept for waf builds under
`#ifndef VPC`. `VPC` is defined by Valve's project scripts and by the
`--retail-osx32` mode, never by the normal waf build.

| function | status | note |
|---|---|---|
| FileSystem_GetFileSystemDLLName | edited | retail tries `filesystem_stdio`, then `filesystem_steam` with `bSteam = true`; the fork's `libfilesystem_stdio`-first lookup stays for `!VPC` (waf names dylibs `lib<name>`) |
| SetupSteamStartupEnvironment | written new | SDK 2013 body restored with `SetSteamInstallPath` (inert on POSIX) |
| FileSystem_SetupSteamEnvironment | edited | SDK 2013 `m_bSteam` tools-mode / `m_bSetSteamDLLPath` block restored |
| SetSteamInstallPath, FileSystem_SetupSteamInstallPath | written new | SDK 2013; inlined or dead-stripped in retail launcher, present in vtex_dll |
| CTempEnvVar::CTempEnvVar | edited | `strlen` (SDK 2013) instead of `Q_strlen` |
| FileSystem_LoadSearchPaths | edited | fork `EXTRAS_VPK_PATH` and BaseDir `Msg` under `!VPC`; not in retail launcher (dead-stripped) |
| LocateGameInfoFile, SetupFileSystemError, FileSystem_GetExecutableDir | identical | need SDK 2013 `ICommandLine` (no `HasParm` at slot 5) |
| CSourceAppSystemGroup::Create | edited | `SetShaderAPI( NULL )` (retail materialsystem picks shaderapiempty for `-noshaderapi`); vstdlib/IProcessUtils load for `-tools` moved under `STAGING_ONLY`; needs `VMaterialSystem080` |
| CSourceAppSystemGroup::PreInit | edited | fork DebugLogger block gone (removed upstream in the tier0 lane merge) |
| _LauncherMain | edited | dropped the leak's `-default` dxlevel block; fork `-multirun` and SDL version `Msg` under `!VPC`; Windows `-multirun` back to the leak's local check. Matches with layer (`CUtlDict::Purge` from utldict.h) |
| GrabSourceMutex | edited | `%lu` in the OSX lock path; fork `g_MultiRun` early-out under `!VPC` |
| ReleaseSourceMutex | edited | fork `g_MultiRun` early-out under `!VPC`; inlined in retail |
| CUtlDict<int,unsigned short>::Purge | identical | needs utldict.h `Purge` that frees keys and calls `m_Elements.Purge()` |
| CResListGenerator::InitCommandFile | identical | needs SDK 2013 `clamp` in mathlib.h |
| CUtlRBTree / CUtlString / CUtlVector instantiations (launcher.o, reslistgenerator.o) | identical | need SDK 2013 utlrbtree.h (`m_LessFunc` first) and CUtlString on CUtlBinaryBlock (tier1 lane) |
| all other launcher functions | identical | byte with layer |
| MessageBox, InitTextMode, GetBaseDirectory, UTIL_ComputeBaseDir, DetermineDefaultMod/Game, TryToLoadSteamOverlayDLL, ... | identical | inlined in retail; the out-of-line copies are dead-stripped, not ours-only |

Link composition: retail links no `memoverride.o`; our wscript adds
`public/tier0/memoverride.cpp`. Retail library order is tier2, tier1,
appframework, tier3 (ours tier1, tier2, tier3, appframework). Retail load
commands include SDL2, steam_api, tier0, togl, Carbon, AppKit, OpenGL and
IOKit; our wscript also lists `UNWIND`.

## Lane residuals3

361/361, and byte-identical through the identity flow (identity/identical.json). See
appframework.md for sdlmgr/glmrendererinfo_osx.
