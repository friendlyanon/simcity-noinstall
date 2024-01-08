#include "stubs.h"

#include <Windows.h>
#include <stdlib.h>
#include <string.h>

#include "array_size.h"

void* winmm_handle = NULL;

int stubs_ctor(void)
{
  wchar_t buffer[MAX_PATH] = {0};
  wchar_t const* winmm = NULL;
  UINT dir_length = GetSystemDirectoryW(buffer, countof(buffer));
  if (dir_length == 0 || dir_length >= countof(buffer)) {
    return 1;
  }

  winmm = L"\\winmm.dll";
  if (buffer[min(dir_length - 1, countof(buffer) - 2)] == L'\\') {
    winmm += 1;
  }

  (void)wcsncat(buffer + dir_length, winmm, countof(buffer) - dir_length);

  winmm_handle = LoadLibraryW(buffer);
  if (winmm_handle == NULL) {
    return 1;
  }

  return stubs_load();
}

int stubs_dtor(void)
{
  if (winmm_handle == NULL) {
    return 0;
  }

  return FreeLibrary(winmm_handle) != 0 ? 0 : 1;
}
