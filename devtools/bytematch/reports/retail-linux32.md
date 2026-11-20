# --retail-linux32 --retail-linux32-engine vs retail depot 224 (build 19307283)

Our functions (deduplicated), our sizes; a function counts as matched when its bytes equal a retail function's once relocated fields are masked (bytematch, stripped-ELF pairing). Objects: what ld linked into our module (its map): loose objects ("module") and the archive members it pulled.

| module | functions | matched | % | bytes | matched | % | own objects % funcs / bytes |
|---|---:|---:|---:|---:|---:|---:|---:|
| bsppack.so | 1412 | 1409 | 99.8% | 296045 | 293708 | 99.2% | 99.7% / 98.5% |
| bugreporter_filequeue.so | 677 | 676 | 99.9% | 114955 | 114951 | 100.0% | 100.0% / 100.0% |
| bugreporter_public.so | 374 | 373 | 99.7% | 68920 | 68916 | 100.0% | 100.0% / 100.0% |
| datacache.so | 1072 | 1065 | 99.3% | 282799 | 274762 | 97.2% | 98.9% / 96.3% |
| engine.so | 18397 | 18228 | 99.1% | 4988685 | 4807353 | 96.4% | 98.8% / 95.0% |
| filesystem_stdio.so | 2117 | 2094 | 98.9% | 577427 | 556021 | 96.3% | 98.4% / 94.9% |
| GameUI.so | 8328 | 8277 | 99.4% | 2080848 | 2033981 | 97.7% | 99.1% / 95.5% |
| inputsystem.so | 628 | 623 | 99.2% | 92953 | 89698 | 96.5% | 96.4% / 82.8% |
| launcher.so | 850 | 843 | 99.2% | 158144 | 151848 | 96.0% | 99.3% / 94.8% |
| libtier0.so | 501 | 487 | 97.2% | 67511 | 64073 | 94.9% | 97.2% / 94.9% |
| libtogl.so | 1317 | 1313 | 99.7% | 335524 | 321807 | 95.9% | 99.4% / 92.7% |
| libvstdlib.so | 712 | 710 | 99.7% | 119653 | 118939 | 99.4% | 99.6% / 98.9% |
| materialsystem.so | 4632 | 4613 | 99.6% | 794085 | 757088 | 95.3% | 99.5% / 92.3% |
| replay.so | 3692 | 3634 | 98.4% | 847819 | 798684 | 94.2% | 97.8% / 87.6% |
| scenefilecache.so | 239 | 238 | 99.6% | 37371 | 37367 | 100.0% | 100.0% / 100.0% |
| ServerBrowser.so | 4772 | 4704 | 98.6% | 1082378 | 1068269 | 98.7% | 99.9% / 100.0% |
| shaderapidx9.so | 2744 | 2730 | 99.5% | 571609 | 556988 | 97.4% | 99.6% / 97.0% |
| shaderapiempty.so | 584 | 583 | 99.8% | 25084 | 25080 | 100.0% | 99.8% / 99.9% |
| soundemittersystem.so | 657 | 656 | 99.8% | 120473 | 120469 | 100.0% | 100.0% / 100.0% |
| stdshader_dx9.so | 2584 | 2580 | 99.8% | 583762 | 582721 | 99.8% | 99.9% / 100.0% |
| studiorender.so | 1677 | 1670 | 99.6% | 659199 | 648745 | 98.4% | 99.3% / 98.0% |
| vaudio_speex.so | 156 | 155 | 99.4% | 21653 | 21649 | 100.0% | 97.6% / 99.9% |
| vgui2.so | 1246 | 1242 | 99.7% | 228739 | 223722 | 97.8% | 99.5% / 96.0% |
| vguimatsurface.so | 4584 | 4559 | 99.5% | 1100231 | 1081741 | 98.3% | 99.0% / 94.0% |
| video_services.so | 427 | 426 | 99.8% | 56392 | 56388 | 100.0% | 100.0% / 100.0% |
| vphysics.so | 4676 | 4604 | 98.5% | 1163065 | 1102533 | 94.8% | 99.5% / 98.1% |
| vtex_dll.so | 1272 | 1270 | 99.8% | 304708 | 302523 | 99.3% | 99.3% / 95.5% |
| hl2_linux | 2 | 2 | 100.0% | 168 | 168 | 100.0% | 100.0% / 100.0% |
| **total** | 70329 | 69764 | 99.2% | 16780200 | 16280192 | 97.0% | |

