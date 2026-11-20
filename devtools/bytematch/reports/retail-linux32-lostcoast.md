# --retail-linux32 vs retail lostcoast (build 19307283)

Our functions (deduplicated), our sizes; a function counts as matched when its bytes equal a retail function's once relocated fields are masked (bytematch, stripped-ELF pairing). Static libraries: the members the retail macOS module of the same build pulls.

## server.so

| objects | functions | matched | % | bytes | matched | % | located |
|---|---:|---:|---:|---:|---:|---:|---:|
| module | 25397 | 25311 | 99.7% | 7511617 | 7374022 | 98.2% | 25380 |
| libchoreoobjects.a | 501 | 501 | 100.0% | 120643 | 120643 | 100.0% | 501 |
| libtier1.a | 733 | 731 | 99.7% | 129877 | 129351 | 99.6% | 733 |
| libparticles.a | 224 | 224 | 100.0% | 50441 | 50441 | 100.0% | 224 |
| libdmxloader.a | 231 | 231 | 100.0% | 68907 | 68907 | 100.0% | 231 |
| libmathlib.a | 280 | 280 | 100.0% | 63491 | 63491 | 100.0% | 280 |
| libtier3.a | 3 | 3 | 100.0% | 1110 | 1110 | 100.0% | 3 |
| libtier2.a | 21 | 21 | 100.0% | 1901 | 1901 | 100.0% | 21 |
| **total** | 27390 | 27302 | 99.7% | 7947987 | 7809866 | 98.3% | 27373 |

Paired with a retail function: 27329 (exact 5899, after masking 21359, differs 71); report: `lostcoast-server.md`.
Linux-only library files with no function in retail (not counted): pathmatch.cpp.12.o.

## client.so

| objects | functions | matched | % | bytes | matched | % | located |
|---|---:|---:|---:|---:|---:|---:|---:|
| module | 16128 | 15965 | 99.0% | 3772849 | 3589246 | 95.1% | 16073 |
| libtier3.a | 20 | 20 | 100.0% | 5977 | 5977 | 100.0% | 20 |
| libmatsys_controls.a | 155 | 153 | 98.7% | 35250 | 35068 | 99.5% | 154 |
| libchoreoobjects.a | 499 | 499 | 100.0% | 120236 | 120236 | 100.0% | 499 |
| libtier2.a | 71 | 71 | 100.0% | 42391 | 42391 | 100.0% | 71 |
| libtier1.a | 800 | 798 | 99.8% | 139536 | 139010 | 99.6% | 800 |
| libvgui_controls.a | 3211 | 3203 | 99.8% | 734956 | 731017 | 99.5% | 3209 |
| libbitmap.a | 167 | 165 | 98.8% | 95515 | 95507 | 100.0% | 165 |
| libvtf.a | 121 | 121 | 100.0% | 22704 | 22704 | 100.0% | 121 |
| libparticles.a | 1551 | 1551 | 100.0% | 238500 | 238500 | 100.0% | 1551 |
| libdmxloader.a | 227 | 226 | 99.6% | 68891 | 68818 | 99.9% | 226 |
| libmathlib.a | 296 | 296 | 100.0% | 69048 | 69048 | 100.0% | 296 |
| libjpeg.a | 405 | 399 | 98.5% | 231781 | 224881 | 97.0% | 399 |
| libpng.a | 352 | 340 | 96.6% | 100535 | 93319 | 92.8% | 347 |
| **total** | 24003 | 23807 | 99.2% | 5678169 | 5475722 | 96.4% | 23931 |

Paired with a retail function: 23917 (exact 6969, after masking 16824, differs 124); report: `lostcoast-client.md`.
