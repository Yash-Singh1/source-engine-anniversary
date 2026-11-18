# ServerBrowser (bin/ServerBrowser) per-function status

Measured like `gameui.md`. Own objects: **667 / 667** after lane `bytematch-gameui3` (whole dylib 3380 / 3380); 665 after `bytematch-gameui2`, 519 / 667 originally.

## Edited

| object | change |
|---|---|
| BaseGamesPage, CustomGames, ServerBrowser | the leak's TF2 workshop-map filter is not in retail (no "workshop" strings in the retail dylib): `CheckWorkshopFilter` virtual, filter combo, `CServerBrowser` subscription list gated out of retail mode. +70 functions |
| QuickListPanel | `m_szMapName[64]` in retail mode |
| DialogGameInfo, BaseGamesPage | (gameui2) no signed-byte workaround or SourceTV connection-port preservation in retail; `ServerResponded` compares connection ports; `GetRegionCodeToFilter` -1; `sb_mod_suggested_maxplayers` without `FCVAR_HIDDEN` |
| CustomGames | (gameui2) `OnSaveFilter` replaces an earlier `gametype` filter |
| BaseGamesPage | (gameui3) `MAX_MAP_NAME` is 32 in retail mode (the leak redefines it as 128 locally); region filter only for codes > 0; `UpdateFilterSettings` adds a `gametype` / `notags` filter (key saved and sent to the master server) |

Nothing left.
