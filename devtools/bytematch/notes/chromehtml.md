# chromehtml

Retail: depot 223 `bin/chromehtml.dylib`. Objects (debug map): `jpegloader`,
`jpegloader_common`, `htmlprotobuf`, `fileio`, `chromewrapper`,
`htmlmessages.pb`, `html_chrome`, `html_mac`, then 45 members of
`lib/public/osx32/libcef_dll_wrapper.a` (CEF's C++ wrapper) and
`libtier1.a(interface, strtools, utlbuffer, utlstring)`. protobuf 2.3.0,
libjpeg and libpng come from prebuilt archives without stabs.

`--retail-osx32` links the module against the depot's `libcef.dylib`, with
the wrapper built from `external/cef` (target `cef_dll_wrapper`).

| object | functions | matched | source |
|---|---:|---:|---|
| htmlmessages.pb | 1494 | 1494 | generated, see below |
| html_chrome | 461 | 461 | SDK 2013 (with SDK 2013's `CStrAutoEncode`, below) |
| chromewrapper | 113 | 113 | SDK 2013 |
| jpegloader, jpegloader_common, fileio, htmlprotobuf, html_mac | 14 | 14 | existing / SDK 2013 / disassembly |
| libtier1 members | 49 | 49 | tier1 |
| libcef_dll_wrapper (45 members) | 760 | 760 | CEF 1025 + Valve changes; Chromium's clang r147225 |

With `--data-check`, the 186 htmlmessages.pb functions that pass `__FILE__`
to protobuf's `GOOGLE_CHECK` differ only by Valve's absolute build path; the
identity flow (`devtools/bytematch/identity`) compiles with Valve's paths.
Under `--all-retail`, protobuf 2.3.0 (SDK 2013's osx32 `libprotobuf.a`, the
2014 archive unchanged) matches entirely and libpng 271/275.

## Valve sources: Source SDK 2013

Valve published `html_chrome.cpp`, `html_chrome.h`, `chromewrapper.cpp`,
`htmlmanager.h` and `html_mac.mm` in the SDK 2013 tree
(`src/vgui2/chromehtml`, last changed in 53e78c50, 2014-05-15). The latest
version is what retail compiled: every html_chrome function matches once
`CStrAutoEncode` has SDK 2013's layout. SDK 2013's first commit (39ed8757)
also has the chromehtml-based `vgui_controls/HTML.cpp`, the consumer of the
same messages.

`public/tier1/strtools.h` has SDK 2013's `CStrAutoEncode` in retail mode
(no `m_bCreatedUCS2`, `m_bCreatedUTF16` at +0xc, the destructor doesn't free
`m_pucs2`); with the pristine leak's version 11 html_chrome functions differ
(`CStrAutoEncode::PopulateUTF8/UTF16`, `CClientHandler::AddHeader` and the 8
functions that inline them).

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

## CEF (external/cef)

Valve's tree has CEF at `src/thirdparty/cef` (debug map N_SOL); here it is
`external/cef` (identity alias). `libcef.dylib` reports revision 1248 and
the user agent in html_chrome says Chrome/18.0.1025.166. The C++ and C API
match upstream CEF1 **branch 1025** (r826, commit 811d5c9), not 1364: no
geolocation handler or `OnFaviconURLChange`, `CefDragData::GetFileExtension`,
no V8 `IsValid`/`IsUInt`/stack traces, `_cef_settings_t` offsets as in 1025.
Commit 85f66912 imports 1025 unchanged; the next commits add Valve's
changes, recovered from the wrapper code and vtables in the retail dylib:

- UTF-8 `cef_string_t`; `CEF_REVISION` 1248 (`cef_version.h`).
- `CefBrowser`: Paint, IsVisuallyNonEmpty, SetShowScrollBars, scroll bar
  size/position/max/visibility getters and setters, SetUserAgent,
  GetLinkAtPosition, scalePageToFitElementAt, scalePageToFocusedElement,
  setPageScaleFactor, ExitFullScreen, pageScaleFactor (C slots 0xac-0xf8).
- `CefRequest`: SetSecurityDetails, BIsSecureRequest, SecurityDetails, and
  the new library-side `CefSecurityDetails` class.
- `CefLifeSpanHandler::OnNewTab`; `OnLoadStart(+bool)`,
  `OnLoadEnd(+CefRefPtr<CefRequest>)`, `OnBeforeBrowse(+bool isNewTabRequest)`.
- `CefRenderHandler`: OnSetCursor (`cef_handler_cursortype_t`, WebKit's
  cursor order), OnFileOpenDialog (passes a raw
  `_cef_web_file_chooser_callback_t*`, a C++ struct whose slot 0 is
  `OnFileChoose(const std::vector<std::wstring>&)`), OnEnterFullScreen,
  OnExitFullScreen.
- `CefInitialize` takes a third, optional string (Steam passes "").
- Mac `cef_window_info_t` has `m_bPopupWindow` after `m_bHidden` (0x34 bytes).
- `ERR_CONNECTION_TIMED_OUT` (-118), `ERR_EMPTY_PROXY_LIST` (-336).

The C API headers and cpptoc/ctocpp files were regenerated with 1025's
`tools/translator.py` (python 2.7; `float` added to its simple types, as the
mangled names need). `OnFileOpenDialog`'s slot can't be expressed to the
translator and is merged by hand. Generated files for unchanged classes are
kept as upstream shipped them. A layout check over 503 wrapper functions
(struct-size tests and CppToC size stores) agrees with retail everywhere
except memset lengths that retail's compiler merged with `attached_to_`.

Valve's request-side wrapper code differs from what the translator writes;
retail has: `load_handler_on_load_end` verifies `request` (not an optional
param), `SetSecurityDetails` unwraps into a local before the call,
`BIsSecureRequest` returns `_retval > 0`, and `SecurityDetails` returns NULL
without calling `Wrap` when the struct is NULL.

### Compiler

The wrapper archive (2013-07-25) came from Steam's CEF build
(`/Users/buildbot/buildslave_steam/cef_osx/build/dev/src/cef`; N_SOL paths are
`./include/...`, i.e. CEF's gyp-generated `cef.xcodeproj` run from `src/cef`).
Chromium's `build/common.gypi` points Xcode's `CC` at the checkout's own clang
(`third_party/llvm-build`, fetched by `tools/clang/scripts/update.sh`). That
clang is llvm.org svn **r147225**, the `CLANG_REVISION` Chromium trunk pinned
from 2011-12-24 to 2012-01-02. With common.gypi's Release flags (`-O3`,
`-fno-strict-aliasing`, `-fno-threadsafe-statics`, `-fvisibility=hidden
-fvisibility-inlines-hidden`, no exceptions or RTTI,
`-mmacosx-version-min=10.5`) all 760 functions match, 100% of bytes. The
18.0.1025 release CEF 1025 names pins r148911 (757); presumably Steam's
checkout kept an older `llvm-build`. `retailflags.CEF_WRAPPER_CLANG`/`cef_wrapper_cxxflags`
hold these; `build-clang.sh r147225` builds the compiler.

Per-revision scores (archive members against retail; "Valve flags" are the
module's -O2 retail flags, scores before the request_ctocpp/load_handler
source fixes add 4):

| compiler | flags | matched |
|---|---|---:|
| clang-600.0.57 (retail toolchain) | Valve | 281 |
| Apple clang-425.0.24 (Xcode 4.6, LLVM 3.2) | Valve | 348 |
| Apple clang-500.2.76 (Xcode 5.0, LLVM 3.3) | Valve | 348 |
| r179138, r182481 (Chromium, Apr/May 2013), LLVM 3.3 release | Valve | 348 |
| r184830, r186332 (Chromium, Jul 2013) | Valve | 346 |
| r148911 (18.0.1025 release pin) | Valve / Chromium | 743 / 753 |
| r148911, r148039, r149886, after the source fixes | Chromium | 757 |
| r148911, 10.6 deployment target (after the fixes) | Chromium | 752 |
| r148911, -O2 / -Os (before the fixes) | Chromium | 746 / 636 |
| r145842 (Chromium, Dec 2011), r146361 | Chromium | 728 |
| r146628, **r147225** | Chromium | **760** |
| r147434 (Chromium, Jan 2012) | Chromium | 759 |

Fingerprint: retail's bool getters zero the result with an 8-bit `xor cl, cl`
in each predecessor block; 3.2-3.3 emit one `xor al, al`, and r182928
(2013-05-30) switched to 32-bit `xor eax, eax`. A clang built with the host at
`-O0` gives byte-identical objects to the `-O3` host build.

## Link

The LC_LOAD_DYLIB list and object order equal retail's. Retail links a
`pngloader.o` whose code is all dead-stripped (ccache compile slot 22915,
between jpegloader_common and htmlprotobuf): it pulls libpng's read side, which
retail exports. `RETAIL_DEAD` links `common/pngloader.cpp` there, and our
dylib exports retail's 267 `png_*` symbols. `__text` is 823261 bytes against
retail's 823232 with r148911 (the 3 functions it misses).
