# tier2 (libtier2.a)

Retail: `libtier2.a` members in depot 223 `engine.dylib` (65 functions). All
match after this lane (62/65 before).

| function | status | note |
|---|---|---|
| InitializeStandardMaterials | edited | takes a reference (`IncrementReferenceCount`) on each of the four materials it creates |
| DrawScreenSpaceRectangle | edited | half-pixel offset is `IsPosix() ? 0.0f : 0.5f` (SDK 2013 viewpostprocess.cpp form) |
| CUtlStreamBuffer::CUtlStreamBuffer(const char*, const char*, int, bool) | edited | delayed-open file name copied with `new char[strlen+1]` + `strcpy`, like the path; `V_strdup` (memcpy) is later |
| ComputeProjectionMatrix (camerautils) | edited | `memset` of the matrix, not the fork's `VMatrix::Init( 0, ... )`. Checked against the anniversary hl2 client and the 2014 hl2mp client (engine dead-strips it) |
