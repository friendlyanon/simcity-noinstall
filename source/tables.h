#pragma once

#include <stddef.h>

char const* const* string_lookup(char const* key, size_t size);

unsigned long const* dword_lookup(char const* key, size_t size);

size_t const* paths_lookup(char const* key, size_t size);

unsigned long const* reg_lookup(char const* key, size_t size);

char const* const* section_lookup(unsigned long key);