## bsppack.so

| objects | functions | matched | % | bytes | matched | % | located |
|---|---:|---:|---:|---:|---:|---:|---:|
| module | 613 | 611 | 99.7% | 152442 | 150109 | 98.5% | 613 |
| libtier1.a | 591 | 591 | 100.0% | 99864 | 99864 | 100.0% | 591 |
| libmathlib.a | 205 | 204 | 99.5% | 42965 | 42961 | 100.0% | 204 |
| libtier2.a | 3 | 3 | 100.0% | 774 | 774 | 100.0% | 3 |
| **total** | 1412 | 1409 | 99.8% | 296045 | 293708 | 99.2% | 1411 |

Paired with a retail function: 1411 (exact 331, after masking 1078, differs 2); report: `bsppack.md`.
Unmatched by macOS status: differs on macOS too 0 (0 bytes), matches on macOS 1 (2181 bytes), not on macOS 2 (156 bytes).

## bugreporter_filequeue.so

| objects | functions | matched | % | bytes | matched | % | located |
|---|---:|---:|---:|---:|---:|---:|---:|
| module | 85 | 85 | 100.0% | 16698 | 16698 | 100.0% | 85 |
| libtier1.a | 592 | 591 | 99.8% | 98257 | 98253 | 100.0% | 591 |
| **total** | 677 | 676 | 99.9% | 114955 | 114951 | 100.0% | 676 |

Paired with a retail function: 676 (exact 172, after masking 504, differs 0); report: `bugreporter_filequeue.md`.
Unmatched by macOS status: differs on macOS too 0 (0 bytes), matches on macOS 0 (0 bytes), not on macOS 1 (4 bytes).

## bugreporter_public.so

| objects | functions | matched | % | bytes | matched | % | located |
|---|---:|---:|---:|---:|---:|---:|---:|
| module | 91 | 91 | 100.0% | 19173 | 19173 | 100.0% | 91 |
| libtier1.a | 269 | 268 | 99.6% | 47857 | 47853 | 100.0% | 268 |
| libmathlib.a | 14 | 14 | 100.0% | 1890 | 1890 | 100.0% | 14 |
| **total** | 374 | 373 | 99.7% | 68920 | 68916 | 100.0% | 373 |

Paired with a retail function: 373 (exact 118, after masking 255, differs 0); report: `bugreporter_public.md`.
Unmatched by macOS status: differs on macOS too 0 (0 bytes), matches on macOS 0 (0 bytes), not on macOS 1 (4 bytes).

## datacache.so

| objects | functions | matched | % | bytes | matched | % | located |
|---|---:|---:|---:|---:|---:|---:|---:|
| module | 612 | 605 | 98.9% | 217966 | 209929 | 96.3% | 609 |
| libtier1.a | 428 | 428 | 100.0% | 56134 | 56134 | 100.0% | 428 |
| libtier3.a | 3 | 3 | 100.0% | 1110 | 1110 | 100.0% | 3 |
| libtier2.a | 29 | 29 | 100.0% | 7589 | 7589 | 100.0% | 29 |
| **total** | 1072 | 1065 | 99.3% | 282799 | 274762 | 97.2% | 1069 |

Paired with a retail function: 1069 (exact 312, after masking 753, differs 4); report: `datacache.md`.
Unmatched by macOS status: differs on macOS too 2 (1880 bytes), matches on macOS 4 (6153 bytes), not on macOS 1 (4 bytes).

## engine.so

