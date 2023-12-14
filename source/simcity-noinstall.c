#include <Windows.h>
#include <shlwapi.h>
#include <stdlib.h>
#include <string.h>

#include "hooks.h"
#include "paths.h"
#include "stubs.h"

static char select_drive(char current_drive, char system_drive)
{
  for (size_t i = 0; i != 26; ++i) {
    char drive = (char)('A' + i);
    if (drive != current_drive && drive != system_drive) {
      return drive;
    }
  }
  __assume(0);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
  if (fdwReason == DLL_PROCESS_ATTACH) {
    char buffer[MAX_PATH] = {0};
    UINT result =
        GetModuleFileNameA((HMODULE)hinstDLL, buffer, _countof(buffer));
    if (result == 0) {
      return FALSE;
    }

    int current_drive = PathGetDriveNumberA(buffer);
    if (current_drive == -1) {
      return FALSE;
    }

    struct paths* paths = paths_ctor(buffer);
    if (paths == NULL) {
      return FALSE;
    }

    if (GetWindowsDirectoryA(buffer, _countof(buffer)) == 0) {
      return FALSE;
    }

    int system_drive = PathGetDriveNumberA(buffer);
    if (system_drive == -1) {
      return FALSE;
    }

    char selected_drive =
        select_drive((char)('A' + current_drive), (char)('A' + system_drive));
    return stubs_ctor() == 0 && hooks_ctor(paths, selected_drive) == 0 ? TRUE
                                                                       : FALSE;
  }

  if (fdwReason == DLL_PROCESS_DETACH && lpvReserved == NULL) {
    if (hooks_dtor() != 0 || stubs_dtor() != 0) {
      abort();
    }
  }

  return TRUE;
}
