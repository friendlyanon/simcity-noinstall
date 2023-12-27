#pragma once

#include <string.h>

struct string_entry { char const* key; char const* value; };

struct string_entry const* string_lookup(char const* str, size_t len);
