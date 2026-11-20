# --retail-win32 match report

Source: `bytematch` `6a651850b` (`ivp` `2215d507`). MSVC 2013 Update 5 under wine; COFF objects of each module vs depot 222 build 19307283 through the symbol-port maps (`byte-match-survey/win32-maps` first, then `symbols/out`; relocation targets named only by map rows of confidence 0.9 or more; targets our own `/OPT:ICF` folded accepted through `/MAP`). RAD Telemetry zones on, through the Telemetry 1.1 declarations in `toolchain/win32/telemetry` (no hl2_src headers). DLL and game rows are scoped by each module's own objects (`--own-objects`); static libraries are matched against retail `engine.dll` when the engine links them, else against the first module that does. Cross-check: the same modules built with `--retail-osx32` at `6a651850b` against depot 223 (380 for episodic).

**Totals (all 24 DLLs/EXEs):**
- `--own-objects` scope: **65,619 / 65,973 functions (99.46%)**, **14,026,098 / 14,131,653 bytes (99.25%)** (20 engine DLLs/EXEs: 17,521/17,747 = 98.73% funcs, 98.06% bytes; 4 game DLLs: 48,098/48,226 = 99.73% funcs, 99.61% bytes).
- Full default scope (DLL + static libraries): **85,088 / 85,397 functions (99.64%)**, **17,701,505 / 17,822,361 bytes (99.32%)** (20 engine DLLs/EXEs: 29,359/29,523 = 99.44% funcs, 98.64% bytes; 4 game DLLs: 55,729/55,874 = 99.74% funcs, 99.61% bytes).

**Build note (`engine.dll`):** 23 of 24 DLLs/EXEs and all 22 static libraries linked cleanly at `6a651850b`. `engine.dll` failed to link with 6 `LNK2019` unresolved externals in `MPAFile.cpp.21.o` (`CVBRHeader` and `CMPAHeader` methods) introduced when `28f38f773` restored `CMPAFile` in `engine/audio/MPAFile.cpp` (dead-stripped on macOS, but MSVC `link.exe` resolves unreferenced COMDAT externals before `/OPT:REF` and `retail-win32.json` omits `MPAHeader.cpp`/`VBRHeader.cpp`). All `engine` COFF objects compiled cleanly at `6a651850b`; `engine` and its 13 static libraries are measured directly from those fresh objects without an `engine.map` `/OPT:ICF` folding map.

