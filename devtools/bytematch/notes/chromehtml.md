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
| html_chrome | 461 | 450 | SDK 2013; 461 with the `CStrAutoEncode` change below |
| chromewrapper | 113 | 113 | SDK 2013 |
| jpegloader, jpegloader_common, fileio, htmlprotobuf, html_mac | 14 | 14 | existing / SDK 2013 / disassembly |
| libtier1 members | 49 | 49 | tier1 |
| libcef_dll_wrapper (45 members) | 760 | 281 | CEF 1025 + Valve changes; different compiler |

## Valve sources: Source SDK 2013

Valve published `html_chrome.cpp`, `html_chrome.h`, `chromewrapper.cpp`,
`htmlmanager.h` and `html_mac.mm` in the SDK 2013 tree
(`src/vgui2/chromehtml`, last changed in 53e78c50, 2014-05-15). The latest
version is what retail compiled: every html_chrome function matches once
`CStrAutoEncode` has SDK 2013's layout. SDK 2013's first commit (39ed8757)
also has the chromehtml-based `vgui_controls/HTML.cpp`, the consumer of the
same messages.

The 11 html_chrome differences (`CStrAutoEncode::PopulateUTF8/UTF16`,
`CClientHandler::AddHeader` and the 8 functions that inline them) come from
`public/tier1/strtools.h`: retail has SDK 2013's `CStrAutoEncode` without the
pristine leak's `m_bCreatedUCS2` member (`m_bCreatedUTF16` at +0xc, the
destructor doesn't free `m_pucs2`). That header belongs to the shared-headers
lane; the change is in `byte-match-survey/chromehtml2-strtools-cstrautoencode.patch`.

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

The retail wrapper was built by Steam's CEF build (`buildslave_steam/cef_osx`)
with Chromium's flags (`-fno-rtti`, no typeinfo in its vtables) and a clang
between LLVM 3.3 and 3.4 (Chromium's bundled clang of mid-2013; e.g. a
duplicated `xor cl, cl` in every bool getter). With the retail toolchain
(clang-600) 281/760 functions match; LLVM 3.3 release gets 348/760. Matching
it needs that clang revision.

## Link

The LC_LOAD_DYLIB list and object order equal retail's. `__text` is 749 KB
against retail's 823 KB: retail exports libpng 1.5.2 (78 KB; all retail
modules that link libpng export it, so dead_strip keeps it), while our shared
`libpng.a` is built with `-fvisibility=hidden` and nothing is pulled. The
rest of the gap is the wrapper's codegen; protobuf and the Valve code have
retail's sizes.
