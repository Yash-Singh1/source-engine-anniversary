# datacache

Retail: depot 223 `bin/datacache.dylib`; own objects `studiobyteswap.o`,
`studio.o`, `studio_virtualmodel.o`, `datacache.o`, `mdlcache.o` (275
functions). tier1/tier2/tier3 members belong to the tier lanes.

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

Totals (matched / 275): orig 95, now 99, +hdr 265 (orig source +hdr: 124;
the original mdlcache.cpp does not compile against the SDK `CLZMA` API).
identical 212, edited 53, still differing 10.

Source changes on this branch: SDK 2013 `public/studio.cpp` and
`public/studio_virtualmodel.cpp`; `public/studio.h` uses the SDK 2013 layout on
32-bit (fork layout kept for `PLATFORM_64BITS`), no activity modifiers;
`common/studiobyteswap.cpp` datadescs follow; `mdlcache.cpp` drops the MDL v49
VTX pass, POSIX `Q_strlower`, the `g_AsyncInfoMap` mutex, the 32-bit
studiohdr2 injection, the "Empty model" check and the `GetStudioHdr` null
check; `CLZMA` is called through an instance (builds with either API).

| object | function | status | orig | now | +hdr | note |
|---|---|---|---|---|---|---|
| datacache | `__GLOBAL__I_a` | differs | no | no | no | static init; not investigated (convar/section globals) |
| datacache | `DataCacheSize_f(IConVar*, char const*, float)` | identical | yes | yes | yes |  |
| datacache | `__CreateCDataCacheIDataCache_interface()` | identical | yes | yes | yes |  |
| datacache | `CDataCache::AddSection(IDataCacheClient*, char const*, DataCacheLimits_t const&, bool)` | identical | yes | yes | yes |  |
| datacache | `CDataCache::Disconnect()` | identical | yes | yes | yes |  |
| datacache | `CDataCache::SetOptions(unsigned int)` | identical | yes | yes | yes |  |
| datacache | `CDataCache::FindSection(char const*)` | identical | yes | yes | yes |  |
| datacache | `CDataCache::OutputReport(DataCacheReportType_t, char const*)` | identical | no | no | yes |  |
| datacache | `CDataCache::RemoveSection(char const*, bool)` | identical | yes | yes | yes |  |
| datacache | `CDataCache::EnsureCapacity(unsigned int)` | identical | no | no | yes |  |
| datacache | `CDataCache::QueryInterface(char const*)` | identical | yes | yes | yes |  |
| datacache | `CDataCache::OutputItemReport(memhandle_t__*)` | identical | no | no | yes |  |
| datacache | `CDataCache::SetSectionLimits(char const*, DataCacheLimits_t const&)` | identical | yes | yes | yes |  |
| datacache | `CDataCache::SortMemhandlesBySizeLessFunc(memhandle_t__* const&, memhandle_t__* const&)` | identical | yes | yes | yes |  |
| datacache | `CDataCache::Init()` | identical | yes | yes | yes |  |
| datacache | `CDataCache::Flush(bool, bool)` | identical | no | no | yes |  |
| datacache | `CDataCache::Purge(unsigned int)` | identical | no | no | yes |  |
| datacache | `CDataCache::Connect(void* (*)(char const*, int*))` | identical | yes | yes | yes |  |
| datacache | `CDataCache::SetSize(int)` | identical | yes | yes | yes |  |
| datacache | `CDataCache::Shutdown()` | identical | yes | yes | yes |  |
| datacache | `CDataCache::GetStatus(DataCacheStatus_t*, DataCacheLimits_t*)` | identical | yes | yes | yes |  |
| datacache | `CDataCache::~CDataCache()` | identical | no | no | yes |  |
| datacache | `CUtlMemory<UtlRBTreeNode_t<memhandle_t__*, int>, int>::Grow(int)` | identical | no | no | yes |  |
| datacache | `CUtlMemory<CDataCacheSection*, int>::Grow(int)` | identical | no | no | yes |  |
| datacache | `CUtlRBTree<memhandle_t__*, int, bool (*)(memhandle_t__* const&, memhandle_t__* const&), CUtlMemory<UtlRBTreeNode_t<memhandle_t__*, int>, int>>::RotateLeft(int)` | identical | no | no | yes |  |
| datacache | `CUtlRBTree<memhandle_t__*, int, bool (*)(memhandle_t__* const&, memhandle_t__* const&), CUtlMemory<UtlRBTreeNode_t<memhandle_t__*, int>, int>>::RotateRight(int)` | identical | no | no | yes |  |
| datacache | `CUtlRBTree<memhandle_t__*, int, bool (*)(memhandle_t__* const&, memhandle_t__* const&), CUtlMemory<UtlRBTreeNode_t<memhandle_t__*, int>, int>>::InsertRebalance(int)` | identical | no | no | yes |  |
| datacache | `CUtlRBTree<memhandle_t__*, int, bool (*)(memhandle_t__* const&, memhandle_t__* const&), CUtlMemory<UtlRBTreeNode_t<memhandle_t__*, int>, int>>::FindInsertionPosition(memhandle_t__* const&, int&, bool&)` | identical | no | no | yes |  |
| datacache | `CUtlRBTree<memhandle_t__*, int, bool (*)(memhandle_t__* const&, memhandle_t__* const&), CUtlMemory<UtlRBTreeNode_t<memhandle_t__*, int>, int>>::InsertAt(int, bool)` | identical | no | no | yes |  |
| datacache | `CUtlRBTree<memhandle_t__*, int, bool (*)(memhandle_t__* const&, memhandle_t__* const&), CUtlMemory<UtlRBTreeNode_t<memhandle_t__*, int>, int>>::RemoveAll()` | identical | no | no | yes |  |
| datacache | `CDataManager<DataCacheItem_t, DataCacheItemData_t, DataCacheItem_t*, CThreadFastMutex>::GetRealSize(void*)` | identical | yes | yes | yes |  |
| datacache | `CDataManager<DataCacheItem_t, DataCacheItemData_t, DataCacheItem_t*, CThreadFastMutex>::DestroyResourceStorage(void*)` | identical | no | no | yes |  |
| datacache | `CDataManager<DataCacheItem_t, DataCacheItemData_t, DataCacheItem_t*, CThreadFastMutex>::Lock()` | identical | no | no | yes |  |
| datacache | `CDataManager<DataCacheItem_t, DataCacheItemData_t, DataCacheItem_t*, CThreadFastMutex>::Unlock()` | identical | no | no | yes |  |
| datacache | `CDataManager<DataCacheItem_t, DataCacheItemData_t, DataCacheItem_t*, CThreadFastMutex>::TryLock()` | identical | no | no | yes |  |
| datacache | `CDataManager<DataCacheItem_t, DataCacheItemData_t, DataCacheItem_t*, CThreadFastMutex>::~CDataManager()` | identical | no | no | yes |  |
| datacache | `CDataManager<DataCacheItem_t, DataCacheItemData_t, DataCacheItem_t*, CThreadFastMutex>::~CDataManager()` | identical | no | no | yes |  |
| datacache | `CUtlHashFast<memhandle_t__*, CUtlHashFastNoHash>::Init(int)` | identical | yes | yes | yes |  |
| datacache | `CUtlHashFast<memhandle_t__*, CUtlHashFastNoHash>::~CUtlHashFast()` | identical | yes | yes | yes |  |
| datacache | `CMemoryPoolMT::Free(void*)` | identical | no | no | yes |  |
| datacache | `CMemoryPoolMT::Alloc(unsigned long)` | identical | no | no | yes |  |
| datacache | `CMemoryPoolMT::~CMemoryPoolMT()` | identical | yes | yes | yes |  |
| datacache | `CUtlLinkedList<CUtlHashFast<memhandle_t__*, CUtlHashFastNoHash>::HashFastData_t_<memhandle_t__*>, int, true, int, CUtlFixedMemory<UtlLinkedListElem_t<CUtlHashFast<memhandle_t__*, CUtlHashFastNoHash>::HashFastData_t_<memhandle_t__*>, int>>>::AllocInternal(bool)` | identical | no | no | yes |  |
| datacache | `CUtlLinkedList<CUtlHashFast<memhandle_t__*, CUtlHashFastNoHash>::HashFastData_t_<memhandle_t__*>, int, true, int, CUtlFixedMemory<UtlLinkedListElem_t<CUtlHashFast<memhandle_t__*, CUtlHashFastNoHash>::HashFastData_t_<memhandle_t__*>, int>>>::Unlink(int)` | identical | yes | yes | yes |  |
| datacache | `CUtlLinkedList<CUtlHashFast<memhandle_t__*, CUtlHashFastNoHash>::HashFastData_t_<memhandle_t__*>, int, true, int, CUtlFixedMemory<UtlLinkedListElem_t<CUtlHashFast<memhandle_t__*, CUtlHashFastNoHash>::HashFastData_t_<memhandle_t__*>, int>>>::RemoveAll()` | identical | yes | yes | yes |  |
| datacache | `CDataCacheSection::GetNoTouch(memhandle_t__*, bool)` | identical | no | no | yes |  |
| datacache | `CDataCacheSection::PurgeItems(unsigned int)` | identical | no | no | yes |  |
| datacache | `CDataCacheSection::SetOptions(unsigned int)` | identical | yes | yes | yes |  |
| datacache | `CDataCacheSection::UpdateSize(memhandle_t__*, unsigned int)` | identical | no | no | yes |  |
| datacache | `CDataCacheSection::DiscardItem(memhandle_t__*, DataCacheNotificationType_t)` | identical | no | no | yes |  |
| datacache | `CDataCacheSection::GetNextItem(memhandle_t__*)` | identical | yes | yes | yes |  |
| datacache | `CDataCacheSection::UnlockMutex()` | identical | no | no | yes |  |
| datacache | `CDataCacheSection::GetLockCount(memhandle_t__*)` | identical | yes | yes | yes |  |
| datacache | `CDataCacheSection::OutputReport(DataCacheReportType_t)` | identical | yes | yes | yes |  |
| datacache | `CDataCacheSection::EnsureCapacity(unsigned int, unsigned int)` | identical | no | no | yes |  |
| datacache | `CDataCacheSection::GetSharedCache()` | identical | yes | yes | yes |  |
| datacache | `CDataCacheSection::IsFrameLocking()` | identical | no | no | yes |  |
| datacache | `CDataCacheSection::DiscardItemData(DataCacheItem_t*, DataCacheNotificationType_t)` | identical | no | no | yes |  |
| datacache | `CDataCacheSection::EndFrameLocking()` | identical | no | no | yes |  |
| datacache | `CDataCacheSection::BeginFrameLocking()` | identical | no | no | yes |  |
| datacache | `CDataCacheSection::GetFirstLockedItem()` | identical | yes | yes | yes |  |
| datacache | `CDataCacheSection::GetFirstUnlockedItem()` | identical | yes | yes | yes |  |
| datacache | `CDataCacheSection::GetFrameUnlockCounterPtr()` | identical | yes | yes | yes |  |
| datacache | `CDataCacheSection::Add(unsigned int, void const*, unsigned int, memhandle_t__**)` | identical | yes | yes | yes |  |
| datacache | `CDataCacheSection::Age(memhandle_t__*)` | identical | yes | yes | yes |  |
| datacache | `CDataCacheSection::Get(memhandle_t__*, bool)` | identical | no | no | yes |  |
| datacache | `CDataCacheSection::Find(unsigned int)` | identical | no | no | yes |  |
| datacache | `CDataCacheSection::Lock(memhandle_t__*)` | identical | no | no | yes |  |
| datacache | `CDataCacheSection::AddEx(unsigned int, void const*, unsigned int, unsigned int, memhandle_t__**)` | identical | no | no | yes |  |
| datacache | `CDataCacheSection::Flush(bool, bool)` | identical | no | no | yes |  |
| datacache | `CDataCacheSection::OnAdd(unsigned int, memhandle_t__*)` | identical | yes | yes | yes |  |
| datacache | `CDataCacheSection::Purge(unsigned int)` | differs | no | no | no | std::min operand order: fork minmax.h uses std::min, retail the SDK macro (tier0 lane) |
| datacache | `CDataCacheSection::Touch(memhandle_t__*)` | identical | yes | yes | yes |  |
| datacache | `CDataCacheSection::DoFind(unsigned int)` | identical | no | no | yes |  |
| datacache | `CDataCacheSection::Remove(memhandle_t__*, void const**, unsigned int*, bool)` | identical | no | no | yes |  |
| datacache | `CDataCacheSection::Unlock(memhandle_t__*)` | identical | no | no | yes |  |
| datacache | `CDataCacheSection::GetName()` | identical | yes | yes | yes |  |
| datacache | `CDataCacheSection::OnRemove(unsigned int)` | identical | yes | yes | yes |  |
| datacache | `CDataCacheSection::BreakLock(memhandle_t__*)` | identical | yes | yes | yes |  |
| datacache | `CDataCacheSection::FrameLock(memhandle_t__*)` | identical | no | no | yes |  |
| datacache | `CDataCacheSection::GetStatus(DataCacheStatus_t*, DataCacheLimits_t*)` | identical | yes | yes | yes |  |
| datacache | `CDataCacheSection::IsPresent(memhandle_t__*)` | identical | yes | yes | yes |  |
| datacache | `CDataCacheSection::LockMutex()` | identical | no | no | yes |  |
| datacache | `CDataCacheSection::SetLimits(DataCacheLimits_t const&)` | identical | yes | yes | yes |  |
| datacache | `CDataCacheSection::CDataCacheSection(CDataCache*, IDataCacheClient*, char const*)` | differs | no | no | no | inlined CThreadFastMutex spin passes an extra 0 in retail (tier0 threadtools.h) |
| datacache | `CDataCacheSection::~CDataCacheSection()` | identical | no | no | yes |  |
| datacache | `CDataCacheSectionFastFind::OnAdd(unsigned int, memhandle_t__*)` | identical | no | no | yes |  |
| datacache | `CDataCacheSectionFastFind::DoFind(unsigned int)` | identical | no | no | yes |  |
| datacache | `CDataCacheSectionFastFind::OnRemove(unsigned int)` | identical | no | no | yes |  |
| datacache | `CDataCacheSectionFastFind::CDataCacheSectionFastFind(CDataCache*, IDataCacheClient*, char const*)` | identical | yes | yes | yes |  |
| datacache | `CFmtStrN<256, false>::InitQuietTruncation()` | identical | yes | yes | yes |  |
| datacache | `CFmtStrN<256, false>::CFmtStrN(char const*, ...)` | identical | yes | yes | yes |  |
| datacache | `CUtlRBTree<memhandle_t__*, int, bool (*)(memhandle_t__* const&, memhandle_t__* const&), CUtlMemory<UtlRBTreeNode_t<memhandle_t__*, int>, int>>::IsLeftChild(int) const` | identical | no | no | yes |  |
| datacache | `CUtlRBTree<memhandle_t__*, int, bool (*)(memhandle_t__* const&, memhandle_t__* const&), CUtlMemory<UtlRBTreeNode_t<memhandle_t__*, int>, int>>::NextInorder(int) const` | identical | no | no | yes |  |
| datacache | `CUtlRBTree<memhandle_t__*, int, bool (*)(memhandle_t__* const&, memhandle_t__* const&), CUtlMemory<UtlRBTreeNode_t<memhandle_t__*, int>, int>>::FirstInorder() const` | identical | no | no | yes |  |
| datacache | `CUtlRBTree<memhandle_t__*, int, bool (*)(memhandle_t__* const&, memhandle_t__* const&), CUtlMemory<UtlRBTreeNode_t<memhandle_t__*, int>, int>>::IsRightChild(int) const` | identical | no | no | yes |  |
| mdlcache | `__GLOBAL__I_a` | differs | no | no | no | static init; not investigated |
| mdlcache | `ReleaseMaterialSystemObjects()` | identical | yes | yes | yes |  |
| mdlcache | `RestoreMaterialSystemObjects(int)` | identical | yes | yes | yes |  |
| mdlcache | `__CreateCMDLCacheIMDLCache_interface()` | identical | yes | yes | yes |  |
| mdlcache | `__CreateCMDLCacheIStudioDataCache_interface()` | identical | yes | yes | yes |  |
| mdlcache | `CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, studiodata_t*, unsigned short>::Node_t, unsigned short>, unsigned short>::Grow(int)` | identical | no | no | yes |  |
| mdlcache | `CUtlMemory<UtlRBTreeNode_t<CUtlMap<int, int, unsigned short>::Node_t, unsigned short>, unsigned short>::Grow(int)` | identical | no | no | yes |  |
| mdlcache | `CUtlMemory<IMaterial*, int>::Grow(int)` | identical | no | no | yes |  |
| mdlcache | `CUtlRBTree<CUtlMap<char const*, studiodata_t*, unsigned short>::Node_t, unsigned short, CUtlMap<char const*, studiodata_t*, unsigned short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, studiodata_t*, unsigned short>::Node_t, unsigned short>, unsigned short>>::RotateLeft(unsigned short)` | identical | no | no | yes |  |
| mdlcache | `CUtlRBTree<CUtlMap<char const*, studiodata_t*, unsigned short>::Node_t, unsigned short, CUtlMap<char const*, studiodata_t*, unsigned short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, studiodata_t*, unsigned short>::Node_t, unsigned short>, unsigned short>>::RotateRight(unsigned short)` | identical | no | no | yes |  |
| mdlcache | `CUtlRBTree<CUtlMap<char const*, studiodata_t*, unsigned short>::Node_t, unsigned short, CUtlMap<char const*, studiodata_t*, unsigned short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, studiodata_t*, unsigned short>::Node_t, unsigned short>, unsigned short>>::InsertRebalance(unsigned short)` | identical | no | no | yes |  |
| mdlcache | `CUtlRBTree<CUtlMap<char const*, studiodata_t*, unsigned short>::Node_t, unsigned short, CUtlMap<char const*, studiodata_t*, unsigned short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, studiodata_t*, unsigned short>::Node_t, unsigned short>, unsigned short>>::RemoveRebalance(unsigned short)` | identical | no | no | yes |  |
| mdlcache | `CUtlRBTree<CUtlMap<char const*, studiodata_t*, unsigned short>::Node_t, unsigned short, CUtlMap<char const*, studiodata_t*, unsigned short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, studiodata_t*, unsigned short>::Node_t, unsigned short>, unsigned short>>::FindInsertionPosition(CUtlMap<char const*, studiodata_t*, unsigned short>::Node_t const&, unsigned short&, bool&)` | identical | no | no | yes |  |
| mdlcache | `CUtlRBTree<CUtlMap<char const*, studiodata_t*, unsigned short>::Node_t, unsigned short, CUtlMap<char const*, studiodata_t*, unsigned short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, studiodata_t*, unsigned short>::Node_t, unsigned short>, unsigned short>>::Insert(CUtlMap<char const*, studiodata_t*, unsigned short>::Node_t const&)` | identical | no | no | yes |  |
| mdlcache | `CUtlRBTree<CUtlMap<char const*, studiodata_t*, unsigned short>::Node_t, unsigned short, CUtlMap<char const*, studiodata_t*, unsigned short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, studiodata_t*, unsigned short>::Node_t, unsigned short>, unsigned short>>::Unlink(unsigned short)` | identical | no | no | yes |  |
| mdlcache | `CUtlRBTree<CUtlMap<char const*, studiodata_t*, unsigned short>::Node_t, unsigned short, CUtlMap<char const*, studiodata_t*, unsigned short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, studiodata_t*, unsigned short>::Node_t, unsigned short>, unsigned short>>::NewNode()` | identical | no | no | yes |  |
| mdlcache | `CUtlRBTree<CUtlMap<char const*, studiodata_t*, unsigned short>::Node_t, unsigned short, CUtlMap<char const*, studiodata_t*, unsigned short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, studiodata_t*, unsigned short>::Node_t, unsigned short>, unsigned short>>::RemoveAll()` | identical | no | no | yes |  |
| mdlcache | `CUtlRBTree<CUtlMap<int, int, unsigned short>::Node_t, unsigned short, CUtlMap<int, int, unsigned short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<int, int, unsigned short>::Node_t, unsigned short>, unsigned short>>::RotateLeft(unsigned short)` | identical | no | no | yes |  |
| mdlcache | `CUtlRBTree<CUtlMap<int, int, unsigned short>::Node_t, unsigned short, CUtlMap<int, int, unsigned short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<int, int, unsigned short>::Node_t, unsigned short>, unsigned short>>::RotateRight(unsigned short)` | identical | no | no | yes |  |
| mdlcache | `CUtlRBTree<CUtlMap<int, int, unsigned short>::Node_t, unsigned short, CUtlMap<int, int, unsigned short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<int, int, unsigned short>::Node_t, unsigned short>, unsigned short>>::InsertRebalance(unsigned short)` | identical | no | no | yes |  |
| mdlcache | `CUtlRBTree<CUtlMap<int, int, unsigned short>::Node_t, unsigned short, CUtlMap<int, int, unsigned short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<int, int, unsigned short>::Node_t, unsigned short>, unsigned short>>::RemoveRebalance(unsigned short)` | identical | no | no | yes |  |
| mdlcache | `CUtlRBTree<CUtlMap<int, int, unsigned short>::Node_t, unsigned short, CUtlMap<int, int, unsigned short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<int, int, unsigned short>::Node_t, unsigned short>, unsigned short>>::FindInsertionPosition(CUtlMap<int, int, unsigned short>::Node_t const&, unsigned short&, bool&)` | identical | no | no | yes |  |
| mdlcache | `CUtlRBTree<CUtlMap<int, int, unsigned short>::Node_t, unsigned short, CUtlMap<int, int, unsigned short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<int, int, unsigned short>::Node_t, unsigned short>, unsigned short>>::Insert(CUtlMap<int, int, unsigned short>::Node_t const&)` | identical | no | no | yes |  |
| mdlcache | `CUtlRBTree<CUtlMap<int, int, unsigned short>::Node_t, unsigned short, CUtlMap<int, int, unsigned short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<int, int, unsigned short>::Node_t, unsigned short>, unsigned short>>::Unlink(unsigned short)` | identical | no | no | yes |  |
| mdlcache | `CUtlRBTree<CUtlMap<int, int, unsigned short>::Node_t, unsigned short, CUtlMap<int, int, unsigned short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<int, int, unsigned short>::Node_t, unsigned short>, unsigned short>>::NewNode()` | identical | no | no | yes |  |
| mdlcache | `CUtlRBTree<CUtlMap<int, int, unsigned short>::Node_t, unsigned short, CUtlMap<int, int, unsigned short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<int, int, unsigned short>::Node_t, unsigned short>, unsigned short>>::RemoveAll()` | identical | no | no | yes |  |
| mdlcache | `CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>::AddRef()` | identical | no | no | yes |  |
| mdlcache | `CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>::Release()` | identical | no | no | yes |  |
| mdlcache | `ModelParts_t::ModelParts_t()` | identical | no | no | yes |  |
| mdlcache | `CUtlLinkedList<AsyncInfo_t, int, true, int, CUtlFixedMemory<UtlLinkedListElem_t<AsyncInfo_t, int>>>::AllocInternal(bool)` | identical | no | no | yes |  |
| mdlcache | `CUtlLinkedList<AsyncInfo_t, int, true, int, CUtlFixedMemory<UtlLinkedListElem_t<AsyncInfo_t, int>>>::Unlink(int)` | identical | yes | yes | yes |  |
| mdlcache | `CUtlLinkedList<AsyncInfo_t, int, true, int, CUtlFixedMemory<UtlLinkedListElem_t<AsyncInfo_t, int>>>::RemoveAll()` | identical | yes | yes | yes |  |
| mdlcache | `virtualgroup_t::~virtualgroup_t()` | identical | yes | yes | yes |  |
| mdlcache | `virtualmodel_t::~virtualmodel_t()` | identical | yes | yes | yes |  |
| mdlcache | `mstudiovertanim_t::ConvertToFixed(float)` | identical | yes | yes | yes |  |
| mdlcache | `CRefCountServiceBase<true, CRefMT>::OnFinalRelease()` | identical | yes | yes | yes |  |
| mdlcache | `CDefOps<int>::LessFunc(int const&, int const&)` | identical | yes | yes | yes |  |
| mdlcache | `CUtlMap<int, int, unsigned short>::~CUtlMap()` | identical | no | no | yes |  |
| mdlcache | `CUtlDict<studiodata_t*, unsigned short>::Purge()` | identical | no | no | yes |  |
| mdlcache | `CFunctor1<void (*)(ModelParts_t*), ModelParts_t*, CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>>::~CFunctor1()` | identical | yes | yes | yes |  |
| mdlcache | `CFunctor1<void (*)(ModelParts_t*), ModelParts_t*, CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>>::~CFunctor1()` | identical | yes | yes | yes |  |
| mdlcache | `CFunctor1<void (*)(ModelParts_t*), ModelParts_t*, CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>>::operator()()` | identical | yes | yes | yes |  |
| mdlcache | `CMDLCache::ClearAsync(unsigned short, MDLCacheDataType_t, int, bool)` | edited | no | no | yes | inlined Get/SetAsyncInfoIndex without the fork's g_AsyncInfoMapMutex |
| mdlcache | `CMDLCache::Disconnect()` | identical | yes | yes | yes |  |
| mdlcache | `CMDLCache::EndMapLoad()` | identical | no | no | yes |  |
| mdlcache | `CMDLCache::GetItemName(unsigned int, void const*, char*, unsigned int)` | identical | yes | yes | yes |  |
| mdlcache | `CMDLCache::GetUserData(unsigned short)` | identical | no | no | yes |  |
| mdlcache | `CMDLCache::GetVCollide(unsigned short)` | identical | yes | yes | yes |  |
| mdlcache | `CMDLCache::ReadMDLFile(unsigned short, char const*, CUtlBuffer&)` | edited | no | no | yes | removed POSIX Q_strlower; inlined ReadFileNative no longer grows the buffer for a studiohdr2_t on 32-bit |
| mdlcache | `CMDLCache::SetUserData(unsigned short, void*)` | identical | no | no | yes |  |
| mdlcache | `CMDLCache::BeginMapLoad()` | identical | no | no | yes |  |
| mdlcache | `CMDLCache::GetAnimBlock(unsigned short, int)` | identical | no | no | yes |  |
| mdlcache | `CMDLCache::GetAsyncLoad(MDLCacheDataType_t)` | identical | yes | yes | yes |  |
| mdlcache | `CMDLCache::GetModelName(unsigned short)` | identical | no | no | yes |  |
| mdlcache | `CMDLCache::GetStudioHdr(unsigned short)` | edited | no | no | yes | no null studiodata_t check (fork addition) |
| mdlcache | `CMDLCache::IsDataLoaded(unsigned short, MDLCacheDataType_t)` | identical | no | no | yes |  |
| mdlcache | `CMDLCache::IsErrorModel(unsigned short)` | identical | no | no | yes |  |
| mdlcache | `CMDLCache::MarkAsLoaded(unsigned short)` | identical | no | no | yes |  |
| mdlcache | `CMDLCache::PreloadModel(unsigned short)` | differs | no | no | no | same size, register allocation; not investigated |
| mdlcache | `CMDLCache::SetAsyncLoad(MDLCacheDataType_t, bool)` | identical | yes | yes | yes |  |
| mdlcache | `CMDLCache::TouchAllData(unsigned short)` | edited | no | yes | yes | child handle via (MDLHandle_t)(intp)cache&0xffff |
| mdlcache | `CMDLCache::GetVCollideEx(unsigned short, bool)` | identical | no | no | yes |  |
| mdlcache | `CMDLCache::GetVertexData(unsigned short)` | identical | yes | yes | yes |  |
| mdlcache | `CMDLCache::LockStudioHdr(unsigned short)` | identical | no | no | yes |  |
| mdlcache | `CMDLCache::VerifyHeaders(studiohdr_t*)` | identical | no | no | yes |  |
| mdlcache | `CMDLCache::FreeAnimBlocks(unsigned short)` | identical | no | no | yes |  |
| mdlcache | `CMDLCache::LoadVertexData(studiohdr_t*)` | edited | no | no | yes | CLZMA through an instance; inlined MakeFilename without Q_strlower |
| mdlcache | `CMDLCache::QueryInterface(char const*)` | identical | yes | yes | yes |  |
| mdlcache | `CMDLCache::SetCacheNotify(IMDLCacheNotify*)` | identical | yes | yes | yes |  |
| mdlcache | `CMDLCache::UnserializeMDL(unsigned short, void*, int, bool)` | edited | no | no | yes | removed the fork's 'Empty model' size check; CLZMA through an instance |
| mdlcache | `CMDLCache::CacheVertexData(studiohdr_t*)` | edited | no | no | yes | inlined async index helpers without the fork's mutex |
| mdlcache | `CMDLCache::ConvertFlexData(studiohdr_t*)` | identical | yes | yes | yes |  |
| mdlcache | `CMDLCache::GetAutoplayList(unsigned short, unsigned short**)` | identical | no | no | yes |  |
| mdlcache | `CMDLCache::GetHardwareData(unsigned short)` | identical | no | no | yes |  |
| mdlcache | `CMDLCache::GetVCollideSize(unsigned short, int*)` | identical | no | no | yes |  |
| mdlcache | `CMDLCache::GetVirtualModel(unsigned short)` | identical | yes | yes | yes |  |
| mdlcache | `CMDLCache::InitPreloadData(bool)` | identical | yes | yes | yes |  |
| mdlcache | `CMDLCache::UnlockStudioHdr(unsigned short)` | identical | no | no | yes |  |
| mdlcache | `CMDLCache::FreeVirtualModel(unsigned short)` | identical | no | no | yes |  |
| mdlcache | `CMDLCache::LoadHardwareData(unsigned short)` | edited | no | no | yes | inlined Get/SetAsyncInfoIndex without the fork's g_AsyncInfoMapMutex |
| mdlcache | `CMDLCache::BuildHardwareData(unsigned short, studiodata_t*, studiohdr_t*, OptimizedModel::FileHeader_t*)` | edited | no | yes | yes | removed the fork's version-49 VTX flag pass; CLZMA through an instance |
| mdlcache | `CMDLCache::ProcessQueuedData(ModelParts_t*, bool)` | identical | no | no | yes |  |
| mdlcache | `CMDLCache::FinishPendingLoads()` | identical | no | no | yes |  |
| mdlcache | `CMDLCache::NotifyFileUnloaded(unsigned short, char const*)` | identical | no | no | yes |  |
| mdlcache | `CMDLCache::ProcessDynamicLoad(ModelParts_t*)` | differs | no | no | no | not investigated |
| mdlcache | `CMDLCache::UnloadHardwareData(unsigned short, bool, bool)` | identical | no | no | yes |  |
| mdlcache | `CMDLCache::GetVirtualModelFast(studiohdr_t const*, unsigned short)` | identical | no | no | yes |  |
| mdlcache | `CMDLCache::ProcessPendingAsync(int)` | edited | no | no | yes | inlined SetAsyncInfoIndex without the fork's mutex |
| mdlcache | `CMDLCache::ShutdownPreloadData()` | identical | yes | yes | yes |  |
| mdlcache | `CMDLCache::UnserializeVCollide(unsigned short, bool)` | edited | no | no | yes | inlined async index helpers without the fork's mutex |
| mdlcache | `CMDLCache::ProcessDataIntoCache(unsigned short, MDLCacheDataType_t, int, void*, int, bool)` | differs | no | no | no | CLZMA through an instance |
| mdlcache | `CMDLCache::ProcessPendingAsyncs(MDLCacheDataType_t)` | edited | no | no | yes | inlined SetAsyncInfoIndex without the fork's mutex |
| mdlcache | `CMDLCache::UnserializeAnimBlock(unsigned short, int)` | edited | no | no | yes | removed POSIX Q_strlower |
| mdlcache | `CMDLCache::ResetErrorModelStatus(unsigned short)` | identical | no | no | yes |  |
| mdlcache | `CMDLCache::BuildAndCacheVertexData(studiohdr_t*, vertexFileHeader_t*)` | differs | no | no | no | std::min operand order (minmax.h) |
| mdlcache | `CMDLCache::HandleCacheNotification(DataCacheNotification_t const&)` | identical | yes | yes | yes |  |
| mdlcache | `CMDLCache::GetFrameUnlockCounterPtr(MDLCacheDataType_t)` | identical | yes | yes | yes |  |
| mdlcache | `CMDLCache::QueuedLoaderCallback_MDL(void*, void*, void const*, int, LoaderError_t)` | identical | no | no | yes |  |
| mdlcache | `CMDLCache::GetFrameUnlockCounterPtrOLD()` | identical | yes | yes | yes |  |
| mdlcache | `CMDLCache::ReleaseMaterialSystemObjects()` | identical | yes | yes | yes |  |
| mdlcache | `CMDLCache::RestoreMaterialSystemObjects(int)` | identical | no | no | yes |  |
| mdlcache | `CMDLCache::Init()` | identical | yes | yes | yes |  |
| mdlcache | `CMDLCache::Flush(MDLCacheFlush_t)` | identical | yes | yes | yes |  |
| mdlcache | `CMDLCache::Flush(unsigned short, int)` | identical | no | no | yes |  |
| mdlcache | `CMDLCache::AddRef(unsigned short)` | identical | no | no | yes |  |
| mdlcache | `CMDLCache::GetRef(unsigned short)` | identical | no | no | yes |  |
| mdlcache | `CMDLCache::Connect(void* (*)(char const*, int*))` | edited | no | yes | yes | no null check around AddReleaseFunc/AddRestoreFunc |
| mdlcache | `CMDLCache::EndLock()` | identical | yes | yes | yes |  |
| mdlcache | `CMDLCache::FindMDL(char const*)` | identical | no | no | yes |  |
| mdlcache | `CMDLCache::Release(unsigned short)` | identical | no | no | yes |  |
| mdlcache | `CMDLCache::Shutdown()` | identical | no | no | yes |  |
| mdlcache | `CMDLCache::BeginLock()` | identical | yes | yes | yes |  |
| mdlcache | `CMDLCache::MarkFrame()` | identical | yes | yes | yes |  |
| mdlcache | `CMDLCache::~CMDLCache()` | identical | no | no | yes |  |
| mdlcache | `CUtlRBTree<CUtlMap<char const*, studiodata_t*, unsigned short>::Node_t, unsigned short, CUtlMap<char const*, studiodata_t*, unsigned short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, studiodata_t*, unsigned short>::Node_t, unsigned short>, unsigned short>>::IsLeftChild(unsigned short) const` | identical | no | no | yes |  |
| mdlcache | `CUtlRBTree<CUtlMap<char const*, studiodata_t*, unsigned short>::Node_t, unsigned short, CUtlMap<char const*, studiodata_t*, unsigned short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, studiodata_t*, unsigned short>::Node_t, unsigned short>, unsigned short>>::NextInorder(unsigned short) const` | identical | no | no | yes |  |
| mdlcache | `CUtlRBTree<CUtlMap<char const*, studiodata_t*, unsigned short>::Node_t, unsigned short, CUtlMap<char const*, studiodata_t*, unsigned short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, studiodata_t*, unsigned short>::Node_t, unsigned short>, unsigned short>>::FirstInorder() const` | identical | no | no | yes |  |
| mdlcache | `CUtlRBTree<CUtlMap<char const*, studiodata_t*, unsigned short>::Node_t, unsigned short, CUtlMap<char const*, studiodata_t*, unsigned short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, studiodata_t*, unsigned short>::Node_t, unsigned short>, unsigned short>>::IsRightChild(unsigned short) const` | identical | no | no | yes |  |
| mdlcache | `CUtlRBTree<CUtlMap<char const*, studiodata_t*, unsigned short>::Node_t, unsigned short, CUtlMap<char const*, studiodata_t*, unsigned short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, studiodata_t*, unsigned short>::Node_t, unsigned short>, unsigned short>>::Find(CUtlMap<char const*, studiodata_t*, unsigned short>::Node_t const&) const` | identical | no | no | yes |  |
| mdlcache | `CUtlRBTree<CUtlMap<int, int, unsigned short>::Node_t, unsigned short, CUtlMap<int, int, unsigned short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<int, int, unsigned short>::Node_t, unsigned short>, unsigned short>>::IsLeftChild(unsigned short) const` | identical | no | no | yes |  |
| mdlcache | `CUtlRBTree<CUtlMap<int, int, unsigned short>::Node_t, unsigned short, CUtlMap<int, int, unsigned short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<int, int, unsigned short>::Node_t, unsigned short>, unsigned short>>::IsRightChild(unsigned short) const` | identical | no | no | yes |  |
| mdlcache | `CUtlRBTree<CUtlMap<int, int, unsigned short>::Node_t, unsigned short, CUtlMap<int, int, unsigned short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<int, int, unsigned short>::Node_t, unsigned short>, unsigned short>>::Find(CUtlMap<int, int, unsigned short>::Node_t const&) const` | identical | no | no | yes |  |
| mdlcache | `studiohdr_t::GetVirtualModel() const` | edited | no | yes | yes | studio.h: virtualModel back in studiohdr_t on 32-bit |
| mdlcache | `studiohdr_t::FindModel(void**, char const*) const` | identical | yes | yes | yes |  |
| mdlcache | `virtualgroup_t::GetStudioHdr() const` | identical | yes | yes | yes |  |
| mdlcache | `non-virtual thunk to CMDLCache::GetItemName(unsigned int, void const*, char*, unsigned int)` | identical | yes | yes | yes |  |
| mdlcache | `non-virtual thunk to CMDLCache::HandleCacheNotification(DataCacheNotification_t const&)` | identical | yes | yes | yes |  |
| mdlcache | `non-virtual thunk to CFunctor1<void (*)(ModelParts_t*), ModelParts_t*, CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>>::~CFunctor1()` | identical | yes | yes | yes |  |
| mdlcache | `non-virtual thunk to CFunctor1<void (*)(ModelParts_t*), ModelParts_t*, CRefCounted1<CFunctor, CRefCountServiceBase<true, CRefMT>>>::~CFunctor1()` | identical | yes | yes | yes |  |
| mdlcache | `non-virtual thunk to CMDLCache::Disconnect()` | identical | yes | yes | yes |  |
| mdlcache | `non-virtual thunk to CMDLCache::VerifyHeaders(studiohdr_t*)` | identical | yes | yes | yes |  |
| mdlcache | `non-virtual thunk to CMDLCache::QueryInterface(char const*)` | identical | yes | yes | yes |  |
| mdlcache | `non-virtual thunk to CMDLCache::CacheVertexData(studiohdr_t*)` | identical | no | no | yes |  |
| mdlcache | `non-virtual thunk to CMDLCache::Init()` | identical | yes | yes | yes |  |
| mdlcache | `non-virtual thunk to CMDLCache::Connect(void* (*)(char const*, int*))` | identical | yes | yes | yes |  |
| mdlcache | `non-virtual thunk to CMDLCache::Shutdown()` | identical | yes | yes | yes |  |
| studio | `__GLOBAL__I_a` | identical | no | no | yes |  |
| studio | `CUtlMemory<int, int>::Grow(int)` | edited | no | no | yes | studio.cpp: SDK 2013 |
| studio | `studiohdr_t::CopyAutoplaySequences(unsigned short*, int) const` | edited | yes | yes | yes | studio.cpp: SDK 2013 |
| studio | `studiohdr_t::CountAutoplaySequences() const` | edited | yes | yes | yes | studio.cpp: SDK 2013 |
| studio | `studiohdr_t::pSeqdesc(int) const` | edited | yes | yes | yes | studio.cpp: SDK 2013 |
| studio_virtualmodel | `__GLOBAL__I_a` | edited | no | no | yes | studio_virtualmodel.cpp: SDK 2013 (CThreadTerminalMutex locking) |
| studio_virtualmodel | `CaselessStringLessThan(char const* const&, char const* const&)` | edited | yes | yes | yes | studio_virtualmodel.cpp: SDK 2013 (CThreadTerminalMutex locking) |
| studio_virtualmodel | `CUtlMemory<modellookup_t, int>::Grow(int)` | edited | no | no | yes | studio_virtualmodel.cpp: SDK 2013 (CThreadTerminalMutex locking) |
| studio_virtualmodel | `CUtlMemory<virtualgroup_t, int>::Grow(int)` | edited | no | no | yes | studio_virtualmodel.cpp: SDK 2013 (CThreadTerminalMutex locking) |
| studio_virtualmodel | `CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, short, short>::Node_t, short>, short>::Grow(int)` | edited | no | no | yes | studio_virtualmodel.cpp: SDK 2013 (CThreadTerminalMutex locking) |
| studio_virtualmodel | `CUtlMemory<virtualgeneric_t, int>::Grow(int)` | edited | no | no | yes | studio_virtualmodel.cpp: SDK 2013 (CThreadTerminalMutex locking) |
| studio_virtualmodel | `CUtlMemory<virtualsequence_t, int>::Grow(int)` | edited | no | no | yes | studio_virtualmodel.cpp: SDK 2013 (CThreadTerminalMutex locking) |
| studio_virtualmodel | `CUtlMemory<unsigned short, int>::Grow(int)` | edited | no | no | yes | studio_virtualmodel.cpp: SDK 2013 (CThreadTerminalMutex locking) |
| studio_virtualmodel | `CUtlRBTree<CUtlMap<char const*, short, short>::Node_t, short, CUtlMap<char const*, short, short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, short, short>::Node_t, short>, short>>::RotateLeft(short)` | edited | no | no | yes | studio_virtualmodel.cpp: SDK 2013 (CThreadTerminalMutex locking) |
| studio_virtualmodel | `CUtlRBTree<CUtlMap<char const*, short, short>::Node_t, short, CUtlMap<char const*, short, short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, short, short>::Node_t, short>, short>>::RotateRight(short)` | edited | no | no | yes | studio_virtualmodel.cpp: SDK 2013 (CThreadTerminalMutex locking) |
| studio_virtualmodel | `CUtlRBTree<CUtlMap<char const*, short, short>::Node_t, short, CUtlMap<char const*, short, short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, short, short>::Node_t, short>, short>>::InsertRebalance(short)` | edited | no | no | yes | studio_virtualmodel.cpp: SDK 2013 (CThreadTerminalMutex locking) |
| studio_virtualmodel | `CUtlRBTree<CUtlMap<char const*, short, short>::Node_t, short, CUtlMap<char const*, short, short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, short, short>::Node_t, short>, short>>::FindInsertionPosition(CUtlMap<char const*, short, short>::Node_t const&, short&, bool&)` | edited | no | no | yes | studio_virtualmodel.cpp: SDK 2013 (CThreadTerminalMutex locking) |
| studio_virtualmodel | `CUtlRBTree<CUtlMap<char const*, short, short>::Node_t, short, CUtlMap<char const*, short, short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, short, short>::Node_t, short>, short>>::Insert(CUtlMap<char const*, short, short>::Node_t const&)` | edited | no | no | yes | studio_virtualmodel.cpp: SDK 2013 (CThreadTerminalMutex locking) |
| studio_virtualmodel | `CUtlRBTree<CUtlMap<char const*, short, short>::Node_t, short, CUtlMap<char const*, short, short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, short, short>::Node_t, short>, short>>::NewNode()` | edited | no | no | yes | studio_virtualmodel.cpp: SDK 2013 (CThreadTerminalMutex locking) |
| studio_virtualmodel | `CUtlRBTree<CUtlMap<char const*, short, short>::Node_t, short, CUtlMap<char const*, short, short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, short, short>::Node_t, short>, short>>::RemoveAll()` | edited | no | no | yes | studio_virtualmodel.cpp: SDK 2013 (CThreadTerminalMutex locking) |
| studio_virtualmodel | `CUtlVector<modellookup_t, CUtlMemory<modellookup_t, int>>::InsertBefore(int)` | edited | no | no | yes | studio_virtualmodel.cpp: SDK 2013 (CThreadTerminalMutex locking) |
| studio_virtualmodel | `CUtlVector<modellookup_t, CUtlMemory<modellookup_t, int>>::~CUtlVector()` | edited | yes | yes | yes | studio_virtualmodel.cpp: SDK 2013 (CThreadTerminalMutex locking) |
| studio_virtualmodel | `CUtlVector<modellookup_t, CUtlMemory<modellookup_t, int>>::~CUtlVector()` | edited | no | no | yes | studio_virtualmodel.cpp: SDK 2013 (CThreadTerminalMutex locking) |
| studio_virtualmodel | `virtualmodel_t::AppendNodes(int, studiohdr_t const*)` | edited | no | no | yes | studio_virtualmodel.cpp: SDK 2013 (CThreadTerminalMutex locking) |
| studio_virtualmodel | `virtualmodel_t::AppendModels(int, studiohdr_t const*)` | edited | no | no | yes | studio_virtualmodel.cpp: SDK 2013 (CThreadTerminalMutex locking) |
| studio_virtualmodel | `virtualmodel_t::AppendBonemap(int, studiohdr_t const*)` | edited | no | no | yes | studio_virtualmodel.cpp: SDK 2013 (CThreadTerminalMutex locking) |
| studio_virtualmodel | `virtualmodel_t::AppendIKLocks(int, studiohdr_t const*)` | edited | no | no | yes | studio_virtualmodel.cpp: SDK 2013 (CThreadTerminalMutex locking) |
| studio_virtualmodel | `virtualmodel_t::AppendSequences(int, studiohdr_t const*)` | edited | no | no | yes | studio_virtualmodel.cpp: SDK 2013 (CThreadTerminalMutex locking) |
| studio_virtualmodel | `virtualmodel_t::AppendAnimations(int, studiohdr_t const*)` | edited | no | no | yes | studio_virtualmodel.cpp: SDK 2013 (CThreadTerminalMutex locking) |
| studio_virtualmodel | `virtualmodel_t::AppendAttachments(int, studiohdr_t const*)` | edited | no | no | yes | studio_virtualmodel.cpp: SDK 2013 (CThreadTerminalMutex locking) |
| studio_virtualmodel | `virtualmodel_t::AppendPoseParameters(int, studiohdr_t const*)` | differs | no | no | no | studio_virtualmodel.cpp: SDK 2013 (CThreadTerminalMutex locking) |
| studio_virtualmodel | `virtualmodel_t::UpdateAutoplaySequences(studiohdr_t const*)` | edited | no | no | yes | studio_virtualmodel.cpp: SDK 2013 (CThreadTerminalMutex locking) |
| studio_virtualmodel | `CModelLookupContext::~CModelLookupContext()` | edited | no | no | yes | studio_virtualmodel.cpp: SDK 2013 (CThreadTerminalMutex locking) |
| studio_virtualmodel | `CThreadTerminalMutex<CThreadFastMutex>::Lock()` | differs | no | no | no | studio_virtualmodel.cpp: SDK 2013 (CThreadTerminalMutex locking) |
| studio_virtualmodel | `CUtlDict<short, short>::Purge()` | edited | no | no | yes | studio_virtualmodel.cpp: SDK 2013 (CThreadTerminalMutex locking) |
| studio_virtualmodel | `CUtlRBTree<CUtlMap<char const*, short, short>::Node_t, short, CUtlMap<char const*, short, short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, short, short>::Node_t, short>, short>>::IsLeftChild(short) const` | edited | no | no | yes | studio_virtualmodel.cpp: SDK 2013 (CThreadTerminalMutex locking) |
| studio_virtualmodel | `CUtlRBTree<CUtlMap<char const*, short, short>::Node_t, short, CUtlMap<char const*, short, short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, short, short>::Node_t, short>, short>>::NextInorder(short) const` | edited | no | no | yes | studio_virtualmodel.cpp: SDK 2013 (CThreadTerminalMutex locking) |
| studio_virtualmodel | `CUtlRBTree<CUtlMap<char const*, short, short>::Node_t, short, CUtlMap<char const*, short, short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, short, short>::Node_t, short>, short>>::FirstInorder() const` | edited | no | no | yes | studio_virtualmodel.cpp: SDK 2013 (CThreadTerminalMutex locking) |
| studio_virtualmodel | `CUtlRBTree<CUtlMap<char const*, short, short>::Node_t, short, CUtlMap<char const*, short, short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, short, short>::Node_t, short>, short>>::IsRightChild(short) const` | edited | no | no | yes | studio_virtualmodel.cpp: SDK 2013 (CThreadTerminalMutex locking) |
| studio_virtualmodel | `CUtlRBTree<CUtlMap<char const*, short, short>::Node_t, short, CUtlMap<char const*, short, short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<CUtlMap<char const*, short, short>::Node_t, short>, short>>::Find(CUtlMap<char const*, short, short>::Node_t const&) const` | edited | no | no | yes | studio_virtualmodel.cpp: SDK 2013 (CThreadTerminalMutex locking) |
| studio_virtualmodel | `___clang_call_terminate` | edited | no | no | yes | studio_virtualmodel.cpp: SDK 2013 (CThreadTerminalMutex locking) |
| studiobyteswap | `__GLOBAL__I_a` | identical | no | no | yes |  |
| studiobyteswap | `CDatadescGeneratedNameHolder::~CDatadescGeneratedNameHolder()` | identical | no | no | yes |  |

Link composition (for the manifest lane): retail links no `memoverride.o`;
our wscript adds `public/tier0/memoverride.cpp`. Retail object order is
studiobyteswap, studio, studio_virtualmodel, datacache, mdlcache, then tier1
(convar, datamanager, generichash, interface, mempool, strtools, tier1,
utlbuffer), tier3, tier2. Load commands: libtier0, libvstdlib, libiconv,
libstdc++, libSystem; our wscript does not list vstdlib or iconv.