| module | functions matched | % functions | bytes matched | % bytes | exact | differs | `__LINE__` only | missing | macOS functions | macOS % |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| tier0 | 302/307 | 98.37% | 39042/39658 | 98.45% | 34 | 3 | 0 | 2 | 367/367 | 100.0% |
| vstdlib | 156/156 | 100.0% | 23314/23314 | 100.0% | 34 | 0 | 0 | 0 | 361/361 | 100.0% |
| scenefilecache | 16/16 | 100.0% | 1255/1255 | 100.0% | 5 | 0 | 0 | 0 | 40/40 | 100.0% |
| soundemittersystem | 90/90 | 100.0% | 15347/15347 | 100.0% | 24 | 0 | 0 | 0 | 236/236 | 100.0% |
| datacache | 315/321 | 98.13% | 54503/56023 | 97.29% | 46 | 2 | 0 | 4 | 406/406 | 100.0% |
| studiorender | 551/554 | 99.46% | 271722/277837 | 97.8% | 45 | 3 | 0 | 0 | 986/988 | 99.8% |
| inputsystem | 103/106 | 97.17% | 11583/11651 | 99.42% | 29 | 0 | 0 | 3 | 193/193 | 100.0% |
| engine | 7890/7982 | 98.85% | 1512166/1530918 | 98.78% | 915 | 61 | 0 | 31 | 13040/13040 | 100.0% |
| filesystem_stdio | 532/545 | 97.61% | 103992/105687 | 98.4% | 99 | 5 | 0 | 8 | 1962/1962 | 100.0% |
| materialsystem | 1429/1445 | 98.89% | 194128/205100 | 94.65% | 400 | 10 | 0 | 6 | 3747/3747 | 100.0% |
| shaderapidx9 | 681/684 | 99.56% | 124438/124869 | 99.65% | 177 | 1 | 0 | 2 | 1592/1592 | 100.0% |
| stdshader_dx9 | 1730/1732 | 99.88% | 233934/238387 | 98.13% | 19 | 2 | 0 | 0 | 1963/1963 | 100.0% |
| vgui2 | 367/370 | 99.19% | 43641/45193 | 96.57% | 176 | 2 | 0 | 1 | 684/684 | 100.0% |
| vguimatsurface | 269/271 | 99.26% | 45043/45166 | 99.73% | 60 | 1 | 0 | 1 | 3002/3002 | 100.0% |
| GameUI | 1390/1456 | 95.47% | 249382/263037 | 94.81% | 94 | 8 | 0 | 58 | 4970/4970 | 100.0% |
| ServerBrowser | 399/406 | 98.28% | 66922/67326 | 99.4% | 37 | 0 | 0 | 7 | 3395/3395 | 100.0% |
| vphysics | 585/588 | 99.49% | 121995/122884 | 99.28% | 135 | 1 | 0 | 2 | 2962/2963 | 99.97% |
| replay | 654/655 | 99.85% | 70237/70257 | 99.97% | 97 | 0 | 0 | 1 | 1442/1442 | 100.0% |
| launcher | 61/62 | 98.39% | 12873/14678 | 87.7% | 5 | 1 | 0 | 0 | 361/361 | 100.0% |
| launcher_main | 1/1 | 100.0% | 336/336 | 100.0% | 0 | 0 | 0 | 0 | 1/1 | 100.0% |
| client_hl2 | 7842/7866 | 99.69% | 1616765/1629085 | 99.24% | 842 | 11 | 0 | 13 | 17887/17892 | 99.97% |
| server_hl2 | 16126/16170 | 99.73% | 3785714/3795526 | 99.74% | 1233 | 35 | 7 | 9 | 22744/22744 | 100.0% |
| client_episodic | 7853/7874 | 99.73% | 1620121/1630645 | 99.35% | 845 | 9 | 0 | 12 | 17892/17893 | 99.99% |
| server_episodic | 16277/16316 | 99.76% | 3807645/3817474 | 99.74% | 1283 | 31 | 7 | 8 | 22744/22744 | 100.0% |
| tier1.lib (vs engine) | 396/419 | 94.51% | 57398/60938 | 94.19% | 106 | 21 | 0 | 2 | 485/485 | 100.0% |
| tier2.lib (vs engine) | 47/51 | 92.16% | 12631/14280 | 88.45% | 13 | 4 | 0 | 0 | 65/65 | 100.0% |
| tier3.lib (vs engine) | 2/2 | 100.0% | 691/691 | 100.0% | 0 | 0 | 0 | 0 | 2/2 | 100.0% |
| bitmap.lib (vs engine) | 23/31 | 74.19% | 6002/9000 | 66.69% | 4 | 3 | 0 | 5 | 102/102 | 100.0% |
| mathlib.lib (vs engine) | 96/97 | 98.97% | 25854/25938 | 99.68% | 26 | 1 | 0 | 0 | 95/95 | 100.0% |
| replay_common.lib (vs engine) | 6/6 | 100.0% | 534/534 | 100.0% | 2 | 0 | 0 | 0 | 6/6 | 100.0% |
| appframework.lib (vs engine) | 32/34 | 94.12% | 5386/6425 | 83.83% | 2 | 1 | 0 | 1 | 33/34 | 97.06% |
| bzip2.lib (vs engine) | 16/22 | 72.73% | 15873/17792 | 89.21% | 2 | 6 | 0 | 0 | 25/25 | 100.0% |
| jpeglib.lib (vs engine) | 0/0 | 0.0% | 0/0 | 0.0% | 0 | 0 | 0 | 0 |  |  |
| dmxloader.lib (vs engine) | 52/54 | 96.3% | 12908/12987 | 99.39% | 5 | 0 | 0 | 2 | 99/99 | 100.0% |
| matsys_controls.lib (vs engine) | 15/17 | 88.24% | 2663/2863 | 93.01% | 4 | 2 | 0 | 0 | 28/28 | 100.0% |
| vtf.lib (vs engine) | 78/83 | 93.98% | 14569/16002 | 91.04% | 22 | 5 | 0 | 0 | 82/82 | 100.0% |
| vgui_controls.lib (vs engine) | 1405/1437 | 97.77% | 251043/261529 | 95.99% | 191 | 31 | 0 | 1 | 2523/2523 | 100.0% |
| vpklib.lib (vs filesystem_stdio) | 38/38 | 100.0% | 14662/14662 | 100.0% | 4 | 0 | 0 | 0 | 241/241 | 100.0% |
| shaderlib.lib (vs materialsystem) | 8/8 | 100.0% | 410/410 | 100.0% | 3 | 0 | 0 | 0 | 13/13 | 100.0% |
| vgui_surfacelib.lib (vs vgui2) | 0/0 | 0.0% | 0/0 | 0.0% | 0 | 0 | 0 | 0 | 0/0 | 0.0% |
| havana_constraints.lib (vs vphysics) | 68/71 | 95.77% | 22098/24993 | 88.42% | 19 | 3 | 0 | 0 | 143/143 | 100.0% |
| hk_base.lib (vs vphysics) | 4/4 | 100.0% | 511/511 | 100.0% | 0 | 0 | 0 | 0 | 5/5 | 100.0% |
| hk_math.lib (vs vphysics) | 14/14 | 100.0% | 3720/3720 | 100.0% | 7 | 0 | 0 | 0 | 15/15 | 100.0% |
| ivp_compactbuilder.lib (vs vphysics) | 371/371 | 100.0% | 144671/144671 | 100.0% | 25 | 0 | 0 | 0 | 393/393 | 100.0% |
| ivp_physics.lib (vs vphysics) | 801/811 | 98.77% | 174217/180553 | 96.49% | 197 | 10 | 0 | 0 | 1207/1207 | 100.0% |
| lzma.lib (vs replay) | 0/0 | 0.0% | 0/0 | 0.0% | 0 | 0 | 0 | 0 |  |  |
| choreoobjects.lib (vs client_hl2) | 196/196 | 100.0% | 41098/41098 | 100.0% | 78 | 0 | 0 | 0 | 245/245 | 100.0% |
| particles.lib (vs client_hl2) | 457/460 | 99.35% | 194204/194422 | 99.89% | 114 | 1 | 0 | 2 | 1461/1462 | 99.93% |

### tier0: 203 functions on both platforms, 201 agree; 2 match only on macOS, 0 only on Windows
- macOS only: `ThreadInterlockedExchange` 96 B, other side missing
- macOS only: `ThreadGetCurrentId` 9 B, other side missing

### vstdlib: 149 functions on both platforms, 149 agree; 0 match only on macOS, 0 only on Windows

