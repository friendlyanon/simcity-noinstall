#pragma once

/**
 * @file
 * Lookup table accessors
 *
 * These functions are used by registry related @link hooks hooks @endlink to
 * quickly access table entries. These tables are looked up linearly.
 */

#include <stddef.h>

/**
 * @defgroup lookup Table lookup
 *
 * Lookup functions for various tables.
 * @{
 */

/**
 * Accessor for the table of string values
 *
 * Accessor for string keys whose associated value is a string.
 * @param [in] key Registry key
 * @param [in] size Length of the key
 * @return A const pointer pointing at the value if found, otherwise @c NULL
 */
char const* const* string_lookup(char const* key, size_t size);

/**
 * Accessor for the table of numeric values
 *
 * Accessor for string keys whose associated value is a number.
 * @param [in] key Registry key
 * @param [in] size Length of the key
 * @return A const pointer pointing at the value if found, otherwise @c NULL
 */
unsigned long const* dword_lookup(char const* key, size_t size);

/**
 * Accessor for the table of path offsets
 *
 * Accessor for string keys whose associated value is an offset into the ::paths
 * struct.
 * @param [in] key Registry key
 * @param [in] size Length of the key
 * @return A const pointer pointing at the offset if found, otherwise @c NULL
 */
size_t const* paths_lookup(char const* key, size_t size);

/**
 * Accessor for the table of fake key handles
 *
 * Accessor for string keys whose associated value is a fake @c HKEY.
 * @param [in] key Registry key
 * @param [in] size Length of the key
 * @return A const pointer pointing at the fake key if found, otherwise @c NULL
 */
unsigned long const* reg_lookup(char const* key, size_t size);

/**
 * Accessor for the table of section names for fake key handles
 *
 * Accessor for fake keys whose associated value is a string to be used as the
 * associated section name.
 * @param [in] key Value of the fake key
 * @return A const pointer pointing at section name if found, otherwise @c NULL
 */
char const* const* section_lookup(unsigned long key);

/** @} */