| objects | functions | matched | % | bytes | matched | % | located |
|---|---:|---:|---:|---:|---:|---:|---:|
| module | 12478 | 12323 | 98.8% | 3399128 | 3228874 | 95.0% | 12411 |
| libbzip2.a | 40 | 40 | 100.0% | 58214 | 58214 | 100.0% | 40 |
| libjpeg.a | 405 | 399 | 98.5% | 231781 | 224881 | 97.0% | 399 |
| libdmxloader.a | 244 | 244 | 100.0% | 77767 | 77767 | 100.0% | 244 |
| libtier1.a | 896 | 895 | 99.9% | 158652 | 158224 | 99.7% | 895 |
| libreplay_common.a | 114 | 114 | 100.0% | 9771 | 9771 | 100.0% | 114 |
| libtier2.a | 146 | 146 | 100.0% | 47192 | 47192 | 100.0% | 146 |
| libvgui_controls.a | 3362 | 3357 | 99.9% | 791107 | 787365 | 99.5% | 3362 |
| libmatsys_controls.a | 47 | 47 | 100.0% | 7435 | 7435 | 100.0% | 47 |
| libbitmap.a | 211 | 210 | 99.5% | 105495 | 105491 | 100.0% | 210 |
| libvtf.a | 121 | 121 | 100.0% | 22704 | 22704 | 100.0% | 121 |
| libappframework.a | 54 | 53 | 98.1% | 13018 | 13014 | 100.0% | 53 |
| libmathlib.a | 276 | 276 | 100.0% | 65311 | 65311 | 100.0% | 276 |
| libtier3.a | 3 | 3 | 100.0% | 1110 | 1110 | 100.0% | 3 |
| **total** | 18397 | 18228 | 99.1% | 4988685 | 4807353 | 96.4% | 18321 |

Paired with a retail function: 18311 (exact 4687, after masking 13531, differs 93); report: `engine.md`.
Unmatched by macOS status: differs on macOS too 58 (53588 bytes), matches on macOS 75 (91708 bytes), not on macOS 36 (36036 bytes).

## filesystem_stdio.so

| objects | functions | matched | % | bytes | matched | % | located |
|---|---:|---:|---:|---:|---:|---:|---:|
| module | 1405 | 1382 | 98.4% | 421861 | 400455 | 94.9% | 1396 |
| libvpklib.a | 102 | 102 | 100.0% | 48634 | 48634 | 100.0% | 102 |
| libtier2.a | 3 | 3 | 100.0% | 774 | 774 | 100.0% | 3 |
| libtier1.a | 607 | 607 | 100.0% | 106158 | 106158 | 100.0% | 607 |
| **total** | 2117 | 2094 | 98.9% | 577427 | 556021 | 96.3% | 2108 |

Paired with a retail function: 2107 (exact 505, after masking 1588, differs 14); report: `filesystem_stdio.md`.
Unmatched by macOS status: differs on macOS too 2 (1044 bytes), matches on macOS 7 (11653 bytes), not on macOS 14 (8709 bytes).

## GameUI.so

| objects | functions | matched | % | bytes | matched | % | located |
|---|---:|---:|---:|---:|---:|---:|---:|
| module | 2673 | 2649 | 99.1% | 593566 | 566834 | 95.5% | 2668 |
| libmathlib.a | 262 | 262 | 100.0% | 61101 | 61101 | 100.0% | 262 |
| libtier3.a | 3 | 3 | 100.0% | 1110 | 1110 | 100.0% | 3 |
| libvgui_controls.a | 3669 | 3662 | 99.8% | 860999 | 854988 | 99.3% | 3669 |
| libmatsys_controls.a | 7 | 7 | 100.0% | 540 | 540 | 100.0% | 7 |
| libbitmap.a | 209 | 207 | 99.0% | 103375 | 103367 | 100.0% | 207 |
| libvtf.a | 122 | 122 | 100.0% | 22812 | 22812 | 100.0% | 122 |
| libtier2.a | 3 | 3 | 100.0% | 774 | 774 | 100.0% | 3 |
| libtier1.a | 623 | 623 | 100.0% | 104255 | 104255 | 100.0% | 623 |
| libjpeg.a | 405 | 399 | 98.5% | 231781 | 224881 | 97.0% | 399 |
| libpng.a | 352 | 340 | 96.6% | 100535 | 93319 | 92.8% | 347 |
| **total** | 8328 | 8277 | 99.4% | 2080848 | 2033981 | 97.7% | 8310 |

