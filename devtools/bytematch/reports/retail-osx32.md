# --retail-osx32 match report (2026-11-18T11:44:54Z)

clang-600.0.57, ld64-242, retail build 19307283. Matched = exact + matches-after-masking (bytematch). Percentages are of the retail functions in scope (debug-map objects); bytes are retail bytes. The totals count linked modules only, since static libraries are linked into them. Game modules (server/client per game, against depots 380, 340 and 223) have their own rows and a second total.

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
| studiorender.dylib | 979/988 | 99.09% | 482513/507743 | 95.03% | 254 | 725 | 9 | 0 |
| libvstdlib.dylib | 351/355 | 98.87% | 51970/55390 | 93.83% | 106 | 245 | 4 | 0 |
| datacache.dylib | 401/406 | 98.77% | 132455/138135 | 95.89% | 201 | 200 | 5 | 0 |
| stdshader_dx9.dylib | 1925/1963 | 98.06% | 389777/535678 | 72.76% | 308 | 1617 | 33 | 5 |
| launcher.dylib | 352/361 | 97.51% | 79216/89485 | 88.52% | 107 | 245 | 9 | 0 |
| shaderapidx9.dylib | 1538/1592 | 96.61% | 266766/302451 | 88.2% | 622 | 916 | 52 | 2 |
| replay.dylib | 1336/1405 | 95.09% | 240299/254041 | 94.59% | 483 | 853 | 19 | 50 |
| materialsystem.dylib | 3556/3747 | 94.9% | 409700/552250 | 74.19% | 1756 | 1800 | 180 | 11 |
| bsppack.dylib | 341/368 | 92.66% | 109385/153265 | 71.37% | 96 | 245 | 18 | 9 |
| sourcevr.dylib | 130/141 | 92.2% | 16902/18800 | 89.9% | 54 | 76 | 10 | 1 |
| ServerBrowser.dylib | 3109/3380 | 91.98% | 628357/788052 | 79.74% | 842 | 2267 | 247 | 24 |
| vtex_dll.dylib | 401/439 | 91.34% | 116453/149812 | 77.73% | 135 | 266 | 34 | 4 |
| libtogl.dylib | 456/502 | 90.84% | 131196/179003 | 73.29% | 163 | 293 | 46 | 0 |
| vguimatsurface.dylib | 2695/2987 | 90.22% | 557117/709304 | 78.54% | 810 | 1885 | 265 | 27 |
| engine.dylib | 11615/12986 | 89.44% | 2955402/3809922 | 77.57% | 3151 | 8464 | 1095 | 276 |
| GameUI.dylib | 4282/4888 | 87.6% | 841002/1159816 | 72.51% | 948 | 3334 | 513 | 93 |
| vphysics.dylib | 2237/2963 | 75.5% | 523329/891892 | 58.68% | 777 | 1460 | 662 | 64 |
| inputsystem.dylib | 113/192 | 58.85% | 9932/28914 | 34.35% | 43 | 70 | 61 | 18 |
| filesystem_stdio.dylib | 993/1953 | 50.84% | 253945/567545 | 44.74% | 297 | 696 | 668 | 292 |
| vpk_osx32 | 630/1883 | 33.46% | 117395/485072 | 24.2% | 247 | 383 | 917 | 336 |
| vaudio_speex.dylib | 19/131 | 14.5% | 2365/61295 | 3.86% | 4 | 15 | 87 | 25 |
| hl2_osx | 0/1 | 0.0% | 0/157 | 0.0% | 0 | 0 | 1 | 0 |
| **total, linked modules** | 40040/46215 | 86.64% | 8698683/11821263 | 73.59% | 13566 | 26474 | 4936 | 1239 |
| **game modules (one --build-games build each)** | | | | | | | | |
| episodic/server.dylib | 21864/22744 | 96.13% | 7011586/7696434 | 91.1% | 5237 | 16627 | 730 | 150 |
| hl2/server.dylib | 21864/22744 | 96.13% | 7011586/7696434 | 91.1% | 5237 | 16627 | 730 | 150 |
| lostcoast/server.dylib | 20213/21442 | 94.27% | 6301061/7181480 | 87.74% | 5047 | 15166 | 1173 | 56 |
| episodic/client.dylib | 16787/17809 | 94.26% | 3775809/4434694 | 85.14% | 5249 | 11538 | 836 | 186 |
| hl2/client.dylib | 16786/17808 | 94.26% | 3775782/4434639 | 85.14% | 5249 | 11537 | 836 | 186 |
| lostcoast/client.dylib | 16161/17459 | 92.57% | 3601850/4349835 | 82.8% | 5112 | 11049 | 1124 | 174 |
| **total, linked modules and games** | 153715/166221 | 92.48% | 40176357/47614779 | 84.38% | 44697 | 109018 | 10365 | 2141 |
| **static libraries (members pulled by the named retail module)** | | | | | | | | |
| libchoreoobjects.a (vs client) | 245/245 | 100.0% | 71361/71361 | 100.0% | 95 | 150 | 0 | 0 |
| libdmxloader.a (vs engine) | 99/99 | 100.0% | 37092/37092 | 100.0% | 13 | 86 | 0 | 0 |
| libhk_math.a (vs vphysics) | 15/15 | 100.0% | 6918/6918 | 100.0% | 7 | 8 | 0 | 0 |
| libshaderlib.a (vs materialsystem) | 13/13 | 100.0% | 1314/1314 | 100.0% | 4 | 9 | 0 | 0 |
| libmathlib.a (vs engine) | 95/95 | 100.0% | 29755/29755 | 100.0% | 31 | 64 | 0 | 0 |
| libreplay_common.a (vs engine) | 6/6 | 100.0% | 657/657 | 100.0% | 2 | 4 | 0 | 0 |
| libtier3.a (vs engine) | 2/2 | 100.0% | 845/845 | 100.0% | 0 | 2 | 0 | 0 |
| libbzip2.a (vs engine) | 25/25 | 100.0% | 53130/53130 | 100.0% | 4 | 21 | 0 | 0 |
| libmatsys_controls.a (vs engine) | 28/28 | 100.0% | 6761/6761 | 100.0% | 5 | 23 | 0 | 0 |
| libtier1.a (vs engine) | 484/485 | 99.79% | 97782/98806 | 98.96% | 167 | 317 | 1 | 0 |
| libvpklib.a (vs filesystem_stdio) | 237/241 | 98.34% | 53767/55440 | 96.98% | 81 | 156 | 4 | 0 |
| libivp_compactbuilder.a (vs vphysics) | 377/393 | 95.93% | 217251/235146 | 92.39% | 36 | 341 | 15 | 1 |
| libvgui_controls.a (vs engine) | 2395/2508 | 95.49% | 511141/595734 | 85.8% | 667 | 1728 | 113 | 0 |
| libtier2.a (vs engine) | 62/65 | 95.38% | 30423/36813 | 82.64% | 19 | 43 | 3 | 0 |
| libappframework.a (vs engine) | 31/34 | 91.18% | 8636/10412 | 82.94% | 1 | 30 | 2 | 1 |
| libparticles.a (vs client) | 1326/1463 | 90.64% | 183337/285508 | 64.21% | 813 | 513 | 127 | 10 |
| libvtf.a (vs engine) | 71/82 | 86.59% | 19602/25047 | 78.26% | 29 | 42 | 11 | 0 |
| libbitmap.a (vs engine) | 83/102 | 81.37% | 28079/53332 | 52.65% | 44 | 39 | 19 | 0 |
| libivp_physics.a (vs vphysics) | 765/1207 | 63.38% | 115458/317532 | 36.36% | 266 | 499 | 406 | 36 |
| libvgui_surfacelib.a (vs vguimatsurface) | 53/92 | 57.61% | 8377/22043 | 38.0% | 15 | 38 | 31 | 8 |
| libhavana_constraints.a (vs vphysics) | 82/143 | 57.34% | 4598/72841 | 6.31% | 40 | 42 | 56 | 5 |
| libhk_base.a (vs vphysics) | 2/5 | 40.0% | 197/1219 | 16.16% | 0 | 2 | 3 | 0 |
