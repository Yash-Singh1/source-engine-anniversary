# --retail-osx32 match report (2026-11-18T08:14:51Z)

clang-600.0.57, ld64-242, retail build 19307283. Matched = exact + matches-after-masking (bytematch). Percentages are of the retail functions in scope (debug-map objects); bytes are retail bytes. The total counts linked modules only, since static libraries are linked into them.

| module | functions matched | % functions | bytes matched | % bytes | exact | masked | differs | missing |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| scenefilecache.dylib | 40/40 | 100.0% | 7785/7785 | 100.0% | 33 | 7 | 0 | 0 |
| soundemittersystem.dylib | 236/236 | 100.0% | 53556/53556 | 100.0% | 236 | 0 | 0 | 0 |
| libtier0.dylib | 367/367 | 100.0% | 58505/58505 | 100.0% | 126 | 241 | 0 | 0 |
| bugreporter_filequeue.dylib | 212/212 | 100.0% | 41424/41424 | 100.0% | 208 | 4 | 0 | 0 |
| bugreporter_public.dylib | 163/163 | 100.0% | 34863/34863 | 100.0% | 163 | 0 | 0 | 0 |
| vgui2.dylib | 684/684 | 100.0% | 133227/133227 | 100.0% | 578 | 106 | 0 | 0 |
| video_quicktime.dylib | 181/181 | 100.0% | 29196/29196 | 100.0% | 181 | 0 | 0 | 0 |
| video_services.dylib | 113/113 | 100.0% | 16116/16116 | 100.0% | 113 | 0 | 0 | 0 |
| shaderapiempty.dylib | 585/588 | 99.49% | 8535/8569 | 99.6% | 524 | 61 | 1 | 2 |
| studiorender.dylib | 977/988 | 98.89% | 479393/507743 | 94.42% | 254 | 723 | 11 | 0 |
| datacache.dylib | 399/406 | 98.28% | 129412/138135 | 93.69% | 127 | 272 | 7 | 0 |
| libvstdlib.dylib | 347/355 | 97.75% | 49523/55390 | 89.41% | 105 | 242 | 8 | 0 |
| stdshader_dx9.dylib | 1911/1963 | 97.35% | 374813/535678 | 69.97% | 307 | 1604 | 47 | 5 |
| launcher.dylib | 351/361 | 97.23% | 78298/89485 | 87.5% | 107 | 244 | 10 | 0 |
| replay.dylib | 1311/1405 | 93.31% | 231637/254041 | 91.18% | 483 | 828 | 43 | 51 |
| sourcevr.dylib | 130/141 | 92.2% | 16902/18800 | 89.9% | 54 | 76 | 10 | 1 |
| libtogl.dylib | 452/502 | 90.04% | 129933/179003 | 72.59% | 162 | 290 | 50 | 0 |
| materialsystem.dylib | 3304/3747 | 88.18% | 340612/552250 | 61.68% | 1638 | 1666 | 423 | 20 |
| vtex_dll.dylib | 353/426 | 82.86% | 84137/149120 | 56.42% | 124 | 229 | 62 | 11 |
| vphysics.dylib | 2225/2963 | 75.09% | 520365/891892 | 58.34% | 774 | 1451 | 675 | 63 |
| filesystem_stdio.dylib | 704/1021 | 68.95% | 164489/345776 | 47.57% | 191 | 513 | 207 | 110 |
| inputsystem.dylib | 113/192 | 58.85% | 9932/28914 | 34.35% | 43 | 70 | 61 | 18 |
| bsppack.dylib | 210/368 | 57.07% | 50674/153265 | 33.06% | 45 | 165 | 125 | 33 |
| engine.dylib | 7186/12767 | 56.29% | 1127400/3778257 | 29.84% | 2214 | 4972 | 5121 | 460 |
| vguimatsurface.dylib | 1575/2987 | 52.73% | 237285/709304 | 33.45% | 423 | 1152 | 1366 | 46 |
| GameUI.dylib | 2432/4888 | 49.75% | 316943/1159816 | 27.33% | 408 | 2024 | 2336 | 120 |
| ServerBrowser.dylib | 1458/3117 | 46.78% | 205007/731116 | 28.04% | 351 | 1107 | 1416 | 243 |
| vaudio_speex.dylib | 19/131 | 14.5% | 2365/61295 | 3.86% | 4 | 15 | 87 | 25 |
| hl2_osx | 0/1 | 0.0% | 0/157 | 0.0% | 0 | 0 | 1 | 0 |
| **total, linked modules** | 28038/41313 | 67.87% | 4932327/10722678 | 46.0% | 9976 | 18062 | 12067 | 1208 |
| **static libraries (members pulled by the named retail module)** | | | | | | | | |
| libchoreoobjects.a (vs client) | 245/245 | 100.0% | 71361/71361 | 100.0% | 95 | 150 | 0 | 0 |
| libhk_math.a (vs vphysics) | 15/15 | 100.0% | 6918/6918 | 100.0% | 7 | 8 | 0 | 0 |
| libmathlib.a (vs engine) | 95/95 | 100.0% | 29755/29755 | 100.0% | 31 | 64 | 0 | 0 |
| libreplay_common.a (vs engine) | 6/6 | 100.0% | 657/657 | 100.0% | 2 | 4 | 0 | 0 |
| libtier3.a (vs engine) | 2/2 | 100.0% | 845/845 | 100.0% | 0 | 2 | 0 | 0 |
| libbzip2.a (vs engine) | 25/25 | 100.0% | 53130/53130 | 100.0% | 4 | 21 | 0 | 0 |
| libtier1.a (vs engine) | 484/485 | 99.79% | 97782/98806 | 98.96% | 167 | 317 | 1 | 0 |
| libivp_compactbuilder.a (vs vphysics) | 377/393 | 95.93% | 217251/235146 | 92.39% | 36 | 341 | 15 | 1 |
| libtier2.a (vs engine) | 62/65 | 95.38% | 30423/36813 | 82.64% | 19 | 43 | 3 | 0 |
| libshaderlib.a (vs materialsystem) | 12/13 | 92.31% | 1103/1314 | 83.94% | 4 | 8 | 1 | 0 |
| libappframework.a (vs engine) | 31/34 | 91.18% | 8636/10412 | 82.94% | 1 | 30 | 2 | 1 |
| libparticles.a (vs client) | 1325/1463 | 90.57% | 182402/285508 | 63.89% | 813 | 512 | 128 | 10 |
| libbitmap.a (vs engine) | 83/102 | 81.37% | 28079/53332 | 52.65% | 44 | 39 | 19 | 0 |
| libvtf.a (vs engine) | 56/82 | 68.29% | 12057/25047 | 48.14% | 26 | 30 | 24 | 2 |
| libivp_physics.a (vs vphysics) | 765/1207 | 63.38% | 115458/317532 | 36.36% | 266 | 499 | 406 | 36 |
| libvpklib.a (vs filesystem_stdio) | 40/65 | 61.54% | 15193/37127 | 40.92% | 5 | 35 | 24 | 1 |
| libvgui_surfacelib.a (vs vguimatsurface) | 53/92 | 57.61% | 8377/22043 | 38.0% | 15 | 38 | 31 | 8 |
| libhavana_constraints.a (vs vphysics) | 82/143 | 57.34% | 4598/72841 | 6.31% | 40 | 42 | 56 | 5 |
| libvgui_controls.a (vs engine) | 1120/2508 | 44.66% | 144111/595734 | 24.19% | 252 | 868 | 1370 | 18 |
| libhk_base.a (vs vphysics) | 2/5 | 40.0% | 197/1219 | 16.16% | 0 | 2 | 3 | 0 |
| libmatsys_controls.a (vs engine) | 11/28 | 39.29% | 1392/6761 | 20.59% | 0 | 11 | 16 | 1 |
| libdmxloader.a (vs engine) | 32/99 | 32.32% | 3599/37092 | 9.7% | 8 | 24 | 66 | 1 |
