#include <Windows.h>
#include <shlwapi.h>
#include <stdlib.h>
#include <string.h>

#include "array_size.h"
#include "hooks.h"
#include "paths.h"
#include "stubs.h"

static char select_drive(int current_drive, int system_drive)
{
  char drive = 'A';
  for (;; ++drive) {
    if (drive != ('A' + current_drive) && drive != ('A' + system_drive)) {
      break;
    }
  }
  return drive;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
  if (fdwReason == DLL_PROCESS_ATTACH) {
    char buffer[MAX_PATH] = {0};
    int current_drive = 0;
    struct paths* paths = NULL;
    int system_drive = 0;
    char drive = '\0';
    {
      UINT result =
          GetModuleFileNameA((HMODULE)hinstDLL, buffer, COUNTOF(buffer));
      if (result == 0 || result >= COUNTOF(buffer)) {
        return FALSE;
      }
    }

    current_drive = PathGetDriveNumberA(buffer);
    if (current_drive == -1) {
      return FALSE;
    }

    paths = paths_ctor(buffer);
    if (paths == NULL) {
      return FALSE;
    }

    {
      UINT result = GetWindowsDirectoryA(buffer, COUNTOF(buffer));
      if (result == 0 || result >= COUNTOF(buffer)) {
        return FALSE;
      }
    }

    system_drive = PathGetDriveNumberA(buffer);
    if (system_drive == -1) {
      return FALSE;
    }

    drive = select_drive(current_drive, system_drive);
    return stubs_ctor() == 0 && hooks_ctor(paths, drive) == 0 ? TRUE : FALSE;
  }

  if (fdwReason == DLL_PROCESS_DETACH && lpvReserved == NULL) {
    if (hooks_dtor() != 0 || stubs_dtor() != 0) {
      abort();
    }
  }

  return TRUE;
}
