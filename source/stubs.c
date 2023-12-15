#include "stubs.h"

#include <Windows.h>
#include <stdlib.h>
#include <string.h>

void* winmm_handle = NULL;

int stubs_ctor()
{
  wchar_t buffer[MAX_PATH] = {0};
  UINT dir_length = GetSystemDirectoryW(buffer, _countof(buffer));
  if (dir_length == 0 || dir_length >= _countof(buffer)) {
    return 1;
  }

  wchar_t const* winmm = L"\\winmm.dll";
  if (buffer[min(dir_length - 1, _countof(buffer) - 2)] == L'\\') {
    winmm += 1;
  }

  (void)wcsncat(buffer + dir_length, winmm, _countof(buffer) - dir_length);

  winmm_handle = LoadLibraryW(buffer);
  if (winmm_handle == NULL) {
    return 1;
  }

  return stubs_load();
}

int stubs_dtor()
{
  if (winmm_handle == NULL) {
    return 0;
  }

  return FreeLibrary(winmm_handle) != 0 ? 0 : 1;
}