Paired with a retail function: 8308 (exact 1961, after masking 6314, differs 33); report: `GameUI.md`.
Unmatched by macOS status: differs on macOS too 5 (4750 bytes), matches on macOS 21 (21847 bytes), not on macOS 25 (20270 bytes).

## inputsystem.so

| objects | functions | matched | % | bytes | matched | % | located |
|---|---:|---:|---:|---:|---:|---:|---:|
| module | 139 | 134 | 96.4% | 18932 | 15677 | 82.8% | 138 |
| libtier2.a | 3 | 3 | 100.0% | 774 | 774 | 100.0% | 3 |
| libtier1.a | 486 | 486 | 100.0% | 73247 | 73247 | 100.0% | 486 |
| **total** | 628 | 623 | 99.2% | 92953 | 89698 | 96.5% | 627 |

Paired with a retail function: 627 (exact 197, after masking 426, differs 4); report: `inputsystem.md`.
Unmatched by macOS status: differs on macOS too 0 (0 bytes), matches on macOS 4 (3251 bytes), not on macOS 1 (4 bytes).

## launcher.so

| objects | functions | matched | % | bytes | matched | % | located |
|---|---:|---:|---:|---:|---:|---:|---:|
| module | 139 | 138 | 99.3% | 42290 | 40109 | 94.8% | 139 |
| libtier2.a | 3 | 3 | 100.0% | 774 | 774 | 100.0% | 3 |
| libtier1.a | 550 | 550 | 100.0% | 83926 | 83926 | 100.0% | 550 |
| libappframework.a | 155 | 149 | 96.1% | 30044 | 25929 | 86.3% | 153 |
| libtier3.a | 3 | 3 | 100.0% | 1110 | 1110 | 100.0% | 3 |
| **total** | 850 | 843 | 99.2% | 158144 | 151848 | 96.0% | 848 |

Paired with a retail function: 848 (exact 235, after masking 608, differs 5); report: `launcher.md`.
Unmatched by macOS status: differs on macOS too 0 (0 bytes), matches on macOS 5 (5742 bytes), not on macOS 2 (554 bytes).

## libtier0.so

| objects | functions | matched | % | bytes | matched | % | located |
|---|---:|---:|---:|---:|---:|---:|---:|
| module | 501 | 487 | 97.2% | 67511 | 64073 | 94.9% | 492 |
| **total** | 501 | 487 | 97.2% | 67511 | 64073 | 94.9% | 492 |

Paired with a retail function: 486 (exact 119, after masking 362, differs 5); report: `libtier0.md`.
Unmatched by macOS status: differs on macOS too 0 (0 bytes), matches on macOS 3 (990 bytes), not on macOS 11 (2448 bytes).

## libtogl.so

| objects | functions | matched | % | bytes | matched | % | located |
|---|---:|---:|---:|---:|---:|---:|---:|
| module | 488 | 485 | 99.4% | 187032 | 173319 | 92.7% | 488 |
| libmathlib.a | 268 | 267 | 99.6% | 61125 | 61121 | 100.0% | 267 |
| libtier2.a | 3 | 3 | 100.0% | 774 | 774 | 100.0% | 3 |
| libtier1.a | 558 | 558 | 100.0% | 86593 | 86593 | 100.0% | 558 |
| **total** | 1317 | 1313 | 99.7% | 335524 | 321807 | 95.9% | 1316 |

Paired with a retail function: 1314 (exact 301, after masking 1010, differs 3); report: `libtogl.md`.
Unmatched by macOS status: differs on macOS too 0 (0 bytes), matches on macOS 3 (13713 bytes), not on macOS 1 (4 bytes).

## libvstdlib.so

| objects | functions | matched | % | bytes | matched | % | located |
|---|---:|---:|---:|---:|---:|---:|---:|
| module | 254 | 253 | 99.6% | 62143 | 61433 | 98.9% | 254 |
| libtier1.a | 458 | 457 | 99.8% | 57510 | 57506 | 100.0% | 457 |
| **total** | 712 | 710 | 99.7% | 119653 | 118939 | 99.4% | 711 |

Paired with a retail function: 709 (exact 191, after masking 517, differs 1); report: `libvstdlib.md`.
Unmatched by macOS status: differs on macOS too 0 (0 bytes), matches on macOS 1 (710 bytes), not on macOS 1 (4 bytes).

