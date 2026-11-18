"""Pinned inputs for the retail Windows i386 toolchain (depot 222, build 19307283).

MSVC 2013 Update 5 (cl 18.00.40629, link 12.00.40629) with the Update 2
library layer (libcmt objects at build 21005) and the Windows 7.1A SDK, all
read from Microsoft's public Visual Studio Express 2013 for Windows Desktop
with Update 5 ISO. Wine is Gcenx's macOS build of WineHQ wine-stable.
Shared by the scripts in this directory and scripts/waifulib/retail_win32.py.
"""
import os
import shutil
import sys

TOOLCHAIN = os.environ.get("BYTEMATCH_TOOLCHAIN", os.path.expanduser("~/.cache/source-bytematch/toolchain"))
ROOT = os.path.join(TOOLCHAIN, "win32")
DOWNLOADS = os.path.join(ROOT, "downloads")
VS = os.environ.get("BYTEMATCH_MSVC2013", os.path.join(ROOT, "vs2013u5"))
PREFIX = os.path.join(ROOT, "prefix")

ISO_URL = ("https://download.microsoft.com/download/2/E/E/2EEC68E6-7B03-437A-AF06-F8E43F04014D/"
           "vs2013.5_dskexp_ENU.iso")
ISO_SIZE = 3537494016

# ISO path -> (size, sha256). Everything else on the ISO is never read.
ISO_FILES = {
    "/packages/vc_compilerCore86/vc_compilerCore86.msi": (327680, "32c934404cfa4473d877011cd73f3c7f8bf014f442f8de9f0d0c3d0162971996"),
    "/packages/vc_compilerCore86/vc_CompilerCore86.cab": (15375253, "f01f9e27de2292d789924b66ba27e70722a9a933a48f75117a700401e53c5a5b"),
    "/packages/vc_compilerCore86res/vc_compilerCore86res.msi": (159744, "858068bba8fb51a6ce4d4df0b208356f364c284af033c2e6922d177a82c16d74"),
    "/packages/vc_compilerCore86res/vc_CompilerCore86Res.cab": (317699, "f34e48f833d68ba25d58d740e18fcbe317180068bc4b69272fc74bd70403523b"),
    "/packages/vc_librarycore86/vc_librarycore86.msi": (299008, "bb39811fd9a70aee30c144f070e78bd08fed85c9faea246360d5438c52fc1243"),
    "/packages/vc_librarycore86/cab1.cab": (4436397, "294853a66c1b4e211f5eac7548b9fc7d71eac6783411eaaf77de54a62907c4a3"),
    "/packages/vc_librarycore86/cab2.cab": (11924707, "87714b195a2f800a0155a9267ec25ad019124e5bbae790354844bed61806ec7c"),
    "/packages/vc_librarycore86/cab3.cab": (7988422, "4eef373d07b979547436cabd0c078c6aab42794d34598bdc86b9f8eee2137acd"),
    "/packages/vc_librarycore86/cab4.cab": (1505365, "a4b9ccd5279109207fbc7e2207a16cce59d6b6bb9566e016c60e3a0962c10953"),
    "/packages/vc_libraryDesktop/x86/vc_LibraryDesktopX86.msi": (126976, "a6c67dd1af51c6014f332f16679e70a6bffa4248b3c3ba7be1277b5faff8376c"),
    "/packages/vc_libraryDesktop/x86/cab1.cab": (38891276, "4838d2aa10501d4b3cfc74daca86a0302987718dcba777bc207cefaee96ae3c1"),
    "/packages/vc_libraryDesktop/x86/cab2.cab": (8572554, "72edc5f7799039e64cd14a05a06be994a2951500f4fdd98a3fd15717d77ccfb5"),
    "/packages/XPSupport/Win_XPSupport.msi": (483328, "00b4d7e2c384d57fef6264336eabc4d93c5523c1ce7987207263b3e85cc03932"),
    "/packages/XPSupport/cab1.cab": (8752406, "b27cd4cf11b7a95fbaac604d5524f1adf5e6ebd65f1aa3c125e2bc38dd2a74da"),
    "/packages/XPSupport/cab2.cab": (4819954, "d5d7c33e1c55a0dfe36c530cdad670a73f507b7b60b5c83c45af4c1b4c91241f"),
    "/packages/XPSupport/cab3.cab": (5794594, "4e8f8e8af4b012bb530668dd55837b217f3c8011c9d6cbec13e18ada0418ae37"),
    "/packages/XPSupport/cab4.cab": (4130192, "cd167d5bb21d01e2fad461180aa9d756d1ba2e4e690552d51a54978bff217599"),
    # Update 5 patches: full replacement files inside QFE.cab.
    "/packages/kb2829760vc/kb2829760vc.msp": (50331648, "b5257a52bf45f3984470d540aa79056eb06ed358434d55a5d6503df4016e5e29"),
    "/packages/kb2829760/kb2829760v2.msp": (231174144, "d9d2fe1e375966805a99420972047944be2f579dcdaa4c19d933fa5ce91c735b"),
}

