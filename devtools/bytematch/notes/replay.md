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
