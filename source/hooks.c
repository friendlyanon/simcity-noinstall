#include "hooks.h"

#include <Windows.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "MinHook.h"
#include "ini.h"
#include "stb_ds.h"

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

#define MAXIS_KEY ((HKEY)(ULONG_PTR)(0x80000040L))
#define SC2K_KEY ((HKEY)(ULONG_PTR)(0x80000041L))
#define PATHS_KEY ((HKEY)(ULONG_PTR)(0x80000042L))
#define WINDOWS_KEY ((HKEY)(ULONG_PTR)(0x80000043L))
#define VERSION_KEY ((HKEY)(ULONG_PTR)(0x80000044L))
#define OPTIONS_KEY ((HKEY)(ULONG_PTR)(0x80000045L))
#define LOCALIZE_KEY ((HKEY)(ULONG_PTR)(0x80000046L))
#define REGISTRATION_KEY ((HKEY)(ULONG_PTR)(0x80000047L))
#define SCURK_KEY ((HKEY)(ULONG_PTR)(0x80000048L))

struct reg_entry
{
  char const* key;
  HKEY value;
};

struct reg_entry* reg_map = NULL;

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
  int index = stbds_shgeti(reg_map, lpSubKey);
  if (index != -1) {
    *phkResult = reg_map[index].value;
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
  if ((ULONG_PTR)MAXIS_KEY <= value && value <= (ULONG_PTR)SCURK_KEY) {
    return ERROR_SUCCESS;
  }

  return RegCloseKey_ptr(hKey);
}

static LSTATUS(APIENTRY* RegQueryValueExA_ptr)(
    HKEY, LPCSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD) = NULL;

static struct paths* paths = NULL;

struct paths_entry
{
  char const* key;
  size_t value;
};

static struct paths_entry* paths_map = NULL;

struct section_entry
{
  HKEY key;
  char const* value;
};

static struct section_entry* section_map = NULL;

struct string_entry
{
  char const* key;
  char const* value;
};

static struct string_entry* string_map = NULL;

struct dword_entry
{
  char const* key;
  DWORD value;
};

