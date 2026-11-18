# tier1 (libtier1.a / libmathlib.a members, depot 223)

Measured with devtools/bytematch (macos-cc.sh, clang-600.0.57, call targets checked) against the
retail module that links each member: engine.dylib, or hl2 client.dylib (lightdesc, powsse,
randsse, ssenoise, rangecheckedvar), hl2 server.dylib (stringpool), scenefilecache.dylib
(lzmaDecoder). "identical" means matched before this lane's edits too; "edited" means it
matches after a source change. Counts: {'edited': 195, 'identical': 312, 'written new': 1, 'uncertain': 1, 'kept (fork)': 1}.

Retail compiles tier1/mathlib from source in every module; the hl2 client/server and libvstdlib
link the same archive members (same N_OSO mtimes as engine). Lost Coast (Oct 2024) is an older
snapshot. Members compiled but not linked by any retail module (keyvaluesjson, utlbinaryblock,
LzmaDec.c) were removed.

| object | function | status | note |
|---|---|---|---|
| KeyValues | `CKeyValuesErrorStack::ReportError(char const*)` | edited | SDK 2013 KeyValues |
| KeyValues | `CUtlSortVector<KeyValues*, CUtlSortVectorKeyValuesByName, CUtlVector<KeyValues*, CUtlMemory<KeyValues*, int>>>::CompareHelper(KeyValues* const*, KeyValues* const*)` | identical |  |
| KeyValues | `EvaluateConditional(char const*)` | edited | SDK 2013 KeyValues |
| KeyValues | `IsSteamDeck(bool)` | written new | written new from the retail disassembly |
| KeyValues | `KeyValues::AddSubKey(KeyValues*)` | identical |  |
| KeyValues | `KeyValues::Clear()` | identical |  |
| KeyValues | `KeyValues::CopySubkeys(KeyValues*) const` | identical |  |
| KeyValues | `KeyValues::CreateKeyUsingKnownLastChild(char const*, KeyValues*)` | edited | SDK 2013 KeyValues |
| KeyValues | `KeyValues::CreateNewKey()` | identical |  |
| KeyValues | `KeyValues::FindKey(char const*, bool)` | edited | SDK 2013 KeyValues |
| KeyValues | `KeyValues::FindKey(int) const` | identical |  |
| KeyValues | `KeyValues::GetBool(char const*, bool)` | edited | SDK 2013 KeyValues |
| KeyValues | `KeyValues::GetColor(char const*)` | identical |  |
| KeyValues | `KeyValues::GetDataType(char const*)` | identical |  |
| KeyValues | `KeyValues::GetFirstTrueSubKey()` | identical |  |
| KeyValues | `KeyValues::GetFirstValue()` | identical |  |
| KeyValues | `KeyValues::GetFloat(char const*, float)` | identical |  |
| KeyValues | `KeyValues::GetInt(char const*, int)` | identical |  |
| KeyValues | `KeyValues::GetName() const` | identical |  |
| KeyValues | `KeyValues::GetNextTrueSubKey()` | identical |  |
| KeyValues | `KeyValues::GetNextValue()` | identical |  |
| KeyValues | `KeyValues::GetPtr(char const*, void*)` | identical |  |
| KeyValues | `KeyValues::GetString(char const*, char const*)` | edited | SDK 2013 KeyValues |
| KeyValues | `KeyValues::GetStringForSymbolClassic(int)` | identical |  |
| KeyValues | `KeyValues::GetSymbolForStringClassic(char const*, bool)` | identical |  |
| KeyValues | `KeyValues::GetUint64(char const*, unsigned long long)` | identical |  |
| KeyValues | `KeyValues::GetWString(char const*, wchar_t const*)` | edited | SDK 2013 KeyValues |
| KeyValues | `KeyValues::IsEmpty(char const*)` | identical |  |
| KeyValues | `KeyValues::KeyValues(char const*)` | identical |  |
| KeyValues | `KeyValues::KeyValues(char const*, char const*, char const*)` | identical |  |
| KeyValues | `KeyValues::KeyValues(char const*, char const*, char const*, char const*, char const*)` | identical |  |
| KeyValues | `KeyValues::KeyValues(char const*, char const*, int)` | identical |  |
| KeyValues | `KeyValues::KeyValues(char const*, char const*, int, char const*, int)` | identical |  |
| KeyValues | `KeyValues::KeyValues(char const*, char const*, wchar_t const*)` | identical |  |
| KeyValues | `KeyValues::LoadFromBuffer(char const*, CUtlBuffer&, IBaseFileSystem*, char const*)` | edited | SDK 2013 KeyValues |
| KeyValues | `KeyValues::LoadFromBuffer(char const*, char const*, IBaseFileSystem*, char const*)` | edited | SDK 2013 KeyValues |
| KeyValues | `KeyValues::LoadFromFile(IBaseFileSystem*, char const*, char const*)` | edited | SDK 2013 KeyValues |
| KeyValues | `KeyValues::MakeCopy() const` | edited | SDK 2013 KeyValues |
| KeyValues | `KeyValues::ParseIncludedKeys(char const*, char const*, IBaseFileSystem*, char const*, CUtlVector<KeyValues*, CUtlMemory<KeyValues*, int>>&)` | edited | SDK 2013 KeyValues |
| KeyValues | `KeyValues::ProcessResolutionKeys(char const*)` | identical |  |
| KeyValues | `KeyValues::ReadAsBinary(CUtlBuffer&, int)` | edited | SDK 2013 KeyValues |
| KeyValues | `KeyValues::ReadToken(CUtlBuffer&, bool&, bool&)` | identical |  |
| KeyValues | `KeyValues::RecursiveLoadFromBuffer(char const*, CUtlBuffer&)` | edited | SDK 2013 KeyValues |
| KeyValues | `KeyValues::RecursiveMergeKeyValues(KeyValues*)` | identical |  |
| KeyValues | `KeyValues::RecursiveSaveToFile(IBaseFileSystem*, void*, CUtlBuffer*, int, bool, bool)` | edited | SDK 2013 KeyValues |
| KeyValues | `KeyValues::RemoveEverything()` | identical |  |
| KeyValues | `KeyValues::RemoveSubKey(KeyValues*)` | identical |  |
| KeyValues | `KeyValues::SaveKeyToFile(KeyValues*, IBaseFileSystem*, void*, CUtlBuffer*, int, bool, bool)` | edited | SDK 2013 KeyValues |
| KeyValues | `KeyValues::SaveToFile(IBaseFileSystem*, char const*, char const*, bool, bool)` | edited | SDK 2013 KeyValues |
| KeyValues | `KeyValues::SetColor(char const*, Color)` | identical |  |
| KeyValues | `KeyValues::SetFloat(char const*, float)` | identical |  |
| KeyValues | `KeyValues::SetInt(char const*, int)` | identical |  |
| KeyValues | `KeyValues::SetName(char const*)` | identical |  |
| KeyValues | `KeyValues::SetPtr(char const*, void*)` | identical |  |
| KeyValues | `KeyValues::SetString(char const*, char const*)` | identical |  |
| KeyValues | `KeyValues::SetStringValue(char const*)` | identical |  |
| KeyValues | `KeyValues::SetWString(char const*, wchar_t const*)` | identical |  |
| KeyValues | `KeyValues::WriteAsBinary(CUtlBuffer&)` | edited | SDK 2013 KeyValues |
| KeyValues | `KeyValues::WriteConvertedString(IBaseFileSystem*, void*, CUtlBuffer*, char const*)` | identical |  |
| KeyValues | `KeyValues::deleteThis()` | identical |  |
| KeyValues | `KeyValues::operator delete(void*)` | edited | SDK 2013 KeyValues |
| KeyValues | `KeyValues::operator new(unsigned long)` | identical |  |
| NetAdr | `netadr_s::Clear()` | identical |  |
| NetAdr | `netadr_s::CompareAdr(netadr_s const&, bool) const` | identical |  |
| NetAdr | `netadr_s::CompareClassBAdr(netadr_s const&) const` | identical |  |
| NetAdr | `netadr_s::CompareClassCAdr(netadr_s const&) const` | identical |  |
| NetAdr | `netadr_s::GetIPHostByteOrder() const` | identical |  |
| NetAdr | `netadr_s::GetIPNetworkByteOrder() const` | identical |  |
| NetAdr | `netadr_s::GetPort() const` | identical |  |
| NetAdr | `netadr_s::GetType() const` | identical |  |
| NetAdr | `netadr_s::IsLocalhost() const` | identical |  |
| NetAdr | `netadr_s::IsLoopback() const` | identical |  |
| NetAdr | `netadr_s::IsReservedAdr() const` | identical |  |
| NetAdr | `netadr_s::IsValid() const` | identical |  |
| NetAdr | `netadr_s::SetFromSockadr(sockaddr const*)` | identical |  |
| NetAdr | `netadr_s::SetFromString(char const*, bool)` | edited | SDK ToString/SetFromString |
| NetAdr | `netadr_s::SetIP(unsigned char, unsigned char, unsigned char, unsigned char)` | identical |  |
| NetAdr | `netadr_s::SetIP(unsigned int)` | identical |  |
| NetAdr | `netadr_s::SetPort(unsigned short)` | identical |  |
| NetAdr | `netadr_s::SetType(netadrtype_t)` | identical |  |
| NetAdr | `netadr_s::ToSockadr(sockaddr*) const` | identical |  |
| NetAdr | `netadr_s::ToString(bool) const` | edited | SDK ToString/SetFromString |
| bitbuf | `SetBitBufErrorHandler(void (*)(BitBufErrorType, char const*))` | identical |  |
| bitbuf | `__GLOBAL__I_a` | identical |  |
| bitbuf | `bf_read::PeekUBitLong(int)` | identical |  |
| bitbuf | `bf_read::ReadAndAllocateString(bool*)` | identical |  |
| bitbuf | `bf_read::ReadBitAngle(int)` | edited | unsigned long words on 32-bit |
| bitbuf | `bf_read::ReadBitCoord()` | identical |  |
| bitbuf | `bf_read::ReadBitCoordBits()` | identical |  |
| bitbuf | `bf_read::ReadBitCoordMP(bool, bool)` | identical |  |
| bitbuf | `bf_read::ReadBitCoordMPBits(bool, bool)` | identical |  |
| bitbuf | `bf_read::ReadBitNormal()` | identical |  |
| bitbuf | `bf_read::ReadBitVec3Coord(Vector&)` | identical |  |
| bitbuf | `bf_read::ReadBits(void*, int)` | edited | unsigned long words on 32-bit |
| bitbuf | `bf_read::ReadBitsClamped_ptr(void*, unsigned long, unsigned long)` | identical |  |
| bitbuf | `bf_read::ReadBytes(void*, int)` | identical |  |
| bitbuf | `bf_read::ReadFloat()` | identical |  |
| bitbuf | `bf_read::ReadLongLong()` | identical |  |
| bitbuf | `bf_read::ReadSBitLong(int)` | identical |  |
| bitbuf | `bf_read::ReadSignedVarInt32()` | identical |  |
| bitbuf | `bf_read::ReadString(char*, int, bool, int*)` | identical |  |
| bitbuf | `bf_read::ReadUBitVarInternal(int)` | identical |  |
| bitbuf | `bf_read::ReadVarInt32()` | identical |  |
| bitbuf | `bf_read::Reset()` | identical |  |
| bitbuf | `bf_read::SetDebugName(char const*)` | identical |  |
| bitbuf | `bf_read::SetOverflowFlag()` | identical |  |
| bitbuf | `bf_read::StartReading(void const*, int, int, int)` | identical |  |
| bitbuf | `bf_read::bf_read()` | identical |  |
| bitbuf | `bf_read::bf_read(char const*, void const*, int, int)` | identical |  |
| bitbuf | `bf_read::bf_read(void const*, int, int)` | identical |  |
| bitbuf | `bf_write::Reset()` | identical |  |
| bitbuf | `bf_write::SeekToBit(int)` | identical |  |
| bitbuf | `bf_write::SetAssertOnOverflow(bool)` | identical |  |
| bitbuf | `bf_write::SetDebugName(char const*)` | identical |  |
| bitbuf | `bf_write::StartWriting(void*, int, int, int)` | identical |  |
| bitbuf | `bf_write::WriteBitAngle(float, int)` | identical |  |
| bitbuf | `bf_write::WriteBitCoord(float)` | edited | unsigned long words on 32-bit |
| bitbuf | `bf_write::WriteBitCoordMP(float, bool, bool)` | identical |  |
| bitbuf | `bf_write::WriteBitNormal(float)` | edited | unsigned long words on 32-bit |
| bitbuf | `bf_write::WriteBitVec3Coord(Vector const&)` | edited | unsigned long words on 32-bit |
| bitbuf | `bf_write::WriteBits(void const*, int)` | edited | unsigned long words on 32-bit |
| bitbuf | `bf_write::WriteBitsFromBuffer(bf_read*, int)` | edited | unsigned long words on 32-bit |
| bitbuf | `bf_write::WriteByte(int)` | identical |  |
| bitbuf | `bf_write::WriteBytes(void const*, int)` | identical |  |
| bitbuf | `bf_write::WriteChar(int)` | identical |  |
| bitbuf | `bf_write::WriteFloat(float)` | identical |  |
| bitbuf | `bf_write::WriteLong(long)` | edited | unsigned long words on 32-bit |
| bitbuf | `bf_write::WriteLongLong(long long)` | edited | unsigned long words on 32-bit |
| bitbuf | `bf_write::WriteSBitLong(int, int)` | identical |  |
| bitbuf | `bf_write::WriteShort(int)` | identical |  |
| bitbuf | `bf_write::WriteSignedVarInt32(int)` | identical |  |
| bitbuf | `bf_write::WriteString(char const*)` | edited | unsigned long words on 32-bit |
| bitbuf | `bf_write::WriteVarInt32(unsigned int)` | edited | unsigned long words on 32-bit |
| bitbuf | `bf_write::WriteWord(int)` | identical |  |
| bitbuf | `bf_write::bf_write()` | identical |  |
| bitbuf | `bf_write::bf_write(char const*, void*, int, int)` | identical |  |
| bitbuf | `bf_write::bf_write(void*, int, int)` | identical |  |
| byteswap | `CByteswap::SwapFieldToTargetEndian(void*, void*, typedescription_t*)` | uncertain | retail has an extra in-place check that neither SDK 2013 nor ours has |
| byteswap | `CByteswap::SwapFieldsToTargetEndian(void*, void*, datamap_t*)` | identical |  |
| characterset | `CharacterSetBuild(characterset_t*, char const*)` | identical |  |
| checksum_crc | `CRC32_Final(unsigned int*)` | identical |  |
| checksum_crc | `CRC32_Init(unsigned int*)` | identical |  |
| checksum_crc | `CRC32_ProcessBuffer(unsigned int*, void const*, int)` | identical |  |
| checksum_md5 | `MD5Final(unsigned char*, MD5Context_t*)` | identical |  |
| checksum_md5 | `MD5Init(MD5Context_t*)` | identical |  |
| checksum_md5 | `MD5Transform(unsigned int*, unsigned int const*)` | identical |  |
| checksum_md5 | `MD5Update(MD5Context_t*, unsigned char const*, unsigned int)` | identical |  |
| checksum_md5 | `MD5_Compare(MD5Value_t const&, MD5Value_t const&)` | identical |  |
| checksum_md5 | `MD5_Print(unsigned char*, int)` | identical |  |
| commandbuffer | `CCommandBuffer::AddText(char const*, int)` | edited | SDK CCommandBuffer layout |
| commandbuffer | `CCommandBuffer::BeginProcessingCommands(int)` | edited | SDK CCommandBuffer layout |
| commandbuffer | `CCommandBuffer::CCommandBuffer()` | edited | SDK CCommandBuffer layout |
| commandbuffer | `CCommandBuffer::CCommandBuffer()` | identical |  |
| commandbuffer | `CCommandBuffer::DequeueNextCommand()` | edited | SDK CCommandBuffer layout |
| commandbuffer | `CCommandBuffer::EndProcessingCommands()` | edited | SDK CCommandBuffer layout |
| commandbuffer | `CCommandBuffer::GetNextCommandHandle()` | edited | SDK CCommandBuffer layout |
| commandbuffer | `CCommandBuffer::InsertCommand(char const*, int, int)` | edited | SDK CCommandBuffer layout |
| commandbuffer | `CCommandBuffer::IsProcessingCommands()` | edited | SDK CCommandBuffer layout |
| commandbuffer | `CCommandBuffer::SetWaitDelayTime(int)` | edited | SDK CCommandBuffer layout |
| commandbuffer | `CCommandBuffer::~CCommandBuffer()` | edited | SDK CCommandBuffer layout |
| commandbuffer | `CUtlLinkedList<CCommandBuffer::Command_t, int, true, int, CUtlFixedMemory<UtlLinkedListElem_t<CCommandBuffer::Command_t, int>>>::AllocInternal(bool)` | edited | SDK CCommandBuffer layout |
| commandbuffer | `CUtlLinkedList<CCommandBuffer::Command_t, int, true, int, CUtlFixedMemory<UtlLinkedListElem_t<CCommandBuffer::Command_t, int>>>::RemoveAll()` | identical |  |
| commandbuffer | `CUtlLinkedList<CCommandBuffer::Command_t, int, true, int, CUtlFixedMemory<UtlLinkedListElem_t<CCommandBuffer::Command_t, int>>>::Unlink(int)` | identical |  |
| convar | `CCommand::CCommand()` | identical |  |
| convar | `CCommand::CCommand(int, char const**)` | identical |  |
| convar | `CCommand::CCommand(int, char const**)` | identical |  |
| convar | `CCommand::DefaultBreakSet()` | identical |  |
| convar | `CCommand::FindArg(char const*) const` | identical |  |
| convar | `CCommand::Reset()` | identical |  |
| convar | `CCommand::Tokenize(char const*, characterset_t*)` | identical |  |
| convar | `CDefaultAccessor::RegisterConCommandBase(ConCommandBase*)` | identical |  |
| convar | `CEmptyConVar::GetName() const` | identical |  |
| convar | `CEmptyConVar::IsFlagSet(int) const` | identical |  |
| convar | `CEmptyConVar::SetValue(char const*)` | identical |  |
| convar | `CEmptyConVar::SetValue(float)` | identical |  |
| convar | `CEmptyConVar::SetValue(int)` | identical |  |
| convar | `CEmptyConVar::~CEmptyConVar()` | identical |  |
| convar | `CEmptyConVar::~CEmptyConVar()` | identical |  |
| convar | `ConCommand::AutoCompleteSuggest(char const*, CUtlVector<CUtlString, CUtlMemory<CUtlString, int>>&)` | edited | SDK 2013 ConVar (no TF2 competitive fields) |
| convar | `ConCommand::CanAutoComplete()` | identical |  |
| convar | `ConCommand::ConCommand(char const*, ICommandCallback*, char const*, int, ICommandCompletionCallback*)` | identical |  |
| convar | `ConCommand::ConCommand(char const*, void (*)(), char const*, int, int (*)(char const*, char (*) [64]))` | identical |  |
| convar | `ConCommand::ConCommand(char const*, void (*)(CCommand const&), char const*, int, int (*)(char const*, char (*) [64]))` | identical |  |
| convar | `ConCommand::Dispatch(CCommand const&)` | identical |  |
| convar | `ConCommand::IsCommand() const` | identical |  |
| convar | `ConCommand::~ConCommand()` | identical |  |
| convar | `ConCommand::~ConCommand()` | identical |  |
| convar | `ConCommand::~ConCommand()` | identical |  |
| convar | `ConCommandBase::AddFlags(int)` | identical |  |
| convar | `ConCommandBase::Create(char const*, char const*, int)` | edited | SDK 2013 ConVar (no TF2 competitive fields) |
| convar | `ConCommandBase::GetDLLIdentifier() const` | identical |  |
| convar | `ConCommandBase::GetHelpText() const` | identical |  |
| convar | `ConCommandBase::GetName() const` | identical |  |
| convar | `ConCommandBase::GetNext()` | identical |  |
| convar | `ConCommandBase::GetNext() const` | identical |  |
| convar | `ConCommandBase::Init()` | identical |  |
| convar | `ConCommandBase::IsFlagSet(int) const` | identical |  |
| convar | `ConCommandBase::IsRegistered() const` | identical |  |
| convar | `ConCommandBase::Shutdown()` | identical |  |
| convar | `ConVar::AddFlags(int)` | identical |  |
| convar | `ConVar::ChangeStringValue(char const*, float)` | edited | SDK 2013 ConVar (no TF2 competitive fields) |
| convar | `ConVar::ClampValue(float&)` | edited | SDK 2013 ConVar (no TF2 competitive fields) |
| convar | `ConVar::ConVar(char const*, char const*, int)` | edited | SDK 2013 ConVar (no TF2 competitive fields) |
| convar | `ConVar::ConVar(char const*, char const*, int)` | edited | SDK 2013 ConVar (no TF2 competitive fields) |
| convar | `ConVar::ConVar(char const*, char const*, int, char const*)` | edited | SDK 2013 ConVar (no TF2 competitive fields) |
| convar | `ConVar::ConVar(char const*, char const*, int, char const*)` | edited | SDK 2013 ConVar (no TF2 competitive fields) |
| convar | `ConVar::ConVar(char const*, char const*, int, char const*, bool, float, bool, float)` | edited | SDK 2013 ConVar (no TF2 competitive fields) |
| convar | `ConVar::ConVar(char const*, char const*, int, char const*, bool, float, bool, float)` | edited | SDK 2013 ConVar (no TF2 competitive fields) |
| convar | `ConVar::ConVar(char const*, char const*, int, char const*, bool, float, bool, float, void (*)(IConVar*, char const*, float))` | edited | SDK 2013 ConVar (no TF2 competitive fields) |
| convar | `ConVar::ConVar(char const*, char const*, int, char const*, bool, float, bool, float, void (*)(IConVar*, char const*, float))` | edited | SDK 2013 ConVar (no TF2 competitive fields) |
| convar | `ConVar::ConVar(char const*, char const*, int, char const*, void (*)(IConVar*, char const*, float))` | edited | SDK 2013 ConVar (no TF2 competitive fields) |
| convar | `ConVar::ConVar(char const*, char const*, int, char const*, void (*)(IConVar*, char const*, float))` | edited | SDK 2013 ConVar (no TF2 competitive fields) |
| convar | `ConVar::Create(char const*, char const*, int, char const*, bool, float, bool, float, void (*)(IConVar*, char const*, float))` | edited | SDK 2013 ConVar (no TF2 competitive fields) |
| convar | `ConVar::GetDefault() const` | identical |  |
| convar | `ConVar::GetHelpText() const` | identical |  |
| convar | `ConVar::GetName() const` | identical |  |
| convar | `ConVar::Init()` | identical |  |
| convar | `ConVar::InstallChangeCallback(void (*)(IConVar*, char const*, float))` | edited | SDK 2013 ConVar (no TF2 competitive fields) |
| convar | `ConVar::InternalSetFloatValue(float)` | edited | SDK 2013 ConVar (no TF2 competitive fields) |
| convar | `ConVar::InternalSetIntValue(int)` | identical |  |
| convar | `ConVar::InternalSetValue(char const*)` | identical |  |
| convar | `ConVar::IsCommand() const` | identical |  |
| convar | `ConVar::IsFlagSet(int) const` | identical |  |
| convar | `ConVar::IsRegistered() const` | identical |  |
| convar | `ConVar::SetValue(char const*)` | identical |  |
| convar | `ConVar::SetValue(float)` | edited | SDK 2013 ConVar (no TF2 competitive fields) |
| convar | `ConVar::SetValue(int)` | identical |  |
| convar | `ConVar::~ConVar()` | identical |  |
| convar | `ConVar::~ConVar()` | identical |  |
| convar | `ConVar::~ConVar()` | identical |  |
| convar | `ConVarRef::ConVarRef(IConVar*)` | identical |  |
| convar | `ConVarRef::ConVarRef(char const*)` | identical |  |
| convar | `ConVarRef::ConVarRef(char const*, bool)` | identical |  |
| convar | `ConVarRef::Init(char const*, bool)` | identical |  |
| convar | `ConVarRef::IsValid() const` | identical |  |
| convar | `ConVar_PrintDescription(ConCommandBase const*)` | edited | SDK 2013 ConVar (no TF2 competitive fields) |
| convar | `ConVar_PrintFlags(ConCommandBase const*)` | identical |  |
| convar | `ConVar_Register(int, IConCommandBaseAccessor*)` | identical |  |
| convar | `ConVar_Unregister()` | identical |  |
| convar | `DefaultCompletionFunc(char const*, char (*) [64])` | identical |  |
| convar | `__GLOBAL__I_a` | edited | SDK 2013 ConVar (no TF2 competitive fields) |
| convar | `non-virtual thunk to CEmptyConVar::GetName() const` | identical |  |
| convar | `non-virtual thunk to CEmptyConVar::IsFlagSet(int) const` | identical |  |
| convar | `non-virtual thunk to CEmptyConVar::SetValue(char const*)` | identical |  |
| convar | `non-virtual thunk to CEmptyConVar::SetValue(float)` | identical |  |
| convar | `non-virtual thunk to CEmptyConVar::SetValue(int)` | identical |  |
| convar | `non-virtual thunk to ConVar::GetName() const` | identical |  |
| convar | `non-virtual thunk to ConVar::IsFlagSet(int) const` | identical |  |
| convar | `non-virtual thunk to ConVar::SetValue(char const*)` | identical |  |
| convar | `non-virtual thunk to ConVar::SetValue(float)` | edited | SDK 2013 ConVar (no TF2 competitive fields) |
| convar | `non-virtual thunk to ConVar::SetValue(int)` | identical |  |
| datamanager | `CDataManagerBase::BreakLock(memhandle_t__*)` | identical |  |
| datamanager | `CDataManagerBase::CDataManagerBase(unsigned int)` | edited | SDK CDataManagerBase/CUtlMultiList |
| datamanager | `CDataManagerBase::CreateHandle(bool)` | identical |  |
| datamanager | `CDataManagerBase::DestroyResource(memhandle_t__*)` | identical |  |
| datamanager | `CDataManagerBase::EnsureCapacity(unsigned int)` | identical |  |
| datamanager | `CDataManagerBase::FlushAll()` | edited | SDK CDataManagerBase/CUtlMultiList |
| datamanager | `CDataManagerBase::GetForFreeByIndex(unsigned short)` | identical |  |
| datamanager | `CDataManagerBase::Lock()` | identical |  |
| datamanager | `CDataManagerBase::LockResource(memhandle_t__*)` | identical |  |
| datamanager | `CDataManagerBase::StoreResourceInHandle(unsigned short, void*, unsigned int)` | identical |  |
| datamanager | `CDataManagerBase::TryLock()` | identical |  |
| datamanager | `CDataManagerBase::Unlock()` | identical |  |
| datamanager | `CDataManagerBase::UnlockResource(memhandle_t__*)` | identical |  |
| datamanager | `CDataManagerBase::~CDataManagerBase()` | identical |  |
| datamanager | `CDataManagerBase::~CDataManagerBase()` | identical |  |
| datamanager | `CDataManagerBase::~CDataManagerBase()` | identical |  |
| datamanager | `CUtlLinkedList<CUtlMultiList<CDataManagerBase::resource_lru_element_t, unsigned short>::List_t, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<CUtlMultiList<CDataManagerBase::resource_lru_element_t, unsigned short>::List_t, unsigned short>, unsigned short>>::AllocInternal(bool)` | edited | SDK CDataManagerBase/CUtlMultiList |
| datamanager | `CUtlLinkedList<CUtlMultiList<CDataManagerBase::resource_lru_element_t, unsigned short>::List_t, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<CUtlMultiList<CDataManagerBase::resource_lru_element_t, unsigned short>::List_t, unsigned short>, unsigned short>>::LinkBefore(unsigned short, unsigned short)` | edited | SDK CDataManagerBase/CUtlMultiList |
| datamanager | `CUtlMemory<CUtlMultiList<CDataManagerBase::resource_lru_element_t, unsigned short>::ListElem_t, int>::Grow(int)` | edited | SDK CDataManagerBase/CUtlMultiList |
| datamanager | `CUtlMemory<UtlLinkedListElem_t<CUtlMultiList<CDataManagerBase::resource_lru_element_t, unsigned short>::List_t, unsigned short>, unsigned short>::Grow(int)` | edited | SDK CDataManagerBase/CUtlMultiList |
| datamanager | `CUtlMultiList<CDataManagerBase::resource_lru_element_t, unsigned short>::Alloc()` | edited | SDK CDataManagerBase/CUtlMultiList |
| datamanager | `CUtlMultiList<CDataManagerBase::resource_lru_element_t, unsigned short>::LinkBefore(unsigned short, unsigned short, unsigned short)` | identical |  |
| datamanager | `CUtlMultiList<CDataManagerBase::resource_lru_element_t, unsigned short>::~CUtlMultiList()` | identical |  |
| generichash | `Hash12(void const*)` | identical |  |
| generichash | `HashBlock(void const*, unsigned int)` | identical |  |
| ilocalize | `ILocalize::ConstructStringVArgsInternal(wchar_t*, int, wchar_t const*, int, char*)` | identical |  |
| ilocalize | `ILocalize::ConvertANSIToUnicode(char const*, wchar_t*, int)` | edited | SDK V_UTF8ToUnicode semantics |
| ilocalize | `ILocalize::ConvertUnicodeToANSI(wchar_t const*, char*, int)` | edited | SDK V_UTF8ToUnicode semantics |
| ilocalize | `void ConstructStringVArgsInternal_Impl<wchar_t>(wchar_t*, int, wchar_t const*, int, char*)` | identical |  |
| interface | `CreateInterfaceInternal(char const*, int*)` | identical |  |
| interface | `InterfaceReg::InterfaceReg(void* (*)(), char const*)` | identical |  |
| interface | `Sys_GetFactory(CSysModule*)` | identical |  |
| interface | `Sys_GetFactoryThis()` | identical |  |
| interface | `Sys_IsDebuggerPresent()` | edited | header change |
| interface | `Sys_LoadLibrary(char const*, Sys_Flags)` | identical |  |
| interface | `Sys_LoadModule(char const*, Sys_Flags)` | kept (fork) | kept: fork lib*/bin/ library search needed by our runtime layout |
| interface | `Sys_UnloadModule(CSysModule*)` | identical |  |
| interface | `_CreateInterface` | identical |  |
| lzmaDecoder | `CLZMA::GetActualSize(unsigned char*)` | edited | LZMA SDK 4.43 |
| lzmaDecoder | `CLZMA::IsCompressed(unsigned char*)` | edited | LZMA SDK 4.43 |
| lzmaDecoder | `CLZMA::Uncompress(unsigned char*, unsigned char*)` | edited | LZMA SDK 4.43 |
| lzmaDecoder | `LzmaDecode(_CLzmaDecoderState*, unsigned char const*, unsigned int, unsigned int*, unsigned char*, unsigned int, unsigned int*)` | edited | LZMA SDK 4.43 |
| lzss | `CLZSS::Compress(unsigned char const*, int, unsigned int*)` | identical |  |
| lzss | `CLZSS::CompressNoAlloc(unsigned char const*, int, unsigned char*, unsigned int*)` | edited | retail SafeUncompress signature |
| lzss | `CLZSS::GetActualSize(unsigned char const*)` | identical |  |
| lzss | `CLZSS::SafeUncompress(unsigned char const*, unsigned char*, unsigned int)` | edited | restored three-argument leak version; the fork's four-argument overload is kept |
| lzss | `CLZSS::Uncompress(unsigned char const*, unsigned char*)` | identical |  |
| mempool | `CUtlMemoryPool::AddNewBlob()` | edited | SDK CUtlMemoryPool layout |
| mempool | `CUtlMemoryPool::Alloc()` | edited | SDK CUtlMemoryPool layout |
| mempool | `CUtlMemoryPool::Alloc(unsigned long)` | edited | SDK CUtlMemoryPool layout |
| mempool | `CUtlMemoryPool::CUtlMemoryPool(int, int, int, char const*, int)` | edited | SDK CUtlMemoryPool layout |
| mempool | `CUtlMemoryPool::CUtlMemoryPool(int, int, int, char const*, int)` | edited | SDK CUtlMemoryPool layout |
| mempool | `CUtlMemoryPool::Clear()` | edited | SDK CUtlMemoryPool layout |
| mempool | `CUtlMemoryPool::Free(void*)` | edited | SDK CUtlMemoryPool layout |
| mempool | `CUtlMemoryPool::~CUtlMemoryPool()` | edited | SDK CUtlMemoryPool layout |
| mempool | `CUtlMemoryPool::~CUtlMemoryPool()` | edited | SDK CUtlMemoryPool layout |
| memstack | `CMemoryStack::CMemoryStack()` | identical |  |
| memstack | `CMemoryStack::CMemoryStack()` | identical |  |
| memstack | `CMemoryStack::CommitTo(unsigned char*)` | identical |  |
| memstack | `CMemoryStack::FreeAll(bool)` | identical |  |
| memstack | `CMemoryStack::FreeToAllocPoint(unsigned int, bool)` | identical |  |
| memstack | `CMemoryStack::GetSize()` | identical |  |
| memstack | `CMemoryStack::Init(unsigned int, unsigned int, unsigned int, unsigned int)` | edited | tier0 headers (tier0 lane) |
| memstack | `CMemoryStack::~CMemoryStack()` | identical |  |
| memstack | `CMemoryStack::~CMemoryStack()` | identical |  |
| rangecheckedvar | `CDisableRangeChecks::CDisableRangeChecks()` | identical |  |
| rangecheckedvar | `CDisableRangeChecks::~CDisableRangeChecks()` | identical |  |
| snappy | `snappy::Compress(snappy::Source*, snappy::Sink*)` | edited | snappy 1.0.3 |
| snappy | `snappy::GetUncompressedLength(char const*, unsigned long, unsigned long*)` | identical |  |
| snappy | `snappy::LogMessageCrash::~LogMessageCrash()` | edited | snappy 1.0.3 |
| snappy | `snappy::LogMessageCrash::~LogMessageCrash()` | edited | snappy 1.0.3 |
| snappy | `snappy::MaxCompressedLength(unsigned long)` | identical |  |
| snappy | `snappy::RawCompress(char const*, unsigned long, char*, unsigned long*)` | identical |  |
| snappy | `snappy::RawUncompress(char const*, unsigned long, char*)` | identical |  |
| snappy | `snappy::RawUncompress(snappy::Source*, char*)` | identical |  |
| snappy | `snappy::SnappyDecompressor::RefillTag()` | edited | snappy 1.0.3 |
| snappy | `snappy::internal::CompressFragment(char const*, unsigned long, char*, unsigned short*, int)` | edited | snappy 1.0.3 |
| snappy | `snappy::internal::WorkingMemory::GetHashTable(unsigned long, int*)` | edited | snappy 1.0.3 |
| snappy | `void snappy::SnappyDecompressor::DecompressAllTags<snappy::SnappyArrayWriter>(snappy::SnappyArrayWriter*)` | edited | snappy 1.0.3 |
| snappy-sinksource | `snappy::ByteArraySource::Available() const` | identical |  |
| snappy-sinksource | `snappy::ByteArraySource::Peek(unsigned long*)` | identical |  |
| snappy-sinksource | `snappy::ByteArraySource::Skip(unsigned long)` | identical |  |
| snappy-sinksource | `snappy::ByteArraySource::~ByteArraySource()` | identical |  |
| snappy-sinksource | `snappy::ByteArraySource::~ByteArraySource()` | identical |  |
| snappy-sinksource | `snappy::UncheckedByteArraySink::Append(char const*, unsigned long)` | identical |  |
| snappy-sinksource | `snappy::UncheckedByteArraySink::GetAppendBuffer(unsigned long, char*)` | identical |  |
| snappy-sinksource | `snappy::UncheckedByteArraySink::~UncheckedByteArraySink()` | identical |  |
| snappy-sinksource | `snappy::UncheckedByteArraySink::~UncheckedByteArraySink()` | identical |  |
| stringpool | `CStringPool::Allocate(char const*)` | edited | SDK CUtlRBTree/CStringPool |
| stringpool | `CStringPool::CStringPool()` | edited | SDK CUtlRBTree/CStringPool |
| stringpool | `CStringPool::Find(char const*)` | edited | SDK CUtlRBTree/CStringPool |
| stringpool | `CStringPool::~CStringPool()` | edited | SDK CUtlRBTree/CStringPool |
| stringpool | `CStringPool::~CStringPool()` | identical |  |
| stringpool | `CUtlMemory<UtlRBTreeNode_t<char const*, unsigned short>, unsigned short>::Grow(int)` | edited | SDK CUtlRBTree/CStringPool |
| stringpool | `CUtlRBTree<char const*, unsigned short, bool (*)(char const* const&, char const* const&), CUtlMemory<UtlRBTreeNode_t<char const*, unsigned short>, unsigned short>>::Find(char const* const&) const` | edited | SDK CUtlRBTree/CStringPool |
| stringpool | `CUtlRBTree<char const*, unsigned short, bool (*)(char const* const&, char const* const&), CUtlMemory<UtlRBTreeNode_t<char const*, unsigned short>, unsigned short>>::FindInsertionPosition(char const* const&, unsigned short&, bool&)` | edited | SDK CUtlRBTree/CStringPool |
| stringpool | `CUtlRBTree<char const*, unsigned short, bool (*)(char const* const&, char const* const&), CUtlMemory<UtlRBTreeNode_t<char const*, unsigned short>, unsigned short>>::FirstInorder() const` | edited | SDK CUtlRBTree/CStringPool |
| stringpool | `CUtlRBTree<char const*, unsigned short, bool (*)(char const* const&, char const* const&), CUtlMemory<UtlRBTreeNode_t<char const*, unsigned short>, unsigned short>>::Insert(char const* const&)` | edited | SDK CUtlRBTree/CStringPool |
| stringpool | `CUtlRBTree<char const*, unsigned short, bool (*)(char const* const&, char const* const&), CUtlMemory<UtlRBTreeNode_t<char const*, unsigned short>, unsigned short>>::InsertRebalance(unsigned short)` | edited | SDK CUtlRBTree/CStringPool |
| stringpool | `CUtlRBTree<char const*, unsigned short, bool (*)(char const* const&, char const* const&), CUtlMemory<UtlRBTreeNode_t<char const*, unsigned short>, unsigned short>>::IsLeftChild(unsigned short) const` | edited | SDK CUtlRBTree/CStringPool |
| stringpool | `CUtlRBTree<char const*, unsigned short, bool (*)(char const* const&, char const* const&), CUtlMemory<UtlRBTreeNode_t<char const*, unsigned short>, unsigned short>>::IsRightChild(unsigned short) const` | edited | SDK CUtlRBTree/CStringPool |
| stringpool | `CUtlRBTree<char const*, unsigned short, bool (*)(char const* const&, char const* const&), CUtlMemory<UtlRBTreeNode_t<char const*, unsigned short>, unsigned short>>::NewNode()` | edited | SDK CUtlRBTree/CStringPool |
| stringpool | `CUtlRBTree<char const*, unsigned short, bool (*)(char const* const&, char const* const&), CUtlMemory<UtlRBTreeNode_t<char const*, unsigned short>, unsigned short>>::NextInorder(unsigned short) const` | edited | SDK CUtlRBTree/CStringPool |
| stringpool | `CUtlRBTree<char const*, unsigned short, bool (*)(char const* const&, char const* const&), CUtlMemory<UtlRBTreeNode_t<char const*, unsigned short>, unsigned short>>::RemoveAll()` | edited | SDK CUtlRBTree/CStringPool |
| stringpool | `CUtlRBTree<char const*, unsigned short, bool (*)(char const* const&, char const* const&), CUtlMemory<UtlRBTreeNode_t<char const*, unsigned short>, unsigned short>>::RotateLeft(unsigned short)` | edited | SDK CUtlRBTree/CStringPool |
| stringpool | `CUtlRBTree<char const*, unsigned short, bool (*)(char const* const&, char const* const&), CUtlMemory<UtlRBTreeNode_t<char const*, unsigned short>, unsigned short>>::RotateRight(unsigned short)` | edited | SDK CUtlRBTree/CStringPool |
| stringpool | `StrLess(char const* const&, char const* const&)` | edited | SDK CUtlRBTree/CStringPool |
| strtools | `Q_StripPrecedingAndTrailingWhitespace(char*)` | edited | SDK conversions, min/max macros |
| strtools | `StringAfterPrefix(char const*, char const*)` | identical |  |
| strtools | `V_AppendSlash(char*, int)` | identical |  |
| strtools | `V_ComposeFileName(char const*, char const*, char*, int)` | identical |  |
| strtools | `V_DefaultExtension(char*, char const*, int)` | identical |  |
| strtools | `V_ExtractFileExtension(char const*, char*, int)` | identical |  |
| strtools | `V_ExtractFilePath(char const*, char*, int)` | edited | SDK conversions, min/max macros |
| strtools | `V_FileBase(char const*, char*, int)` | identical |  |
| strtools | `V_FixDoubleSlashes(char*)` | identical |  |
| strtools | `V_FixSlashes(char*, char)` | identical |  |
| strtools | `V_FixupPathName(char*, unsigned long, char const*)` | identical |  |
| strtools | `V_GetFileExtension(char const*)` | identical |  |
| strtools | `V_GetFileName(char const*)` | edited | SDK conversions, min/max macros |
| strtools | `V_IsAbsolutePath(char const*)` | identical |  |
| strtools | `V_MakeAbsolutePath(char*, int, char const*, char const*)` | identical |  |
| strtools | `V_RemoveDotSlashes(char*, char, bool)` | identical |  |
| strtools | `V_SetExtension(char*, char const*, int)` | edited | SDK conversions, min/max macros |
| strtools | `V_SplitString(char const*, char const*, CUtlVector<char*, CUtlMemory<char*, int>>&)` | identical |  |
| strtools | `V_SplitString2(char const*, char const**, int, CUtlVector<char*, CUtlMemory<char*, int>>&)` | identical |  |
| strtools | `V_StrLeft(char const*, int, char*, int)` | identical |  |
| strtools | `V_StrRight(char const*, int, char*, int)` | identical |  |
| strtools | `V_StripExtension(char const*, char*, int)` | edited | SDK conversions, min/max macros |
| strtools | `V_StripFilename(char*)` | identical |  |
| strtools | `V_StripLastDir(char*, int)` | identical |  |
| strtools | `V_StripTrailingSlash(char*)` | identical |  |
| strtools | `V_UTF8ToUnicode(char const*, wchar_t*, int)` | edited | SDK conversions, min/max macros |
| strtools | `V_UnicodeToUTF8(wchar_t const*, char*, int)` | edited | SDK conversions, min/max macros |
| strtools | `V_UnqualifiedFileName(char const*)` | edited | SDK conversions, min/max macros |
| strtools | `V_atof(char const*)` | edited | SDK conversions, min/max macros |
| strtools | `V_atoi(char const*)` | edited | SDK conversions, min/max macros |
| strtools | `V_atoi64(char const*)` | edited | SDK conversions, min/max macros |
| strtools | `V_atoui64(char const*)` | identical |  |
| strtools | `V_binarytohex(unsigned char const*, int, char*, int)` | identical |  |
| strtools | `V_hextobinary(char const*, int, unsigned char*, int)` | edited | SDK conversions, min/max macros |
| strtools | `V_pretifymem(float, int, bool)` | identical |  |
| strtools | `V_snprintf(char*, int, char const*, ...)` | identical |  |
| strtools | `V_stricmp(char const*, char const*)` | identical |  |
| strtools | `V_stristr(char const*, char const*)` | identical |  |
| strtools | `V_stristr(char*, char const*)` | identical |  |
| strtools | `V_strlower(char*)` | identical |  |
| strtools | `V_strncat(char*, char const*, unsigned long, int)` | identical |  |
| strtools | `V_strncmp(char const*, char const*, int)` | identical |  |
| strtools | `V_strncpy(char*, char const*, int)` | identical |  |
| strtools | `V_strnicmp(char const*, char const*, int)` | identical |  |
| strtools | `V_strnistr(char const*, char const*, int)` | identical |  |
| strtools | `V_vsnprintf(char*, int, char const*, char*)` | identical |  |
| strtools | `V_vsnprintfRet(char*, int, char const*, char*, bool*)` | identical |  |
| strtools | `V_vsnwprintf(wchar_t*, int, wchar_t const*, char*)` | identical |  |
| strtools | `V_wcsncpy(wchar_t*, wchar_t const*, int)` | identical |  |
| tier1 | `ConnectTier1Libraries(void* (**)(char const*, int*), int)` | identical |  |
| tier1 | `DisconnectTier1Libraries()` | identical |  |
| uniqueid | `CreateUniqueId(UniqueId_t*)` | identical |  |
| uniqueid | `InvalidateUniqueId(UniqueId_t*)` | identical |  |
| uniqueid | `Serialize(CUtlBuffer&, UniqueId_t const&)` | identical |  |
| utlbuffer | `CUtlBuffer::AddNullTermination()` | edited | SDK 2013 CUtlBuffer |
| utlbuffer | `CUtlBuffer::AssumeMemory(void*, int, int, int)` | edited | SDK 2013 CUtlBuffer |
| utlbuffer | `CUtlBuffer::CUtlBuffer(int, int, int)` | edited | SDK 2013 CUtlBuffer |
| utlbuffer | `CUtlBuffer::CUtlBuffer(int, int, int)` | identical |  |
| utlbuffer | `CUtlBuffer::CUtlBuffer(void const*, int, int)` | edited | SDK 2013 CUtlBuffer |
| utlbuffer | `CUtlBuffer::CheckArbitraryPeekGet(int, int&)` | identical |  |
| utlbuffer | `CUtlBuffer::CheckGet(int)` | identical |  |
| utlbuffer | `CUtlBuffer::CheckPut(int)` | identical |  |
| utlbuffer | `CUtlBuffer::EatCPPComment()` | edited | SDK 2013 CUtlBuffer |
| utlbuffer | `CUtlBuffer::EatWhiteSpace()` | identical |  |
| utlbuffer | `CUtlBuffer::EnsureCapacity(int)` | identical |  |
| utlbuffer | `CUtlBuffer::Get(void*, int)` | edited | SDK 2013 CUtlBuffer |
| utlbuffer | `CUtlBuffer::GetDelimitedCharInternal(CUtlCharConversion*)` | edited | SDK 2013 CUtlBuffer |
| utlbuffer | `CUtlBuffer::GetDelimitedString(CUtlCharConversion*, char*, int)` | identical |  |
| utlbuffer | `CUtlBuffer::GetOverflow(int)` | identical |  |
| utlbuffer | `CUtlBuffer::GetString(char*, int)` | edited | SDK 2013 CUtlBuffer |
| utlbuffer | `CUtlBuffer::GetUpTo(void*, int)` | identical |  |
| utlbuffer | `CUtlBuffer::ParseToken(characterset_t*, char*, int, bool)` | edited | SDK 2013 CUtlBuffer |
| utlbuffer | `CUtlBuffer::PeekGet(int, int)` | identical |  |
| utlbuffer | `CUtlBuffer::PeekStringLength()` | identical |  |
| utlbuffer | `CUtlBuffer::PeekStringMatch(int, char const*, int)` | identical |  |
| utlbuffer | `CUtlBuffer::PeekWhiteSpace(int)` | identical |  |
| utlbuffer | `CUtlBuffer::Printf(char const*, ...)` | edited | SDK 2013 CUtlBuffer |
| utlbuffer | `CUtlBuffer::Put(void const*, int)` | edited | SDK 2013 CUtlBuffer |
| utlbuffer | `CUtlBuffer::PutChar(char)` | edited | SDK 2013 CUtlBuffer |
| utlbuffer | `CUtlBuffer::PutDelimitedString(CUtlCharConversion*, char const*)` | edited | SDK 2013 CUtlBuffer |
| utlbuffer | `CUtlBuffer::PutOverflow(int)` | identical |  |
| utlbuffer | `CUtlBuffer::PutString(char const*)` | edited | SDK 2013 CUtlBuffer |
| utlbuffer | `CUtlBuffer::Scanf(char const*, ...)` | identical |  |
| utlbuffer | `CUtlBuffer::SeekGet(CUtlBuffer::SeekType_t, int)` | identical |  |
| utlbuffer | `CUtlBuffer::SeekPut(CUtlBuffer::SeekType_t, int)` | edited | SDK 2013 CUtlBuffer |
| utlbuffer | `CUtlBuffer::SetBigEndian(bool)` | identical |  |
| utlbuffer | `CUtlBuffer::SetExternalBuffer(void*, int, int, int)` | edited | SDK 2013 CUtlBuffer |
| utlbuffer | `CUtlBuffer::SetOverflowFuncs(bool (CUtlBuffer::*)(int), bool (CUtlBuffer::*)(int))` | identical |  |
| utlbuffer | `CUtlBuffer::VaScanf(char const*, char*)` | edited | SDK 2013 CUtlBuffer |
| utlbuffer | `CUtlCStringConversion::CUtlCStringConversion(char, char const*, int, CUtlCharConversion::ConversionArray_t*)` | identical |  |
| utlbuffer | `CUtlCStringConversion::FindConversion(char const*, int*)` | identical |  |
| utlbuffer | `CUtlCharConversion::FindConversion(char const*, int*)` | identical |  |
| utlbuffer | `CUtlNoEscConversion::FindConversion(char const*, int*)` | identical |  |
| utlbuffer | `GetCStringCharConversion()` | identical |  |
| utlbuffer | `GetNoEscCharConversion()` | identical |  |
| utlbuffer | `__GLOBAL__I_a` | identical |  |
| utlbufferutil | `Serialize(CUtlBuffer&, CUtlBinaryBlock const&)` | edited | follows CUtlBuffer/CUtlString |
| utlbufferutil | `Serialize(CUtlBuffer&, CUtlString const&)` | identical |  |
| utlbufferutil | `Serialize(CUtlBuffer&, Color const&)` | edited | follows CUtlBuffer/CUtlString |
| utlbufferutil | `Serialize(CUtlBuffer&, QAngle const&)` | edited | follows CUtlBuffer/CUtlString |
| utlbufferutil | `Serialize(CUtlBuffer&, Quaternion const&)` | edited | follows CUtlBuffer/CUtlString |
| utlbufferutil | `Serialize(CUtlBuffer&, VMatrix const&)` | edited | follows CUtlBuffer/CUtlString |
| utlbufferutil | `Serialize(CUtlBuffer&, Vector const&)` | edited | follows CUtlBuffer/CUtlString |
| utlbufferutil | `Serialize(CUtlBuffer&, Vector2D const&)` | edited | follows CUtlBuffer/CUtlString |
| utlbufferutil | `Serialize(CUtlBuffer&, Vector4D const&)` | edited | follows CUtlBuffer/CUtlString |
| utlbufferutil | `Serialize(CUtlBuffer&, bool const&)` | edited | follows CUtlBuffer/CUtlString |
| utlbufferutil | `Serialize(CUtlBuffer&, float const&)` | edited | follows CUtlBuffer/CUtlString |
| utlbufferutil | `Serialize(CUtlBuffer&, int const&)` | edited | follows CUtlBuffer/CUtlString |
| utlbufferutil | `SetSerializationArrayDelimiter(char const*)` | identical |  |
| utlbufferutil | `SetSerializationDelimiter(CUtlCharConversion*)` | identical |  |
| utlstring | `CUtlBinaryBlock::CUtlBinaryBlock(int, int)` | edited | SDK 2013 CUtlString/CUtlBinaryBlock |
| utlstring | `CUtlBinaryBlock::Set(void const*, int)` | edited | SDK 2013 CUtlString/CUtlBinaryBlock |
| utlstring | `CUtlString::Append(char const*)` | edited | SDK 2013 CUtlString/CUtlBinaryBlock |
| utlstring | `CUtlString::CUtlString()` | edited | SDK 2013 CUtlString/CUtlBinaryBlock |
| utlstring | `CUtlString::CUtlString(CUtlString const&)` | edited | SDK 2013 CUtlString/CUtlBinaryBlock |
| utlstring | `CUtlString::CUtlString(char const*)` | edited | SDK 2013 CUtlString/CUtlBinaryBlock |
| utlstring | `CUtlString::Format(char const*, ...)` | edited | SDK 2013 CUtlString/CUtlBinaryBlock |
| utlstring | `CUtlString::Get()` | edited | SDK 2013 CUtlString/CUtlBinaryBlock |
| utlstring | `CUtlString::Get() const` | edited | SDK 2013 CUtlString/CUtlBinaryBlock |
| utlstring | `CUtlString::Length() const` | edited | SDK 2013 CUtlString/CUtlBinaryBlock |
| utlstring | `CUtlString::Set(char const*)` | edited | SDK 2013 CUtlString/CUtlBinaryBlock |
| utlstring | `CUtlString::operator char const*() const` | edited | SDK 2013 CUtlString/CUtlBinaryBlock |
| utlstring | `CUtlString::operator+=(char const*)` | edited | SDK 2013 CUtlString/CUtlBinaryBlock |
| utlstring | `CUtlString::operator=(CUtlString const&)` | edited | SDK 2013 CUtlString/CUtlBinaryBlock |
| utlstring | `CUtlString::operator=(char const*)` | edited | SDK 2013 CUtlString/CUtlBinaryBlock |
| utlsymbol | `CCleanupUtlSymbolTable::~CCleanupUtlSymbolTable()` | edited | SDK + retail thread-local search string |
| utlsymbol | `CThreadLocalPtr<char>::~CThreadLocalPtr()` | edited | SDK + retail thread-local search string |
| utlsymbol | `CUtlMemory<CUtlSymbolTable::StringPool_t*, int>::Grow(int)` | edited | SDK + retail thread-local search string |
| utlsymbol | `CUtlMemory<UtlRBTreeNode_t<CUtlSymbolTable::CStringPoolIndex, unsigned short>, unsigned short>::Grow(int)` | edited | SDK + retail thread-local search string |
| utlsymbol | `CUtlRBTree<CUtlSymbolTable::CStringPoolIndex, unsigned short, CUtlSymbolTable::CLess, CUtlMemory<UtlRBTreeNode_t<CUtlSymbolTable::CStringPoolIndex, unsigned short>, unsigned short>>::Find(CUtlSymbolTable::CStringPoolIndex const&) const` | edited | SDK + retail thread-local search string |
| utlsymbol | `CUtlRBTree<CUtlSymbolTable::CStringPoolIndex, unsigned short, CUtlSymbolTable::CLess, CUtlMemory<UtlRBTreeNode_t<CUtlSymbolTable::CStringPoolIndex, unsigned short>, unsigned short>>::FindInsertionPosition(CUtlSymbolTable::CStringPoolIndex const&, unsigned short&, bool&)` | edited | SDK + retail thread-local search string |
| utlsymbol | `CUtlRBTree<CUtlSymbolTable::CStringPoolIndex, unsigned short, CUtlSymbolTable::CLess, CUtlMemory<UtlRBTreeNode_t<CUtlSymbolTable::CStringPoolIndex, unsigned short>, unsigned short>>::Insert(CUtlSymbolTable::CStringPoolIndex const&)` | edited | SDK + retail thread-local search string |
| utlsymbol | `CUtlRBTree<CUtlSymbolTable::CStringPoolIndex, unsigned short, CUtlSymbolTable::CLess, CUtlMemory<UtlRBTreeNode_t<CUtlSymbolTable::CStringPoolIndex, unsigned short>, unsigned short>>::InsertRebalance(unsigned short)` | edited | SDK + retail thread-local search string |
| utlsymbol | `CUtlRBTree<CUtlSymbolTable::CStringPoolIndex, unsigned short, CUtlSymbolTable::CLess, CUtlMemory<UtlRBTreeNode_t<CUtlSymbolTable::CStringPoolIndex, unsigned short>, unsigned short>>::IsLeftChild(unsigned short) const` | edited | SDK + retail thread-local search string |
| utlsymbol | `CUtlRBTree<CUtlSymbolTable::CStringPoolIndex, unsigned short, CUtlSymbolTable::CLess, CUtlMemory<UtlRBTreeNode_t<CUtlSymbolTable::CStringPoolIndex, unsigned short>, unsigned short>>::IsRightChild(unsigned short) const` | edited | SDK + retail thread-local search string |
| utlsymbol | `CUtlRBTree<CUtlSymbolTable::CStringPoolIndex, unsigned short, CUtlSymbolTable::CLess, CUtlMemory<UtlRBTreeNode_t<CUtlSymbolTable::CStringPoolIndex, unsigned short>, unsigned short>>::NewNode()` | edited | SDK + retail thread-local search string |
| utlsymbol | `CUtlRBTree<CUtlSymbolTable::CStringPoolIndex, unsigned short, CUtlSymbolTable::CLess, CUtlMemory<UtlRBTreeNode_t<CUtlSymbolTable::CStringPoolIndex, unsigned short>, unsigned short>>::RemoveAll()` | edited | SDK + retail thread-local search string |
| utlsymbol | `CUtlRBTree<CUtlSymbolTable::CStringPoolIndex, unsigned short, CUtlSymbolTable::CLess, CUtlMemory<UtlRBTreeNode_t<CUtlSymbolTable::CStringPoolIndex, unsigned short>, unsigned short>>::RotateLeft(unsigned short)` | edited | SDK + retail thread-local search string |
| utlsymbol | `CUtlRBTree<CUtlSymbolTable::CStringPoolIndex, unsigned short, CUtlSymbolTable::CLess, CUtlMemory<UtlRBTreeNode_t<CUtlSymbolTable::CStringPoolIndex, unsigned short>, unsigned short>>::RotateRight(unsigned short)` | edited | SDK + retail thread-local search string |
| utlsymbol | `CUtlSymbol::CUtlSymbol(char const*)` | edited | SDK + retail thread-local search string |
| utlsymbol | `CUtlSymbol::Initialize()` | edited | SDK + retail thread-local search string |
| utlsymbol | `CUtlSymbol::String() const` | identical |  |
| utlsymbol | `CUtlSymbolTable::AddString(char const*)` | edited | SDK + retail thread-local search string |
| utlsymbol | `CUtlSymbolTable::CLess::operator()(CUtlSymbolTable::CStringPoolIndex const&, CUtlSymbolTable::CStringPoolIndex const&) const` | edited | SDK + retail thread-local search string |
| utlsymbol | `CUtlSymbolTable::CUtlSymbolTable(int, int, bool)` | edited | SDK + retail thread-local search string |
| utlsymbol | `CUtlSymbolTable::CUtlSymbolTable(int, int, bool)` | edited | SDK + retail thread-local search string |
| utlsymbol | `CUtlSymbolTable::Find(char const*) const` | edited | SDK + retail thread-local search string |
| utlsymbol | `CUtlSymbolTable::RemoveAll()` | edited | SDK + retail thread-local search string |
| utlsymbol | `CUtlSymbolTable::String(CUtlSymbol) const` | edited | SDK + retail thread-local search string |
| utlsymbol | `CUtlSymbolTable::~CUtlSymbolTable()` | edited | SDK + retail thread-local search string |
| utlsymbol | `CUtlSymbolTable::~CUtlSymbolTable()` | identical |  |
| utlsymbol | `__GLOBAL__I_a` | edited | SDK + retail thread-local search string |
