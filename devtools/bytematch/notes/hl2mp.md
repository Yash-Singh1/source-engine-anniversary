# hl2mp (depot 223 hl2mp/bin/client.dylib, server.dylib)

A May 2014 build (`rel_hl2_osx`, objects dated 2014-05-15) of the Source SDK
2013 multiplayer tree, not the anniversary code: Xcode 5.1 (clang-503.0.38,
ld64-236.3 by the 2^0 `__unwind_info`), `MACOSX_DEPLOYMENT_TARGET` 10.5,
SDK 2013's xcconfig flags. Source: `sdk2013mp/` (SDK commit 53e78c50, the
same day). Build: `--retail-osx32 --build-games=hl2mp` (see
`sdk2013mp/wscript`).

Measured per object with the debug-map scope, all objects plus archives:

| module | functions | bytes |
|---|---:|---:|
| client loose objects | 13116/13116 | 100% |
| server loose objects | 20393/20393 | 100% |
| client, all | 19920/20011 (99.55%) | 98.24% |
| server, all | 21179/21192 (99.94%) | 99.88% |

Both link with retail's load commands, header flags and `LC_LOAD_DYLIB`
list; server `__const`, `__gcc_except_tab`, `__data`, `__common` and `__bss`
have retail's sizes.

## Archives

| archive | source | client match |
|---|---|---:|
| libtier1, libmathlib, libchoreoobjects | sdk2013mp, unmodified | 100% |
| libvgui_controls | sdk2013mp, `htmlmessages.pb.cc` from protoc 2.3.0 | 4275/4275 |
| libmatsys_controls | sdk2013mp, 2014 forms of the main tree's files | 94/94 |
| libtier3 | main tree, 2014 headers | 12/12 |
| libtier2 | main tree, 2014 headers | 32/34 |
| libdmxloader | main tree, 2014 headers | 81/85 |
| libparticles | main tree, its own headers | 1395/1463 |
| libbitmap | main tree, its own headers | 92/98 |
| libvtf | main tree, its own headers | 71/82 |
| libprotobuf | SDK 2013's prebuilt osx32 archive (no stabs in retail) | not in scope |

The main-tree differences are the owning lanes' (anniversary forms mostly
have the same sizes, so the code is shared): particles render ops with the
leak's TF signatures (retail `RenderSpriteCard( CMeshBuilder&,
SpriteRenderInfo_t&, int, ParticleRenderData_t const* )` and the other
helpers have no context/camera arguments), particle initializers and
`CParticleCollection::Init`, `CVTFTexture::Unserialize/LoadImageData`
(fork streaming code), `ImageLoader::ConvertImageFormat` (stb DXT),
`ReadDMXHeader`/`CDmxSerializer::Unserialize`, `InitializeStandardMaterials`,
`ComputeProjectionMatrix`. A main-tree file that matches here and in the
anniversary modules is right on two compilers.

## Toolchain findings

- clang-503.0.38 leaves `MaskAndBranchFoldingIsLegal` uninitialized on x86,
  so `CodeGenPrepare::sinkAndCmp` ran in some of Valve's compiles and not
  others (sunk: `test cl, 0x10` repeated per block; not sunk: the masked value
  kept live). `patches/clang-503.0.38-maskandbranch.patch` adds
  `-mllvm -mask-and-branch-folding`; `SINK_AND_CMP` in `sdk2013mp/wscript`
  lists the 11 objects that need it, found per object against retail. Client
  and server `saverestore.o` differ, so it can't be a module-wide setting.
- Source SDK 2013's prebuilt osx32 archives are clang-318 (Xcode 4.3)
  builds, not retail's members (tier1: 71/362).

## Open

- Server `__TEXT` section order: retail `__cstring, __const,
  __gcc_except_tab`, ours `__const, __gcc_except_tab, __cstring` (client
  matches). Not investigated further.
- libprotobuf's position in the client link: retail places its code after
  libmathlib's members; waf links it last.
