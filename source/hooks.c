#include "hooks.h"

#include <Windows.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "MinHook.h"
#include "array_size.h"
#include "ini.h"
#include "reg_keys.h"
#include "tables/dword_tbl.h"
#include "tables/paths_tbl.h"
#include "tables/reg_tbl.h"
#include "tables/section_tbl.h"
#include "tables/string_tbl.h"

static char fake_path[] = "A:\\DATA\\";

static BOOL(WINAPI* GetVolumeInformationA_ptr)(
    LPCSTR, LPSTR, DWORD, LPDWORD, LPDWORD, LPDWORD, LPSTR, DWORD) = NULL;

static BOOL WINAPI GetVolumeInformationA_fn(  //
    LPCSTR lpRootPathName,
    LPSTR lpVolumeNameBuffer,
    DWORD nVolumeNameSize,
    LPDWORD lpVolumeSerialNumber,
    LPDWORD lpMaximumComponentLength,
    LPDWORD lpFileSystemFlags,
    LPSTR lpFileSystemNameBuffer,
    DWORD nFileSystemNameSize)
{
  size_t length = strlen(lpRootPathName);
  if (length == 3 && memcmp(fake_path, lpRootPathName, 3) == 0) {
    return TRUE;
  }

  return GetVolumeInformationA_ptr(  //
      lpRootPathName,
      lpVolumeNameBuffer,
      nVolumeNameSize,
      lpVolumeSerialNumber,
      lpMaximumComponentLength,
      lpFileSystemFlags,
      lpFileSystemNameBuffer,
      nFileSystemNameSize);
}

static struct paths const* paths = NULL;

static char const* rewrite_path(char* buffer, char const* path)
{
  size_t length = strlen(path);
  size_t fake_length = lengthof(fake_path);
  if (length < fake_length || memcmp(fake_path, path, fake_length) != 0) {
    return path;
  }

  {
    size_t movies_length = strlen(paths->movies);
    (void)memcpy(buffer, paths->movies, movies_length);
    (void)memcpy(buffer + movies_length,
                 path + fake_length,
                 min(MAX_PATH - movies_length, length - fake_length));
  }

  return buffer;
}

static HANDLE(WINAPI* FindFirstFileA_ptr)(LPCSTR, LPWIN32_FIND_DATAA) = NULL;

static HANDLE WINAPI FindFirstFileA_fn(  //
    LPCSTR lpFileName,
    LPWIN32_FIND_DATAA lpFindFileData)
{
  char buffer[MAX_PATH] = {0};
  return FindFirstFileA_ptr(rewrite_path(buffer, lpFileName), lpFindFileData);
}

static HANDLE(WINAPI* CreateFileA_ptr)(
    LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE) = NULL;

static HANDLE WINAPI CreateFileA_fn(  //
    LPCSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile)
{
  char buffer[MAX_PATH] = {0};
  return CreateFileA_ptr(  //
      rewrite_path(buffer, lpFileName),
      dwDesiredAccess,
      dwShareMode,
      lpSecurityAttributes,
      dwCreationDisposition,
      dwFlagsAndAttributes,
      hTemplateFile);
}

typedef BOOL(APIENTRY* dialog_fn)(LPOPENFILENAMEA);

static BOOL dialog_fix(LPOPENFILENAMEA options, dialog_fn original)
{
  options->Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
  return original(options);
}

static dialog_fn GetOpenFileNameA_ptr = NULL;

static BOOL APIENTRY GetOpenFileNameA_fn(LPOPENFILENAMEA options)
{
  return dialog_fix(options, GetOpenFileNameA_ptr);
}

static dialog_fn GetSaveFileNameA_ptr = NULL;

static BOOL APIENTRY GetSaveFileNameA_fn(LPOPENFILENAMEA options)
{
  return dialog_fix(options, GetSaveFileNameA_ptr);
}

static LSTATUS(APIENTRY* RegCreateKeyExA_ptr)(  //
    HKEY,
    LPCSTR,
    DWORD,
    LPSTR,
    DWORD,
    REGSAM,
    CONST LPSECURITY_ATTRIBUTES,
    PHKEY,
    LPDWORD) = NULL;

