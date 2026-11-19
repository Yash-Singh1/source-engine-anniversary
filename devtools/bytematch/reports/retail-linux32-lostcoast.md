# --retail-linux32 vs retail lostcoast (build 19307283)

Our functions (deduplicated), our sizes; a function counts as matched when its bytes equal a retail function's once relocated fields are masked (bytematch, stripped-ELF pairing). Static libraries: the members the retail macOS module of the same build pulls.

## server.so

| objects | functions | matched | % | bytes | matched | % | located |
|---|---:|---:|---:|---:|---:|---:|---:|
| module | 25307 | 25112 | 99.2% | 7487066 | 7220151 | 96.4% | 25250 |
| libchoreoobjects.a | 501 | 500 | 99.8% | 120643 | 114340 | 94.8% | 501 |
| libtier1.a | 771 | 728 | 94.4% | 135286 | 128814 | 95.2% | 731 |
| libparticles.a | 224 | 223 | 99.6% | 50473 | 50105 | 99.3% | 224 |
| libdmxloader.a | 230 | 230 | 100.0% | 68795 | 68795 | 100.0% | 230 |
| libmathlib.a | 280 | 277 | 98.9% | 63528 | 62818 | 98.9% | 280 |
| libtier3.a | 3 | 3 | 100.0% | 1110 | 1110 | 100.0% | 3 |
| libtier2.a | 21 | 21 | 100.0% | 1901 | 1901 | 100.0% | 21 |
| **total** | 27337 | 27094 | 99.1% | 7928802 | 7648034 | 96.5% | 27240 |

Paired with a retail function: 27192 (exact 5830, after masking 21216, differs 146); report: `lostcoast-server.md`.
Linux-only library files with no function in retail (not counted): pathmatch.cpp.12.o.

## client.so

| objects | functions | matched | % | bytes | matched | % | located |
|---|---:|---:|---:|---:|---:|---:|---:|
| module | 16100 | 15780 | 98.0% | 3764729 | 3431649 | 91.2% | 15990 |
| libtier3.a | 20 | 20 | 100.0% | 5977 | 5977 | 100.0% | 20 |
| libmatsys_controls.a | 155 | 152 | 98.1% | 35242 | 34736 | 98.6% | 154 |
| libchoreoobjects.a | 499 | 498 | 99.8% | 120236 | 113933 | 94.8% | 499 |
| libtier2.a | 71 | 70 | 98.6% | 42366 | 38064 | 89.8% | 71 |
| libtier1.a | 839 | 795 | 94.8% | 144962 | 136833 | 94.4% | 800 |
| libvgui_controls.a | 3211 | 3188 | 99.3% | 734942 | 726054 | 98.8% | 3208 |
| libbitmap.a | 167 | 160 | 95.8% | 95587 | 92094 | 96.3% | 165 |
| libvtf.a | 121 | 111 | 91.7% | 22865 | 20005 | 87.5% | 121 |
| libparticles.a | 1551 | 1547 | 99.7% | 238596 | 233930 | 98.0% | 1550 |
| libdmxloader.a | 226 | 225 | 99.6% | 68779 | 68706 | 99.9% | 225 |
| libmathlib.a | 296 | 293 | 99.0% | 69085 | 68375 | 99.0% | 296 |
| **total** | 23256 | 22839 | 98.2% | 5343366 | 4970356 | 93.0% | 23099 |

Paired with a retail function: 23075 (exact 6506, after masking 16309, differs 260); report: `lostcoast-client.md`.