### scenefilecache: 14 functions on both platforms, 14 agree; 0 match only on macOS, 0 only on Windows

### soundemittersystem: 81 functions on both platforms, 81 agree; 0 match only on macOS, 0 only on Windows

### datacache: 207 functions on both platforms, 202 agree; 5 match only on macOS, 0 only on Windows
- macOS only: `CMDLCache::ProcessDynamicLoad(ModelParts_t*)` 171 B, other side differs
- macOS only: `CThreadTerminalMutex<CThreadFastMutex>::Lock()` 137 B, other side missing
- macOS only: `ConVar::ConVar(charconst*,charconst*,int)` 97 B, other side missing
- macOS only: `clang_call_terminate` 72 B, other side missing
- macOS only: `studiohdr_t::GetVirtualModel()const` 32 B, other side differs

### studiorender: 509 functions on both platforms, 504 agree; 3 match only on macOS, 2 only on Windows
- macOS only: `CStudioRender::R_StudioDrawBones()` 2562 B, other side differs
- macOS only: `CStudioRender::R_StudioFlexVerts(mstudiomesh_t*,int)` 1783 B, other side differs
- macOS only: `CStudioRenderContext::GetTriangles(DrawModelInfo_tconst&,matrix3x4_t*,GetTriangles_Output_t&)` 1781 B, other side differs
- Windows only: `CStudioRender::R_GatherStats(studiomeshgroup_t*,CMeshBuilder&,IMesh*,IMaterial*)` 9668 B, other side differs
- Windows only: `CStudioRenderContext::DrawModel(DrawModelResults_t*,DrawModelInfo_tconst&,matrix3x4_t*,float*,float*,Vectorconst&,int)` 2662 B, other side differs

### inputsystem: 96 functions on both platforms, 93 agree; 3 match only on macOS, 0 only on Windows
- macOS only: `CCallbackImpl<32>::~CCallbackImpl()#D0` 50 B, other side missing
- macOS only: `CCallbackImpl<32>::Run(void*,bool,unsignedlonglong)` 12 B, other side missing
- macOS only: `CCallbackImpl<32>::GetCallbackSizeBytes()` 6 B, other side missing

### engine: 6233 functions on both platforms, 6151 agree; 82 match only on macOS, 0 only on Windows
- macOS only: `CSaveRestore::SaveGameState(bool,CSaveRestoreData**,bool,bool)` 1181 B, other side differs
- macOS only: `CShadowMgr::DrawFlashlightDepthTexture()` 1141 B, other side differs
- macOS only: `IntersectRayWithBox(Ray_tconst&,VectorAlignedconst&,VectorAlignedconst&,VectorAlignedconst&,CGameTrace*)` 1084 B, other side differs
- macOS only: `IntersectRayWithBoxBrush(TraceInfo_t*,cbrush_tconst*,cboxbrush_t*)` 926 B, other side differs
- macOS only: `CSaveRestore::LoadGame(charconst*)` 915 B, other side differs
- macOS only: `Cmd_Exec_f(CCommandconst&)` 902 B, other side differs
- macOS only: `CM_TraceToLeaf<1>(TraceInfo_t*,int,float,float)` 790 B, other side differs
- macOS only: `CNetChan::CNetChan()` 663 B, other side differs
- macOS only: `CDemoRecorder::StartupDemoFile()` 640 B, other side differs
- macOS only: `Mod_LoadLeafs()` 426 B, other side differs
- macOS only: `CErrorText::BuildComment(charconst*,bool)` 422 B, other side differs
- macOS only: `Sys_SpewFunc(SpewType_t,charconst*)` 383 B, other side differs
- macOS only: `NET_GetPacket(int,unsignedchar*)` 365 B, other side differs
- macOS only: `CBaseServer::Clear()` 361 B, other side differs
- macOS only: `CDemoRecorder::WriteBSPDecals()` 340 B, other side differs

### filesystem_stdio: 502 functions on both platforms, 489 agree; 13 match only on macOS, 0 only on Windows
- macOS only: `CBaseFileSystem::UpdateSearchPathsForCurrentGameExpansion()` 1066 B, other side differs
- macOS only: `TrackedFile_t::ProcessFileRead(void*,unsignedlong)` 320 B, other side missing
- macOS only: `CryptoPP::SourceTemplate<CryptoPP::StringStore>::Pump2(unsignedlonglong&,bool)` 128 B, other side differs
- macOS only: `CryptoPP::SourceTemplate<CryptoPP::StringStore>::PumpMessages2(unsignedint&,bool)` 125 B, other side differs
- macOS only: `CryptoPP::SourceTemplate<CryptoPP::StringStore>::PumpAll2(bool)` 122 B, other side differs
- macOS only: `CMemoryFileBacking::~CMemoryFileBacking()#D0` 64 B, other side differs
- macOS only: `CUtlSymbolTableMT::String(CUtlSymbol)const` 53 B, other side missing
- macOS only: `CryptoPP::InputRejecting<CryptoPP::Filter>::Put2(unsignedcharconst*,unsignedlong,int,bool)` 28 B, other side missing
- macOS only: `clang_call_terminate` 20 B, other side missing
- macOS only: `CryptoPP::IteratedHashWithStaticTransform<unsignedint,CryptoPP::EnumToType<CryptoPP::ByteOrder,1>,64u,32u,CryptoPP::SHA256,32u,1>::StateBuf()` 7 B, other side missing
- macOS only: `CryptoPP::IteratedHashWithStaticTransform<unsignedint,CryptoPP::EnumToType<CryptoPP::ByteOrder,1>,64u,32u,CryptoPP::SHA256,32u,1>::DigestSize()const` 6 B, other side missing
- macOS only: `CryptoPP::IteratedHash<unsignedint,CryptoPP::EnumToType<CryptoPP::ByteOrder,1>,64u,CryptoPP::HashTransformation>::BlockSize()const` 6 B, other side missing
- macOS only: `CryptoPP::IteratedHash<unsignedint,CryptoPP::EnumToType<CryptoPP::ByteOrder,1>,64u,CryptoPP::HashTransformation>::DataBuf()` 4 B, other side missing

