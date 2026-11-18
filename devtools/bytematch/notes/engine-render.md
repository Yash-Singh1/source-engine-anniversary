# engine-render (engine.dylib, render/client file set)

Retail: depot 223 `bin/engine.dylib`, objects scored per debug-map stem with
`devtools/bytematch` (retail compile lines from `build-retail/compile_commands.json`).
"With pending" means measured with three changes from other lanes in place: the
engine-core lane's `baseclientstate.h` (6471c6ec), SDK 2013's
`CFixedBitVecBase::FindNextSetBit` loop in `public/bitvec.h`, and
`IVideoMode::NoWindowBorder` (now in this branch, retail mode only).

| object | functions before | after | with pending | bytes before | after |
|---|---:|---:|---:|---:|---:|
| modelloader | 109/169 | 162/169 | 162 | 43.5% | 91.8% |
| r_decal | 67/74 | 74/74 | 74 | 49.8% | 100.0% |
| gl_rsurf | 93/97 | 96/97 | 96 | 83.6% | 97.2% |
| cl_main | 75/94 | 87/94 | 87 | 64.1% | 94.0% |
| cl_texturelistpanel | 141/145 | 144/145 | 144 | 83.1% | 96.4% |
| vgui_baseui_interface | 59/87 | 83/87 | 83 | 23.1% | 66.6% |
| cl_foguipanel | 9/16 | 16/16 | 16 | 8.3% | 100.0% |
| cl_demo | 58/78 | 72/78 | 78 | 38.8% | 70.3% |
| l_studio | 87/95 | 94/95 | 94 | 79.9% | 91.4% |
| client | 20/55 | 21/55 | 54 | 17.2% | 20.2% |
| cl_ents_parse | 6/10 | 6/10 | 10 | 17.5% | 17.5% |
| staticpropmgr | 217/220 | 218/220 | 218 | 90.1% | 94.1% |

After merging bytematch 88902342 (engine-core's CBaseClientState, headers3:
utlvector, Steam headers, MAX_MAP_NAME), branch alone: file set 2528/2564
functions (98.6%), up from 2294; client 51/55 (55 with the bitvec.h loop),
modelloader 167/169, cl_main 92/94, vgui_baseui_interface 84/87, and cl_demo,
cl_ents_parse, cl_steamauth, cl_texturelistpanel, staticpropmgr, view,
r_areaportal, vgui_helpers at 100%.

Smaller objects gained through the shared fixes (`CanCheat`, `IModelLoader`,
`IVideoMode` slots): vgui_texturebudgetpanel, gl_rmisc, cl_demoaction_types and
vgui_budgetpanel now 100%; vgui_vprofpanel, cl_entityreport, gl_matsysiface, view,
vgui_vprofgraphpanel and vgui_DebugSystemPanel +1 or +2.

## Edited to retail (all retail-mode differences are under `BYTEMATCH_RETAIL`)

