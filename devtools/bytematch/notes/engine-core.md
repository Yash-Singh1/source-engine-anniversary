# engine (engine-core lane)

Retail: depot 223 `bin/engine.dylib`. Scope here: the engine-core file set
(host*, sv_*, net*, baseclient*/baseserver*, hltv*, replay (engine side), sys_*,
cmd*, cvar*, audio, pure_server, dt_send/dt_encode, saverestore, mod_*, sound*,
voice*, matchmaking and everything not owned by engine-render). Engine-render
objects that gained only because of shared engine headers changed here are
listed too.

Verification: build-retail objects (clang-600.0.57, `--retail-osx32`) compared
per N_OSO object with `bytematch report` / `bytematch diff`; "yes" = exact or
matches-after-masking. Columns: **head** = `bytematch` 7906c3eb, **branch** =
this branch, **+ovl** = this branch plus the three shared-header overlays
listed in the lane report (`MAX_MAP_NAME` 32, `CanCheat` with
`Cmd_IsRptActive`, SDK 2013 `CClassMemoryPool::Clear`). Patches and tools:
/Volumes/Storage/sco/byte-match-survey/lane-engine-core-scratch/.

Totals (engine objects, 8702 functions): head 7549, branch 7838, +ovl 7938.
Whole engine.dylib (branch): 11874/12986 functions, 79.75% of bytes.
No function matched at head is lost on the branch.

## Per object (objects whose count changed)

| object | head | branch | +ovl | total | cause |
|---|---:|---:|---:|---:|---|
| EngineSoundClient | 33 | 34 | 34 | 34 | CBaseClientState layout |
| EngineSoundServer | 26 | 28 | 28 | 28 | edict layout |
| LocalNetworkBackdoor | 8 | 12 | 12 | 12 | CBaseClientState layout |
| ModelInfo | 82 | 84 | 84 | 95 | CBaseClientState/edict layout |
| OcclusionSystem | 41 | 42 | 42 | 42 | CBaseClientState layout |
| baseclient | 103 | 104 | 104 | 118 | UpdateUserSettings |
| baseclientstate | 63 | 77 | 77 | 92 | CBaseClientState layout |
| bugreporter | 78 | 82 | 82 | 88 | CBaseClientState layout |
| cdll_engine_int | 143 | 153 | 153 | 164 | CBaseClientState layout, OnHL2ExpansionLoaded |
| cl_demo | 58 | 60 | 60 | 78 | CBaseClientState layout |
| cl_ents_parse | 6 | 9 | 9 | 10 | CBaseClientState layout |
| cl_parse_event | 3 | 4 | 4 | 4 | CBaseClientState layout |
| cl_pred | 0 | 1 | 1 | 1 | CBaseClientState layout |
| cl_rcon | 36 | 37 | 37 | 47 | CBaseClientState layout |
| client | 20 | 38 | 38 | 55 | CBaseClientState layout |
| clockdriftmgr | 7 | 8 | 8 | 8 | CBaseClientState layout |
| console | 8 | 9 | 9 | 30 | CBaseClientState layout |
| cvar | 31 | 31 | 33 | 35 | shared header overlay |
| disp | 31 | 31 | 33 | 33 | shared header overlay |
| disp_interface | 57 | 57 | 58 | 60 | shared header overlay |
| dt_common_eng | 19 | 21 | 21 | 21 | CBaseClientState layout |
| engine_replay_int | 51 | 53 | 53 | 56 | CBaseClientState layout |
| enginetool | 91 | 92 | 92 | 95 | CBaseClientState / edicts layout |
| enginetrace | 41 | 43 | 43 | 44 | edict layout |
| gl_rlight | 12 | 13 | 13 | 13 | CBaseClientState layout |
| gl_rsurf | 93 | 93 | 95 | 97 | shared header overlay |
| hltvclient | 54 | 55 | 55 | 55 | CBaseClientState layout |
| hltvclientstate | 27 | 61 | 61 | 66 | CBaseClientState layout |
| hltvdemo | 21 | 22 | 22 | 23 | CBaseClientState layout |
| hltvserver | 106 | 159 | 159 | 164 | CBaseClientState layout |
| host | 56 | 62 | 64 | 88 | Host_RunFrame/_Host_RunFrame, small host fns |
| host_cmd | 41 | 46 | 46 | 64 | status, edicts |
| host_listmaps | 18 | 19 | 19 | 29 | Host_MapListFlush |
| host_saverestore | 135 | 137 | 137 | 146 | edict/client layout |
| l_studio | 87 | 87 | 88 | 95 | shared header overlay |
| lightcache | 30 | 30 | 31 | 33 | shared header overlay |
| matchmakingclient | 6 | 6 | 16 | 16 | shared header overlay |
| matchmakinghost | 2 | 2 | 20 | 20 | shared header overlay |
| matchmakingmigrate | 2 | 2 | 4 | 4 | shared header overlay |
| matchmakingshared | 91 | 91 | 128 | 129 | shared header overlay |
| modelloader | 112 | 113 | 113 | 169 | CBaseClientState layout |
| net_chan | 88 | 96 | 96 | 98 | retail CNetChan |
| networkstringtableclient | 2 | 3 | 3 | 3 | CBaseClientState layout |
| perfuipanel | 58 | 58 | 60 | 61 | shared header overlay |
| pr_edict | 1 | 9 | 9 | 10 | retail ED_Alloc/ED_Free/ED_ClearEdict |
| r_areaportal | 11 | 13 | 13 | 13 | CBaseClientState layout |
| r_decal | 67 | 67 | 72 | 74 | shared header overlay |
| servermsghandler | 35 | 43 | 43 | 50 | CBaseClientState layout |
| snd_dma | 122 | 125 | 137 | 141 | S_ClearSoundPool; CClassMemoryPool overlay |
| snd_mix | 48 | 49 | 49 | 52 | commentary ducking |
| snd_mp3_source | 52 | 61 | 61 | 61 | pre-2016 MP3 source |
| snd_wave_mixer_mp3 | 3 | 14 | 14 | 14 | pre-2016 MP3 mixer |
| snd_wave_source | 122 | 123 | 123 | 133 | MP3 source |
| sv_client | 80 | 82 | 83 | 86 | edict layout |
| sv_framesnapshot | 24 | 25 | 25 | 26 | edict layout |
| sv_log | 11 | 22 | 22 | 22 | retail CLog |
| sv_main | 46 | 57 | 57 | 71 | edict allocator (no free_edicts) |
| sv_packedentities | 15 | 20 | 20 | 20 | edict allocator layout |
| sv_precache | 7 | 23 | 23 | 23 | edict layout |
| sys_engine | 10 | 10 | 11 | 15 | shared header overlay |
| vengineserver_impl | 114 | 121 | 121 | 127 | edict allocator layout |
| vgui_budgetpanel | 12 | 12 | 13 | 13 | shared header overlay |
| view | 61 | 62 | 62 | 63 | CBaseClientState layout |
| voice_record_mac_audioqueue | 12 | 16 | 16 | 16 | Init() |

## Functions now matching (branch)