### materialsystem: 1363 functions on both platforms, 1350 agree; 13 match only on macOS, 0 only on Windows
- macOS only: `CMatLightmaps::BumpedLightmapBitsToPixelWriter_HDRI(float*,float*,float*,float*,int*,int*,FloatBitMap_t*)` 5147 B, other side differs
- macOS only: `CMatLightmaps::LightmapBitsToPixelWriter_HDRI(float*,int*,int*,FloatBitMap_t*)` 1374 B, other side differs
- macOS only: `CTexture::LoadTextureBitsFromFile(char*,char**,bool)` 1305 B, other side differs
- macOS only: `CMaterialSystem::CMaterialSystem()` 1067 B, other side differs
- macOS only: `CTexture::ComputeActualSize(bool,IVTFTexture*)` 661 B, other side differs
- macOS only: `CMaterialSubRect::ParseMaterialVars(KeyValues&)` 635 B, other side differs
- macOS only: `CTexture::Precache()` 427 B, other side differs
- macOS only: `CUtlVector<CUtlVector<CMorph::MorphSegment_t,CUtlMemory<CMorph::MorphSegment_t,int>>,CUtlMemory<CUtlVector<CMorph::MorphSegment_t,CUtlMemory<CMorph::MorphSegment_t,int>>,int>>::Purge()` 145 B, other side missing
- macOS only: `CTexture::GetTextureHandle(int,int)` 53 B, other side differs
- macOS only: `CUtlSymbolTableMT::String(CUtlSymbol)const` 53 B, other side missing
- macOS only: `clang_call_terminate` 20 B, other side missing
- macOS only: `CMaterialVar::operatorITexture*()` 4 B, other side missing
- macOS only: `CDummyMesh::Draw(int,int)` 3 B, other side differs

### shaderapidx9: 652 functions on both platforms, 649 agree; 3 match only on macOS, 0 only on Windows
- macOS only: `CShaderDeviceDx8::InvokeCreateDevice(void*,int,unsignedint)` 390 B, other side missing
- macOS only: `CIndexBufferBase::CIndexBufferBase(charconst*)` 72 B, other side differs
- macOS only: `clang_call_terminate` 20 B, other side missing

### stdshader_dx9: 1402 functions on both platforms, 1400 agree; 2 match only on macOS, 0 only on Windows
- macOS only: `Spritecard::CShader::OnDrawElements(IMaterialVar**,IShaderShadow*,IShaderDynamicAPI*,VertexCompressionType_t,CBasePerMaterialContextData**)` 2501 B, other side differs
- macOS only: `InitParamsVertexLitGeneric_DX9(CBaseVSShader*,IMaterialVar**,charconst*,bool,VertexLitGeneric_DX9_Vars_t&)` 2028 B, other side differs

### vgui2: 364 functions on both platforms, 361 agree; 3 match only on macOS, 0 only on Windows
- macOS only: `CScheme::LoadFonts()` 1092 B, other side differs
- macOS only: `CSchemeManager::LoadSchemeFromFileEx(unsignedint,charconst*,charconst*)` 392 B, other side differs
- macOS only: `KeyValues::KeyValues(charconst*)` 84 B, other side missing

### vguimatsurface: 266 functions on both platforms, 264 agree; 2 match only on macOS, 0 only on Windows
- macOS only: `CUtlRBTree<CUtlMap<charconst*,int,int>::Node_t,int,CUtlMap<charconst*,int,int>::CKeyLess,CUtlMemory<UtlRBTreeNode_t<CUtlMap<charconst*,int,int>::Node_t,int>,int>>::InsertAt(int,bool)` 115 B, other side differs
- macOS only: `KeyValues::KeyValues(charconst*)` 84 B, other side missing

### GameUI: 1415 functions on both platforms, 1350 agree; 65 match only on macOS, 0 only on Windows
- macOS only: `CBasePanel::RunMenuCommand(charconst*)` 2741 B, other side differs
- macOS only: `CBasePanel::CBasePanel()` 2190 B, other side differs
- macOS only: `CBasePanel::RefreshDialogs()` 857 B, other side differs
- macOS only: `vgui::PanelListPanel::PanelListPanel(vgui::Panel*,charconst*)` 753 B, other side missing
- macOS only: `vgui::PanelListPanel::PerformLayout()` 603 B, other side missing
- macOS only: `vgui::CBitmapImagePanel::ApplySettings(KeyValues*)` 524 B, other side missing
- macOS only: `vgui::CBitmapImagePanel::ComputeImagePosition(int&,int&,int&,int&)` 460 B, other side missing
- macOS only: `COptionsSubVideo::OnTextChanged(vgui::Panel*,charconst*)` 349 B, other side differs
- macOS only: `CUtlLinkedList<vgui::PanelListPanel::dataitem_s,int,0,int,CUtlMemory<UtlLinkedListElem_t<vgui::PanelListPanel::dataitem_s,int>,int>>::AllocInternal(bool)` 298 B, other side missing
- macOS only: `vgui::PanelListPanel::AddItem(vgui::Panel*,vgui::Panel*)` 255 B, other side missing
- macOS only: `vgui::PanelListPanel::ComputeVPixelsNeeded()` 213 B, other side missing
- macOS only: `vgui::CBitmapImagePanel::GetSettings(KeyValues*)` 193 B, other side missing
- macOS only: `vgui::PanelListPanel::SetSelectedPanel(vgui::Panel*)` 191 B, other side missing
- macOS only: `vgui::PanelListPanel::ScrollToItem(int)` 190 B, other side missing
- macOS only: `vgui::CBitmapImagePanel::CBitmapImagePanel(vgui::Panel*,charconst*,charconst*)` 184 B, other side missing

