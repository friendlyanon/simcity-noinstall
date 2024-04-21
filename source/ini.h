#pragma once

/**
 * @file
 * Managing game settings
 *
 * The game normally interacts with the Windows registry to locate its assets
 * and to store settings. These functions handle the ways that game interacts
 * with it and redirect them to an ini file.
 */

/**
 * @defgroup ini Game settings
 * @{
 */

/**
 * Retrieve string values from the settings file
 *
 * @param [in] ini_path The @link ::paths.ini ini @endlink member of the
 * ::paths singleton
 * @param [in] section ini section
 * @param [in] key Key in the specified ini @p section
 * @param [in] default_ A default value to write to the ini when the @p key
 * in the @p section can't be retrieved
 * @return The string associated to the @p key in the @p section
 *
 * @details To handle some differences between the game and SCURK, some of the
 * keys are converted to uppercase.
 */
char const* get_ini_string(  //
    char const* ini_path,
    char const* section,
    char const* key,
    char const* default_);

/**
 * Retrieve numeric values from the settings file
 *
 * @param [in] ini_path The @link ::paths.ini ini @endlink member of the
 * ::paths singleton
 * @param [in] section ini section
 * @param [in] key Key in the specified ini @p section
 * @param [in] default_ A default value to write to the ini when the @p key
 * in the @p section can't be retrieved
 * @return The number associated to the @p key in the @p section
 */
unsigned long get_ini_dword(  //
    char const* ini_path,
    char const* section,
    char const* key,
    unsigned long default_);

/**
 * Write numeric values to the settings file
 *
 * @param [in] ini_path The @link ::paths.ini ini @endlink member of the
 * ::paths singleton
 * @param [in] section ini section
 * @param [in] key Key in the specified ini @p section
 * @param [in] value The number to write to the @p key in the @p section
 */
void set_ini_dword(  //
    char const* ini_path,
    char const* section,
    char const* key,
    unsigned long value);

/** @} */
