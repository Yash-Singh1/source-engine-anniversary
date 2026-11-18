# vstdlib (libvstdlib.dylib, depot 223)

Retail objects: cvar, jobthread, KeyValuesSystem, osversion, random, vcover, plus
libcoroutine_osx.a(coroutine.o) and libtier1.a members (bitbuf, convar, generichash, interface,
mempool, memstack, strtools, tier1, utlbuffer, utlstring, utlsymbol; owned by the tier1 lane).
No memoverride.o (NO_MALLOC_OVERRIDE). Our wscript lacks the separate coroutine_osx static lib
and adds memoverride.cpp (manifest lane).

Status legend as in tier0.md. Most remaining differences come from tier1 headers: SDK 2013
`CUtlString` (16 bytes, out-of-line ctor) inside `CCvar::QueuedConVarSet_t`, `ConVar` member
layout (`m_fnChangeCallback` at 0x44), `CUtlRBTree`/`CUtlMemoryPool`/`CMemoryStack` layouts in
`CKeyValuesSystem` and `CVCoverage`. The coroutine diffs are register-allocation level and
uncertain.

Main edits: leak `jobthread.cpp` (ExecuteHighPriorityFunctor, cooperative suspend, Yield,
WaitForIdle), restored `osversion.cpp`, SDK `jobthread.h` (IThreadPool slot restored),
`CUtlBuffer::GetString` in `DisplayQueuedMessages`, `CGlobalThreadPool::Start` without a name.