### ServerBrowser: 397 functions on both platforms, 390 agree; 7 match only on macOS, 0 only on Windows
- macOS only: `CUtlMap<charconst*,CQuickListMapServerList,int>::Find(charconst*const&)const` 208 B, other side missing
- macOS only: `vgui::URLLabel::GetMessageMap()` 42 B, other side missing
- macOS only: `vgui::QueryBox::GetKBMap()` 42 B, other side missing
- macOS only: `vgui::URLLabel::GetKBMap()` 42 B, other side missing
- macOS only: `vgui::QueryBox::GetMessageMap()` 42 B, other side missing
- macOS only: `vgui::QueryBox::GetAnimMap()` 14 B, other side missing
- macOS only: `vgui::URLLabel::GetAnimMap()` 14 B, other side missing

### vphysics: 558 functions on both platforms, 554 agree; 3 match only on macOS, 1 only on Windows
- macOS only: `CreatePhysicsObject(CPhysicsEnvironment*,CPhysCollideconst*,int,Vectorconst&,QAngleconst&,objectparams_t*,bool)` 838 B, other side differs
- macOS only: `CSurface::~CSurface()` 33 B, other side missing
- macOS only: `clang_call_terminate` 20 B, other side missing
- Windows only: `PackLedgeIntoBuffer(packedhull_t*,CUtlBuffer&,IVP_Compact_Ledgeconst*,virtualmeshlist_tconst&)` 1988 B, other side differs

### replay: 581 functions on both platforms, 580 agree; 1 match only on macOS, 0 only on Windows
- macOS only: `clang_call_terminate` 20 B, other side missing

### launcher: 61 functions on both platforms, 60 agree; 1 match only on macOS, 0 only on Windows
- macOS only: `LauncherMain` 1603 B, other side differs

### launcher_main: 0 functions on both platforms, 0 agree; 0 match only on macOS, 0 only on Windows

### client_hl2: 6655 functions on both platforms, 6631 agree; 22 match only on macOS, 2 only on Windows
- macOS only: `CHudHintKeyDisplay::SetHintText(charconst*)` 5180 B, other side differs
- macOS only: `ParseParticleEffects(bool,bool)` 684 B, other side differs
- macOS only: `CInput::UpdateQuickTurnAndCameraReset(QAngle&,float)` 403 B, other side differs
- macOS only: `CInput::ScaleAxisValue(float,float)` 387 B, other side differs
- macOS only: `CHud::AddSearchableHudIconToList(CHudTexture&)` 251 B, other side differs
- macOS only: `CInterpolatedVar<Vector>::CInterpolatedVar(charconst*)` 149 B, other side differs
- macOS only: `CMapOverview::GetBounds(int&,int&,int&,int&)#thunk420` 137 B, other side missing
- macOS only: `ILocalize::ConstructString<char>(char*,int,charconst*,int,...)` 108 B, other side differs
- macOS only: `CGameStringPool::FreeAll()` 100 B, other side missing
- macOS only: `CUtlLinkedList<CUtlMultiList<CPixelVisSet,unsignedshort>::List_t,unsignedshort,0,unsignedshort,CUtlMemory<UtlLinkedListElem_t<CUtlMultiList<CPixelVisSet,unsignedshort>::List_t,unsignedshort>,unsignedshort>>::Purge()` 96 B, other side differs
- macOS only: `CModelPanelModelAnimation::~CModelPanelModelAnimation()` 91 B, other side missing
- macOS only: `C_FireSmoke::OnDataChanged(DataUpdateType_t)#thunk8` 77 B, other side missing
- macOS only: `vgui::Panel::SetScheme(charconst*)` 74 B, other side missing
- macOS only: `C_BaseEntity::NotifyShouldTransmit(ShouldTransmitState_t)#thunk8` 61 B, other side missing
- macOS only: `CMDLPanel::GetKBMap()` 42 B, other side missing
- Windows only: `CSteamTimelineEvent_WeaponEquipped::HandleEvent(CSteamShareSystem*,IGameEvent*)` 608 B, other side differs
- Windows only: `CSteamTimelineEvent_InventoryUpdated::AddCurrentInventoryToGamePhase(IGameEvent*)` 498 B, other side differs

