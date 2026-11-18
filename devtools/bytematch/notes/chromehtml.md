# chromehtml

Retail: depot 223 `bin/chromehtml.dylib`. Objects (debug map): `jpegloader`,
`jpegloader_common`, `htmlprotobuf`, `fileio`, `chromewrapper`,
`htmlmessages.pb`, `html_chrome`, `html_mac`, then 45 members of the prebuilt
`lib/public/osx32/libcef_dll_wrapper.a` (CEF's C++ wrapper, third party) and
`libtier1.a(interface, strtools, utlbuffer, utlstring)`. protobuf 2.3.0,
libjpeg and libpng are linked from prebuilt archives without stabs.

None of the Valve sources were in this tree. Ancestors found: SDK 2013
`public/html/*` (its `IHTMLChromeController` "ChromeHTML_Controller_001"
matches retail slot for slot). No public ancestor exists for `chromewrapper.cpp`,
`html_chrome.cpp`, `html_mac.mm` or `htmlmessages.proto`.

`--retail-osx32` builds the module as objects only (no link) until
`html_chrome.cpp` exists; the wscript comment says what to change.

| object | functions | matched | note |
|---|---:|---:|---|
| htmlmessages.pb | 1494 | 1494 | generated, see below |
| chromewrapper | 113 | 112 | written from disassembly; `RunFrame` differs |
| jpegloader, jpegloader_common | 6 | 6 | existing source |
| fileio | 4 | 4 | existing source |
| htmlprotobuf | 2 | 2 | SDK 2013 |
| html_mac | 2 | 2 | written from disassembly |
| html_chrome | 461 | 0 | not written (needs Valve's patched CEF 1 headers) |

## htmlmessages.proto

`protobuf_AddDesc_htmlmessages_2eproto` passes the serialized
`FileDescriptorProto` (7235 bytes at 0xd2a60) to `InternalAddGeneratedFile`.
Decoding it gives 92 flat messages, no package, `optimize_for = SPEED`,
`cc_generic_services = false`; `vgui2/chromehtml/htmlmessages.proto` is that
descriptor printed as .proto. protoc 2.3.0 (built from SDK 2013
`src/thirdparty/protobuf-2.3.0`; modern libc++ needs `#include <istream>` and
`<ostream>` added to `message.cc`) regenerates a byte-identical descriptor:

    cd vgui2/chromehtml && protoc --proto_path=. --cpp_out=generated_proto htmlmessages.proto

The generated files are committed so the build doesn't need protoc 2.3.0.
Headers and `libprotobuf.a` come from SDK 2013 (`SOURCE_SDK_2013`).

## chromewrapper.cpp

| function | status | note |
|---|---|---|
| CHTMLController::CreateBrowser, RemoveBrowser | written new | request map (+4), serial (+0x28), browser map (+0x2c), frame rate (+0x50) |
| CHTMLController::Init, Shutdown, SetWebCookie, SetClientBuildID, GetWebCookiesForURL, BHasPendingMessages, SetCefThreadTargetFrameRate, WakeThread, GetFreeCommandBuffer, PushCommand, dtors | written new | forwarders to `html_chrome.cpp` / `CCEFThread` |
| CHTMLController::RunFrame | written new, differs | same calls and control flow; register allocation differs (our vector size lives in ebx and is spilled in every case; retail keeps it in memory). Case order comes from the retail stack-slot order (IHTMLResponses order with `BrowserCreateResponse` first); that also fixed the hoisting order |
| CHTMLProtoBufMsg<T> dtors, CUtlMap/CUtlRBTree<uint32, IHTMLResponses*> | identical | headers |

## html_chrome.cpp (not written)

`html_chrome.h` declares `CCEFThread` with the member layout from retail's
constructor (CThread base, queues at +0x220/+0x258, unused-buffer list at
+0x290, frame rate at +0x348, size-change map at +0x350); `SizeChange_t`
field names are provisional (16 bytes). The rest of the file (CClientHandler,
CChromePainter, CChromeRenderBuffer, CookieVisitor, the Thread* command
handlers, about 100 KB of code) needs CEF 1 headers.

CEF: `libcef.dylib` reports `cef_build_revision()` = 1248, CEF branch 1364
(Chrome 27), git commit 80fc925b in chromiumembedded/cef, `cef1/include`, with
UTF-8 `cef_string_t`. Valve patched that API; the additions can be recovered
from the wrapper code linked into chromehtml.dylib (C struct slot offsets in
the ctocpp/cpptoc functions):

- CefBrowser: ExitFullScreen, GetLinkAtPosition, HorizontalScroll,
  HorizontalScrollBarSize, HorizontalScrollMax, IsHorizontalScrollBarVisible,
  IsVeritcalScrollBarVisible, IsVisuallyNonEmpty, Paint, SetHorizontalScroll,
  SetShowScrollBars, SetUserAgent, SetVerticalScroll, VerticalScroll,
  VerticalScrollBarSize, VerticalScrollMax, pageScaleFactor,
  scalePageToFitElementAt, scalePageToFocusedElement, setPageScaleFactor
- CefRequest: BIsSecureRequest, SecurityDetails, SetSecurityDetails; new
  CefSecurityDetails class
- CefDragData: GetFileExtension
- handler callbacks used by CClientHandler/CChromePainter that upstream lacks:
  OnFocusedNodeChanged, OnNewTab, OnEnterFullScreen, OnFileOpenDialog
  (`_cef_web_file_chooser_callback_t`), among others

Linking also needs a `libcef_dll_wrapper.a` built from the same patched CEF
(retail's is a 2013-07-25 prebuilt, not available).
