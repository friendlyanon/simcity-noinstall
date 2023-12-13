#include "ini.h"

#include <Windows.h>
#include <ctype.h>
#include <stdio.h>

static char mayor_name[33] = {0};
static char company_name[32] = {0};
static char language[33] = {0};

char const* get_ini_string(char const* ini_path,
                           char const* section,
                           char const* key,
                           char const* default_)
{
  char* out = NULL;
  DWORD size = 0;
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
    default:
      __assume(0);
  }

  if (*out != '\0') {
    return out;
  }

  char uppercase_key[33] = {0};
  size_t length = strlen(key);
  for (size_t i = 0; i != length; ++i) {
    uppercase_key[i] = toupper(key[i]);
  }

  (void)GetPrivateProfileStringA(
      section, uppercase_key, default_, out, size, ini_path);
  if (GetLastError() != ERROR_SUCCESS) {
    (void)WritePrivateProfileStringA(section, uppercase_key, out, ini_path);
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
