# soundemittersystem

Retail: depot 223 `bin/soundemittersystem.dylib`, objects `interval.o`
(game/shared), `SoundParametersInternal.o` (public), `soundemittersystembase.o`,
plus tier1 members (checksum_crc, convar, interface, KeyValues, strtools, tier1,
utlbuffer, utlsymbol), which the tier1 lane owns.

Verification: clang-600.0.57, TARGET.md Release flags, per N_OSO object,
"strict" = every byte equal except relocated fields, which must resolve to the
same symbol / string / literal. "hdrfix" means compiled
against an overlay of `public/` carrying header changes owned by other lanes:
- `tier1/utlmemory.h`: SDK 2013 `UtlMemory_CalcNewAllocationCount` (no
  "exact size if larger" clamp) and `IsIdxValid` as one unsigned compare;
- `tier1/utllinkedlist.h`: `AllocInternal` without `RESTRICT`, overflow paths
  `ExecuteNTimes( 10, Warning( ... ) )` instead of `Error`;
- `tier1/utlsymbol.h`: `CUtlSymbolTable` default `initSize` 32 (was 16);
- `tier1/KeyValues.h`: `LoadFromFile` without the `refreshCache` parameter;
- `tier1/utlbuffer.h`: SDK 2013 header (retail inlines `AddNullTermination()`
  without the `nPut` argument, among other differences).

Before: 78/99 strict. After: 81/99 with current headers, 99/99 with hdrfix.

| function | status | note |
|---|---|---|
| CSoundEmitterSystemBase::ModInit | edited | body moved in from `InternalModInit` (retail has no such function); `faceposer_file` manifest branch removed (string absent in retail) |
| CSoundEmitterSystemBase::ModShutdown | edited | body moved in from `InternalModShutdown` |
| CSoundEmitterSystemBase::InternalModInit / InternalModShutdown | removed | folded into ModInit / ModShutdown |
| CSoundEmitterSystemBase::Flush | uncertain | not in retail (no vtable slot); kept only because our `ISoundEmitterSystemBase` declares it; now forwards to ModShutdown/ModInit. Unused in the tree. Remove when the interface drops it |
| CSoundEmitterSystemBase::InitSoundInternalParameters | edited | `MAX()` macro for `delay_msec` (retail calls GetInt twice) |
| CSoundEmitterSystemBase::LoadGlobalActors | identical | byte with hdrfix (KeyValues.h) |
| CSoundEmitterSystemBase::GetParametersForSound | identical | byte with hdrfix (utlsymbol.h, function-static table) |
| CSoundEmitterSystemBase::CSoundEmitterSystemBase | identical | byte with hdrfix (utlsymbol.h) |
| CSoundEmitterSystemBase::SaveChangesToSoundScript | identical | byte with hdrfix (utlbuffer.h) |
| CSoundEmitterSystemBase::FindBestSoundForGender, EnsureAvailableSlotsForGender, ReloadSoundEntriesInList, GetParametersForSoundEx, IsValidIndex, InternalGetParametersForSound | identical | byte with hdrfix (utlmemory.h / utllinkedlist.h inlines) |
| CUtlMemory<...>::Grow (4 instantiations) | identical | byte with hdrfix (utlmemory.h) |
| CUtlLinkedList<...>::LinkBefore, AllocInternal | identical | byte with hdrfix (utlmemory.h, utllinkedlist.h) |
| CUtlStableHashtable<...>::RemoveAndAdvance, DoRemove | identical | byte with hdrfix (utlmemory.h IsIdxValid) |
| CUtlHashtable<...>::DoLookup<char const*> | identical | byte; only differs in a PIC displacement to `""` |
| all other functions in soundemittersystembase.o (60) | identical | byte |
| SoundParametersInternal.o (15 functions) | identical | byte |
| interval.o ReadInterval | identical | byte |
| TranslateChannel, TranslateSoundLevel, AddSoundName, RandomInterval, string helpers in SoundParametersInternal.cpp | identical | inlined; out-of-line copies dead-stripped in retail, not ours-only |

Vtable: retail `CSoundEmitterSystemBase` has 46 slots (SDK 2013
`ISoundEmitterSystemBase` + destructors); ours has `Flush` in addition.

Link composition: retail objects are interval.o, SoundParametersInternal.o,
soundemittersystembase.o and tier1 members; no `memoverride.o`. Retail load
commands: libtier0, libvstdlib, libiconv, libstdc++, libSystem.
