# replay

Retail: depot 223 `bin/replay.dylib` (49 own objects plus `libreplay_common.a`).
Measured with `--retail-osx32` and `bytematch diff` on the linked dylib, scoped to
our objects; "matched" = exact or matches-after-masking with call targets checked.

| state | matched / in scope |
|---|---:|
| lane start (bytematch 64c10856) | 1336 / 1405 (`sv_ftp.o` not in scope: we had no such object) |
| this lane | 1441 / 1442 (`sv_ftp.o`, 37 functions, now in scope) |
| + `public/datamap.h` from SDK 2013 | 1442 / 1442 |

The one left is the inlined `CDatadescGeneratedNameHolder` destructor attributed
to `libvtf.a(vtf.o)`: retail's `datamap.h` deletes the names with `delete`, ours
with `delete[]`.

## Written from the disassembly (Valve stripped FTP offloading from the leak)

- `sv_ftp.h`/`sv_ftp.cpp` (new): `IFtp`, `CFtp` (ctor, `UseProxy`, `Go`,
  `ReadCallback`, the six `FTP_*` forwarders), `CFtpUploadJob` (ctor, `DoCleanup`,
  `DoExecute`, `FTP_*` overrides), `SV_CreateFtpUploadJob`,
  `replay_publish_simulate_delay_ftp`. Layouts follow the retail offsets
  (`CFtp` 0x530 bytes, `CFtpUploadJob` 0x2f4).
- `sv_fileservercleanup`: `CFtpFileDeleterJob` (`*DELE` commands sent with
  `CURLOPT_PREQUOTE`, 404 accepted), `SV_CreateFtpFileDeleterJob`, and
  `SV_CreateDeleteFileJob` choosing FTP or local by `replay_fileserver_offload_enable`
  (moved next to `MarkFileForDelete`, where retail emits it). Retail's deleter has
  0x208 bytes between its bases and `m_pCommands` that nothing touches; kept as
  `m_szUnused`.
- `sv_filepublish`: `CFtpFileserverPublisher`; `SV_PublishFile` picks it when the
  session manager's `ShouldOffload()` is set.
- `sv_cvars`: the eleven `replay_fileserver_offload_*` convars (hostname and proxy
  host carry the existing change callbacks, password is `FCVAR_PROTECTED`).
- `sv_publishtest`: `Test_RemoteWebServerCVars`, the FTP branch of
  `Test_FilePublish`, `bOffload` from the convar.
- `sv_recordingsessionmanager::OnSessionStart`, `CSessionRecorder::StartRecording`,
  `CSessionBlockPublisher::PublishBlock`, `CSessionInfoPublisher::Think`: read
  `replay_fileserver_offload_enable` again (the leak hardcoded `false`).

Dead-stripped functions that came back once referenced: `CBasePublishJob` ctor,
dtors and `SimulateDelay`; `CSimpleSpewer` dtors and `PrintMsg`;
`OnFileserver*HostnameChanged`; `CServerReplayContext::UpdateFileserver*IPFromHostname`;
tier1's callback `ConVar` constructor.

## Older than the leak

Retail replay predates two hardening passes in the leak:
- `V_strcpy` instead of `V_strcpy_safe` in `CReplayScreenshot::Read`,
  `CServerRecordingSessionBlock::Read`, `CSessionInfoPublisher::Think`,
  `CScreenshotManager::DoCaptureScreenshot`,
  `CReplayPerformanceManager::GeneratePerformanceFilename`,
  `CHttpDownloader::BeginDownload`, `Replay_GetDownloadURL` and (retail mode only)
  `ImgUtl_WriteGenericVMT` in `common/imageutils.cpp`.
- `CSessionBlockDownloader::AbortDownloadsAndCleanup` walks the linked list by index
  up to `Count()` (an old `FOR_EACH_VEC` on a `CUtlLinkedList`; our `FOR_EACH_VEC`
  rejects that, so the loop is written out).

replay is only built by `--retail-osx32`, so these edits are not gated.

## Whole-file identity

replay is byte-identical since round 6 of the identity lane (`identical.json`).

The last difference was in `imageutils.o`: retail's `___clang_call_terminate`
atom is 0x20 bytes ending in a 10-byte nop, so a coalesced function followed
it, and in the clients and GameUI the except-table labels after
`ImgUtl_ReadVTFAsRGBA` are one higher (3/8/33 against 3/7/32). Both come from
`ImgUtl_ReadJPEGAsRGBAFromBuffer` (name not recovered). The Linux `replay.so`
(gcc, no section GC) keeps it right after `ImgUtl_ReadJPEGAsRGBA`, with no
caller: it decodes a JPEG held in one `CUtlBuffer` into RGBA in another, through
a `jpeg_source_mgr` whose five callbacks are COMDAT inline functions (empty,
return 0, empty, return 0, empty: `FileSystemOpenDlg.cpp`'s `CJpegSourceMgr`
with its Asserts compiled out). On OSX the extern function is emitted in
`__text` between `ReadJPEGAsRGBA` and `ReadPNGAsRGBA` and dead-stripped. The
callbacks are static members installed by an inline constructor, so clang
creates them in the deferred-emission pass after `~Bitmap_t` has created
`___clang_call_terminate`; they land after it in `__textcoal_nt`, dead too.

Fixed in round 5 of the identity lane:
- libpng against zlib 1.2.5's headers (`build-thirdparty.py`).
- `CFtpDirListRetriever` in `sv_fileservercleanup.cpp`. Retail emits six more
  functions between the `CFtpFileDeleterJob` constructor and
  `SV_CreateFtpFileDeleterJob` (except-table labels). The Linux `replay.so`
  keeps the class: an abstract job that lists a remote directory into a
  buffer. Clang skips the complete constructor of an abstract class, so it
  gives exactly six functions: base constructor, two `IFtp` overrides with
  thunks, and the static write callback. Its dead 16-aligned typeinfo name
  also extends `CFtpFileDeleterJob`'s to 32 bytes, since an atom runs to the
  next symbol. That put `__ZTS4CFtp` at retail's address; no dead constant in
  `sv_ftp.o` was needed.
- `CFtpUploadJob` declares its `IFtp` overrides in retail's vtable order.