# MSI package dir -> subtree of its extracted payload -> destination under VS.
MSI_LAYOUT = [
    ("vc_compilerCore86", "Program Files/Microsoft Visual Studio 12.0", ""),
    ("vc_compilerCore86res", "Program Files/Microsoft Visual Studio 12.0", ""),
    ("vc_librarycore86", "Program Files/Microsoft Visual Studio 12.0", ""),
    ("vc_libraryDesktop/x86", "Program Files/Microsoft Visual Studio 12.0", ""),
    ("XPSupport", "Program Files/Microsoft SDKs/Windows/v7.1A", "sdk71a"),
]

# kb2829760vc.msp (compiler product {678800C0...}) QFE.cab member -> VC/bin file.
COMPILER_QFE = [
    ("WinC_common_x86_cl.exe_F", "cl.exe"),
    ("WinC_common_x86_c2.dll_F", "c2.dll"),
    ("WinC_compiler_x86_c1.dll_F", "c1.dll"),
    ("WinC_compiler_x86_c1ast.dll_F", "c1ast.dll"),
    ("WinC_compiler_x86_c1xx.dll_F", "c1xx.dll"),
    ("WinC_compiler_x86_c1xxast.dll_F", "c1xxast.dll"),
    ("WinC_compiler_x86_dpcmi.dll_F", "dpcmi.dll"),
    ("WinC_compiler_x86_link.exe_F", "link.exe"),
    ("WinC_compiler_x86_ml.exe_F", "ml.exe"),
    ("WinC_compiler_x86_mspdb120.dll_F", "mspdb120.dll"),
    ("WinC_compiler_x86_mspdbcore.dll_F", "mspdbcore.dll"),
    ("WinC_compiler_x86_mspdbsrv.exe_F", "mspdbsrv.exe"),
    ("WinC_compiler_x86_vcmeta.dll_F", "vcmeta.dll"),
    ("FL_msobj120_dll_x86", "msobj120.dll"),
    ("FL_mspdbst_dll_x86", "mspdbst.dll"),
]

# kb2829760v2.msp (library products) QFE.cab member -> path under VC.
LIBRARY_QFE = [(f"WinC_x86_{n}.{e}_F", f"lib/{n}.{e}")
               for n in ("libcmt", "libcmtd", "libcpmt", "libcpmt1", "libcpmtd", "libcpmtd0", "libcpmtd1")
               for e in ("lib", "pdb")] + [
    ("FL_msvcrt_lib_11021_x86_ln.3643236F_FC70_11D3_A536_0090278A1BB8", "lib/msvcrt.lib"),
    ("FL_msvcprt_lib_13701_x86_ln.3643236F_FC70_11D3_A536_0090278A1BB8", "lib/msvcprt.lib"),
    ("FL_delayimp_lib_4757_x86_ln.3643236F_FC70_11D3_A536_0090278A1BB8", "lib/delayimp.lib"),
    ("F_HDR_CRTVERSION_H", "include/crtversion.h"),
    ("FL_Headers_ppltasks_h", "include/ppltasks.h"),
    ("FL_Headers_amprt_h", "include/amprt.h"),
]

# The tools' own CRT. Running them on Microsoft's msvcr120 rather than wine's
# builtin keeps host-side behaviour (float literal parsing, sorting) native.
NATIVE_RUNTIME = ["msvcr120.dll", "msvcp120.dll"]

