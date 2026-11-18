# filesystem_stdio: per-function status vs retail depot 223

Generated from devtools/bytematch on the --retail-osx32 build (linked dylib vs retail dylib, scope = retail debug-map objects).
"with needed" is the per-object status once the shared header changes in lane-mod-filesystem.md land (lane-mod-filesystem-scratch/needed.patch).

## kevvaluescompiler (33/33)

| function | status | with needed | note |
|---|---|---|---|
| `CCompiledKeyValuesReader::CCompiledKeyValuesReader()` | masked | masked |  |
| `CCompiledKeyValuesReader::CCompiledKeyValuesReader()` | exact | exact |  |
| `CCompiledKeyValuesReader::CreateInPlaceFromData(KeyValues&, CCompiledKeyValuesReader::FileInfo_t const&)` | masked | masked |  |
| `CCompiledKeyValuesReader::FileInfo_t::Less(CCompiledKeyValuesReader::FileInfo_t const&, CCompiledKeyValuesReader::FileInfo_t const&)` | exact | exact |  |
| `CCompiledKeyValuesReader::Instance(char const*)` | masked | masked |  |
| `CCompiledKeyValuesReader::InstanceInPlace(KeyValues&, char const*)` | masked | masked |  |
| `CCompiledKeyValuesReader::LoadFile(char const*)` | masked | masked |  |
| `CCompiledKeyValuesReader::LookupKeyValuesRootKeyName(char const*, char*, unsigned long)` | masked | masked |  |
| `CRunTimeKeyValuesStringTable::ReadStringTable(int, CUtlBuffer&)` | masked | masked |  |
| `CUtlMemory<KVInfo_t, int>::Grow(int)` | masked | masked |  |
| `CUtlMemory<UtlRBTreeNode_t<CCompiledKeyValuesReader::FileInfo_t, unsigned short>, unsigned short>::Grow(int)` | masked | masked |  |
| `CUtlMemory<UtlRBTreeNode_t<CreateHelper_t, int>, int>::Grow(int)` | masked | masked |  |
| `CUtlMemory<char const*, int>::Grow(int)` | masked | masked |  |
| `CUtlRBTree<CCompiledKeyValuesReader::FileInfo_t, unsigned short, bool (*)(CCompiledKeyValuesReader::FileInfo_t const&, CCompiledKeyValuesReader::FileI` | masked | masked |  |
| `CUtlRBTree<CCompiledKeyValuesReader::FileInfo_t, unsigned short, bool (*)(CCompiledKeyValuesReader::FileInfo_t const&, CCompiledKeyValuesReader::FileI` | masked | masked |  |
| `CUtlRBTree<CCompiledKeyValuesReader::FileInfo_t, unsigned short, bool (*)(CCompiledKeyValuesReader::FileInfo_t const&, CCompiledKeyValuesReader::FileI` | exact | masked |  |
| `CUtlRBTree<CCompiledKeyValuesReader::FileInfo_t, unsigned short, bool (*)(CCompiledKeyValuesReader::FileInfo_t const&, CCompiledKeyValuesReader::FileI` | masked | masked |  |
| `CUtlRBTree<CCompiledKeyValuesReader::FileInfo_t, unsigned short, bool (*)(CCompiledKeyValuesReader::FileInfo_t const&, CCompiledKeyValuesReader::FileI` | masked | masked |  |
| `CUtlRBTree<CCompiledKeyValuesReader::FileInfo_t, unsigned short, bool (*)(CCompiledKeyValuesReader::FileInfo_t const&, CCompiledKeyValuesReader::FileI` | masked | masked |  |
| `CUtlRBTree<CCompiledKeyValuesReader::FileInfo_t, unsigned short, bool (*)(CCompiledKeyValuesReader::FileInfo_t const&, CCompiledKeyValuesReader::FileI` | masked | masked |  |
| `CUtlRBTree<CCompiledKeyValuesReader::FileInfo_t, unsigned short, bool (*)(CCompiledKeyValuesReader::FileInfo_t const&, CCompiledKeyValuesReader::FileI` | masked | masked |  |
| `CUtlRBTree<CCompiledKeyValuesReader::FileInfo_t, unsigned short, bool (*)(CCompiledKeyValuesReader::FileInfo_t const&, CCompiledKeyValuesReader::FileI` | masked | masked |  |
| `CUtlRBTree<CCompiledKeyValuesReader::FileInfo_t, unsigned short, bool (*)(CCompiledKeyValuesReader::FileInfo_t const&, CCompiledKeyValuesReader::FileI` | masked | masked |  |
| `CUtlRBTree<CreateHelper_t, int, bool (*)(CreateHelper_t const&, CreateHelper_t const&), CUtlMemory<UtlRBTreeNode_t<CreateHelper_t, int>, int>>::Find(C` | masked | masked |  |
| `CUtlRBTree<CreateHelper_t, int, bool (*)(CreateHelper_t const&, CreateHelper_t const&), CUtlMemory<UtlRBTreeNode_t<CreateHelper_t, int>, int>>::FindIn` | masked | masked |  |
| `CUtlRBTree<CreateHelper_t, int, bool (*)(CreateHelper_t const&, CreateHelper_t const&), CUtlMemory<UtlRBTreeNode_t<CreateHelper_t, int>, int>>::Insert` | masked | masked |  |
| `CUtlRBTree<CreateHelper_t, int, bool (*)(CreateHelper_t const&, CreateHelper_t const&), CUtlMemory<UtlRBTreeNode_t<CreateHelper_t, int>, int>>::Insert` | masked | masked |  |
| `CUtlRBTree<CreateHelper_t, int, bool (*)(CreateHelper_t const&, CreateHelper_t const&), CUtlMemory<UtlRBTreeNode_t<CreateHelper_t, int>, int>>::IsLeft` | masked | masked |  |
| `CUtlRBTree<CreateHelper_t, int, bool (*)(CreateHelper_t const&, CreateHelper_t const&), CUtlMemory<UtlRBTreeNode_t<CreateHelper_t, int>, int>>::IsRigh` | masked | masked |  |
| `CUtlRBTree<CreateHelper_t, int, bool (*)(CreateHelper_t const&, CreateHelper_t const&), CUtlMemory<UtlRBTreeNode_t<CreateHelper_t, int>, int>>::Remove` | masked | masked |  |
| `CUtlRBTree<CreateHelper_t, int, bool (*)(CreateHelper_t const&, CreateHelper_t const&), CUtlMemory<UtlRBTreeNode_t<CreateHelper_t, int>, int>>::Rotate` | masked | masked |  |
| `CUtlRBTree<CreateHelper_t, int, bool (*)(CreateHelper_t const&, CreateHelper_t const&), CUtlMemory<UtlRBTreeNode_t<CreateHelper_t, int>, int>>::Rotate` | masked | masked |  |
| `CreateHelper_t::Less(CreateHelper_t const&, CreateHelper_t const&)` | exact | exact |  |

## zip_utils (64/65)

| function | status | with needed | note |
|---|---|---|---|
| `CBufferStream::Put(void const*, int)` | masked | masked |  |
| `CBufferStream::Tell()` | exact | exact |  |
| `CDatadescGeneratedNameHolder::~CDatadescGeneratedNameHolder()` | differs | differs | reloc target at +0x2e: ours _ZdaPv, retail _ZdlPv |
| `CFileStream::Put(void const*, int)` | masked | masked |  |
| `CFileStream::Tell()` | masked | masked |  |
| `CUtlLinkedList<CZip*, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<CZip*, unsigned short>, unsigned short>>::AllocInternal(bo` | masked | masked |  |
| `CUtlLinkedList<CZip*, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<CZip*, unsigned short>, unsigned short>>::LinkBefore(unsig` | exact | exact |  |
| `CUtlLinkedList<CZip*, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<CZip*, unsigned short>, unsigned short>>::~CUtlLinkedList(` | masked | masked |  |
| `CUtlMemory<UtlLinkedListElem_t<CZip*, unsigned short>, unsigned short>::Grow(int)` | masked | masked |  |
| `CUtlMemory<UtlRBTreeNode_t<CZipFile::CZipEntry, int>, int>::Grow(int)` | masked | masked |  |
| `CUtlRBTree<CZipFile::CZipEntry, int, bool (*)(CZipFile::CZipEntry const&, CZipFile::CZipEntry const&), CUtlMemory<UtlRBTreeNode_t<CZipFile::CZipEntry,` | masked | masked |  |
| `CUtlRBTree<CZipFile::CZipEntry, int, bool (*)(CZipFile::CZipEntry const&, CZipFile::CZipEntry const&), CUtlMemory<UtlRBTreeNode_t<CZipFile::CZipEntry,` | masked | masked |  |
| `CUtlRBTree<CZipFile::CZipEntry, int, bool (*)(CZipFile::CZipEntry const&, CZipFile::CZipEntry const&), CUtlMemory<UtlRBTreeNode_t<CZipFile::CZipEntry,` | masked | masked |  |
| `CUtlRBTree<CZipFile::CZipEntry, int, bool (*)(CZipFile::CZipEntry const&, CZipFile::CZipEntry const&), CUtlMemory<UtlRBTreeNode_t<CZipFile::CZipEntry,` | masked | masked |  |
| `CUtlRBTree<CZipFile::CZipEntry, int, bool (*)(CZipFile::CZipEntry const&, CZipFile::CZipEntry const&), CUtlMemory<UtlRBTreeNode_t<CZipFile::CZipEntry,` | masked | masked |  |
| `CUtlRBTree<CZipFile::CZipEntry, int, bool (*)(CZipFile::CZipEntry const&, CZipFile::CZipEntry const&), CUtlMemory<UtlRBTreeNode_t<CZipFile::CZipEntry,` | masked | masked |  |
| `CUtlRBTree<CZipFile::CZipEntry, int, bool (*)(CZipFile::CZipEntry const&, CZipFile::CZipEntry const&), CUtlMemory<UtlRBTreeNode_t<CZipFile::CZipEntry,` | masked | masked |  |
| `CUtlRBTree<CZipFile::CZipEntry, int, bool (*)(CZipFile::CZipEntry const&, CZipFile::CZipEntry const&), CUtlMemory<UtlRBTreeNode_t<CZipFile::CZipEntry,` | masked | masked |  |
| `CUtlRBTree<CZipFile::CZipEntry, int, bool (*)(CZipFile::CZipEntry const&, CZipFile::CZipEntry const&), CUtlMemory<UtlRBTreeNode_t<CZipFile::CZipEntry,` | masked | masked |  |
| `CUtlRBTree<CZipFile::CZipEntry, int, bool (*)(CZipFile::CZipEntry const&, CZipFile::CZipEntry const&), CUtlMemory<UtlRBTreeNode_t<CZipFile::CZipEntry,` | masked | masked |  |
| `CUtlRBTree<CZipFile::CZipEntry, int, bool (*)(CZipFile::CZipEntry const&, CZipFile::CZipEntry const&), CUtlMemory<UtlRBTreeNode_t<CZipFile::CZipEntry,` | masked | masked |  |
| `CUtlRBTree<CZipFile::CZipEntry, int, bool (*)(CZipFile::CZipEntry const&, CZipFile::CZipEntry const&), CUtlMemory<UtlRBTreeNode_t<CZipFile::CZipEntry,` | masked | masked |  |
| `CUtlRBTree<CZipFile::CZipEntry, int, bool (*)(CZipFile::CZipEntry const&, CZipFile::CZipEntry const&), CUtlMemory<UtlRBTreeNode_t<CZipFile::CZipEntry,` | masked | masked |  |
| `CUtlRBTree<CZipFile::CZipEntry, int, bool (*)(CZipFile::CZipEntry const&, CZipFile::CZipEntry const&), CUtlMemory<UtlRBTreeNode_t<CZipFile::CZipEntry,` | masked | masked |  |
| `CWin32File::CreateTempFile(CUtlString&, CUtlString&)` | masked | masked |  |
| `CZip::ActivateByteSwapping(bool)` | exact | exact |  |
| `CZip::AddBufferToZip(char const*, void*, int, bool)` | exact | masked |  |
| `CZip::AddFileToZip(char const*, char const*)` | exact | masked |  |
| `CZip::CZip(char const*, bool)` | masked | masked |  |
| `CZip::EstimateSize()` | exact | masked |  |
| `CZip::FileExistsInZip(char const*)` | exact | masked |  |
| `CZip::ForceAlignment(bool, bool, unsigned int)` | exact | exact |  |
| `CZip::GetAlignment()` | exact | exact |  |
| `CZip::GetNextFilename(int, char*, int, int&)` | masked | masked |  |
| `CZip::ParseFromBuffer(void*, int)` | masked | masked |  |
| `CZip::ParseFromDisk(char const*)` | masked | masked |  |
| `CZip::PrintDirectory()` | masked | masked |  |
| `CZip::ReadFileFromZip(char const*, bool, CUtlBuffer&)` | exact | masked |  |
| `CZip::ReadFileFromZip(void*, char const*, bool, CUtlBuffer&)` | exact | masked |  |
| `CZip::RemoveFileFromZip(char const*)` | exact | masked |  |
| `CZip::Reset()` | masked | masked |  |
| `CZip::SaveToBuffer(CUtlBuffer&)` | masked | masked |  |
| `CZip::SaveToDisk(__sFILE*)` | masked | masked |  |
| `CZip::SaveToDisk(void*)` | masked | masked |  |
| `CZip::SetBigEndian(bool)` | exact | exact |  |
| `CZip::~CZip()` | masked | masked |  |
| `CZip::~CZip()` | masked | masked |  |
| `CZipFile::AddBufferToZip(char const*, void*, int, bool)` | masked | masked |  |
| `CZipFile::AddFileToZip(char const*, char const*)` | masked | masked |  |
| `CZipFile::CZipEntry::ZipFileLessFunc(CZipFile::CZipEntry const&, CZipFile::CZipEntry const&)` | exact | exact |  |
| `CZipFile::CZipEntry::ZipFileLessFunc_CaselessSort(CZipFile::CZipEntry const&, CZipFile::CZipEntry const&)` | masked | masked |  |
| `CZipFile::CZipFile(char const*, bool)` | masked | masked |  |
| `CZipFile::CalculateSize()` | masked | masked |  |
| `CZipFile::FileExistsInZip(char const*)` | masked | masked |  |
| `CZipFile::ParseFromBuffer(void*, int)` | masked | masked |  |
| `CZipFile::ParseFromDisk(char const*)` | masked | masked |  |
| `CZipFile::ReadFileFromZip(char const*, bool, CUtlBuffer&)` | masked | masked |  |
| `CZipFile::ReadFileFromZip(void*, char const*, bool, CUtlBuffer&)` | masked | masked |  |
| `CZipFile::RemoveFileFromZip(char const*)` | masked | masked |  |
| `CZipFile::SaveDirectory(IWriteStream&)` | masked | masked |  |
| `CZipFile::~CZipFile()` | masked | masked |  |
| `IZip::CreateZip(char const*, bool)` | masked | masked |  |
| `IZip::ReleaseZip(IZip*)` | masked | masked |  |
| `__GLOBAL__I_a` | masked | masked |  |
| `___clang_call_terminate` | masked | masked |  |