static struct dword_entry* dword_map = NULL;

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
  if (dwType == REG_DWORD || dwType == REG_BINARY && cbData == sizeof(DWORD)) {
    int i = stbds_hmgeti(section_map, hKey);
    if (i != -1) {
      set_ini_dword(
          paths->ini, section_map[i].value, lpValueName, *(DWORD*)lpData);
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
  if (hKey == PATHS_KEY) {
    size_t offset = stbds_shget(paths_map, lpValueName);
    out_string((char const*)paths + offset, lpData, lpcbData);
    return ERROR_SUCCESS;
  }

  if (hKey == VERSION_KEY && strcmp("SimCity 2000", lpValueName) == 0) {
    out_dword(get_ini_dword(paths->ini, "VERSION", lpValueName, 256),
              lpData,
              lpcbData);
    return ERROR_SUCCESS;
  }

  if (hKey == WINDOWS_KEY && strcmp("SimCity 2000", lpValueName) == 0) {
    out_string(get_ini_string(paths->ini, "Windows", lpValueName, "8 1"),
               lpData,
               lpcbData);
    return ERROR_SUCCESS;
  }

  {
    int i = stbds_hmgeti(section_map, hKey);
    if (i != -1) {
      char const* section = section_map[i].value;
      int j = stbds_shgeti(string_map, lpValueName);
      if (j != -1) {
        out_string(get_ini_string(
                       paths->ini, section, lpValueName, string_map[j].value),
                   lpData,
                   lpcbData);
      } else {
        int k = stbds_shgeti(dword_map, lpValueName);
        if (k != -1) {
          out_dword(get_ini_dword(
                        paths->ini, section, lpValueName, dword_map[k].value),
                    lpData,
                    lpcbData);
        } else {
          size_t value_len = strlen(lpValueName);
          size_t prefix_len = sizeof(MESSAGE_PREFIX) - 1;
          size_t remaining = sizeof(unknown_value_message) - prefix_len - 1;
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

static int hook(LPCWSTR pszModule,
                LPCSTR pszProcName,
                LPVOID pDetour,
                LPVOID* ppOriginal)
{
  LPVOID target = NULL;
  if (MH_CreateHookApiEx(pszModule, pszProcName, pDetour, ppOriginal, &target)
      != MH_OK)
  {
    return 1;
  }

  if (MH_EnableHook(target) != MH_OK) {
    return 1;
  }

  return 0;
}

static void section_put(HKEY key, char const* value)
{
  stbds_hmput(section_map, key, value);
}

int hooks_ctor(struct paths* paths_)
{
  paths = paths_;
  stbds_sh_new_arena(paths_map);
  stbds_shput(paths_map, "DATA", offsetof(struct paths, data));
  stbds_shput(paths_map, "GRAPHICS", offsetof(struct paths, graphics));
  stbds_shput(paths_map, "Music", offsetof(struct paths, sounds));
  stbds_shput(paths_map, "MUSIC", offsetof(struct paths, sounds));
  stbds_shput(paths_map, "SOUND", offsetof(struct paths, sounds));
  stbds_shput(paths_map, "SCENARIOS", offsetof(struct paths, scenarios));
  stbds_shput(paths_map, "HOME", offsetof(struct paths, home));
  stbds_shput(paths_map, "CITIES", offsetof(struct paths, cities));
  stbds_shput(paths_map, "Cities", offsetof(struct paths, cities));
  stbds_shput(paths_map, "SAVEGAME", offsetof(struct paths, cities));
  stbds_shput(paths_map, "TILESETS", offsetof(struct paths, tilesets));
  stbds_shput(paths_map, "TileSets", offsetof(struct paths, tilesets));
  stbds_shput(paths_map, "HOME", offsetof(struct paths, home));
  stbds_shput(paths_map, "GOODIES", offsetof(struct paths, goodies));

  stbds_sh_new_arena(reg_map);
  stbds_shput(reg_map, "Maxis", MAXIS_KEY);
  stbds_shput(reg_map, "SimCity 2000", SC2K_KEY);
  stbds_shput(reg_map, "PATHS", PATHS_KEY);
  stbds_shput(reg_map, "Paths", PATHS_KEY);
  stbds_shput(reg_map, "Windows", WINDOWS_KEY);
  stbds_shput(reg_map, "VERSION", VERSION_KEY);
  stbds_shput(reg_map, "Version", VERSION_KEY);
  stbds_shput(reg_map, "OPTIONS", OPTIONS_KEY);
  stbds_shput(reg_map, "LOCALIZE", LOCALIZE_KEY);
  stbds_shput(reg_map, "Localize", LOCALIZE_KEY);
  stbds_shput(reg_map, "REGISTRATION", REGISTRATION_KEY);
  stbds_shput(reg_map, "SCURK", SCURK_KEY);

  section_put(WINDOWS_KEY, "Windows");
  section_put(VERSION_KEY, "VERSION");
  section_put(OPTIONS_KEY, "OPTIONS");
  section_put(LOCALIZE_KEY, "LOCALIZE");
  section_put(REGISTRATION_KEY, "REGISTRATION");
  section_put(SCURK_KEY, "SCURK");

  stbds_sh_new_arena(string_map);
  stbds_shput(string_map, "Mayor Name", "MissingMayor");
  stbds_shput(string_map, "MAYOR NAME", "MissingMayor");
  stbds_shput(string_map, "Company Name", "MissingCompany");
  stbds_shput(string_map, "COMPANY NAME", "MissingCompany");
  stbds_shput(string_map, "Language", "USA");
  stbds_shput(string_map, "LANGUAGE", "USA");

  stbds_sh_new_arena(dword_map);
  stbds_shput(dword_map, "AUTOBUDGET", 0);
  stbds_shput(dword_map, "AUTOGOTO", 1);
  stbds_shput(dword_map, "AUTOSAVE", 0);
  stbds_shput(dword_map, "DISASTERS", 1);
  stbds_shput(dword_map, "MUSIC", 1);
  stbds_shput(dword_map, "SOUND", 1);
  stbds_shput(dword_map, "SPEED", 1);
  stbds_shput(dword_map, "CycleColors", 1);
  stbds_shput(dword_map, "GridHeight", 2);
  stbds_shput(dword_map, "GridWidth", 2);
  stbds_shput(dword_map, "ShowClipRegion", 0);
  stbds_shput(dword_map, "ShowDrawGrid", 0);
  stbds_shput(dword_map, "SnapToGrid", 0);
  stbds_shput(dword_map, "Speed", 1);
  stbds_shput(dword_map, "Sound", 1);
  stbds_shput(dword_map, "SCURK", 256);
  stbds_shput(dword_map, "Color Check", 0);
  stbds_shput(dword_map, "Last Color Depth", 32);

  if (MH_Initialize() != MH_OK) {
    return 1;
  }

  return hook(L"advapi32.dll",
              "RegQueryValueExA",
              (LPVOID)RegQueryValueExA_fn,
              (LPVOID*)&RegQueryValueExA_ptr)
      || hook(L"advapi32.dll",
              "RegSetValueExA",
              (LPVOID)RegSetValueExA_fn,
              (LPVOID*)&RegSetValueExA_ptr)
      || hook(L"advapi32.dll",
              "RegCreateKeyExA",
              (LPVOID)RegCreateKeyExA_fn,
              (LPVOID*)&RegCreateKeyExA_ptr)
      || hook(L"advapi32.dll",
              "RegCloseKey",
              (LPVOID)RegCloseKey_fn,
              (LPVOID*)&RegCloseKey_ptr)
      || hook(L"comdlg32.dll",
              "GetOpenFileNameA",
              (LPVOID)GetOpenFileNameA_fn,
              (LPVOID*)&GetOpenFileNameA_ptr)
      || hook(L"comdlg32.dll",
              "GetSaveFileNameA",
              (LPVOID)GetSaveFileNameA_fn,
              (LPVOID*)&GetSaveFileNameA_ptr);
}

int hooks_dtor()
{
  stbds_shfree(dword_map);
  stbds_shfree(string_map);
  stbds_hmfree(section_map);
  stbds_shfree(reg_map);
  stbds_shfree(paths_map);

  MH_STATUS result = MH_Uninitialize();
  if (result == MH_OK || result == MH_ERROR_NOT_INITIALIZED) {
    return 0;
  }

  return 1;
}