## materialsystem.so

| objects | functions | matched | % | bytes | matched | % | located |
|---|---:|---:|---:|---:|---:|---:|---:|
| module | 3424 | 3406 | 99.5% | 482321 | 445328 | 92.3% | 3418 |
| libvtf.a | 122 | 122 | 100.0% | 22812 | 22812 | 100.0% | 122 |
| libtier2.a | 58 | 58 | 100.0% | 35182 | 35182 | 100.0% | 58 |
| libmathlib.a | 268 | 268 | 100.0% | 61662 | 61662 | 100.0% | 268 |
| libshaderlib.a | 18 | 18 | 100.0% | 1370 | 1370 | 100.0% | 18 |
| libtier1.a | 544 | 544 | 100.0% | 88809 | 88809 | 100.0% | 544 |
| libbitmap.a | 198 | 197 | 99.5% | 101929 | 101925 | 100.0% | 197 |
| **total** | 4632 | 4613 | 99.6% | 794085 | 757088 | 95.3% | 4625 |

Paired with a retail function: 4623 (exact 2103, after masking 2508, differs 12); report: `materialsystem.md`.
Unmatched by macOS status: differs on macOS too 4 (962 bytes), matches on macOS 9 (30507 bytes), not on macOS 6 (5528 bytes).

## replay.so

| objects | functions | matched | % | bytes | matched | % | located |
|---|---:|---:|---:|---:|---:|---:|---:|
| module | 1818 | 1778 | 97.8% | 281429 | 246410 | 87.6% | 1785 |
| libtier1.a | 639 | 639 | 100.0% | 99746 | 99746 | 100.0% | 639 |
| libreplay_common.a | 107 | 107 | 100.0% | 9022 | 9022 | 100.0% | 107 |
| libtier2.a | 3 | 3 | 100.0% | 774 | 774 | 100.0% | 3 |
| libpng.a | 352 | 340 | 96.6% | 100535 | 93319 | 92.8% | 347 |
| libvtf.a | 122 | 122 | 100.0% | 22812 | 22812 | 100.0% | 122 |
| libbzip2.a | 40 | 40 | 100.0% | 58214 | 58214 | 100.0% | 40 |
| libmathlib.a | 206 | 206 | 100.0% | 43506 | 43506 | 100.0% | 206 |
| libjpeg.a | 405 | 399 | 98.5% | 231781 | 224881 | 97.0% | 399 |
| **total** | 3692 | 3634 | 98.4% | 847819 | 798684 | 94.2% | 3648 |

Paired with a retail function: 3647 (exact 1399, after masking 2234, differs 14); report: `replay.md`.
Unmatched by macOS status: differs on macOS too 31 (26728 bytes), matches on macOS 6 (7325 bytes), not on macOS 21 (15082 bytes).

## scenefilecache.so

| objects | functions | matched | % | bytes | matched | % | located |
|---|---:|---:|---:|---:|---:|---:|---:|
| module | 19 | 19 | 100.0% | 1965 | 1965 | 100.0% | 19 |
| libtier1.a | 220 | 219 | 99.5% | 35406 | 35402 | 100.0% | 219 |
| **total** | 239 | 238 | 99.6% | 37371 | 37367 | 100.0% | 238 |

Paired with a retail function: 238 (exact 81, after masking 157, differs 0); report: `scenefilecache.md`.
Unmatched by macOS status: differs on macOS too 0 (0 bytes), matches on macOS 0 (0 bytes), not on macOS 1 (4 bytes).

## ServerBrowser.so

| objects | functions | matched | % | bytes | matched | % | located |
|---|---:|---:|---:|---:|---:|---:|---:|
| module | 801 | 800 | 99.9% | 191434 | 191430 | 100.0% | 800 |
| libtier3.a | 3 | 3 | 100.0% | 1110 | 1110 | 100.0% | 3 |
| libvgui_controls.a | 3332 | 3327 | 99.8% | 786330 | 782588 | 99.5% | 3332 |
| libtier2.a | 3 | 3 | 100.0% | 774 | 774 | 100.0% | 3 |
| libtier1.a | 633 | 571 | 90.2% | 102730 | 92367 | 89.9% | 571 |
| **total** | 4772 | 4704 | 98.6% | 1082378 | 1068269 | 98.7% | 4709 |

