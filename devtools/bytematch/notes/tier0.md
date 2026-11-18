# tier0 (libtier0.dylib, depot 223)

Retail object list (N_OSO, 21 objects): assert_dialog, commandline, cpu, cpu_posix, cpu_usage, dbg,
dynfunction, fasttimer, mem, minidump, platform_posix, pme_posix, progressbar, security,
systeminformation, thread, threadtools, tier0_strtools, tslist, vcrmode_posix, vprof.
Our wscript additionally builds cpumonitoring, mem_helpers, memdbg, memstd, memvalidate,
pch_tier0, PMELib and stacktools (manifest lane).

Measured with `tools/mv_measure.sh` (clang-600.0.57, bytematch strict call targets).
Status legend: identical = matched before and after this lane; edited = matched only after
source/header edits here; new = absent before; header = matches only with the measurement
overlays (lane-mod-small hdrfix.patch + SDK 2013 `ICommandLine`); differs = still differs.

Main edits: SDK 2013 `threadtools.h`/`tslist.h`/`vprof.h` (VPROF re-enabled) with a
PLATFORM_64BITS layer, leak-era `threadtools.cpp` plus the retail POSIX event wake generation,
SDK `CPUInformation`, `Plat_IsInDebugSession( bool )`, leak `dbg.cpp`/`cpu_posix.cpp`/
`vcrmode_posix.cpp`/`tslist.cpp`, retail assert dialog flow and command-line joining.

