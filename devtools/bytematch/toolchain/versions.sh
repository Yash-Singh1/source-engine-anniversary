# Pinned public sources for the retail macOS i386 toolchain. Sourced by the
# other scripts in this directory.

: "${BYTEMATCH_TOOLCHAIN:=$HOME/.cache/source-bytematch/toolchain}"
: "${BYTEMATCH_JOBS:=$(sysctl -n hw.ncpu 2>/dev/null || echo 8)}"

# Apple clang from apple-oss-distributions/clang release tags.
# Xcode 6.0.1 = 600.0.51, Xcode 6.1 = 600.0.54, Xcode 6.2 = 600.0.57,
# Xcode 5.1 = 503.0.38 (2014 hl2mp modules).
CLANG_URL_BASE=https://github.com/apple-oss-distributions/clang/archive/refs/tags
clang_sha256() {
	case $1 in
	503.0.38) echo 64c0c3d2b3511937f1d7b4b5b8d3352ef84b2f4198848614c65a09c706681746 ;;
	600.0.51) echo 3a161eff905526d0732960930bd66a04ff7f5e57a50d703039832ce034ca9d34 ;;
	600.0.54) echo 59e74469353a727b0f034606f44af0b5c7da2b0704ac2ad3ffdc75612640ca48 ;;
	600.0.57) echo fdd6a0efa0122f88d22c76aaa7d8dab007ff00d525cf6fbeda05d5e703b08bce ;;
	*) return 1 ;;
	esac
}

# ld64 from tpoechtrager/cctools-port, pinned to branch-head commits.
# 236.3 = Xcode 5.1 (2014 hl2mp modules), 241.9 = Xcode 6.1, 242 = Xcode 6.2.
CCTOOLS_REPO=https://github.com/tpoechtrager/cctools-port.git
ld64_branch() {
	case $1 in
	236.3) echo 855-ld64-236.3 ;;
	241.9) echo 862-ld64-241.9 ;;
	242) echo 870-ld64-242 ;;
	*) return 1 ;;
	esac
}
ld64_commit() {
	case $1 in
	236.3) echo d15e9342b7e541a9dba04d958d6d27d3f0f69cf1 ;;
	241.9) echo c3d16b5c8c8d23b5730b5fb2a731810dd526a65b ;;
	242) echo b90fb3fd8a79d4aa557ee61ce9f4b8c2b0cafa65 ;;
	*) return 1 ;;
	esac
}

DEFAULT_CLANG=600.0.57
DEFAULT_LD64=242