| object | function | status | note |
|---|---|---|---|
| EngineSoundClient | `CEngineSoundClient::EmitAmbientSound(char const*, float, int, int, float)` | yes | CBaseClientState layout |
| EngineSoundServer | `CEngineSoundServer::EmitSentenceByIndex(IRecipientFilter&, int, int, int, float, soundlevel_t, int, int, int, Vector const*, Vector const*, CUtlVector<Vector, CUtlMemory<Vector, int>>*, bool, float, int)` | yes | edict layout |
| EngineSoundServer | `CEngineSoundServer::EmitSound(IRecipientFilter&, int, int, char const*, float, soundlevel_t, int, int, int, Vector const*, Vector const*, CUtlVector<Vector, CUtlMemory<Vector, int>>*, bool, float, int)` | yes | edict layout |
| LocalNetworkBackdoor | `CLocalNetworkBackdoor::InitFastCopy()` | yes | CBaseClientState layout |
| LocalNetworkBackdoor | `CLocalNetworkBackdoor::ProcessDormantEntities()` | yes | CBaseClientState layout |
| LocalNetworkBackdoor | `CLocalNetworkBackdoor::AddToPendingDormantEntityList(unsigned short)` | yes | CBaseClientState layout |
| LocalNetworkBackdoor | `CLocalNetworkBackdoor::EntState(int, int, int, SendTable const*, void const*, bool, bool)` | yes | CBaseClientState layout |
| ModelInfo | `CModelInfoClient::GetDynamicModelStringTable() const` | yes | CBaseClientState/edict layout |
| ModelInfo | `CModelInfoServer::GetDynamicModelStringTable() const` | yes | CBaseClientState/edict layout |
| OcclusionSystem | `COcclusionSystem::RecomputeOccluderEdgeList()` | yes | CBaseClientState layout |
| baseclient | `CBaseClient::UpdateUserSettings()` | yes | UpdateUserSettings |
| baseclientstate | `CBaseClientState::LinkClasses()` | yes | CBaseClientState layout |
| baseclientstate | `CBaseClientState::ProcessTick(NET_Tick*)` | yes | CBaseClientState layout |
| baseclientstate | `CBaseClientState::GetClientClass(int)` | yes | CBaseClientState layout |
| baseclientstate | `CBaseClientState::ForceFullUpdate()` | yes | CBaseClientState layout |
| baseclientstate | `CBaseClientState::GetClassBaseline(int, void const**, int*)` | yes | CBaseClientState layout |
| baseclientstate | `CBaseClientState::ProcessClassInfo(SVC_ClassInfo*)` | yes | CBaseClientState layout |
| baseclientstate | `CBaseClientState::GetEntityBaseline(int, int)` | yes | CBaseClientState layout |
| baseclientstate | `CBaseClientState::ProcessServerInfo(SVC_ServerInfo*)` | yes | CBaseClientState layout |
| baseclientstate | `CBaseClientState::SetEntityBaseline(int, ClientClass*, int, char*, int)` | yes | CBaseClientState layout |
| baseclientstate | `CBaseClientState::CopyEntityBaseline(int, int)` | yes | CBaseClientState layout |
| baseclientstate | `CBaseClientState::ProcessUpdateStringTable(SVC_UpdateStringTable*)` | yes | CBaseClientState layout |
| baseclientstate | `CBaseClientState::Clear()` | yes | CBaseClientState layout |
| baseclientstate | `CBaseClientState::CBaseClientState()` | yes | CBaseClientState layout |
| baseclientstate | `CBaseClientState::GetStringTable(char const*) const` | yes | CBaseClientState layout |
| bugreporter | `CBugUIPanel::OnChooseArea(vgui::Panel*)` | yes | CBaseClientState layout |
| bugreporter | `CBugUIPanel::Activate()` | yes | CBaseClientState layout |
| bugreporter | `CBugUIPanel::OnSubmit()` | yes | CBaseClientState layout |
| bugreporter | `CEngineBugReporter::ShouldPause() const` | yes | CBaseClientState layout |
| cdll_engine_int | `CEngineClient::GetMaxClients()` | yes | CBaseClientState layout, OnHL2ExpansionLoaded |
| cdll_engine_int | `CEngineClient::GetViewAngles(QAngle&)` | yes | CBaseClientState layout, OnHL2ExpansionLoaded |
| cdll_engine_int | `CEngineClient::SetViewAngles(QAngle&)` | yes | CBaseClientState layout, OnHL2ExpansionLoaded |
| cdll_engine_int | `CEngineClient::GetLastTimeStamp()` | yes | CBaseClientState layout, OnHL2ExpansionLoaded |
| cdll_engine_int | `CEngineClient::OnHL2ExpansionLoaded()` | yes | CBaseClientState layout, OnHL2ExpansionLoaded |
| cdll_engine_int | `CEngineClient::IsLevelMainMenuBackground()` | yes | CBaseClientState layout, OnHL2ExpansionLoaded |
| cdll_engine_int | `CEngineClient::SetRestrictClientCommands(bool)` | yes | CBaseClientState layout, OnHL2ExpansionLoaded |
| cdll_engine_int | `CEngineClient::SetRestrictServerCommands(bool)` | yes | CBaseClientState layout, OnHL2ExpansionLoaded |
| cdll_engine_int | `CEngineClient::IsHLTV()` | yes | CBaseClientState layout, OnHL2ExpansionLoaded |
| cdll_engine_int | `CEngineClient::ClientCmd(char const*)` | yes | CBaseClientState layout, OnHL2ExpansionLoaded |
| cl_demo | `CDemoRecorder::WriteMessages(bf_write&)` | yes | CBaseClientState layout |
| cl_demo | `CDemoRecorder::GetRecordingTick()` | yes | CBaseClientState layout |
| cl_ents_parse | `CL_CopyExistingEntity(CEntityReadInfo&)` | yes | CBaseClientState layout |
| cl_ents_parse | `CL_PreprocessEntities()` | yes | CBaseClientState layout |
| cl_ents_parse | `CL_ProcessPacketEntities(SVC_PacketEntities*)` | yes | CBaseClientState layout |
| cl_parse_event | `CL_FireEvents()` | yes | CBaseClientState layout |
| cl_pred | `CL_RunPrediction(PREDICTION_REASON)` | yes | CBaseClientState layout |
| cl_rcon | `CRConClient::Authenticate()` | yes | CBaseClientState layout |
| client | `CClientState::SetGeneric(int)` | yes | CBaseClientState layout |
| client | `CClientState::ClearSounds()` | yes | CBaseClientState layout |
| client | `CClientState::FullConnect(netadr_s&)` | yes | CBaseClientState layout |
| client | `CClientState::GetSoundName(int)` | yes | CBaseClientState layout |
| client | `CClientState::SetFriendsID(unsigned int, char const*)` | yes | CBaseClientState layout |
| client | `CClientState::AddCustomFile(int, char const*)` | yes | CBaseClientState layout |
| client | `CClientState::ConsistencyCheck(bool)` | yes | CBaseClientState layout |
| client | `CClientState::LookupModelIndex(char const*)` | yes | CBaseClientState layout |
| client | `CClientState::LookupSoundIndex(char const*)` | yes | CBaseClientState layout |
| client | `CClientState::DumpPrecacheStats(char const*)` | yes | CBaseClientState layout |
| client | `CClientState::HookClientStringTable(char const*)` | yes | CBaseClientState layout |
| client | `CClientState::InstallEngineStringTableCallback(char const*)` | yes | CBaseClientState layout |
| client | `CClientState::GetSound(int)` | yes | CBaseClientState layout |
| client | `CClientState::SetDecal(int)` | yes | CBaseClientState layout |
| client | `CClientState::GetFrameTime() const` | yes | CBaseClientState layout |
| client | `CClientState::GetTime() const` | yes | CBaseClientState layout |
| client | `non-virtual thunk to CClientState::~CClientState()` | yes | CBaseClientState layout |
| client | `non-virtual thunk to CClientState::~CClientState()` | yes | CBaseClientState layout |
| clockdriftmgr | `CClockDriftMgr::SetServerTick(int)` | yes | CBaseClientState layout |
| console | `CConPanel::PaintBackground()` | yes | CBaseClientState layout |
| dt_common_eng | `DataTable_ParseClassInfosFromBuffer(CClientState*, bf_read*)` | yes | CBaseClientState layout |
| dt_common_eng | `DataTable_CreateClientClassInfosFromServerClasses(CBaseClientState*)` | yes | CBaseClientState layout |
| engine_replay_int | `CEngineClientReplay::GetLevelNameShort()` | yes | CBaseClientState layout |
| engine_replay_int | `CEngineClientReplay::GetLastServerTickTime()` | yes | CBaseClientState layout |
| enginetool | `CEngineTool::GetMaxClients()` | yes | CBaseClientState / edicts layout |
| enginetrace | `CEngineTraceServer::SetTraceEntity(ICollideable*, CGameTrace*)` | yes | edict layout |
| enginetrace | `CEngineTraceServer::GetWorldCollideable()` | yes | edict layout |
| gl_rlight | `R_AnimateLight()` | yes | CBaseClientState layout |
| hltvclient | `CHLTVClient::SpawnPlayer()` | yes | CBaseClientState layout |
| hltvclientstate | `CHLTVClientState::ProcessMenu(SVC_Menu*)` | yes | CBaseClientState layout |
| hltvclientstate | `CHLTVClientState::UpdateStats()` | yes | CBaseClientState layout |
| hltvclientstate | `CHLTVClientState::CopyNewEntity(CEntityReadInfo&, int, int)` | yes | CBaseClientState layout |
| hltvclientstate | `CHLTVClientState::ProcessSounds(SVC_Sounds*)` | yes | CBaseClientState layout |
| hltvclientstate | `CHLTVClientState::ProcessSetView(SVC_SetView*)` | yes | CBaseClientState layout |
| hltvclientstate | `CHLTVClientState::ProcessBSPDecal(SVC_BSPDecal*)` | yes | CBaseClientState layout |
| hltvclientstate | `CHLTVClientState::ProcessFixAngle(SVC_FixAngle*)` | yes | CBaseClientState layout |
| hltvclientstate | `CHLTVClientState::ProcessPrefetch(SVC_Prefetch*)` | yes | CBaseClientState layout |
| hltvclientstate | `CHLTVClientState::ProcessGameEvent(SVC_GameEvent*)` | yes | CBaseClientState layout |
| hltvclientstate | `CHLTVClientState::ProcessSetConVar(NET_SetConVar*)` | yes | CBaseClientState layout |
| hltvclientstate | `CHLTVClientState::ProcessStringCmd(NET_StringCmd*)` | yes | CBaseClientState layout |
| hltvclientstate | `CHLTVClientState::ProcessVoiceData(SVC_VoiceData*)` | yes | CBaseClientState layout |
| hltvclientstate | `CHLTVClientState::ProcessVoiceInit(SVC_VoiceInit*)` | yes | CBaseClientState layout |
| hltvclientstate | `CHLTVClientState::ProcessServerInfo(SVC_ServerInfo*)` | yes | CBaseClientState layout |
| hltvclientstate | `CHLTVClientState::ProcessUserMessage(SVC_UserMessage*)` | yes | CBaseClientState layout |
| hltvclientstate | `CHLTVClientState::ProcessTempEntities(SVC_TempEntities*)` | yes | CBaseClientState layout |
| hltvclientstate | `CHLTVClientState::ProcessEntityMessage(SVC_EntityMessage*)` | yes | CBaseClientState layout |
| hltvclientstate | `CHLTVClientState::ProcessGameEventList(SVC_GameEventList*)` | yes | CBaseClientState layout |
| hltvclientstate | `CHLTVClientState::HookClientStringTable(char const*)` | yes | CBaseClientState layout |
| hltvclientstate | `CHLTVClientState::ProcessCrosshairAngle(SVC_CrosshairAngle*)` | yes | CBaseClientState layout |
| hltvclientstate | `CHLTVClientState::ProcessPacketEntities(SVC_PacketEntities*)` | yes | CBaseClientState layout |
| hltvclientstate | `CHLTVClientState::InstallStringTableCallback(char const*)` | yes | CBaseClientState layout |
| hltvclientstate | `CHLTVClientState::Clear()` | yes | CBaseClientState layout |
| hltvclientstate | `CHLTVClientState::PacketEnd()` | yes | CBaseClientState layout |
| hltvclientstate | `CHLTVClientState::CHLTVClientState()` | yes | CBaseClientState layout |
| hltvclientstate | `non-virtual thunk to CHLTVClientState::ProcessMenu(SVC_Menu*)` | yes | CBaseClientState layout |
| hltvclientstate | `non-virtual thunk to CHLTVClientState::ProcessSetView(SVC_SetView*)` | yes | CBaseClientState layout |
| hltvclientstate | `non-virtual thunk to CHLTVClientState::ProcessBSPDecal(SVC_BSPDecal*)` | yes | CBaseClientState layout |
| hltvclientstate | `non-virtual thunk to CHLTVClientState::ProcessFixAngle(SVC_FixAngle*)` | yes | CBaseClientState layout |
| hltvclientstate | `non-virtual thunk to CHLTVClientState::ProcessPrefetch(SVC_Prefetch*)` | yes | CBaseClientState layout |
| hltvclientstate | `non-virtual thunk to CHLTVClientState::ProcessSetConVar(NET_SetConVar*)` | yes | CBaseClientState layout |
| hltvclientstate | `non-virtual thunk to CHLTVClientState::ProcessStringCmd(NET_StringCmd*)` | yes | CBaseClientState layout |
| hltvclientstate | `non-virtual thunk to CHLTVClientState::ProcessVoiceInit(SVC_VoiceInit*)` | yes | CBaseClientState layout |
| hltvclientstate | `non-virtual thunk to CHLTVClientState::ProcessCrosshairAngle(SVC_CrosshairAngle*)` | yes | CBaseClientState layout |
| hltvdemo | `CHLTVDemoRecorder::WriteSignonData()` | yes | CBaseClientState layout |
| hltvserver | `EditDemo_f(CCommand const&)` | yes | CBaseClientState layout |
| hltvserver | `tv_stoprecord(CCommand const&)` | yes | CBaseClientState layout |
| hltvserver | `tv_relay(CCommand const&)` | yes | CBaseClientState layout |
| hltvserver | `tv_retry(CCommand const&)` | yes | CBaseClientState layout |
| hltvserver | `tv_record(CCommand const&)` | yes | CBaseClientState layout |
| hltvserver | `tv_status(CCommand const&)` | yes | CBaseClientState layout |
| hltvserver | `CHLTVServer::SendNetMsg(INetMessage&, bool)` | yes | CBaseClientState layout |
| hltvserver | `CHLTVServer::StartRelay()` | yes | CBaseClientState layout |
| hltvserver | `CHLTVServer::UpdateTick()` | yes | CBaseClientState layout |
| hltvserver | `CHLTVServer::Changelevel()` | yes | CBaseClientState layout |
| hltvserver | `CHLTVServer::GetDemoFile()` | yes | CBaseClientState layout |
| hltvserver | `CHLTVServer::GetDirector()` | yes | CBaseClientState layout |
| hltvserver | `CHLTVServer::GetHLTVSlot()` | yes | CBaseClientState layout |
| hltvserver | `CHLTVServer::StartMaster(CGameClient*)` | yes | CBaseClientState layout |
| hltvserver | `CHLTVServer::UpdateStats()` | yes | CBaseClientState layout |
| hltvserver | `CHLTVServer::FireGameEvent(IGameEvent*)` | yes | CBaseClientState layout |
| hltvserver | `CHLTVServer::GetDeltaFrame(int)` | yes | CBaseClientState layout |
| hltvserver | `CHLTVServer::GetOnlineTime()` | yes | CBaseClientState layout |
| hltvserver | `CHLTVServer::GetTotalTicks()` | yes | CBaseClientState layout |
| hltvserver | `CHLTVServer::IsPlayingBack()` | yes | CBaseClientState layout |
| hltvserver | `CHLTVServer::FillServerInfo(SVC_ServerInfo&)` | yes | CBaseClientState layout |
| hltvserver | `CHLTVServer::GetGlobalStats(int&, int&, int&)` | yes | CBaseClientState layout |
| hltvserver | `CHLTVServer::SignonComplete()` | yes | CBaseClientState layout |
| hltvserver | `CHLTVServer::GetPlaybackTick()` | yes | CBaseClientState layout |
| hltvserver | `CHLTVServer::IsPlaybackPaused()` | yes | CBaseClientState layout |
| hltvserver | `CHLTVServer::SendClientMessages(bool)` | yes | CBaseClientState layout |
| hltvserver | `CHLTVServer::InstallStringTables()` | yes | CBaseClientState layout |
| hltvserver | `CHLTVServer::GetPlaybackStartTick()` | yes | CBaseClientState layout |
| hltvserver | `CHLTVServer::GetPlaybackTimeScale()` | yes | CBaseClientState layout |
| hltvserver | `CHLTVServer::InitClientRecvTables()` | yes | CBaseClientState layout |
| hltvserver | `CHLTVServer::SetPlaybackTimeScale(float)` | yes | CBaseClientState layout |
| hltvserver | `CHLTVServer::Init(bool)` | yes | CBaseClientState layout |
| hltvserver | `CHLTVServer::Clear()` | yes | CBaseClientState layout |
| hltvserver | `CHLTVServer::RunFrame()` | yes | CBaseClientState layout |
| hltvserver | `CHLTVServer::Shutdown()` | yes | CBaseClientState layout |
| hltvserver | `CHLTVServer::GetBuffer(int)` | yes | CBaseClientState layout |
| hltvserver | `CHLTVServer::CHLTVServer()` | yes | CBaseClientState layout |
| hltvserver | `CHLTVServer::~CHLTVServer()` | yes | CBaseClientState layout |
| hltvserver | `non-virtual thunk to CHLTVServer::GetDirector()` | yes | CBaseClientState layout |
| hltvserver | `non-virtual thunk to CHLTVServer::GetHLTVSlot()` | yes | CBaseClientState layout |
| hltvserver | `non-virtual thunk to CHLTVServer::GetOnlineTime()` | yes | CBaseClientState layout |
| hltvserver | `non-virtual thunk to CHLTVServer::GetGlobalStats(int&, int&, int&)` | yes | CBaseClientState layout |
| hltvserver | `non-virtual thunk to CHLTVServer::GetDemoFile()` | yes | CBaseClientState layout |
| hltvserver | `non-virtual thunk to CHLTVServer::GetTotalTicks()` | yes | CBaseClientState layout |
| hltvserver | `non-virtual thunk to CHLTVServer::IsPlayingBack()` | yes | CBaseClientState layout |
| hltvserver | `non-virtual thunk to CHLTVServer::GetPlaybackTick()` | yes | CBaseClientState layout |
| hltvserver | `non-virtual thunk to CHLTVServer::IsPlaybackPaused()` | yes | CBaseClientState layout |
| hltvserver | `non-virtual thunk to CHLTVServer::GetPlaybackStartTick()` | yes | CBaseClientState layout |
| hltvserver | `non-virtual thunk to CHLTVServer::GetPlaybackTimeScale()` | yes | CBaseClientState layout |
| hltvserver | `non-virtual thunk to CHLTVServer::SetPlaybackTimeScale(float)` | yes | CBaseClientState layout |
| hltvserver | `non-virtual thunk to CHLTVServer::FillServerInfo(SVC_ServerInfo&)` | yes | CBaseClientState layout |
| hltvserver | `non-virtual thunk to CHLTVServer::SendClientMessages(bool)` | yes | CBaseClientState layout |
| hltvserver | `non-virtual thunk to CHLTVServer::Init(bool)` | yes | CBaseClientState layout |
| host | `_Host_RunFrame(float)` | yes | Host_RunFrame/_Host_RunFrame, small host fns |
| host | `CL_ApplyAddAngle()` | yes | Host_RunFrame/_Host_RunFrame, small host fns |
| host | `Host_GetSoundDuration(char const*)` | yes | Host_RunFrame/_Host_RunFrame, small host fns |
| host | `Host_IsSinglePlayerGame()` | yes | Host_RunFrame/_Host_RunFrame, small host fns |
| host | `Host_IsSecureServerAllowed()` | yes | Host_RunFrame/_Host_RunFrame, small host fns |
| host | `CFrameTimer::MarkFrame()` | yes | Host_RunFrame/_Host_RunFrame, small host fns |
| host_cmd | `Disconnect()` | yes | status, edicts |
| host_cmd | `Host_PrintDemoList()` | yes | status, edicts |
| host_cmd | `startdemos(CCommand const&)` | yes | status, edicts |
| host_cmd | `demos(CCommand const&)` | yes | status, edicts |
| host_cmd | `nextdemo(CCommand const&)` | yes | status, edicts |
| host_listmaps | `Host_MapListFlush()` | yes | Host_MapListFlush |
| host_saverestore | `LoadSaveGame(char const*)` | yes | edict/client layout |
| host_saverestore | `CSaveRestore::RestoreClientState(char const*, bool)` | yes | edict/client layout |
| modelloader | `CModelLoader::DebugPrintDynamicModels()` | yes | CBaseClientState layout |
| net_chan | `CNetChan::FlowNewPacket(int, int, int, int, int, int)` | yes | retail CNetChan |
| net_chan | `CNetChan::ProcessMessages(bf_read&)` | yes | retail CNetChan |
| net_chan | `CNetChan::CheckReceivingList(int)` | yes | retail CNetChan |
| net_chan | `CNetChan::IsValidFileForTransfer(char const*)` | yes | retail CNetChan |
| net_chan | `CNetChan::Clear()` | yes | retail CNetChan |
| net_chan | `CNetChan::Setup(int, netadr_s*, char const*, INetChannelHandler*, int)` | yes | retail CNetChan |
| net_chan | `CNetChan::Shutdown(char const*)` | yes | retail CNetChan |
| net_chan | `CNetChan::CNetChan()` | yes | retail CNetChan |
| networkstringtableclient | `CL_PrintStringTables()` | yes | CBaseClientState layout |
| pr_edict | `__GLOBAL__I_a` | yes | retail ED_Alloc/ED_Free/ED_ClearEdict |
| pr_edict | `ED_ClearEdict(edict_t*)` | yes | retail ED_Alloc/ED_Free/ED_ClearEdict |
| pr_edict | `NUM_FOR_EDICT(edict_t const*)` | yes | retail ED_Alloc/ED_Free/ED_ClearEdict |
| pr_edict | `ED_AllowImmediateReuse()` | yes | retail ED_Alloc/ED_Free/ED_ClearEdict |
| pr_edict | `ED_Free(edict_t*)` | yes | retail ED_Alloc/ED_Free/ED_ClearEdict |
| pr_edict | `EDICT_NUM(int)` | yes | retail ED_Alloc/ED_Free/ED_ClearEdict |
| pr_edict | `CBaseEdict::GetChangeAccessor()` | yes | retail ED_Alloc/ED_Free/ED_ClearEdict |
| pr_edict | `CBaseEdict::GetChangeAccessor() const` | yes | retail ED_Alloc/ED_Free/ED_ClearEdict |
| r_areaportal | `R_SetupAreaBits(int, VisOverrideData_t const*, float*)` | yes | CBaseClientState layout |
| r_areaportal | `R_FlowThroughArea(int, Vector const&, CPortalRect const*, VisOverrideData_t const*, float*)` | yes | CBaseClientState layout |
| servermsghandler | `CClientState::Disconnect(char const*, bool)` | yes | CBaseClientState layout |
| servermsghandler | `CClientState::PacketStart(int, int)` | yes | CBaseClientState layout |
| servermsghandler | `CClientState::ProcessTick(NET_Tick*)` | yes | CBaseClientState layout |
| servermsghandler | `CClientState::ProcessFixAngle(SVC_FixAngle*)` | yes | CBaseClientState layout |
| servermsghandler | `CClientState::ProcessTempEntities(SVC_TempEntities*)` | yes | CBaseClientState layout |
| servermsghandler | `CClientState::ProcessSoundsWithProtoVersion(SVC_Sounds*, CUtlVector<SoundInfo_t, CUtlMemory<SoundInfo_t, int>>&, int)` | yes | CBaseClientState layout |
| servermsghandler | `CClientState::PacketEnd()` | yes | CBaseClientState layout |
| servermsghandler | `non-virtual thunk to CClientState::ProcessTick(NET_Tick*)` | yes | CBaseClientState layout |
| snd_dma | `S_Shutdown()` | yes | S_ClearSoundPool; CClassMemoryPool overlay |
| snd_dma | `S_ClearSoundPool()` | yes | S_ClearSoundPool; CClassMemoryPool overlay |
| snd_dma | `S_ReloadAudioCaches()` | yes | S_ClearSoundPool; CClassMemoryPool overlay |
| snd_mix | `MIX_ScaleChannelVolume(paintbuffer_t*, channel_t*, int*, int)` | yes | commentary ducking |
| snd_mp3_source | `Audio_CreateMemoryMP3(CSfxTable*)` | yes | pre-2016 MP3 source |
| snd_mp3_source | `Audio_CreateStreamedMP3(CSfxTable*)` | yes | pre-2016 MP3 source |
| snd_mp3_source | `CAudioSourceMP3::SampleRate()` | yes | pre-2016 MP3 source |
| snd_mp3_source | `CAudioSourceMP3::IsAsyncLoad()` | yes | pre-2016 MP3 source |
| snd_mp3_source | `CAudioSourceMP3::GetCacheData(CAudioSourceCachedInfo*)` | yes | pre-2016 MP3 source |
| snd_mp3_source | `CAudioSourceMP3::IsVoiceSource()` | yes | pre-2016 MP3 source |
| snd_mp3_source | `CAudioSourceMP3Cache::CreateMixer(int)` | yes | pre-2016 MP3 source |
| snd_mp3_source | `CAudioSourceMP3Cache::GetOutputData(void**, int, int, char*)` | yes | pre-2016 MP3 source |
| snd_mp3_source | `CAudioSourceStreamMP3::CreateMixer(int)` | yes | pre-2016 MP3 source |
| snd_wave_mixer_mp3 | `CreateMP3Mixer(IWaveData*, int*)` | yes | pre-2016 MP3 mixer |
| snd_wave_mixer_mp3 | `CAudioMixerWaveMP3::GetOutputData(void**, int, char*)` | yes | pre-2016 MP3 mixer |
| snd_wave_mixer_mp3 | `CAudioMixerWaveMP3::GetMixSampleSize()` | yes | pre-2016 MP3 mixer |
| snd_wave_mixer_mp3 | `CAudioMixerWaveMP3::StreamRequestData(void*, int, int)` | yes | pre-2016 MP3 mixer |
| snd_wave_mixer_mp3 | `CAudioMixerWaveMP3::GetID3HeaderOffset()` | yes | pre-2016 MP3 mixer |
| snd_wave_mixer_mp3 | `CAudioMixerWaveMP3::GetPositionForSave()` | yes | pre-2016 MP3 mixer |
| snd_wave_mixer_mp3 | `CAudioMixerWaveMP3::GetStreamOutputRate()` | yes | pre-2016 MP3 mixer |
| snd_wave_mixer_mp3 | `CAudioMixerWaveMP3::SetPositionFromSaved(int)` | yes | pre-2016 MP3 mixer |
| snd_wave_mixer_mp3 | `CAudioMixerWaveMP3::SetStartupDelaySamples(int)` | yes | pre-2016 MP3 mixer |
| snd_wave_mixer_mp3 | `CAudioMixerWaveMP3::Mix(IAudioDevice*, channel_t*, void*, int, int, unsigned int, int, int)` | yes | pre-2016 MP3 mixer |
| snd_wave_mixer_mp3 | `non-virtual thunk to CAudioMixerWaveMP3::StreamRequestData(void*, int, int)` | yes | pre-2016 MP3 mixer |
| snd_wave_source | `CAudioSourceCachedInfo::Rebuild(char const*)` | yes | MP3 source |
| sv_client | `CGameClient::SendSignonData()` | yes | edict layout |
| sv_client | `CGameClient::ShouldSendMessages()` | yes | edict layout |
| sv_framesnapshot | `CFrameSnapshotManager::TakeTickSnapshot(int)` | yes | edict layout |
| sv_log | `__GLOBAL__I_a` | yes | retail CLog |
| sv_log | `log(CCommand const&)` | yes | retail CLog |
| sv_log | `CLog::Init()` | yes | retail CLog |
| sv_log | `CLog::Open()` | yes | retail CLog |
| sv_log | `CLog::Close()` | yes | retail CLog |
| sv_log | `CLog::Print(char const*)` | yes | retail CLog |
| sv_log | `CLog::RunFrame()` | yes | retail CLog |
| sv_log | `CLog::Shutdown()` | yes | retail CLog |
| sv_log | `CLog::CLog()` | yes | retail CLog |
| sv_log | `CLog::~CLog()` | yes | retail CLog |
| sv_log | `CLog::~CLog()` | yes | retail CLog |
| sv_main | `CGameServer::SetMaxClients(int)` | yes | edict allocator (no free_edicts) |
| sv_main | `CGameServer::SendClientMessages(bool)` | yes | edict allocator (no free_edicts) |
| sv_main | `CGameServer::CreateEngineStringTables()` | yes | edict allocator (no free_edicts) |
| sv_main | `CGameServer::CopyPureServerWhitelistToStringTable()` | yes | edict allocator (no free_edicts) |
| sv_main | `CGameServer::Init(bool)` | yes | edict allocator (no free_edicts) |
| sv_main | `CGameServer::Clear()` | yes | edict allocator (no free_edicts) |
| sv_main | `CGameServer::Shutdown()` | yes | edict allocator (no free_edicts) |
| sv_main | `CGameServer::CGameServer()` | yes | edict allocator (no free_edicts) |
| sv_main | `CGameServer::~CGameServer()` | yes | edict allocator (no free_edicts) |
| sv_main | `CGameServer::IsInPureServerMode() const` | yes | edict allocator (no free_edicts) |
| sv_main | `CGameServer::GetPureServerWhitelist() const` | yes | edict allocator (no free_edicts) |
| sv_packedentities | `GetObjectClassName(int)` | yes | edict allocator layout |
| sv_packedentities | `PackEntities_Normal(int, CGameClient**, CFrameSnapshot*)` | yes | edict allocator layout |
| sv_packedentities | `SV_ComputeClientPacks(int, CGameClient**, CFrameSnapshot*)` | yes | edict allocator layout |
| sv_packedentities | `SV_EnsureInstanceBaseline(ServerClass*, int, void const*, int)` | yes | edict allocator layout |
| sv_packedentities | `PackEntities_NetworkBackDoor(int, CGameClient**, CFrameSnapshot*)` | yes | edict allocator layout |
| sv_precache | `SV_DecalIndex(char const*)` | yes | edict layout |
| sv_precache | `SV_ModelIndex(char const*)` | yes | edict layout |
| sv_precache | `SV_SoundIndex(char const*)` | yes | edict layout |
| sv_precache | `SV_GenericIndex(char const*)` | yes | edict layout |
| sv_precache | `sv_precacheinfo(CCommand const&)` | yes | edict layout |
| sv_precache | `CGameServer::PrecacheDecal(char const*, int)` | yes | edict layout |
| sv_precache | `CGameServer::PrecacheModel(char const*, int, model_t*)` | yes | edict layout |
| sv_precache | `CGameServer::PrecacheSound(char const*, int)` | yes | edict layout |
| sv_precache | `CGameServer::PrecacheGeneric(char const*, int)` | yes | edict layout |
| sv_precache | `CGameServer::LookupModelIndex(char const*)` | yes | edict layout |
| sv_precache | `CGameServer::LookupSoundIndex(char const*)` | yes | edict layout |
| sv_precache | `CGameServer::DumpPrecacheStats(INetworkStringTable*)` | yes | edict layout |
| sv_precache | `CGameServer::GetModel(int)` | yes | edict layout |
| sv_precache | `CGameServer::GetSound(int)` | yes | edict layout |
| sv_precache | `CGameServer::GetModelPrecacheTable() const` | yes | edict layout |
| sv_precache | `CGameServer::GetSoundPrecacheTable() const` | yes | edict layout |
| vengineserver_impl | `InvalidateSharedEdictChangeInfos()` | yes | edict allocator layout |
| vengineserver_impl | `CVEngineServer::StaticDecal(Vector const&, int, int, int, bool)` | yes | edict allocator layout |
| vengineserver_impl | `CVEngineServer::IndexOfEdict(edict_t const*)` | yes | edict allocator layout |
| vengineserver_impl | `CVEngineServer::GetEntityCount()` | yes | edict allocator layout |
| vengineserver_impl | `CVEngineServer::EmitAmbientSound(int, Vector const&, char const*, float, soundlevel_t, int, int, float)` | yes | edict allocator layout |
| vengineserver_impl | `CVEngineServer::GetChangeAccessor(edict_t const*)` | yes | edict allocator layout |
| vengineserver_impl | `CVEngineServer::NotifyEdictFlagsChange(int)` | yes | edict allocator layout |
| view | `CVRenderView::SetAreaState(unsigned char*, unsigned char*)` | yes | CBaseClientState layout |
| voice_record_mac_audioqueue | `__GLOBAL__I_a` | yes | Init() |
| voice_record_mac_audioqueue | `StartAudio(void*)` | yes | Init() |
| voice_record_mac_audioqueue | `CreateVoiceRecord_AudioQueue(int)` | yes | Init() |
| voice_record_mac_audioqueue | `VoiceRecord_AudioQueue::Init()` | yes | Init() |

