#pragma once

/**
 * @file
 * Hooks
 *
 * To fix issues in the game, Win32 API functions are hooked using the Detours
 * mechanism and operations are intercepted. The MinHook library is used to
 * simplify access to this mechanism.
 *
 * Hooks fix the following issues with the game:
 *   - Opening the dialog for save/load causes a crash
 *   - Videos are fit to the desktop screen, because otherwise they are 320x240
 *   - Game relies on the 16-bit installer setting registry entries to assets
 *   - Game relies on palette swapping redrawing the entire screen
 *
 * As a nice-to-have, the hooks make the game portable and not rely on the
 * registry for anything.
 */

#include "paths.h"

/**
 * @defgroup hooks Hooks
 * @{
 */

/**
 * Pre-calculated desktop dimensions
 *
 * Members contain the dimensions already halved for video scaling calculations.
 */
struct screen
{
  int half_width;
  int half_height;
};

/**
 * Initializes the hooks
 *
 * @param [in] paths ::paths singleton
 * @param [in] drive Drive letter to fake loading videos from
 * @param [in] screen
 * @return 0 if successful, non-zero if an error occured
 */
int hooks_ctor(  //
    struct paths const* paths,
    char drive,
    struct screen const* screen);

/**
 * Destroys resources related to hooks
 *
 * @return 0 if successful, non-zero if an error occured
 */
int hooks_dtor(void);

/** @} */
