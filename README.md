# simcity-noinstall

Fixes issues with SimCity 2000 Special Edition (Win95) without any patches, to
keep the files in their original state.

# How to use

To install SimCity 2000 (Win95), copy the `DATA` and `WIN95\SC2K` folders from
the disc to some location where the program can create files. The two folders
should end up next to each other. Please make sure that they aren't too far
from the root of the drive, because there is a 260 character limit.

To use simcity-noinstall, copy `WINMM.DLL` to the `SC2K` folder.

After you start the game, `simcity-noinstall.ini` will be created in the `SC2K`
folder, where you can edit your mayor and company names, among other settings.
This setup allows you to freely relocate the game, because every setting of the
game is stored in the ini.

# Building

Due to the limitations of CMake's VS generator regarding MASM source files, you
have to use the `Ninja` or `NMake` generator to create a build system.

# TODO

Look into the `WM_MDIGETACTIVE` message spam. This causes high CPU usage on the
core where the game is running.
