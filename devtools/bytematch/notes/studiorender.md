# studiorender

Retail: depot 223 `bin/studiorender.dylib`; own objects flexrenderdata,
r_studio, r_studiodecal, r_studiodraw, r_studioflex, r_studiogettriangles,
r_studiolight, r_studiostats, studiorender, studiorendercontext (881
functions). `r_studiodraw_computeflexedvertex.cpp` is Windows-only (no retail
object). tier1/2/3 and mathlib members belong to other lanes.

Verification: each TU compiled with `devtools/bytematch/macos-cc.sh`
(clang-600.0.57, TARGET.md Release flags plus `-funswitch-loops` and the
module's VPC defines) and compared per N_OSO object with `bytematch diff`;
"yes" means exact or matches-after-masking with call targets checked. The
strict check from lane-mod-small (`modcmp.py`, which also resolves data
references) gives the same totals. Tooling and patches:
/Volumes/Storage/sco/byte-match-survey/lane-mod-datacache-scratch/ (README.md).

Columns: **orig** = source at 7896bcef with current headers, **now** = this
branch with current headers, **+hdr** = this branch with `hdrfix2.patch`
(SDK 2013 / retail versions of tier0, tier1, mathlib and materialsystem
headers owned by other lanes; see the lane report). "edited" = the function's
source on this branch differs from 7896bcef, including whole files replaced
with their SDK 2013 versions.

Totals (matched / 881): orig 651, now 663, +hdr 859 (orig source +hdr: 803).
identical 827 (514 of them are the r_studiolight light-function template
instances, summarised rather than listed), edited 32, still differing 22.

Source changes on this branch: SDK 2013 `public/optimize.h` (no MDL v49 strip
structs, `char boneID`) and no v49 handling in `R_StudioBuildMeshStrips`;
`R_StudioSetupModel` without the missing-bodypart warning;
`R_StudioSetupSkinAndLighting` without `$lightmap`; `$modelmaterial` lookup
back in `CStudioRender::AddDecal` (no `GetModelSpecificDecalMaterial`);
`memcpy` in `CopyLocalLightingState` / `SetAmbientLightColors`; scalar delete
in `R_StudioDestroyStaticMeshes`; `FastVertexSSE` in the SIMD mesh paths on
32-bit.

| object | function | status | orig | now | +hdr | note |
|---|---|---|---|---|---|---|
| flexrenderdata | `CUtlMemory<CUtlVector<CCachedRenderData::CacheDict_t, CUtlMemory<CCachedRenderData::CacheDict_t, int>>, int>::Grow(int)` | identical | no | no | yes |  |
| flexrenderdata | `CUtlMemory<CUtlVector<CUtlVector<CCachedRenderData::CacheDict_t, CUtlMemory<CCachedRenderData::CacheDict_t, int>>, CUtlMemory<CUtlVector<CCachedRenderData::CacheDict_t, CUtlMemory<CCachedRenderData::CacheDict_t, int>>, int>>, int>::Grow(int)` | identical | no | no | yes |  |
| flexrenderdata | `CUtlMemory<CCachedRenderData::CacheDict_t, int>::Grow(int)` | identical | no | no | yes |  |
| flexrenderdata | `CCachedRenderData::StartModel()` | identical | no | no | yes |  |
| flexrenderdata | `CCachedRenderData::SetBodyPart(int)` | identical | no | no | yes |  |
| flexrenderdata | `CCachedRenderData::CreateFlexVertex(int)` | identical | no | no | yes |  |
| flexrenderdata | `CCachedRenderData::SetupComputation(mstudiomesh_t*, bool)` | identical | no | no | yes |  |
| flexrenderdata | `CCachedRenderData::CreateWorldVertex(int)` | identical | no | no | yes |  |
| flexrenderdata | `CCachedRenderData::CreateThinFlexVertex(int)` | identical | no | no | yes |  |
| flexrenderdata | `CCachedRenderData::RenormalizeFlexVertices(bool)` | identical | no | no | yes |  |
| flexrenderdata | `CCachedRenderData::SetMesh(int)` | identical | no | no | yes |  |
| flexrenderdata | `CCachedRenderData::SetModel(int)` | identical | no | no | yes |  |
| flexrenderdata | `CCachedRenderData::CCachedRenderData()` | identical | no | no | yes |  |
| flexrenderdata | `CCachedRenderData::IsFlexComputationDone() const` | identical | no | no | yes |  |
| r_studio | `ComputePoseToWorld(matrix3x4_t*, studiohdr_t*, int, Vector const&, matrix3x4_t const*)` | identical | yes | yes | yes |  |
| r_studio | `R_StudioSetupModel(int, int, mstudiomodel_t**, studiohdr_t const*)` | edited | no | yes | yes | removed the fork's 'missing body part' warning |
| r_studio | `CStudioRender::R_StudioSetupSkinAndLighting(IMatRenderContext*, int, IMaterial**, int, void*, ColorMeshInfo_t*, StudioModelLighting_t&)` | differs | no | no | no | removed the fork's $lightmap binding |
| r_studio | `___clang_call_terminate` | identical | no | no | yes |  |
| r_studiodecal | `IntersectPlane(DecalClipState_t&, int, int, int, float)` | identical | yes | yes | yes |  |
| r_studiodecal | `ClipTriangleAgainstPlane(DecalClipState_t&, int, int, float)` | identical | yes | yes | yes |  |
| r_studiodecal | `CUtlMemory<UtlLinkedListElem_t<DecalVertex_t, unsigned short>, unsigned short>::Grow(int)` | identical | no | no | yes |  |
| r_studiodecal | `CUtlMemory<UtlLinkedListElem_t<CStudioRender::DecalHistory_t, unsigned short>, unsigned short>::Grow(int)` | identical | no | no | yes |  |
| r_studiodecal | `CUtlMemory<UtlLinkedListElem_t<CStudioRender::DecalMaterial_t, unsigned short>, unsigned short>::Grow(int)` | identical | no | no | yes |  |
| r_studiodecal | `CUtlMemory<UtlLinkedListElem_t<CStudioRender::Decal_t, unsigned short>, unsigned short>::Grow(int)` | identical | no | no | yes |  |
| r_studiodecal | `CUtlMemory<unsigned short, int>::Grow(int)` | identical | no | no | yes |  |
| r_studiodecal | `CStudioRender::RetireDecal(CStudioRender::DecalModelList_t&, unsigned short, int, int)` | identical | no | no | yes |  |
| r_studiodecal | `CStudioRender::PreDrawDecal(IMatRenderContext*, DrawModelInfo_t const&)` | identical | no | no | yes |  |
| r_studiodecal | `CStudioRender::AddDecalToMesh(CStudioRender::DecalBuildInfo_t&)` | edited | yes | yes | yes | $modelmaterial lookup moved back here |
| r_studiodecal | `CStudioRender::AddDecalToModel(CStudioRender::DecalBuildInfo_t&)` | edited | no | yes | yes | $modelmaterial lookup moved back here |
| r_studiodecal | `CStudioRender::CreateDecalList(studiohwdata_t*)` | identical | no | no | yes |  |
| r_studiodecal | `CStudioRender::DestroyDecalList(StudioDecalHandle_t__*)` | identical | no | no | yes |  |
| r_studiodecal | `CStudioRender::GetDecalMaterial(CStudioRender::DecalLod_t&, IMaterial*)` | identical | yes | yes | yes |  |
| r_studiodecal | `CStudioRender::DrawDecalMaterial(IMatRenderContext*, CStudioRender::DecalMaterial_t&, studiohdr_t*, studioloddata_t*)` | identical | no | no | yes |  |
| r_studiodecal | `CStudioRender::AddTriangleToDecal(CStudioRender::DecalBuildInfo_t&, int, int, int, int, int, int)` | identical | yes | yes | yes |  |
| r_studiodecal | `CStudioRender::ComputePoseToDecal(Ray_t const&, Vector const&)` | identical | yes | yes | yes |  |
| r_studiodecal | `CStudioRender::DrawMultiBoneDecals(CMeshBuilder&, CStudioRender::DecalMaterial_t&, studiohdr_t*)` | differs | no | no | no | retail smaller; not investigated (CMeshBuilder inlines) |
| r_studiodecal | `CStudioRender::DrawSingleBoneDecals(CMeshBuilder&, CStudioRender::DecalMaterial_t&)` | differs | no | no | no | same size; not investigated |
| r_studiodecal | `CStudioRender::DrawStaticPropDecals(DrawModelInfo_t const&, StudioRenderContext_t const&, matrix3x4_t const&)` | identical | no | no | yes |  |
| r_studiodecal | `CStudioRender::ProjectDecalOntoMesh(CStudioRender::DecalBuildInfo_t&, CStudioRender::DecalBuildVertexInfo_t*, mstudiomesh_t*)` | identical | no | yes | yes |  |
| r_studiodecal | `CStudioRender::TransformToDecalSpace(CStudioRender::DecalBuildInfo_t&, Vector const&, mstudioboneweight_t*, Vector2D&)` | identical | yes | yes | yes |  |
| r_studiodecal | `CStudioRender::ComputeVertexAllocation(int, int, studiohwdata_t*, MeshVertexInfo_t*)` | identical | yes | yes | yes |  |
| r_studiodecal | `CStudioRender::AddClippedDecalToTriangle(CStudioRender::DecalBuildInfo_t&, DecalClipState_t&)` | identical | yes | yes | yes |  |
| r_studiodecal | `CStudioRender::DrawMultiBoneFlexedDecals(IMatRenderContext*, CMeshBuilder&, CStudioRender::DecalMaterial_t&, studiohdr_t*, studioloddata_t*)` | differs | no | no | no | retail smaller; not investigated |
| r_studiodecal | `CStudioRender::DrawSingleBoneFlexedDecals(IMatRenderContext*, CMeshBuilder&, CStudioRender::DecalMaterial_t&)` | differs | no | no | no | retail smaller; not investigated |
| r_studiodecal | `CStudioRender::ConvertMeshVertexToDecalVertex(CStudioRender::DecalBuildInfo_t&, int, DecalVertex_t&, int)` | identical | yes | yes | yes |  |
| r_studiodecal | `CStudioRender::AddDecal(StudioDecalHandle_t__*, StudioRenderContext_t const&, matrix3x4_t*, studiohdr_t*, Ray_t const&, Vector const&, IMaterial*, float, int, bool, int)` | differs | no | no | no | $modelmaterial lookup moved back here |
| r_studiodecal | `CStudioRender::ClipDecal(CStudioRender::DecalBuildInfo_t&, int, int, int, int*)` | identical | yes | yes | yes |  |
| r_studiodecal | `CStudioRender::DrawDecal(DrawModelInfo_t const&, int, int)` | identical | no | no | yes |  |
| r_studiodecal | `CUtlLinkedList<DecalVertex_t, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<DecalVertex_t, unsigned short>, unsigned short>>::LinkBefore(unsigned short, unsigned short)` | identical | no | no | yes |  |
| r_studiodecal | `CUtlLinkedList<DecalVertex_t, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<DecalVertex_t, unsigned short>, unsigned short>>::InsertBefore(unsigned short, DecalVertex_t const&)` | identical | yes | yes | yes |  |
| r_studiodecal | `CUtlLinkedList<DecalVertex_t, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<DecalVertex_t, unsigned short>, unsigned short>>::AllocInternal(bool)` | identical | no | no | yes |  |
| r_studiodecal | `CUtlLinkedList<DecalVertex_t, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<DecalVertex_t, unsigned short>, unsigned short>>::Purge()` | identical | yes | yes | yes |  |
| r_studiodecal | `CUtlLinkedList<CStudioRender::DecalLRU_t, int, true, int, CUtlFixedMemory<UtlLinkedListElem_t<CStudioRender::DecalLRU_t, int>>>::AllocInternal(bool)` | identical | no | no | yes |  |
| r_studiodecal | `CUtlLinkedList<CStudioRender::DecalLRU_t, int, true, int, CUtlFixedMemory<UtlLinkedListElem_t<CStudioRender::DecalLRU_t, int>>>::Unlink(int)` | identical | yes | yes | yes |  |
| r_studiodecal | `CUtlLinkedList<CStudioRender::DecalHistory_t, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<CStudioRender::DecalHistory_t, unsigned short>, unsigned short>>::LinkBefore(unsigned short, unsigned short)` | identical | no | no | yes |  |
| r_studiodecal | `CUtlLinkedList<CStudioRender::DecalHistory_t, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<CStudioRender::DecalHistory_t, unsigned short>, unsigned short>>::AllocInternal(bool)` | identical | no | no | yes |  |
| r_studiodecal | `CUtlLinkedList<CStudioRender::DecalMaterial_t, unsigned short, true, unsigned short, CUtlMemory<UtlLinkedListElem_t<CStudioRender::DecalMaterial_t, unsigned short>, unsigned short>>::LinkBefore(unsigned short, unsigned short)` | identical | no | no | yes |  |
| r_studiodecal | `CUtlLinkedList<CStudioRender::DecalMaterial_t, unsigned short, true, unsigned short, CUtlMemory<UtlLinkedListElem_t<CStudioRender::DecalMaterial_t, unsigned short>, unsigned short>>::AllocInternal(bool)` | identical | no | no | yes |  |
| r_studiodecal | `CUtlLinkedList<CStudioRender::DecalMaterial_t, unsigned short, true, unsigned short, CUtlMemory<UtlLinkedListElem_t<CStudioRender::DecalMaterial_t, unsigned short>, unsigned short>>::Free(unsigned short)` | identical | no | no | yes |  |
| r_studiodecal | `CUtlLinkedList<CStudioRender::DecalModelList_t, int, true, int, CUtlFixedMemory<UtlLinkedListElem_t<CStudioRender::DecalModelList_t, int>>>::AllocInternal(bool)` | identical | no | no | yes |  |
| r_studiodecal | `CUtlLinkedList<CStudioRender::DecalModelList_t, int, true, int, CUtlFixedMemory<UtlLinkedListElem_t<CStudioRender::DecalModelList_t, int>>>::Unlink(int)` | identical | yes | yes | yes |  |
| r_studiodecal | `CUtlLinkedList<CStudioRender::Decal_t, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<CStudioRender::Decal_t, unsigned short>, unsigned short>>::LinkBefore(unsigned short, unsigned short)` | identical | no | no | yes |  |
| r_studiodecal | `CUtlLinkedList<CStudioRender::Decal_t, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<CStudioRender::Decal_t, unsigned short>, unsigned short>>::AllocInternal(bool)` | identical | no | no | yes |  |
| r_studiodecal | `CUtlLinkedList<CStudioRender::Decal_t, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<CStudioRender::Decal_t, unsigned short>, unsigned short>>::Purge()` | identical | yes | yes | yes |  |
| r_studiodecal | `thinModelVertices_t::GetBoneWeights(int, mstudioboneweight_t*) const` | differs | no | no | no | not investigated |
| r_studiodraw | `__GLOBAL__I_a` | identical | no | no | yes |  |
| r_studiodraw | `ComputeSkinMatrix(mstudioboneweight_t&, matrix3x4_t*, matrix3x4_t&)` | identical | yes | yes | yes |  |
| r_studiodraw | `CStudioRender::DrawShadows(DrawModelInfo_t const&, int, int)` | identical | no | no | yes |  |
| r_studiodraw | `CStudioRender::EnableScissor(FlashlightState_t*)` | identical | no | no | yes |  |
| r_studiodraw | `CStudioRender::DisableScissor()` | identical | no | no | yes |  |
| r_studiodraw | `CStudioRender::R_StudioDrawMesh(IMatRenderContext*, mstudiomesh_t*, studiomeshdata_t*, StudioModelLighting_t, IMaterial*, ColorMeshInfo_t*, int)` | identical | no | no | yes |  |
| r_studiodraw | `CStudioRender::R_StudioDrawBones()` | identical | no | no | yes |  |
| r_studiodraw | `CStudioRender::R_StudioDrawHulls(int, bool)` | identical | no | no | yes |  |
| r_studiodraw | `CStudioRender::ComputeFlexWeights(int, mstudioflex_t*, MorphWeight_t*)` | identical | yes | yes | yes |  |
| r_studiodraw | `CStudioRender::R_StudioDrawPoints(IMatRenderContext*, int, void*, IMaterial**, int*, int, int, ColorMeshInfo_t*)` | identical | no | no | yes |  |
| r_studiodraw | `CStudioRender::SetEyeMaterialVars(IMaterial*, mstudioeyeball_t*, Vector const&, matrix3x4_t const&, matrix3x4_t const&)` | identical | yes | yes | yes |  |
| r_studiodraw | `CStudioRender::R_StudioDrawEyeball(IMatRenderContext*, mstudiomesh_t*, studiomeshdata_t*, StudioModelLighting_t, IMaterial*, int)` | differs | no | no | no | not investigated |
| r_studiodraw | `CStudioRender::R_StudioRenderFinal(IMatRenderContext*, int, int, CStudioRender::BodyPartInfo_t*, void*, IMaterial**, int*, int, int, ColorMeshInfo_t*)` | identical | no | no | yes |  |
| r_studiodraw | `CStudioRender::R_StudioRenderModel(IMatRenderContext*, int, int, int, void*, IMaterial**, int*, int, int, int, ColorMeshInfo_t*)` | identical | no | no | yes |  |
| r_studiodraw | `void CStudioRender::R_StudioRestoreMesh<(VertexCompressionType_t)0>(mstudiomesh_t*, studiomeshgroup_t*)` | identical | no | yes | yes |  |
| r_studiodraw | `void CStudioRender::R_StudioRestoreMesh<(VertexCompressionType_t)1>(mstudiomesh_t*, studiomeshgroup_t*)` | identical | no | yes | yes |  |
| r_studiodraw | `CStudioRender::DrawFlashlightDecals(DrawModelInfo_t const&, int)` | identical | no | no | yes |  |
| r_studiodraw | `CStudioRender::DrawStaticPropShadows(DrawModelInfo_t const&, StudioRenderContext_t const&, matrix3x4_t const&, int)` | identical | yes | yes | yes |  |
| r_studiodraw | `CStudioRender::R_StudioFlexMeshGroup(studiomeshgroup_t*)` | identical | no | no | yes |  |
| r_studiodraw | `CStudioRender::R_StudioDrawStaticMesh(IMatRenderContext*, mstudiomesh_t*, studiomeshgroup_t*, StudioModelLighting_t, float, IMaterial*, int, ColorMeshInfo_t*)` | identical | no | no | yes |  |
| r_studiodraw | `CStudioRender::R_StudioDrawDynamicMesh(IMatRenderContext*, mstudiomesh_t*, studiomeshgroup_t*, StudioModelLighting_t, float, IMaterial*, int)` | identical | no | no | yes |  |
| r_studiodraw | `CStudioRender::R_StudioDrawGroupHWSkin(IMatRenderContext*, studiomeshgroup_t*, IMesh*, ColorMeshInfo_t*)` | identical | yes | yes | yes |  |
| r_studiodraw | `CStudioRender::R_StudioDrawGroupSWSkin(studiomeshgroup_t*, IMesh*)` | identical | no | no | yes |  |
| r_studiodraw | `CStudioRender::GenerateMorphAccumulator(mstudiomodel_t*)` | identical | no | no | yes |  |
| r_studiodraw | `CStudioRender::R_StudioProcessFlexedMesh(mstudiomesh_t*, CMeshBuilder&, int, unsigned short*)` | differs | no | no | no | not investigated |
| r_studiodraw | `CStudioRender::R_StudioSoftwareProcessMesh(mstudiomesh_t*, CMeshBuilder&, int, unsigned short*, StudioModelLighting_t, bool, float, bool, bool, IMaterial*)` | differs | no | no | no | not investigated |
| r_studiodraw | `CStudioRender::R_StudioSoftwareProcessMesh_Normals(mstudiomesh_t*, CMeshBuilder&, int, unsigned short*, StudioModelLighting_t, bool, float, bool, bool)` | differs | no | no | no | not investigated |
| r_studiodraw | `CStudioRender::R_StudioProcessFlexedMesh_StreamOffset(mstudiomesh_t*, int)` | identical | no | no | yes |  |
| r_studiodraw | `void CCachedRenderData::ComputeFlexedVertex_StreamOffset<mstudiovertanim_t>(studiohdr_t*, mstudioflex_t*, mstudiovertanim_t*, int, float, float, float, float)` | identical | no | no | yes |  |
| r_studiodraw | `void CCachedRenderData::ComputeFlexedVertex_StreamOffset<mstudiovertanim_wrinkle_t>(studiohdr_t*, mstudioflex_t*, mstudiovertanim_wrinkle_t*, int, float, float, float, float)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<0, 0, 0, 0, 0>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<0, 0, 0, 0, 1>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<0, 0, 0, 1, 0>::R_PerformLighting(Vector const&, float, Vector const&, Vector const&, unsigned int, unsigned int*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<0, 0, 0, 1, 0>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<0, 0, 0, 1, 1>::R_PerformLighting(Vector const&, float, Vector const&, Vector const&, unsigned int, unsigned int*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<0, 0, 0, 1, 1>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<0, 0, 0, 2, 0>::R_PerformLighting(Vector const&, float, Vector const&, Vector const&, unsigned int, unsigned int*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<0, 0, 0, 2, 0>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<0, 0, 0, 2, 1>::R_PerformLighting(Vector const&, float, Vector const&, Vector const&, unsigned int, unsigned int*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<0, 0, 0, 2, 1>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<0, 0, 1, 0, 0>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | edited | no | no | yes | SIMD instance: FastVertexSSE on 32-bit |
| r_studiodraw | `CProcessMeshWrapper<0, 0, 1, 0, 1>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | edited | no | no | yes | SIMD instance: FastVertexSSE on 32-bit |
| r_studiodraw | `CProcessMeshWrapper<0, 0, 1, 1, 0>::R_PerformLighting(Vector const&, float, Vector const&, Vector const&, unsigned int, unsigned int*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<0, 0, 1, 1, 0>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | edited | no | no | yes | SIMD instance: FastVertexSSE on 32-bit |
| r_studiodraw | `CProcessMeshWrapper<0, 0, 1, 1, 1>::R_PerformLighting(Vector const&, float, Vector const&, Vector const&, unsigned int, unsigned int*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<0, 0, 1, 1, 1>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | edited | no | no | yes | SIMD instance: FastVertexSSE on 32-bit |
| r_studiodraw | `CProcessMeshWrapper<0, 0, 1, 2, 0>::R_PerformLighting(Vector const&, float, Vector const&, Vector const&, unsigned int, unsigned int*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<0, 0, 1, 2, 0>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | edited | no | no | yes | SIMD instance: FastVertexSSE on 32-bit |
| r_studiodraw | `CProcessMeshWrapper<0, 0, 1, 2, 1>::R_PerformLighting(Vector const&, float, Vector const&, Vector const&, unsigned int, unsigned int*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<0, 0, 1, 2, 1>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | edited | no | no | yes | SIMD instance: FastVertexSSE on 32-bit |
| r_studiodraw | `CProcessMeshWrapper<0, 1, 0, 0, 0>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<0, 1, 0, 0, 1>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<0, 1, 0, 1, 0>::R_PerformLighting(Vector const&, float, Vector const&, Vector const&, unsigned int, unsigned int*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<0, 1, 0, 1, 0>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<0, 1, 0, 1, 1>::R_PerformLighting(Vector const&, float, Vector const&, Vector const&, unsigned int, unsigned int*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<0, 1, 0, 1, 1>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<0, 1, 0, 2, 0>::R_PerformLighting(Vector const&, float, Vector const&, Vector const&, unsigned int, unsigned int*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<0, 1, 0, 2, 0>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<0, 1, 0, 2, 1>::R_PerformLighting(Vector const&, float, Vector const&, Vector const&, unsigned int, unsigned int*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<0, 1, 0, 2, 1>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<0, 1, 1, 0, 0>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | edited | no | no | yes | SIMD instance: FastVertexSSE on 32-bit |
| r_studiodraw | `CProcessMeshWrapper<0, 1, 1, 0, 1>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | edited | no | no | yes | SIMD instance: FastVertexSSE on 32-bit |
| r_studiodraw | `CProcessMeshWrapper<0, 1, 1, 1, 0>::R_PerformLighting(Vector const&, float, Vector const&, Vector const&, unsigned int, unsigned int*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<0, 1, 1, 1, 0>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | edited | no | no | yes | SIMD instance: FastVertexSSE on 32-bit |
| r_studiodraw | `CProcessMeshWrapper<0, 1, 1, 1, 1>::R_PerformLighting(Vector const&, float, Vector const&, Vector const&, unsigned int, unsigned int*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<0, 1, 1, 1, 1>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | edited | no | no | yes | SIMD instance: FastVertexSSE on 32-bit |
| r_studiodraw | `CProcessMeshWrapper<0, 1, 1, 2, 0>::R_PerformLighting(Vector const&, float, Vector const&, Vector const&, unsigned int, unsigned int*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<0, 1, 1, 2, 0>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | edited | no | no | yes | SIMD instance: FastVertexSSE on 32-bit |
| r_studiodraw | `CProcessMeshWrapper<0, 1, 1, 2, 1>::R_PerformLighting(Vector const&, float, Vector const&, Vector const&, unsigned int, unsigned int*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<0, 1, 1, 2, 1>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | edited | no | no | yes | SIMD instance: FastVertexSSE on 32-bit |
| r_studiodraw | `CProcessMeshWrapper<1, 0, 0, 0, 0>::R_TransformVert(Vector const*, Vector const*, Vector4D const*, matrix3x4_t*, VectorAligned&, Vector&, Vector4DAligned&)` | identical | yes | yes | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 0, 0, 0, 0>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 0, 0, 0, 1>::R_TransformVert(Vector const*, Vector const*, Vector4D const*, matrix3x4_t*, VectorAligned&, Vector&, Vector4DAligned&)` | identical | yes | yes | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 0, 0, 0, 1>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 0, 0, 1, 0>::R_TransformVert(Vector const*, Vector const*, Vector4D const*, matrix3x4_t*, VectorAligned&, Vector&, Vector4DAligned&)` | identical | yes | yes | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 0, 0, 1, 0>::R_PerformLighting(Vector const&, float, Vector const&, Vector const&, unsigned int, unsigned int*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 0, 0, 1, 0>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 0, 0, 1, 1>::R_TransformVert(Vector const*, Vector const*, Vector4D const*, matrix3x4_t*, VectorAligned&, Vector&, Vector4DAligned&)` | identical | yes | yes | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 0, 0, 1, 1>::R_PerformLighting(Vector const&, float, Vector const&, Vector const&, unsigned int, unsigned int*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 0, 0, 1, 1>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 0, 0, 2, 0>::R_TransformVert(Vector const*, Vector const*, Vector4D const*, matrix3x4_t*, VectorAligned&, Vector&, Vector4DAligned&)` | identical | yes | yes | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 0, 0, 2, 0>::R_PerformLighting(Vector const&, float, Vector const&, Vector const&, unsigned int, unsigned int*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 0, 0, 2, 0>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 0, 0, 2, 1>::R_TransformVert(Vector const*, Vector const*, Vector4D const*, matrix3x4_t*, VectorAligned&, Vector&, Vector4DAligned&)` | identical | yes | yes | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 0, 0, 2, 1>::R_PerformLighting(Vector const&, float, Vector const&, Vector const&, unsigned int, unsigned int*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 0, 0, 2, 1>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 0, 1, 0, 0>::R_TransformVert(Vector const*, Vector const*, Vector4D const*, matrix3x4_t*, VectorAligned&, Vector&, Vector4DAligned&)` | identical | yes | yes | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 0, 1, 0, 0>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | edited | no | no | yes | SIMD instance: FastVertexSSE on 32-bit |
| r_studiodraw | `CProcessMeshWrapper<1, 0, 1, 0, 1>::R_TransformVert(Vector const*, Vector const*, Vector4D const*, matrix3x4_t*, VectorAligned&, Vector&, Vector4DAligned&)` | identical | yes | yes | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 0, 1, 0, 1>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | edited | no | no | yes | SIMD instance: FastVertexSSE on 32-bit |
| r_studiodraw | `CProcessMeshWrapper<1, 0, 1, 1, 0>::R_TransformVert(Vector const*, Vector const*, Vector4D const*, matrix3x4_t*, VectorAligned&, Vector&, Vector4DAligned&)` | identical | yes | yes | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 0, 1, 1, 0>::R_PerformLighting(Vector const&, float, Vector const&, Vector const&, unsigned int, unsigned int*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 0, 1, 1, 0>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | edited | no | no | yes | SIMD instance: FastVertexSSE on 32-bit |
| r_studiodraw | `CProcessMeshWrapper<1, 0, 1, 1, 1>::R_TransformVert(Vector const*, Vector const*, Vector4D const*, matrix3x4_t*, VectorAligned&, Vector&, Vector4DAligned&)` | identical | yes | yes | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 0, 1, 1, 1>::R_PerformLighting(Vector const&, float, Vector const&, Vector const&, unsigned int, unsigned int*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 0, 1, 1, 1>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | edited | no | no | yes | SIMD instance: FastVertexSSE on 32-bit |
| r_studiodraw | `CProcessMeshWrapper<1, 0, 1, 2, 0>::R_TransformVert(Vector const*, Vector const*, Vector4D const*, matrix3x4_t*, VectorAligned&, Vector&, Vector4DAligned&)` | identical | yes | yes | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 0, 1, 2, 0>::R_PerformLighting(Vector const&, float, Vector const&, Vector const&, unsigned int, unsigned int*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 0, 1, 2, 0>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | edited | no | no | yes | SIMD instance: FastVertexSSE on 32-bit |
| r_studiodraw | `CProcessMeshWrapper<1, 0, 1, 2, 1>::R_TransformVert(Vector const*, Vector const*, Vector4D const*, matrix3x4_t*, VectorAligned&, Vector&, Vector4DAligned&)` | identical | yes | yes | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 0, 1, 2, 1>::R_PerformLighting(Vector const&, float, Vector const&, Vector const&, unsigned int, unsigned int*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 0, 1, 2, 1>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | edited | no | no | yes | SIMD instance: FastVertexSSE on 32-bit |
| r_studiodraw | `CProcessMeshWrapper<1, 1, 0, 0, 0>::R_TransformVert(Vector const*, Vector const*, Vector4D const*, matrix3x4_t*, VectorAligned&, Vector&, Vector4DAligned&)` | identical | yes | yes | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 1, 0, 0, 0>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 1, 0, 0, 1>::R_TransformVert(Vector const*, Vector const*, Vector4D const*, matrix3x4_t*, VectorAligned&, Vector&, Vector4DAligned&)` | identical | yes | yes | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 1, 0, 0, 1>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 1, 0, 1, 0>::R_TransformVert(Vector const*, Vector const*, Vector4D const*, matrix3x4_t*, VectorAligned&, Vector&, Vector4DAligned&)` | identical | yes | yes | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 1, 0, 1, 0>::R_PerformLighting(Vector const&, float, Vector const&, Vector const&, unsigned int, unsigned int*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 1, 0, 1, 0>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 1, 0, 1, 1>::R_TransformVert(Vector const*, Vector const*, Vector4D const*, matrix3x4_t*, VectorAligned&, Vector&, Vector4DAligned&)` | identical | yes | yes | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 1, 0, 1, 1>::R_PerformLighting(Vector const&, float, Vector const&, Vector const&, unsigned int, unsigned int*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 1, 0, 1, 1>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 1, 0, 2, 0>::R_TransformVert(Vector const*, Vector const*, Vector4D const*, matrix3x4_t*, VectorAligned&, Vector&, Vector4DAligned&)` | identical | yes | yes | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 1, 0, 2, 0>::R_PerformLighting(Vector const&, float, Vector const&, Vector const&, unsigned int, unsigned int*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 1, 0, 2, 0>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 1, 0, 2, 1>::R_TransformVert(Vector const*, Vector const*, Vector4D const*, matrix3x4_t*, VectorAligned&, Vector&, Vector4DAligned&)` | identical | yes | yes | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 1, 0, 2, 1>::R_PerformLighting(Vector const&, float, Vector const&, Vector const&, unsigned int, unsigned int*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 1, 0, 2, 1>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 1, 1, 0, 0>::R_TransformVert(Vector const*, Vector const*, Vector4D const*, matrix3x4_t*, VectorAligned&, Vector&, Vector4DAligned&)` | identical | yes | yes | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 1, 1, 0, 0>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | edited | no | no | yes | SIMD instance: FastVertexSSE on 32-bit |
| r_studiodraw | `CProcessMeshWrapper<1, 1, 1, 0, 1>::R_TransformVert(Vector const*, Vector const*, Vector4D const*, matrix3x4_t*, VectorAligned&, Vector&, Vector4DAligned&)` | identical | yes | yes | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 1, 1, 0, 1>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | edited | no | no | yes | SIMD instance: FastVertexSSE on 32-bit |
| r_studiodraw | `CProcessMeshWrapper<1, 1, 1, 1, 0>::R_TransformVert(Vector const*, Vector const*, Vector4D const*, matrix3x4_t*, VectorAligned&, Vector&, Vector4DAligned&)` | identical | yes | yes | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 1, 1, 1, 0>::R_PerformLighting(Vector const&, float, Vector const&, Vector const&, unsigned int, unsigned int*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 1, 1, 1, 0>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | edited | no | no | yes | SIMD instance: FastVertexSSE on 32-bit |
| r_studiodraw | `CProcessMeshWrapper<1, 1, 1, 1, 1>::R_TransformVert(Vector const*, Vector const*, Vector4D const*, matrix3x4_t*, VectorAligned&, Vector&, Vector4DAligned&)` | identical | yes | yes | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 1, 1, 1, 1>::R_PerformLighting(Vector const&, float, Vector const&, Vector const&, unsigned int, unsigned int*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 1, 1, 1, 1>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | edited | no | no | yes | SIMD instance: FastVertexSSE on 32-bit |
| r_studiodraw | `CProcessMeshWrapper<1, 1, 1, 2, 0>::R_TransformVert(Vector const*, Vector const*, Vector4D const*, matrix3x4_t*, VectorAligned&, Vector&, Vector4DAligned&)` | identical | yes | yes | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 1, 1, 2, 0>::R_PerformLighting(Vector const&, float, Vector const&, Vector const&, unsigned int, unsigned int*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 1, 1, 2, 0>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | edited | no | no | yes | SIMD instance: FastVertexSSE on 32-bit |
| r_studiodraw | `CProcessMeshWrapper<1, 1, 1, 2, 1>::R_TransformVert(Vector const*, Vector const*, Vector4D const*, matrix3x4_t*, VectorAligned&, Vector&, Vector4DAligned&)` | identical | yes | yes | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 1, 1, 2, 1>::R_PerformLighting(Vector const&, float, Vector const&, Vector const&, unsigned int, unsigned int*)` | identical | no | no | yes |  |
| r_studiodraw | `CProcessMeshWrapper<1, 1, 1, 2, 1>::R_StudioSoftwareProcessMesh(mstudio_meshvertexdata_t const*, matrix3x4_t*, CCachedRenderData&, CMeshBuilder&, int, unsigned short*, unsigned int, IMaterial*)` | edited | no | no | yes | SIMD instance: FastVertexSSE on 32-bit |
| r_studioflex | `__GLOBAL__I_a` | identical | no | no | yes |  |
| r_studioflex | `CStudioRender::UncacheGlint()` | identical | yes | yes | yes |  |
| r_studioflex | `CStudioRender::PrecacheGlint()` | identical | no | no | yes |  |
| r_studioflex | `CStudioRender::RampFlexWeight(mstudioflex_t&, float)` | identical | yes | yes | yes |  |
| r_studioflex | `CStudioRender::R_MouthLighting(float, Vector const&, Vector const&, Vector&)` | identical | yes | yes | yes |  |
| r_studioflex | `CStudioRender::R_StudioFlexVerts(mstudiomesh_t*, int)` | identical | no | no | yes |  |
| r_studioflex | `CStudioRender::R_StudioEyelidFACS(mstudioeyeball_t const*, eyeballstate_t const*)` | differs | no | no | no | not investigated |
| r_studioflex | `CStudioRender::RenderGlintTexture(eyeballstate_t const*, Vector const&, Vector const&, Vector const&)` | identical | no | no | yes |  |
| r_studioflex | `CStudioRender::BuildGlintRenderData(CStudioRender::GlintRenderData_t*, int, eyeballstate_t const*, Vector const&, Vector const&, Vector const&)` | identical | yes | yes | yes |  |
| r_studioflex | `CStudioRender::R_StudioEyeballGlint(eyeballstate_t const*, IMaterialVar*, Vector const&, Vector const&, Vector const&)` | identical | no | no | yes |  |
| r_studioflex | `CStudioRender::R_StudioEyeballPosition(mstudioeyeball_t const*, eyeballstate_t*)` | identical | yes | yes | yes |  |
| r_studioflex | `CStudioRender::R_MouthSetupVertexShader(IMaterial*)` | identical | yes | yes | yes |  |
| r_studioflex | `CStudioRender::R_MouthComputeLightingValues(float&, Vector&)` | identical | yes | yes | yes |  |
| r_studioflex | `CStudioRender::ComputeGlintTextureProjection(eyeballstate_t const*, Vector const&, Vector const&, matrix3x4_t&)` | identical | yes | yes | yes |  |
| r_studioflex | `CStudioRender::AddGlint(CPixelWriter&, float, float, Vector const&)` | identical | no | no | yes |  |
| r_studioflex | `CGlintTextureRegenerator::RegenerateTextureBits(ITexture*, IVTFTexture*, Rect_t*)` | identical | no | no | yes |  |
| r_studioflex | `CGlintTextureRegenerator::Release()` | identical | yes | yes | yes |  |
| r_studiogettriangles | `CUtlMemory<GetTriangles_Vertex_t, int>::Grow(int)` | identical | no | no | yes |  |
| r_studiogettriangles | `CUtlMemory<GetTriangles_MaterialBatch_t, int>::Grow(int)` | identical | no | no | yes |  |
| r_studiogettriangles | `CUtlMemory<int, int>::Grow(int)` | identical | no | no | yes |  |
| r_studiogettriangles | `CStudioRenderContext::GetTriangles(DrawModelInfo_t const&, matrix3x4_t*, GetTriangles_Output_t&)` | differs | no | no | no | not investigated |
| r_studiolight | `R_LightAmbient_4D(Vector const&, Vector4D*, Vector&)` | identical | yes | yes | yes |  |
| r_studiolight | `R_LightStrengthWorld(Vector const&, int, LightDesc_t*, lightpos_t*)` | identical | yes | yes | yes |  |
| r_studiolight | `CopyLocalLightingState(int, LightDesc_t*, int, LightDesc_t const*)` | edited | no | yes | yes | memcpy per light (retail rep movs) |
| r_studiolight | `CStudioRender::R_LightGlintPosition(int, Vector const&, Vector&, Vector&)` | identical | no | no | yes |  |
| r_studiolight | `CStudioRender::R_InitLightEffectsWorld3()` | identical | no | no | yes |  |
| r_studiolight | `CStudioRenderContext::ComputeLighting(Vector const*, int, LightDesc_t*, Vector const&, Vector const&, Vector&)` | identical | no | no | yes |  |
| r_studiolight | `CStudioRenderContext::ComputeLightingConstDirectional(Vector const*, int, LightDesc_t*, Vector const&, Vector const&, Vector&, float)` | identical | no | no | yes |  |
| r_studiolight | `__Function_R_WorldLightDistanceFalloffFunctionTable<0>::Run(LightDesc_t const*, Vector const&)` | identical | yes | yes | yes |  |
| r_studiolight | `__Function_R_WorldLightDistanceFalloffFunctionTable<1>::Run(LightDesc_t const*, Vector const&)` | identical | yes | yes | yes |  |
| r_studiolight | `__Function_R_WorldLightDistanceFalloffFunctionTable<2>::Run(LightDesc_t const*, Vector const&)` | identical | yes | yes | yes |  |
| r_studiolight | `__Function_R_WorldLightDistanceFalloffFunctionTable<3>::Run(LightDesc_t const*, Vector const&)` | identical | yes | yes | yes |  |
| r_studiolight | `__Function_R_WorldLightDistanceFalloffFunctionTable<4>::Run(LightDesc_t const*, Vector const&)` | identical | yes | yes | yes |  |
| r_studiolight | `__Function_R_WorldLightDistanceFalloffFunctionTable<5>::Run(LightDesc_t const*, Vector const&)` | identical | yes | yes | yes |  |
| r_studiolight | `__Function_R_WorldLightDistanceFalloffFunctionTable<6>::Run(LightDesc_t const*, Vector const&)` | identical | yes | yes | yes |  |
| r_studiolight | `__Function_R_WorldLightDistanceFalloffFunctionTable<7>::Run(LightDesc_t const*, Vector const&)` | identical | yes | yes | yes |  |
| r_studiolight | `__Function_R_WorldLightDistanceFalloffFunctionTable<8>::Run(LightDesc_t const*, Vector const&)` | identical | yes | yes | yes |  |
| r_studiostats | `__GLOBAL__I_a` | identical | no | no | yes |  |
| r_studiostats | `CStudioRender::ModelStats(DrawModelInfo_t const&, StudioRenderContext_t const&, matrix3x4_t*, FlexWeights_t const&, int)` | identical | no | no | yes |  |
| r_studiostats | `CStudioRender::R_GatherStats(studiomeshgroup_t*, CMeshBuilder&, IMesh*, IMaterial*)` | differs | no | no | no | not investigated |
| studiorender | `__GLOBAL__I_a` | identical | no | no | yes |  |
| studiorender | `ReleaseMaterialSystemObjects()` | identical | yes | yes | yes |  |
| studiorender | `RestoreMaterialSystemObjects(int)` | identical | yes | yes | yes |  |
| studiorender | `CUtlMemory<CStudioRender::ShadowState_t, int>::Grow(int)` | identical | no | no | yes |  |
| studiorender | `CUtlVector<CUtlVector<CCachedRenderData::CacheDict_t, CUtlMemory<CCachedRenderData::CacheDict_t, int>>, CUtlMemory<CUtlVector<CCachedRenderData::CacheDict_t, CUtlMemory<CCachedRenderData::CacheDict_t, int>>, int>>::Purge()` | identical | yes | yes | yes |  |
| studiorender | `CUtlVector<CUtlVector<CUtlVector<CCachedRenderData::CacheDict_t, CUtlMemory<CCachedRenderData::CacheDict_t, int>>, CUtlMemory<CUtlVector<CCachedRenderData::CacheDict_t, CUtlMemory<CCachedRenderData::CacheDict_t, int>>, int>>, CUtlMemory<CUtlVector<CUtlVector<CCachedRenderData::CacheDict_t, CUtlMemory<CCachedRenderData::CacheDict_t, int>>, CUtlMemory<CUtlVector<CCachedRenderData::CacheDict_t, CUtlMemory<CCachedRenderData::CacheDict_t, int>>, int>>, int>>::Purge()` | identical | yes | yes | yes |  |
| studiorender | `CStudioRender::DrawModelArray(DrawModelInfo_t const&, StudioRenderContext_t const&, int, model_array_instance_t*, int, int)` | identical | no | no | yes |  |
| studiorender | `CStudioRender::ClearAllShadows()` | identical | yes | yes | yes |  |
| studiorender | `CStudioRender::InitDebugMaterials()` | identical | no | no | yes |  |
| studiorender | `CStudioRender::DrawModelStaticProp(DrawModelInfo_t const&, StudioRenderContext_t const&, matrix3x4_t const&, int)` | identical | no | no | yes |  |
| studiorender | `CStudioRender::SetLightingRenderState()` | differs | no | no | no | not investigated |
| studiorender | `CStudioRender::Init()` | identical | yes | yes | yes |  |
| studiorender | `CStudioRender::Shutdown()` | identical | yes | yes | yes |  |
| studiorender | `CStudioRender::AddShadow(IMaterial*, void*, FlashlightState_t*, VMatrix*, ITexture*)` | identical | yes | yes | yes |  |
| studiorender | `CStudioRender::DrawModel(DrawModelInfo_t const&, StudioRenderContext_t const&, matrix3x4_t*, FlexWeights_t const&, int)` | identical | no | no | yes |  |
| studiorender | `CStudioRender::CStudioRender()` | identical | no | no | yes |  |
| studiorender | `CStudioRender::~CStudioRender()` | identical | yes | yes | yes |  |
| studiorender | `CStudioRender::~CStudioRender()` | identical | no | no | yes |  |
| studiorender | `CUtlLinkedList<CStudioRender::DecalLRU_t, int, true, int, CUtlFixedMemory<UtlLinkedListElem_t<CStudioRender::DecalLRU_t, int>>>::RemoveAll()` | identical | yes | yes | yes |  |
| studiorender | `CUtlLinkedList<CStudioRender::DecalMaterial_t, unsigned short, true, unsigned short, CUtlMemory<UtlLinkedListElem_t<CStudioRender::DecalMaterial_t, unsigned short>, unsigned short>>::RemoveAll()` | identical | no | no | yes |  |
| studiorender | `CUtlLinkedList<CStudioRender::DecalModelList_t, int, true, int, CUtlFixedMemory<UtlLinkedListElem_t<CStudioRender::DecalModelList_t, int>>>::RemoveAll()` | identical | yes | yes | yes |  |
| studiorendercontext | `__GLOBAL__I_a` | identical | no | no | yes |  |
| studiorendercontext | `StudioChangeCallback(IConVar*, char const*, float)` | identical | no | no | yes |  |
| studiorendercontext | `SortVertCount(void const*, void const*)` | identical | yes | yes | yes |  |
| studiorendercontext | `__CreateCStudioRenderContextIStudioRender_interface()` | identical | yes | yes | yes |  |
| studiorendercontext | `CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>::AddRef()` | identical | no | no | yes |  |
| studiorendercontext | `CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>::Release()` | identical | no | no | yes |  |
| studiorendercontext | `mstudiomodel_t::CacheVertexData(void*)` | identical | yes | yes | yes |  |
| studiorendercontext | `CMemberFunctor0<CStudioRender*, void (CStudioRender::*)(), CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>, CFuncMemPolicyNone>::~CMemberFunctor0()` | identical | yes | yes | yes |  |
| studiorendercontext | `CMemberFunctor0<CStudioRender*, void (CStudioRender::*)(), CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>, CFuncMemPolicyNone>::~CMemberFunctor0()` | identical | yes | yes | yes |  |
| studiorendercontext | `CMemberFunctor0<CStudioRender*, void (CStudioRender::*)(), CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>, CFuncMemPolicyNone>::operator()()` | identical | yes | yes | yes |  |
| studiorendercontext | `CMemberFunctor1<CStudioRender*, void (CStudioRender::*)(StudioDecalHandle_t__*), StudioDecalHandle_t__*, CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>, CFuncMemPolicyNone>::~CMemberFunctor1()` | identical | yes | yes | yes |  |
| studiorendercontext | `CMemberFunctor1<CStudioRender*, void (CStudioRender::*)(StudioDecalHandle_t__*), StudioDecalHandle_t__*, CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>, CFuncMemPolicyNone>::~CMemberFunctor1()` | identical | yes | yes | yes |  |
| studiorendercontext | `CMemberFunctor1<CStudioRender*, void (CStudioRender::*)(StudioDecalHandle_t__*), StudioDecalHandle_t__*, CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>, CFuncMemPolicyNone>::operator()()` | identical | yes | yes | yes |  |
| studiorendercontext | `CMemberFunctor3<CStudioRender*, void (CStudioRender::*)(DrawModelInfo_t const&, StudioRenderContext_t const&, matrix3x4_t const&), DrawModelInfo_t, StudioRenderContext_t, matrix3x4_t, CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>, CFuncMemPolicyNone>::~CMemberFunctor3()` | identical | yes | yes | yes |  |
| studiorendercontext | `CMemberFunctor3<CStudioRender*, void (CStudioRender::*)(DrawModelInfo_t const&, StudioRenderContext_t const&, matrix3x4_t const&), DrawModelInfo_t, StudioRenderContext_t, matrix3x4_t, CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>, CFuncMemPolicyNone>::~CMemberFunctor3()` | identical | yes | yes | yes |  |
| studiorendercontext | `CMemberFunctor3<CStudioRender*, void (CStudioRender::*)(DrawModelInfo_t const&, StudioRenderContext_t const&, matrix3x4_t const&), DrawModelInfo_t, StudioRenderContext_t, matrix3x4_t, CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>, CFuncMemPolicyNone>::operator()()` | identical | no | no | yes |  |
| studiorendercontext | `CMemberFunctor4<CStudioRender*, void (CStudioRender::*)(DrawModelInfo_t const&, StudioRenderContext_t const&, matrix3x4_t const&, int), DrawModelInfo_t, StudioRenderContext_t, matrix3x4_t, int, CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>, CFuncMemPolicyNone>::~CMemberFunctor4()` | identical | yes | yes | yes |  |
| studiorendercontext | `CMemberFunctor4<CStudioRender*, void (CStudioRender::*)(DrawModelInfo_t const&, StudioRenderContext_t const&, matrix3x4_t const&, int), DrawModelInfo_t, StudioRenderContext_t, matrix3x4_t, int, CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>, CFuncMemPolicyNone>::~CMemberFunctor4()` | identical | yes | yes | yes |  |
| studiorendercontext | `CMemberFunctor4<CStudioRender*, void (CStudioRender::*)(DrawModelInfo_t const&, StudioRenderContext_t const&, matrix3x4_t const&, int), DrawModelInfo_t, StudioRenderContext_t, matrix3x4_t, int, CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>, CFuncMemPolicyNone>::operator()()` | identical | no | no | yes |  |
| studiorendercontext | `CMemberFunctor5<CStudioRender*, void (CStudioRender::*)(IMaterial*, void*, FlashlightState_t*, VMatrix*, ITexture*), IMaterial*, void*, FlashlightState_t*, VMatrix*, ITexture*, CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>, CFuncMemPolicyNone>::~CMemberFunctor5()` | identical | yes | yes | yes |  |
| studiorendercontext | `CMemberFunctor5<CStudioRender*, void (CStudioRender::*)(IMaterial*, void*, FlashlightState_t*, VMatrix*, ITexture*), IMaterial*, void*, FlashlightState_t*, VMatrix*, ITexture*, CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>, CFuncMemPolicyNone>::~CMemberFunctor5()` | identical | yes | yes | yes |  |
| studiorendercontext | `CMemberFunctor5<CStudioRender*, void (CStudioRender::*)(IMaterial*, void*, FlashlightState_t*, VMatrix*, ITexture*), IMaterial*, void*, FlashlightState_t*, VMatrix*, ITexture*, CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>, CFuncMemPolicyNone>::operator()()` | identical | yes | yes | yes |  |
| studiorendercontext | `CMemberFunctor5<CStudioRender*, void (CStudioRender::*)(DrawModelInfo_t const&, StudioRenderContext_t const&, matrix3x4_t*, FlexWeights_t const&, int), DrawModelInfo_t, StudioRenderContext_t, matrix3x4_t*, FlexWeights_t, int, CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>, CFuncMemPolicyNone>::~CMemberFunctor5()` | identical | yes | yes | yes |  |
| studiorendercontext | `CMemberFunctor5<CStudioRender*, void (CStudioRender::*)(DrawModelInfo_t const&, StudioRenderContext_t const&, matrix3x4_t*, FlexWeights_t const&, int), DrawModelInfo_t, StudioRenderContext_t, matrix3x4_t*, FlexWeights_t, int, CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>, CFuncMemPolicyNone>::~CMemberFunctor5()` | identical | yes | yes | yes |  |
| studiorendercontext | `CMemberFunctor5<CStudioRender*, void (CStudioRender::*)(DrawModelInfo_t const&, StudioRenderContext_t const&, matrix3x4_t*, FlexWeights_t const&, int), DrawModelInfo_t, StudioRenderContext_t, matrix3x4_t*, FlexWeights_t, int, CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>, CFuncMemPolicyNone>::operator()()` | identical | no | no | yes |  |
| studiorendercontext | `CMemberFunctor11<CStudioRender*, void (CStudioRender::*)(StudioDecalHandle_t__*, StudioRenderContext_t const&, matrix3x4_t*, studiohdr_t*, Ray_t const&, Vector const&, IMaterial*, float, int, bool, int), StudioDecalHandle_t__*, StudioRenderContext_t, matrix3x4_t*, studiohdr_t*, Ray_t, Vector, IMaterial*, float, int, bool, int, CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>, CFuncMemPolicyNone>::~CMemberFunctor11()` | identical | yes | yes | yes |  |
| studiorendercontext | `CMemberFunctor11<CStudioRender*, void (CStudioRender::*)(StudioDecalHandle_t__*, StudioRenderContext_t const&, matrix3x4_t*, studiohdr_t*, Ray_t const&, Vector const&, IMaterial*, float, int, bool, int), StudioDecalHandle_t__*, StudioRenderContext_t, matrix3x4_t*, studiohdr_t*, Ray_t, Vector, IMaterial*, float, int, bool, int, CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>, CFuncMemPolicyNone>::~CMemberFunctor11()` | identical | yes | yes | yes |  |
| studiorendercontext | `CMemberFunctor11<CStudioRender*, void (CStudioRender::*)(StudioDecalHandle_t__*, StudioRenderContext_t const&, matrix3x4_t*, studiohdr_t*, Ray_t const&, Vector const&, IMaterial*, float, int, bool, int), StudioDecalHandle_t__*, StudioRenderContext_t, matrix3x4_t*, studiohdr_t*, Ray_t, Vector, IMaterial*, float, int, bool, int, CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>, CFuncMemPolicyNone>::operator()()` | identical | no | no | yes |  |
| studiorendercontext | `CRefCountServiceBase<true, CRefMT>::OnFinalRelease()` | identical | yes | yes | yes |  |
| studiorendercontext | `CStudioRenderContext::BeginFrame()` | identical | no | no | yes |  |
| studiorendercontext | `CStudioRenderContext::Disconnect()` | identical | yes | yes | yes |  |
| studiorendercontext | `CStudioRenderContext::UnloadModel(studiohwdata_t*)` | identical | yes | yes | yes |  |
| studiorendercontext | `CStudioRenderContext::SetViewState(Vector const&, Vector const&, Vector const&, Vector const&)` | identical | no | no | yes |  |
| studiorendercontext | `CStudioRenderContext::UpdateConfig(StudioRenderConfig_t const&)` | identical | no | no | yes |  |
| studiorendercontext | `CStudioRenderContext::LoadMaterials(studiohdr_t*, OptimizedModel::FileHeader_t*, studioloddata_t&, int)` | identical | no | no | yes |  |
| studiorendercontext | `CStudioRenderContext::DrawModelArray(DrawModelInfo_t const&, int, model_array_instance_t*, int, int)` | identical | no | no | yes |  |
| studiorendercontext | `CStudioRenderContext::QueryInterface(char const*)` | identical | yes | yes | yes |  |
| studiorendercontext | `CStudioRenderContext::SetLocalLights(int, LightDesc_t const*)` | differs | no | no | no | not investigated |
| studiorendercontext | `CStudioRenderContext::ClearAllShadows()` | identical | no | no | yes |  |
| studiorendercontext | `CStudioRenderContext::ComputeModelLod(studiohwdata_t*, float, float*)` | identical | yes | yes | yes |  |
| studiorendercontext | `CStudioRenderContext::CreateDecalList(studiohwdata_t*)` | identical | yes | yes | yes |  |
| studiorendercontext | `CStudioRenderContext::GetMaterialList(studiohdr_t*, int, IMaterial**)` | identical | no | no | yes |  |
| studiorendercontext | `CStudioRenderContext::LockFlexWeights(int, float**, float**)` | identical | no | no | yes |  |
| studiorendercontext | `CStudioRenderContext::ComputeRenderLOD(IMatRenderContext*, DrawModelInfo_t const&, Vector const&, float*)` | differs | no | no | no | branch layout around clamp(); likely tier0 basetypes/minmax |
| studiorendercontext | `CStudioRenderContext::DestroyDecalList(StudioDecalHandle_t__*)` | identical | no | no | yes |  |
| studiorendercontext | `CStudioRenderContext::GetCurrentConfig(StudioRenderConfig_t&)` | identical | no | no | yes |  |
| studiorendercontext | `CStudioRenderContext::LockBoneMatrices(int)` | identical | no | no | yes |  |
| studiorendercontext | `CStudioRenderContext::RefreshStudioHdr(studiohdr_t*, studiohwdata_t*)` | identical | yes | yes | yes |  |
| studiorendercontext | `CStudioRenderContext::SetEyeViewTarget(studiohdr_t const*, int, Vector const&)` | identical | no | no | yes |  |
| studiorendercontext | `CStudioRenderContext::BuildDecalBoneMap(studiohdr_t*, int*, int*, int*, mstudiomesh_t*, OptimizedModel::StripGroupHeader_t*)` | identical | no | yes | yes |  |
| studiorendercontext | `CStudioRenderContext::InvokeBindProxies(DrawModelInfo_t const&)` | differs | no | no | no | register allocation; not investigated |
| studiorendercontext | `bool CStudioRenderContext::R_AddVertexToMesh<(VertexCompressionType_t)0>(char const*, bool, CMeshBuilder&, OptimizedModel::Vertex_t*, mstudiomesh_t*, mstudio_meshvertexdata_t const*, bool)` | differs | no | no | no | not investigated |
| studiorendercontext | `bool CStudioRenderContext::R_AddVertexToMesh<(VertexCompressionType_t)1>(char const*, bool, CMeshBuilder&, OptimizedModel::Vertex_t*, mstudiomesh_t*, mstudio_meshvertexdata_t const*, bool)` | differs | no | no | no | not investigated |
| studiorendercontext | `CStudioRenderContext::SetLODSwitchValue(studiohwdata_t&, int, float)` | identical | no | no | yes |  |
| studiorendercontext | `CStudioRenderContext::UnlockFlexWeights()` | identical | yes | yes | yes |  |
| studiorendercontext | `CStudioRenderContext::R_StudioBuildMorph(studiohdr_t*, studiomeshgroup_t*, mstudiomesh_t*, OptimizedModel::StripGroupHeader_t*)` | identical | no | no | yes |  |
| studiorendercontext | `CStudioRenderContext::SetAlphaModulation(float)` | identical | no | no | yes |  |
| studiorendercontext | `CStudioRenderContext::SetColorModulation(float const*)` | identical | no | no | yes |  |
| studiorendercontext | `CStudioRenderContext::UnlockBoneMatrices()` | identical | yes | yes | yes |  |
| studiorendercontext | `CStudioRenderContext::CountFlexedVertices(mstudiomesh_t*, OptimizedModel::StripGroupHeader_t*)` | identical | yes | yes | yes |  |
| studiorendercontext | `CStudioRenderContext::DetermineHWMorphing(mstudiomodel_t*, OptimizedModel::ModelLODHeader_t*)` | identical | no | yes | yes |  |
| studiorendercontext | `CStudioRenderContext::DrawModelStaticProp(DrawModelInfo_t const&, matrix3x4_t const&, int)` | identical | no | no | yes |  |
| studiorendercontext | `CStudioRenderContext::ComputeMaterialFlags(studiohdr_t*, studioloddata_t&, IMaterial*)` | identical | yes | yes | yes |  |
| studiorendercontext | `CStudioRenderContext::DrawStaticPropDecals(DrawModelInfo_t const&, matrix3x4_t const&)` | identical | no | no | yes |  |
| studiorendercontext | `CStudioRenderContext::CalculateVertexFormat(studiohdr_t const*, studioloddata_t const*, mstudiomesh_t const*, OptimizedModel::StripGroupHeader_t*, bool)` | identical | no | yes | yes |  |
| studiorendercontext | `CStudioRenderContext::DrawStaticPropShadows(DrawModelInfo_t const&, matrix3x4_t const&, int)` | identical | no | no | yes |  |
| studiorendercontext | `CStudioRenderContext::SetAmbientLightColors(Vector const*)` | identical | no | no | yes |  |
| studiorendercontext | `CStudioRenderContext::SetAmbientLightColors(Vector4D const*)` | edited | no | no | yes | memcpy instead of memutils::copy |
| studiorendercontext | `CStudioRenderContext::ForcedMaterialOverride(IMaterial*, OverrideType_t)` | identical | no | no | yes |  |
| studiorendercontext | `CStudioRenderContext::R_StudioBuildMeshGroup(char const*, bool, studiomeshgroup_t*, OptimizedModel::StripGroupHeader_t*, mstudiomesh_t*, studiohdr_t*, unsigned long long)` | identical | no | no | yes |  |
| studiorendercontext | `CStudioRenderContext::R_StudioBuildMeshStrips(studiomeshgroup_t*, OptimizedModel::StripGroupHeader_t*)` | edited | no | yes | yes | removed the fork's MDL v49 strip header size |
| studiorendercontext | `CStudioRenderContext::R_StudioCreateSingleMesh(studiohdr_t*, studioloddata_t*, mstudiomesh_t*, OptimizedModel::MeshHeader_t*, int, studiomeshdata_t*, int*)` | identical | no | yes | yes |  |
| studiorendercontext | `CStudioRenderContext::GenerateRandomFlexWeights(int, float*, float*)` | identical | yes | yes | yes |  |
| studiorendercontext | `CStudioRenderContext::GetAmbientLightDirections()` | identical | yes | yes | yes |  |
| studiorendercontext | `CStudioRenderContext::GetNumAmbientLightSamples()` | identical | yes | yes | yes |  |
| studiorendercontext | `CStudioRenderContext::R_StudioCreateStaticMeshes(studiohdr_t*, OptimizedModel::FileHeader_t*, studiohwdata_t*, int, int*)` | identical | yes | yes | yes |  |
| studiorendercontext | `CStudioRenderContext::R_StudioDestroyStaticMeshes(int, studiomeshdata_t**)` | edited | no | no | yes | scalar delete of the mesh array, as retail |
| studiorendercontext | `CStudioRenderContext::ComputeHWMorphDecalBoneRemap(studiohdr_t*, OptimizedModel::FileHeader_t*, studiohwdata_t*, int)` | identical | no | yes | yes |  |
| studiorendercontext | `CStudioRenderContext::GetMaterialListFromBodyAndSkin(unsigned short, int, int, int, IMaterial**)` | identical | yes | yes | yes |  |
| studiorendercontext | `CStudioRenderContext::Init()` | identical | yes | yes | yes |  |
| studiorendercontext | `CStudioRenderContext::Connect(void* (*)(char const*, int*))` | identical | yes | yes | yes |  |
| studiorendercontext | `CStudioRenderContext::AddDecal(StudioDecalHandle_t__*, studiohdr_t*, matrix3x4_t*, Ray_t const&, Vector const&, IMaterial*, float, int, bool, int)` | edited | no | no | yes | no main-thread GetModelSpecificDecalMaterial |
| studiorendercontext | `CStudioRenderContext::EndFrame()` | identical | yes | yes | yes |  |
| studiorendercontext | `CStudioRenderContext::Mat_Stub(IMaterialSystem*)` | identical | yes | yes | yes |  |
| studiorendercontext | `CStudioRenderContext::Shutdown()` | identical | yes | yes | yes |  |
| studiorendercontext | `CStudioRenderContext::AddShadow(IMaterial*, void*, FlashlightState_t*, VMatrix*, ITexture*)` | identical | no | no | yes |  |
| studiorendercontext | `CStudioRenderContext::DrawModel(DrawModelResults_t*, DrawModelInfo_t const&, matrix3x4_t*, float*, float*, Vector const&, int)` | differs | no | no | no | not investigated |
| studiorendercontext | `CStudioRenderContext::LoadModel(studiohdr_t*, void*, studiohwdata_t*)` | differs | no | no | no | not investigated |
| studiorendercontext | `CStudioRenderContext::~CStudioRenderContext()` | identical | yes | yes | yes |  |
| studiorendercontext | `CStudioRenderContext::~CStudioRenderContext()` | identical | yes | yes | yes |  |
| studiorendercontext | `CStudioRenderContext::GetNumLODs(studiohwdata_t const&) const` | identical | yes | yes | yes |  |
| studiorendercontext | `CStudioRenderContext::GetPerfStats(DrawModelResults_t*, DrawModelInfo_t const&, CUtlBuffer*) const` | identical | no | no | yes |  |
| studiorendercontext | `CStudioRenderContext::GetLODSwitchValue(studiohwdata_t const&, int) const` | identical | yes | yes | yes |  |
| studiorendercontext | `non-virtual thunk to CMemberFunctor0<CStudioRender*, void (CStudioRender::*)(), CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>, CFuncMemPolicyNone>::~CMemberFunctor0()` | identical | yes | yes | yes |  |
| studiorendercontext | `non-virtual thunk to CMemberFunctor0<CStudioRender*, void (CStudioRender::*)(), CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>, CFuncMemPolicyNone>::~CMemberFunctor0()` | identical | yes | yes | yes |  |
| studiorendercontext | `non-virtual thunk to CMemberFunctor1<CStudioRender*, void (CStudioRender::*)(StudioDecalHandle_t__*), StudioDecalHandle_t__*, CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>, CFuncMemPolicyNone>::~CMemberFunctor1()` | identical | yes | yes | yes |  |
| studiorendercontext | `non-virtual thunk to CMemberFunctor1<CStudioRender*, void (CStudioRender::*)(StudioDecalHandle_t__*), StudioDecalHandle_t__*, CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>, CFuncMemPolicyNone>::~CMemberFunctor1()` | identical | yes | yes | yes |  |
| studiorendercontext | `non-virtual thunk to CMemberFunctor3<CStudioRender*, void (CStudioRender::*)(DrawModelInfo_t const&, StudioRenderContext_t const&, matrix3x4_t const&), DrawModelInfo_t, StudioRenderContext_t, matrix3x4_t, CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>, CFuncMemPolicyNone>::~CMemberFunctor3()` | identical | yes | yes | yes |  |
| studiorendercontext | `non-virtual thunk to CMemberFunctor3<CStudioRender*, void (CStudioRender::*)(DrawModelInfo_t const&, StudioRenderContext_t const&, matrix3x4_t const&), DrawModelInfo_t, StudioRenderContext_t, matrix3x4_t, CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>, CFuncMemPolicyNone>::~CMemberFunctor3()` | identical | yes | yes | yes |  |
| studiorendercontext | `non-virtual thunk to CMemberFunctor4<CStudioRender*, void (CStudioRender::*)(DrawModelInfo_t const&, StudioRenderContext_t const&, matrix3x4_t const&, int), DrawModelInfo_t, StudioRenderContext_t, matrix3x4_t, int, CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>, CFuncMemPolicyNone>::~CMemberFunctor4()` | identical | yes | yes | yes |  |
| studiorendercontext | `non-virtual thunk to CMemberFunctor4<CStudioRender*, void (CStudioRender::*)(DrawModelInfo_t const&, StudioRenderContext_t const&, matrix3x4_t const&, int), DrawModelInfo_t, StudioRenderContext_t, matrix3x4_t, int, CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>, CFuncMemPolicyNone>::~CMemberFunctor4()` | identical | yes | yes | yes |  |
| studiorendercontext | `non-virtual thunk to CMemberFunctor5<CStudioRender*, void (CStudioRender::*)(IMaterial*, void*, FlashlightState_t*, VMatrix*, ITexture*), IMaterial*, void*, FlashlightState_t*, VMatrix*, ITexture*, CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>, CFuncMemPolicyNone>::~CMemberFunctor5()` | identical | yes | yes | yes |  |
| studiorendercontext | `non-virtual thunk to CMemberFunctor5<CStudioRender*, void (CStudioRender::*)(IMaterial*, void*, FlashlightState_t*, VMatrix*, ITexture*), IMaterial*, void*, FlashlightState_t*, VMatrix*, ITexture*, CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>, CFuncMemPolicyNone>::~CMemberFunctor5()` | identical | yes | yes | yes |  |
| studiorendercontext | `non-virtual thunk to CMemberFunctor5<CStudioRender*, void (CStudioRender::*)(DrawModelInfo_t const&, StudioRenderContext_t const&, matrix3x4_t*, FlexWeights_t const&, int), DrawModelInfo_t, StudioRenderContext_t, matrix3x4_t*, FlexWeights_t, int, CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>, CFuncMemPolicyNone>::~CMemberFunctor5()` | identical | yes | yes | yes |  |
| studiorendercontext | `non-virtual thunk to CMemberFunctor5<CStudioRender*, void (CStudioRender::*)(DrawModelInfo_t const&, StudioRenderContext_t const&, matrix3x4_t*, FlexWeights_t const&, int), DrawModelInfo_t, StudioRenderContext_t, matrix3x4_t*, FlexWeights_t, int, CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>, CFuncMemPolicyNone>::~CMemberFunctor5()` | identical | yes | yes | yes |  |
| studiorendercontext | `non-virtual thunk to CMemberFunctor11<CStudioRender*, void (CStudioRender::*)(StudioDecalHandle_t__*, StudioRenderContext_t const&, matrix3x4_t*, studiohdr_t*, Ray_t const&, Vector const&, IMaterial*, float, int, bool, int), StudioDecalHandle_t__*, StudioRenderContext_t, matrix3x4_t*, studiohdr_t*, Ray_t, Vector, IMaterial*, float, int, bool, int, CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>, CFuncMemPolicyNone>::~CMemberFunctor11()` | identical | yes | yes | yes |  |
| studiorendercontext | `non-virtual thunk to CMemberFunctor11<CStudioRender*, void (CStudioRender::*)(StudioDecalHandle_t__*, StudioRenderContext_t const&, matrix3x4_t*, studiohdr_t*, Ray_t const&, Vector const&, IMaterial*, float, int, bool, int), StudioDecalHandle_t__*, StudioRenderContext_t, matrix3x4_t*, studiohdr_t*, Ray_t, Vector, IMaterial*, float, int, bool, int, CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>, CFuncMemPolicyNone>::~CMemberFunctor11()` | identical | yes | yes | yes |  |

r_studiolight: all 514 `__Function_R_LightEffects*FunctionTable<N>::Run`
instances match with current headers and with the patch.

Link composition (for the manifest lane): retail links no `memoverride.o`
and no `r_studiodraw_computeflexedvertex.o`. Retail order: flexrenderdata,
r_studio, r_studiodecal, r_studiodraw, r_studioflex, r_studiogettriangles,
r_studiolight, r_studiostats, studiorender, studiorendercontext, then tier3,
mathlib (color_conversion, mathlib_base, sse, sseconst, vmatrix), tier2
(meshutils, tier2), tier1 (convar, interface, KeyValues, strtools, tier1,
utlbuffer). Retail does not link bitmap; our wscript does.
