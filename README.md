# simcity-noinstall

Fixes issues with SimCity 2000 Special Edition (Win95) without any patches, to
keep the files in their original state.

# How to use

The project produces `simcity-noinstall.dll`. This will have to be renamed to
`WINMM.DLL` and placed in the game's `SC2K` folder.

Upon launching the game, `simcity-noinstall.ini` will be created, where you can
edit your mayor and company names.

# TODO

Look into the `WM_MDIGETACTIVE` message spam. This causes high CPU usage on the
core where the game is running.