Paired with a retail function: 4709 (exact 1183, after masking 3521, differs 5); report: `ServerBrowser.md`.
Objects with no function located in retail: `pathmatch.cpp.0.o`.
Unmatched by macOS status: differs on macOS too 0 (0 bytes), matches on macOS 3 (1395 bytes), not on macOS 65 (12714 bytes).

## shaderapidx9.so

| objects | functions | matched | % | bytes | matched | % | located |
|---|---:|---:|---:|---:|---:|---:|---:|
| module | 1587 | 1581 | 99.6% | 284363 | 275839 | 97.0% | 1582 |
| libbitmap.a | 193 | 193 | 100.0% | 101909 | 101909 | 100.0% | 193 |
| libmathlib.a | 262 | 262 | 100.0% | 61101 | 61101 | 100.0% | 262 |
| libappframework.a | 92 | 86 | 93.5% | 16419 | 12304 | 74.9% | 90 |
| libtier2.a | 8 | 8 | 100.0% | 2271 | 2271 | 100.0% | 8 |
| libtier1.a | 602 | 600 | 99.7% | 105546 | 103564 | 98.1% | 602 |
| **total** | 2744 | 2730 | 99.5% | 571609 | 556988 | 97.4% | 2737 |

Paired with a retail function: 2735 (exact 1018, after masking 1710, differs 7); report: `shaderapidx9.md`.
Unmatched by macOS status: differs on macOS too 3 (7965 bytes), matches on macOS 4 (3561 bytes), not on macOS 7 (3095 bytes).

## shaderapiempty.so

| objects | functions | matched | % | bytes | matched | % | located |
|---|---:|---:|---:|---:|---:|---:|---:|
| module | 452 | 451 | 99.8% | 5799 | 5795 | 99.9% | 451 |
| libtier1.a | 132 | 132 | 100.0% | 19285 | 19285 | 100.0% | 132 |
| **total** | 584 | 583 | 99.8% | 25084 | 25080 | 100.0% | 583 |

Paired with a retail function: 583 (exact 434, after masking 149, differs 0); report: `shaderapiempty.md`.
Unmatched by macOS status: differs on macOS too 0 (0 bytes), matches on macOS 0 (0 bytes), not on macOS 1 (4 bytes).

## soundemittersystem.so

| objects | functions | matched | % | bytes | matched | % | located |
|---|---:|---:|---:|---:|---:|---:|---:|
| module | 128 | 128 | 100.0% | 31977 | 31977 | 100.0% | 128 |
| libtier1.a | 529 | 528 | 99.8% | 88496 | 88492 | 100.0% | 528 |
| **total** | 657 | 656 | 99.8% | 120473 | 120469 | 100.0% | 656 |

Paired with a retail function: 656 (exact 181, after masking 475, differs 0); report: `soundemittersystem.md`.
Unmatched by macOS status: differs on macOS too 0 (0 bytes), matches on macOS 0 (0 bytes), not on macOS 1 (4 bytes).

## stdshader_dx9.so

| objects | functions | matched | % | bytes | matched | % | located |
|---|---:|---:|---:|---:|---:|---:|---:|
| module | 1860 | 1859 | 99.9% | 463613 | 463476 | 100.0% | 1859 |
| libshaderlib.a | 105 | 102 | 97.1% | 14138 | 13234 | 93.6% | 103 |
| libtier2.a | 3 | 3 | 100.0% | 774 | 774 | 100.0% | 3 |
| libmathlib.a | 262 | 262 | 100.0% | 61101 | 61101 | 100.0% | 262 |
| libtier1.a | 354 | 354 | 100.0% | 44136 | 44136 | 100.0% | 354 |
| **total** | 2584 | 2580 | 99.8% | 583762 | 582721 | 99.8% | 2581 |

Paired with a retail function: 2581 (exact 468, after masking 2112, differs 1); report: `stdshader_dx9.md`.
Unmatched by macOS status: differs on macOS too 0 (0 bytes), matches on macOS 0 (0 bytes), not on macOS 4 (1041 bytes).