### server_hl2: 14285 functions on both platforms, 14244 agree; 41 match only on macOS, 0 only on Windows
- macOS only: `CBaseFlex::ProcessMoveToSceneEvent(CSceneEventInfo*,CChoreoScene*,CChoreoEvent*)` 1596 B, other side differs
- macOS only: `CBasePlayer::ClientCommand(CCommandconst&)` 1540 B, other side differs
- macOS only: `CNPC_BaseZombie::RunTask(Task_tconst*)` 750 B, other side differs
- macOS only: `ParseParticleEffects(bool,bool)` 684 B, other side differs
- macOS only: `CNPC_Vortigaunt::TryRestoreHull()` 633 B, other side differs
- macOS only: `WriteAchievementGlobalState(KeyValues*,bool)` 552 B, other side differs
- macOS only: `CNPC_Vortigaunt::RangeAttack1Conditions(float,float)` 516 B, other side differs
- macOS only: `CAchievementMgr::LoadGlobalState()` 509 B, other side differs
- macOS only: `CMultiplayRules::DeathNotice(CBasePlayer*,CTakeDamageInfoconst&)` 439 B, other side differs
- macOS only: `CPointCamera::CPointCamera()` 409 B, other side differs
- macOS only: `CBaseCombatWeapon::ItemPreFrame()` 258 B, other side differs
- macOS only: `CUtlVector<CUtlVector<ikcontextikrule_t,CUtlMemory<ikcontextikrule_t,int>>,CUtlMemory<CUtlVector<ikcontextikrule_t,CUtlMemory<ikcontextikrule_t,int>>,int>>::Purge()` 145 B, other side missing
- macOS only: `HasEpisodicData()` 141 B, other side differs
- macOS only: `CAI_Hint::GetPosition(CBaseCombatCharacter*,Vector*)` 137 B, other side differs
- macOS only: `CPASAttenuationFilter::CPASAttenuationFilter(CBaseEntity*,charconst*)` 133 B, other side differs

### client_episodic: 6664 functions on both platforms, 6644 agree; 20 match only on macOS, 0 only on Windows
- macOS only: `CHudHintKeyDisplay::SetHintText(charconst*)` 5180 B, other side differs
- macOS only: `ParseParticleEffects(bool,bool)` 684 B, other side differs
- macOS only: `CInput::UpdateQuickTurnAndCameraReset(QAngle&,float)` 403 B, other side differs
- macOS only: `CInput::ScaleAxisValue(float,float)` 387 B, other side differs
- macOS only: `CHud::AddSearchableHudIconToList(CHudTexture&)` 251 B, other side differs
- macOS only: `CInterpolatedVar<Vector>::CInterpolatedVar(charconst*)` 149 B, other side differs
- macOS only: `CMapOverview::GetBounds(int&,int&,int&,int&)#thunk420` 137 B, other side missing
- macOS only: `CGameStringPool::FreeAll()` 100 B, other side missing
- macOS only: `CUtlLinkedList<CUtlMultiList<CPixelVisSet,unsignedshort>::List_t,unsignedshort,0,unsignedshort,CUtlMemory<UtlLinkedListElem_t<CUtlMultiList<CPixelVisSet,unsignedshort>::List_t,unsignedshort>,unsignedshort>>::Purge()` 96 B, other side differs
- macOS only: `CModelPanelModelAnimation::~CModelPanelModelAnimation()` 91 B, other side missing
- macOS only: `vgui::Panel::SetScheme(charconst*)` 74 B, other side missing
- macOS only: `C_BaseEntity::NotifyShouldTransmit(ShouldTransmitState_t)#thunk8` 61 B, other side missing
- macOS only: `CMDLPanel::GetKBMap()` 42 B, other side missing
- macOS only: `CPotteryWheelPanel::GetKBMap()` 42 B, other side missing
- macOS only: `CMDLPanel::GetMessageMap()` 42 B, other side missing

### server_episodic: 14429 functions on both platforms, 14394 agree; 35 match only on macOS, 0 only on Windows
- macOS only: `CBaseFlex::ProcessMoveToSceneEvent(CSceneEventInfo*,CChoreoScene*,CChoreoEvent*)` 1596 B, other side differs
- macOS only: `CBasePlayer::ClientCommand(CCommandconst&)` 1540 B, other side differs
- macOS only: `CNPC_BaseZombie::RunTask(Task_tconst*)` 750 B, other side differs
- macOS only: `ParseParticleEffects(bool,bool)` 684 B, other side differs
- macOS only: `CNPC_Vortigaunt::TryRestoreHull()` 633 B, other side differs
- macOS only: `WriteAchievementGlobalState(KeyValues*,bool)` 552 B, other side differs
- macOS only: `CNPC_Vortigaunt::RangeAttack1Conditions(float,float)` 516 B, other side differs
- macOS only: `CAchievementMgr::LoadGlobalState()` 509 B, other side differs
- macOS only: `CMultiplayRules::DeathNotice(CBasePlayer*,CTakeDamageInfoconst&)` 439 B, other side differs
- macOS only: `CPointCamera::CPointCamera()` 409 B, other side differs
- macOS only: `CBaseCombatWeapon::ItemPreFrame()` 258 B, other side differs
- macOS only: `CUtlVector<CUtlVector<ikcontextikrule_t,CUtlMemory<ikcontextikrule_t,int>>,CUtlMemory<CUtlVector<ikcontextikrule_t,CUtlMemory<ikcontextikrule_t,int>>,int>>::Purge()` 145 B, other side missing
- macOS only: `HasEpisodicData()` 141 B, other side differs
- macOS only: `CPASAttenuationFilter::CPASAttenuationFilter(CBaseEntity*,charconst*)` 133 B, other side differs
- macOS only: `CAI_NamespaceInfos::Compare(AI_NamespaceAddInfo_tconst*,AI_NamespaceAddInfo_tconst*)` 132 B, other side differs