static LSTATUS APIENTRY RegCreateKeyExA_fn(  //
    HKEY hKey,
    LPCSTR lpSubKey,
    DWORD Reserved,
    LPSTR lpClass,
    DWORD dwOptions,
    REGSAM samDesired,
    CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    PHKEY phkResult,
    LPDWORD lpdwDisposition)
{
  struct reg_entry const* entry = reg_lookup(lpSubKey, strlen(lpSubKey));
  if (entry != NULL) {
    *phkResult = (HKEY)(KEY_BASE + entry->value);
    return ERROR_SUCCESS;
  }

  return RegCreateKeyExA_ptr(  //
      hKey,
      lpSubKey,
      Reserved,
      lpClass,
      dwOptions,
      samDesired,
      lpSecurityAttributes,
      phkResult,
      lpdwDisposition);
}

static LSTATUS(APIENTRY* RegCloseKey_ptr)(HKEY) = NULL;

static LSTATUS APIENTRY RegCloseKey_fn(HKEY hKey)
{
  ULONG_PTR value = (ULONG_PTR)hKey;
  if (IS_FAKE_KEY(value)) {
    return ERROR_SUCCESS;
  }

  return RegCloseKey_ptr(hKey);
}

static LSTATUS(APIENTRY* RegQueryValueExA_ptr)(
    HKEY, LPCSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD) = NULL;

static LSTATUS(APIENTRY* RegSetValueExA_ptr)(
    HKEY, LPCSTR, DWORD, DWORD, CONST BYTE*, DWORD) = NULL;

static LSTATUS APIENTRY RegSetValueExA_fn(  //
    HKEY hKey,
    LPCSTR lpValueName,
    DWORD Reserved,
    DWORD dwType,
    CONST BYTE* lpData,
    DWORD cbData)
{
  if (dwType == REG_DWORD || (dwType == REG_BINARY && cbData == sizeof(DWORD)))
  {
    struct section_entry const* entry = NULL;
    SECTION_LOOKUP(entry, hKey);
    if (entry != NULL) {
      set_ini_dword(
          paths->ini, entry->value, lpValueName, *(CONST DWORD*)lpData);
      return ERROR_SUCCESS;
    }
  }

  return RegSetValueExA_ptr(
      hKey, lpValueName, Reserved, dwType, lpData, cbData);
}

static void out_string(char const* value, LPBYTE lpData, LPDWORD lpcbData)
{
  if (lpData == NULL) {
    *lpcbData = (DWORD)strlen(value) + 1;
  } else {
    (void)memcpy(lpData, value, *lpcbData);
  }
}

static void out_dword(DWORD value, LPBYTE lpData, LPDWORD lpcbData)
{
  if (lpData == NULL) {
    *lpcbData = (DWORD)sizeof(DWORD);
  } else {
    (void)memcpy(lpData, &value, sizeof(DWORD));
  }
}

#define MESSAGE_PREFIX "Unknown value name: "
static char unknown_value_message[513] = MESSAGE_PREFIX;

