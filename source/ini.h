#pragma once

char const* get_ini_string(char const* ini_path,
                           char const* section,
                           char const* key,
                           char const* default_);

unsigned long get_ini_dword(char const* ini_path,
                            char const* section,
                            char const* key,
                            unsigned long default_);

void set_ini_dword(char const* ini_path,
                   char const* section,
                   char const* key,
                   unsigned long value);
