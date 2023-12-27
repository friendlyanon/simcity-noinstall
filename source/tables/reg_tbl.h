#pragma once

#include <string.h>

struct reg_entry { char const* key; unsigned long value; };

struct reg_entry const* reg_lookup(char const* str, size_t len);