# sha256 of the assembled tree's key files (cl/c1/c1xx/c2 18.00.40629, link/ml/mspdb 12.00.40629,
# libcmt/libcpmt at build 21005).
EXPECTED = {
    "VC/bin/cl.exe": "2f7fea06e2d024a28dd271620ab469e0641e9758a99d5e20e1adad87cb440039",
    "VC/bin/c1.dll": "dec8e41efc221fd57b2c88e18fdbf0cffb5bba48c64bda23da78408a977ea346",
    "VC/bin/c1xx.dll": "909f0f7467f67c989dc713c93bf437250ddf21f5e7155d3229a30d27d07202b1",
    "VC/bin/c2.dll": "b36b20760aca86528458ef8efea8639d5df32f50085702cb0baa9005381c40d9",
    "VC/bin/link.exe": "b1f7252bbc9b0aad1892f8d5cfa2776f8ecc9f29008489a20f3c3d7f7aa6d874",
    "VC/bin/lib.exe": "f8aa5ed7ada8f69d2883f65e78c1bd47904c6d01e3176f9af3416355e82ad8e5",
    "VC/bin/ml.exe": "e1128d2619d924ced437048dd8ddbd07c3160b60fc12dbf9c0de10ef364a62cf",
    "VC/bin/mspdb120.dll": "9e35d5b27d9015639fc9368c097b80e603214aa778254ee745a43152c2c90de7",
    "VC/bin/mspdbcore.dll": "ca0f0148951a549b03670e7af58233a01144e903c521934d293ba08fc3695cea",
    "VC/bin/mspdbsrv.exe": "3468824afc9c7bf0a3f94c600acab40d7702c765b669bc12fdd5c0bebc63031b",
    "VC/bin/msobj120.dll": "c84abb5adc15a1587cf55f3ddb9649695dfb70deee90bd8d0216382650abb1a1",
    "VC/bin/cvtres.exe": "34524ce21c8290f24a8a7699a04c32a10c3664d8ac3d58f4f4d754e864c7d72f",
    "VC/lib/libcmt.lib": "f24c62d0151d61375bae2b8d40837b737498f6fd2e966c56064c566f8035fe16",
    "VC/lib/libcpmt.lib": "ac96811a4744c9b31c56996d8b556d020a1dc524ceacc35c1a681ab3abb281e7",
    "VC/lib/oldnames.lib": "438596a40b60282bb285d0d5bf088dba9462f726cd6c8d84977ccf4ad1ec1462",
    "VC/include/crtversion.h": "30b791f6162782c9ac2fd33b9b3058da8f5d41a6b174f9ff24767421ffc0deda",
    "VC/include/xutility": "7843fe4ba8afcfbc98c5c3f1223fb5ffe5a63091441ee5c6db9dc1a5ff304ca3",
    "sdk71a/Lib/kernel32.Lib": "5c76cde0da4a277962316070b966e0829d35df93d5b54d79d81f964e56cd4819",
    "sdk71a/Include/Windows.h": "5c4970263724e56c13ab0f28d1cf49bec1cb0f29c32cb5a00069c5db73438ae3",
    "sdk71a/Bin/RC.Exe": "b72a640265ca5f0d8cad043e5661081aee9bd4aecaf8256aed1dd69742d44222",
}

# MSXML 6.0 SP2 (KB2957482). The 7.1A SDK's mc.exe validates ETW manifests
# through MSXML schemas, which wine's builtin msxml6 only stubs.
MSXML6_URL = ("https://download.microsoft.com/download/2/7/7/277681BE-4048-4A58-ABBA-259C465B1699/"
              "msxml6-KB2957482-enu-amd64.exe")
MSXML6_SHA256 = "260cd870851ffc3c6d10b71691f134e20d8d03ac26073bb36951eacb7aa85897"
MSXML6_X86_FILES = {"msxml6.dll.86F857F6_A743_463D_B2FE_98CB5F727E09": "msxml6.dll",
                    "msxml6r.dll.86F857F6_A743_463D_B2FE_98CB5F727E09": "msxml6r.dll"}

# Gcenx/macOS_Wine_builds: WineHQ wine-stable for macOS, WoW64 (32-bit PE code
# in a 64-bit process, which macOS Rosetta 2 can run).
WINE_VERSION = "11.0_1"
WINE_URL = ("https://github.com/Gcenx/macOS_Wine_builds/releases/download/11.0_1/"
            "wine-stable-11.0_1-osx64.tar.xz")
WINE_SHA256 = "b50dc50ec7f41d58b115a6b685d4d1315ba3c797bd3aa0f49213f2703cb82388"
WINE_DIR = os.path.join(ROOT, "wine-stable-" + WINE_VERSION)
WINE_BIN = os.path.join(WINE_DIR, "Wine Stable.app", "Contents", "Resources", "wine", "bin")
if sys.platform != "darwin":
    # Linux hosts and docker/: the distribution's wine (old-style WoW64 with wine32).
    WINE_BIN = os.path.dirname(shutil.which("wine") or "/usr/bin/wine")
    PREFIX = os.environ.get("WINEPREFIX", PREFIX)