## studiorender.so

| objects | functions | matched | % | bytes | matched | % | located |
|---|---:|---:|---:|---:|---:|---:|---:|
| module | 913 | 907 | 99.3% | 521690 | 511240 | 98.0% | 913 |
| libtier3.a | 3 | 3 | 100.0% | 1110 | 1110 | 100.0% | 3 |
| libmathlib.a | 268 | 267 | 99.6% | 61125 | 61121 | 100.0% | 267 |
| libtier2.a | 8 | 8 | 100.0% | 2271 | 2271 | 100.0% | 8 |
| libtier1.a | 485 | 485 | 100.0% | 73003 | 73003 | 100.0% | 485 |
| **total** | 1677 | 1670 | 99.6% | 659199 | 648745 | 98.4% | 1676 |

Paired with a retail function: 1676 (exact 390, after masking 1280, differs 6); report: `studiorender.md`.
Unmatched by macOS status: differs on macOS too 1 (3029 bytes), matches on macOS 5 (7421 bytes), not on macOS 1 (4 bytes).

## vaudio_speex.so

| objects | functions | matched | % | bytes | matched | % | located |
|---|---:|---:|---:|---:|---:|---:|---:|
| module | 42 | 41 | 97.6% | 4103 | 4099 | 99.9% | 41 |
| libtier1.a | 114 | 114 | 100.0% | 17550 | 17550 | 100.0% | 114 |
| **total** | 156 | 155 | 99.4% | 21653 | 21649 | 100.0% | 155 |

Paired with a retail function: 155 (exact 43, after masking 112, differs 0); report: `vaudio_speex.md`.
Unmatched by macOS status: differs on macOS too 0 (0 bytes), matches on macOS 0 (0 bytes), not on macOS 1 (4 bytes).

## vgui2.so

| objects | functions | matched | % | bytes | matched | % | located |
|---|---:|---:|---:|---:|---:|---:|---:|
| module | 640 | 637 | 99.5% | 126812 | 121799 | 96.0% | 638 |
| libtier1.a | 600 | 599 | 99.8% | 100043 | 100039 | 100.0% | 599 |
| libtier2.a | 3 | 3 | 100.0% | 774 | 774 | 100.0% | 3 |
| libtier3.a | 3 | 3 | 100.0% | 1110 | 1110 | 100.0% | 3 |
| **total** | 1246 | 1242 | 99.7% | 228739 | 223722 | 97.8% | 1243 |

Paired with a retail function: 1242 (exact 396, after masking 845, differs 1); report: `vgui2.md`.
Unmatched by macOS status: differs on macOS too 2 (2832 bytes), matches on macOS 1 (2181 bytes), not on macOS 1 (4 bytes).

## vguimatsurface.so

| objects | functions | matched | % | bytes | matched | % | located |
|---|---:|---:|---:|---:|---:|---:|---:|
| module | 386 | 382 | 99.0% | 109480 | 102937 | 94.0% | 386 |
| libvgui_controls.a | 3033 | 3028 | 99.8% | 693889 | 690147 | 99.5% | 3033 |
| libtier1.a | 555 | 555 | 100.0% | 91531 | 91531 | 100.0% | 555 |
| libbitmap.a | 191 | 189 | 99.0% | 96782 | 96774 | 100.0% | 189 |
| libtier2.a | 26 | 26 | 100.0% | 3398 | 3398 | 100.0% | 26 |
| libmathlib.a | 262 | 262 | 100.0% | 61101 | 61101 | 100.0% | 262 |
| libvgui_surfacelib.a | 128 | 114 | 89.1% | 42940 | 34743 | 80.9% | 124 |
| libtier3.a | 3 | 3 | 100.0% | 1110 | 1110 | 100.0% | 3 |
| **total** | 4584 | 4559 | 99.5% | 1100231 | 1081741 | 98.3% | 4578 |

Paired with a retail function: 4572 (exact 1154, after masking 3399, differs 19); report: `vguimatsurface.md`.
Unmatched by macOS status: differs on macOS too 0 (0 bytes), matches on macOS 8 (8394 bytes), not on macOS 17 (10096 bytes).

