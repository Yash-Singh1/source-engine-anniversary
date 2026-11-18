# hl2_osx (launcher_main)

Retail: depot 223 `hl2_osx`, one function. Matches after this lane.

| function | status | note |
|---|---|---|
| main | edited | the leak's form: `dlopen("bin/launcher.dylib")`, "Failed to load the launcher" via `fprintf`, tail call into `LauncherMain`. The fork's `LD_LIBRARY_PATH`/`execve` re-exec and `liblauncher` fallback stay under `#ifndef BYTEMATCH_RETAIL` |