| object | function | status | note |
|---|---|---|---|
| KeyValuesSystem | `_KeyValuesSystem` | identical |  |
| KeyValuesSystem | `CUtlMemory<CKeyValuesSystem::hash_item_t, int>::Grow(int)` | header |  |
| KeyValuesSystem | `CUtlRBTree<CKeyValuesSystem::MemoryLeakTracker_t, int, bool (*)(CKeyValuesSystem::MemoryLeakTracker_t const&, CKeyValuesSystem::MemoryLeakTracker_t const&), CUtlMemory<UtlRBTreeNode_t<CKeyValuesSystem::MemoryLeakTracker_t, int>, int>>::RemoveAll()` | differs | size retail 224 vs ours 268 |
| KeyValuesSystem | `CKeyValuesSystem::GetStringForSymbol(int)` | identical |  |
| KeyValuesSystem | `CKeyValuesSystem::GetSymbolForString(char const*, bool)` | edited |  |
| KeyValuesSystem | `CKeyValuesSystem::FreeKeyValuesMemory(void*)` | identical |  |
| KeyValuesSystem | `CKeyValuesSystem::AllocKeyValuesMemory(int)` | identical |  |
| KeyValuesSystem | `CKeyValuesSystem::RegisterSizeofKeyValues(int)` | identical |  |
| KeyValuesSystem | `CKeyValuesSystem::MemoryLeakTrackerLessFunc(CKeyValuesSystem::MemoryLeakTracker_t const&, CKeyValuesSystem::MemoryLeakTracker_t const&)` | identical |  |
| KeyValuesSystem | `CKeyValuesSystem::AddKeyValuesToMemoryLeakList(void*, int)` | identical |  |
| KeyValuesSystem | `CKeyValuesSystem::RemoveKeyValuesFromMemoryLeakList(void*)` | identical |  |
| KeyValuesSystem | `CKeyValuesSystem::CKeyValuesSystem()` | differs | size retail 689 vs ours 845 |
| KeyValuesSystem | `CKeyValuesSystem::~CKeyValuesSystem()` | identical |  |
| KeyValuesSystem | `CKeyValuesSystem::~CKeyValuesSystem()` | differs | size retail 240 vs ours 572 |
| coroutine | `_Coroutine_Cancel` | identical |  |
| coroutine | `_Coroutine_Continue` | identical |  |
| coroutine | `_Coroutine_Create` | edited |  |
| coroutine | `_Coroutine_DebugAssert` | identical |  |
| coroutine | `_Coroutine_DebugBreak` | identical |  |
| coroutine | `_Coroutine_GetCurrentlyActive` | identical |  |
| coroutine | `_Coroutine_GetStackDepth` | edited |  |
| coroutine | `_Coroutine_IsActive` | identical |  |
| coroutine | `_Coroutine_ReleaseThreadMemory` | edited |  |
| coroutine | `_Coroutine_Test` | header |  |
| coroutine | `_Coroutine_ValidateGlobals` | identical |  |
| coroutine | `_Coroutine_YieldToMain` | differs | size retail 1236 vs ours 1260 |
| coroutine | `__GLOBAL__I_a` | edited |  |
| coroutine | `GCoroutineMgr()` | edited |  |
| coroutine | `CoroutineTestL1(void*)` | edited |  |
| coroutine | `CoroutineTestL2(void*)` | identical |  |
| coroutine | `Coroutine_Finish()` | differs | size retail 612 vs ours 605 |
| coroutine | `Coroutine_Launch(CCoroutine&)` | differs | size retail 50 vs ours 54 |
| coroutine | `CoroutineTestFunc(void*)` | edited |  |
| coroutine | `Internal_Coroutine_Continue(int, char const*, char const*)` | differs | size retail 1522 vs ours 1557 |
| coroutine | `CUtlMemory<UtlLinkedListElem_t<CCoroutine, int>, int>::Grow(int)` | header |  |
| coroutine | `CUtlMemory<CVProfNode*, int>::Grow(int)` | header |  |
| coroutine | `CUtlMemory<CCoroutineMgr*, int>::Grow(int)` | header |  |
| coroutine | `CUtlMemory<int, int>::Grow(int)` | header |  |
| coroutine | `CUtlVector<CCoroutineMgr*, CUtlMemory<CCoroutineMgr*, int>>::~CUtlVector()` | identical |  |
| coroutine | `CThreadMutex::~CThreadMutex()` | identical |  |
| coroutine | `CCoroutineMgr::CCoroutineMgr()` | edited |  |
| coroutine | `CUtlLinkedList<CCoroutine, int, false, int, CUtlMemory<UtlLinkedListElem_t<CCoroutine, int>, int>>::AllocInternal(bool)` | header |  |
| coroutine | `CUtlLinkedList<CCoroutine, int, false, int, CUtlMemory<UtlLinkedListElem_t<CCoroutine, int>, int>>::Free(int)` | header |  |
| coroutine | `CUtlLinkedList<CCoroutine, int, false, int, CUtlMemory<UtlLinkedListElem_t<CCoroutine, int>, int>>::AddToTail()` | header |  |
| coroutine | `CUtlLinkedList<CCoroutine, int, false, int, CUtlMemory<UtlLinkedListElem_t<CCoroutine, int>, int>>::RemoveAll()` | new |  |
| coroutine | `CThreadLocalPtr<CCoroutineMgr>::~CThreadLocalPtr()` | new |  |
| cvar | `_VStdLib_GetICVarFactory` | identical |  |
| cvar | `__CreateCCvarICvar_interface()` | identical |  |
| cvar | `CUtlMemory<CCvar::QueuedConVarSet_t, int>::Grow(int)` | differs | size retail 160 vs ours 163 |
| cvar | `CUtlMemory<IConsoleDisplayFunc*, int>::Grow(int)` | header |  |
| cvar | `CUtlMemory<void (*)(IConVar*, char const*, float), int>::Grow(int)` | header |  |
| cvar | `CUtlVector<CCvar::QueuedConVarSet_t, CUtlMemory<CCvar::QueuedConVarSet_t, int>>::~CUtlVector()` | differs | size retail 178 vs ours 151 |
| cvar | `CBaseAppSystem<ICvarQuery>::Disconnect()` | identical |  |
| cvar | `CBaseAppSystem<ICvarQuery>::Init()` | identical |  |
| cvar | `CBaseAppSystem<ICvarQuery>::Connect(void* (*)(char const*, int*))` | identical |  |
| cvar | `CBaseAppSystem<ICvarQuery>::Shutdown()` | identical |  |
| cvar | `CDefaultCvarQuery::QueryInterface(char const*)` | identical |  |
| cvar | `CDefaultCvarQuery::AreConVarsLinkable(ConVar const*, ConVar const*)` | identical |  |
| cvar | `CConCommandMemberAccessor<CCvar>::CommandCallback(CCommand const&)` | identical |  |
| cvar | `CConCommandMemberAccessor<CCvar>::CommandCompletionCallback(char const*, CUtlVector<CUtlString, CUtlMemory<CUtlString, int>>&)` | identical |  |
| cvar | `CConCommandMemberAccessor<CCvar>::~CConCommandMemberAccessor()` | identical |  |
| cvar | `CConCommandMemberAccessor<CCvar>::~CConCommandMemberAccessor()` | identical |  |
| cvar | `CCvar::Disconnect()` | identical |  |
| cvar | `CCvar::FindCommand(char const*)` | identical |  |
| cvar | `CCvar::GetCommands()` | identical |  |
| cvar | `CCvar::QueryInterface(char const*)` | identical |  |
| cvar | `CCvar::FindCommandBase(char const*)` | identical |  |
| cvar | `CCvar::InstallCVarQuery(ICvarQuery*)` | identical |  |
| cvar | `CCvar::RegisterConCommand(ConCommandBase*)` | differs | bytes |
| cvar | `CCvar::GetCommandLineValue(char const*)` | header |  |
| cvar | `CCvar::RevertFlaggedConVars(int)` | identical |  |
| cvar | `CCvar::UnregisterConCommand(ConCommandBase*)` | identical |  |
| cvar | `CCvar::AllocateDLLIdentifier()` | identical |  |
| cvar | `CCvar::CCVarIteratorInternal::Get()` | identical |  |
| cvar | `CCvar::CCVarIteratorInternal::Next()` | identical |  |
| cvar | `CCvar::CCVarIteratorInternal::IsValid()` | identical |  |
| cvar | `CCvar::CCVarIteratorInternal::SetFirst()` | identical |  |
| cvar | `CCvar::CCVarIteratorInternal::~CCVarIteratorInternal()` | identical |  |
| cvar | `CCvar::CCVarIteratorInternal::~CCVarIteratorInternal()` | identical |  |
| cvar | `CCvar::DisplayQueuedMessages()` | header |  |
| cvar | `CCvar::UnregisterConCommands(int)` | identical |  |
| cvar | `CCvar::FactoryInternalIterator()` | identical |  |
| cvar | `CCvar::RemoveConsoleDisplayFunc(IConsoleDisplayFunc*)` | identical |  |
| cvar | `CCvar::CallGlobalChangeCallbacks(ConVar*, char const*, float)` | identical |  |
| cvar | `CCvar::InstallConsoleDisplayFunc(IConsoleDisplayFunc*)` | identical |  |
| cvar | `CCvar::RemoveGlobalChangeCallback(void (*)(IConVar*, char const*, float))` | identical |  |
| cvar | `CCvar::InstallGlobalChangeCallback(void (*)(IConVar*, char const*, float))` | identical |  |
| cvar | `CCvar::QueueMaterialThreadSetValue(ConVar*, char const*)` | differs | size retail 174 vs ours 152 |
| cvar | `CCvar::QueueMaterialThreadSetValue(ConVar*, float)` | differs | size retail 157 vs ours 144 |
| cvar | `CCvar::QueueMaterialThreadSetValue(ConVar*, int)` | differs | size retail 162 vs ours 140 |
| cvar | `CCvar::ProcessQueuedMaterialThreadConVarSets()` | differs | size retail 373 vs ours 333 |
| cvar | `CCvar::Find(CCommand const&)` | identical |  |
| cvar | `CCvar::Init()` | identical |  |
| cvar | `CCvar::Connect(void* (*)(char const*, int*))` | identical |  |
| cvar | `CCvar::FindVar(char const*)` | edited |  |
| cvar | `CCvar::Shutdown()` | identical |  |
| cvar | `CCvar::CCvar()` | identical |  |
| cvar | `CCvar::~CCvar()` | identical |  |
| cvar | `CCvar::FindCommand(char const*) const` | identical |  |
| cvar | `CCvar::GetCommands() const` | identical |  |
| cvar | `CCvar::ConsolePrintf(char const*, ...) const` | header |  |
| cvar | `CCvar::ConsoleDPrintf(char const*, ...) const` | header |  |
| cvar | `CCvar::FindCommandBase(char const*) const` | identical |  |
| cvar | `CCvar::ConsoleColorPrintf(Color const&, char const*, ...) const` | header |  |
| cvar | `CCvar::IsMaterialThreadSetAllowed() const` | identical |  |
| cvar | `CCvar::HasQueuedMaterialThreadConVarSets() const` | identical |  |
| cvar | `CCvar::FindVar(char const*) const` | edited |  |
| cvar | `non-virtual thunk to CConCommandMemberAccessor<CCvar>::CommandCallback(CCommand const&)` | identical |  |
| cvar | `non-virtual thunk to CConCommandMemberAccessor<CCvar>::CommandCompletionCallback(char const*, CUtlVector<CUtlString, CUtlMemory<CUtlString, int>>&)` | identical |  |
| cvar | `___clang_call_terminate` | identical |  |
| jobthread | `_CreateThreadPool` | identical |  |
| jobthread | `_DestroyThreadPool` | identical |  |
| jobthread | `_RunThreadPoolTests` | identical |  |
| jobthread | `CJobThread::Run()` | edited |  |
| jobthread | `CJobThread::CJobThread(CThreadPool*, int)` | edited |  |
| jobthread | `CJobThread::~CJobThread()` | edited |  |
| jobthread | `CJobThread::~CJobThread()` | edited |  |
| jobthread | `CUtlMemory<CJobThread*, int>::Grow(int)` | header |  |
| jobthread | `CUtlMemory<CThreadEvent*, int>::Grow(int)` | header |  |
| jobthread | `CFunctorJob::Describe()` | edited |  |
| jobthread | `CFunctorJob::DoExecute()` | edited |  |
| jobthread | `CFunctorJob::CFunctorJob(CFunctor*, char const*)` | edited |  |
| jobthread | `CFunctorJob::~CFunctorJob()` | edited |  |
| jobthread | `CFunctorJob::~CFunctorJob()` | edited |  |
| jobthread | `CThreadPool::Distribute(bool, int*)` | identical |  |
| jobthread | `CThreadPool::NumThreads()` | identical |  |
| jobthread | `CThreadPool::GetDummyJob()` | identical |  |
| jobthread | `CThreadPool::GetJobCount()` | identical |  |
| jobthread | `CThreadPool::ChangePriority(CJob*, JobPriority_t)` | identical |  |
| jobthread | `CThreadPool::NumIdleThreads()` | identical |  |
| jobthread | `CThreadPool::ResumeExecution()` | edited |  |
| jobthread | `CThreadPool::SuspendExecution()` | edited |  |
| jobthread | `CThreadPool::ExecuteToPriority(JobPriority_t, bool (*)(CJob*))` | header |  |
| jobthread | `CThreadPool::AddFunctorInternal(CFunctor*, CJob**, char const*, unsigned int)` | edited |  |
| jobthread | `CThreadPool::ExecuteHighPriorityFunctor(CFunctor*)` | new |  |
| jobthread | `CThreadPool::Stop(int)` | edited |  |
| jobthread | `CThreadPool::Start(ThreadPoolStartParams_t const&)` | edited |  |
| jobthread | `CThreadPool::Start(ThreadPoolStartParams_t const&, char const*)` | edited |  |
| jobthread | `CThreadPool::Yield(unsigned int)` | edited |  |
| jobthread | `CThreadPool::AddJob(CJob*)` | edited |  |
| jobthread | `CThreadPool::AbortAll()` | edited |  |
| jobthread | `CThreadPool::Reserved1()` | identical |  |
| jobthread | `CThreadPool::YieldWait(CThreadEvent**, int, bool, unsigned int)` | edited |  |
| jobthread | `CThreadPool::YieldWait(CJob**, int, bool, unsigned int)` | identical |  |
| jobthread | `CThreadPool::CThreadPool()` | identical |  |
| jobthread | `CThreadPool::~CThreadPool()` | identical |  |
| jobthread | `CThreadPool::~CThreadPool()` | identical |  |
| jobthread | `CThreadPool::~CThreadPool()` | identical |  |
| jobthread | `IThreadPool::AddFunctor(CFunctor*, CJob**, char const*, unsigned int)` | edited |  |
| jobthread | `CRefCounted1<IRefCounted, CRefCountServiceBase<true, CRefMT>>::AddRef()` | edited |  |
| jobthread | `CRefCounted1<IRefCounted, CRefCountServiceBase<true, CRefMT>>::Release()` | edited |  |
| jobthread | `CRefCounted1<IThreadPool, CRefCountServiceBase<true, CRefMT>>::AddRef()` | edited |  |
| jobthread | `CRefCounted1<IThreadPool, CRefCountServiceBase<true, CRefMT>>::Release()` | edited |  |
| jobthread | `CThreadEvent::~CThreadEvent()` | identical |  |
| jobthread | `CWorkerThread::~CWorkerThread()` | edited |  |
| jobthread | `CWorkerThread::~CWorkerThread()` | edited |  |
| jobthread | `ThreadPoolTest::CExecuteTestJob::DoExecute()` | edited |  |
| jobthread | `ThreadPoolTest::CExecuteTestJob::~CExecuteTestJob()` | identical |  |
| jobthread | `ThreadPoolTest::CExecuteTestJob::~CExecuteTestJob()` | identical |  |
| jobthread | `ThreadPoolTest::TestForcedExecute()` | edited |  |
| jobthread | `ThreadPoolTest::CExecuteTestExecuteJob::DoExecute()` | edited |  |
| jobthread | `ThreadPoolTest::CExecuteTestExecuteJob::~CExecuteTestExecuteJob()` | identical |  |
| jobthread | `ThreadPoolTest::CExecuteTestExecuteJob::~CExecuteTestExecuteJob()` | identical |  |
| jobthread | `ThreadPoolTest::Test(bool, bool, bool, bool)` | edited |  |
| jobthread | `ThreadPoolTest::CCountJob::DoExecute()` | edited |  |
| jobthread | `ThreadPoolTest::CCountJob::~CCountJob()` | identical |  |
| jobthread | `ThreadPoolTest::CCountJob::~CCountJob()` | identical |  |
| jobthread | `CGlobalThreadPool::OnFinalRelease()` | identical |  |
| jobthread | `CGlobalThreadPool::Start(ThreadPoolStartParams_t const&)` | header |  |
| jobthread | `CGlobalThreadPool::~CGlobalThreadPool()` | identical |  |
| jobthread | `CGlobalThreadPool::~CGlobalThreadPool()` | identical |  |
| jobthread | `CRefCountServiceBase<true, CRefMT>::OnFinalRelease()` | identical |  |
| jobthread | `CJob::SetDescription(char const*)` | edited |  |
| jobthread | `CJob::DoAbort(bool)` | identical |  |
| jobthread | `CJob::Describe()` | edited |  |
| jobthread | `CJob::DoCleanup()` | identical |  |
| jobthread | `CJob::CJob(JobPriority_t)` | edited |  |
| jobthread | `CJob::~CJob()` | identical |  |
| jobthread | `CJob::~CJob()` | identical |  |
| jobthread | `CFmtStrN<256, false>::InitQuietTruncation()` | identical |  |
| jobthread | `CFmtStrN<256, false>::CFmtStrN(char const*, ...)` | identical |  |
| jobthread | `CTSQueue<CJob*, false>::Pop()` | edited |  |
| jobthread | `CTSQueue<CJob*, false>::Push(CTSQueue<CJob*, false>::Node_t*)` | edited |  |
| jobthread | `CTSQueue<CJob*, false>::Purge()` | identical |  |
| jobthread | `CTSQueue<CJob*, false>::PopItem(CJob**)` | identical |  |
| jobthread | `CTSQueue<CJob*, false>::CTSQueue()` | edited |  |
| jobthread | `CTSQueue<CJob*, false>::~CTSQueue()` | identical |  |
| jobthread | `CJobQueue::Pop(CJob**)` | identical |  |
| jobthread | `CJobQueue::Push(CJob*, int)` | edited |  |
| jobthread | `CJobQueue::Flush()` | identical |  |
| jobthread | `CJobQueue::CJobQueue()` | edited |  |
| jobthread | `CJobQueue::~CJobQueue()` | identical |  |
| jobthread | `non-virtual thunk to CFunctorJob::~CFunctorJob()` | edited |  |
| jobthread | `non-virtual thunk to CFunctorJob::~CFunctorJob()` | edited |  |
| jobthread | `non-virtual thunk to CThreadPool::~CThreadPool()` | identical |  |
| jobthread | `non-virtual thunk to CThreadPool::~CThreadPool()` | identical |  |
| jobthread | `non-virtual thunk to ThreadPoolTest::CExecuteTestJob::~CExecuteTestJob()` | identical |  |
| jobthread | `non-virtual thunk to ThreadPoolTest::CExecuteTestJob::~CExecuteTestJob()` | identical |  |
| jobthread | `non-virtual thunk to ThreadPoolTest::CExecuteTestExecuteJob::~CExecuteTestExecuteJob()` | identical |  |
| jobthread | `non-virtual thunk to ThreadPoolTest::CExecuteTestExecuteJob::~CExecuteTestExecuteJob()` | identical |  |
| jobthread | `non-virtual thunk to ThreadPoolTest::CCountJob::~CCountJob()` | identical |  |
| jobthread | `non-virtual thunk to ThreadPoolTest::CCountJob::~CCountJob()` | identical |  |
| jobthread | `non-virtual thunk to CGlobalThreadPool::OnFinalRelease()` | identical |  |
| jobthread | `non-virtual thunk to CGlobalThreadPool::~CGlobalThreadPool()` | identical |  |
| jobthread | `non-virtual thunk to CGlobalThreadPool::~CGlobalThreadPool()` | identical |  |
| jobthread | `non-virtual thunk to CJob::~CJob()` | identical |  |
| jobthread | `non-virtual thunk to CJob::~CJob()` | identical |  |
| jobthread | `non-virtual thunk to CThreadPool::GetDummyJob()::CDummyJob::~CDummyJob()` | identical |  |
| jobthread | `non-virtual thunk to CThreadPool::GetDummyJob()::CDummyJob::~CDummyJob()` | identical |  |
| jobthread | `CThreadPool::GetDummyJob()::CDummyJob::DoExecute()` | identical |  |
| jobthread | `CThreadPool::GetDummyJob()::CDummyJob::~CDummyJob()` | identical |  |
| jobthread | `CThreadPool::GetDummyJob()::CDummyJob::~CDummyJob()` | identical |  |
| jobthread | `___cxx_global_array_dtor` | edited |  |
| osversion | `_GetNameFromOSType` | new |  |
| osversion | `_GetOSDetailString` | new |  |
| osversion | `_GetOSType` | new |  |
| osversion | `_GetPlatformName` | new |  |
| osversion | `_OSTypesAreCompatible` | new |  |
| random | `_InstallUniformRandomStream` | identical |  |
| random | `_RandomFloat` | identical |  |
| random | `_RandomFloatExp` | identical |  |
| random | `_RandomGaussianFloat` | identical |  |
| random | `_RandomInt` | identical |  |
| random | `_RandomSeed` | identical |  |
| random | `CUniformRandomStream::RandomFloat(float, float)` | identical |  |
| random | `CUniformRandomStream::RandomFloatExp(float, float, float)` | identical |  |
| random | `CUniformRandomStream::GenerateRandomNumber()` | edited |  |
| random | `CUniformRandomStream::SetSeed(int)` | edited |  |
| random | `CUniformRandomStream::RandomInt(int, int)` | identical |  |
| random | `CUniformRandomStream::CUniformRandomStream()` | identical |  |
| random | `CUniformRandomStream::CUniformRandomStream()` | identical |  |
| random | `CGaussianRandomStream::RandomFloat(float, float)` | edited |  |
| random | `CGaussianRandomStream::AttachToStream(IUniformRandomStream*)` | edited |  |
| random | `CGaussianRandomStream::CGaussianRandomStream(IUniformRandomStream*)` | edited |  |
| random | `CGaussianRandomStream::CGaussianRandomStream(IUniformRandomStream*)` | edited |  |
| vcover | `CUtlRBTree<CVCoverage::Location_t, unsigned short, CVCoverage::CLocationLess, CUtlMemory<UtlRBTreeNode_t<CVCoverage::Location_t, unsigned short>, unsigned short>>::RemoveAll()` | differs | size retail 268 vs ours 283 |
| vcover | `CVCoverage::~CVCoverage()` | differs | size retail 108 vs ours 85 |

Counts: differs 16, edited 59, header 20, identical 134, new 8