## basefilesystem (155/257)

| function | status | with needed | note |
|---|---|---|---|
| `BaseFileSystem()` | masked | masked |  |
| `CBaseFileSystem::AddFilesToFileCache(void*, char const**, int, char const*)` | exact | exact |  |
| `CBaseFileSystem::AddGameAndModSearchPathsFromGameInfo(char const*, char const*, KeyValues*)` | differs | differs | size retail 4128 vs ours 3260 |
| `CBaseFileSystem::AddLoggingFunc(void (*)(char const*, char const*))` | masked | masked |  |
| `CBaseFileSystem::AddMapPackFile(char const*, char const*, SearchPathAdd_t)` | differs | differs | size retail 2701 vs ours 2062 |
| `CBaseFileSystem::AddPackFile(char const*, char const*)` | masked | masked |  |
| `CBaseFileSystem::AddPackFileFromPath(char const*, char const*, bool, char const*)` | differs | differs | size retail 1032 vs ours 759 |
| `CBaseFileSystem::AddPackFiles(char const*, CUtlSymbol const&, SearchPathAdd_t)` | differs | differs | size retail 2767 vs ours 2106 |
| `CBaseFileSystem::AddSearchPath(char const*, char const*, SearchPathAdd_t)` | masked | masked |  |
| `CBaseFileSystem::AddSearchPathInternal(char const*, char const*, SearchPathAdd_t, bool)` | masked | masked |  |
| `CBaseFileSystem::AddVPKFile(char const*, char const*, SearchPathAdd_t)` | differs | masked | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CBaseFileSystem::AllocOptimalReadBuffer(void*, unsigned int, unsigned int)` | masked | masked |  |
| `CBaseFileSystem::AsyncAppend(char const*, void const*, int, bool, FSAsyncControl_t__**)` | exact | exact |  |
| `CBaseFileSystem::BeginMapAccess()` | differs | differs | size retail 450 vs ours 338 |
| `CBaseFileSystem::CBaseFileSystem()` | differs | differs | size retail 1930 vs ours 2032 |
| `CBaseFileSystem::CFileCacheObject::AddFiles(char const**, int)` | differs | differs | size retail 740 vs ours 756 |
| `CBaseFileSystem::CFileCacheObject::IOCallback(FileAsyncRequest_t const&, int, FSAsyncStatus_t)` | masked | masked |  |
| `CBaseFileSystem::CFileCacheObject::~CFileCacheObject()` | masked | masked |  |
| `CBaseFileSystem::CSearchPath::~CSearchPath()` | masked | masked |  |
| `CBaseFileSystem::CSearchPathsIterator::CSearchPathsIterator(CBaseFileSystem*, char const**, char const*, PathTypeFilter_t)` | masked | masked |  |
| `CBaseFileSystem::CSearchPathsIterator::CSearchPathsIterator(CBaseFileSystem*, char const*, PathTypeFilter_t)` | masked | masked |  |
| `CBaseFileSystem::CSearchPathsIterator::CopySearchPaths(CUtlVector<CBaseFileSystem::CSearchPath, CUtlMemory<CBaseFileSystem::CSearchPath, int>> const&)` | differs | differs | size retail 288 vs ours 272 |
| `CBaseFileSystem::CSearchPathsIterator::GetNext()` | differs | differs | size retail 351 vs ours 281 |
| `CBaseFileSystem::CSearchPathsVisits::MarkVisit(CBaseFileSystem::CSearchPath const&)` | masked | masked |  |
| `CBaseFileSystem::CacheAllVPKFileHashes(bool, bool)` | differs | masked | size retail 1177 vs ours 1161 |
| `CBaseFileSystem::CacheFileCRCs(char const*, ECacheCRCType, IFileList*)` | exact | exact |  |
| `CBaseFileSystem::CheckCachedFileHash(char const*, char const*, int, FileHash_t*)` | masked | masked |  |
| `CBaseFileSystem::CheckVPKFileHash(int, int, int, MD5Value_t&)` | differs | masked | size retail 452 vs ours 454 |
| `CBaseFileSystem::Close(void*)` | masked | masked |  |
| `CBaseFileSystem::CreateDirHierarchy(char const*, char const*)` | differs | differs | size retail 487 vs ours 519 |
| `CBaseFileSystem::CreateFileCache()` | masked | masked |  |
| `CBaseFileSystem::DestroyFileCache(void*)` | masked | masked |  |
| `CBaseFileSystem::DiscardPreloadData()` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CBaseFileSystem::EnableWhitelistFileTracking(bool, bool, bool)` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CBaseFileSystem::EndMapAccess()` | differs | differs | size retail 354 vs ours 290 |
| `CBaseFileSystem::EndOfFile(void*)` | differs | differs | size retail 190 vs ours 68 |
| `CBaseFileSystem::ExtractRootKeyName(IFileSystem::KeyValuesPreloadType_t, char*, unsigned long, char const*, char const*)` | masked | differs |  |
| `CBaseFileSystem::FPrintf(void*, char const*, ...)` | masked | masked |  |
| `CBaseFileSystem::FS_GetSectorSize(__sFILE*)` | exact | exact |  |
| `CBaseFileSystem::FS_setmode(__sFILE*, FileMode_t)` | exact | exact |  |
| `CBaseFileSystem::FastFileTime(CBaseFileSystem::CSearchPath const*, char const*)` | differs | differs | size retail 341 vs ours 309 |
| `CBaseFileSystem::FileExists(char const*, char const*)` | masked | masked |  |
| `CBaseFileSystem::FileTimeToString(char*, int, long)` | masked | masked |  |
| `CBaseFileSystem::FindClose(int)` | differs | masked | reloc target at +0xfd: ours _ZdlPv, retail _ZdaPv |
| `CBaseFileSystem::FindData_t::~FindData_t()` | differs | masked | size retail 639 vs ours 721 |
| `CBaseFileSystem::FindFileInSearchPath(CFileOpenInfo&)` | differs | masked | size retail 1025 vs ours 1012 |
| `CBaseFileSystem::FindFileInVPKs(char const*, char const*)` | missing | missing |  |
| `CBaseFileSystem::FindFileName(char const*)` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CBaseFileSystem::FindFirst(char const*, int*)` | exact | exact |  |
| `CBaseFileSystem::FindFirstEx(char const*, char const*, int*)` | exact | exact |  |
| `CBaseFileSystem::FindFirstHelper(char const*, char const*, int*, int*)` | differs | differs | size retail 2830 vs ours 2884 |
| `CBaseFileSystem::FindIsDirectory(int)` | exact | exact |  |
| `CBaseFileSystem::FindNext(int)` | masked | masked |  |
| `CBaseFileSystem::FindNextFileHelper(CBaseFileSystem::FindData_t*, int*)` | differs | differs | size retail 806 vs ours 998 |
| `CBaseFileSystem::FindNextFileInVPKHelper(CBaseFileSystem::FindData_t*)` | missing | missing |  |
| `CBaseFileSystem::FindOrAddFileName(char const*)` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CBaseFileSystem::FindOrAddPathIDInfo(CUtlSymbol const&, int)` | masked | masked |  |
| `CBaseFileSystem::FindSearchPathByStoreId(int)` | exact | exact |  |
| `CBaseFileSystem::FindWritePath(char const*, char const*)` | masked | masked |  |
| `CBaseFileSystem::FixUpPath(char const*, char*, int)` | masked | masked |  |
| `CBaseFileSystem::Flush(void*)` | masked | masked |  |
| `CBaseFileSystem::FreeOptimalReadBuffer(void*)` | masked | masked |  |
| `CBaseFileSystem::FullPathToRelativePath(char const*, char*, int)` | exact | exact |  |
| `CBaseFileSystem::FullPathToRelativePathEx(char const*, char const*, char*, int)` | differs | differs | size retail 696 vs ours 647 |
| `CBaseFileSystem::GetCurrentDirectory(char*, int)` | masked | masked |  |
| `CBaseFileSystem::GetCurrentGameExpansionId(char*, int)` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CBaseFileSystem::GetDVDMode()` | masked | masked |  |
| `CBaseFileSystem::GetFileTime(char const*, char const*)` | masked | masked |  |
| `CBaseFileSystem::GetFileTypeForFullPath(char const*, wchar_t*, unsigned long)` | masked | masked |  |
| `CBaseFileSystem::GetFilesystemStatistics()` | exact | exact |  |
| `CBaseFileSystem::GetFontTempFile(char const*, char*, int)` | masked | masked |  |
| `CBaseFileSystem::GetLocalCopy(char const*)` | exact | exact |  |
| `CBaseFileSystem::GetLocalPath(char const*, char*, int)` | exact | exact |  |
| `CBaseFileSystem::GetOptimalIOConstraints(void*, unsigned int*, unsigned int*, unsigned int*)` | exact | exact |  |
| `CBaseFileSystem::GetPathIndex(void* const&)` | differs | exact | size retail 11 vs ours 13 |
| `CBaseFileSystem::GetPathTime(char const*, char const*)` | masked | masked |  |
| `CBaseFileSystem::GetSearchPath(char const*, bool, char*, int)` | masked | masked |  |
| `CBaseFileSystem::GetSearchPaths2(char const*, bool, CUtlVector<CUtlString, CUtlMemory<CUtlString, int>>&)` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CBaseFileSystem::GetUnverifiedFileHashes(CUnverifiedFileHash*, int)` | masked | masked |  |
| `CBaseFileSystem::GetWhitelistSpewFlags()` | exact | exact |  |
| `CBaseFileSystem::GetWritePath(char const*, char const*)` | masked | masked |  |
| `CBaseFileSystem::HandleOpenRegularFile(CFileOpenInfo&, bool)` | masked | masked |  |
| `CBaseFileSystem::Init()` | differs | differs | size retail 612 vs ours 504 |
| `CBaseFileSystem::InstallDirtyDiskReportFunc(void (*)())` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CBaseFileSystem::IsDirectory(char const*, char const*)` | differs | masked | size retail 3276 vs ours 2540 |
| `CBaseFileSystem::IsFileCacheFileLoaded(void*, char const*)` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CBaseFileSystem::IsFileCacheLoaded(void*)` | exact | exact |  |
| `CBaseFileSystem::IsFileWritable(char const*, char const*)` | masked | masked |  |
| `CBaseFileSystem::IsOk(void*)` | masked | masked |  |
| `CBaseFileSystem::LoadCompiledKeyValues(IFileSystem::KeyValuesPreloadType_t, char const*)` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CBaseFileSystem::LoadKeyValues(IFileSystem::KeyValuesPreloadType_t, char const*, char const*)` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CBaseFileSystem::LoadKeyValues(KeyValues&, IFileSystem::KeyValuesPreloadType_t, char const*, char const*)` | masked | differs |  |
| `CBaseFileSystem::LoadModule(char const*, char const*, bool)` | differs | differs | size retail 739 vs ours 623 |
| `CBaseFileSystem::LogAccessToFile(char const*, char const*, char const*)` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CBaseFileSystem::LogFileAccess(char const*)` | masked | masked |  |
| `CBaseFileSystem::LookupKeyValuesRootKeyName(char const*, char const*, char*, unsigned long)` | masked | masked |  |
| `CBaseFileSystem::MarkAllCRCsUnverified()` | masked | masked |  |
| `CBaseFileSystem::MarkPathIDByRequestOnly(char const*, bool)` | masked | masked |  |
| `CBaseFileSystem::NotifyFileUnloaded(char const*, char const*)` | masked | masked |  |
| `CBaseFileSystem::Open(char const*, char const*, char const*)` | exact | exact |  |
| `CBaseFileSystem::OpenEx(char const*, char const*, unsigned int, char const*, char**)` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CBaseFileSystem::OpenForRead(char const*, char const*, unsigned int, char const*, char**)` | differs | differs | size retail 2801 vs ours 2881 |
| `CBaseFileSystem::OpenForWrite(char const*, char const*, char const*)` | differs | differs | size retail 475 vs ours 558 |
| `CBaseFileSystem::PopNoFinishAllOnSearchPaths()` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CBaseFileSystem::Precache(char const*, char const*)` | masked | masked |  |
| `CBaseFileSystem::PrintOpenedFiles()` | exact | exact |  |
| `CBaseFileSystem::PrintSearchPaths()` | differs | differs | size retail 330 vs ours 319 |
| `CBaseFileSystem::PushNoFinishAllOnSearchPaths()` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CBaseFileSystem::QueryInterface(char const*)` | masked | masked |  |
| `CBaseFileSystem::Read(void*, int, void*)` | exact | exact |  |
| `CBaseFileSystem::ReadEx(void*, int, int, void*)` | masked | masked |  |
| `CBaseFileSystem::ReadFile(char const*, char const*, CUtlBuffer&, int, int, void* (*)(char const*, unsigned int))` | masked | masked |  |
| `CBaseFileSystem::ReadFileEx(char const*, char const*, void**, bool, bool, int, int, void* (*)(char const*, unsigned int))` | masked | masked |  |
| `CBaseFileSystem::ReadLine(char*, int, void*)` | masked | masked |  |
| `CBaseFileSystem::ReadToBuffer(void*, CUtlBuffer&, int, void* (*)(char const*, unsigned int))` | masked | masked |  |
| `CBaseFileSystem::RegisterFileWhitelist(IPureServerWhitelist*, IFileList**)` | masked | masked |  |
| `CBaseFileSystem::RegisterMemoryFile(CMemoryFileBacking*, CMemoryFileBacking**)` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CBaseFileSystem::RelativePathToFullPath(char const*, char const*, char*, int, PathTypeFilter_t, unsigned int*)` | differs | differs | size retail 1702 vs ours 1670 |
| `CBaseFileSystem::RemoveAllSearchPaths()` | masked | masked |  |
| `CBaseFileSystem::RemoveFile(char const*, char const*)` | masked | masked |  |
| `CBaseFileSystem::RemoveLoggingFunc(void (*)(char const*, char const*))` | masked | masked |  |
| `CBaseFileSystem::RemoveSearchPath(char const*, char const*)` | differs | differs | size retail 785 vs ours 780 |
| `CBaseFileSystem::RemoveSearchPaths(char const*)` | differs | differs | size retail 223 vs ours 226 |
| `CBaseFileSystem::RenameFile(char const*, char const*, char const*)` | masked | masked |  |
| `CBaseFileSystem::Seek(void*, int, FileSystemSeek_t)` | masked | masked |  |
| `CBaseFileSystem::SetAudioLanguageOverride(char const*)` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CBaseFileSystem::SetBufferSize(void*, unsigned int)` | differs | differs | size retail 124 vs ours 110 |
| `CBaseFileSystem::SetCurrentGameExpansionId(char const*)` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CBaseFileSystem::SetFileWritable(char const*, bool, char const*)` | masked | masked |  |
| `CBaseFileSystem::SetSearchPathIsTrustedSource(CBaseFileSystem::CSearchPath*)` | differs | masked | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CBaseFileSystem::SetUseHDMount(bool)` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CBaseFileSystem::SetUseLowViolancePaths(bool)` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CBaseFileSystem::SetWarningFunc(void (*)(char const*, ...))` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CBaseFileSystem::SetWarningLevel(FileWarningLevel_t)` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CBaseFileSystem::SetWhitelistSpewFlags(int)` | exact | exact |  |
| `CBaseFileSystem::SetupPreloadData()` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CBaseFileSystem::Shutdown()` | masked | masked |  |
| `CBaseFileSystem::Size(char const*, char const*)` | masked | masked |  |
| `CBaseFileSystem::Size(void*)` | differs | differs | size retail 450 vs ours 459 |
| `CBaseFileSystem::String(void* const&, char*, int)` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CBaseFileSystem::Tell(void*)` | differs | differs | size retail 474 vs ours 483 |
| `CBaseFileSystem::Trace_FClose(__sFILE*)` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CBaseFileSystem::Trace_FOpen(char const*, char const*, unsigned int, long long*)` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CBaseFileSystem::Trace_FRead(int, __sFILE*)` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CBaseFileSystem::Trace_FWrite(int, __sFILE*)` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CBaseFileSystem::UnloadCompiledKeyValues()` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CBaseFileSystem::UnloadModule(CSysModule*)` | masked | masked |  |
| `CBaseFileSystem::UnregisterMemoryFile(CMemoryFileBacking*)` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CBaseFileSystem::UnzipFile(char const*, char const*, char const*)` | masked | masked |  |
| `CBaseFileSystem::UpdateSearchPathsForCurrentGameExpansion()` | differs | differs | size retail 2144 vs ours 2154 |
| `CBaseFileSystem::Warning(FileWarningLevel_t, char const*, ...)` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CBaseFileSystem::Write(void const*, int, void*)` | masked | masked |  |
| `CBaseFileSystem::WriteFile(char const*, char const*, CUtlBuffer&)` | masked | masked |  |
| `CBaseFileSystem::~CBaseFileSystem()` | differs | differs | size retail 2413 vs ours 2427 |
| `CFileHandle::GetSectorSize()` | differs | differs | size retail 118 vs ours 80 |
| `CFileHandle::Read(void*, int, int)` | differs | differs | size retail 296 vs ours 216 |
| `CFileHandle::Seek(long long, int)` | differs | differs | size retail 544 vs ours 498 |
| `CFileHandle::Size()` | differs | differs | size retail 56 vs ours 67 |
| `CFileHandle::~CFileHandle()` | missing | differs |  |
| `CFileHandle::~CFileHandle()` | differs | differs | size retail 9 vs ours 111 |
| `CFileHandle::~CFileHandle()` | differs | differs | size retail 26 vs ours 101 |
| `CFileOpenInfo::HandleFileCRCTracking(char const*)` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CFileOpenInfo::SetAbsolutePath(char const*, ...)` | masked | masked |  |
| `CMemoryFileBacking::~CMemoryFileBacking()` | masked | masked |  |
| `CMemoryFileBacking::~CMemoryFileBacking()` | masked | masked |  |
| `CMemoryFileHandle::~CMemoryFileHandle()` | exact | masked |  |
| `CMemoryFileHandle::~CMemoryFileHandle()` | masked | masked |  |
| `CMemoryFileHandle::~CMemoryFileHandle()` | differs | differs | size retail 98 vs ours 266 |
| `CPackFile::DiscardPreloadData()` | missing | missing | retail keeps pack-file code in basefilesystem.cpp (older layout than packfile.cpp) |
| `CPackFile::OpenFile(char const*, char const*)` | missing | missing | retail keeps pack-file code in basefilesystem.cpp (older layout than packfile.cpp) |
| `CPackFile::ReadFromPack(int, void*, int, int, long long)` | missing | missing | retail keeps pack-file code in basefilesystem.cpp (older layout than packfile.cpp) |
| `CPackFile::SetupPreloadData()` | missing | missing | retail keeps pack-file code in basefilesystem.cpp (older layout than packfile.cpp) |
| `CPackFile::~CPackFile()` | missing | missing | retail keeps pack-file code in basefilesystem.cpp (older layout than packfile.cpp) |
| `CPackFile::~CPackFile()` | missing | missing | retail keeps pack-file code in basefilesystem.cpp (older layout than packfile.cpp) |
| `CPackedStoreRefCount::~CPackedStoreRefCount()` | masked | masked |  |
| `CPackedStoreRefCount::~CPackedStoreRefCount()` | masked | masked |  |
| `CRefCountServiceBase<true, CRefMT>::OnFinalRelease()` | exact | exact |  |
| `CThreadLocal<char*>::~CThreadLocal()` | masked | masked |  |
| `CTier1AppSystem<IFileSystem, 0>::Connect(void* (*)(char const*, int*))` | masked | masked |  |
| `CTier1AppSystem<IFileSystem, 0>::Disconnect()` | masked | masked |  |
| `CUtlDict<int, int>::Purge()` | masked | masked |  |
| `CUtlHashtable<char const*, CMemoryFileBacking*, DefaultHashFunctor<char const*>, DefaultEqualFunctor<char const*>, undefined_t>::BumpEntry(unsigned in` | exact | exact |  |
| `CUtlHashtable<char const*, CMemoryFileBacking*, DefaultHashFunctor<char const*>, DefaultEqualFunctor<char const*>, undefined_t>::DoInsertUnconstructed` | exact | masked |  |
| `CUtlHashtable<char const*, CMemoryFileBacking*, DefaultHashFunctor<char const*>, DefaultEqualFunctor<char const*>, undefined_t>::DoRealloc(int)` | masked | masked |  |
| `CUtlHashtable<char const*, CMemoryFileBacking*, DefaultHashFunctor<char const*>, DefaultEqualFunctor<char const*>, undefined_t>::Find(char const*) con` | masked | masked |  |
| `CUtlLinkedList<CBaseFileSystem::FindData_t, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<CBaseFileSystem::FindData_t, unsigne` | masked | masked |  |
| `CUtlLinkedList<CBaseFileSystem::FindData_t, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<CBaseFileSystem::FindData_t, unsigne` | exact | exact |  |
| `CUtlLinkedList<CBaseFileSystem::FindData_t, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<CBaseFileSystem::FindData_t, unsigne` | masked | masked |  |
| `CUtlMemory<CBaseFileSystem::CFileCacheObject::Info_t*, int>::Grow(int)` | masked | masked |  |
| `CUtlMemory<CBaseFileSystem::COpenedFile, int>::Grow(int)` | masked | masked |  |
| `CUtlMemory<CBaseFileSystem::CPathIDInfo*, int>::Grow(int)` | masked | masked |  |
| `CUtlMemory<CBaseFileSystem::CSearchPath, int>::Grow(int)` | masked | masked |  |
| `CUtlMemory<CPackFile*, int>::Grow(int)` | masked | masked |  |
| `CUtlMemory<CUtlString, int>::Grow(int)` | masked | masked |  |
| `CUtlMemory<CZipPackFile::CPackFileEntry, int>::Grow(int)` | missing | missing | retail keeps pack-file code in basefilesystem.cpp (older layout than packfile.cpp) |
| `CUtlMemory<UtlLinkedListElem_t<CBaseFileSystem::FindData_t, unsigned short>, unsigned short>::Grow(int)` | masked | masked |  |
| `CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, int, int>::Node_t, int>, int>::Grow(int)` | masked | masked |  |
| `CUtlMemory<char*, int>::Grow(int)` | masked | masked |  |
| `CUtlMemory<char, int>::Grow(int)` | masked | masked |  |
| `CUtlMemory<int, int>::Grow(int)` | masked | masked |  |
| `CUtlMemory<void (*)(char const*, char const*), int>::Grow(int)` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, int, int>::Node_t, int, CUtlMap<char const*, int, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, int,` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, int, int>::Node_t, int, CUtlMap<char const*, int, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, int,` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, int, int>::Node_t, int, CUtlMap<char const*, int, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, int,` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, int, int>::Node_t, int, CUtlMap<char const*, int, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, int,` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, int, int>::Node_t, int, CUtlMap<char const*, int, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, int,` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, int, int>::Node_t, int, CUtlMap<char const*, int, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, int,` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, int, int>::Node_t, int, CUtlMap<char const*, int, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, int,` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, int, int>::Node_t, int, CUtlMap<char const*, int, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, int,` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, int, int>::Node_t, int, CUtlMap<char const*, int, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, int,` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, int, int>::Node_t, int, CUtlMap<char const*, int, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, int,` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, int, int>::Node_t, int, CUtlMap<char const*, int, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, int,` | masked | masked |  |
| `CUtlSortVector<CZipPackFile::CPackFileEntry, CZipPackFile::CPackFileLessFunc, CUtlVector<CZipPackFile::CPackFileEntry, CUtlMemory<CZipPackFile::CPackF` | missing | missing | retail keeps pack-file code in basefilesystem.cpp (older layout than packfile.cpp) |
| `CUtlStringList::CopyAndAddToTail(char const*)` | masked | masked |  |
| `CUtlSymbolTableMT::Find(char const*) const` | missing | missing |  |
| `CUtlSymbolTableMT::String(CUtlSymbol) const` | masked | masked |  |
| `CUtlSymbolTableMT::~CUtlSymbolTableMT()` | masked | masked |  |
| `CUtlVector<CBaseFileSystem::COpenedFile, CUtlMemory<CBaseFileSystem::COpenedFile, int>>::InsertBefore(int, CBaseFileSystem::COpenedFile const&)` | masked | masked |  |
| `CUtlVector<CBaseFileSystem::CSearchPath, CUtlMemory<CBaseFileSystem::CSearchPath, int>>::InsertBefore(int)` | masked | masked |  |
| `CUtlVector<CBaseFileSystem::CSearchPath, CUtlMemory<CBaseFileSystem::CSearchPath, int>>::InsertMultipleBefore(int, int, CBaseFileSystem::CSearchPath c` | missing | missing |  |
| `CUtlVector<void*, CUtlMemory<void*, int>>::~CUtlVector()` | masked | masked |  |
| `CUtlVectorAutoPurge<char*>::~CUtlVectorAutoPurge()` | missing | masked |  |
| `CZipPackFile::DiscardPreloadData()` | missing | missing | retail keeps pack-file code in basefilesystem.cpp (older layout than packfile.cpp) |
| `CZipPackFile::FindFile(char const*, int&, long long&, int&)` | missing | missing | retail keeps pack-file code in basefilesystem.cpp (older layout than packfile.cpp) |
| `CZipPackFile::GetPackFileBaseOffset()` | missing | missing | retail keeps pack-file code in basefilesystem.cpp (older layout than packfile.cpp) |
| `CZipPackFile::IndexToFilename(int, char*, int)` | missing | missing | retail keeps pack-file code in basefilesystem.cpp (older layout than packfile.cpp) |
| `CZipPackFile::Prepare(long long, long long)` | missing | missing | retail keeps pack-file code in basefilesystem.cpp (older layout than packfile.cpp) |
| `CZipPackFile::ReadFromPack(int, void*, int, int, long long)` | missing | missing | retail keeps pack-file code in basefilesystem.cpp (older layout than packfile.cpp) |
| `CZipPackFile::SetupPreloadData()` | missing | missing | retail keeps pack-file code in basefilesystem.cpp (older layout than packfile.cpp) |
| `CZipPackFile::~CZipPackFile()` | missing | missing | retail keeps pack-file code in basefilesystem.cpp (older layout than packfile.cpp) |
| `CZipPackFile::~CZipPackFile()` | missing | missing | retail keeps pack-file code in basefilesystem.cpp (older layout than packfile.cpp) |
| `CZipPackFile::~CZipPackFile()` | missing | missing | retail keeps pack-file code in basefilesystem.cpp (older layout than packfile.cpp) |
| `CaselessStringLessThan(char const* const&, char const* const&)` | masked | masked |  |
| `FileSystem_AddLoadedSearchPath(IFileSystem*, char const*, char const*, char const*, bool, bool)` | masked | masked |  |
| `LogFileOpen(char const*, char const*, char const*)` | masked | masked |  |
| `SortStricmp(char* const*, char* const*)` | masked | masked |  |
| `__GLOBAL__I_a` | differs | differs | size retail 823 vs ours 735 |
| `int CUtlHashtable<char const*, CMemoryFileBacking*, DefaultHashFunctor<char const*>, DefaultEqualFunctor<char const*>, undefined_t>::DoRemove<char con` | masked | masked |  |
| `int CUtlSortVector<CZipPackFile::CPackFileEntry, CZipPackFile::CPackFileLessFunc, CUtlVector<CZipPackFile::CPackFileEntry, CUtlMemory<CZipPackFile::CP` | missing | missing | retail keeps pack-file code in basefilesystem.cpp (older layout than packfile.cpp) |
| `non-virtual thunk to CBaseFileSystem::Close(void*)` | exact | exact |  |
| `non-virtual thunk to CBaseFileSystem::FileExists(char const*, char const*)` | exact | exact |  |
| `non-virtual thunk to CBaseFileSystem::Flush(void*)` | exact | masked |  |
| `non-virtual thunk to CBaseFileSystem::GetFileTime(char const*, char const*)` | exact | exact |  |
| `non-virtual thunk to CBaseFileSystem::IsFileWritable(char const*, char const*)` | exact | exact |  |
| `non-virtual thunk to CBaseFileSystem::Open(char const*, char const*, char const*)` | exact | exact |  |
| `non-virtual thunk to CBaseFileSystem::Precache(char const*, char const*)` | exact | exact |  |
| `non-virtual thunk to CBaseFileSystem::Read(void*, int, void*)` | exact | exact |  |
| `non-virtual thunk to CBaseFileSystem::ReadFile(char const*, char const*, CUtlBuffer&, int, int, void* (*)(char const*, unsigned int))` | masked | masked |  |
| `non-virtual thunk to CBaseFileSystem::Seek(void*, int, FileSystemSeek_t)` | masked | masked |  |
| `non-virtual thunk to CBaseFileSystem::SetFileWritable(char const*, bool, char const*)` | exact | exact |  |
| `non-virtual thunk to CBaseFileSystem::Size(char const*, char const*)` | exact | exact |  |
| `non-virtual thunk to CBaseFileSystem::Size(void*)` | masked | masked |  |
| `non-virtual thunk to CBaseFileSystem::Tell(void*)` | masked | masked |  |
| `non-virtual thunk to CBaseFileSystem::UnzipFile(char const*, char const*, char const*)` | exact | exact |  |
| `non-virtual thunk to CBaseFileSystem::Write(void const*, int, void*)` | exact | masked |  |
| `non-virtual thunk to CBaseFileSystem::WriteFile(char const*, char const*, CUtlBuffer&)` | masked | masked |  |
| `unsigned int CUtlHashtable<char const*, CMemoryFileBacking*, DefaultHashFunctor<char const*>, DefaultEqualFunctor<char const*>, undefined_t>::DoInsert` | masked | masked |  |

## filesystem_async (98/107)

| function | status | with needed | note |
|---|---|---|---|
| `AsyncOpenedFile_t::~AsyncOpenedFile_t()` | exact | exact |  |
| `AsyncOpenedFile_t::~AsyncOpenedFile_t()` | masked | masked |  |
| `AsyncWriteJobFilter(CJob*)` | masked | masked |  |
| `CAsyncOpenedFiles::AddRef(FSAsyncFile_t__*)` | masked | masked |  |
| `CAsyncOpenedFiles::FindOrAdd(char const*)` | masked | masked |  |
| `CAsyncOpenedFiles::Get(FSAsyncFile_t__*)` | masked | masked |  |
| `CAsyncOpenedFiles::Release(FSAsyncFile_t__*)` | masked | masked |  |
| `CAsyncOpenedFiles::~CAsyncOpenedFiles()` | masked | masked |  |
| `CBaseFileSystem::AsyncAbort(FSAsyncControl_t__*)` | masked | masked |  |
| `CBaseFileSystem::AsyncAddFetcher(IAsyncFileFetch*)` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CBaseFileSystem::AsyncAddRef(FSAsyncControl_t__*)` | exact | exact |  |
| `CBaseFileSystem::AsyncAppendFile(char const*, char const*, FSAsyncControl_t__**)` | masked | masked |  |
| `CBaseFileSystem::AsyncBeginRead(char const*, FSAsyncFile_t__**)` | masked | masked |  |
| `CBaseFileSystem::AsyncEndRead(FSAsyncFile_t__*)` | masked | masked |  |
| `CBaseFileSystem::AsyncFinish(FSAsyncControl_t__*, bool)` | masked | masked |  |
| `CBaseFileSystem::AsyncFinishAll(int)` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CBaseFileSystem::AsyncFinishAllWrites()` | masked | masked |  |
| `CBaseFileSystem::AsyncFlush()` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CBaseFileSystem::AsyncGetResult(FSAsyncControl_t__*, void**, int*)` | exact | exact |  |
| `CBaseFileSystem::AsyncReadMultiple(FileAsyncRequest_t const*, int, FSAsyncControl_t__**)` | exact | exact |  |
| `CBaseFileSystem::AsyncReadMultipleCreditAlloc(FileAsyncRequest_t const*, int, char const*, int, FSAsyncControl_t__**)` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CBaseFileSystem::AsyncRelease(FSAsyncControl_t__*)` | exact | exact |  |
| `CBaseFileSystem::AsyncRemoveFetcher(IAsyncFileFetch*)` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CBaseFileSystem::AsyncResume()` | exact | exact |  |
| `CBaseFileSystem::AsyncSetPriority(FSAsyncControl_t__*, int)` | exact | exact |  |
| `CBaseFileSystem::AsyncStatus(FSAsyncControl_t__*)` | exact | exact |  |
| `CBaseFileSystem::AsyncSuspend()` | exact | exact |  |
| `CBaseFileSystem::AsyncWrite(char const*, void const*, int, bool, bool, FSAsyncControl_t__**)` | masked | masked |  |
| `CBaseFileSystem::AsyncWriteFile(char const*, CUtlBuffer const*, int, bool, bool, FSAsyncControl_t__**)` | masked | masked |  |
| `CBaseFileSystem::DoAsyncCallback(FileAsyncRequest_t const&, void*, int, FSAsyncStatus_t)` | masked | masked |  |
| `CBaseFileSystem::InitAsync()` | differs | differs | size retail 219 vs ours 229 |
| `CBaseFileSystem::ShutdownAsync()` | exact | exact |  |
| `CBaseFileSystem::SyncAppendFile(char const*, char const*)` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CBaseFileSystem::SyncRead(FileAsyncRequest_t const&)` | masked | differs |  |
| `CFileAsyncAppendFileJob::CFileAsyncAppendFileJob(char const*, char const*)` | masked | masked |  |
| `CFileAsyncAppendFileJob::Describe()` | exact | exact |  |
| `CFileAsyncAppendFileJob::DoExecute()` | masked | masked |  |
| `CFileAsyncAppendFileJob::IsWrite() const` | exact | exact |  |
| `CFileAsyncAppendFileJob::~CFileAsyncAppendFileJob()` | masked | masked |  |
| `CFileAsyncAppendFileJob::~CFileAsyncAppendFileJob()` | masked | masked |  |
| `CFileAsyncFileSizeJob::DoExecute()` | masked | masked |  |
| `CFileAsyncFileSizeJob::~CFileAsyncFileSizeJob()` | exact | masked |  |
| `CFileAsyncFileSizeJob::~CFileAsyncFileSizeJob()` | masked | masked |  |
| `CFileAsyncJob::GetResult(void**, int*)` | exact | exact |  |
| `CFileAsyncJob::IsWrite() const` | exact | exact |  |
| `CFileAsyncJob::~CFileAsyncJob()` | masked | masked |  |
| `CFileAsyncJob::~CFileAsyncJob()` | masked | masked |  |
| `CFileAsyncReadJob::CFileAsyncReadJob(FileAsyncRequest_t const&, CBaseFileSystem*)` | masked | masked |  |
| `CFileAsyncReadJob::Describe()` | exact | exact |  |
| `CFileAsyncReadJob::DoExecute()` | masked | masked |  |
| `CFileAsyncReadJob::GetResult(void**, int*)` | exact | exact |  |
| `CFileAsyncReadJob::InterceptCallback(FileAsyncRequest_t const&, int, FSAsyncStatus_t)` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CFileAsyncReadJob::~CFileAsyncReadJob()` | masked | masked |  |
| `CFileAsyncReadJob::~CFileAsyncReadJob()` | exact | masked |  |
| `CFileAsyncReadJob::~CFileAsyncReadJob()` | masked | masked |  |
| `CFileAsyncWriteFileJob::DoCleanup()` | masked | masked |  |
| `CFileAsyncWriteFileJob::~CFileAsyncWriteFileJob()` | masked | masked |  |
| `CFileAsyncWriteFileJob::~CFileAsyncWriteFileJob()` | masked | masked |  |
| `CFileAsyncWriteJob::Describe()` | exact | exact |  |
| `CFileAsyncWriteJob::DoCleanup()` | masked | masked |  |
| `CFileAsyncWriteJob::DoExecute()` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CFileAsyncWriteJob::IsWrite() const` | exact | exact |  |
| `CFileAsyncWriteJob::~CFileAsyncWriteJob()` | masked | masked |  |
| `CFileAsyncWriteJob::~CFileAsyncWriteJob()` | masked | masked |  |
| `CJob::CJob(JobPriority_t)` | masked | masked |  |
| `CJob::Describe()` | masked | masked |  |
| `CJob::DoAbort(bool)` | exact | exact |  |
| `CJob::DoCleanup()` | exact | exact |  |
| `CJob::SetDescription(char const*)` | masked | masked |  |
| `CJob::~CJob()` | masked | masked |  |
| `CJob::~CJob()` | masked | masked |  |
| `CRefCountServiceBase<false, CRefMT>::OnFinalRelease()` | exact | exact |  |
| `CRefCounted1<IRefCounted, CRefCountServiceBase<true, CRefMT>>::AddRef()` | masked | masked |  |
| `CRefCounted1<IRefCounted, CRefCountServiceBase<true, CRefMT>>::Release()` | masked | masked |  |
| `CThreadMutex::~CThreadMutex()` | masked | masked |  |
| `CUtlMemory<CFileAsyncReadJob*, int>::Grow(int)` | masked | masked |  |
| `CUtlMemory<IAsyncFileFetch*, int>::Grow(int)` | masked | masked |  |
| `CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>::Node_t, unsigned short>, unsigned short>::Grow(int)` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>::Node_t, unsigned short, CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>:` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>::Node_t, unsigned short, CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>:` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>::Node_t, unsigned short, CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>:` | exact | masked |  |
| `CUtlRBTree<CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>::Node_t, unsigned short, CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>:` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>::Node_t, unsigned short, CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>:` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>::Node_t, unsigned short, CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>:` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>::Node_t, unsigned short, CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>:` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>::Node_t, unsigned short, CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>:` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>::Node_t, unsigned short, CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>:` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>::Node_t, unsigned short, CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>:` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>::Node_t, unsigned short, CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>:` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>::Node_t, unsigned short, CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>:` | masked | masked |  |
| `__GLOBAL__I_a` | masked | masked |  |
| `async_resume(CCommand const&)` | masked | masked |  |
| `async_suspend(CCommand const&)` | masked | masked |  |
| `non-virtual thunk to CFileAsyncAppendFileJob::~CFileAsyncAppendFileJob()` | masked | masked |  |
| `non-virtual thunk to CFileAsyncAppendFileJob::~CFileAsyncAppendFileJob()` | masked | masked |  |
| `non-virtual thunk to CFileAsyncFileSizeJob::~CFileAsyncFileSizeJob()` | exact | masked |  |
| `non-virtual thunk to CFileAsyncFileSizeJob::~CFileAsyncFileSizeJob()` | masked | masked |  |
| `non-virtual thunk to CFileAsyncJob::~CFileAsyncJob()` | masked | masked |  |
| `non-virtual thunk to CFileAsyncJob::~CFileAsyncJob()` | masked | masked |  |
| `non-virtual thunk to CFileAsyncReadJob::~CFileAsyncReadJob()` | exact | masked |  |
| `non-virtual thunk to CFileAsyncReadJob::~CFileAsyncReadJob()` | masked | masked |  |
| `non-virtual thunk to CFileAsyncWriteFileJob::~CFileAsyncWriteFileJob()` | masked | masked |  |
| `non-virtual thunk to CFileAsyncWriteFileJob::~CFileAsyncWriteFileJob()` | masked | masked |  |
| `non-virtual thunk to CFileAsyncWriteJob::~CFileAsyncWriteJob()` | masked | masked |  |
| `non-virtual thunk to CFileAsyncWriteJob::~CFileAsyncWriteJob()` | masked | masked |  |
| `non-virtual thunk to CJob::~CJob()` | masked | masked |  |
| `non-virtual thunk to CJob::~CJob()` | masked | masked |  |

## filesystem_stdio (43/65)

| function | status | with needed | note |
|---|---|---|---|
| `CDefOps<int>::LessFunc(int const&, int const&)` | missing |  |  |
| `CFileSystem_Stdio::AllocOptimalReadBuffer(void*, unsigned int, unsigned int)` | differs | differs | size retail 122 vs ours 121 |
| `CFileSystem_Stdio::CancelWaitForResources(int)` | exact | exact |  |
| `CFileSystem_Stdio::FS_FindClose(void*)` | masked | masked |  |
| `CFileSystem_Stdio::FS_FindFirstFile(char const*, FIND_DATA*)` | masked | masked |  |
| `CFileSystem_Stdio::FS_FindNextFile(void*, FIND_DATA*)` | masked | masked |  |
| `CFileSystem_Stdio::FS_GetSectorSize(__sFILE*)` | exact | exact |  |
| `CFileSystem_Stdio::FS_chmod(char const*, int)` | masked | masked |  |
| `CFileSystem_Stdio::FS_fclose(__sFILE*)` | differs | differs | size retail 65 vs ours 34 |
| `CFileSystem_Stdio::FS_feof(__sFILE*)` | exact | exact |  |
| `CFileSystem_Stdio::FS_ferror(__sFILE*)` | exact | exact |  |
| `CFileSystem_Stdio::FS_fflush(__sFILE*)` | exact | exact |  |
| `CFileSystem_Stdio::FS_fgets(char*, int, __sFILE*)` | exact | exact |  |
| `CFileSystem_Stdio::FS_fopen(char const*, char const*, unsigned int, long long*)` | masked | masked |  |
| `CFileSystem_Stdio::FS_fread(void*, unsigned long, unsigned long, __sFILE*)` | differs | differs | size retail 124 vs ours 77 |
| `CFileSystem_Stdio::FS_fseek(__sFILE*, long long, int)` | differs | differs | size retail 105 vs ours 47 |
| `CFileSystem_Stdio::FS_ftell(__sFILE*)` | exact | exact |  |
| `CFileSystem_Stdio::FS_fwrite(void const*, unsigned long, __sFILE*)` | masked | masked |  |
| `CFileSystem_Stdio::FS_setbufsize(__sFILE*, unsigned int)` | exact | exact |  |
| `CFileSystem_Stdio::FS_setmode(__sFILE*, FileMode_t)` | exact | exact |  |
| `CFileSystem_Stdio::FS_stat(char const*, stat*, bool*)` | masked | masked |  |
| `CFileSystem_Stdio::FS_vfprintf(__sFILE*, char const*, char*)` | exact | exact |  |
| `CFileSystem_Stdio::FreeOptimalReadBuffer(void*)` | masked | masked |  |
| `CFileSystem_Stdio::GetLocalCopy(char const*)` | exact | exact |  |
| `CFileSystem_Stdio::GetOptimalIOConstraints(void*, unsigned int*, unsigned int*, unsigned int*)` | differs | differs | size retail 142 vs ours 134 |
| `CFileSystem_Stdio::GetWaitForResourcesProgress(int, float*, bool*)` | exact | exact |  |
| `CFileSystem_Stdio::HintResourceNeed(char const*, int)` | exact | exact |  |
| `CFileSystem_Stdio::IsFileImmediatelyAvailable(char const*)` | exact | exact |  |
| `CFileSystem_Stdio::IsSteam() const` | exact | exact |  |
| `CFileSystem_Stdio::MountSteamContent(int)` | exact | exact |  |
| `CFileSystem_Stdio::QueryInterface(char const*)` | masked | masked |  |
| `CFileSystem_Stdio::WaitForResources(char const*)` | exact | exact |  |
| `CFileSystem_Stdio::~CFileSystem_Stdio()` | differs | differs | size retail 35 vs ours 9 |
| `CStdFilesystemFile::FS_GetSectorSize()` | exact | exact |  |
| `CStdioFile::FS_fclose()` | differs | differs | size retail 152 vs ours 213 |
| `CStdioFile::FS_feof()` | masked | masked |  |
| `CStdioFile::FS_ferror()` | masked | masked |  |
| `CStdioFile::FS_fflush()` | masked | masked |  |
| `CStdioFile::FS_fgets(char*, int)` | masked | masked |  |
| `CStdioFile::FS_fopen(char const*, char const*, long long*)` | differs | differs | size retail 632 vs ours 676 |
| `CStdioFile::FS_fread(void*, unsigned long, unsigned long)` | masked | masked |  |
| `CStdioFile::FS_fseek(long long, int)` | masked | masked |  |
| `CStdioFile::FS_ftell()` | masked | masked |  |
| `CStdioFile::FS_fwrite(void const*, unsigned long)` | masked | masked |  |
| `CStdioFile::FS_setbufsize(unsigned int)` | exact | exact |  |
| `CStdioFile::FS_setmode(FileMode_t)` | exact | exact |  |
| `CStdioFile::FS_vfprintf(char const*, char*)` | masked | masked |  |
| `CStdioFile::~CStdioFile()` | exact | exact |  |
| `CStdioFile::~CStdioFile()` | masked | masked |  |
| `CUtlMap<int, CInterlockedIntT<int>, unsigned short>::~CUtlMap()` | missing | missing |  |
| `CUtlMemory<UtlRBTreeNode_t<CUtlMap<int, CInterlockedIntT<int>, unsigned short>::Node_t, unsigned short>, unsigned short>::Grow(int)` | missing | missing |  |
| `CUtlRBTree<CUtlMap<int, CInterlockedIntT<int>, unsigned short>::Node_t, unsigned short, CUtlMap<int, CInterlockedIntT<int>, unsigned short>::CKeyLess,` | missing | missing |  |
| `CUtlRBTree<CUtlMap<int, CInterlockedIntT<int>, unsigned short>::Node_t, unsigned short, CUtlMap<int, CInterlockedIntT<int>, unsigned short>::CKeyLess,` | missing | missing |  |
| `CUtlRBTree<CUtlMap<int, CInterlockedIntT<int>, unsigned short>::Node_t, unsigned short, CUtlMap<int, CInterlockedIntT<int>, unsigned short>::CKeyLess,` | missing | missing |  |
| `CUtlRBTree<CUtlMap<int, CInterlockedIntT<int>, unsigned short>::Node_t, unsigned short, CUtlMap<int, CInterlockedIntT<int>, unsigned short>::CKeyLess,` | missing | missing |  |
| `CUtlRBTree<CUtlMap<int, CInterlockedIntT<int>, unsigned short>::Node_t, unsigned short, CUtlMap<int, CInterlockedIntT<int>, unsigned short>::CKeyLess,` | missing | missing |  |
| `CUtlRBTree<CUtlMap<int, CInterlockedIntT<int>, unsigned short>::Node_t, unsigned short, CUtlMap<int, CInterlockedIntT<int>, unsigned short>::CKeyLess,` | missing | missing |  |
| `CUtlRBTree<CUtlMap<int, CInterlockedIntT<int>, unsigned short>::Node_t, unsigned short, CUtlMap<int, CInterlockedIntT<int>, unsigned short>::CKeyLess,` | missing | missing |  |
| `CUtlRBTree<CUtlMap<int, CInterlockedIntT<int>, unsigned short>::Node_t, unsigned short, CUtlMap<int, CInterlockedIntT<int>, unsigned short>::CKeyLess,` | missing | missing |  |
| `CUtlRBTree<CUtlMap<int, CInterlockedIntT<int>, unsigned short>::Node_t, unsigned short, CUtlMap<int, CInterlockedIntT<int>, unsigned short>::CKeyLess,` | missing | missing |  |
| `CUtlRBTree<CUtlMap<int, CInterlockedIntT<int>, unsigned short>::Node_t, unsigned short, CUtlMap<int, CInterlockedIntT<int>, unsigned short>::CKeyLess,` | missing | missing |  |
| `UseOptimalBufferAllocation()` | masked | masked |  |
| `__CreateCFileSystem_StdioIBaseFileSystem_interface()` | masked | masked |  |
| `__CreateCFileSystem_StdioIFileSystem_interface()` | masked | masked |  |
| `__GLOBAL__I_a` | differs | differs | size retail 631 vs ours 707 |

## filetracker (23/96)

| function | status | with needed | note |
|---|---|---|---|
| `CDefOps<__sFILE*>::LessFunc(__sFILE* const&, __sFILE* const&)` | missing | missing | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CFileTracker2::AddFileHashForVPKFile(int, int, int, MD5Value_t&, CPackedStoreFileHandle&)` | differs | differs | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CFileTracker2::BlockUntilMD5RequestComplete(int, MD5Value_t*)` | differs | differs | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CFileTracker2::CFileTracker2(CBaseFileSystem*)` | differs | differs | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CFileTracker2::CFileTracker2(CBaseFileSystem*)` | masked | masked |  |
| `CFileTracker2::CheckCachedFileHash(char const*, char const*, int, FileHash_t*)` | differs | differs | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CFileTracker2::GetFilesToUnloadForWhitelistChange(IPureServerWhitelist*)` | differs | differs | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CFileTracker2::GetUnverifiedFileHashes(CUnverifiedFileHash*, int)` | differs | differs | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CFileTracker2::IdxFileFromName(char const*, char const*, int, long long, bool, bool)` | missing | missing | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CFileTracker2::IsMD5RequestComplete(int, MD5Value_t*)` | differs | differs | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CFileTracker2::MarkAllCRCsUnverified()` | differs | differs | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CFileTracker2::NoteFileIgnoredForPureServer(char const*, char const*, int)` | differs | differs | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CFileTracker2::NoteFileLoadedFromDisk(char const*, char const*, int, __sFILE*, long long)` | differs | differs | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CFileTracker2::NoteFileUnloaded(char const*, char const*)` | differs | differs | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CFileTracker2::NotePackFileAccess(char const*, char const*, int, CPackedStoreFileHandle&)` | differs | differs | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CFileTracker2::NotePackFileOpened(char const*, char const*, long long)` | differs | differs | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CFileTracker2::RecordFileClose(__sFILE*)` | missing | missing | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CFileTracker2::RecordFileRead(void*, unsigned long, unsigned long, __sFILE*)` | missing | missing | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CFileTracker2::RecordFileSeek(__sFILE*, long long, int)` | missing | missing | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CFileTracker2::ShutdownAsync()` | differs | differs | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CFileTracker2::SubmitThreadedMD5Request(unsigned char*, int, int, int, int)` | differs | differs | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CFileTracker2::ThreadedProcessMD5Requests()` | differs | differs | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CFileTracker2::~CFileTracker2()` | differs | differs | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CFileTracker2::~CFileTracker2()` | masked | masked |  |
| `CTSQueue<StuffToMD5_t, false>::CTSQueue()` | differs | differs | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CTSQueue<StuffToMD5_t, false>::Pop()` | differs | differs | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CTSQueue<StuffToMD5_t, false>::Purge()` | masked | masked |  |
| `CTSQueue<StuffToMD5_t, false>::Push(CTSQueue<StuffToMD5_t, false>::Node_t*)` | masked | masked |  |
| `CTSQueue<StuffToMD5_t, false>::~CTSQueue()` | masked | masked |  |
| `CUtlLinkedList<SubmittedMd5Job_t, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<SubmittedMd5Job_t, unsigned short>, unsigned s` | masked | masked |  |
| `CUtlLinkedList<SubmittedMd5Job_t, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<SubmittedMd5Job_t, unsigned short>, unsigned s` | exact | exact |  |
| `CUtlLinkedList<int, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<int, unsigned short>, unsigned short>>::AllocInternal(bool)` | missing | missing | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlLinkedList<int, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<int, unsigned short>, unsigned short>>::LinkAfter(unsigned s` | missing | missing | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlLinkedList<int, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<int, unsigned short>, unsigned short>>::LinkBefore(unsigned ` | missing | missing | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlLinkedList<int, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<int, unsigned short>, unsigned short>>::Purge()` | missing | missing | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlMemory<UtlLinkedListElem_t<SubmittedMd5Job_t, unsigned short>, unsigned short>::Grow(int)` | masked | masked |  |
| `CUtlMemory<UtlLinkedListElem_t<int, unsigned short>, unsigned short>::Grow(int)` | missing | missing | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlMemory<UtlRBTreeNode_t<CUtlMap<__sFILE*, int, int>::Node_t, int>, int>::Grow(int)` | missing | missing | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlMemory<UtlRBTreeNode_t<FileInVPK_t, int>, int>::Grow(int)` | missing | missing | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlMemory<UtlRBTreeNode_t<TrackedFile_t, int>, int>::Grow(int)` | differs | differs | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlMemory<UtlRBTreeNode_t<TrackedVPKFile_t, int>, int>::Grow(int)` | masked | masked |  |
| `CUtlRBTree<CUtlMap<__sFILE*, int, int>::Node_t, int, CUtlMap<__sFILE*, int, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<__sFILE*, int, int>::No` | missing | missing | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlRBTree<CUtlMap<__sFILE*, int, int>::Node_t, int, CUtlMap<__sFILE*, int, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<__sFILE*, int, int>::No` | missing | missing | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlRBTree<CUtlMap<__sFILE*, int, int>::Node_t, int, CUtlMap<__sFILE*, int, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<__sFILE*, int, int>::No` | missing | missing | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlRBTree<CUtlMap<__sFILE*, int, int>::Node_t, int, CUtlMap<__sFILE*, int, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<__sFILE*, int, int>::No` | missing | missing | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlRBTree<CUtlMap<__sFILE*, int, int>::Node_t, int, CUtlMap<__sFILE*, int, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<__sFILE*, int, int>::No` | missing | missing | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlRBTree<CUtlMap<__sFILE*, int, int>::Node_t, int, CUtlMap<__sFILE*, int, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<__sFILE*, int, int>::No` | missing | missing | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlRBTree<CUtlMap<__sFILE*, int, int>::Node_t, int, CUtlMap<__sFILE*, int, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<__sFILE*, int, int>::No` | missing | missing | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlRBTree<CUtlMap<__sFILE*, int, int>::Node_t, int, CUtlMap<__sFILE*, int, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<__sFILE*, int, int>::No` | missing | missing | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlRBTree<CUtlMap<__sFILE*, int, int>::Node_t, int, CUtlMap<__sFILE*, int, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<__sFILE*, int, int>::No` | missing | missing | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlRBTree<CUtlMap<__sFILE*, int, int>::Node_t, int, CUtlMap<__sFILE*, int, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<__sFILE*, int, int>::No` | missing | missing | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlRBTree<CUtlMap<__sFILE*, int, int>::Node_t, int, CUtlMap<__sFILE*, int, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<__sFILE*, int, int>::No` | missing | missing | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlRBTree<FileInVPK_t, int, bool (*)(FileInVPK_t const&, FileInVPK_t const&), CUtlMemory<UtlRBTreeNode_t<FileInVPK_t, int>, int>>::Find(FileInVPK_t c` | missing | missing | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlRBTree<FileInVPK_t, int, bool (*)(FileInVPK_t const&, FileInVPK_t const&), CUtlMemory<UtlRBTreeNode_t<FileInVPK_t, int>, int>>::FindInsertionPosit` | missing | missing | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlRBTree<FileInVPK_t, int, bool (*)(FileInVPK_t const&, FileInVPK_t const&), CUtlMemory<UtlRBTreeNode_t<FileInVPK_t, int>, int>>::Insert(FileInVPK_t` | missing | missing | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlRBTree<FileInVPK_t, int, bool (*)(FileInVPK_t const&, FileInVPK_t const&), CUtlMemory<UtlRBTreeNode_t<FileInVPK_t, int>, int>>::InsertRebalance(in` | missing | missing | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlRBTree<FileInVPK_t, int, bool (*)(FileInVPK_t const&, FileInVPK_t const&), CUtlMemory<UtlRBTreeNode_t<FileInVPK_t, int>, int>>::IsLeftChild(int) c` | missing | missing | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlRBTree<FileInVPK_t, int, bool (*)(FileInVPK_t const&, FileInVPK_t const&), CUtlMemory<UtlRBTreeNode_t<FileInVPK_t, int>, int>>::IsRightChild(int) ` | missing | missing | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlRBTree<FileInVPK_t, int, bool (*)(FileInVPK_t const&, FileInVPK_t const&), CUtlMemory<UtlRBTreeNode_t<FileInVPK_t, int>, int>>::NewNode()` | missing | missing | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlRBTree<FileInVPK_t, int, bool (*)(FileInVPK_t const&, FileInVPK_t const&), CUtlMemory<UtlRBTreeNode_t<FileInVPK_t, int>, int>>::RemoveAll()` | missing | missing | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlRBTree<FileInVPK_t, int, bool (*)(FileInVPK_t const&, FileInVPK_t const&), CUtlMemory<UtlRBTreeNode_t<FileInVPK_t, int>, int>>::RotateLeft(int)` | missing | missing | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlRBTree<FileInVPK_t, int, bool (*)(FileInVPK_t const&, FileInVPK_t const&), CUtlMemory<UtlRBTreeNode_t<FileInVPK_t, int>, int>>::RotateRight(int)` | missing | missing | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlRBTree<TrackedFile_t, int, bool (*)(TrackedFile_t const&, TrackedFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedFile_t, int>, int>>::Find(Track` | differs | differs | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlRBTree<TrackedFile_t, int, bool (*)(TrackedFile_t const&, TrackedFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedFile_t, int>, int>>::FindInsert` | differs | differs | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlRBTree<TrackedFile_t, int, bool (*)(TrackedFile_t const&, TrackedFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedFile_t, int>, int>>::FirstInord` | differs | differs | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlRBTree<TrackedFile_t, int, bool (*)(TrackedFile_t const&, TrackedFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedFile_t, int>, int>>::Insert(Tra` | differs | differs | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlRBTree<TrackedFile_t, int, bool (*)(TrackedFile_t const&, TrackedFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedFile_t, int>, int>>::InsertReba` | differs | differs | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlRBTree<TrackedFile_t, int, bool (*)(TrackedFile_t const&, TrackedFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedFile_t, int>, int>>::IsLeftChil` | differs | differs | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlRBTree<TrackedFile_t, int, bool (*)(TrackedFile_t const&, TrackedFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedFile_t, int>, int>>::IsRightChi` | differs | differs | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlRBTree<TrackedFile_t, int, bool (*)(TrackedFile_t const&, TrackedFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedFile_t, int>, int>>::NewNode()` | differs | differs | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlRBTree<TrackedFile_t, int, bool (*)(TrackedFile_t const&, TrackedFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedFile_t, int>, int>>::NextInorde` | differs | differs | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlRBTree<TrackedFile_t, int, bool (*)(TrackedFile_t const&, TrackedFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedFile_t, int>, int>>::Reinsert(i` | differs | differs | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlRBTree<TrackedFile_t, int, bool (*)(TrackedFile_t const&, TrackedFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedFile_t, int>, int>>::RemoveAll(` | differs | differs | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlRBTree<TrackedFile_t, int, bool (*)(TrackedFile_t const&, TrackedFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedFile_t, int>, int>>::RemoveReba` | differs | differs | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlRBTree<TrackedFile_t, int, bool (*)(TrackedFile_t const&, TrackedFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedFile_t, int>, int>>::RotateLeft` | differs | differs | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlRBTree<TrackedFile_t, int, bool (*)(TrackedFile_t const&, TrackedFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedFile_t, int>, int>>::RotateRigh` | differs | differs | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlRBTree<TrackedFile_t, int, bool (*)(TrackedFile_t const&, TrackedFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedFile_t, int>, int>>::Unlink(int` | differs | differs | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlRBTree<TrackedVPKFile_t, int, bool (*)(TrackedVPKFile_t const&, TrackedVPKFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedVPKFile_t, int>, int>>` | masked | masked |  |
| `CUtlRBTree<TrackedVPKFile_t, int, bool (*)(TrackedVPKFile_t const&, TrackedVPKFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedVPKFile_t, int>, int>>` | masked | masked |  |
| `CUtlRBTree<TrackedVPKFile_t, int, bool (*)(TrackedVPKFile_t const&, TrackedVPKFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedVPKFile_t, int>, int>>` | masked | masked |  |
| `CUtlRBTree<TrackedVPKFile_t, int, bool (*)(TrackedVPKFile_t const&, TrackedVPKFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedVPKFile_t, int>, int>>` | masked | masked |  |
| `CUtlRBTree<TrackedVPKFile_t, int, bool (*)(TrackedVPKFile_t const&, TrackedVPKFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedVPKFile_t, int>, int>>` | masked | masked |  |
| `CUtlRBTree<TrackedVPKFile_t, int, bool (*)(TrackedVPKFile_t const&, TrackedVPKFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedVPKFile_t, int>, int>>` | masked | masked |  |
| `CUtlRBTree<TrackedVPKFile_t, int, bool (*)(TrackedVPKFile_t const&, TrackedVPKFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedVPKFile_t, int>, int>>` | differs | differs | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `CUtlRBTree<TrackedVPKFile_t, int, bool (*)(TrackedVPKFile_t const&, TrackedVPKFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedVPKFile_t, int>, int>>` | masked | masked |  |
| `CUtlRBTree<TrackedVPKFile_t, int, bool (*)(TrackedVPKFile_t const&, TrackedVPKFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedVPKFile_t, int>, int>>` | masked | masked |  |
| `CUtlRBTree<TrackedVPKFile_t, int, bool (*)(TrackedVPKFile_t const&, TrackedVPKFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedVPKFile_t, int>, int>>` | masked | masked |  |
| `FileInVPK_t::Less(FileInVPK_t const&, FileInVPK_t const&)` | missing | missing | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `FileListToUnloadForWhitelistChange::AddFile(char const*)` | masked | masked |  |
| `FileListToUnloadForWhitelistChange::IsFileInList(char const*)` | masked | masked |  |
| `FileListToUnloadForWhitelistChange::Release()` | masked | masked |  |
| `ThreadStubProcessMD5Requests(void*)` | exact | exact |  |
| `TrackedFile_t::Less(TrackedFile_t const&, TrackedFile_t const&)` | differs | differs | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `TrackedFile_t::ProcessFileRead(void*, unsigned long)` | missing | missing | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `TrackedFile_t::RebuildFileName(char const*, char const*)` | missing | missing | TF2-era CFileTracker2 (file-read MD5 tracking) not in our tree |
| `TrackedVPKFile_t::Less(TrackedVPKFile_t const&, TrackedVPKFile_t const&)` | exact | exact |  |

## linux_support (5/5)

| function | status | with needed | note |
|---|---|---|---|
| `FileSelect(dirent const*)` | masked | masked |  |
| `FillDataStruct(FIND_DATA*)` | masked | masked |  |
| `FindClose(void*)` | exact | exact |  |
| `FindFirstFile(char const*, FIND_DATA*)` | masked | masked |  |
| `FindNextFile(void*, FIND_DATA*)` | masked | masked |  |

## QueuedLoader (72/89)

| function | status | with needed | note |
|---|---|---|---|
| `AdjustAsyncIOSpeed()` | masked | masked |  |
| `CBaseAppSystem<IQueuedLoader>::QueryInterface(char const*)` | exact | exact |  |
| `CDummyProgress::BeginProgress()` | exact | exact |  |
| `CDummyProgress::EndProgress()` | exact | exact |  |
| `CDummyProgress::UpdateProgress(float)` | exact | exact |  |
| `CFunctor4<void (*)(FileJob_t*, void (*)(void*, void*, void const*, int, LoaderError_t), void*, void*), FileJob_t*, void (*)(void*, void*, void const*,` | exact | exact |  |
| `CFunctor4<void (*)(FileJob_t*, void (*)(void*, void*, void const*, int, LoaderError_t), void*, void*), FileJob_t*, void (*)(void*, void*, void const*,` | exact | exact |  |
| `CFunctor4<void (*)(FileJob_t*, void (*)(void*, void*, void const*, int, LoaderError_t), void*, void*), FileJob_t*, void (*)(void*, void*, void const*,` | masked | masked |  |
| `CFunctor4<void (*)(FileJob_t*, void*, int, LoaderError_t), FileJob_t*, void*, int, LoaderError_t, CRefCounted1<CFunctor, CRefCountServiceBase<true, CR` | exact | exact |  |
| `CFunctor4<void (*)(FileJob_t*, void*, int, LoaderError_t), FileJob_t*, void*, int, LoaderError_t, CRefCounted1<CFunctor, CRefCountServiceBase<true, CR` | exact | exact |  |
| `CFunctor4<void (*)(FileJob_t*, void*, int, LoaderError_t), FileJob_t*, void*, int, LoaderError_t, CRefCounted1<CFunctor, CRefCountServiceBase<true, CR` | masked | masked |  |
| `CQueuedLoader::AddJob(LoaderJob_t const*)` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CQueuedLoader::AddMapResource(char const*)` | masked | differs |  |
| `CQueuedLoader::AddResourceToTable(char const*)` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CQueuedLoader::BeginMapLoading(char const*, bool, bool)` | exact | exact |  |
| `CQueuedLoader::BuildMaterialResources(IResourcePreload*, CUtlSortVector<void*, CQueuedLoader::CResourceNameLessFunc, CUtlVector<void*, CUtlMemory<void` | masked | masked |  |
| `CQueuedLoader::BuildResources(IResourcePreload*, CUtlSortVector<void*, CQueuedLoader::CResourceNameLessFunc, CUtlVector<void*, CUtlMemory<void*, int>>` | masked | masked |  |
| `CQueuedLoader::CFileJobsLessFunc::GetLayoutOrderForFilename(char const*)` | masked | masked |  |
| `CQueuedLoader::CFileJobsLessFunc::Less(FileJob_t* const&, FileJob_t* const&, void*)` | masked | masked |  |
| `CQueuedLoader::CQueuedLoader()` | differs | differs | size retail 1945 vs ours 2216 |
| `CQueuedLoader::CResourceNameLessFunc::Less(void* const&, void* const&, void*)` | masked | masked |  |
| `CQueuedLoader::ClaimAnonymousJob(char const*, void (*)(void*, void*, void const*, int, LoaderError_t), void*, void*)` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CQueuedLoader::ClaimAnonymousJob(char const*, void**, int*, LoaderError_t*)` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CQueuedLoader::CleanQueue()` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CQueuedLoader::CompleteDynamicLoad()` | masked | masked |  |
| `CQueuedLoader::DynamicLoadMapResource(char const*, void (*)(char const*, void*, void*), void*, void*)` | masked | masked |  |
| `CQueuedLoader::EndMapLoading(bool)` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CQueuedLoader::GetJobRequests()` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CQueuedLoader::GetSpewDetail() const` | masked | masked |  |
| `CQueuedLoader::Init()` | masked | masked |  |
| `CQueuedLoader::InstallLoader(ResourcePreload_t, IResourcePreload*)` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CQueuedLoader::InstallProgress(ILoaderProgress*)` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CQueuedLoader::IsBatching() const` | exact | exact |  |
| `CQueuedLoader::IsDynamic() const` | exact | exact |  |
| `CQueuedLoader::IsFinished() const` | masked | masked |  |
| `CQueuedLoader::IsMapLoading() const` | exact | exact |  |
| `CQueuedLoader::IsSameMapLoading() const` | exact | exact |  |
| `CQueuedLoader::PurgeAll()` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CQueuedLoader::QueueDynamicLoadFunctor(CFunctor*)` | masked | masked |  |
| `CQueuedLoader::Shutdown()` | masked | masked |  |
| `CQueuedLoader::SpewInfo()` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CQueuedLoader::SubmitBatchedJobs()` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CQueuedLoader::SubmitPendingJobs()` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CQueuedLoader::~CQueuedLoader()` | masked | masked |  |
| `CQueuedLoader::~CQueuedLoader()` | differs | differs | size retail 732 vs ours 862 |
| `CQueuedLoader::~CQueuedLoader()` | masked | masked |  |
| `CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>::AddRef()` | masked | masked |  |
| `CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>::Release()` | masked | masked |  |
| `CResourcePreloadAnonymous::CreateResource(char const*)` | masked | masked |  |
| `CResourcePreloadAnonymous::OnEndMapLoading(bool)` | exact | exact |  |
| `CResourcePreloadAnonymous::PurgeAll()` | exact | exact |  |
| `CResourcePreloadAnonymous::PurgeUnreferencedResources()` | exact | exact |  |
| `CTSList<FileJob_t*>::~CTSList()` | masked | masked |  |
| `CTier2AppSystem<IQueuedLoader, 0>::Connect(void* (*)(char const*, int*))` | masked | masked |  |
| `CTier2AppSystem<IQueuedLoader, 0>::Disconnect()` | masked | masked |  |
| `CUtlDict<FileJob_t*, int>::Purge()` | masked | masked |  |
| `CUtlLinkedList<FileJob_t*, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<FileJob_t*, unsigned short>, unsigned short>>::AllocI` | masked | masked |  |
| `CUtlLinkedList<FileJob_t*, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<FileJob_t*, unsigned short>, unsigned short>>::LinkBe` | exact | exact |  |
| `CUtlMemory<CFunctor*, int>::Grow(int)` | masked | masked |  |
| `CUtlMemory<UtlLinkedListElem_t<FileJob_t*, unsigned short>, unsigned short>::Grow(int)` | masked | masked |  |
| `CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, FileJob_t*, int>::Node_t, int>, int>::Grow(int)` | masked | masked |  |
| `CUtlMemory<void*, int>::Grow(int)` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, FileJob_t*, int>::Node_t, int, CUtlMap<char const*, FileJob_t*, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<cha` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, FileJob_t*, int>::Node_t, int, CUtlMap<char const*, FileJob_t*, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<cha` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, FileJob_t*, int>::Node_t, int, CUtlMap<char const*, FileJob_t*, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<cha` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, FileJob_t*, int>::Node_t, int, CUtlMap<char const*, FileJob_t*, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<cha` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, FileJob_t*, int>::Node_t, int, CUtlMap<char const*, FileJob_t*, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<cha` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, FileJob_t*, int>::Node_t, int, CUtlMap<char const*, FileJob_t*, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<cha` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, FileJob_t*, int>::Node_t, int, CUtlMap<char const*, FileJob_t*, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<cha` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, FileJob_t*, int>::Node_t, int, CUtlMap<char const*, FileJob_t*, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<cha` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, FileJob_t*, int>::Node_t, int, CUtlMap<char const*, FileJob_t*, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<cha` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, FileJob_t*, int>::Node_t, int, CUtlMap<char const*, FileJob_t*, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<cha` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, FileJob_t*, int>::Node_t, int, CUtlMap<char const*, FileJob_t*, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<cha` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, FileJob_t*, int>::Node_t, int, CUtlMap<char const*, FileJob_t*, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<cha` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, FileJob_t*, int>::Node_t, int, CUtlMap<char const*, FileJob_t*, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<cha` | masked | masked |  |
| `CUtlSortVector<FileJob_t*, CQueuedLoader::CFileJobsLessFunc, CUtlVector<FileJob_t*, CUtlMemory<FileJob_t*, int>>>::CompareHelper(FileJob_t* const*, Fi` | masked | masked |  |
| `CUtlSortVector<void*, CQueuedLoader::CResourceNameLessFunc, CUtlVector<void*, CUtlMemory<void*, int>>>::CompareHelper(void* const*, void* const*)` | masked | masked |  |
| `CUtlSortVector<void*, CQueuedLoader::CResourceNameLessFunc, CUtlVector<void*, CUtlMemory<void*, int>>>::Remove(void* const&)` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `FinishAnonymousJob(FileJob_t*, void (*)(void*, void*, void const*, int, LoaderError_t), void*, void*)` | masked | masked |  |
| `IOAsyncCallback(FileAsyncRequest_t const&, int, FSAsyncStatus_t)` | masked | masked |  |
| `IOComputationJob(FileJob_t*, void*, int, LoaderError_t)` | masked | masked |  |
| `__CreateCQueuedLoaderIQueuedLoader_interface()` | masked | masked |  |
| `__GLOBAL__I_a` | masked | masked |  |
| `int CUtlSortVector<void*, CQueuedLoader::CResourceNameLessFunc, CUtlVector<void*, CUtlMemory<void*, int>>>::Find<void*>(void* const&) const` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `loader_dump_table(CCommand const&)` | masked | masked |  |
| `non-virtual thunk to CFunctor4<void (*)(FileJob_t*, void (*)(void*, void*, void const*, int, LoaderError_t), void*, void*), FileJob_t*, void (*)(void*` | exact | exact |  |
| `non-virtual thunk to CFunctor4<void (*)(FileJob_t*, void (*)(void*, void*, void const*, int, LoaderError_t), void*, void*), FileJob_t*, void (*)(void*` | masked | masked |  |
| `non-virtual thunk to CFunctor4<void (*)(FileJob_t*, void*, int, LoaderError_t), FileJob_t*, void*, int, LoaderError_t, CRefCounted1<CFunctor, CRefCoun` | exact | exact |  |
| `non-virtual thunk to CFunctor4<void (*)(FileJob_t*, void*, int, LoaderError_t), FileJob_t*, void*, int, LoaderError_t, CRefCounted1<CFunctor, CRefCoun` | masked | masked |  |

