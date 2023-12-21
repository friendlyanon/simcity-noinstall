#include "ini.h"

#include <Windows.h>
#include <stdio.h>

#include "array_size.h"

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
  DWORD size = COUNTOF(windows_sc2k);
  char uppercase_key[33] = {0};
  switch (key[0]) {
    case 'M':
      out = mayor_name;
      size = COUNTOF(mayor_name);
      break;
    case 'C':
      out = company_name;
      size = COUNTOF(company_name);
      break;
    case 'L':
      out = language;
      size = COUNTOF(language);
      break;
  }

  if (*out != '\0') {
    return out;
  }

  if (out != windows_sc2k) {
    size_t length = strlen(key);
    size_t i = 0;
    for (; i != length; ++i) {
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
  unsigned long value = default_;
  char buffer[11] = {0};
  (void)GetPrivateProfileStringA(
      section, key, "", buffer, COUNTOF(buffer), ini_path);
  if (GetLastError() != ERROR_SUCCESS) {
    return value;
  }

  (void)sscanf(buffer, "%lu", &value);
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