| object | function | status | note |
|---|---|---|---|
| assert_dialog | `_AreAllAssertsDisabled` | identical |  |
| assert_dialog | `_DoNewAssertDialog` | edited |  |
| assert_dialog | `_GetAssertDialogParent` | identical |  |
| assert_dialog | `_SetAllAssertsDisabled` | identical |  |
| assert_dialog | `_SetAssertDialogParent` | identical |  |
| assert_dialog | `_ShouldUseNewAssertDialog` | identical |  |
| assert_dialog | `___clang_call_terminate` | identical |  |
| commandline | `_CommandLine_Tier0` | identical |  |
| commandline | `CCommandLine::AppendParm(char const*, char const*)` | header |  |
| commandline | `CCommandLine::RemoveParm(char const*)` | identical |  |
| commandline | `CCommandLine::CreateCmdLine(char const*)` | identical |  |
| commandline | `CCommandLine::CreateCmdLine(int, char**)` | edited |  |
| commandline | `CCommandLine::ParseCommandLine()` | edited |  |
| commandline | `CCommandLine::LoadParametersFromFile(char const*&, char*&, int, bool)` | identical |  |
| commandline | `CCommandLine::SetParm(int, char const*)` | identical |  |
| commandline | `CCommandLine::~CCommandLine()` | identical |  |
| commandline | `CCommandLine::~CCommandLine()` | identical |  |
| commandline | `CCommandLine::GetCmdLine() const` | identical |  |
| commandline | `CCommandLine::GetParm(int) const` | identical |  |
| commandline | `CCommandLine::FindParm(char const*) const` | identical |  |
| commandline | `CCommandLine::CheckParm(char const*, char const**) const` | header |  |
| commandline | `CCommandLine::ParmCount() const` | identical |  |
| commandline | `CCommandLine::ParmValue(char const*, char const*) const` | header |  |
| commandline | `CCommandLine::ParmValue(char const*, float) const` | header |  |
| commandline | `CCommandLine::ParmValue(char const*, int) const` | header |  |
| cpu | `_GetCPUInformation` | edited |  |
| cpu_posix | `CalculateCPUFreq()` | edited |  |
| cpu_usage | `_GetCPUUsage` | identical |  |
| dbg | `_AssertValidStringPtr` | identical |  |
| dbg | `_COM_TimestampedLog` | identical |  |
| dbg | `_CallAssertFailedNotifyFunc` | identical |  |
| dbg | `_ColorSpewMessage` | identical |  |
| dbg | `_ConColorMsg` | identical |  |
| dbg | `_ConDColorMsg` | identical |  |
| dbg | `_ConDLog` | identical |  |
| dbg | `_ConDMsg` | identical |  |
| dbg | `_ConDWarning` | identical |  |
| dbg | `_ConLog` | identical |  |
| dbg | `_ConMsg` | identical |  |
| dbg | `_ConWarning` | identical |  |
| dbg | `_CrackSmokingCompiler` | identical |  |
| dbg | `_DLog` | identical |  |
| dbg | `_DMsg` | identical |  |
| dbg | `_DWarning` | identical |  |
| dbg | `_DefaultSpewFunc` | edited |  |
| dbg | `_DefaultSpewFuncAbortOnAsserts` | edited |  |
| dbg | `_DevLog` | identical |  |
| dbg | `_DevMsg` | identical |  |
| dbg | `_DevWarning` | identical |  |
| dbg | `_Error` | identical |  |
| dbg | `_GetSpewOutputColor` | edited |  |
| dbg | `_GetSpewOutputFunc` | identical |  |
| dbg | `_GetSpewOutputGroup` | edited |  |
| dbg | `_GetSpewOutputLevel` | edited |  |
| dbg | `_IsSpewActive` | identical |  |
| dbg | `_Log` | identical |  |
| dbg | `_Msg` | identical |  |
| dbg | `_NetLog` | identical |  |
| dbg | `_NetMsg` | identical |  |
| dbg | `_NetWarning` | identical |  |
| dbg | `_Plat_SimpleLog` | identical |  |
| dbg | `_SetAssertFailedNotifyFunc` | identical |  |
| dbg | `_SpewActivate` | identical |  |
| dbg | `_SpewOutputFunc` | identical |  |
| dbg | `_Warning` | identical |  |
| dbg | `__AssertValidReadPtr` | identical |  |
| dbg | `__AssertValidReadWritePtr` | identical |  |
| dbg | `__AssertValidWritePtr` | identical |  |
| dbg | `__DSpewMessage` | identical |  |
| dbg | `__ExitOnFatalAssert` | edited |  |
| dbg | `__SpewInfo` | identical |  |
| dbg | `__SpewMessage` | identical |  |
| dbg | `ConWarning(char const*, ...)` | identical |  |
| dbg | `DevWarning(char const*, ...)` | identical |  |
| dbg | `ConColorMsg(Color const&, char const*, ...)` | identical |  |
| dbg | `ConLog(char const*, ...)` | identical |  |
| dbg | `ConMsg(char const*, ...)` | identical |  |
| dbg | `DevLog(char const*, ...)` | identical |  |
| dbg | `DevMsg(char const*, ...)` | identical |  |
| dbg | `_SpewMessage(SpewType_t, char const*, int, Color const*, char const*, char*)` | edited |  |
| dbg | `CThreadLocalPtr<SpewInfo_t>::~CThreadLocalPtr()` | new |  |
| dynfunction | `_VoidFnPtrLookup_Tier0` | identical |  |
| dynfunction | `CSharedLibraryCache::CSharedLibraryItem::~CSharedLibraryItem()` | identical |  |
| dynfunction | `CSharedLibraryCache::GetHandle(char const*)` | identical |  |
| dynfunction | `CSharedLibraryCache::~CSharedLibraryCache()` | identical |  |
| fasttimer | `CClockSpeedInit::Init()` | identical |  |
| mem | `_MemAllocScratch` | identical |  |
| mem | `_MemFreeScratch` | identical |  |
| minidump | `_CatchAndWriteMiniDump` | identical |  |
| minidump | `_WriteMiniDump` | identical |  |
| platform_posix | `_ApproximateProcessMemoryUsage` | edited |  |
| platform_posix | `_Is64BitOS` | identical |  |
| platform_posix | `_Plat_DebugString` | identical |  |
| platform_posix | `_Plat_ExitProcess` | identical |  |
| platform_posix | `_Plat_FloatTime` | identical |  |
| platform_posix | `_Plat_GetCommandLine` | identical |  |
| platform_posix | `_Plat_GetCommandLineA` | identical |  |
| platform_posix | `_Plat_IsInBenchmarkMode` | identical |  |
| platform_posix | `_Plat_IsInDebugSession` | edited |  |
| platform_posix | `_Plat_MSTime` | identical |  |
| platform_posix | `_Plat_SetBenchmarkMode` | identical |  |
| platform_posix | `_Plat_SetCommandLine` | identical |  |
| platform_posix | `_Plat_ctime` | identical |  |
| platform_posix | `_Plat_gmtime` | identical |  |
| platform_posix | `_Plat_localtime` | identical |  |
| platform_posix | `_Plat_timegm` | identical |  |
| platform_posix | `_vtune` | identical |  |
| pme_posix | `_InitPME` | identical |  |
| pme_posix | `_ShutdownPME` | identical |  |
| pme_posix | `CL2Cache::End()` | identical |  |
| pme_posix | `CL2Cache::Start()` | identical |  |
| pme_posix | `CL2Cache::CL2Cache()` | identical |  |
| pme_posix | `CL2Cache::~CL2Cache()` | identical |  |
| progressbar | `_InstallProgressReportHandler` | identical |  |
| progressbar | `_ReportProgress` | identical |  |
| security | `_Plat_FastVerifyHardwareKey` | identical |  |
| security | `_Plat_VerifyHardwareKey` | identical |  |
| security | `_Plat_VerifyHardwareKeyDriver` | identical |  |
| security | `_Plat_VerifyHardwareKeyPrompt` | identical |  |
| systeminformation | `_Plat_GetMemPageSize` | identical |  |
| systeminformation | `_Plat_GetPagedPoolInfo` | identical |  |
| thread | `_Plat_ApplyHardwareDataBreakpointsToNewThread` | identical |  |
| thread | `_Plat_SetHardwareDataBreakpoint` | identical |  |
| threadtools | `_CreateSimpleThread` | edited |  |
| threadtools | `_DeclareCurrentThreadIsMainThread` | identical |  |
| threadtools | `_GetThreadedLoadLibraryFunc` | identical |  |
| threadtools | `_ReleaseThreadHandle` | identical |  |
| threadtools | `_SetThreadedLoadLibraryFunc` | identical |  |
| threadtools | `_ThreadDetach` | new |  |
| threadtools | `_ThreadGetCurrentHandle` | identical |  |
| threadtools | `_ThreadGetCurrentId` | identical |  |
| threadtools | `_ThreadGetPriority` | edited |  |
| threadtools | `_ThreadInMainThread` | identical |  |
| threadtools | `_ThreadInterlockedAssignIf` | new |  |
| threadtools | `_ThreadInterlockedAssignIf64` | identical |  |
| threadtools | `_ThreadInterlockedAssignPointerIf` | new |  |
| threadtools | `_ThreadInterlockedCompareExchange` | new |  |
| threadtools | `_ThreadInterlockedCompareExchange64` | edited |  |
| threadtools | `_ThreadInterlockedCompareExchangePointer` | new |  |
| threadtools | `_ThreadInterlockedDecrement` | new |  |
| threadtools | `_ThreadInterlockedExchange` | new |  |
| threadtools | `_ThreadInterlockedExchange64` | identical |  |
| threadtools | `_ThreadInterlockedExchangeAdd` | new |  |
| threadtools | `_ThreadInterlockedExchangePointer` | new |  |
| threadtools | `_ThreadInterlockedIncrement` | new |  |
| threadtools | `_ThreadJoin` | identical |  |
| threadtools | `_ThreadSetAffinity` | identical |  |
| threadtools | `_ThreadSetDebugName` | identical |  |
| threadtools | `_ThreadSetPriority` | edited |  |
| threadtools | `_ThreadSleep` | new |  |
| threadtools | `CreateSimpleThread(unsigned int (*)(void*), void*, unsigned long*, unsigned int)` | new |  |
| threadtools | `DefaultWaitFunc(int, CThreadEvent* const*, int, unsigned int)` | new |  |
| threadtools | `ThreadProcConvert(void*)` | edited |  |
| threadtools | `CThreadEvent::Set()` | edited |  |
| threadtools | `CThreadEvent::Wait(unsigned int)` | identical |  |
| threadtools | `CThreadEvent::Check()` | edited |  |
| threadtools | `CThreadEvent::Reset()` | edited |  |
| threadtools | `CThreadEvent::CThreadEvent(bool)` | identical |  |
| threadtools | `CThreadEvent::CThreadEvent(bool)` | edited |  |
| threadtools | `CThreadMutex::TryLock()` | identical |  |
| threadtools | `CThreadRWLock::UnlockWrite()` | edited |  |
| threadtools | `CThreadRWLock::WaitForRead()` | edited |  |
| threadtools | `CThreadRWLock::LockForWrite()` | edited |  |
| threadtools | `CWorkerThread::CallMaster(unsigned int, unsigned int)` | edited |  |
| threadtools | `CWorkerThread::CallWorker(unsigned int, unsigned int, bool, CFunctor*)` | new |  |
| threadtools | `CWorkerThread::WaitForCall(unsigned int*)` | edited |  |
| threadtools | `CWorkerThread::WaitForCall(unsigned int, unsigned int*)` | edited |  |
| threadtools | `CWorkerThread::WaitForReply(unsigned int)` | edited |  |
| threadtools | `CWorkerThread::WaitForReply(unsigned int, unsigned int (*)(int, CThreadEvent* const*, int, unsigned int))` | new |  |
| threadtools | `CWorkerThread::BoostPriority()` | edited |  |
| threadtools | `CWorkerThread::GetCallHandle()` | edited |  |
| threadtools | `CWorkerThread::Call(unsigned int, unsigned int, bool, unsigned int (*)(int, CThreadEvent* const*, int, unsigned int), CFunctor*)` | new |  |
| threadtools | `CWorkerThread::Reply(unsigned int)` | edited |  |
| threadtools | `CWorkerThread::PeekCall(unsigned int*, CFunctor**)` | new |  |
| threadtools | `CWorkerThread::CWorkerThread()` | edited |  |
| threadtools | `CWorkerThread::~CWorkerThread()` | edited |  |
| threadtools | `CWorkerThread::~CWorkerThread()` | edited |  |
| threadtools | `CThreadLocalPtr<CThread>::~CThreadLocalPtr()` | new |  |
| threadtools | `CThreadLocalBase::Set(void*)` | new |  |
| threadtools | `CThreadLocalBase::CThreadLocalBase()` | new |  |
| threadtools | `CThreadLocalBase::CThreadLocalBase()` | new |  |
| threadtools | `CThreadLocalBase::~CThreadLocalBase()` | new |  |
| threadtools | `CThreadLocalBase::~CThreadLocalBase()` | new |  |
| threadtools | `CThreadSpinRWLock::UnlockRead()` | new |  |
| threadtools | `CThreadSpinRWLock::LockForRead()` | new |  |
| threadtools | `CThreadSpinRWLock::UnlockWrite()` | new |  |
| threadtools | `CThreadSpinRWLock::SpinLockForWrite(unsigned int)` | new |  |
| threadtools | `CThreadSyncObject::AssertUseable()` | identical |  |
| threadtools | `CThreadSyncObject::Wait(unsigned int)` | edited |  |
| threadtools | `CThreadSyncObject::CThreadSyncObject()` | identical |  |
| threadtools | `CThreadSyncObject::CThreadSyncObject()` | identical |  |
| threadtools | `CThreadSyncObject::~CThreadSyncObject()` | identical |  |
| threadtools | `CThreadSyncObject::~CThreadSyncObject()` | identical |  |
| threadtools | `CThread::ThreadProc(void*)` | edited |  |
| threadtools | `CThread::SetPriority(int)` | edited |  |
| threadtools | `CThread::GetThreadProc()` | identical |  |
| threadtools | `CThread::IsThreadRunning()` | edited |  |
| threadtools | `CThread::GetCurrentCThread()` | identical |  |
| threadtools | `CThread::ResumeCooperative()` | new |  |
| threadtools | `CThread::SuspendCooperative()` | new |  |
| threadtools | `CThread::WaitForCreateComplete(CThreadEvent*)` | identical |  |
| threadtools | `CThread::BWaitForThreadSuspendCooperative()` | new |  |
| threadtools | `CThread::Init()` | identical |  |
| threadtools | `CThread::Join(unsigned int)` | edited |  |
| threadtools | `CThread::Stop(int)` | edited |  |
| threadtools | `CThread::Sleep(unsigned int)` | identical |  |
| threadtools | `CThread::Start(unsigned int)` | new |  |
| threadtools | `CThread::Yield()` | edited |  |
| threadtools | `CThread::OnExit()` | identical |  |
| threadtools | `CThread::Resume()` | edited |  |
| threadtools | `CThread::Cleanup()` | new |  |
| threadtools | `CThread::GetName()` | edited |  |
| threadtools | `CThread::IsAlive()` | edited |  |
| threadtools | `CThread::SetName(char const*)` | edited |  |
| threadtools | `CThread::Suspend()` | edited |  |
| threadtools | `CThread::GetResult()` | edited |  |
| threadtools | `CThread::Terminate(int)` | edited |  |
| threadtools | `CThread::CThread()` | edited |  |
| threadtools | `CThread::~CThread()` | identical |  |
| threadtools | `CThread::~CThread()` | identical |  |
| threadtools | `CThread::~CThread()` | edited |  |
| threadtools | `CWorkerThread::GetCallParam(CFunctor**) const` | new |  |
| threadtools | `CThreadLocalBase::Get() const` | new |  |
| threadtools | `CThreadSyncObject::operator!() const` | identical |  |
| threadtools | `CThread::GetPriority() const` | edited |  |
| threadtools | `CThreadFastMutex::Lock(unsigned int, unsigned int) volatile` | edited |  |
| tier0_strtools | `_V_tier0_stricmp` | identical |  |
| tslist | `_RunTSListTests` | identical |  |
| tslist | `_RunTSQueueTests` | identical |  |
| tslist | `RunSharedTests(int)` | edited |  |
| tslist | `CTSListBase::~CTSListBase()` | identical |  |
| tslist | `TSListTests::MTPushMTPop(bool)` | edited |  |
| tslist | `TSListTests::MTPushSTPop(bool)` | edited |  |
| tslist | `TSListTests::PushPopTest()` | edited |  |
| tslist | `TSListTests::STPushMTPop(bool)` | edited |  |
| tslist | `TSListTests::MTPushSeqPop(bool)` | edited |  |
| tslist | `TSListTests::SeqPushMTPop(bool)` | edited |  |
| tslist | `TSListTests::PopThreadFunc(void*)` | edited |  |
| tslist | `TSListTests::PushThreadFunc(void*)` | edited |  |
| tslist | `TSListTests::MTPushPopPopInterleaved(bool)` | edited |  |
| tslist | `TSListTests::PushPopInterleavedTestGuts()` | edited |  |
| tslist | `TSListTests::PushPopInterleavedTestThreadFunc(void*)` | edited |  |
| tslist | `TSListTests::TestEnd(bool)` | edited |  |
| tslist | `TSListTests::CListOps::Pop(int*)` | edited |  |
| tslist | `TSListTests::CListOps::Push(int)` | edited |  |
| tslist | `TSListTests::CListOps::IsEmpty()` | identical |  |
| tslist | `TSListTests::CListOps::Validate()` | identical |  |
| tslist | `TSListTests::CQueueOps::Pop(int*)` | edited |  |
| tslist | `TSListTests::CQueueOps::Push(int)` | edited |  |
| tslist | `TSListTests::CQueueOps::IsEmpty()` | identical |  |
| tslist | `TSListTests::CQueueOps::Validate()` | identical |  |
| tslist | `CTSList<int>::~CTSList()` | identical |  |
| tslist | `CTSList<int>::~CTSList()` | identical |  |
| tslist | `CTSQueue<int, true>::Pop()` | edited |  |
| tslist | `CTSQueue<int, true>::Push(CTSQueue<int, true>::Node_t*)` | edited |  |
| tslist | `CTSQueue<int, true>::Purge()` | identical |  |
| tslist | `CTSQueue<int, true>::CTSQueue()` | edited |  |
| tslist | `CTSQueue<int, true>::~CTSQueue()` | identical |  |
| tslist | `CTSQueue<int, true>::~CTSQueue()` | identical |  |
| tslist | `std::list<ThreadHandle_t__*, std::allocator<ThreadHandle_t__*>>::~list()` | identical |  |
| vcrmode_posix | `_BuildCmdLine` | identical |  |
| vcrmode_posix | `VCR_Hook_Time(long*)` | identical |  |
| vcrmode_posix | `VCR_CreateThread(void*, unsigned long, void*, void*, unsigned long, unsigned long*)` | new |  |
| vcrmode_posix | `VCR_GenericValue(char const*, void*, int)` | identical |  |
| vcrmode_posix | `VCR_GenericRecord(char const*, void const*, int)` | identical |  |
| vcrmode_posix | `VCR_GenericString(char const*, char const*)` | identical |  |
| vcrmode_posix | `VCR_Hook_LocalTime(tm*)` | identical |  |
| vcrmode_posix | `VCR_GenericPlayback(char const*, void*, int, bool)` | identical |  |
| vcrmode_posix | `VCR_Hook_GetKeyState(int)` | identical |  |
| vcrmode_posix | `VCR_GenericValueVerify(char const*, void const*, int)` | identical |  |
| vcrmode_posix | `VCR_Hook_RecordGameMsg(InputEvent_t const&)` | identical |  |
| vcrmode_posix | `VCR_GetPercentCompleted()` | identical |  |
| vcrmode_posix | `VCR_WaitForSingleObject(void*, unsigned long)` | identical |  |
| vcrmode_posix | `VCR_EnterCriticalSection(void*)` | identical |  |
| vcrmode_posix | `VCR_Hook_PlaybackGameMsg(InputEvent_t*)` | identical |  |
| vcrmode_posix | `VCR_Hook_ReadConsoleInput(void*, void*, int, unsigned long*)` | identical |  |
| vcrmode_posix | `VCR_Hook_RecordEndGameMsg()` | identical |  |
| vcrmode_posix | `VCR_WaitForMultipleObjects(unsigned int, void const**, int, unsigned int)` | identical |  |
| vcrmode_posix | `VCR_Hook_GetNumberOfConsoleInputEvents(void*, unsigned long*)` | identical |  |
| vcrmode_posix | `VCR_GetMode()` | identical |  |
| vcrmode_posix | `VCR_Hook_recv(int, char*, int, int)` | identical |  |
| vcrmode_posix | `VCR_Hook_send(int, char const*, int, int)` | identical |  |
| vcrmode_posix | `VCR_SyncToken(char const*)` | identical |  |
| vcrmode_posix | `VCR_SetEnabled(int)` | identical |  |
| vcrmode_posix | `VCR_Hook_Cmd_Exec(char**)` | identical |  |
| vcrmode_posix | `VCR_Hook_recvfrom(int, char*, int, int, sockaddr*, int*)` | identical |  |
| vcrmode_posix | `VCR_Hook_PeekMessage(tagMSG*, void*, unsigned int, unsigned int, unsigned int)` | identical |  |
| vcrmode_posix | `VCR_Hook_RegCloseKey(void*)` | identical |  |
| vcrmode_posix | `VCR_Hook_GetCursorPos(tagPOINT*)` | identical |  |
| vcrmode_posix | `VCR_Hook_RegOpenKeyEx(void*, char const*, unsigned long, unsigned long, void*)` | identical |  |
| vcrmode_posix | `VCR_Hook_RegSetValueEx(void*, char const*, unsigned long, unsigned long, unsigned char const*, unsigned long)` | identical |  |
| vcrmode_posix | `VCR_Hook_Sys_FloatTime(double)` | identical |  |
| vcrmode_posix | `VCR_Hook_GetCommandLine()` | identical |  |
| vcrmode_posix | `VCR_Hook_RegCreateKeyEx(void*, char const*, unsigned long, char*, unsigned long, unsigned long, void*, void*, unsigned long*)` | identical |  |
| vcrmode_posix | `VCR_Hook_ScreenToClient(void*, tagPOINT*)` | identical |  |
| vcrmode_posix | `VCR_GetVCRTraceInterface()` | identical |  |
| vcrmode_posix | `VCR_Hook_RegQueryValueEx(void*, char const*, unsigned long*, unsigned long*, unsigned char*, unsigned long*)` | identical |  |
| vcrmode_posix | `VCR_End()` | identical |  |
| vcrmode_posix | `VCR_Error(char const*, ...)` | identical |  |
| vcrmode_posix | `VCR_Event(VCREvent)` | identical |  |
| vcrmode_posix | `VCR_Start(char const*, bool, IVCRHelpers*)` | identical |  |
| vcrmode_posix | `CVCRTrace::Read(void*, int)` | identical |  |
| vcrmode_posix | `CVCRTrace::ReadEvent()` | identical |  |
| vprof | `__GLOBAL__I_a` | identical |  |
| vprof | `DumpSorted(void (*)(char const*, ...), char const*, double, bool (*)(TimeSums_t const&, TimeSums_t const&), int)` | new |  |
| vprof | `PeakCompare(TimeSums_t const&, TimeSums_t const&)` | new |  |
| vprof | `TimeCompare(TimeSums_t const&, TimeSums_t const&)` | new |  |
| vprof | `AverageTimeCompare(TimeSums_t const&, TimeSums_t const&)` | new |  |
| vprof | `PeakOverAverageCompare(TimeSums_t const&, TimeSums_t const&)` | new |  |
| vprof | `TimeLessChildrenCompare(TimeSums_t const&, TimeSums_t const&)` | new |  |
| vprof | `AverageTimeLessChildrenCompare(TimeSums_t const&, TimeSums_t const&)` | new |  |
| vprof | `CVProfNode::EnterScope()` | new |  |
| vprof | `CVProfNode::GetSubNode(char const*, int, char const*)` | new |  |
| vprof | `CVProfNode::GetSubNode(char const*, int, char const*, int)` | new |  |
| vprof | `CVProfNode::SetCurFrameTime(unsigned long)` | new |  |
| vprof | `CVProfNode::Pause()` | new |  |
| vprof | `CVProfNode::Reset()` | new |  |
| vprof | `CVProfNode::Resume()` | new |  |
| vprof | `CVProfNode::ExitScope()` | new |  |
| vprof | `CVProfNode::MarkFrame()` | new |  |
| vprof | `CVProfNode::ResetPeak()` | new |  |
| vprof | `CVProfNode::~CVProfNode()` | new |  |
| vprof | `CVProfNode::~CVProfNode()` | new |  |
| vprof | `CVProfile::FreeNodes_R(CVProfNode*)` | new |  |
| vprof | `CVProfile::OutputReport(int, char const*, int)` | new |  |
| vprof | `CVProfile::ResetCounters(CounterGroup_t)` | new |  |
| vprof | `CVProfile::HideBudgetGroup(int, bool)` | new |  |
| vprof | `CVProfile::SetOutputStream(void (*)(char const*, ...))` | new |  |
| vprof | `CVProfile::AddBudgetGroupName(char const*, int)` | new |  |
| vprof | `CVProfile::GetNumBudgetGroups()` | new |  |
| vprof | `CVProfile::FindBudgetGroupName(char const*)` | new |  |
| vprof | `CVProfile::FindOrCreateCounter(char const*, CounterGroup_t)` | new |  |
| vprof | `CVProfile::GetBudgetGroupColor(int, int&, int&, int&, int&)` | new |  |
| vprof | `CVProfile::BudgetGroupNameToBudgetGroupID(char const*)` | new |  |
| vprof | `CVProfile::BudgetGroupNameToBudgetGroupID(char const*, int)` | new |  |
| vprof | `CVProfile::RegisterNumBudgetGroupsChangedCallBack(void (*)())` | new |  |
| vprof | `CVProfile::Term()` | new |  |
| vprof | `CVProfile::FindNode(CVProfNode*, char const*)` | new |  |
| vprof | `CVProfile::SumTimes(CVProfNode*, int)` | new |  |
| vprof | `CVProfile::SumTimes(char const*, int)` | new |  |
| vprof | `CVProfile::DumpNodes(CVProfNode*, int, bool)` | new |  |
| vprof | `CVProfile::CVProfile()` | new |  |
| vprof | `CVProfile::CVProfile()` | new |  |
| vprof | `CVProfile::~CVProfile()` | new |  |
| vprof | `CVProfile::~CVProfile()` | new |  |
| vprof | `CVProfile::GetCounterName(int) const` | new |  |
| vprof | `CVProfile::GetNumCounters() const` | new |  |
| vprof | `CVProfile::GetCounterGroup(int) const` | new |  |
| vprof | `CVProfile::GetCounterValue(int) const` | new |  |
| vprof | `CVProfile::GetCounterNameAndValue(int, int&) const` | new |  |
| vprof | `std::map<CVProfNode*, double, std::less<CVProfNode*>, std::allocator<std::pair<CVProfNode* const, double>>>::~map()` | new |  |
| vprof | `std::map<char const*, unsigned long, std::less<char const*>, std::allocator<std::pair<char const* const, unsigned long>>>::~map()` | new |  |
| vprof | `std::vector<TimeSums_t, std::allocator<TimeSums_t>>::_M_insert_aux(__gnu_cxx::__normal_iterator<TimeSums_t*, std::vector<TimeSums_t, std::allocator<TimeSums_t>>>, TimeSums_t const&)` | new |  |
| vprof | `std::vector<TimeSums_t, std::allocator<TimeSums_t>>::~vector()` | new |  |
| vprof | `std::vector<TimeSums_t, std::allocator<TimeSums_t>>::operator=(std::vector<TimeSums_t, std::allocator<TimeSums_t>> const&)` | new |  |
| vprof | `std::_Rb_tree<CVProfNode*, std::pair<CVProfNode* const, double>, std::_Select1st<std::pair<CVProfNode* const, double>>, std::less<CVProfNode*>, std::allocator<std::pair<CVProfNode* const, double>>>::_M_insert_unique(std::pair<CVProfNode* const, double> const&)` | new |  |
| vprof | `std::_Rb_tree<CVProfNode*, std::pair<CVProfNode* const, double>, std::_Select1st<std::pair<CVProfNode* const, double>>, std::less<CVProfNode*>, std::allocator<std::pair<CVProfNode* const, double>>>::_M_erase(std::_Rb_tree_node<std::pair<CVProfNode* const, double>>*)` | new |  |
| vprof | `std::_Rb_tree<char const*, std::pair<char const* const, unsigned long>, std::_Select1st<std::pair<char const* const, unsigned long>>, std::less<char const*>, std::allocator<std::pair<char const* const, unsigned long>>>::_M_insert_unique(std::pair<char const* const, unsigned long> const&)` | new |  |
| vprof | `std::_Rb_tree<char const*, std::pair<char const* const, unsigned long>, std::_Select1st<std::pair<char const* const, unsigned long>>, std::less<char const*>, std::allocator<std::pair<char const* const, unsigned long>>>::_M_erase(std::_Rb_tree_node<std::pair<char const* const, unsigned long>>*)` | new |  |
| vprof | `void std::__heap_select<__gnu_cxx::__normal_iterator<TimeSums_t*, std::vector<TimeSums_t, std::allocator<TimeSums_t>>>, bool (*)(TimeSums_t const&, TimeSums_t const&)>(__gnu_cxx::__normal_iterator<TimeSums_t*, std::vector<TimeSums_t, std::allocator<TimeSums_t>>>, __gnu_cxx::__normal_iterator<TimeSums_t*, std::vector<TimeSums_t, std::allocator<TimeSums_t>>>, __gnu_cxx::__normal_iterator<TimeSums_t*, std::vector<TimeSums_t, std::allocator<TimeSums_t>>>, bool (*)(TimeSums_t const&, TimeSums_t const&))` | new |  |
| vprof | `void std::__introsort_loop<__gnu_cxx::__normal_iterator<TimeSums_t*, std::vector<TimeSums_t, std::allocator<TimeSums_t>>>, int, bool (*)(TimeSums_t const&, TimeSums_t const&)>(__gnu_cxx::__normal_iterator<TimeSums_t*, std::vector<TimeSums_t, std::allocator<TimeSums_t>>>, __gnu_cxx::__normal_iterator<TimeSums_t*, std::vector<TimeSums_t, std::allocator<TimeSums_t>>>, int, bool (*)(TimeSums_t const&, TimeSums_t const&))` | new |  |
| vprof | `void std::__final_insertion_sort<__gnu_cxx::__normal_iterator<TimeSums_t*, std::vector<TimeSums_t, std::allocator<TimeSums_t>>>, bool (*)(TimeSums_t const&, TimeSums_t const&)>(__gnu_cxx::__normal_iterator<TimeSums_t*, std::vector<TimeSums_t, std::allocator<TimeSums_t>>>, __gnu_cxx::__normal_iterator<TimeSums_t*, std::vector<TimeSums_t, std::allocator<TimeSums_t>>>, bool (*)(TimeSums_t const&, TimeSums_t const&))` | new |  |

Counts: edited 74, header 5, identical 189, new 94
