# Linux image for the retail Windows toolchain: Debian trixie's wine 10 (old-style
# WoW64, wine32:i386) plus the tools fetch-msvc2013.py needs. On x86_64 Linux it
# runs cl.exe natively. On Apple Silicon (OrbStack/Docker Desktop) the 32-bit
# wine loader falls outside Rosetta and runs under qemu-i386: cl.exe works but
# slowly, and link /DEBUG fails with LNK1101. Use setup-wine.py on macOS instead.
FROM debian:trixie-slim@sha256:a72b4433de1cf7d6bae0c4cee74d0547f58601a643b3375f070c51600d63b6b2
ARG WINE_VERSION=10.0~repack-6
ENV DEBIAN_FRONTEND=noninteractive WINEDEBUG=-all WINEPREFIX=/wine WINEARCH=win32 \
    BYTEMATCH_TOOLCHAIN=/toolchain
RUN dpkg --add-architecture i386 && apt-get update && apt-get install -y --no-install-recommends \
        wine=${WINE_VERSION} wine32:i386=${WINE_VERSION} \
        msitools cabextract 7zip python3 ca-certificates \
    && rm -rf /var/lib/apt/lists/*
RUN wineboot -i >/dev/null 2>&1 || true; wineserver -w || true
WORKDIR /work