## video_services.so

| objects | functions | matched | % | bytes | matched | % | located |
|---|---:|---:|---:|---:|---:|---:|---:|
| module | 61 | 61 | 100.0% | 10348 | 10348 | 100.0% | 61 |
| libtier2.a | 3 | 3 | 100.0% | 774 | 774 | 100.0% | 3 |
| libtier3.a | 3 | 3 | 100.0% | 1110 | 1110 | 100.0% | 3 |
| libtier1.a | 360 | 359 | 99.7% | 44160 | 44156 | 100.0% | 359 |
| **total** | 427 | 426 | 99.8% | 56392 | 56388 | 100.0% | 426 |

Paired with a retail function: 426 (exact 161, after masking 265, differs 0); report: `video_services.md`.
Unmatched by macOS status: differs on macOS too 0 (0 bytes), matches on macOS 0 (0 bytes), not on macOS 1 (4 bytes).

## vphysics.so

| objects | functions | matched | % | bytes | matched | % | located |
|---|---:|---:|---:|---:|---:|---:|---:|
| module | 1270 | 1264 | 99.5% | 262640 | 257636 | 98.1% | 1269 |
| libivp_compactbuilder.a | 602 | 598 | 99.3% | 264125 | 257471 | 97.5% | 601 |
| libhavana_constraints.a | 181 | 173 | 95.6% | 84136 | 61311 | 72.9% | 175 |
| libivp_physics.a | 1892 | 1839 | 97.2% | 410971 | 384926 | 93.7% | 1869 |
| libtier1.a | 441 | 441 | 100.0% | 68123 | 68123 | 100.0% | 441 |
| libhk_math.a | 46 | 46 | 100.0% | 13191 | 13191 | 100.0% | 46 |
| libmathlib.a | 217 | 217 | 100.0% | 58072 | 58072 | 100.0% | 217 |
| libhk_base.a | 27 | 26 | 96.3% | 1807 | 1803 | 99.8% | 26 |
| **total** | 4676 | 4604 | 98.5% | 1163065 | 1102533 | 94.8% | 4644 |

Paired with a retail function: 4640 (exact 1346, after masking 3254, differs 40); report: `vphysics.md`.
Unmatched by macOS status: differs on macOS too 22 (19255 bytes), matches on macOS 26 (32556 bytes), not on macOS 24 (8721 bytes).

## vtex_dll.so

| objects | functions | matched | % | bytes | matched | % | located |
|---|---:|---:|---:|---:|---:|---:|---:|
| module | 145 | 144 | 99.3% | 48381 | 46200 | 95.5% | 145 |
| libbitmap.a | 218 | 217 | 99.5% | 102695 | 102691 | 100.0% | 217 |
| libvtf.a | 122 | 122 | 100.0% | 22812 | 22812 | 100.0% | 122 |
| libmathlib.a | 200 | 200 | 100.0% | 42945 | 42945 | 100.0% | 200 |
| libtier2.a | 37 | 37 | 100.0% | 3949 | 3949 | 100.0% | 37 |
| libtier1.a | 550 | 550 | 100.0% | 83926 | 83926 | 100.0% | 550 |
| **total** | 1272 | 1270 | 99.8% | 304708 | 302523 | 99.3% | 1271 |

Paired with a retail function: 1271 (exact 305, after masking 965, differs 1); report: `vtex_dll.md`.
Unmatched by macOS status: differs on macOS too 0 (0 bytes), matches on macOS 1 (2181 bytes), not on macOS 1 (4 bytes).

## hl2_linux

| objects | functions | matched | % | bytes | matched | % | located |
|---|---:|---:|---:|---:|---:|---:|---:|
| module | 2 | 2 | 100.0% | 168 | 168 | 100.0% | 2 |
| **total** | 2 | 2 | 100.0% | 168 | 168 | 100.0% | 2 |

Paired with a retail function: 2 (exact 1, after masking 1, differs 0); report: `hl2_linux.md`.
Unmatched by macOS status: differs on macOS too 0 (0 bytes), matches on macOS 0 (0 bytes), not on macOS 0 (0 bytes).
