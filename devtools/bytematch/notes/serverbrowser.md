# ServerBrowser (bin/ServerBrowser) per-function status

Measured like `gameui.md`. Own objects: 659 / 667 after merging `bytematch` 26995eb0 (589 before), up from 519 / 667. The table below is from the pre-merge run.

## Edited

| object | change |
|---|---|
| BaseGamesPage, CustomGames, ServerBrowser | the leak's TF2 workshop-map filter is not in retail (no "workshop" strings in the retail dylib): `CheckWorkshopFilter` virtual, filter combo, `CServerBrowser` subscription list gated out of retail mode. +70 functions |
| QuickListPanel | `m_szMapName[64]` in retail mode |

## Not matching yet

34 of the remaining 78 call newer Steamworks accessors (`SteamInternal_ContextInit`, `CCallback` impl);
most others shift with vgui_controls vtable/offset differences.

### BaseGamesPage (30 left) — newer Steamworks accessors/CCallback, vgui_controls offsets

| function | status | note |
|---|---|---|
| `CBaseGamesPage::OnFavoritesMsg(FavoritesListChanged_t*)` | differs | size retail 591 vs ours 573 |
| `CBaseGamesPage::~CBaseGamesPage()` | differs | size retail 1793 vs ours 1783 |
| `CBaseGamesPage::PerformLayout()` | differs | size retail 661 vs ours 643 |
| `CBaseGamesPage::PrepareQuickListMap(char const*, int)` | differs | bytes |
| `CBaseGamesPage::GetServer(unsigned int)` | differs | size retail 84 vs ours 66 |
| `non-virtual thunk to CBaseGamesPage::GetServer(unsigned int)` | differs | size retail 84 vs ours 66 |
| `CBaseGamesPage::ServerResponded(void*, int)` | differs | size retail 85 vs ours 117 |
| `non-virtual thunk to CBaseGamesPage::ServerResponded(void*, int)` | differs | size retail 90 vs ours 122 |
| `CBaseGamesPage::ServerResponded(int, gameserveritem_t*)` | differs | reloc target at +0x4b6: ours _Z10V_snpri |
| `CBaseGamesPage::ApplyGameFilters()` | differs | size retail 1757 vs ours 1749 |
| `CBaseGamesPage::UpdateFilterSettings()` | differs | size retail 3333 vs ours 3055 |
| `CBaseGamesPage::OnCommand(char const*)` | differs | size retail 246 vs ours 232 |
| `CBaseGamesPage::OnKeyCodePressed(ButtonCode_t)` | differs | size retail 146 vs ours 179 |
| `CBaseGamesPage::OnAddToFavorites()` | differs | size retail 209 vs ours 186 |
| `CBaseGamesPage::OnAddToBlacklist()` | differs | size retail 222 vs ours 199 |
| `CBaseGamesPage::OnRefreshServer(int)` | differs | size retail 215 vs ours 197 |
| `CBaseGamesPage::StartRefresh()` | differs | size retail 608 vs ours 566 |
| `CBaseGamesPage::StopRefresh()` | differs | size retail 114 vs ours 98 |
| `non-virtual thunk to CBaseGamesPage::StopRefresh()` | differs | size retail 136 vs ours 120 |
| `CBaseGamesPage::IsRefreshing()` | differs | size retail 78 vs ours 60 |
| `non-virtual thunk to CBaseGamesPage::IsRefreshing()` | differs | size retail 78 vs ours 60 |
| `CCallback<CBaseGamesPage, FavoritesListChanged_t, false>::~CCallback()` | missing |  |
| `vgui::CheckButton::IsCheckButtonCheckable() const` | differs | bytes |
| `CBaseGamesPage::GetRegionCodeToFilter()` | differs | bytes |
| `CCallback<CBaseGamesPage, FavoritesListChanged_t, false>::~CCallback()` | missing |  |
| `CCallbackImpl<28>::~CCallbackImpl()` | missing |  |
| `CCallbackImpl<28>::~CCallbackImpl()` | missing |  |
| `SteamInternal_Init_SteamMatchmaking(ISteamMatchmaking**)` | missing |  |
| `SteamInternal_Init_SteamMatchmakingServers(ISteamMatchmakingServers**)` | missing |  |
| `__GLOBAL__I_a` | differs | bytes |

### DialogGameInfo (21 left) — anniversary rewrite + Steamworks CCallback

