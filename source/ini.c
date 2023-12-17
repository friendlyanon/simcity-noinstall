#include "ini.h"

#include <Windows.h>
#include <stdio.h>

static char ini_toupper(char c)
{
  return 'a' <= c && c <= 'z' ? c - ('a' - 'A') : c;
}

static char mayor_name[33] = {0};
static char company_name[32] = {0};
static char language[33] = {0};
static char windows_sc2k[33] = {0};

char const* get_ini_string(char const* ini_path,
                           char const* section,
                           char const* key,
                           char const* default_)
{
  char* out = windows_sc2k;
  DWORD size = sizeof(windows_sc2k);
  switch (key[0]) {
    case 'M':
      out = mayor_name;
      size = sizeof(mayor_name);
      break;
    case 'C':
      out = company_name;
      size = sizeof(company_name);
      break;
    case 'L':
      out = language;
      size = sizeof(language);
      break;
  }

  if (*out != '\0') {
    return out;
  }

  char uppercase_key[33] = {0};
  if (out != windows_sc2k) {
    size_t length = strlen(key);
    for (size_t i = 0; i != length; ++i) {
      uppercase_key[i] = ini_toupper(key[i]);
    }
    key = uppercase_key;
  }

  (void)GetPrivateProfileStringA(section, key, default_, out, size, ini_path);
  if (GetLastError() != ERROR_SUCCESS) {
    (void)WritePrivateProfileStringA(section, key, out, ini_path);
  }

  return out;
}

unsigned long get_ini_dword(char const* ini_path,
                            char const* section,
                            char const* key,
                            unsigned long default_)
{
  UINT value = GetPrivateProfileIntA(section, key, default_, ini_path);
  if (GetLastError() != ERROR_SUCCESS) {
    set_ini_dword(ini_path, section, key, value);
  }

  return value;
}

void set_ini_dword(char const* ini_path,
                   char const* section,
                   char const* key,
                   unsigned long value)
{
  char buffer[11] = {0};
  (void)sprintf(buffer, "%lu", value);
  (void)WritePrivateProfileStringA(section, key, buffer, ini_path);
}
