# Pinned public sources for the retail macOS i386 toolchain. Sourced by the
# other scripts in this directory.

: "${BYTEMATCH_TOOLCHAIN:=$HOME/.cache/source-bytematch/toolchain}"
: "${BYTEMATCH_JOBS:=$(sysctl -n hw.ncpu 2>/dev/null || echo 8)}"

# Apple clang from apple-oss-distributions/clang release tags.
# Xcode 6.0.1 = 600.0.51, Xcode 6.1 = 600.0.54, Xcode 6.2 = 600.0.57,
# Xcode 5.1 = 503.0.38 (2014 hl2mp modules).
# Xcode 4.3 = 318.0.45 (libjpeg in build-thirdparty.py).
CLANG_URL_BASE=https://github.com/apple-oss-distributions/clang/archive/refs/tags
clang_sha256() {
	case $1 in
	318.0.45) echo a9698a034b8eaa4a531658b4a9246df7d210cd67bb604654c24e0502c2d0bb37 ;;
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

# Apple gcc from apple-oss-distributions/{gcc,gcc_42} release tags, for the
# period third-party archives (build-thirdparty.py). Xcode 3.1 = gcc_42-5564
# (4.2.1) and gcc-5484 (4.0.1), Xcode 3.2 = gcc-5646, Xcode 3.2.6/4.0/4.1 =
# gcc-5666.3; 5646.1, 5659 and 5664 are the Xcode 3.2.x builds in between, kept
# because retail's Crypto++ matches every 4.2.1 build from 5646 to 5666.3.
GCC_URL_BASE=https://github.com/apple-oss-distributions
gcc_repo() {
	case $1 in
	5666.3|5664|5659|5646.1|5646|5484) echo gcc-$1 ;;
	5564) echo gcc_42-$1 ;;
	*) return 1 ;;
	esac
}
gcc_sha256() {
	case $1 in
	5666.3) echo 2e9889ce0136f5a33298cf7cce5247d31a5fb1856e6f301423bde4a81a5e7ea6 ;;
	5664) echo 72df8643eeae1269d4bea5e730e522c34edf77a15edd444271d33e2d41c4620b ;;
	5659) echo f13a6971c5beb56bbec9be9e7c380b40a03b7b6ce6dd1bec8b21990c0fd97699 ;;
	5646.1) echo 8e3f2bf24e3f2739f980bfcff5e666dad726ba12994d89cbf40743626018dba8 ;;
	5646) echo 743d2f5f2eeb9ae2918acf63bf1d9d77dc859f0d9abb828ca7e49fad1e87e15d ;;
	5564) echo 9c4ba58fa75f8fd30baf555bcd75b59a4b595ac080759d7f3f312295243c97f8 ;;
	5484) echo 72f9fdf16e1723b22b580a79766e07244fe104da84707a294380b4e1a32539b6 ;;
	*) return 1 ;;
	esac
}
gcc_target() {
	case $1 in
	5666.3|5664|5659|5646.1|5646) echo i686-apple-darwin10 ;;
	*) echo i686-apple-darwin9 ;;
	esac
}
# The gcc builds drive cctools-port's i386 assembler from the ld64 pin below.
GCC_AS_LD64=242

DEFAULT_CLANG=600.0.57
DEFAULT_LD64=242
