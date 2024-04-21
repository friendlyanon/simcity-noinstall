#pragma once

/**
 * @file
 * Fake registry key handles
 *
 * To properly fool the game to not create registry entries and for the hooks to
 * know when the game passes in fake keys, values starting with @c 0x80000040
 * correspond to fake keys that are never passed to the Win32 API. The Win32 API
 * also uses predefined keys to address hives for example.
 */

/**
 * @defgroup fake_keys Fake keys
 * @{
 */

/**
 * @defgroup fake_keys_offsets Offsets
 * @ingroup fake_keys
 * @{
 */

#define MAXIS_KEY 0
#define SC2K_KEY 1
#define PATHS_KEY 2
#define WINDOWS_KEY 3
#define VERSION_KEY 4
#define OPTIONS_KEY 5
#define LOCALIZE_KEY 6
#define REGISTRATION_KEY 7
#define SCURK_KEY 8

#define FIRST_KEY 0
#define LAST_KEY 8

/** @} */

#define KEY_BASE 0x80000040UL

/**
 * Is value equal to the specified @link fake_keys_offsets key @endlink?
 *
 * @param [in] value A @c HKEY value
 * @param [in] key One of the @link fake_keys_offsets keys @endlink
 *
 * @details All arguments are expanded once.
 */
#define IS_KEY(value, key) ((value) == (HKEY)(KEY_BASE + (key)))

/**
 * Is the specified value one of the @link fake_keys_offsets fake keys @endlink?
 *
 * @param [in] key A @c HKEY value
 *
 * @details @p key is expanded twice.
 */
#define IS_FAKE_KEY(key) \
  ((KEY_BASE + FIRST_KEY) <= (key) && (key) <= (KEY_BASE + LAST_KEY))

/** @} */
