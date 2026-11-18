"""Compiler and linker settings of Valve's retail macOS i386 build (depot 223).

Source: Source SDK 2013 singleplayer src/devtools/{base,release}.xcconfig and
vpc_scripts/source_*posix*.vpc, Xcode 6 defaults, and the deployment target
and SDK recorded in the retail binaries (LC_VERSION_MIN 10.6 / sdk 10.9).
Shared by the waf --retail-osx32 mode and the scripts in this directory.
"""
import os

TOOLCHAIN = os.environ.get("BYTEMATCH_TOOLCHAIN",
                           os.path.expanduser("~/.cache/source-bytematch/toolchain"))
CLANG_VERSION = "600.0.57"
LD64_VERSION = "242"
SDK_NAME = "MacOSX10.9.sdk"
MIN_OSX = "10.6"
RETAIL_DEPOT = "/Volumes/Expansion/mac-ports/hl2-anniversary-depot/depots/223/19307283"


def paths(toolchain=None, clang=None, ld64=None):
    t = toolchain or TOOLCHAIN
    return {
        "cc": os.path.join(t, "clang-" + (clang or CLANG_VERSION), "bin", "clang"),
        "cxx": os.path.join(t, "clang-" + (clang or CLANG_VERSION), "bin", "clang++"),
        "ld_dir": os.path.join(t, "ld64-" + (ld64 or LD64_VERSION), "bin") + "/",
        "ld_version": ld64 or LD64_VERSION,
        "sdk": os.path.join(t, "sdk", SDK_NAME),
    }


# GCC_PREPROCESSOR_DEFINITIONS (xcconfig) + source_base/source_posix_base VPC, Release.
DEFINES = [
    "VPC", "NDEBUG", "GNUC", "POSIX", "_POSIX", "COMPILER_GCC", "_DLL_EXT=.dylib",
    "_OSX", "OSX", "_DARWIN_UNLIMITED_SELECT", "FD_SETSIZE=10240", "OVERRIDE_V_DEFINES",
    "FRAME_POINTER_OMISSION_DISABLED",
    "NO_MALLOC_OVERRIDE=1", "VPROF_LEVEL=1", "NO_HOOK_MALLOC=1", "PNG_NO_PEDANTIC_WARNINGS",
    "CPP11_NO_LIBCXX",
    # Not Valve's: excludes this fork's runtime additions from retail builds.
    "BYTEMATCH_RETAIL",
]

# BASE_CFLAGS -U options; they follow every -D on Xcode's command line.
UNDEFS = ["-Usprintf", "-Ustrncpy", "-UPROTECTED_THINGS_ENABLE"]
UNDEFINED_NAMES = ["sprintf", "strncpy", "PROTECTED_THINGS_ENABLE"]


def common_flags(sdk):
    # -funswitch-loops (release.xcconfig) is accepted and ignored by clang-600;
    # objects are byte-identical with and without it. Kept because Valve passed it.
    return [
        "-arch", "i386", "-mmacosx-version-min=" + MIN_OSX, "-isysroot", sdk,
        "-fmessage-length=0", "-fpascal-strings", "-fasm-blocks", "-fstrict-aliasing",
        "-O2", "-g", "-gdwarf-2", "-g2",
        "-ffast-math", "-ftree-vectorize", "-funswitch-loops",
        "-fvisibility=hidden",
        "-Qunused-arguments", "-w",
        # HEADER_SEARCH_PATHS = $(HEADER_SEARCH_PATHS) $(SDKROOT)/usr/include/malloc
        "-I" + os.path.join(sdk, "usr", "include", "malloc"),
    ]


def cflags(sdk):
    return common_flags(sdk) + ["-std=gnu99"]


def cxxflags(sdk):
    return common_flags(sdk) + ["-std=gnu++11", "-stdlib=libstdc++",
                                "-fvisibility-inlines-hidden", "-ftemplate-depth=512"]


def objcflags():
    return ["-fobjc-exceptions"]


def linkflags(p):
    """Driver flags for linking (clang++ driver, as Xcode's Ld step)."""
    return [
        "-arch", "i386", "-mmacosx-version-min=" + MIN_OSX, "-isysroot", p["sdk"],
        "-stdlib=libstdc++",
        "-B", p["ld_dir"], "-mlinker-version=" + p["ld_version"],
        # DEAD_CODE_STRIPPING / PRESERVE_DEAD_CODE_INITS_AND_TERMS
        "-dead_strip", "-Wl,-no_dead_strip_inits_and_terms",
    ]


def dylib_flags(install_name):
    return ["-dynamiclib", "-install_name", install_name,
            "-compatibility_version", "1", "-current_version", "1"]
