#pragma once

/**
 * @file
 * Paths to assets
 *
 * This file contains code responsible for populating and storing the paths to
 * various assets and the path to the .ini file storing the game's settings.
 */

/**
 * @defgroup paths Paths
 * @{
 */

/**
 * A value equal to @c MAX_PATH
 *
 * Specifying the value like this avoids having to include @c Windows.h in this
 * header.
 */
#define PATHS_LENGTH 260

/**
 * Buffers to hold various paths for the hooks
 *
 * The size of all members is @ref PATHS_LENGTH and some members have their
 * offset stored in ::paths_lookup that were acquired using @c offsetof. Only
 * one instance of this type should exist, which can be initialized and
 * retrieved using ::paths_ctor.
 */
struct paths
{
  char ini[PATHS_LENGTH]; /**< Path to the game's settings */

  char data[PATHS_LENGTH];
  char graphics[PATHS_LENGTH];
  char sounds[PATHS_LENGTH];
  char scenarios[PATHS_LENGTH];
  char cities[PATHS_LENGTH];
  char savegame[PATHS_LENGTH];
  char tilesets[PATHS_LENGTH];

  char home[PATHS_LENGTH];
  char movies[PATHS_LENGTH];
};

/**
 * Initializes and returns the pointer to the ::paths singleton
 *
 * @param [in] dll_path The full path to the current DLL with the hooks
 * @return A pointer to the ::paths singleton if successful, otherwise @c NULL
 *
 * @details Members of the singleton will be null terminated for ease of use
 * with the Win32 API.
 */
struct paths* paths_ctor(char const* dll_path);

/** @} */
