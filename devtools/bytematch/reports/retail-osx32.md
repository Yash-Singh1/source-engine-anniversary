# --retail-osx32 match report (2026-11-18T14:34:37Z)

clang-600.0.57, ld64-242, retail build 19307283. Matched = exact + matches-after-masking (bytematch). Percentages are of the retail functions in scope (debug-map objects); bytes are retail bytes. The totals count linked modules only, since static libraries are linked into them. Game modules (server/client per game, against depots 380, 340 and 223) have their own rows and a second total.

| module | functions matched | % functions | bytes matched | % bytes | exact | masked | differs | missing |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| inputsystem.dylib | 193/193 | 100.0% | 29183/29183 | 100.0% | 193 | 0 | 0 | 0 |
| stdshader_dx9.dylib | 1963/1963 | 100.0% | 535678/535678 | 100.0% | 1963 | 0 | 0 | 0 |
| scenefilecache.dylib | 40/40 | 100.0% | 7785/7785 | 100.0% | 33 | 7 | 0 | 0 |
| soundemittersystem.dylib | 236/236 | 100.0% | 53556/53556 | 100.0% | 236 | 0 | 0 | 0 |
| libtier0.dylib | 367/367 | 100.0% | 58505/58505 | 100.0% | 126 | 241 | 0 | 0 |
| bugreporter_filequeue.dylib | 212/212 | 100.0% | 41424/41424 | 100.0% | 208 | 4 | 0 | 0 |
| bugreporter_public.dylib | 163/163 | 100.0% | 34863/34863 | 100.0% | 163 | 0 | 0 | 0 |
| vgui2.dylib | 684/684 | 100.0% | 133227/133227 | 100.0% | 578 | 106 | 0 | 0 |
| video_quicktime.dylib | 181/181 | 100.0% | 29196/29196 | 100.0% | 181 | 0 | 0 | 0 |
| video_services.dylib | 113/113 | 100.0% | 16116/16116 | 100.0% | 113 | 0 | 0 | 0 |
| libvstdlib.dylib | 355/355 | 100.0% | 55390/55390 | 100.0% | 114 | 241 | 0 | 0 |
| shaderapiempty.dylib | 586/588 | 99.66% | 8555/8569 | 99.84% | 525 | 61 | 0 | 2 |
| libtogl.dylib | 499/502 | 99.4% | 174993/179003 | 97.76% | 229 | 270 | 3 | 0 |
| studiorender.dylib | 979/988 | 99.09% | 482513/507743 | 95.03% | 254 | 725 | 9 | 0 |
| datacache.dylib | 402/406 | 99.01% | 132603/138135 | 96.0% | 202 | 200 | 4 | 0 |
| launcher.dylib | 352/361 | 97.51% | 79216/89485 | 88.52% | 107 | 245 | 9 | 0 |
| shaderapidx9.dylib | 1542/1592 | 96.86% | 273178/302451 | 90.32% | 623 | 919 | 48 | 2 |
| materialsystem.dylib | 3629/3747 | 96.85% | 457102/552250 | 82.77% | 1775 | 1854 | 116 | 2 |
| replay.dylib | 1337/1405 | 95.16% | 240447/254041 | 94.65% | 483 | 854 | 18 | 50 |
| engine.dylib | 12309/12986 | 94.79% | 3302494/3809922 | 86.68% | 3255 | 9054 | 527 | 150 |
| ServerBrowser.dylib | 3173/3380 | 93.88% | 653086/788052 | 82.87% | 849 | 2324 | 206 | 1 |
| bsppack.dylib | 343/368 | 93.21% | 110703/153265 | 72.23% | 95 | 248 | 16 | 9 |
| sourcevr.dylib | 130/141 | 92.2% | 16902/18800 | 89.9% | 54 | 76 | 10 | 1 |
| vphysics.dylib | 2730/2963 | 92.14% | 708541/891892 | 79.44% | 927 | 1803 | 215 | 18 |
| vtex_dll.dylib | 404/439 | 92.03% | 118473/149812 | 79.08% | 136 | 268 | 31 | 4 |
| vguimatsurface.dylib | 2708/2987 | 90.66% | 562607/709304 | 79.32% | 811 | 1897 | 263 | 16 |
| GameUI.dylib | 4312/4888 | 88.22% | 850483/1159816 | 73.33% | 951 | 3361 | 498 | 78 |
| filesystem_stdio.dylib | 1187/1953 | 60.78% | 356116/567545 | 62.75% | 338 | 849 | 548 | 218 |
| vpk_osx32 | 633/1883 | 33.62% | 119720/485072 | 24.68% | 247 | 386 | 915 | 335 |
| vaudio_speex.dylib | 19/131 | 14.5% | 2365/61295 | 3.86% | 4 | 15 | 87 | 25 |
| hl2_osx | 0/1 | 0.0% | 0/157 | 0.0% | 0 | 0 | 1 | 0 |
| **total, linked modules** | 41781/46216 | 90.4% | 9645020/11821532 | 81.59% | 15773 | 26008 | 3524 | 911 |
| **game modules (one --build-games build each)** | | | | | | | | |
| episodic/server.dylib | 22114/22744 | 97.23% | 7265792/7696434 | 94.4% | 5262 | 16852 | 505 | 125 |
| hl2/server.dylib | 22114/22744 | 97.23% | 7265792/7696434 | 94.4% | 5262 | 16852 | 505 | 125 |
| episodic/client.dylib | 17118/17821 | 96.06% | 3956706/4443243 | 89.05% | 5335 | 11783 | 572 | 131 |
| hl2/client.dylib | 17117/17820 | 96.05% | 3956679/4443188 | 89.05% | 5339 | 11778 | 572 | 131 |
| lostcoast/server.dylib | 20283/21442 | 94.59% | 6349744/7181480 | 88.42% | 5033 | 15250 | 1126 | 33 |
| lostcoast/client.dylib | 16445/17468 | 94.14% | 3758580/4356276 | 86.28% | 5183 | 11262 | 904 | 119 |
| **total, linked modules and games** | 156972/166255 | 94.42% | 42198313/47638587 | 88.58% | 47187 | 109785 | 7708 | 1575 |
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
| libparticles.a (vs client) | 1444/1463 | 98.7% | 253440/285508 | 88.77% | 842 | 602 | 14 | 5 |
| libvpklib.a (vs filesystem_stdio) | 237/241 | 98.34% | 53767/55440 | 96.98% | 81 | 156 | 4 | 0 |
| libivp_compactbuilder.a (vs vphysics) | 381/393 | 96.95% | 220322/235146 | 93.7% | 36 | 345 | 11 | 1 |
| libvgui_controls.a (vs engine) | 2396/2508 | 95.53% | 511678/595734 | 85.89% | 667 | 1729 | 112 | 0 |
| libtier2.a (vs engine) | 62/65 | 95.38% | 30423/36813 | 82.64% | 19 | 43 | 3 | 0 |
| libappframework.a (vs engine) | 31/34 | 91.18% | 8636/10412 | 82.94% | 1 | 30 | 2 | 1 |
| libvtf.a (vs engine) | 73/82 | 89.02% | 21474/25047 | 85.73% | 30 | 43 | 9 | 0 |
| libivp_physics.a (vs vphysics) | 1049/1207 | 86.91% | 211656/317532 | 66.66% | 338 | 711 | 152 | 6 |
| libhavana_constraints.a (vs vphysics) | 123/143 | 86.01% | 39070/72841 | 53.64% | 52 | 71 | 19 | 1 |
| libbitmap.a (vs engine) | 83/102 | 81.37% | 28079/53332 | 52.65% | 44 | 39 | 19 | 0 |
| libhk_base.a (vs vphysics) | 4/5 | 80.0% | 599/1219 | 49.14% | 0 | 4 | 1 | 0 |
| libvgui_surfacelib.a (vs vguimatsurface) | 54/92 | 58.7% | 8525/22043 | 38.67% | 15 | 39 | 30 | 8 |
