# --retail-osx32 match report (2026-11-20T20:53:23Z)

clang-600.0.57, ld64-242, retail build 19307283. Matched = exact + matches-after-masking (bytematch). Percentages are of the retail functions in scope (debug-map objects); bytes are retail bytes. The totals count linked modules only, since static libraries are linked into them. Game modules (server/client per game, against depots 380, 340 and 223) have their own rows and a second total.

| module | functions matched | % functions | bytes matched | % bytes | exact | masked | differs | missing |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| datacache.dylib | 406/406 | 100.0% | 138135/138135 | 100.0% | 322 | 84 | 0 | 0 |
| vaudio_miles.dylib | 51/51 | 100.0% | 6250/6250 | 100.0% | 51 | 0 | 0 | 0 |
| filesystem_stdio.dylib | 1962/1962 | 100.0% | 567780/567780 | 100.0% | 1962 | 0 | 0 | 0 |
| GameUI.dylib | 4970/4970 | 100.0% | 1197344/1197344 | 100.0% | 4970 | 0 | 0 | 0 |
| inputsystem.dylib | 193/193 | 100.0% | 29183/29183 | 100.0% | 193 | 0 | 0 | 0 |
| launcher.dylib | 361/361 | 100.0% | 89485/89485 | 100.0% | 305 | 56 | 0 | 0 |
| materialsystem.dylib | 3747/3747 | 100.0% | 552250/552250 | 100.0% | 3747 | 0 | 0 | 0 |
| shaderapidx9.dylib | 1592/1592 | 100.0% | 302451/302451 | 100.0% | 1434 | 158 | 0 | 0 |
| shaderapiempty.dylib | 588/588 | 100.0% | 8569/8569 | 100.0% | 588 | 0 | 0 | 0 |
| stdshader_dx9.dylib | 1963/1963 | 100.0% | 535678/535678 | 100.0% | 1963 | 0 | 0 | 0 |
| replay.dylib | 1442/1442 | 100.0% | 257979/257979 | 100.0% | 1360 | 82 | 0 | 0 |
| scenefilecache.dylib | 40/40 | 100.0% | 7785/7785 | 100.0% | 33 | 7 | 0 | 0 |
| ServerBrowser.dylib | 3395/3395 | 100.0% | 789959/789959 | 100.0% | 3395 | 0 | 0 | 0 |
| soundemittersystem.dylib | 236/236 | 100.0% | 53556/53556 | 100.0% | 236 | 0 | 0 | 0 |
| sourcevr.dylib | 141/141 | 100.0% | 18800/18800 | 100.0% | 141 | 0 | 0 | 0 |
| libtier0.dylib | 367/367 | 100.0% | 58505/58505 | 100.0% | 367 | 0 | 0 | 0 |
| libtogl.dylib | 502/502 | 100.0% | 179003/179003 | 100.0% | 502 | 0 | 0 | 0 |
| bsppack.dylib | 368/368 | 100.0% | 153265/153265 | 100.0% | 368 | 0 | 0 | 0 |
| bugreporter_filequeue.dylib | 212/212 | 100.0% | 41424/41424 | 100.0% | 212 | 0 | 0 | 0 |
| bugreporter_public.dylib | 163/163 | 100.0% | 34863/34863 | 100.0% | 163 | 0 | 0 | 0 |
| vtex_dll.dylib | 439/439 | 100.0% | 149812/149812 | 100.0% | 374 | 65 | 0 | 0 |
| chromehtml.dylib | 2891/2891 | 100.0% | 435407/435407 | 100.0% | 2891 | 0 | 0 | 0 |
| vgui2.dylib | 684/684 | 100.0% | 133227/133227 | 100.0% | 684 | 0 | 0 | 0 |
| vguimatsurface.dylib | 3002/3002 | 100.0% | 711211/711211 | 100.0% | 2799 | 203 | 0 | 0 |
| video_quicktime.dylib | 181/181 | 100.0% | 29196/29196 | 100.0% | 181 | 0 | 0 | 0 |
| video_services.dylib | 113/113 | 100.0% | 16116/16116 | 100.0% | 113 | 0 | 0 | 0 |
| libvstdlib.dylib | 361/361 | 100.0% | 56548/56548 | 100.0% | 361 | 0 | 0 | 0 |
| hl2_osx | 1/1 | 100.0% | 157/157 | 100.0% | 0 | 1 | 0 | 0 |
| engine.dylib | 13038/13040 | 99.98% | 3818572/3820121 | 99.96% | 13038 | 0 | 2 | 0 |
| vphysics.dylib | 2962/2963 | 99.97% | 889904/891892 | 99.78% | 1541 | 1421 | 1 | 0 |
| vpk_osx32 | 1881/1883 | 99.89% | 476592/485072 | 98.25% | 1425 | 456 | 2 | 0 |
| studiorender.dylib | 986/988 | 99.8% | 495413/507743 | 97.57% | 259 | 727 | 2 | 0 |
| vaudio_speex.dylib | 130/131 | 99.24% | 54977/61295 | 89.69% | 77 | 53 | 1 | 0 |
| **total, linked modules** | 49368/49376 | 99.98% | 12289396/12320061 | 99.75% | 46055 | 3313 | 8 | 0 |
| **game modules (one --build-games build each)** | | | | | | | | |
| hl2mp/server.dylib | 21192/21192 | 100.0% | 6526716/6526716 | 100.0% | 10795 | 10397 | 0 | 0 |
| hl2mp/client.dylib | 20011/20011 | 100.0% | 4715459/4715459 | 100.0% | 11086 | 8925 | 0 | 0 |
| episodic/server.dylib | 22744/22744 | 100.0% | 7696434/7696434 | 100.0% | 19793 | 2951 | 0 | 0 |
| hl2/server.dylib | 22744/22744 | 100.0% | 7696434/7696434 | 100.0% | 19793 | 2951 | 0 | 0 |
| lostcoast/server.dylib | 21442/21442 | 100.0% | 7181480/7181480 | 100.0% | 18712 | 2730 | 0 | 0 |
| episodic/client.dylib | 17892/17893 | 99.99% | 4453761/4459034 | 99.88% | 6337 | 11555 | 1 | 0 |
| lostcoast/client.dylib | 17507/17510 | 99.98% | 4361181/4365944 | 99.89% | 6145 | 11362 | 3 | 0 |
| hl2/client.dylib | 17887/17892 | 99.97% | 4449765/4459007 | 99.79% | 6231 | 11656 | 5 | 0 |
| **total, linked modules and games** | 210787/210804 | 99.99% | 59370626/59420569 | 99.92% | 144947 | 65840 | 17 | 0 |
| **static libraries (members pulled by the named retail module)** | | | | | | | | |
| libappframework.a (vs launcher) | 136/136 | 100.0% | 35205/35205 | 100.0% | 36 | 100 | 0 | 0 |
| libbitmap.a (vs engine) | 102/102 | 100.0% | 53332/53332 | 100.0% | 44 | 58 | 0 | 0 |
| libchoreoobjects.a (vs client) | 245/245 | 100.0% | 71361/71361 | 100.0% | 95 | 150 | 0 | 0 |
| libdmxloader.a (vs engine) | 99/99 | 100.0% | 37092/37092 | 100.0% | 13 | 86 | 0 | 0 |
| libhk_base.a (vs vphysics) | 5/5 | 100.0% | 1219/1219 | 100.0% | 0 | 5 | 0 | 0 |
| libhk_math.a (vs vphysics) | 15/15 | 100.0% | 6918/6918 | 100.0% | 7 | 8 | 0 | 0 |
| libhavana_constraints.a (vs vphysics) | 143/143 | 100.0% | 72841/72841 | 100.0% | 53 | 90 | 0 | 0 |
| libivp_compactbuilder.a (vs vphysics) | 393/393 | 100.0% | 235146/235146 | 100.0% | 37 | 356 | 0 | 0 |
| libivp_physics.a (vs vphysics) | 1207/1207 | 100.0% | 317532/317532 | 100.0% | 360 | 847 | 0 | 0 |
| libshaderlib.a (vs materialsystem) | 13/13 | 100.0% | 1314/1314 | 100.0% | 4 | 9 | 0 | 0 |
| libmathlib.a (vs engine) | 95/95 | 100.0% | 29755/29755 | 100.0% | 31 | 64 | 0 | 0 |
| libmathlib_105.a (vs sourcevr) | 16/16 | 100.0% | 2554/2554 | 100.0% | 7 | 9 | 0 | 0 |
| libparticles.a (vs client) | 1463/1463 | 100.0% | 285508/285508 | 100.0% | 842 | 621 | 0 | 0 |
| libreplay_common.a (vs engine) | 6/6 | 100.0% | 657/657 | 100.0% | 2 | 4 | 0 | 0 |
| libtier1.a (vs engine) | 485/485 | 100.0% | 98806/98806 | 100.0% | 168 | 317 | 0 | 0 |
| libtier1_105.a (vs sourcevr) | 75/75 | 100.0% | 6098/6098 | 100.0% | 37 | 38 | 0 | 0 |
| libtier2.a (vs engine) | 65/65 | 100.0% | 36813/36813 | 100.0% | 19 | 46 | 0 | 0 |
| libtier2_105.a (vs sourcevr) | 10/10 | 100.0% | 1279/1279 | 100.0% | 1 | 9 | 0 | 0 |
| libtier3.a (vs engine) | 2/2 | 100.0% | 845/845 | 100.0% | 0 | 2 | 0 | 0 |
| libtier3_105.a (vs sourcevr) | 2/2 | 100.0% | 845/845 | 100.0% | 0 | 2 | 0 | 0 |
| libbzip2.a (vs engine) | 25/25 | 100.0% | 53130/53130 | 100.0% | 4 | 21 | 0 | 0 |
| libcef_dll_wrapper.a (vs chromehtml) | 760/760 | 100.0% | 74631/74631 | 100.0% | 437 | 323 | 0 | 0 |
| libmatsys_controls.a (vs engine) | 28/28 | 100.0% | 6761/6761 | 100.0% | 5 | 23 | 0 | 0 |
| libvgui_controls.a (vs engine) | 2523/2523 | 100.0% | 597641/597641 | 100.0% | 681 | 1842 | 0 | 0 |
| libvgui_surfacelib.a (vs vguimatsurface) | 92/92 | 100.0% | 22043/22043 | 100.0% | 23 | 69 | 0 | 0 |
| libvpklib.a (vs filesystem_stdio) | 241/241 | 100.0% | 55440/55440 | 100.0% | 81 | 160 | 0 | 0 |
| libcoroutine_osx.a (vs libvstdlib) | 32/32 | 100.0% | 9013/9013 | 100.0% | 8 | 24 | 0 | 0 |
| libvtf.a (vs engine) | 82/82 | 100.0% | 25047/25047 | 100.0% | 30 | 52 | 0 | 0 |
