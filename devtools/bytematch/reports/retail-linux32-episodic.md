# --retail-linux32 vs retail episodic (build 19307283)

Our functions (deduplicated), our sizes; a function counts as matched when its bytes equal a retail function's once relocated fields are masked (bytematch, stripped-ELF pairing). Static libraries: the members the retail macOS module of the same build pulls.

## server.so

| objects | functions | matched | % | bytes | matched | % | located |
|---|---:|---:|---:|---:|---:|---:|---:|
| module | 26688 | 26434 | 99.0% | 8009129 | 7736434 | 96.6% | 26602 |
| libchoreoobjects.a | 502 | 501 | 99.8% | 121009 | 114706 | 94.8% | 502 |
| libtier1.a | 771 | 729 | 94.6% | 135495 | 129258 | 95.4% | 731 |
| libparticles.a | 224 | 223 | 99.6% | 50473 | 50105 | 99.3% | 224 |
| libdmxloader.a | 230 | 230 | 100.0% | 68795 | 68795 | 100.0% | 230 |
| libmathlib.a | 280 | 277 | 98.9% | 63528 | 62818 | 98.9% | 280 |
| libtier3.a | 3 | 3 | 100.0% | 1110 | 1110 | 100.0% | 3 |
| libtier2.a | 21 | 21 | 100.0% | 1901 | 1901 | 100.0% | 21 |
| **total** | 28719 | 28418 | 99.0% | 8451440 | 8165127 | 96.6% | 28593 |

Paired with a retail function: 28541 (exact 5949, after masking 22417, differs 175); report: `episodic-server.md`.
Linux-only library files with no function in retail (not counted): pathmatch.cpp.0.o.

## client.so

| objects | functions | matched | % | bytes | matched | % | located |
|---|---:|---:|---:|---:|---:|---:|---:|
| module | 16430 | 16155 | 98.3% | 3831238 | 3569671 | 93.2% | 16324 |
| libtier3.a | 20 | 20 | 100.0% | 5977 | 5977 | 100.0% | 20 |
| libmatsys_controls.a | 155 | 152 | 98.1% | 35242 | 34736 | 98.6% | 154 |
| libchoreoobjects.a | 498 | 497 | 99.8% | 120101 | 113798 | 94.8% | 498 |
| libtier2.a | 71 | 70 | 98.6% | 42366 | 38064 | 89.8% | 71 |
| libtier1.a | 839 | 796 | 94.9% | 145171 | 137277 | 94.6% | 800 |
| libvgui_controls.a | 3218 | 3198 | 99.4% | 759534 | 750843 | 98.9% | 3217 |
| libbitmap.a | 167 | 160 | 95.8% | 95587 | 92094 | 96.3% | 165 |
| libvtf.a | 121 | 111 | 91.7% | 22865 | 20005 | 87.5% | 121 |
| libparticles.a | 1551 | 1547 | 99.7% | 238596 | 233930 | 98.0% | 1550 |
| libdmxloader.a | 226 | 225 | 99.6% | 68779 | 68706 | 99.9% | 225 |
| libmathlib.a | 296 | 293 | 99.0% | 69085 | 68375 | 99.0% | 296 |
| **total** | 23592 | 23224 | 98.4% | 5434541 | 5133476 | 94.5% | 23441 |

Paired with a retail function: 23420 (exact 6562, after masking 16641, differs 217); report: `episodic-client.md`.