| function | status | note |
|---|---|---|
| CMapLoadHelper ctor/dtor, LumpSize(int) | matched | no LZMA lump decompression; `m_pUncompressedData` slot kept but unused |
| Mod_LoadGameLumpDict, Mod_LoadGameLump, Mod_GameLumpSize/Version | matched | SDK-era `CUtlVector<dgamelump_t>`; compressed game lumps are read and then rejected (retail links no LZMA) |
| Mod_LoadFaces/Leafs/Cubemap…, Map_CheckForHDR, Mod_LoadMap lighting | matched | `GetHDRType() != HDR_TYPE_NONE` lump choice; `mat_hdr_level >= 2` in Map_CheckForHDR |
| EnableHDR (inlined) | matched | pre-JohnS: no `mod_offline_hdr_switch` |
| CMapLoadHelper::Init/InitFromMemory, GetDiskName | matched | 64-byte map names, `GetMapPathNameOnDisk` helper (PC: copy if different), new `GetDiskName` |
| CModelLoader::Map_IsValid | matched | one argument (no bQuiet), no fork `maps/`/`.bsp` fixup; `IModelLoader` keeps an inline two-argument forwarder for engine-core callers |
| IModelLoader::ResetModelServerCounts | matched (layout) | no slot in retail; empty inline in retail mode so engine-core's sv_main call compiles to nothing |
| LoadModel, UnloadModel | matched | on-disk name for Add/RemoveSearchPath |
| InternalUpdateDynamicModels | edited, differs | `UncacheUnusedMaterials(false)` queued through the call queue; remaining diff is `sv` (CGameServer) layout |
| UpdateDynamicModelLoadQueue, PurgeAll, PurgeUnreferencedResources | matched | no `CleanupDynamicLoad` (same hunk as headers2) |
| Mod_LoadWorldlights, StaticPropMgr::UnserializeModels | matched / differs | 88-byte `dworldlight_t`, SDK 2013 static prop lump (v6 layout, versions 4-6); UnserializeModels only differs by `CUtlVector::InsertMultipleBefore` (utlvector.h) |
| R_DecalShoot_, R_DecalSurface | matched | fade from `$decalFadeDuration`/`$decalFadeTime` set at shoot time |
| CanCheat (gl_cvars.h) | matched | retail allows RPT (`sv_cheats || Cmd_IsRptActive()`); fixes every wireframe check |
| CClientState layout, ctor, Clear, Get/SetModel, SetSound, ReadPreserveEnt | matched with pending | no `m_bPrepareClientDLL`, no `m_pPendingPureFileReloads`; `cl_forcepreload`; MAX_MODELS bound in SetModel |
| FinishSignonState_New, Check/StartUpdatingSteamResources | edited | SDK-era flow, no `cl_always_flush_models` |
| CL_HandlePureServerWhitelist, CL_CheckForPureServerWhitelist | matched | one-argument SDK-era versions with the reload inline |
| CL_FullyConnected, CL_ClearState, CL_Connect | matched | `R_BuildCubemapSamples` for `-buildcubemaps`; no dev_loadtime, PhonemeMP3Shutdown, UpdateMaterialSystemConfig |
| CL_GetBackgroundLevelIndex/Name, CL_ShouldLoadBackgroundLevel | matched | anniversary `hl2_random_chapter_index`, `hl2_expansion`, `sv_unlockedchapters_*`; force loads the background map, end-game video then queues map_background, `-nobackground`, `+snd_buildcache` |
| CL_InitLanguageCvar, CL_InitHL2DemoFlag, CL_InitPortalDemoFlag, CL_ChangeCloudSettingsCvar, CL_Init | edited, differs | logic retail; bytes need Steamworks >= 1.60 context accessors |
| cl_main static init | matched | no `redirect` command |
| CDemoPlayer layout, ReadPacket, CDemoRecorder Startup/Close/RecordStringTables, WriteTimeDemoResults | matched | no skip-packet counter, single-buffer string tables, no OnDemoRecordStart/Stop, no filename sanitising, `r_lightmap_bicubic` column |
| CFogUIPanel (all) | matched | radial fog check buttons, first-slider resync flag |
| SetRootLOD_f, CreateInstance, Load/ValidateStaticPropColorData | matched | anniversary root LOD drives `cl_detaildist`/`r_lod`; pristine static lighting; GetHDRType |
| l_studio, modelloader, gl_rsurf static init | matched | `mat_specular_scalar`; flags of r_proplightingfromdisk, mat_excludetextures, r_fastzreject |
| ShallWarnTx, OnTurnedOn | matched | no DXT5_RUNTIME, no MakePopup |
| R_LoadSkys, GetScreenAspect | matched | `_hdr` sky falls back to LDR name without HDR; CMatRenderContextPtr |
| CL_CopyNewEntity, CL_PreserveExistingEntity | matched with pending / removed | upper bound only; no CL_PreserveExistingEntity in retail |
| CEngineVGui static sizing panel, UpdateStaticSizingPanelSize, GetPanel, Paint | matched | `vgui_ui_scale_width/height/factor`; scheme loaded against the sizing panel; GameUI panel uses that scheme |
| HideGameUI, OnLevelLoadingStarted/Finished | matched | `gameui_hide`, `loading_level` (is_loading) client events |
| ActivateGameUI | edited, differs | `gameui_activate` event, menu Steam controller action set: retail passes 2, ours `GAME_ACTION_SET_MENUCONTROLS` = 0 |
| CEngineVGui::Init | edited, differs | everything except the Steam Deck background-map check, which needs `GetPlatformMapPath` (retail host.o) |

## Not done

| function | reason |
|---|---|
| CEngineVGui::Key_Event, Simulate | button-code ranges come from retail's ButtonCode_t (input headers) |
| CRender::ViewDrawFade, CShadowMgr::DrawFlashlightDepthTexture | inlined CMeshBuilder code differs |
| UpdateStaticPropColorData, Overlay_Tri*, DrawLightSprite, R_AddDynamicLights, R_DrawSkyBox, R_DrawBrushModel, CreateResource, Mod_LoadMarksurfaces, SetupSubModels | codegen/register allocation, no source difference found |
| CRenderTextureEditor::OnCommand | retail string references do not resolve cleanly |
| cl_steamauth, Steam accessors in cl_main | Steamworks >= 1.60 headers |