### tier1.lib (vs engine): 389 functions on both platforms, 368 agree; 21 match only on macOS, 0 only on Windows
- macOS only: `KeyValues::RecursiveLoadFromBuffer(charconst*,CUtlBuffer&)` 940 B, other side differs
- macOS only: `snappy::Compress(snappy::Source*,snappy::Sink*)` 537 B, other side differs
- macOS only: `CUtlRBTree<CUtlSymbolTable::CStringPoolIndex,unsignedshort,CUtlSymbolTable::CLess,CUtlMemory<UtlRBTreeNode_t<CUtlSymbolTable::CStringPoolIndex,unsignedshort>,unsignedshort>>::Insert(CUtlSymbolTable::CStringPoolIndexconst&)` 462 B, other side differs
- macOS only: `KeyValues::GetWString(charconst*,wchar_tconst*)` 273 B, other side differs
- macOS only: `CUtlBuffer::PutString(charconst*)` 223 B, other side differs
- macOS only: `ConVar::ChangeStringValue(charconst*,float)` 181 B, other side differs
- macOS only: `bf_read::bf_read()` 148 B, other side differs
- macOS only: `KeyValues::RemoveEverything()` 142 B, other side differs
- macOS only: `KeyValues::SetWString(charconst*,wchar_tconst*)` 138 B, other side differs
- macOS only: `KeyValues::SetString(charconst*,charconst*)` 121 B, other side differs
- macOS only: `KeyValues::SetStringValue(charconst*)` 95 B, other side differs
- macOS only: `bf_read::Reset()` 94 B, other side differs
- macOS only: `const*()const` 90 B, other side missing
- macOS only: `KeyValues::operatordelete(void*)` 88 B, other side missing
- macOS only: `ConVar::~ConVar()#D0` 70 B, other side differs

### tier2.lib (vs engine): 47 functions on both platforms, 43 agree; 4 match only on macOS, 0 only on Windows
- macOS only: `RenderWireframeSweptBox(Vectorconst&,Vectorconst&,QAngleconst&,Vectorconst&,Vectorconst&,Color,bool)` 1092 B, other side differs
- macOS only: `CUtlStreamBuffer::CUtlStreamBuffer(charconst*,charconst*,int,bool)` 283 B, other side differs
- macOS only: `CUtlStreamBuffer::Close()` 200 B, other side differs
- macOS only: `CP4File::~CP4File()#D0` 74 B, other side differs

### tier3.lib (vs engine): 2 functions on both platforms, 2 agree; 0 match only on macOS, 0 only on Windows

### bitmap.lib (vs engine): 23 functions on both platforms, 20 agree; 3 match only on macOS, 0 only on Windows
- macOS only: `ImageLoader::ConvertImageFormat(unsignedcharconst*,ImageFormat,unsignedchar*,ImageFormat,int,int,int,int)` 1752 B, other side differs
- macOS only: `ImageLoader::ResampleRGBA8888(ImageLoader::ResampleInfo_tconst&)` 990 B, other side differs
- macOS only: `FloatBitMap_t::~FloatBitMap_t()` 15 B, other side differs

### mathlib.lib (vs engine): 86 functions on both platforms, 85 agree; 1 match only on macOS, 0 only on Windows
- macOS only: `IceKey::~IceKey()` 84 B, other side differs

### replay_common.lib (vs engine): 6 functions on both platforms, 6 agree; 0 match only on macOS, 0 only on Windows

### appframework.lib (vs engine): 31 functions on both platforms, 30 agree; 1 match only on macOS, 0 only on Windows
- macOS only: `LoadExpansionInfo(charconst*,char*,unsignedlong)` 235 B, other side missing

### bzip2.lib (vs engine): 22 functions on both platforms, 16 agree; 6 match only on macOS, 0 only on Windows
- macOS only: `BZ2_bzWriteClose64` 488 B, other side differs
- macOS only: `bzopen_or_bzdopen(charconst*,int,charconst*,int)` 472 B, other side differs
- macOS only: `BZ2_bzReadOpen` 352 B, other side differs
- macOS only: `BZ2_bzWriteOpen` 321 B, other side differs
- macOS only: `BZ2_bzclose` 168 B, other side differs
- macOS only: `BZ2_bzReadClose` 118 B, other side differs

### dmxloader.lib (vs engine): 46 functions on both platforms, 45 agree; 1 match only on macOS, 0 only on Windows
- macOS only: `CUtlSymbolTableMT::String(CUtlSymbol)const` 53 B, other side missing

### matsys_controls.lib (vs engine): 17 functions on both platforms, 15 agree; 2 match only on macOS, 0 only on Windows
- macOS only: `CProceduralTexturePanel::~CProceduralTexturePanel()` 123 B, other side differs
- macOS only: `CProceduralTexturePanel::Shutdown()` 77 B, other side differs

### vtf.lib (vs engine): 68 functions on both platforms, 63 agree; 5 match only on macOS, 0 only on Windows
- macOS only: `CVTFTexture::ConvertImageFormat(ImageFormat,bool)` 631 B, other side differs
- macOS only: `CVTFTexture::Init(int,int,int,ImageFormat,int,int,int)` 341 B, other side differs
- macOS only: `CVTFTexture::LoadImageData(CUtlBuffer&,VTFFileHeader_tconst&,int)` 303 B, other side differs
- macOS only: `CVTFTexture::Shutdown()` 119 B, other side differs
- macOS only: `CVTFTexture::InitLowResImage(int,int,ImageFormat)` 100 B, other side differs