## Functions matching only with the shared-header overlays

| object | function | status | note |
|---|---|---|---|
| cvar | `CCvarUtilities::IsValidToggleCommand(char const*)` | +ovl | CanCheat / mempool overlay |
| cvar | `CCvarUtilities::IsCommand(CCommand const&)` | +ovl | CanCheat / mempool overlay |
| disp | `DispInfoRenderDebugModes()` | +ovl | CanCheat / mempool overlay |
| disp | `CDispInfo::Render(CGroupMesh*, bool)` | +ovl | CanCheat / mempool overlay |
| disp_interface | `DispInfo_DrawDecalsGroup(int, int)` | +ovl | CanCheat / mempool overlay |
| gl_rsurf | `R_SceneBegin()` | +ovl | CanCheat / mempool overlay |
| gl_rsurf | `DrawDebugInformation(CUtlVector<msurface2_t*, CUtlMemory<msurface2_t*, int>> const&)` | +ovl | CanCheat / mempool overlay |
| host | `Host_AccumulateTime(float)` | +ovl | CanCheat / mempool overlay |
| host | `CheckVarRange_Generic(ConVar*, int, int)` | +ovl | CanCheat / mempool overlay |
| l_studio | `UpdateStudioRenderConfig()` | +ovl | CanCheat / mempool overlay |
| lightcache | `CClassMemoryPool<PropLightcache_t>::Clear()` | +ovl | CanCheat / mempool overlay |
| matchmakingclient | `CMatchmaking::StartClient(bool)` | +ovl | MAX_MAP_NAME 32 |
| matchmakingclient | `CMatchmaking::ConnectToHost()` | +ovl | MAX_MAP_NAME 32 |
| matchmakingclient | `CMatchmaking::SelectSession(unsigned int)` | +ovl | MAX_MAP_NAME 32 |
| matchmakingclient | `CMatchmaking::SendJoinRequest(netadr_s*)` | +ovl | MAX_MAP_NAME 32 |
| matchmakingclient | `CMatchmaking::SearchForSession()` | +ovl | MAX_MAP_NAME 32 |
| matchmakingclient | `CMatchmaking::JoinInviteSession(_XSESSION_INFO*)` | +ovl | MAX_MAP_NAME 32 |
| matchmakingclient | `CMatchmaking::ClearSearchResults()` | +ovl | MAX_MAP_NAME 32 |
| matchmakingclient | `CMatchmaking::ProcessJoinResponse(MM_JoinResponse*)` | +ovl | MAX_MAP_NAME 32 |
| matchmakingclient | `CMatchmaking::HandleSystemLinkReply(netpacket_s*)` | +ovl | MAX_MAP_NAME 32 |
| matchmakingclient | `CMatchmaking::ApplySessionProperties(int, int, _XUSER_CONTEXT*, _XUSER_PROPERTY*)` | +ovl | MAX_MAP_NAME 32 |
| matchmakinghost | `CMatchmaking::ModifySession()` | +ovl | MAX_MAP_NAME 32 |
| matchmakinghost | `CMatchmaking::CancelStartGame()` | +ovl | MAX_MAP_NAME 32 |
| matchmakinghost | `CMatchmaking::EndSessionModify()` | +ovl | MAX_MAP_NAME 32 |
| matchmakinghost | `CMatchmaking::GetPlayersNeeded()` | +ovl | MAX_MAP_NAME 32 |
| matchmakinghost | `CMatchmaking::EndStatsReporting()` | +ovl | MAX_MAP_NAME 32 |
| matchmakinghost | `CMatchmaking::HandleJoinRequest(netpacket_s*)` | +ovl | MAX_MAP_NAME 32 |
| matchmakinghost | `CMatchmaking::CountPlayersOnTeam(int)` | +ovl | MAX_MAP_NAME 32 |
| matchmakinghost | `CMatchmaking::OnHostSessionCreated()` | +ovl | MAX_MAP_NAME 32 |
| matchmakinghost | `CMatchmaking::SwitchToNextOpenTeam(CClientInfo*)` | +ovl | MAX_MAP_NAME 32 |
| matchmakinghost | `CMatchmaking::TellClientsToConnect()` | +ovl | MAX_MAP_NAME 32 |
| matchmakinghost | `CMatchmaking::HandleSystemLinkSearch(netpacket_s*)` | +ovl | MAX_MAP_NAME 32 |
| matchmakinghost | `CMatchmaking::ProcessRegisterResponse(MM_RegisterResponse*)` | +ovl | MAX_MAP_NAME 32 |
| matchmakinghost | `CMatchmaking::SendModifySessionMessage()` | +ovl | MAX_MAP_NAME 32 |
| matchmakinghost | `CMatchmaking::ProcessRegistrationResults()` | +ovl | MAX_MAP_NAME 32 |
| matchmakinghost | `CMatchmaking::EndGame()` | +ovl | MAX_MAP_NAME 32 |
| matchmakinghost | `CMatchmaking::StartGame()` | +ovl | MAX_MAP_NAME 32 |
| matchmakinghost | `CMatchmaking::StartHost(bool)` | +ovl | MAX_MAP_NAME 32 |
| matchmakinghost | `non-virtual thunk to CMatchmaking::ProcessRegisterResponse(MM_RegisterResponse*)` | +ovl | MAX_MAP_NAME 32 |
| matchmakingmigrate | `CMatchmaking::EndMigration()` | +ovl | MAX_MAP_NAME 32 |
| matchmakingmigrate | `CMatchmaking::ProcessMigrate(MM_Migrate*)` | +ovl | MAX_MAP_NAME 32 |
| matchmakingshared | `mm_message(CCommand const&)` | +ovl | MAX_MAP_NAME 32 |
| matchmakingshared | `CMatchmaking::ChangeTeam(char const*)` | +ovl | MAX_MAP_NAME 32 |
| matchmakingshared | `CMatchmaking::SendMessage(INetMessage*, CClientInfo*, bool)` | +ovl | MAX_MAP_NAME 32 |
| matchmakingshared | `CMatchmaking::SendMessage(INetMessage*, netadr_s*, bool)` | +ovl | MAX_MAP_NAME 32 |
| matchmakingshared | `CMatchmaking::GameIsActive()` | +ovl | MAX_MAP_NAME 32 |
| matchmakingshared | `CMatchmaking::ClientDropped(CClientInfo*)` | +ovl | MAX_MAP_NAME 32 |
| matchmakingshared | `CMatchmaking::IsPlayerMuted(int, unsigned long long)` | +ovl | MAX_MAP_NAME 32 |
| matchmakingshared | `CMatchmaking::SwitchToState(int)` | +ovl | MAX_MAP_NAME 32 |
| matchmakingshared | `CMatchmaking::PlayerIdToXuid(int)` | +ovl | MAX_MAP_NAME 32 |
| matchmakingshared | `CMatchmaking::UpdateMuteList()` | +ovl | MAX_MAP_NAME 32 |
| matchmakingshared | `CMatchmaking::ShowSessionInfo()` | +ovl | MAX_MAP_NAME 32 |
| matchmakingshared | `CMatchmaking::AddRemoteChannel(netadr_s*)` | +ovl | MAX_MAP_NAME 32 |
| matchmakingshared | `CMatchmaking::PerformDisconnect()` | +ovl | MAX_MAP_NAME 32 |
| matchmakingshared | `CMatchmaking::ProcessCheckpoint(MM_Checkpoint*)` | +ovl | MAX_MAP_NAME 32 |
| matchmakingshared | `CMatchmaking::ProcessClientInfo(MM_ClientInfo*)` | +ovl | MAX_MAP_NAME 32 |
| matchmakingshared | `CMatchmaking::SetChannelTimeout(netadr_s*, int)` | +ovl | MAX_MAP_NAME 32 |
| matchmakingshared | `CMatchmaking::AddSessionProperty(unsigned int, char const*, char const*, char const*)` | +ovl | MAX_MAP_NAME 32 |
| matchmakingshared | `CMatchmaking::AddPlayersToSession(CClientInfo*)` | +ovl | MAX_MAP_NAME 32 |
| matchmakingshared | `CMatchmaking::RemoveRemoteChannel(netadr_s*, char const*)` | +ovl | MAX_MAP_NAME 32 |
| matchmakingshared | `CMatchmaking::SendToRemoteClients(INetMessage*, bool, unsigned long long)` | +ovl | MAX_MAP_NAME 32 |
| matchmakingshared | `CMatchmaking::SessionNotification(SESSION_NOTIFY, int)` | +ovl | MAX_MAP_NAME 32 |
| matchmakingshared | `CMatchmaking::GetSessionProperties()` | +ovl | MAX_MAP_NAME 32 |
| matchmakingshared | `CMatchmaking::SetSessionProperties(KeyValues*)` | +ovl | MAX_MAP_NAME 32 |
| matchmakingshared | `CMatchmaking::CleanupMarkedChannels()` | +ovl | MAX_MAP_NAME 32 |
| matchmakingshared | `CMatchmaking::InitializeLocalClient(bool)` | +ovl | MAX_MAP_NAME 32 |
| matchmakingshared | `CMatchmaking::KickPlayerFromSession(unsigned long long)` | +ovl | MAX_MAP_NAME 32 |
| matchmakingshared | `CMatchmaking::AddLocalPlayersToTeams()` | +ovl | MAX_MAP_NAME 32 |
| matchmakingshared | `CMatchmaking::CancelCurrentOperation()` | +ovl | MAX_MAP_NAME 32 |
| matchmakingshared | `CMatchmaking::IsAcceptingConnections()` | +ovl | MAX_MAP_NAME 32 |
| matchmakingshared | `CMatchmaking::OnLevelLoadingFinished()` | +ovl | MAX_MAP_NAME 32 |
| matchmakingshared | `CMatchmaking::PreventFullServerStartup()` | +ovl | MAX_MAP_NAME 32 |
| matchmakingshared | `CMatchmaking::AddSessionPropertyInternal(KeyValues*)` | +ovl | MAX_MAP_NAME 32 |
| matchmakingshared | `CMatchmaking::SetPreventFullServerStartup(bool, char const*, ...)` | +ovl | MAX_MAP_NAME 32 |
| matchmakingshared | `CMatchmaking::Cleanup()` | +ovl | MAX_MAP_NAME 32 |
| matchmakingshared | `CMatchmaking::IsServer()` | +ovl | MAX_MAP_NAME 32 |
| matchmakingshared | `CMatchmaking::CMatchmaking()` | +ovl | MAX_MAP_NAME 32 |
| matchmakingshared | `CMatchmaking::~CMatchmaking()` | +ovl | MAX_MAP_NAME 32 |
| perfuipanel | `CPerfUIPanel::OnTick()` | +ovl | CanCheat / mempool overlay |
| perfuipanel | `CPerfUIPanel::Activate()` | +ovl | CanCheat / mempool overlay |
| r_decal | `R_DrawDecalsAll(IMatRenderContext*, int, int, Vector const&, int, float)` | +ovl | CanCheat / mempool overlay |
| r_decal | `Shader_DecalDrawPoly(CDecalVert*, IMaterial*, msurface2_t*, int, decal_t*, float)` | +ovl | CanCheat / mempool overlay |
| r_decal | `R_DrawDecalsAllImmediate(IMatRenderContext*, int, int, Vector const&, int, float)` | +ovl | CanCheat / mempool overlay |
| r_decal | `R_DrawDecalsAll_Gathered(IMatRenderContext*, decal_t**, int, Vector const&, float)` | +ovl | CanCheat / mempool overlay |
| r_decal | `R_DrawDecalsAllImmediate_Gathered(IMatRenderContext*, decal_t**, int, Vector const&, float)` | +ovl | CanCheat / mempool overlay |
| snd_dma | `CUtlMemory<UtlRBTreeNode_t<void*, unsigned short>, unsigned short>::Grow(int)` | +ovl | CClassMemoryPool::Clear (SDK 2013) |
| snd_dma | `CUtlRBTree<void*, unsigned short, bool (*)(void* const&, void* const&), CUtlMemory<UtlRBTreeNode_t<void*, unsigned short>, unsigned short>>::RotateLeft(unsigned short)` | +ovl | CClassMemoryPool::Clear (SDK 2013) |
| snd_dma | `CUtlRBTree<void*, unsigned short, bool (*)(void* const&, void* const&), CUtlMemory<UtlRBTreeNode_t<void*, unsigned short>, unsigned short>>::RotateRight(unsigned short)` | +ovl | CClassMemoryPool::Clear (SDK 2013) |
| snd_dma | `CUtlRBTree<void*, unsigned short, bool (*)(void* const&, void* const&), CUtlMemory<UtlRBTreeNode_t<void*, unsigned short>, unsigned short>>::InsertRebalance(unsigned short)` | +ovl | CClassMemoryPool::Clear (SDK 2013) |
| snd_dma | `CUtlRBTree<void*, unsigned short, bool (*)(void* const&, void* const&), CUtlMemory<UtlRBTreeNode_t<void*, unsigned short>, unsigned short>>::FindInsertionPosition(void* const&, unsigned short&, bool&)` | +ovl | CClassMemoryPool::Clear (SDK 2013) |
| snd_dma | `CUtlRBTree<void*, unsigned short, bool (*)(void* const&, void* const&), CUtlMemory<UtlRBTreeNode_t<void*, unsigned short>, unsigned short>>::Insert(void* const&)` | +ovl | CClassMemoryPool::Clear (SDK 2013) |
| snd_dma | `CUtlRBTree<void*, unsigned short, bool (*)(void* const&, void* const&), CUtlMemory<UtlRBTreeNode_t<void*, unsigned short>, unsigned short>>::NewNode()` | +ovl | CClassMemoryPool::Clear (SDK 2013) |
| snd_dma | `CUtlRBTree<void*, unsigned short, bool (*)(void* const&, void* const&), CUtlMemory<UtlRBTreeNode_t<void*, unsigned short>, unsigned short>>::RemoveAll()` | +ovl | CClassMemoryPool::Clear (SDK 2013) |
| snd_dma | `CClassMemoryPool<CSfxTable>::Clear()` | +ovl | CClassMemoryPool::Clear (SDK 2013) |
| snd_dma | `CUtlRBTree<void*, unsigned short, bool (*)(void* const&, void* const&), CUtlMemory<UtlRBTreeNode_t<void*, unsigned short>, unsigned short>>::IsLeftChild(unsigned short) const` | +ovl | CClassMemoryPool::Clear (SDK 2013) |
| snd_dma | `CUtlRBTree<void*, unsigned short, bool (*)(void* const&, void* const&), CUtlMemory<UtlRBTreeNode_t<void*, unsigned short>, unsigned short>>::IsRightChild(unsigned short) const` | +ovl | CClassMemoryPool::Clear (SDK 2013) |
| snd_dma | `CUtlRBTree<void*, unsigned short, bool (*)(void* const&, void* const&), CUtlMemory<UtlRBTreeNode_t<void*, unsigned short>, unsigned short>>::Find(void* const&) const` | +ovl | CClassMemoryPool::Clear (SDK 2013) |
| sv_client | `CGameClient::ExecuteStringCommand(char const*)` | +ovl | CanCheat / mempool overlay |
| sys_engine | `CEngine::FilterTime(float)` | +ovl | CanCheat / mempool overlay |
| vgui_budgetpanel | `CBudgetPanelEngine::OnTick()` | +ovl | CanCheat / mempool overlay |

