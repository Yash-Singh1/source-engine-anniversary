# --retail-osx32 match report (2026-11-18T07:34:53Z)

clang-600.0.57, ld64-242, retail build 19307283. Matched = exact + matches-after-masking (bytematch). Percentages are of the retail functions in scope (debug-map objects); bytes are retail bytes. The total counts linked modules only, since static libraries are linked into them.

| module | functions matched | % functions | bytes matched | % bytes | exact | masked | differs | missing |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| libtier0.dylib | 362/367 | 98.64% | 57868/58505 | 98.91% | 124 | 238 | 5 | 0 |
| shaderapiempty.dylib | 579/588 | 98.47% | 7748/8569 | 90.42% | 521 | 58 | 4 | 5 |
| video_quicktime.dylib | 159/181 | 87.85% | 23018/29196 | 78.84% | 67 | 92 | 17 | 5 |
| video_services.dylib | 97/113 | 85.84% | 12319/16116 | 76.44% | 44 | 53 | 12 | 4 |
| stdshader_dx9.dylib | 1665/1956 | 85.12% | 131479/533175 | 24.66% | 296 | 1369 | 282 | 9 |
| libvstdlib.dylib | 288/355 | 81.13% | 34143/55390 | 61.64% | 95 | 193 | 56 | 11 |
| studiorender.dylib | 763/987 | 77.3% | 346035/506588 | 68.31% | 190 | 573 | 218 | 6 |
| scenefilecache.dylib | 30/40 | 75.0% | 3123/7785 | 40.12% | 12 | 18 | 8 | 2 |
| sourcevr.dylib | 100/139 | 71.94% | 8169/18512 | 44.13% | 44 | 56 | 30 | 9 |
| vphysics.dylib | 2080/2963 | 70.2% | 467885/891892 | 52.46% | 753 | 1327 | 814 | 69 |
| replay.dylib | 981/1405 | 69.82% | 141264/254041 | 55.61% | 371 | 610 | 345 | 79 |
| soundemittersystem.dylib | 159/236 | 67.37% | 27029/53556 | 50.47% | 52 | 107 | 66 | 11 |
| vtex_dll.dylib | 282/426 | 66.2% | 55243/149120 | 37.05% | 108 | 174 | 124 | 20 |
| vgui2.dylib | 452/684 | 66.08% | 45841/133227 | 34.41% | 237 | 215 | 211 | 21 |
| datacache.dylib | 240/406 | 59.11% | 41199/138135 | 29.83% | 76 | 164 | 155 | 11 |
| bsppack.dylib | 210/368 | 57.07% | 50674/153265 | 33.06% | 45 | 165 | 125 | 33 |
| engine.dylib | 7186/12767 | 56.29% | 1127400/3778257 | 29.84% | 2214 | 4972 | 5121 | 460 |
| bugreporter_public.dylib | 84/163 | 51.53% | 9673/34863 | 27.75% | 39 | 45 | 71 | 8 |
| inputsystem.dylib | 93/192 | 48.44% | 6225/28914 | 21.53% | 38 | 55 | 68 | 31 |
| filesystem_stdio.dylib | 442/1021 | 43.29% | 55577/345776 | 16.07% | 166 | 276 | 434 | 145 |
| bugreporter_filequeue.dylib | 87/212 | 41.04% | 6908/41424 | 16.68% | 41 | 46 | 114 | 11 |
| GameUI.dylib | 1975/4888 | 40.41% | 176085/1159816 | 15.18% | 326 | 1649 | 2756 | 157 |
| vguimatsurface.dylib | 1178/2984 | 39.48% | 108263/707940 | 15.29% | 340 | 838 | 1718 | 88 |
| ServerBrowser.dylib | 1109/3117 | 35.58% | 93394/731116 | 12.77% | 279 | 830 | 1729 | 279 |
| vaudio_speex.dylib | 19/131 | 14.5% | 2365/61295 | 3.86% | 4 | 15 | 87 | 25 |
| hl2_osx | 0/1 | 0.0% | 0/157 | 0.0% | 0 | 0 | 1 | 0 |
| **total, linked modules** | 20620/36690 | 56.2% | 3038927/9896630 | 30.71% | 6482 | 14138 | 14571 | 1499 |
| **static libraries (members pulled by the named retail module)** | | | | | | | | |
| libhk_math.a (vs vphysics) | 15/15 | 100.0% | 6918/6918 | 100.0% | 7 | 8 | 0 | 0 |
| libtier3.a (vs engine) | 2/2 | 100.0% | 845/845 | 100.0% | 0 | 2 | 0 | 0 |
| libbzip2.a (vs engine) | 25/25 | 100.0% | 53130/53130 | 100.0% | 4 | 21 | 0 | 0 |
| libivp_compactbuilder.a (vs vphysics) | 377/393 | 95.93% | 217251/235146 | 92.39% | 36 | 341 | 15 | 1 |
| libmathlib.a (vs engine) | 71/75 | 94.67% | 17873/20017 | 89.29% | 22 | 49 | 3 | 1 |
| libparticles.a (vs client) | 1221/1463 | 83.46% | 121073/285508 | 42.41% | 788 | 433 | 232 | 10 |
| libshaderlib.a (vs materialsystem) | 10/13 | 76.92% | 798/1314 | 60.73% | 4 | 6 | 3 | 0 |
| libtier2.a (vs engine) | 47/65 | 72.31% | 10336/36813 | 28.08% | 19 | 28 | 18 | 0 |
| libbitmap.a (vs engine) | 70/102 | 68.63% | 18529/53332 | 34.74% | 37 | 33 | 32 | 0 |
| libreplay_common.a (vs engine) | 4/6 | 66.67% | 543/657 | 82.65% | 0 | 4 | 2 | 0 |
| libvtf.a (vs engine) | 53/82 | 64.63% | 11574/25047 | 46.21% | 26 | 27 | 27 | 2 |
| libtier1.a (vs engine) | 313/485 | 64.54% | 38006/98806 | 38.47% | 136 | 177 | 152 | 20 |
| libivp_physics.a (vs vphysics) | 765/1207 | 63.38% | 115458/317532 | 36.36% | 266 | 499 | 406 | 36 |
| libhavana_constraints.a (vs vphysics) | 82/143 | 57.34% | 4598/72841 | 6.31% | 40 | 42 | 56 | 5 |
| libhk_base.a (vs vphysics) | 2/5 | 40.0% | 197/1219 | 16.16% | 0 | 2 | 3 | 0 |
| libmatsys_controls.a (vs engine) | 11/28 | 39.29% | 1392/6761 | 20.59% | 0 | 11 | 16 | 1 |
| libvgui_controls.a (vs engine) | 876/2508 | 34.93% | 69928/595734 | 11.74% | 207 | 669 | 1600 | 32 |
| libchoreoobjects.a (vs client) | 85/245 | 34.69% | 13618/71361 | 19.08% | 41 | 44 | 158 | 2 |
| libappframework.a (vs engine) | 11/34 | 32.35% | 1610/10412 | 15.46% | 1 | 10 | 22 | 1 |
| libdmxloader.a (vs engine) | 32/99 | 32.32% | 3599/37092 | 9.7% | 8 | 24 | 66 | 1 |
| libvgui_surfacelib.a (vs vguimatsurface) | 27/92 | 29.35% | 1757/22043 | 7.97% | 8 | 19 | 57 | 8 |
| libvpklib.a (vs filesystem_stdio) | 13/65 | 20.0% | 3051/37127 | 8.22% | 4 | 9 | 51 | 1 |