### vgui_controls.lib (vs engine): 1392 functions on both platforms, 1362 agree; 30 match only on macOS, 0 only on Windows
- macOS only: `vgui::TextEntry::CreateEditMenu()` 1441 B, other side differs
- macOS only: `vgui::Label::ApplySettings(KeyValues*)` 1172 B, other side differs
- macOS only: `vgui::ImagePanel::ApplySettings(KeyValues*)` 1031 B, other side differs
- macOS only: `vgui::BuildGroup::~BuildGroup()` 846 B, other side differs
- macOS only: `vgui::ListPanel::~ListPanel()` 795 B, other side differs
- macOS only: `vgui::Label::Paint()` 703 B, other side differs
- macOS only: `vgui::CBitmapImagePanel::ApplySettings(KeyValues*)` 524 B, other side differs
- macOS only: `vgui::BuildGroup::LoadControlSettings(charconst*,charconst*,KeyValues*,KeyValues*)` 483 B, other side differs
- macOS only: `vgui::ScalableImagePanel::ApplySettings(KeyValues*)` 478 B, other side differs
- macOS only: `vgui::ListPanel::RemoveItem(int)` 454 B, other side differs
- macOS only: `vgui::RichText::ApplySettings(KeyValues*)` 398 B, other side differs
- macOS only: `vgui::Panel::PinToSibling(charconst*,vgui::Panel::PinCorner_e,vgui::Panel::PinCorner_e)` 249 B, other side differs
- macOS only: `vgui::Panel::AddKeyBinding(charconst*,int,int)` 229 B, other side differs
- macOS only: `vgui::Button::OnKeyCodePressed(ButtonCode_t)` 197 B, other side differs
- macOS only: `vgui::ScalableImagePanel::SetImage(charconst*)` 189 B, other side differs

### vpklib.lib (vs filesystem_stdio): 38 functions on both platforms, 38 agree; 0 match only on macOS, 0 only on Windows

### shaderlib.lib (vs materialsystem): 8 functions on both platforms, 8 agree; 0 match only on macOS, 0 only on Windows

### vgui_surfacelib.lib (vs vgui2): 0 functions on both platforms, 0 agree; 0 match only on macOS, 0 only on Windows

### havana_constraints.lib (vs vphysics): 70 functions on both platforms, 67 agree; 3 match only on macOS, 0 only on Windows
- macOS only: `hk_Ragdoll_Constraint::setup_and_step_constraint(hk_PSI_Info&,void*,float,float)` 2521 B, other side differs
- macOS only: `BreakableConstraintHelper::BreakableConstraintHelper(hk_Rigid_Body*,float)` 329 B, other side differs
- macOS only: `hk_Local_Constraint_System::hk_Local_Constraint_System(hk_Environment*,hk_Local_Constraint_System_BP*)` 125 B, other side differs

### hk_base.lib (vs vphysics): 4 functions on both platforms, 4 agree; 0 match only on macOS, 0 only on Windows

### hk_math.lib (vs vphysics): 14 functions on both platforms, 14 agree; 0 match only on macOS, 0 only on Windows

### ivp_compactbuilder.lib (vs vphysics): 366 functions on both platforms, 366 agree; 0 match only on macOS, 0 only on Windows

### ivp_physics.lib (vs vphysics): 800 functions on both platforms, 790 agree; 10 match only on macOS, 0 only on Windows
- macOS only: `IVP_Contact_Point::recalc_friction_s_vals()` 1776 B, other side differs
- macOS only: `IVP_Impact_Solver::do_impact(IVP_Core**,IVP_BOOL,int,float)` 1602 B, other side differs
- macOS only: `IVP_Contact_Point::friction_force_local_constraint_2d_wheel(IVP_Core*,IVP_Impact_Solver_Long_Term*,IVP_Event_Simconst*,float&)` 1340 B, other side differs
- macOS only: `IVP_Controller_Raycast_Car::CalcSteeringForces(IVP_Raycast_Car_Wheel_Temp*,IVP_Core*,IVP_Event_Sim*,float*)` 442 B, other side differs
- macOS only: `IVP_Sim_Units_Manager::simulate_sim_units_psi(IVP_Environment*,IVP_U_Vector<IVP_Core>*)` 375 B, other side differs
- macOS only: `IVP_U_Quat::set_interpolate_smoothly(IVP_U_Quatconst*,IVP_U_Quatconst*,double)` 360 B, other side differs
- macOS only: `IVP_Core::apply_velocity_limit()` 304 B, other side differs
- macOS only: `IVP_Mindist_Recursive::invalid_mindist_went_exact()` 295 B, other side differs
- macOS only: `IVP_U_Float_Point::real_length()const` 191 B, other side differs
- macOS only: `IVP_Car_System_Real_Wheels::get_body_speed(IVP_COORDINATE_INDEX)` 66 B, other side differs

### choreoobjects.lib (vs client_hl2): 196 functions on both platforms, 196 agree; 0 match only on macOS, 0 only on Windows

### particles.lib (vs client_hl2): 459 functions on both platforms, 456 agree; 3 match only on macOS, 0 only on Windows
- macOS only: `C_OP_RenderSprites::GetParticlesToRender(CParticleCollection*,void*,int,int,int,int*,int*)const` 241 B, other side differs
- macOS only: `CUtlStringMap<CParticleSystemDefinition*>::operator[](charconst*)` 99 B, other side missing
- macOS only: `CParticleOperatorInstance::operatornew(unsignedlong)` 41 B, other side missing
