#pragma once

#include <string.h>

struct dword_entry { char const* key; unsigned long value; };

struct dword_entry const* dword_lookup(char const* str, size_t len);