| function | status | note |
|---|---|---|
| `CDialogGameInfo::RequestInfo()` | missing |  |
| `CDialogGameInfo::CDialogGameInfo(vgui::Panel*, int, int, unsigned short, char const*)` | differs | size retail 3059 vs ours 3182 |
| `CDialogGameInfo::~CDialogGameInfo()` | differs | size retail 390 vs ours 353 |
| `CDialogGameInfo::SendPlayerQuery(unsigned int, unsigned short)` | differs | size retail 141 vs ours 118 |
| `CDialogGameInfo::Run(char const*)` | differs | size retail 100 vs ours 220 |
| `CDialogGameInfo::ChangeGame(int, int, unsigned short)` | differs | size retail 337 vs ours 439 |
| `CDialogGameInfo::SetFriend(unsigned long long)` | differs | size retail 357 vs ours 347 |
| `CDialogGameInfo::PerformLayout()` | differs | reloc target at +0xc6: ours _Z10V_snprin |
| `CDialogGameInfo::OnKeyCodePressed(ButtonCode_t)` | differs | size retail 51 vs ours 61 |
| `CDialogGameInfo::OnConnect()` | differs | size retail 78 vs ours 203 |
| `CDialogGameInfo::OnRefresh()` | differs | size retail 22 vs ours 155 |
| `CDialogGameInfo::OnTick()` | differs | size retail 75 vs ours 197 |
| `CDialogGameInfo::ServerResponded(gameserveritem_t&)` | differs | size retail 312 vs ours 371 |
| `CDialogGameInfo::ConnectToServer()` | differs | reloc target at +0x2df: ours _Z10V_snpri |
| `CDialogGameInfo::ApplyConnectCommand(gameserveritem_t const&)` | differs | reloc target at +0xe7: ours _Z10V_snprin |
| `CDialogGameInfo::ConstructConnectArgs(char*, int, gameserveritem_t const&)` | differs | reloc target at +0x6f: ours _Z10V_snprin |
| `CCallback<CDialogGameInfo, PersonaStateChange_t, false>::~CCallback()` | missing |  |
| `CCallback<CDialogGameInfo, PersonaStateChange_t, false>::~CCallback()` | missing |  |
| `CCallbackImpl<12>::~CCallbackImpl()` | missing |  |
| `CCallbackImpl<12>::~CCallbackImpl()` | missing |  |
| `SteamInternal_Init_SteamFriends(ISteamFriends**)` | missing |  |

### DialogAddServer (5 left)

| function | status | note |
|---|---|---|
| `CDialogAddServer::CDialogAddServer(vgui::Panel*, IGameList*)` | differs | bytes |
| `CDialogAddServer::CDialogAddServer(vgui::Panel*, IGameList*)` | differs | bytes |
| `CDialogAddServer::~CDialogAddServer()` | differs | size retail 637 vs ours 625 |
| `CDialogAddServer::TestServers()` | differs | size retail 1554 vs ours 1535 |
| `CDialogAddServer::ServerResponded(gameserveritem_t&)` | differs | reloc target at +0xbe: ours _Z10V_snprin |

### FavoriteGames (5 left)

| function | status | note |
|---|---|---|
| `CFavoriteGames::LoadFavoritesList()` | differs | size retail 142 vs ours 125 |
| `CFavoriteGames::RefreshComplete(void*, EMatchMakingServerResponse)` | differs | size retail 149 vs ours 133 |
| `non-virtual thunk to CFavoriteGames::RefreshComplete(void*, EMatchMakingServerResponse)` | differs | size retail 41 vs ours 154 |
| `CFavoriteGames::OnRemoveFromFavorites()` | differs | size retail 351 vs ours 307 |
| `CFavoriteGames::OnAddCurrentServer()` | differs | size retail 451 vs ours 437 |

### ServerBrowserDialog (5 left)

| function | status | note |
|---|---|---|
| `CServerBrowserDialog::CServerBrowserDialog(vgui::Panel*)` | differs | bytes |
| `CServerBrowserDialog::AddServerToFavorites(gameserveritem_t&)` | differs | size retail 423 vs ours 415 |
| `CServerBrowserDialog::OpenGameInfoDialog(IGameList*, unsigned int)` | differs | reloc target at +0x12e: ours _Z10V_snpri |
| `CServerBrowserDialog::OnConnectToGame(KeyValues*)` | differs | size retail 872 vs ours 856 |
| `CServerBrowserDialog::OnKeyCodePressed(ButtonCode_t)` | differs | size retail 529 vs ours 551 |

### ServerBrowser (3 left)

| function | status | note |
|---|---|---|
| `CServerBrowser::Initialize(void* (**)(char const*, int*), int)` | differs | size retail 357 vs ours 387 |
| `CServerBrowser::OpenGameInfoDialog(unsigned long long, char const*)` | differs | size retail 280 vs ours 273 |
| `__GLOBAL__I_a` | differs | size retail 397 vs ours 587 |

### blacklisted_server_manager (2 left)

| function | status | note |
|---|---|---|
| `CBlacklistedServerManager::AddServer(gameserveritem_t&)` | differs | reloc target at +0xea: ours _Z10V_snprin |
| `CBlacklistedServerManager::IsServerBlacklisted(gameserveritem_t const&) const` | differs | reloc target at +0x84: ours _Z10V_snprin |

### InternetGames (2 left)

| function | status | note |
|---|---|---|
| `CInternetGames::ServerFailedToRespond(void*, int)` | differs | size retail 181 vs ours 176 |
| `non-virtual thunk to CInternetGames::ServerFailedToRespond(void*, int)` | differs | size retail 41 vs ours 185 |

### BlacklistedServers (1 left)

| function | status | note |
|---|---|---|
| `CBlacklistedServers::OnAddCurrentServer()` | differs | reloc target at +0x121: ours _Z10V_snpri |

### CustomGames (1 left)

| function | status | note |
|---|---|---|
| `CCustomGames::OnSaveFilter(KeyValues*)` | differs | size retail 822 vs ours 622 |

### HistoryGames (1 left)

| function | status | note |
|---|---|---|
| `CHistoryGames::OnRemoveFromHistory()` | differs | size retail 319 vs ours 288 |

### QuickListPanel (1 left)

| function | status | note |
|---|---|---|
| `CQuickListPanel::CQuickListPanel(vgui::Panel*, char const*)` | differs | bytes |

### ServerListCompare (1 left)

| function | status | note |
|---|---|---|
| `ServerNameCompare(vgui::ListPanel*, vgui::ListPanelItem const&, vgui::ListPanelItem const&)` | differs | reloc target at +0xdd: ours _Z10V_snprin |