## Edited, still differing (cause known)

| object | function | status | note |
|---|---|---|---|
| host | `Host_WriteConfiguration(char const*)` | missing | retail signature/ROOT_MOD path not written yet; Steam cloud block needs newer `ISteamRemoteStorage` (v016, 64-bit `GetQuota`) |
| host | `Host_ReadConfiguration()`, `Host_ReadPreStartupConfiguration()` | differs | anniversary `hl2_complete` / `//ROOT_MOD` config, `Host_RestoreHL2EpisodicUnlocks` not written; Steam context |
| host | `Host_RunFrame(float)`, `Host_ShutdownServer()`, `Host_FreeStateAndWorld(bool)` | differs | only `IModelLoader` slot: retail has no `ResetModelServerCounts` (engine-render's modelloader.h) |
| host | `Host_AbortServer()`, `Host_Error`, `Host_EndGame` | differs | toolchain SDK stub declares `longjmp` without `__dead2` |
| host | `Host_CheckGore`, `Host_Init`, `Host_ShowIPCCallCount` | differs | Steamworks context layout (`CSteamAPIContext`); Host_Init also `IThreadPool::Start(params)` overload, `Host_InitVersion` |
| host | `CDatadescGeneratedNameHolder::~CDatadescGeneratedNameHolder()` | differs | retail uses `delete`, public/datamap.h has `delete[]` (headers lane) |
| host_cmd | `status`, `version`, `GetSteamAppID`, missing `ParseSteamInfFile`, `DetermineAppID`, `Host_InitAppID`, `Host_InitVersion`, `GetClientVersion`, `GetServerVersion` | differs/missing | retail keeps the SDK 2013 version code (`gpszVersionString`, steam.inf parsing in host_cmd); tree uses `GetSteamInfIDVersionInfo()` |
| snd_dma | `S_Update(AudioState_t const*)` | differs | logic and size equal retail; register/scheduling only (also in the leak's own voice loops) |
| sv_steamauth | most `CSteam3Server` functions, `CCallback` templates, `SteamInternal_Init_*` | differs/missing | Steamworks SDK version (retail: `SteamInternal_ContextInit`, 32-byte `CSteamGameServerAPIContext`) |
| net_chan | `ReadSubChannelData`, `CompressFragments` | differs | stack frame differences, not investigated |


# Round 2 (engine-core2 lane)

Branch `bytematch-engine-core2`, started at `bytematch` 72205168, merged with
`bytematch-engine-render` 1c238e70 (finished, needed for the `IVideoMode` slot
and `IModelLoader`/`CanCheat` changes) and later with `bytematch` 88902342
(Steamworks 1.60, `MAX_MAP_NAME` 32, SDK 2013 `CClassMemoryPool::Clear`).
Measured with `lane-engine-core2-scratch/run.sh` (engine/ objects; 8713
functions once `rpt_engine` is compiled).

Totals: 7841/8702 (77.19% bytes) at 72205168 -> 8371/8713 (88.87%). Whole
engine.dylib: 12429/12997 functions, 88.13% of bytes. No function matched at
the start is lost.

## Per object, engine-core file set (start -> end / total)

| object | start | end | total | cause |
|---|---:|---:|---:|---|
| host_cmd | 46 | 59 | 64 | SDK 2013 version code (gpszVersionString, ParseSteamInfFile, DetermineAppID, Host_InitAppID, Host_InitVersion, build_number), Host_Map_Helper/changelevel without FindMap |
| host | 62 | 76 | 88 | GetPlatformMapPath, void Host_Changelevel (SDK 2013 + save_transition_music), anniversary config write/read, Host_RestoreHL2EpisodicUnlocks, Host_InitVersion |
| host_state | 15 | 16 | 20 | State_ChangeLevelMP |
| sv_main | 57 | 68 | 71 | SDK-era sv_pure, shutdown/hibernation, SpawnServer(char*), no edict freetime |
| sv_client | 82 | 86 | 86 | 66/10 update rates, player_connect only |
| sv_steamauth | 30 | 73 | 73 | Steam 1.60 (headers3) + retail Activate() without server type, relay/spectator details |
| sv_rcon, cl_rcon | 30, 37 | 34, 44 | 36, 47 | ENABLE_RPT |
| rpt_engine | 0 | 11 | 11 | ENABLE_RPT compiles it |
| baseclient | 104 | 116 | 118 | no m_bInitialConVarsSet, ClientInfo without signon check, GUID memcpy |
| networkstringtable | 96 | 99 | 99 | sv_compressstringtablebaselines (LZSS), older substring parse |
| sys_getmodes | 25 | 48 | 51 | borderless window (m_bNoWindowBorder, 4-arg CreateGameWindow/SetMode), startup graphic |
| sys_dll2 | 44 | 48 | 59 | Host_InitAppID in SetStartupInfo/ModInit, expansion id in CEngineAPI::ModInit, IsValveMod(pModName) |
| matsys_interface | 31 | 40 | 41 | NO_WINDOW_BORDER/LIGHTMAP_BICUBIC flags, ScreenNoBorder, _rt_CameraHD, retail convars |
| cvar | 31 | 34 | 35 | WriteVariables(CUtlBuffer&) writes all archived cvars |
| matchmaking* | 101 | 168 | 169 | MAX_MAP_NAME 32 (headers3) |
| snd_dma | 125 | 137 | 141 | CClassMemoryPool::Clear (headers3) |

Render-lane objects in the diff (modelloader, cl_*, r_decal, l_studio,
vgui_baseui_interface, replaydemoplayer, ...) moved because of the
engine-render merge, not this lane.

## Edited, still differing (cause known)

| object | function | note |
|---|---|---|
| host | `Host_WriteConfiguration`, `Host_ReadConfiguration`, `Host_ReadPreStartupConfiguration` | logic is retail's; register/stack-slot allocation differs (`g_bConfigCfgExecuted` loads as movzx/cmp instead of xor/test) |
| host | `Host_Init` | IThreadPool::Start(params) overload |
| host | `Host_Error`, `Host_EndGame`, `Host_AbortServer` | 10.9 stub SDK `longjmp` without `__dead2` |
| host_cmd | `status`, `Host_Status_PrintClient` | not investigated |
| host_cmd | `Host_VoiceRecordStop_f`, `voiceconvar_file_changed_f`, `sv_use_steam_voice` | 2013 voice API (Voice_RecordStop, CL_SendVoicePacket(true)) |
| sv_main | `SV_CreateBaseline`, `__GLOBAL__I_a` | 2013 voice init: `SVC_VoiceInit("vaudio_speex"/"", 5)`, no sv_voicecodec |
| sv_main | `SV_ActivateServer` | not investigated |
| voice | most | SDK 2013-era voice.cpp (Speex/Miles, Steam voice via SteamUser(), Voice_Init(const char*)) |
| sys_getmodes | `TakeSnapshotJPEGToBuffer`, `WriteMovieFrame`, `VID_ProcessMovieFrame` | retail build has HAVE_JPEG (static libjpeg) |
| sys_dll2 | `CErrorText` (non-template), `EditorToggle_f`, `CDedicatedServerAPI::Connect` | not done |
| host_state | `HostState_Init`, `HostState_NewGame`, `State_Run`, `FrameUpdate` | HostState_Init tail-calls CCS_Init (no watchdog); NewGame strips the extension; not done |
