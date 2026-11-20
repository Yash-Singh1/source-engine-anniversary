# --retail-linux32 vs retail episodic (build 19307283)

Our functions (deduplicated), our sizes; a function counts as matched when its bytes equal a retail function's once relocated fields are masked (bytematch, stripped-ELF pairing). Static libraries: the members the retail macOS module of the same build pulls.

## server.so

| objects | functions | matched | % | bytes | matched | % | located |
|---|---:|---:|---:|---:|---:|---:|---:|
| module | 26796 | 26729 | 99.7% | 8047095 | 7974635 | 99.1% | 26769 |
| libchoreoobjects.a | 502 | 502 | 100.0% | 121009 | 121009 | 100.0% | 502 |
| libtier1.a | 733 | 733 | 100.0% | 130176 | 130176 | 100.0% | 733 |
| libparticles.a | 224 | 224 | 100.0% | 50441 | 50441 | 100.0% | 224 |
| libdmxloader.a | 231 | 231 | 100.0% | 68907 | 68907 | 100.0% | 231 |
| libmathlib.a | 280 | 280 | 100.0% | 63491 | 63491 | 100.0% | 280 |
| libtier3.a | 3 | 3 | 100.0% | 1110 | 1110 | 100.0% | 3 |
| libtier2.a | 21 | 21 | 100.0% | 1901 | 1901 | 100.0% | 21 |
| **total** | 28790 | 28723 | 99.8% | 8484130 | 8411670 | 99.1% | 28763 |

Paired with a retail function: 28719 (exact 6045, after masking 22634, differs 40); report: `episodic-server.md`.
Linux-only library files with no function in retail (not counted): pathmatch.cpp.0.o.

## client.so

| objects | functions | matched | % | bytes | matched | % | located |
|---|---:|---:|---:|---:|---:|---:|---:|
| module | 16467 | 16396 | 99.6% | 3847510 | 3782772 | 98.3% | 16434 |
| libtier3.a | 20 | 20 | 100.0% | 5977 | 5977 | 100.0% | 20 |
| libmatsys_controls.a | 155 | 153 | 98.7% | 35250 | 35068 | 99.5% | 154 |
| libchoreoobjects.a | 498 | 498 | 100.0% | 120101 | 120101 | 100.0% | 498 |
| libtier2.a | 71 | 71 | 100.0% | 42391 | 42391 | 100.0% | 71 |
| libtier1.a | 800 | 800 | 100.0% | 139835 | 139835 | 100.0% | 800 |
| libvgui_controls.a | 3218 | 3213 | 99.8% | 759548 | 755806 | 99.5% | 3218 |
| libbitmap.a | 167 | 165 | 98.8% | 95515 | 95507 | 100.0% | 165 |
| libvtf.a | 121 | 121 | 100.0% | 22704 | 22704 | 100.0% | 121 |
| libparticles.a | 1551 | 1551 | 100.0% | 238500 | 238500 | 100.0% | 1551 |
| libdmxloader.a | 227 | 226 | 99.6% | 68891 | 68818 | 99.9% | 226 |
| libmathlib.a | 296 | 296 | 100.0% | 69048 | 69048 | 100.0% | 296 |
| libjpeg.a | 405 | 399 | 98.5% | 231781 | 224881 | 97.0% | 399 |
| libpng.a | 352 | 340 | 96.6% | 100535 | 93319 | 92.8% | 347 |
| **total** | 24348 | 24249 | 99.6% | 5777586 | 5694727 | 98.6% | 24300 |

Paired with a retail function: 24293 (exact 7034, after masking 17208, differs 51); report: `episodic-client.md`.
