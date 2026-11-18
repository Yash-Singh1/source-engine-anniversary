# mathlib (libtier1.a / libmathlib.a members, depot 223)

Measured with devtools/bytematch (macos-cc.sh, clang-600.0.57, call targets checked) against the
retail module that links each member: engine.dylib, or hl2 client.dylib (lightdesc, powsse,
randsse, ssenoise, rangecheckedvar), hl2 server.dylib (stringpool), scenefilecache.dylib
(lzmaDecoder). "identical" means matched before this lane's edits too; "edited" means it
matches after a source change. Counts: {'identical': 81, 'uncertain': 1, 'edited': 24}.

Retail compiles tier1/mathlib from source in every module; the hl2 client/server and libvstdlib
link the same archive members (same N_OSO mtimes as engine). Lost Coast (Oct 2024) is an older
snapshot. Members compiled but not linked by any retail module (keyvaluesjson, utlbinaryblock,
LzmaDec.c) were removed.

| object | function | status | note |
|---|---|---|---|
| IceKey | `IceKey::IceKey(int)` | identical |  |
| IceKey | `IceKey::IceKey(int)` | identical |  |
| IceKey | `IceKey::encrypt(unsigned char const*, unsigned char*) const` | identical |  |
| IceKey | `IceKey::scheduleBuild(unsigned short*, int, int const*)` | identical |  |
| IceKey | `IceKey::set(unsigned char const*)` | identical |  |
| IceKey | `IceKey::~IceKey()` | identical |  |
| bumpvects | `GetBumpNormals(Vector const&, Vector const&, Vector const&, Vector const&, Vector*)` | identical |  |
| color_conversion | `BuildGammaTable(float, float, float, int)` | uncertain | source identical to SDK 2013; retail is 18 bytes larger, cause not found |
| color_conversion | `ColorRGBExp32ToVector(ColorRGBExp32 const&, Vector&)` | identical |  |
| color_conversion | `LinearToScreenGamma(float)` | identical |  |
| color_conversion | `LinearToTexture(float)` | identical |  |
| color_conversion | `TextureToLinear(int)` | identical |  |
| color_conversion | `VectorToColorRGBExp32(Vector const&, ColorRGBExp32&)` | identical |  |
| lightdesc | `LightDesc_t::ComputeLightAtPoints(FourVectors const&, FourVectors const&, FourVectors&, bool) const` | identical |  |
| lightdesc | `LightDesc_t::ComputeNonincidenceLightAtPoints(FourVectors const&, FourVectors&) const` | identical |  |
| lightdesc | `LightDesc_t::RecalculateDerivedValues()` | identical |  |
| lightdesc | `LightDesc_t::SetupNewStyleAttenuation(float, float)` | identical |  |
| mathlib_base | `AddPointToBounds(Vector const&, Vector&, Vector&)` | identical |  |
| mathlib_base | `AngleIMatrix(QAngle const&, matrix3x4_t&)` | identical |  |
| mathlib_base | `AngleMatrix(QAngle const&, Vector const&, matrix3x4_t&)` | identical |  |
| mathlib_base | `AngleMatrix(QAngle const&, matrix3x4_t&)` | identical |  |
| mathlib_base | `AngleNormalize(float)` | identical |  |
| mathlib_base | `AngleNormalizePositive(float)` | identical |  |
| mathlib_base | `AngleQuaternion(QAngle const&, Quaternion&)` | identical |  |
| mathlib_base | `AngleVectors(QAngle const&, Vector*)` | identical |  |
| mathlib_base | `AngleVectors(QAngle const&, Vector*, Vector*, Vector*)` | identical |  |
| mathlib_base | `Approach(float, float, float)` | identical |  |
| mathlib_base | `BoxOnPlaneSide(float const*, float const*, cplane_t const*)` | identical |  |
| mathlib_base | `CalcFovY(float, float)` | identical |  |
| mathlib_base | `CalcSqrDistanceToAABB(Vector const&, Vector const&, Vector const&)` | identical |  |
| mathlib_base | `Catmull_Rom_Spline(Vector const&, Vector const&, Vector const&, Vector const&, float, Vector&)` | identical |  |
| mathlib_base | `Catmull_Rom_Spline_Normalize(Vector const&, Vector const&, Vector const&, Vector const&, float, Vector&)` | identical |  |
| mathlib_base | `ClearBounds(Vector&, Vector&)` | identical |  |
| mathlib_base | `ClipPolyToPlane(Vector*, int, Vector*, Vector const&, float, float)` | identical |  |
| mathlib_base | `ComputeTrianglePlane(Vector const&, Vector const&, Vector const&, Vector&, float&)` | identical |  |
| mathlib_base | `CrossProduct(float const*, float const*, float*)` | identical |  |
| mathlib_base | `GeneratePerspectiveFrustum(Vector const&, Vector const&, Vector const&, Vector const&, float, float, float, float, Frustum_t&)` | identical |  |
| mathlib_base | `HSVtoRGB(Vector const&, Vector&)` | identical |  |
| mathlib_base | `MathLib_3DNowEnabled()` | identical |  |
| mathlib_base | `MathLib_Init(float, float, float, int, bool, bool, bool, bool)` | identical |  |
| mathlib_base | `MathLib_MMXEnabled()` | identical |  |
| mathlib_base | `MathLib_SSE2Enabled()` | identical |  |
| mathlib_base | `MathLib_SSEEnabled()` | identical |  |
| mathlib_base | `MatrixAngles(matrix3x4_t const&, float*)` | identical |  |
| mathlib_base | `MatrixBuildRotationAboutAxis(Vector const&, float, matrix3x4_t&)` | edited | restored from SDK 2013 (was missing) |
| mathlib_base | `MatrixCopy(matrix3x4_t const&, matrix3x4_t&)` | identical |  |
| mathlib_base | `MatrixGetColumn(matrix3x4_t const&, int, Vector&)` | identical |  |
| mathlib_base | `PolyFromPlane(Vector*, Vector const&, float, float)` | identical |  |
| mathlib_base | `Q_log2(int)` | identical |  |
| mathlib_base | `QuaternionAlign(Quaternion const&, Quaternion const&, Quaternion&)` | identical |  |
| mathlib_base | `QuaternionAngles(Quaternion const&, QAngle&)` | identical |  |
| mathlib_base | `QuaternionBlend(Quaternion const&, Quaternion const&, float, Quaternion&)` | identical |  |
| mathlib_base | `QuaternionMatrix(Quaternion const&, matrix3x4_t&)` | edited | SDK asserts, MatrixBuildRotationAboutAxis |
| mathlib_base | `QuaternionSlerp(Quaternion const&, Quaternion const&, float, Quaternion&)` | identical |  |
| mathlib_base | `QuaternionSlerpNoAlign(Quaternion const&, Quaternion const&, float, Quaternion&)` | identical |  |
| mathlib_base | `RGBtoHSV(Vector const&, Vector&)` | edited | SDK asserts, MatrixBuildRotationAboutAxis |
| mathlib_base | `R_CullBox(Vector const&, Vector const&, Frustum_t const&)` | identical |  |
| mathlib_base | `R_CullBoxSkipNear(Vector const&, Vector const&, Frustum_t const&)` | identical |  |
| mathlib_base | `SetIdentityMatrix(matrix3x4_t&)` | identical |  |
| mathlib_base | `SignbitsForPlane(cplane_t*)` | identical |  |
| mathlib_base | `SinCos(float, float*, float*)` | identical |  |
| mathlib_base | `TransformAABB(matrix3x4_t const&, Vector const&, Vector const&, Vector&, Vector&)` | identical |  |
| mathlib_base | `VectorAngles(Vector const&, QAngle&)` | identical |  |
| mathlib_base | `VectorIRotate(float const*, matrix3x4_t const&, float*)` | identical |  |
| mathlib_base | `VectorITransform(float const*, matrix3x4_t const&, float*)` | identical |  |
| mathlib_base | `VectorRotate(float const*, matrix3x4_t const&, float*)` | identical |  |
| mathlib_base | `VectorTransform(float const*, matrix3x4_t const&, float*)` | identical |  |
| mathlib_base | `VectorVectors(Vector const&, Vector&, Vector&)` | identical |  |
| mathlib_base | `_InvRSquared(float const*)` | identical |  |
| mathlib_base | `_VectorNormalize(Vector&)` | identical |  |
| mathlib_base | `_VectorNormalizeFast(Vector&)` | identical |  |
| mathlib_base | `_rsqrtf(float)` | identical |  |
| mathlib_base | `_sqrtf(float)` | identical |  |
| powsse | `Pow_FixedPoint_Exponent_SIMD(float vector[4] const&, int)` | identical |  |
| randsse | `GetSIMDRandContext()` | edited | tier0 threadtools (tier0 lane) |
| randsse | `RandSIMD(int)` | identical |  |
| randsse | `ReleaseSIMDRandContext(int)` | identical |  |
| randsse | `SeedRandSIMD(unsigned int)` | identical |  |
| sse | `_SSE_InvRSquared(float const*)` | identical |  |
| sse | `_SSE_RSqrtAccurate(float)` | identical |  |
| sse | `_SSE_RSqrtFast(float)` | identical |  |
| sse | `_SSE_Sqrt(float)` | identical |  |
| sse | `_SSE_VectorNormalizeFast(Vector&)` | identical |  |
| sse | `__GLOBAL__I_a` | identical |  |
| ssenoise | `NoiseSIMD(FourVectors const&)` | identical |  |
| ssenoise | `NoiseSIMD(float vector[4] const&, float vector[4] const&, float vector[4] const&)` | identical |  |
| vmatrix | `CalculateAABBFromProjectionMatrixInverse(VMatrix const&, Vector*, Vector*)` | edited | SDK out-of-line VMatrix |
| vmatrix | `CalculateSphereFromProjectionMatrixInverse(VMatrix const&, Vector*, float*)` | edited | SDK out-of-line VMatrix |
| vmatrix | `FrustumPlanesFromMatrix(VMatrix const&, Frustum_t&)` | edited | SDK out-of-line VMatrix |
| vmatrix | `MatrixBuildOrtho(VMatrix&, double, double, double, double, double, double)` | edited | SDK out-of-line VMatrix |
| vmatrix | `MatrixBuildPerspectiveOffCenterX(VMatrix&, double, double, double, double, double, double, double, double)` | edited | SDK out-of-line VMatrix |
| vmatrix | `MatrixBuildPerspectiveX(VMatrix&, double, double, double, double)` | edited | SDK out-of-line VMatrix |
| vmatrix | `MatrixBuildPerspectiveZRange(VMatrix&, double, double)` | edited | SDK out-of-line VMatrix |
| vmatrix | `MatrixBuildRotationAboutAxis(VMatrix&, Vector const&, float)` | edited | SDK out-of-line VMatrix |
| vmatrix | `MatrixBuildTranslation(VMatrix&, Vector const&)` | edited | SDK out-of-line VMatrix |
| vmatrix | `MatrixCopy(VMatrix const&, VMatrix&)` | edited | SDK out-of-line VMatrix |
| vmatrix | `MatrixInverseGeneral(VMatrix const&, VMatrix&)` | edited | SDK out-of-line VMatrix |
| vmatrix | `MatrixInverseTR(VMatrix const&, VMatrix&)` | edited | SDK out-of-line VMatrix |
| vmatrix | `MatrixMultiply(VMatrix const&, VMatrix const&, VMatrix&)` | edited | SDK out-of-line VMatrix |
| vmatrix | `MatrixSetIdentity(VMatrix&)` | edited | SDK out-of-line VMatrix |
| vmatrix | `MatrixTransformPlane(VMatrix const&, cplane_t const&, cplane_t&)` | edited | SDK out-of-line VMatrix |
| vmatrix | `VMatrix::SetupMatrixOrgAngles(Vector const&, QAngle const&)` | edited | SDK out-of-line VMatrix |
| vmatrix | `VMatrix::operator=(VMatrix const&)` | edited | SDK out-of-line VMatrix |
| vmatrix | `Vector3DMultiply(VMatrix const&, Vector const&, Vector&)` | edited | SDK out-of-line VMatrix |
| vmatrix | `Vector3DMultiplyPositionProjective(VMatrix const&, Vector const&, Vector&)` | edited | SDK out-of-line VMatrix |
| vmatrix | `Vector4DMultiply(VMatrix const&, Vector4D const&, Vector4D&)` | edited | SDK out-of-line VMatrix |
