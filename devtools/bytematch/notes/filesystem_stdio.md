# filesystem_stdio: per-function status vs retail depot 223

Generated per object (the module's own retail objects) with fsmeasure.py in
byte-match-survey/lane-mod-filesystem2-scratch: clang-600.0.57 with the `--retail-osx32` command lines,
devtools/bytematch against depot 223 filesystem_stdio.dylib. "with needed" is the status with the shared
header changes listed in lane-mod-filesystem2.md applied (needed.patch in that folder).

## kevvaluescompiler (33/33, 33 with needed)

| function | status | with needed | note |
|---|---|---|---|
| `CRunTimeKeyValuesStringTable::ReadStringTable(int, CUtlBuffer&)` | masked | masked |  |
| `CCompiledKeyValuesReader::CCompiledKeyValuesReader()` | masked | masked |  |
| `CCompiledKeyValuesReader::CCompiledKeyValuesReader()` | exact | exact |  |
| `CCompiledKeyValuesReader::LoadFile(char const*)` | masked | masked |  |
| `CCompiledKeyValuesReader::CreateInPlaceFromData(KeyValues&, CCompiledKeyValuesReader::FileInfo_t const&)` | masked | masked |  |
| `CCompiledKeyValuesReader::InstanceInPlace(KeyValues&, char const*)` | masked | masked |  |
| `CCompiledKeyValuesReader::Instance(char const*)` | masked | masked |  |
| `CCompiledKeyValuesReader::LookupKeyValuesRootKeyName(char const*, char*, unsigned long)` | masked | masked |  |
| `CCompiledKeyValuesReader::FileInfo_t::Less(CCompiledKeyValuesReader::FileInfo_t const&, CCompiledKeyValuesReader::FileInfo_t const&)` | exact | exact |  |
| `CUtlRBTree<CCompiledKeyValuesReader::FileInfo_t, unsigned short, bool (*)(CCompiledKeyValuesReader::FileInfo_t const&, CCompiledKeyValuesReader::FileI` | masked | masked |  |
| `CreateHelper_t::Less(CreateHelper_t const&, CreateHelper_t const&)` | exact | exact |  |
| `CUtlRBTree<CreateHelper_t, int, bool (*)(CreateHelper_t const&, CreateHelper_t const&), CUtlMemory<UtlRBTreeNode_t<CreateHelper_t, int>, int>>::Find(C` | masked | masked |  |
| `CUtlRBTree<CCompiledKeyValuesReader::FileInfo_t, unsigned short, bool (*)(CCompiledKeyValuesReader::FileInfo_t const&, CCompiledKeyValuesReader::FileI` | masked | masked |  |
| `CUtlRBTree<CreateHelper_t, int, bool (*)(CreateHelper_t const&, CreateHelper_t const&), CUtlMemory<UtlRBTreeNode_t<CreateHelper_t, int>, int>>::FindIn` | masked | masked |  |
| `CUtlRBTree<CreateHelper_t, int, bool (*)(CreateHelper_t const&, CreateHelper_t const&), CUtlMemory<UtlRBTreeNode_t<CreateHelper_t, int>, int>>::Insert` | masked | masked |  |
| `CUtlRBTree<CreateHelper_t, int, bool (*)(CreateHelper_t const&, CreateHelper_t const&), CUtlMemory<UtlRBTreeNode_t<CreateHelper_t, int>, int>>::Insert` | masked | masked |  |
| `CUtlRBTree<CreateHelper_t, int, bool (*)(CreateHelper_t const&, CreateHelper_t const&), CUtlMemory<UtlRBTreeNode_t<CreateHelper_t, int>, int>>::IsLeft` | masked | masked |  |
| `CUtlRBTree<CreateHelper_t, int, bool (*)(CreateHelper_t const&, CreateHelper_t const&), CUtlMemory<UtlRBTreeNode_t<CreateHelper_t, int>, int>>::IsRigh` | masked | masked |  |
| `CUtlRBTree<CreateHelper_t, int, bool (*)(CreateHelper_t const&, CreateHelper_t const&), CUtlMemory<UtlRBTreeNode_t<CreateHelper_t, int>, int>>::Rotate` | masked | masked |  |
| `CUtlRBTree<CreateHelper_t, int, bool (*)(CreateHelper_t const&, CreateHelper_t const&), CUtlMemory<UtlRBTreeNode_t<CreateHelper_t, int>, int>>::Rotate` | masked | masked |  |
| `CUtlMemory<UtlRBTreeNode_t<CreateHelper_t, int>, int>::Grow(int)` | masked | masked |  |
| `CUtlRBTree<CreateHelper_t, int, bool (*)(CreateHelper_t const&, CreateHelper_t const&), CUtlMemory<UtlRBTreeNode_t<CreateHelper_t, int>, int>>::Remove` | masked | masked |  |
| `CUtlRBTree<CCompiledKeyValuesReader::FileInfo_t, unsigned short, bool (*)(CCompiledKeyValuesReader::FileInfo_t const&, CCompiledKeyValuesReader::FileI` | masked | masked |  |
| `CUtlRBTree<CCompiledKeyValuesReader::FileInfo_t, unsigned short, bool (*)(CCompiledKeyValuesReader::FileInfo_t const&, CCompiledKeyValuesReader::FileI` | masked | masked |  |
| `CUtlRBTree<CCompiledKeyValuesReader::FileInfo_t, unsigned short, bool (*)(CCompiledKeyValuesReader::FileInfo_t const&, CCompiledKeyValuesReader::FileI` | masked | masked |  |
| `CUtlRBTree<CCompiledKeyValuesReader::FileInfo_t, unsigned short, bool (*)(CCompiledKeyValuesReader::FileInfo_t const&, CCompiledKeyValuesReader::FileI` | masked | masked |  |
| `CUtlRBTree<CCompiledKeyValuesReader::FileInfo_t, unsigned short, bool (*)(CCompiledKeyValuesReader::FileInfo_t const&, CCompiledKeyValuesReader::FileI` | masked | masked |  |
| `CUtlRBTree<CCompiledKeyValuesReader::FileInfo_t, unsigned short, bool (*)(CCompiledKeyValuesReader::FileInfo_t const&, CCompiledKeyValuesReader::FileI` | masked | masked |  |
| `CUtlRBTree<CCompiledKeyValuesReader::FileInfo_t, unsigned short, bool (*)(CCompiledKeyValuesReader::FileInfo_t const&, CCompiledKeyValuesReader::FileI` | masked | masked |  |
| `CUtlMemory<UtlRBTreeNode_t<CCompiledKeyValuesReader::FileInfo_t, unsigned short>, unsigned short>::Grow(int)` | masked | masked |  |
| `CUtlRBTree<CCompiledKeyValuesReader::FileInfo_t, unsigned short, bool (*)(CCompiledKeyValuesReader::FileInfo_t const&, CCompiledKeyValuesReader::FileI` | masked | masked |  |
| `CUtlMemory<KVInfo_t, int>::Grow(int)` | masked | masked |  |
| `CUtlMemory<char const*, int>::Grow(int)` | masked | masked |  |

## zip_utils (64/65, 64 with needed)

| function | status | with needed | note |
|---|---|---|---|
| `CZipFile::CZipFile(char const*, bool)` | masked | masked |  |
| `CZipFile::CZipEntry::ZipFileLessFunc_CaselessSort(CZipFile::CZipEntry const&, CZipFile::CZipEntry const&)` | masked | masked |  |
| `CZipFile::CZipEntry::ZipFileLessFunc(CZipFile::CZipEntry const&, CZipFile::CZipEntry const&)` | exact | exact |  |
| `CZipFile::~CZipFile()` | masked | masked |  |
| `CZipFile::ParseFromBuffer(void*, int)` | masked | masked |  |
| `CZipFile::ParseFromDisk(char const*)` | masked | masked |  |
| `CZipFile::AddBufferToZip(char const*, void*, int, bool)` | masked | masked |  |
| `CZipFile::ReadFileFromZip(char const*, bool, CUtlBuffer&)` | masked | masked |  |
| `CZipFile::ReadFileFromZip(void*, char const*, bool, CUtlBuffer&)` | masked | masked |  |
| `CZipFile::FileExistsInZip(char const*)` | masked | masked |  |
| `CZipFile::AddFileToZip(char const*, char const*)` | masked | masked |  |
| `CZipFile::RemoveFileFromZip(char const*)` | masked | masked |  |
| `CZipFile::CalculateSize()` | masked | masked |  |
| `CZipFile::SaveDirectory(IWriteStream&)` | masked | masked |  |
| `IZip::CreateZip(char const*, bool)` | masked | masked |  |
| `IZip::ReleaseZip(IZip*)` | masked | masked |  |
| `CZip::CZip(char const*, bool)` | masked | masked |  |
| `CZip::~CZip()` | masked | masked |  |
| `CZip::~CZip()` | masked | masked |  |
| `CZip::SetBigEndian(bool)` | exact | exact |  |
| `CZip::ActivateByteSwapping(bool)` | exact | exact |  |
| `CZip::AddFileToZip(char const*, char const*)` | masked | masked |  |
| `CZip::FileExistsInZip(char const*)` | masked | masked |  |
| `CZip::ReadFileFromZip(char const*, bool, CUtlBuffer&)` | masked | masked |  |
| `CZip::ReadFileFromZip(void*, char const*, bool, CUtlBuffer&)` | masked | masked |  |
| `CZip::RemoveFileFromZip(char const*)` | masked | masked |  |
| `CZip::GetNextFilename(int, char*, int, int&)` | masked | masked |  |
| `CZip::PrintDirectory()` | masked | masked |  |
| `CZip::Reset()` | masked | masked |  |
| `CZip::EstimateSize()` | masked | masked |  |
| `CZip::AddBufferToZip(char const*, void*, int, bool)` | masked | masked |  |
| `CZip::SaveToBuffer(CUtlBuffer&)` | masked | masked |  |
| `CZip::SaveToDisk(__sFILE*)` | masked | masked |  |
| `CZip::SaveToDisk(void*)` | masked | masked |  |
| `CZip::ParseFromBuffer(void*, int)` | masked | masked |  |
| `CZip::ParseFromDisk(char const*)` | masked | masked |  |
| `CZip::ForceAlignment(bool, bool, unsigned int)` | exact | exact |  |
| `CZip::GetAlignment()` | exact | exact |  |
| `CDatadescGeneratedNameHolder::~CDatadescGeneratedNameHolder()` | differs | differs | datamap.h: scalar delete |
| `___clang_call_terminate` | masked | masked |  |
| `CUtlRBTree<CZipFile::CZipEntry, int, bool (*)(CZipFile::CZipEntry const&, CZipFile::CZipEntry const&), CUtlMemory<UtlRBTreeNode_t<CZipFile::CZipEntry,` | masked | masked |  |
| `CWin32File::CreateTempFile(CUtlString&, CUtlString&)` | masked | masked |  |
| `CUtlRBTree<CZipFile::CZipEntry, int, bool (*)(CZipFile::CZipEntry const&, CZipFile::CZipEntry const&), CUtlMemory<UtlRBTreeNode_t<CZipFile::CZipEntry,` | masked | masked |  |
| `CUtlRBTree<CZipFile::CZipEntry, int, bool (*)(CZipFile::CZipEntry const&, CZipFile::CZipEntry const&), CUtlMemory<UtlRBTreeNode_t<CZipFile::CZipEntry,` | masked | masked |  |
| `CUtlRBTree<CZipFile::CZipEntry, int, bool (*)(CZipFile::CZipEntry const&, CZipFile::CZipEntry const&), CUtlMemory<UtlRBTreeNode_t<CZipFile::CZipEntry,` | masked | masked |  |
| `CUtlRBTree<CZipFile::CZipEntry, int, bool (*)(CZipFile::CZipEntry const&, CZipFile::CZipEntry const&), CUtlMemory<UtlRBTreeNode_t<CZipFile::CZipEntry,` | masked | masked |  |
| `CUtlLinkedList<CZip*, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<CZip*, unsigned short>, unsigned short>>::~CUtlLinkedList(` | masked | masked |  |
| `CUtlLinkedList<CZip*, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<CZip*, unsigned short>, unsigned short>>::AllocInternal(bo` | masked | masked |  |
| `CUtlLinkedList<CZip*, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<CZip*, unsigned short>, unsigned short>>::LinkBefore(unsig` | exact | exact |  |
| `CUtlMemory<UtlLinkedListElem_t<CZip*, unsigned short>, unsigned short>::Grow(int)` | masked | masked |  |
| `CUtlRBTree<CZipFile::CZipEntry, int, bool (*)(CZipFile::CZipEntry const&, CZipFile::CZipEntry const&), CUtlMemory<UtlRBTreeNode_t<CZipFile::CZipEntry,` | masked | masked |  |
| `CUtlRBTree<CZipFile::CZipEntry, int, bool (*)(CZipFile::CZipEntry const&, CZipFile::CZipEntry const&), CUtlMemory<UtlRBTreeNode_t<CZipFile::CZipEntry,` | masked | masked |  |
| `CUtlRBTree<CZipFile::CZipEntry, int, bool (*)(CZipFile::CZipEntry const&, CZipFile::CZipEntry const&), CUtlMemory<UtlRBTreeNode_t<CZipFile::CZipEntry,` | masked | masked |  |
| `CUtlRBTree<CZipFile::CZipEntry, int, bool (*)(CZipFile::CZipEntry const&, CZipFile::CZipEntry const&), CUtlMemory<UtlRBTreeNode_t<CZipFile::CZipEntry,` | masked | masked |  |
| `CUtlRBTree<CZipFile::CZipEntry, int, bool (*)(CZipFile::CZipEntry const&, CZipFile::CZipEntry const&), CUtlMemory<UtlRBTreeNode_t<CZipFile::CZipEntry,` | masked | masked |  |
| `CUtlRBTree<CZipFile::CZipEntry, int, bool (*)(CZipFile::CZipEntry const&, CZipFile::CZipEntry const&), CUtlMemory<UtlRBTreeNode_t<CZipFile::CZipEntry,` | masked | masked |  |
| `CUtlRBTree<CZipFile::CZipEntry, int, bool (*)(CZipFile::CZipEntry const&, CZipFile::CZipEntry const&), CUtlMemory<UtlRBTreeNode_t<CZipFile::CZipEntry,` | masked | masked |  |
| `CUtlRBTree<CZipFile::CZipEntry, int, bool (*)(CZipFile::CZipEntry const&, CZipFile::CZipEntry const&), CUtlMemory<UtlRBTreeNode_t<CZipFile::CZipEntry,` | masked | masked |  |
| `CUtlRBTree<CZipFile::CZipEntry, int, bool (*)(CZipFile::CZipEntry const&, CZipFile::CZipEntry const&), CUtlMemory<UtlRBTreeNode_t<CZipFile::CZipEntry,` | masked | masked |  |
| `CUtlMemory<UtlRBTreeNode_t<CZipFile::CZipEntry, int>, int>::Grow(int)` | masked | masked |  |
| `CBufferStream::Put(void const*, int)` | masked | masked |  |
| `CBufferStream::Tell()` | exact | exact |  |
| `CFileStream::Put(void const*, int)` | masked | masked |  |
| `CFileStream::Tell()` | masked | masked |  |
| `__GLOBAL__I_a` | masked | masked |  |

## basefilesystem (220/257, 248 with needed)

| function | status | with needed | note |
|---|---|---|---|
| `BaseFileSystem()` | masked | masked |  |
| `CBaseFileSystem::CBaseFileSystem()` | differs | masked |  |
| `CBaseFileSystem::~CBaseFileSystem()` | differs | masked |  |
| `CBaseFileSystem::QueryInterface(char const*)` | masked | masked |  |
| `CBaseFileSystem::Init()` | masked | masked |  |
| `CBaseFileSystem::Shutdown()` | masked | masked |  |
| `CBaseFileSystem::UnloadCompiledKeyValues()` | differs | masked |  |
| `CBaseFileSystem::InstallDirtyDiskReportFunc(void (*)())` | differs | exact |  |
| `CBaseFileSystem::LogAccessToFile(char const*, char const*, char const*)` | masked | masked |  |
| `CBaseFileSystem::Warning(FileWarningLevel_t, char const*, ...)` | masked | masked |  |
| `CBaseFileSystem::Trace_FOpen(char const*, char const*, unsigned int, long long*)` | masked | masked |  |
| `CBaseFileSystem::Trace_FClose(__sFILE*)` | masked | masked |  |
| `CBaseFileSystem::Trace_FRead(int, __sFILE*)` | masked | masked |  |
| `CBaseFileSystem::Trace_FWrite(int, __sFILE*)` | masked | masked |  |
| `CBaseFileSystem::PrintOpenedFiles()` | exact | exact |  |
| `CBaseFileSystem::AddVPKFile(char const*, char const*, SearchPathAdd_t)` | differs | masked |  |
| `CBaseFileSystem::FindOrAddPathIDInfo(CUtlSymbol const&, int)` | masked | masked |  |
| `CBaseFileSystem::SetSearchPathIsTrustedSource(CBaseFileSystem::CSearchPath*)` | differs | masked |  |
| `CBaseFileSystem::AddPackFile(char const*, char const*)` | masked | masked |  |
| `CBaseFileSystem::AddPackFileFromPath(char const*, char const*, bool, char const*)` | masked | masked |  |
| `CPackFile::ReadFromPack(int, void*, int, int, long long)` | masked | masked |  |
| `CPackFile::OpenFile(char const*, char const*)` | masked | masked |  |
| `CBaseFileSystem::FindFileInVPKs(char const*, char const*)` | masked | masked |  |
| `CZipPackFile::ReadFromPack(int, void*, int, int, long long)` | masked | masked |  |
| `CZipPackFile::IndexToFilename(int, char*, int)` | masked | masked |  |
| `CZipPackFile::FindFile(char const*, int&, long long&, int&)` | masked | masked |  |
| `CZipPackFile::SetupPreloadData()` | masked | masked |  |
| `CZipPackFile::DiscardPreloadData()` | masked | masked |  |
| `CZipPackFile::Prepare(long long, long long)` | masked | masked |  |
| `CZipPackFile::~CZipPackFile()` | masked | masked |  |
| `CZipPackFile::~CZipPackFile()` | exact | exact |  |
| `CZipPackFile::~CZipPackFile()` | masked | masked |  |
| `CBaseFileSystem::AddPackFiles(char const*, CUtlSymbol const&, SearchPathAdd_t)` | masked | masked |  |
| `CBaseFileSystem::CSearchPathsIterator::GetNext()` | masked | masked |  |
| `CBaseFileSystem::AddMapPackFile(char const*, char const*, SearchPathAdd_t)` | differs | differs | block order of the loose/VPK .bsp header read |
| `CBaseFileSystem::BeginMapAccess()` | masked | masked |  |
| `CBaseFileSystem::EndMapAccess()` | masked | masked |  |
| `CBaseFileSystem::PrintSearchPaths()` | masked | masked |  |
| `CBaseFileSystem::AddSearchPathInternal(char const*, char const*, SearchPathAdd_t, bool)` | masked | masked |  |
| `CBaseFileSystem::SetCurrentGameExpansionId(char const*)` | differs | masked |  |
| `CBaseFileSystem::GetCurrentGameExpansionId(char*, int)` | differs | masked |  |
| `CBaseFileSystem::SetUseLowViolancePaths(bool)` | differs | exact |  |
| `CBaseFileSystem::SetUseHDMount(bool)` | differs | exact |  |
| `CBaseFileSystem::SetAudioLanguageOverride(char const*)` | differs | masked |  |
| `CBaseFileSystem::AddGameAndModSearchPathsFromGameInfo(char const*, char const*, KeyValues*)` | differs | masked |  |
| `FileSystem_AddLoadedSearchPath(IFileSystem*, char const*, char const*, char const*, bool, bool)` | masked | masked |  |
| `SortStricmp(char* const*, char* const*)` | masked | masked |  |
| `CBaseFileSystem::UpdateSearchPathsForCurrentGameExpansion()` | differs | differs | tier1: CUtlSymbolTableMT::Find is out of line in retail |
| `CBaseFileSystem::FindSearchPathByStoreId(int)` | exact | exact |  |
| `CBaseFileSystem::AddSearchPath(char const*, char const*, SearchPathAdd_t)` | masked | masked |  |
| `CBaseFileSystem::GetSearchPath(char const*, bool, char*, int)` | masked | masked |  |
| `CBaseFileSystem::GetSearchPaths2(char const*, bool, CUtlVector<CUtlString, CUtlMemory<CUtlString, int>>&)` | differs | differs | scheduling of the count increment |
| `CBaseFileSystem::RemoveSearchPath(char const*, char const*)` | masked | masked |  |
| `CBaseFileSystem::RemoveSearchPaths(char const*)` | masked | masked |  |
| `CBaseFileSystem::FindWritePath(char const*, char const*)` | masked | masked |  |
| `CBaseFileSystem::GetWritePath(char const*, char const*)` | masked | masked |  |
| `CBaseFileSystem::ReadToBuffer(void*, CUtlBuffer&, int, void* (*)(char const*, unsigned int))` | masked | masked |  |
| `CBaseFileSystem::ReadFile(char const*, char const*, CUtlBuffer&, int, int, void* (*)(char const*, unsigned int))` | masked | masked |  |
| `non-virtual thunk to CBaseFileSystem::ReadFile(char const*, char const*, CUtlBuffer&, int, int, void* (*)(char const*, unsigned int))` | masked | masked |  |
| `CBaseFileSystem::ReadFileEx(char const*, char const*, void**, bool, bool, int, int, void* (*)(char const*, unsigned int))` | masked | masked |  |
| `CBaseFileSystem::WriteFile(char const*, char const*, CUtlBuffer&)` | masked | masked |  |
| `non-virtual thunk to CBaseFileSystem::WriteFile(char const*, char const*, CUtlBuffer&)` | masked | masked |  |
| `CBaseFileSystem::UnzipFile(char const*, char const*, char const*)` | masked | masked |  |
| `non-virtual thunk to CBaseFileSystem::UnzipFile(char const*, char const*, char const*)` | exact | exact |  |
| `CBaseFileSystem::RemoveAllSearchPaths()` | masked | masked |  |
| `CBaseFileSystem::LogFileAccess(char const*)` | masked | masked |  |
| `CBaseFileSystem::HandleOpenRegularFile(CFileOpenInfo&, bool)` | masked | masked |  |
| `LogFileOpen(char const*, char const*, char const*)` | masked | masked |  |
| `CBaseFileSystem::FindFileInSearchPath(CFileOpenInfo&)` | differs | masked |  |
| `CBaseFileSystem::OpenForRead(char const*, char const*, unsigned int, char const*, char**)` | differs | masked |  |
| `CBaseFileSystem::OpenForWrite(char const*, char const*, char const*)` | masked | masked |  |
| `CBaseFileSystem::Open(char const*, char const*, char const*)` | exact | exact |  |
| `non-virtual thunk to CBaseFileSystem::Open(char const*, char const*, char const*)` | exact | exact |  |
| `CBaseFileSystem::OpenEx(char const*, char const*, unsigned int, char const*, char**)` | masked | masked |  |
| `CBaseFileSystem::Close(void*)` | masked | masked |  |
| `non-virtual thunk to CBaseFileSystem::Close(void*)` | exact | exact |  |
| `CBaseFileSystem::Seek(void*, int, FileSystemSeek_t)` | masked | masked |  |
| `CFileHandle::Seek(long long, int)` | exact | exact |  |
| `non-virtual thunk to CBaseFileSystem::Seek(void*, int, FileSystemSeek_t)` | exact | exact |  |
| `CBaseFileSystem::Tell(void*)` | masked | masked |  |
| `non-virtual thunk to CBaseFileSystem::Tell(void*)` | masked | masked |  |
| `CBaseFileSystem::Size(void*)` | masked | masked |  |
| `CFileHandle::Size()` | exact | exact |  |
| `non-virtual thunk to CBaseFileSystem::Size(void*)` | exact | exact |  |
| `CBaseFileSystem::Size(char const*, char const*)` | masked | masked |  |
| `non-virtual thunk to CBaseFileSystem::Size(char const*, char const*)` | exact | exact |  |
| `CBaseFileSystem::FastFileTime(CBaseFileSystem::CSearchPath const*, char const*)` | masked | masked |  |
| `CBaseFileSystem::EndOfFile(void*)` | masked | masked |  |
| `CBaseFileSystem::Read(void*, int, void*)` | exact | exact |  |
| `non-virtual thunk to CBaseFileSystem::Read(void*, int, void*)` | exact | exact |  |
| `CBaseFileSystem::ReadEx(void*, int, int, void*)` | masked | masked |  |
| `CFileHandle::Read(void*, int, int)` | masked | masked |  |
| `CBaseFileSystem::LoadCompiledKeyValues(IFileSystem::KeyValuesPreloadType_t, char const*)` | differs | exact |  |
| `CBaseFileSystem::LoadKeyValues(KeyValues&, IFileSystem::KeyValuesPreloadType_t, char const*, char const*)` | differs | masked |  |
| `CBaseFileSystem::LoadKeyValues(IFileSystem::KeyValuesPreloadType_t, char const*, char const*)` | differs | masked |  |
| `CBaseFileSystem::LookupKeyValuesRootKeyName(char const*, char const*, char*, unsigned long)` | masked | masked |  |
| `CBaseFileSystem::ExtractRootKeyName(IFileSystem::KeyValuesPreloadType_t, char*, unsigned long, char const*, char const*)` | differs | masked |  |
| `CBaseFileSystem::SetupPreloadData()` | differs | masked |  |
| `CBaseFileSystem::DiscardPreloadData()` | masked | masked |  |
| `CBaseFileSystem::Write(void const*, int, void*)` | masked | masked |  |
| `non-virtual thunk to CBaseFileSystem::Write(void const*, int, void*)` | masked | masked |  |
| `CBaseFileSystem::FPrintf(void*, char const*, ...)` | masked | masked |  |
| `CBaseFileSystem::SetBufferSize(void*, unsigned int)` | masked | masked |  |
| `CBaseFileSystem::IsOk(void*)` | masked | masked |  |
| `CBaseFileSystem::Flush(void*)` | masked | masked |  |
| `non-virtual thunk to CBaseFileSystem::Flush(void*)` | masked | masked |  |
| `CBaseFileSystem::Precache(char const*, char const*)` | masked | masked |  |
| `non-virtual thunk to CBaseFileSystem::Precache(char const*, char const*)` | exact | exact |  |
| `CBaseFileSystem::ReadLine(char*, int, void*)` | masked | masked |  |
| `CBaseFileSystem::GetFileTime(char const*, char const*)` | masked | masked |  |
| `non-virtual thunk to CBaseFileSystem::GetFileTime(char const*, char const*)` | exact | exact |  |
| `CBaseFileSystem::GetPathTime(char const*, char const*)` | masked | masked |  |
| `CBaseFileSystem::MarkAllCRCsUnverified()` | masked | masked |  |
| `CBaseFileSystem::CacheFileCRCs(char const*, ECacheCRCType, IFileList*)` | exact | exact |  |
| `CBaseFileSystem::CheckCachedFileHash(char const*, char const*, int, FileHash_t*)` | masked | masked |  |
| `CBaseFileSystem::EnableWhitelistFileTracking(bool, bool, bool)` | differs | masked |  |
| `CBaseFileSystem::CacheAllVPKFileHashes(bool, bool)` | differs | masked |  |
| `CBaseFileSystem::CheckVPKFileHash(int, int, int, MD5Value_t&)` | differs | masked |  |
| `CBaseFileSystem::RegisterFileWhitelist(IPureServerWhitelist*, IFileList**)` | masked | masked |  |
| `CBaseFileSystem::NotifyFileUnloaded(char const*, char const*)` | masked | masked |  |
| `CBaseFileSystem::GetUnverifiedFileHashes(CUnverifiedFileHash*, int)` | masked | masked |  |
| `CBaseFileSystem::GetWhitelistSpewFlags()` | exact | exact |  |
| `CBaseFileSystem::SetWhitelistSpewFlags(int)` | exact | exact |  |
| `CBaseFileSystem::FileTimeToString(char*, int, long)` | masked | masked |  |
| `CBaseFileSystem::FileExists(char const*, char const*)` | masked | masked |  |
| `non-virtual thunk to CBaseFileSystem::FileExists(char const*, char const*)` | exact | exact |  |
| `CBaseFileSystem::IsFileWritable(char const*, char const*)` | masked | masked |  |
| `non-virtual thunk to CBaseFileSystem::IsFileWritable(char const*, char const*)` | exact | exact |  |
| `CBaseFileSystem::SetFileWritable(char const*, bool, char const*)` | masked | masked |  |
| `non-virtual thunk to CBaseFileSystem::SetFileWritable(char const*, bool, char const*)` | exact | exact |  |
| `CBaseFileSystem::IsDirectory(char const*, char const*)` | differs | masked |  |
| `CBaseFileSystem::CreateDirHierarchy(char const*, char const*)` | differs | differs | loop bound materialised from the frame pointer |
| `CBaseFileSystem::FindFirstEx(char const*, char const*, int*)` | exact | exact |  |
| `CBaseFileSystem::FindFirstHelper(char const*, char const*, int*, int*)` | masked | masked |  |
| `CBaseFileSystem::FindNextFileInVPKHelper(CBaseFileSystem::FindData_t*)` | masked | masked |  |
| `CBaseFileSystem::FindFirst(char const*, int*)` | exact | exact |  |
| `CBaseFileSystem::FindNextFileHelper(CBaseFileSystem::FindData_t*, int*)` | masked | masked |  |
| `CBaseFileSystem::FindNext(int)` | masked | masked |  |
| `CBaseFileSystem::FindIsDirectory(int)` | exact | exact |  |
| `CBaseFileSystem::FindClose(int)` | differs | masked |  |
| `CBaseFileSystem::GetFontTempFile(char const*, char*, int)` | masked | masked |  |
| `CBaseFileSystem::PushNoFinishAllOnSearchPaths()` | exact | exact |  |
| `CBaseFileSystem::PopNoFinishAllOnSearchPaths()` | exact | exact |  |
| `CBaseFileSystem::GetLocalCopy(char const*)` | exact | exact |  |
| `CBaseFileSystem::FixUpPath(char const*, char*, int)` | masked | masked |  |
| `CBaseFileSystem::RelativePathToFullPath(char const*, char const*, char*, int, PathTypeFilter_t, unsigned int*)` | masked | masked |  |
| `CBaseFileSystem::GetLocalPath(char const*, char*, int)` | exact | exact |  |
| `CBaseFileSystem::FullPathToRelativePathEx(char const*, char const*, char*, int)` | masked | masked |  |
| `CBaseFileSystem::FullPathToRelativePath(char const*, char*, int)` | exact | exact |  |
| `CBaseFileSystem::RemoveFile(char const*, char const*)` | masked | masked |  |
| `CBaseFileSystem::RenameFile(char const*, char const*, char const*)` | masked | masked |  |
| `CBaseFileSystem::GetCurrentDirectory(char*, int)` | masked | masked |  |
| `CBaseFileSystem::SetWarningFunc(void (*)(char const*, ...))` | exact | exact |  |
| `CBaseFileSystem::SetWarningLevel(FileWarningLevel_t)` | exact | exact |  |
| `CBaseFileSystem::GetFilesystemStatistics()` | exact | exact |  |
| `CBaseFileSystem::CSearchPath::~CSearchPath()` | masked | masked |  |
| `CBaseFileSystem::LoadModule(char const*, char const*, bool)` | masked | masked |  |
| `CBaseFileSystem::UnloadModule(CSysModule*)` | masked | masked |  |
| `CBaseFileSystem::AddLoggingFunc(void (*)(char const*, char const*))` | masked | masked |  |
| `CBaseFileSystem::RemoveLoggingFunc(void (*)(char const*, char const*))` | masked | masked |  |
| `CBaseFileSystem::FindOrAddFileName(char const*)` | masked | masked |  |
| `CBaseFileSystem::FindFileName(char const*)` | masked | masked |  |
| `CBaseFileSystem::String(void* const&, char*, int)` | masked | masked |  |
| `CBaseFileSystem::GetPathIndex(void* const&)` | differs | exact |  |
| `CBaseFileSystem::MarkPathIDByRequestOnly(char const*, bool)` | masked | masked |  |
| `CBaseFileSystem::GetFileTypeForFullPath(char const*, wchar_t*, unsigned long)` | masked | masked |  |
| `CBaseFileSystem::GetOptimalIOConstraints(void*, unsigned int*, unsigned int*, unsigned int*)` | exact | exact |  |
| `CBaseFileSystem::CreateFileCache()` | masked | masked |  |
| `CBaseFileSystem::AddFilesToFileCache(void*, char const**, int, char const*)` | exact | exact |  |
| `CBaseFileSystem::CFileCacheObject::AddFiles(char const**, int)` | differs | differs | register allocation |
| `CBaseFileSystem::IsFileCacheLoaded(void*)` | exact | exact |  |
| `CBaseFileSystem::DestroyFileCache(void*)` | masked | masked |  |
| `CBaseFileSystem::IsFileCacheFileLoaded(void*, char const*)` | masked | masked |  |
| `CBaseFileSystem::RegisterMemoryFile(CMemoryFileBacking*, CMemoryFileBacking**)` | masked | masked |  |
| `CBaseFileSystem::UnregisterMemoryFile(CMemoryFileBacking*)` | masked | masked |  |
| `CFileHandle::~CFileHandle()` | masked | masked |  |
| `CFileHandle::~CFileHandle()` | exact | exact |  |
| `CFileHandle::~CFileHandle()` | masked | masked |  |
| `CFileHandle::GetSectorSize()` | exact | exact |  |
| `CBaseFileSystem::CFileCacheObject::IOCallback(FileAsyncRequest_t const&, int, FSAsyncStatus_t)` | masked | masked |  |
| `CBaseFileSystem::CFileCacheObject::~CFileCacheObject()` | masked | masked |  |
| `CUtlSymbolTableMT::~CUtlSymbolTableMT()` | masked | masked |  |
| `CUtlVector<void*, CUtlMemory<void*, int>>::~CUtlVector()` | masked | masked |  |
| `int CUtlSortVector<CZipPackFile::CPackFileEntry, CZipPackFile::CPackFileLessFunc, CUtlVector<CZipPackFile::CPackFileEntry, CUtlMemory<CZipPackFile::CP` | differs | differs | bytes |
| `CUtlVector<CBaseFileSystem::CSearchPath, CUtlMemory<CBaseFileSystem::CSearchPath, int>>::InsertBefore(int)` | masked | masked |  |
| `CUtlStringList::CopyAndAddToTail(char const*)` | masked | masked |  |
| `CUtlSymbolTableMT::Find(char const*) const` | missing | missing | tier1 utlsymbol.h: inlined in ours |
| `CThreadLocal<char*>::~CThreadLocal()` | masked | masked |  |
| `CFileOpenInfo::HandleFileCRCTracking(char const*)` | differs | masked |  |
| `CFileOpenInfo::SetAbsolutePath(char const*, ...)` | masked | masked |  |
| `CUtlHashtable<char const*, CMemoryFileBacking*, DefaultHashFunctor<char const*>, DefaultEqualFunctor<char const*>, undefined_t>::Find(char const*) con` | masked | masked |  |
| `CBaseFileSystem::CSearchPathsVisits::MarkVisit(CBaseFileSystem::CSearchPath const&)` | masked | masked |  |
| `CTier1AppSystem<IFileSystem, 0>::Connect(void* (*)(char const*, int*))` | masked | masked |  |
| `CTier1AppSystem<IFileSystem, 0>::Disconnect()` | masked | masked |  |
| `CBaseFileSystem::AsyncAppend(char const*, void const*, int, bool, FSAsyncControl_t__**)` | exact | exact |  |
| `CBaseFileSystem::AllocOptimalReadBuffer(void*, unsigned int, unsigned int)` | masked | masked |  |
| `CBaseFileSystem::FreeOptimalReadBuffer(void*)` | masked | masked |  |
| `CBaseFileSystem::GetDVDMode()` | masked | masked |  |
| `CBaseFileSystem::FS_setmode(__sFILE*, FileMode_t)` | exact | exact |  |
| `CBaseFileSystem::FS_GetSectorSize(__sFILE*)` | exact | exact |  |
| `CPackFile::~CPackFile()` | masked | masked |  |
| `CPackFile::~CPackFile()` | masked | masked |  |
| `CRefCountServiceBase<true, CRefMT>::OnFinalRelease()` | exact | exact |  |
| `CPackFile::SetupPreloadData()` | exact | exact |  |
| `CPackFile::DiscardPreloadData()` | exact | exact |  |
| `CZipPackFile::GetPackFileBaseOffset()` | exact | exact |  |
| `CPackedStoreRefCount::~CPackedStoreRefCount()` | masked | masked |  |
| `CPackedStoreRefCount::~CPackedStoreRefCount()` | masked | masked |  |
| `CUtlMemory<CBaseFileSystem::CFileCacheObject::Info_t*, int>::Grow(int)` | masked | masked |  |
| `int CUtlHashtable<char const*, CMemoryFileBacking*, DefaultHashFunctor<char const*>, DefaultEqualFunctor<char const*>, undefined_t>::DoRemove<char con` | masked | masked |  |
| `unsigned int CUtlHashtable<char const*, CMemoryFileBacking*, DefaultHashFunctor<char const*>, DefaultEqualFunctor<char const*>, undefined_t>::DoInsert` | masked | masked |  |
| `CUtlHashtable<char const*, CMemoryFileBacking*, DefaultHashFunctor<char const*>, DefaultEqualFunctor<char const*>, undefined_t>::DoInsertUnconstructed` | masked | masked |  |
| `CUtlHashtable<char const*, CMemoryFileBacking*, DefaultHashFunctor<char const*>, DefaultEqualFunctor<char const*>, undefined_t>::DoRealloc(int)` | masked | masked |  |
| `CUtlHashtable<char const*, CMemoryFileBacking*, DefaultHashFunctor<char const*>, DefaultEqualFunctor<char const*>, undefined_t>::BumpEntry(unsigned in` | exact | exact |  |
| `CUtlMemory<CBaseFileSystem::CPathIDInfo*, int>::Grow(int)` | masked | masked |  |
| `CUtlMemory<void (*)(char const*, char const*), int>::Grow(int)` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, int, int>::Node_t, int, CUtlMap<char const*, int, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, int,` | masked | masked |  |
| `CBaseFileSystem::FindData_t::~FindData_t()` | differs | masked |  |
| `CUtlDict<int, int>::Purge()` | masked | masked |  |
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
| `CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, int, int>::Node_t, int>, int>::Grow(int)` | masked | masked |  |
| `CUtlMemory<char, int>::Grow(int)` | masked | masked |  |
| `CUtlLinkedList<CBaseFileSystem::FindData_t, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<CBaseFileSystem::FindData_t, unsigne` | masked | masked |  |
| `CUtlLinkedList<CBaseFileSystem::FindData_t, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<CBaseFileSystem::FindData_t, unsigne` | exact | exact |  |
| `CaselessStringLessThan(char const* const&, char const* const&)` | masked | masked |  |
| `CUtlMemory<UtlLinkedListElem_t<CBaseFileSystem::FindData_t, unsigned short>, unsigned short>::Grow(int)` | masked | masked |  |
| `CUtlMemory<CPackFile*, int>::Grow(int)` | masked | masked |  |
| `CUtlMemory<CBaseFileSystem::CSearchPath, int>::Grow(int)` | masked | masked |  |
| `CUtlMemory<CUtlString, int>::Grow(int)` | masked | masked |  |
| `CUtlSortVector<CZipPackFile::CPackFileEntry, CZipPackFile::CPackFileLessFunc, CUtlVector<CZipPackFile::CPackFileEntry, CUtlMemory<CZipPackFile::CPackF` | exact | exact |  |
| `CUtlMemory<CZipPackFile::CPackFileEntry, int>::Grow(int)` | masked | masked |  |
| `CUtlVector<CBaseFileSystem::COpenedFile, CUtlMemory<CBaseFileSystem::COpenedFile, int>>::InsertBefore(int, CBaseFileSystem::COpenedFile const&)` | masked | masked |  |
| `CUtlMemory<CBaseFileSystem::COpenedFile, int>::Grow(int)` | masked | masked |  |
| `CUtlLinkedList<CBaseFileSystem::FindData_t, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<CBaseFileSystem::FindData_t, unsigne` | masked | masked |  |
| `CMemoryFileBacking::~CMemoryFileBacking()` | masked | masked |  |
| `CMemoryFileBacking::~CMemoryFileBacking()` | masked | masked |  |
| `CUtlMemory<int, int>::Grow(int)` | masked | masked |  |
| `CMemoryFileHandle::~CMemoryFileHandle()` | masked | masked |  |
| `CMemoryFileHandle::~CMemoryFileHandle()` | masked | masked |  |
| `CMemoryFileHandle::~CMemoryFileHandle()` | masked | masked |  |
| `CBaseFileSystem::CSearchPathsIterator::CSearchPathsIterator(CBaseFileSystem*, char const*, PathTypeFilter_t)` | masked | masked |  |
| `CBaseFileSystem::CSearchPathsIterator::CopySearchPaths(CUtlVector<CBaseFileSystem::CSearchPath, CUtlMemory<CBaseFileSystem::CSearchPath, int>> const&)` | differs | differs | tier1 utlvector.h: SDK 2013 InsertMultipleBefore/AddMultipleToTail with defaulted pToCopy |
| `CUtlVector<CBaseFileSystem::CSearchPath, CUtlMemory<CBaseFileSystem::CSearchPath, int>>::InsertMultipleBefore(int, int, CBaseFileSystem::CSearchPath c` | missing | missing | tier1 utlvector.h: SDK 2013 InsertMultipleBefore/AddMultipleToTail with defaulted pToCopy |
| `CUtlMemory<char*, int>::Grow(int)` | masked | masked |  |
| `CUtlVectorAutoPurge<char*>::~CUtlVectorAutoPurge()` | missing | masked |  |
| `CUtlSymbolTableMT::String(CUtlSymbol) const` | masked | masked |  |
| `CBaseFileSystem::CSearchPathsIterator::CSearchPathsIterator(CBaseFileSystem*, char const**, char const*, PathTypeFilter_t)` | masked | masked |  |
| `__GLOBAL__I_a` | masked | masked |  |

## filesystem_async (101/107, 106 with needed)

| function | status | with needed | note |
|---|---|---|---|
| `async_suspend(CCommand const&)` | masked | masked |  |
| `async_resume(CCommand const&)` | masked | masked |  |
| `CBaseFileSystem::InitAsync()` | differs | differs | vstdlib jobthread.h: IThreadPool::Start slot and ThreadPoolStartParams_t size |
| `CBaseFileSystem::ShutdownAsync()` | exact | exact |  |
| `CBaseFileSystem::AsyncAddFetcher(IAsyncFileFetch*)` | differs | masked |  |
| `CBaseFileSystem::AsyncRemoveFetcher(IAsyncFileFetch*)` | differs | masked |  |
| `CBaseFileSystem::AsyncReadMultiple(FileAsyncRequest_t const*, int, FSAsyncControl_t__**)` | exact | exact |  |
| `CBaseFileSystem::AsyncReadMultipleCreditAlloc(FileAsyncRequest_t const*, int, char const*, int, FSAsyncControl_t__**)` | differs | masked |  |
| `CBaseFileSystem::AsyncWrite(char const*, void const*, int, bool, bool, FSAsyncControl_t__**)` | masked | masked |  |
| `CBaseFileSystem::AsyncWriteFile(char const*, CUtlBuffer const*, int, bool, bool, FSAsyncControl_t__**)` | masked | masked |  |
| `CBaseFileSystem::AsyncAppendFile(char const*, char const*, FSAsyncControl_t__**)` | masked | masked |  |
| `CBaseFileSystem::AsyncFinishAll(int)` | masked | masked |  |
| `CBaseFileSystem::AsyncFinishAllWrites()` | masked | masked |  |
| `AsyncWriteJobFilter(CJob*)` | masked | masked |  |
| `CBaseFileSystem::AsyncSuspend()` | exact | exact |  |
| `CBaseFileSystem::AsyncResume()` | exact | exact |  |
| `CBaseFileSystem::AsyncBeginRead(char const*, FSAsyncFile_t__**)` | masked | masked |  |
| `CBaseFileSystem::AsyncEndRead(FSAsyncFile_t__*)` | masked | masked |  |
| `CBaseFileSystem::AsyncFinish(FSAsyncControl_t__*, bool)` | masked | masked |  |
| `CBaseFileSystem::AsyncGetResult(FSAsyncControl_t__*, void**, int*)` | exact | exact |  |
| `CBaseFileSystem::AsyncAbort(FSAsyncControl_t__*)` | masked | masked |  |
| `CBaseFileSystem::AsyncStatus(FSAsyncControl_t__*)` | exact | exact |  |
| `CBaseFileSystem::AsyncFlush()` | differs | exact |  |
| `CBaseFileSystem::AsyncSetPriority(FSAsyncControl_t__*, int)` | exact | exact |  |
| `CBaseFileSystem::AsyncAddRef(FSAsyncControl_t__*)` | exact | exact |  |
| `CBaseFileSystem::AsyncRelease(FSAsyncControl_t__*)` | exact | exact |  |
| `CBaseFileSystem::SyncRead(FileAsyncRequest_t const&)` | masked | masked |  |
| `CBaseFileSystem::DoAsyncCallback(FileAsyncRequest_t const&, void*, int, FSAsyncStatus_t)` | masked | masked |  |
| `CBaseFileSystem::SyncAppendFile(char const*, char const*)` | masked | masked |  |
| `CAsyncOpenedFiles::~CAsyncOpenedFiles()` | masked | masked |  |
| `CThreadMutex::~CThreadMutex()` | masked | masked |  |
| `CAsyncOpenedFiles::FindOrAdd(char const*)` | masked | masked |  |
| `CAsyncOpenedFiles::Release(FSAsyncFile_t__*)` | masked | masked |  |
| `CAsyncOpenedFiles::Get(FSAsyncFile_t__*)` | masked | masked |  |
| `CUtlMemory<CFileAsyncReadJob*, int>::Grow(int)` | masked | masked |  |
| `CUtlMemory<IAsyncFileFetch*, int>::Grow(int)` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>::Node_t, unsigned short, CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>:` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>::Node_t, unsigned short, CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>:` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>::Node_t, unsigned short, CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>:` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>::Node_t, unsigned short, CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>:` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>::Node_t, unsigned short, CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>:` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>::Node_t, unsigned short, CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>:` | masked | masked |  |
| `AsyncOpenedFile_t::~AsyncOpenedFile_t()` | exact | exact |  |
| `AsyncOpenedFile_t::~AsyncOpenedFile_t()` | masked | masked |  |
| `CRefCountServiceBase<false, CRefMT>::OnFinalRelease()` | exact | exact |  |
| `CUtlRBTree<CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>::Node_t, unsigned short, CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>:` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>::Node_t, unsigned short, CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>:` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>::Node_t, unsigned short, CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>:` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>::Node_t, unsigned short, CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>:` | masked | masked |  |
| `CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>::Node_t, unsigned short>, unsigned short>::Grow(int)` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>::Node_t, unsigned short, CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>:` | masked | masked |  |
| `CFileAsyncAppendFileJob::CFileAsyncAppendFileJob(char const*, char const*)` | masked | masked |  |
| `CRefCounted1<IRefCounted, CRefCountServiceBase<true, CRefMT>>::AddRef()` | masked | masked |  |
| `CRefCounted1<IRefCounted, CRefCountServiceBase<true, CRefMT>>::Release()` | masked | masked |  |
| `CFileAsyncAppendFileJob::~CFileAsyncAppendFileJob()` | masked | masked |  |
| `CFileAsyncAppendFileJob::~CFileAsyncAppendFileJob()` | masked | masked |  |
| `CFileAsyncAppendFileJob::Describe()` | exact | exact |  |
| `CJob::SetDescription(char const*)` | masked | masked |  |
| `CFileAsyncAppendFileJob::DoExecute()` | masked | masked |  |
| `CJob::DoAbort(bool)` | exact | exact |  |
| `CJob::DoCleanup()` | exact | exact |  |
| `CFileAsyncJob::GetResult(void**, int*)` | exact | exact |  |
| `CFileAsyncAppendFileJob::IsWrite() const` | exact | exact |  |
| `non-virtual thunk to CFileAsyncAppendFileJob::~CFileAsyncAppendFileJob()` | masked | masked |  |
| `non-virtual thunk to CFileAsyncAppendFileJob::~CFileAsyncAppendFileJob()` | masked | masked |  |
| `CJob::~CJob()` | masked | masked |  |
| `CJob::~CJob()` | masked | masked |  |
| `CJob::Describe()` | masked | masked |  |
| `non-virtual thunk to CJob::~CJob()` | masked | masked |  |
| `non-virtual thunk to CJob::~CJob()` | masked | masked |  |
| `CJob::CJob(JobPriority_t)` | masked | masked |  |
| `CFileAsyncJob::~CFileAsyncJob()` | masked | masked |  |
| `CFileAsyncJob::~CFileAsyncJob()` | masked | masked |  |
| `CFileAsyncJob::IsWrite() const` | exact | exact |  |
| `non-virtual thunk to CFileAsyncJob::~CFileAsyncJob()` | masked | masked |  |
| `non-virtual thunk to CFileAsyncJob::~CFileAsyncJob()` | masked | masked |  |
| `CFileAsyncWriteFileJob::~CFileAsyncWriteFileJob()` | masked | masked |  |
| `CFileAsyncWriteFileJob::~CFileAsyncWriteFileJob()` | masked | masked |  |
| `CFileAsyncWriteJob::Describe()` | exact | exact |  |
| `CFileAsyncWriteJob::DoExecute()` | masked | masked |  |
| `CFileAsyncWriteFileJob::DoCleanup()` | masked | masked |  |
| `CFileAsyncWriteJob::IsWrite() const` | exact | exact |  |
| `non-virtual thunk to CFileAsyncWriteFileJob::~CFileAsyncWriteFileJob()` | masked | masked |  |
| `non-virtual thunk to CFileAsyncWriteFileJob::~CFileAsyncWriteFileJob()` | masked | masked |  |
| `CFileAsyncWriteJob::~CFileAsyncWriteJob()` | masked | masked |  |
| `CFileAsyncWriteJob::~CFileAsyncWriteJob()` | masked | masked |  |
| `CFileAsyncWriteJob::DoCleanup()` | masked | masked |  |
| `non-virtual thunk to CFileAsyncWriteJob::~CFileAsyncWriteJob()` | masked | masked |  |
| `non-virtual thunk to CFileAsyncWriteJob::~CFileAsyncWriteJob()` | masked | masked |  |
| `CFileAsyncReadJob::CFileAsyncReadJob(FileAsyncRequest_t const&, CBaseFileSystem*)` | masked | masked |  |
| `CFileAsyncFileSizeJob::~CFileAsyncFileSizeJob()` | masked | masked |  |
| `CFileAsyncFileSizeJob::~CFileAsyncFileSizeJob()` | masked | masked |  |
| `CFileAsyncReadJob::Describe()` | exact | exact |  |
| `CFileAsyncFileSizeJob::DoExecute()` | masked | masked |  |
| `CFileAsyncReadJob::GetResult(void**, int*)` | exact | exact |  |
| `non-virtual thunk to CFileAsyncFileSizeJob::~CFileAsyncFileSizeJob()` | masked | masked |  |
| `non-virtual thunk to CFileAsyncFileSizeJob::~CFileAsyncFileSizeJob()` | masked | masked |  |
| `CFileAsyncReadJob::~CFileAsyncReadJob()` | masked | masked |  |
| `CFileAsyncReadJob::~CFileAsyncReadJob()` | masked | masked |  |
| `CFileAsyncReadJob::~CFileAsyncReadJob()` | masked | masked |  |
| `CFileAsyncReadJob::DoExecute()` | masked | masked |  |
| `non-virtual thunk to CFileAsyncReadJob::~CFileAsyncReadJob()` | masked | masked |  |
| `non-virtual thunk to CFileAsyncReadJob::~CFileAsyncReadJob()` | masked | masked |  |
| `CFileAsyncReadJob::InterceptCallback(FileAsyncRequest_t const&, int, FSAsyncStatus_t)` | differs | masked |  |
| `CAsyncOpenedFiles::AddRef(FSAsyncFile_t__*)` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>::Node_t, unsigned short, CUtlMap<char const*, AsyncOpenedFile_t*, unsigned short>:` | masked | masked |  |
| `__GLOBAL__I_a` | masked | masked |  |

## filesystem_stdio (62/65, 65 with needed)

| function | status | with needed | note |
|---|---|---|---|
| `CFileSystem_Stdio::~CFileSystem_Stdio()` | masked | masked |  |
| `__CreateCFileSystem_StdioIFileSystem_interface()` | masked | masked |  |
| `__CreateCFileSystem_StdioIBaseFileSystem_interface()` | masked | masked |  |
| `UseOptimalBufferAllocation()` | masked | masked |  |
| `CFileSystem_Stdio::QueryInterface(char const*)` | masked | masked |  |
| `CFileSystem_Stdio::GetOptimalIOConstraints(void*, unsigned int*, unsigned int*, unsigned int*)` | masked | masked |  |
| `CFileSystem_Stdio::AllocOptimalReadBuffer(void*, unsigned int, unsigned int)` | masked | masked |  |
| `CFileSystem_Stdio::FreeOptimalReadBuffer(void*)` | masked | masked |  |
| `CFileSystem_Stdio::FS_fopen(char const*, char const*, unsigned int, long long*)` | masked | masked |  |
| `CStdioFile::FS_fopen(char const*, char const*, long long*)` | masked | masked |  |
| `CFileSystem_Stdio::FS_setbufsize(__sFILE*, unsigned int)` | exact | exact |  |
| `CFileSystem_Stdio::FS_fclose(__sFILE*)` | differs | masked |  |
| `CFileSystem_Stdio::FS_fseek(__sFILE*, long long, int)` | differs | masked |  |
| `CFileSystem_Stdio::FS_ftell(__sFILE*)` | exact | exact |  |
| `CFileSystem_Stdio::FS_feof(__sFILE*)` | exact | exact |  |
| `CFileSystem_Stdio::FS_fread(void*, unsigned long, unsigned long, __sFILE*)` | differs | masked |  |
| `CFileSystem_Stdio::FS_fwrite(void const*, unsigned long, __sFILE*)` | masked | masked |  |
| `CFileSystem_Stdio::FS_setmode(__sFILE*, FileMode_t)` | exact | exact |  |
| `CFileSystem_Stdio::FS_vfprintf(__sFILE*, char const*, char*)` | exact | exact |  |
| `CFileSystem_Stdio::FS_ferror(__sFILE*)` | exact | exact |  |
| `CFileSystem_Stdio::FS_fflush(__sFILE*)` | exact | exact |  |
| `CFileSystem_Stdio::FS_fgets(char*, int, __sFILE*)` | exact | exact |  |
| `CFileSystem_Stdio::FS_chmod(char const*, int)` | masked | masked |  |
| `CFileSystem_Stdio::FS_stat(char const*, stat*, bool*)` | masked | masked |  |
| `CFileSystem_Stdio::FS_FindFirstFile(char const*, FIND_DATA*)` | masked | masked |  |
| `CFileSystem_Stdio::FS_FindNextFile(void*, FIND_DATA*)` | masked | masked |  |
| `CFileSystem_Stdio::FS_FindClose(void*)` | masked | masked |  |
| `CFileSystem_Stdio::FS_GetSectorSize(__sFILE*)` | exact | exact |  |
| `CFileSystem_Stdio::IsFileImmediatelyAvailable(char const*)` | exact | exact |  |
| `CFileSystem_Stdio::WaitForResources(char const*)` | exact | exact |  |
| `CFileSystem_Stdio::GetWaitForResourcesProgress(int, float*, bool*)` | exact | exact |  |
| `CFileSystem_Stdio::CancelWaitForResources(int)` | exact | exact |  |
| `CFileSystem_Stdio::GetLocalCopy(char const*)` | exact | exact |  |
| `CFileSystem_Stdio::HintResourceNeed(char const*, int)` | exact | exact |  |
| `CStdioFile::FS_setbufsize(unsigned int)` | exact | exact |  |
| `CStdioFile::FS_fclose()` | masked | masked |  |
| `CStdioFile::FS_fseek(long long, int)` | masked | masked |  |
| `CStdioFile::FS_ftell()` | masked | masked |  |
| `CStdioFile::FS_feof()` | masked | masked |  |
| `CStdioFile::FS_fread(void*, unsigned long, unsigned long)` | masked | masked |  |
| `CStdioFile::FS_fwrite(void const*, unsigned long)` | masked | masked |  |
| `CStdioFile::FS_setmode(FileMode_t)` | exact | exact |  |
| `CStdioFile::FS_vfprintf(char const*, char*)` | masked | masked |  |
| `CStdioFile::FS_ferror()` | masked | masked |  |
| `CStdioFile::FS_fflush()` | masked | masked |  |
| `CStdioFile::FS_fgets(char*, int)` | masked | masked |  |
| `CUtlMap<int, CInterlockedIntT<int>, unsigned short>::~CUtlMap()` | masked | masked |  |
| `CStdioFile::~CStdioFile()` | exact | exact |  |
| `CStdioFile::~CStdioFile()` | masked | masked |  |
| `CStdFilesystemFile::FS_GetSectorSize()` | exact | exact |  |
| `CFileSystem_Stdio::IsSteam() const` | exact | exact |  |
| `CFileSystem_Stdio::MountSteamContent(int)` | exact | exact |  |
| `CUtlRBTree<CUtlMap<int, CInterlockedIntT<int>, unsigned short>::Node_t, unsigned short, CUtlMap<int, CInterlockedIntT<int>, unsigned short>::CKeyLess,` | masked | masked |  |
| `CUtlRBTree<CUtlMap<int, CInterlockedIntT<int>, unsigned short>::Node_t, unsigned short, CUtlMap<int, CInterlockedIntT<int>, unsigned short>::CKeyLess,` | masked | masked |  |
| `CUtlRBTree<CUtlMap<int, CInterlockedIntT<int>, unsigned short>::Node_t, unsigned short, CUtlMap<int, CInterlockedIntT<int>, unsigned short>::CKeyLess,` | masked | masked |  |
| `CUtlRBTree<CUtlMap<int, CInterlockedIntT<int>, unsigned short>::Node_t, unsigned short, CUtlMap<int, CInterlockedIntT<int>, unsigned short>::CKeyLess,` | masked | masked |  |
| `CUtlRBTree<CUtlMap<int, CInterlockedIntT<int>, unsigned short>::Node_t, unsigned short, CUtlMap<int, CInterlockedIntT<int>, unsigned short>::CKeyLess,` | masked | masked |  |
| `CUtlRBTree<CUtlMap<int, CInterlockedIntT<int>, unsigned short>::Node_t, unsigned short, CUtlMap<int, CInterlockedIntT<int>, unsigned short>::CKeyLess,` | masked | masked |  |
| `CUtlRBTree<CUtlMap<int, CInterlockedIntT<int>, unsigned short>::Node_t, unsigned short, CUtlMap<int, CInterlockedIntT<int>, unsigned short>::CKeyLess,` | masked | masked |  |
| `CUtlRBTree<CUtlMap<int, CInterlockedIntT<int>, unsigned short>::Node_t, unsigned short, CUtlMap<int, CInterlockedIntT<int>, unsigned short>::CKeyLess,` | masked | masked |  |
| `CUtlMemory<UtlRBTreeNode_t<CUtlMap<int, CInterlockedIntT<int>, unsigned short>::Node_t, unsigned short>, unsigned short>::Grow(int)` | masked | masked |  |
| `CUtlRBTree<CUtlMap<int, CInterlockedIntT<int>, unsigned short>::Node_t, unsigned short, CUtlMap<int, CInterlockedIntT<int>, unsigned short>::CKeyLess,` | masked | masked |  |
| `CDefOps<int>::LessFunc(int const&, int const&)` | exact | exact |  |
| `CUtlRBTree<CUtlMap<int, CInterlockedIntT<int>, unsigned short>::Node_t, unsigned short, CUtlMap<int, CInterlockedIntT<int>, unsigned short>::CKeyLess,` | masked | masked |  |
| `__GLOBAL__I_a` | masked | masked |  |

## filetracker (95/96, 95 with needed)

| function | status | with needed | note |
|---|---|---|---|
| `ThreadStubProcessMD5Requests(void*)` | exact | exact |  |
| `CFileTracker2::ThreadedProcessMD5Requests()` | masked | masked |  |
| `CFileTracker2::SubmitThreadedMD5Request(unsigned char*, int, int, int, int)` | masked | masked |  |
| `CFileTracker2::IsMD5RequestComplete(int, MD5Value_t*)` | masked | masked |  |
| `CFileTracker2::BlockUntilMD5RequestComplete(int, MD5Value_t*)` | masked | masked |  |
| `CFileTracker2::CFileTracker2(CBaseFileSystem*)` | masked | masked |  |
| `CFileTracker2::CFileTracker2(CBaseFileSystem*)` | exact | exact |  |
| `CFileTracker2::~CFileTracker2()` | masked | masked |  |
| `CFileTracker2::~CFileTracker2()` | exact | exact |  |
| `CFileTracker2::ShutdownAsync()` | masked | masked |  |
| `CFileTracker2::MarkAllCRCsUnverified()` | masked | masked |  |
| `CFileTracker2::GetUnverifiedFileHashes(CUnverifiedFileHash*, int)` | masked | masked |  |
| `CFileTracker2::CheckCachedFileHash(char const*, char const*, int, FileHash_t*)` | masked | masked |  |
| `TrackedFile_t::RebuildFileName(char const*, char const*)` | masked | masked |  |
| `TrackedFile_t::ProcessFileRead(void*, unsigned long)` | differs | differs | register allocation only (int64 position compares) |
| `CFileTracker2::NotePackFileAccess(char const*, char const*, int, CPackedStoreFileHandle&)` | masked | masked |  |
| `CFileTracker2::IdxFileFromName(char const*, char const*, int, long long, bool, bool)` | masked | masked |  |
| `CFileTracker2::GetFilesToUnloadForWhitelistChange(IPureServerWhitelist*)` | masked | masked |  |
| `CFileTracker2::AddFileHashForVPKFile(int, int, int, MD5Value_t&, CPackedStoreFileHandle&)` | masked | masked |  |
| `CFileTracker2::NotePackFileOpened(char const*, char const*, long long)` | masked | masked |  |
| `CFileTracker2::NoteFileIgnoredForPureServer(char const*, char const*, int)` | masked | masked |  |
| `CFileTracker2::NoteFileLoadedFromDisk(char const*, char const*, int, __sFILE*, long long)` | masked | masked |  |
| `CFileTracker2::NoteFileUnloaded(char const*, char const*)` | masked | masked |  |
| `CFileTracker2::RecordFileClose(__sFILE*)` | masked | masked |  |
| `CFileTracker2::RecordFileSeek(__sFILE*, long long, int)` | masked | masked |  |
| `CFileTracker2::RecordFileRead(void*, unsigned long, unsigned long, __sFILE*)` | masked | masked |  |
| `CUtlRBTree<TrackedVPKFile_t, int, bool (*)(TrackedVPKFile_t const&, TrackedVPKFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedVPKFile_t, int>, int>>` | masked | masked |  |
| `TrackedFile_t::Less(TrackedFile_t const&, TrackedFile_t const&)` | masked | masked |  |
| `CDefOps<__sFILE*>::LessFunc(__sFILE* const&, __sFILE* const&)` | exact | exact |  |
| `FileInVPK_t::Less(FileInVPK_t const&, FileInVPK_t const&)` | exact | exact |  |
| `TrackedVPKFile_t::Less(TrackedVPKFile_t const&, TrackedVPKFile_t const&)` | exact | exact |  |
| `CUtlLinkedList<int, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<int, unsigned short>, unsigned short>>::Purge()` | masked | masked |  |
| `CUtlRBTree<TrackedFile_t, int, bool (*)(TrackedFile_t const&, TrackedFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedFile_t, int>, int>>::FirstInord` | masked | masked |  |
| `CUtlRBTree<TrackedFile_t, int, bool (*)(TrackedFile_t const&, TrackedFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedFile_t, int>, int>>::NextInorde` | masked | masked |  |
| `CUtlRBTree<TrackedFile_t, int, bool (*)(TrackedFile_t const&, TrackedFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedFile_t, int>, int>>::Find(Track` | masked | masked |  |
| `CUtlRBTree<FileInVPK_t, int, bool (*)(FileInVPK_t const&, FileInVPK_t const&), CUtlMemory<UtlRBTreeNode_t<FileInVPK_t, int>, int>>::Find(FileInVPK_t c` | masked | masked |  |
| `CUtlRBTree<FileInVPK_t, int, bool (*)(FileInVPK_t const&, FileInVPK_t const&), CUtlMemory<UtlRBTreeNode_t<FileInVPK_t, int>, int>>::Insert(FileInVPK_t` | masked | masked |  |
| `CUtlRBTree<TrackedVPKFile_t, int, bool (*)(TrackedVPKFile_t const&, TrackedVPKFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedVPKFile_t, int>, int>>` | masked | masked |  |
| `FileListToUnloadForWhitelistChange::AddFile(char const*)` | masked | masked |  |
| `CUtlRBTree<TrackedFile_t, int, bool (*)(TrackedFile_t const&, TrackedFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedFile_t, int>, int>>::Insert(Tra` | masked | masked |  |
| `CUtlRBTree<TrackedFile_t, int, bool (*)(TrackedFile_t const&, TrackedFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedFile_t, int>, int>>::Reinsert(i` | masked | masked |  |
| `CUtlRBTree<CUtlMap<__sFILE*, int, int>::Node_t, int, CUtlMap<__sFILE*, int, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<__sFILE*, int, int>::No` | masked | masked |  |
| `CUtlRBTree<CUtlMap<__sFILE*, int, int>::Node_t, int, CUtlMap<__sFILE*, int, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<__sFILE*, int, int>::No` | masked | masked |  |
| `CUtlRBTree<CUtlMap<__sFILE*, int, int>::Node_t, int, CUtlMap<__sFILE*, int, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<__sFILE*, int, int>::No` | masked | masked |  |
| `CUtlRBTree<CUtlMap<__sFILE*, int, int>::Node_t, int, CUtlMap<__sFILE*, int, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<__sFILE*, int, int>::No` | masked | masked |  |
| `CUtlRBTree<CUtlMap<__sFILE*, int, int>::Node_t, int, CUtlMap<__sFILE*, int, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<__sFILE*, int, int>::No` | masked | masked |  |
| `CUtlRBTree<CUtlMap<__sFILE*, int, int>::Node_t, int, CUtlMap<__sFILE*, int, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<__sFILE*, int, int>::No` | masked | masked |  |
| `CUtlRBTree<CUtlMap<__sFILE*, int, int>::Node_t, int, CUtlMap<__sFILE*, int, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<__sFILE*, int, int>::No` | masked | masked |  |
| `CUtlRBTree<CUtlMap<__sFILE*, int, int>::Node_t, int, CUtlMap<__sFILE*, int, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<__sFILE*, int, int>::No` | masked | masked |  |
| `CUtlRBTree<CUtlMap<__sFILE*, int, int>::Node_t, int, CUtlMap<__sFILE*, int, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<__sFILE*, int, int>::No` | masked | masked |  |
| `CUtlRBTree<CUtlMap<__sFILE*, int, int>::Node_t, int, CUtlMap<__sFILE*, int, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<__sFILE*, int, int>::No` | masked | masked |  |
| `CUtlMemory<UtlRBTreeNode_t<CUtlMap<__sFILE*, int, int>::Node_t, int>, int>::Grow(int)` | masked | masked |  |
| `CUtlRBTree<TrackedFile_t, int, bool (*)(TrackedFile_t const&, TrackedFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedFile_t, int>, int>>::Unlink(int` | masked | masked |  |
| `CUtlRBTree<TrackedFile_t, int, bool (*)(TrackedFile_t const&, TrackedFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedFile_t, int>, int>>::FindInsert` | masked | masked |  |
| `CUtlRBTree<TrackedFile_t, int, bool (*)(TrackedFile_t const&, TrackedFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedFile_t, int>, int>>::InsertReba` | masked | masked |  |
| `CUtlRBTree<TrackedFile_t, int, bool (*)(TrackedFile_t const&, TrackedFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedFile_t, int>, int>>::IsLeftChil` | masked | masked |  |
| `CUtlRBTree<TrackedFile_t, int, bool (*)(TrackedFile_t const&, TrackedFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedFile_t, int>, int>>::IsRightChi` | masked | masked |  |
| `CUtlRBTree<TrackedFile_t, int, bool (*)(TrackedFile_t const&, TrackedFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedFile_t, int>, int>>::RotateLeft` | masked | masked |  |
| `CUtlRBTree<TrackedFile_t, int, bool (*)(TrackedFile_t const&, TrackedFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedFile_t, int>, int>>::RotateRigh` | masked | masked |  |
| `CUtlRBTree<TrackedFile_t, int, bool (*)(TrackedFile_t const&, TrackedFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedFile_t, int>, int>>::RemoveReba` | masked | masked |  |
| `CUtlRBTree<TrackedFile_t, int, bool (*)(TrackedFile_t const&, TrackedFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedFile_t, int>, int>>::NewNode()` | masked | masked |  |
| `CUtlMemory<UtlRBTreeNode_t<TrackedFile_t, int>, int>::Grow(int)` | masked | masked |  |
| `CUtlRBTree<TrackedVPKFile_t, int, bool (*)(TrackedVPKFile_t const&, TrackedVPKFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedVPKFile_t, int>, int>>` | masked | masked |  |
| `CUtlRBTree<TrackedVPKFile_t, int, bool (*)(TrackedVPKFile_t const&, TrackedVPKFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedVPKFile_t, int>, int>>` | masked | masked |  |
| `CUtlRBTree<TrackedVPKFile_t, int, bool (*)(TrackedVPKFile_t const&, TrackedVPKFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedVPKFile_t, int>, int>>` | masked | masked |  |
| `CUtlRBTree<TrackedVPKFile_t, int, bool (*)(TrackedVPKFile_t const&, TrackedVPKFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedVPKFile_t, int>, int>>` | masked | masked |  |
| `CUtlRBTree<TrackedVPKFile_t, int, bool (*)(TrackedVPKFile_t const&, TrackedVPKFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedVPKFile_t, int>, int>>` | masked | masked |  |
| `CUtlRBTree<TrackedVPKFile_t, int, bool (*)(TrackedVPKFile_t const&, TrackedVPKFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedVPKFile_t, int>, int>>` | masked | masked |  |
| `CUtlRBTree<TrackedVPKFile_t, int, bool (*)(TrackedVPKFile_t const&, TrackedVPKFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedVPKFile_t, int>, int>>` | masked | masked |  |
| `CUtlMemory<UtlRBTreeNode_t<TrackedVPKFile_t, int>, int>::Grow(int)` | masked | masked |  |
| `CUtlRBTree<FileInVPK_t, int, bool (*)(FileInVPK_t const&, FileInVPK_t const&), CUtlMemory<UtlRBTreeNode_t<FileInVPK_t, int>, int>>::FindInsertionPosit` | masked | masked |  |
| `CUtlRBTree<FileInVPK_t, int, bool (*)(FileInVPK_t const&, FileInVPK_t const&), CUtlMemory<UtlRBTreeNode_t<FileInVPK_t, int>, int>>::NewNode()` | masked | masked |  |
| `CUtlRBTree<FileInVPK_t, int, bool (*)(FileInVPK_t const&, FileInVPK_t const&), CUtlMemory<UtlRBTreeNode_t<FileInVPK_t, int>, int>>::InsertRebalance(in` | masked | masked |  |
| `CUtlRBTree<FileInVPK_t, int, bool (*)(FileInVPK_t const&, FileInVPK_t const&), CUtlMemory<UtlRBTreeNode_t<FileInVPK_t, int>, int>>::IsLeftChild(int) c` | masked | masked |  |
| `CUtlRBTree<FileInVPK_t, int, bool (*)(FileInVPK_t const&, FileInVPK_t const&), CUtlMemory<UtlRBTreeNode_t<FileInVPK_t, int>, int>>::IsRightChild(int) ` | masked | masked |  |
| `CUtlRBTree<FileInVPK_t, int, bool (*)(FileInVPK_t const&, FileInVPK_t const&), CUtlMemory<UtlRBTreeNode_t<FileInVPK_t, int>, int>>::RotateLeft(int)` | masked | masked |  |
| `CUtlRBTree<FileInVPK_t, int, bool (*)(FileInVPK_t const&, FileInVPK_t const&), CUtlMemory<UtlRBTreeNode_t<FileInVPK_t, int>, int>>::RotateRight(int)` | masked | masked |  |
| `CUtlMemory<UtlRBTreeNode_t<FileInVPK_t, int>, int>::Grow(int)` | masked | masked |  |
| `CUtlLinkedList<int, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<int, unsigned short>, unsigned short>>::AllocInternal(bool)` | masked | masked |  |
| `CUtlLinkedList<int, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<int, unsigned short>, unsigned short>>::LinkBefore(unsigned ` | exact | exact |  |
| `CUtlMemory<UtlLinkedListElem_t<int, unsigned short>, unsigned short>::Grow(int)` | masked | masked |  |
| `CUtlLinkedList<int, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<int, unsigned short>, unsigned short>>::LinkAfter(unsigned s` | exact | exact |  |
| `CTSQueue<StuffToMD5_t, false>::~CTSQueue()` | masked | masked |  |
| `CTSQueue<StuffToMD5_t, false>::Purge()` | masked | masked |  |
| `CTSQueue<StuffToMD5_t, false>::Pop()` | masked | masked |  |
| `CUtlRBTree<TrackedVPKFile_t, int, bool (*)(TrackedVPKFile_t const&, TrackedVPKFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedVPKFile_t, int>, int>>` | masked | masked |  |
| `CUtlRBTree<FileInVPK_t, int, bool (*)(FileInVPK_t const&, FileInVPK_t const&), CUtlMemory<UtlRBTreeNode_t<FileInVPK_t, int>, int>>::RemoveAll()` | masked | masked |  |
| `CUtlRBTree<TrackedFile_t, int, bool (*)(TrackedFile_t const&, TrackedFile_t const&), CUtlMemory<UtlRBTreeNode_t<TrackedFile_t, int>, int>>::RemoveAll(` | masked | masked |  |
| `CTSQueue<StuffToMD5_t, false>::CTSQueue()` | masked | masked |  |
| `CUtlLinkedList<SubmittedMd5Job_t, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<SubmittedMd5Job_t, unsigned short>, unsigned s` | masked | masked |  |
| `CUtlLinkedList<SubmittedMd5Job_t, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<SubmittedMd5Job_t, unsigned short>, unsigned s` | exact | exact |  |
| `CUtlMemory<UtlLinkedListElem_t<SubmittedMd5Job_t, unsigned short>, unsigned short>::Grow(int)` | masked | masked |  |
| `CTSQueue<StuffToMD5_t, false>::Push(CTSQueue<StuffToMD5_t, false>::Node_t*)` | masked | masked |  |
| `FileListToUnloadForWhitelistChange::IsFileInList(char const*)` | masked | masked |  |
| `FileListToUnloadForWhitelistChange::Release()` | masked | masked |  |
| `CUtlRBTree<CUtlMap<__sFILE*, int, int>::Node_t, int, CUtlMap<__sFILE*, int, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<__sFILE*, int, int>::No` | masked | masked |  |

## linux_support (5/5, 5 with needed)

| function | status | with needed | note |
|---|---|---|---|
| `FileSelect(dirent const*)` | masked | masked |  |
| `FillDataStruct(FIND_DATA*)` | masked | masked |  |
| `FindFirstFile(char const*, FIND_DATA*)` | masked | masked |  |
| `FindNextFile(void*, FIND_DATA*)` | masked | masked |  |
| `FindClose(void*)` | exact | exact |  |

## QueuedLoader (71/89, 71 with needed)

| function | status | with needed | note |
|---|---|---|---|
| `CQueuedLoader::~CQueuedLoader()` | masked | masked | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `__CreateCQueuedLoaderIQueuedLoader_interface()` | masked | masked | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `loader_dump_table(CCommand const&)` | masked | masked |  |
| `CQueuedLoader::CQueuedLoader()` | differs | differs | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `CQueuedLoader::~CQueuedLoader()` | differs | differs | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `CQueuedLoader::~CQueuedLoader()` | masked | masked | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `CQueuedLoader::BuildResources(IResourcePreload*, CUtlSortVector<void*, CQueuedLoader::CResourceNameLessFunc, CUtlVector<void*, CUtlMemory<void*, int>>` | masked | masked | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `CQueuedLoader::BuildMaterialResources(IResourcePreload*, CUtlSortVector<void*, CQueuedLoader::CResourceNameLessFunc, CUtlVector<void*, CUtlMemory<void` | masked | masked | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `AdjustAsyncIOSpeed()` | masked | masked |  |
| `CQueuedLoader::IsDynamic() const` | exact | exact | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `CQueuedLoader::GetSpewDetail() const` | masked | masked | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `IOComputationJob(FileJob_t*, void*, int, LoaderError_t)` | masked | masked |  |
| `CQueuedLoader::IsMapLoading() const` | exact | exact | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `FinishAnonymousJob(FileJob_t*, void (*)(void*, void*, void const*, int, LoaderError_t), void*, void*)` | masked | masked |  |
| `IOAsyncCallback(FileAsyncRequest_t const&, int, FSAsyncStatus_t)` | masked | masked |  |
| `CQueuedLoader::QueueDynamicLoadFunctor(CFunctor*)` | masked | masked | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `CQueuedLoader::SubmitPendingJobs()` | differs | differs | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `CQueuedLoader::CResourceNameLessFunc::Less(void* const&, void* const&, void*)` | masked | masked | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `CQueuedLoader::CFileJobsLessFunc::GetLayoutOrderForFilename(char const*)` | masked | masked | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `CQueuedLoader::CFileJobsLessFunc::Less(FileJob_t* const&, FileJob_t* const&, void*)` | masked | masked | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `CQueuedLoader::AddJob(LoaderJob_t const*)` | differs | differs | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `CQueuedLoader::AddMapResource(char const*)` | differs | differs | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `CQueuedLoader::ClaimAnonymousJob(char const*, void (*)(void*, void*, void const*, int, LoaderError_t), void*, void*)` | differs | differs | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `CQueuedLoader::ClaimAnonymousJob(char const*, void**, int*, LoaderError_t*)` | differs | differs | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `CQueuedLoader::SubmitBatchedJobs()` | differs | differs | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `CQueuedLoader::CleanQueue()` | differs | differs | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `CQueuedLoader::SpewInfo()` | differs | differs | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `CQueuedLoader::Init()` | masked | masked | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `CQueuedLoader::Shutdown()` | masked | masked | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `CQueuedLoader::InstallLoader(ResourcePreload_t, IResourcePreload*)` | differs | differs | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `CQueuedLoader::InstallProgress(ILoaderProgress*)` | differs | differs | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `CQueuedLoader::PurgeAll()` | differs | differs | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `CQueuedLoader::GetJobRequests()` | differs | differs | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `CQueuedLoader::AddResourceToTable(char const*)` | differs | differs | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `CQueuedLoader::BeginMapLoading(char const*, bool, bool)` | exact | exact | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `CQueuedLoader::EndMapLoading(bool)` | differs | differs | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `CQueuedLoader::IsSameMapLoading() const` | exact | exact | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `CQueuedLoader::IsFinished() const` | masked | masked | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `CQueuedLoader::IsBatching() const` | exact | exact | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `CQueuedLoader::DynamicLoadMapResource(char const*, void (*)(char const*, void*, void*), void*, void*)` | masked | masked | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `CQueuedLoader::CompleteDynamicLoad()` | masked | masked | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `CUtlSortVector<void*, CQueuedLoader::CResourceNameLessFunc, CUtlVector<void*, CUtlMemory<void*, int>>>::Remove(void* const&)` | differs | differs | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `CUtlDict<FileJob_t*, int>::Purge()` | masked | masked |  |
| `CTier2AppSystem<IQueuedLoader, 0>::Connect(void* (*)(char const*, int*))` | masked | masked |  |
| `CTier2AppSystem<IQueuedLoader, 0>::Disconnect()` | masked | masked |  |
| `CBaseAppSystem<IQueuedLoader>::QueryInterface(char const*)` | exact | exact |  |
| `CDummyProgress::BeginProgress()` | exact | exact |  |
| `CDummyProgress::EndProgress()` | exact | exact |  |
| `CDummyProgress::UpdateProgress(float)` | exact | exact |  |
| `CResourcePreloadAnonymous::CreateResource(char const*)` | masked | masked |  |
| `CResourcePreloadAnonymous::PurgeUnreferencedResources()` | exact | exact |  |
| `CResourcePreloadAnonymous::OnEndMapLoading(bool)` | exact | exact |  |
| `CResourcePreloadAnonymous::PurgeAll()` | exact | exact |  |
| `CUtlMemory<CFunctor*, int>::Grow(int)` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, FileJob_t*, int>::Node_t, int, CUtlMap<char const*, FileJob_t*, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<cha` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, FileJob_t*, int>::Node_t, int, CUtlMap<char const*, FileJob_t*, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<cha` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, FileJob_t*, int>::Node_t, int, CUtlMap<char const*, FileJob_t*, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<cha` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, FileJob_t*, int>::Node_t, int, CUtlMap<char const*, FileJob_t*, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<cha` | masked | masked |  |
| `CUtlMemory<void*, int>::Grow(int)` | masked | masked |  |
| `CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>::AddRef()` | masked | masked |  |
| `CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>::Release()` | masked | masked |  |
| `CFunctor4<void (*)(FileJob_t*, void (*)(void*, void*, void const*, int, LoaderError_t), void*, void*), FileJob_t*, void (*)(void*, void*, void const*,` | exact | exact |  |
| `CFunctor4<void (*)(FileJob_t*, void (*)(void*, void*, void const*, int, LoaderError_t), void*, void*), FileJob_t*, void (*)(void*, void*, void const*,` | masked | masked |  |
| `CFunctor4<void (*)(FileJob_t*, void (*)(void*, void*, void const*, int, LoaderError_t), void*, void*), FileJob_t*, void (*)(void*, void*, void const*,` | exact | exact |  |
| `non-virtual thunk to CFunctor4<void (*)(FileJob_t*, void (*)(void*, void*, void const*, int, LoaderError_t), void*, void*), FileJob_t*, void (*)(void*` | exact | exact |  |
| `non-virtual thunk to CFunctor4<void (*)(FileJob_t*, void (*)(void*, void*, void const*, int, LoaderError_t), void*, void*), FileJob_t*, void (*)(void*` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, FileJob_t*, int>::Node_t, int, CUtlMap<char const*, FileJob_t*, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<cha` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, FileJob_t*, int>::Node_t, int, CUtlMap<char const*, FileJob_t*, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<cha` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, FileJob_t*, int>::Node_t, int, CUtlMap<char const*, FileJob_t*, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<cha` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, FileJob_t*, int>::Node_t, int, CUtlMap<char const*, FileJob_t*, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<cha` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, FileJob_t*, int>::Node_t, int, CUtlMap<char const*, FileJob_t*, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<cha` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, FileJob_t*, int>::Node_t, int, CUtlMap<char const*, FileJob_t*, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<cha` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, FileJob_t*, int>::Node_t, int, CUtlMap<char const*, FileJob_t*, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<cha` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, FileJob_t*, int>::Node_t, int, CUtlMap<char const*, FileJob_t*, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<cha` | masked | masked |  |
| `CUtlRBTree<CUtlMap<char const*, FileJob_t*, int>::Node_t, int, CUtlMap<char const*, FileJob_t*, int>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<cha` | masked | masked |  |
| `CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, FileJob_t*, int>::Node_t, int>, int>::Grow(int)` | masked | masked |  |
| `CUtlLinkedList<FileJob_t*, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<FileJob_t*, unsigned short>, unsigned short>>::AllocI` | masked | masked |  |
| `CUtlLinkedList<FileJob_t*, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<FileJob_t*, unsigned short>, unsigned short>>::LinkBe` | exact | exact |  |
| `CUtlMemory<UtlLinkedListElem_t<FileJob_t*, unsigned short>, unsigned short>::Grow(int)` | masked | masked |  |
| `CUtlSortVector<FileJob_t*, CQueuedLoader::CFileJobsLessFunc, CUtlVector<FileJob_t*, CUtlMemory<FileJob_t*, int>>>::CompareHelper(FileJob_t* const*, Fi` | masked | masked | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `CFunctor4<void (*)(FileJob_t*, void*, int, LoaderError_t), FileJob_t*, void*, int, LoaderError_t, CRefCounted1<CFunctor, CRefCountServiceBase<true, CR` | exact | exact |  |
| `CFunctor4<void (*)(FileJob_t*, void*, int, LoaderError_t), FileJob_t*, void*, int, LoaderError_t, CRefCounted1<CFunctor, CRefCountServiceBase<true, CR` | masked | masked |  |
| `CFunctor4<void (*)(FileJob_t*, void*, int, LoaderError_t), FileJob_t*, void*, int, LoaderError_t, CRefCounted1<CFunctor, CRefCountServiceBase<true, CR` | exact | exact |  |
| `non-virtual thunk to CFunctor4<void (*)(FileJob_t*, void*, int, LoaderError_t), FileJob_t*, void*, int, LoaderError_t, CRefCounted1<CFunctor, CRefCoun` | exact | exact |  |
| `non-virtual thunk to CFunctor4<void (*)(FileJob_t*, void*, int, LoaderError_t), FileJob_t*, void*, int, LoaderError_t, CRefCounted1<CFunctor, CRefCoun` | masked | masked |  |
| `int CUtlSortVector<void*, CQueuedLoader::CResourceNameLessFunc, CUtlVector<void*, CUtlMemory<void*, int>>>::Find<void*>(void* const&) const` | differs | differs | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `CUtlSortVector<void*, CQueuedLoader::CResourceNameLessFunc, CUtlVector<void*, CUtlMemory<void*, int>>>::CompareHelper(void* const*, void* const*)` | masked | masked | IQueuedLoader/CQueuedLoader layout (headers lane) |
| `CTSList<FileJob_t*>::~CTSList()` | masked | masked |  |
| `__GLOBAL__I_a` | masked | masked |  |
