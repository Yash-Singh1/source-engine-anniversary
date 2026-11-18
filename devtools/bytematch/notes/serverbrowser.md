# ServerBrowser (bin/ServerBrowser) per-function status

Measured like `gameui.md`. Own objects: 665 / 667 after lane `bytematch-gameui2` (659 at its start), up from 519 / 667.

## Edited

| object | change |
|---|---|
| BaseGamesPage, CustomGames, ServerBrowser | the leak's TF2 workshop-map filter is not in retail (no "workshop" strings in the retail dylib): `CheckWorkshopFilter` virtual, filter combo, `CServerBrowser` subscription list gated out of retail mode. +70 functions |
| QuickListPanel | `m_szMapName[64]` in retail mode |
| DialogGameInfo, BaseGamesPage | (gameui2) no signed-byte workaround or SourceTV connection-port preservation in retail; `ServerResponded` compares connection ports; `GetRegionCodeToFilter` -1; `sb_mod_suggested_maxplayers` without `FCVAR_HIDDEN` |
| CustomGames | (gameui2) `OnSaveFilter` replaces an earlier `gametype` filter |

## Not matching yet

### BaseGamesPage (2 left)

| function | status | note |
|---|---|---|
| `CBaseGamesPage::PrepareQuickListMap(char const*, int)` | differs | bytes |
| `CBaseGamesPage::UpdateFilterSettings()` | differs | size retail 3333 vs ours 3055 |

