#include <Windows.h>
#include <shlwapi.h>
#include <stdlib.h>
#include <string.h>

#include "array_size.h"
#include "float.h"
#include "hooks.h"
#include "paths.h"
#include "stubs.h"

#ifndef Abort
#  define Abort abort
#endif

static struct screen screen = {0};

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
    RECT rect = {0};
    char buffer[MAX_PATH] = {0};
    int current_drive = 0;
    struct paths* paths = NULL;
    int system_drive = 0;
    char drive = '\0';

    if (GetWindowRect(GetDesktopWindow(), &rect) == 0 || rect.right <= 0
        || rect.bottom <= 0 || rect.right > 65535 || rect.bottom > 65535)
    {
      return FALSE;
    }

    {
      int width = (int)rect.right;
      int height = (int)rect.bottom;
      screen.half_width = width / 2;
      screen.half_height = height / 2;
      calculate_growth(&width, &height);
    }

    {
      UINT result = GetModuleFileNameA(hinstDLL, buffer, countof(buffer));
      if (result == 0 || result >= countof(buffer)) {
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
      UINT result = GetWindowsDirectoryA(buffer, countof(buffer));
      if (result == 0 || result >= countof(buffer)) {
        return FALSE;
      }
    }

    system_drive = PathGetDriveNumberA(buffer);
    if (system_drive == -1) {
      return FALSE;
    }

    drive = select_drive(current_drive, system_drive);
    return stubs_ctor() == 0 && hooks_ctor(paths, drive, &screen) == 0  //
        ? TRUE
        : FALSE;
  }

  if (fdwReason == DLL_PROCESS_DETACH && lpvReserved == NULL) {
    if (hooks_dtor() != 0 || stubs_dtor() != 0) {
      Abort();
    }
  }

  return TRUE;
}