static LSTATUS APIENTRY RegQueryValueExA_fn(  //
    HKEY hKey,
    LPCSTR lpValueName,
    LPDWORD lpReserved,
    LPDWORD lpType,
    LPBYTE lpData,
    LPDWORD lpcbData)
{
  if (IS_KEY(hKey, PATHS_KEY)) {
    if (strcmp("GOODIES", lpValueName) == 0) {
      char drive[] = "A:\\";
      drive[0] = fake_path[0];
      out_string(drive, lpData, lpcbData);
    } else {
      size_t offset = paths_lookup(lpValueName, strlen(lpValueName))->value;
      out_string((char const*)paths + offset, lpData, lpcbData);
    }
    return ERROR_SUCCESS;
  }

  if (IS_KEY(hKey, VERSION_KEY) && strcmp("SimCity 2000", lpValueName) == 0) {
    out_dword(get_ini_dword(paths->ini, "VERSION", lpValueName, 256),
              lpData,
              lpcbData);
    return ERROR_SUCCESS;
  }

  if (IS_KEY(hKey, WINDOWS_KEY) && strcmp("SimCity 2000", lpValueName) == 0) {
    out_string(get_ini_string(paths->ini, "Windows", lpValueName, "8 1"),
               lpData,
               lpcbData);
    return ERROR_SUCCESS;
  }

  {
    struct section_entry const* section_entry = NULL;
    SECTION_LOOKUP(section_entry, hKey);
    if (section_entry != NULL) {
      char const* section = section_entry->value;
      size_t value_length = strlen(lpValueName);
      struct string_entry const* string_entry =
          string_lookup(lpValueName, value_length);
      if (string_entry != NULL) {
        out_string(get_ini_string(
                       paths->ini, section, lpValueName, string_entry->value),
                   lpData,
                   lpcbData);
      } else {
        struct dword_entry const* dword_entry =
            dword_lookup(lpValueName, value_length);
        if (dword_entry != NULL) {
          out_dword(get_ini_dword(
                        paths->ini, section, lpValueName, dword_entry->value),
                    lpData,
                    lpcbData);
        } else {
          size_t value_len = strlen(lpValueName);
          size_t prefix_len = lengthof(MESSAGE_PREFIX);
          size_t remaining = lengthof(unknown_value_message) - prefix_len;
          (void)memcpy(unknown_value_message + prefix_len,
                       lpValueName,
                       min(value_len + 1, remaining));
          (void)MessageBoxA(NULL,
                            unknown_value_message,
                            "Unknown value name",
                            MB_ICONERROR | MB_OK);
          out_dword(0, lpData, lpcbData);
        }
      }

      return ERROR_SUCCESS;
    }
  }

  return RegQueryValueExA_ptr(
      hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}

static HWND main_window = NULL;

static HMENU(WINAPI* GetMenu_ptr)(HWND) = NULL;

#define TITLE "SimCity 2000"

static HMENU WINAPI GetMenu_fn(HWND hWnd)
{
  if (main_window == NULL) {
    char buffer[countof(TITLE)];
    if (GetWindowTextA(hWnd, buffer, countof(TITLE)) == lengthof(TITLE)
        && memcmp(TITLE, buffer, countof(TITLE)) == 0)
    {
      main_window = hWnd;
    }
  }

  return GetMenu_ptr(hWnd);
}

static int(WINAPI* ReleaseDC_ptr)(HWND, HDC) = NULL;

static int WINAPI ReleaseDC_fn(HWND hWnd, HDC hDC)
{
  if (main_window != NULL && main_window == hWnd) {
    (void)RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN);
  }

  return ReleaseDC_ptr(hWnd, hDC);
}

int WINAPI hook(LPCWSTR pszModule,
                LPCSTR pszProcName,
                FARPROC pDetour,
                FARPROC* ppOriginal);

int hooks_ctor(struct paths const* paths_, char drive_)
{
  paths = paths_;
  fake_path[0] = drive_;

  if (MH_Initialize() != MH_OK) {
    return 1;
  }

  return hook(L"advapi32.dll",
              "RegQueryValueExA",
              (FARPROC)RegQueryValueExA_fn,
              (FARPROC*)&RegQueryValueExA_ptr)
      || hook(L"advapi32.dll",
              "RegSetValueExA",
              (FARPROC)RegSetValueExA_fn,
              (FARPROC*)&RegSetValueExA_ptr)
      || hook(L"advapi32.dll",
              "RegCreateKeyExA",
              (FARPROC)RegCreateKeyExA_fn,
              (FARPROC*)&RegCreateKeyExA_ptr)
      || hook(L"advapi32.dll",
              "RegCloseKey",
              (FARPROC)RegCloseKey_fn,
              (FARPROC*)&RegCloseKey_ptr)
      || hook(L"comdlg32.dll",
              "GetOpenFileNameA",
              (FARPROC)GetOpenFileNameA_fn,
              (FARPROC*)&GetOpenFileNameA_ptr)
      || hook(L"comdlg32.dll",
              "GetSaveFileNameA",
              (FARPROC)GetSaveFileNameA_fn,
              (FARPROC*)&GetSaveFileNameA_ptr)
      || hook(L"kernel32.dll",
              "GetVolumeInformationA",
              (FARPROC)GetVolumeInformationA_fn,
              (FARPROC*)&GetVolumeInformationA_ptr)
      || hook(L"kernel32.dll",
              "FindFirstFileA",
              (FARPROC)FindFirstFileA_fn,
              (FARPROC*)&FindFirstFileA_ptr)
      || hook(L"kernel32.dll",
              "CreateFileA",
              (FARPROC)CreateFileA_fn,
              (FARPROC*)&CreateFileA_ptr)
      || hook(L"user32.dll",
              "GetMenu",
              (FARPROC)GetMenu_fn,
              (FARPROC*)&GetMenu_ptr)
      || hook(L"user32.dll",
              "ReleaseDC",
              (FARPROC)ReleaseDC_fn,
              (FARPROC*)&ReleaseDC_ptr);
}

int hooks_dtor(void)
{
  MH_STATUS result = MH_Uninitialize();
  if (result == MH_OK || result == MH_ERROR_NOT_INITIALIZED) {
    return 0;
  }

  return 1;
}
