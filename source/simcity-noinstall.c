#include <Windows.h>
#include <stdlib.h>
#include <string.h>

#include "hooks.h"
#include "paths.h"
#include "stubs.h"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
  if (fdwReason == DLL_PROCESS_ATTACH) {
    char buffer[MAX_PATH] = {0};
    UINT result =
        GetModuleFileNameA((HMODULE)hinstDLL, buffer, _countof(buffer));
    if (result == 0) {
      return FALSE;
    }

    struct paths* paths = paths_ctor(buffer);
    if (paths == NULL) {
      return FALSE;
    }

    return stubs_ctor() == 0 && hooks_ctor(paths) == 0 ? TRUE : FALSE;
  }

  if (fdwReason == DLL_PROCESS_DETACH && lpvReserved == NULL) {
    if (hooks_dtor() != 0 || stubs_dtor() != 0) {
      abort();
    }
  }

  return TRUE;
}
