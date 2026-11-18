# vaudio_miles

Retail: depot 223 `bin/vaudio_miles.dylib`, 51 functions in five objects
(`voice_codec_frame`, `milesbase`, `mp3_miles`, `voice_codec_miles`,
`libtier1.a(interface.o)`). Sources come from Source SDK 2012
(nn-firc/source-sdk-2012 `engine/voice_codecs/miles/`), ported with the
retail changes below. Miles itself is user-provided: the build links the
depot's `libMilesX86.dylib`, and `common/miles/mss.h` declares only the MSS 9
API this module uses (types chosen to give retail's mangled names, e.g.
`HPROPERTY` = `unsigned int`, `AILASIFETCHCB` = `int (*)(unsigned, void*, int, int)`).

Result with `--retail-osx32`: 51/51 functions match (23 exact, 28 after
relocation masking). Linked with a libtier1 that doesn't pull `utlbuffer.o`
(see below), every `__TEXT` and `__DATA` section of the dylib is byte-identical
to retail; only `__LINKEDIT` (symbols, debug map, UUID) differs.

Functions retail lacks (`CProvider::*`, `ASISTRUCT::Clear`, `CMilesMP3::CMilesMP3`/`Init`,
`FrameEncoder_Miles::FrameEncoder_Miles`/`Shutdown`/`FigureOutFrameSizes`,
`Con_Printf`, `Convert16*`) are in the source as SDK 2012 has them: they are
inlined and the out-of-line copies are removed by `-dead_strip`. Making them
`inline` matches every function too, but changes function and string order
(`AudioStreamEventCB` and `.MP3`/`.RAW` come first in retail because the
out-of-line `CMilesMP3::Init` is emitted early).

| function | status | note |
|---|---|---|
| IncrementRefMiles | edited | retail caches `-nosound` (`s_bMilesSound`, guarded by a local `s_bInited`) and skips Miles startup and the refcount when it is set |
| DecrementRefMiles | edited | returns early when `s_bMilesSound` is false |
| ASISTRUCT::Init | edited | returns false when `s_bMilesSound` is false; `Warning("Unable to find Miles ASI codec input=%s, output=%s\n")` when no provider |
| ASISTRUCT::ASISTRUCT, ~ASISTRUCT, Shutdown, Process, IsActive, GetProperty, Seek | identical | SDK 2012 |
| AudioStreamEventCB, CMilesMP3::*, CVAudio::*, CreateVoiceCodec_CVAudio | identical | SDK 2012 minus `CreateMilesAudioEngine`/`DestroyMilesAudioEngine` (not in `IVAudio` "VAudio002") |
| EncodeStreamCB, DecodeStreamCB | edited | file-static (`_ZL`) instead of static members; friends of `FrameEncoder_Miles` |
| EncodeStreamCB_fixup, DecodeStreamCB_fixup | written new | `MSS_DEF_CB_STACK_ALIGN`: thunk that realigns the stack to 16 bytes; passed as the ASI fetch callbacks through `MSS_CALLBACK_ALIGNED_NAME` |
| FrameEncoder_Miles::* (others), CreateVoiceCodec_Miles | identical | SDK 2012 |
| VoiceCodec_Frame::Decompress | edited | retail passes `&pCompressed[curCompressedByte]` without the leak's NULL check (also matches retail vaudio_speex) |
| InterfaceReg::InterfaceReg, CreateInterface | identical | tier1 |

Link note: our `tier1/strtools.cpp` has functions retail's doesn't
(`V_StripAndPreserveHTML*`, `V_LogMultiline`, URL/UTF-8 helpers; none exist in
any depot 223 module) that reference `CUtlBuffer`, `CUtlString` and
`Q_UTF32ToUTF8`. `interface.o` pulls `strtools.o` for `V_snprintf`, which then
pulls `utlbuffer.o`, whose static initializer survives
`-no_dead_strip_inits_and_terms` (721 extra bytes of `__text`, 4.9 KB of
`__common`). Retail loads `strtools.o` too but strips it completely.
