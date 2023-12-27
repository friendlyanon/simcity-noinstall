#pragma once

#include <string.h>

struct paths_entry { char const* key; size_t value; };

struct paths_entry const* paths_lookup(char const* str, size_t len);
