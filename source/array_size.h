#pragma once

/**
 * @file
 * Common C array macros
 */

/**
 * @defgroup carray_size Array macros
 * @{
 */

/**
 * Get the number of elements of a C array
 *
 * @param [in] x C array with static bounds
 * @return Number of elements
 *
 * @details @p x is evaluated twice.
 */
#define countof(x) (sizeof(x) / sizeof(*(x)))

/**
 * Get the size of a C string
 *
 * This macro function is like @c strlen, but for string literals only.
 * @param [in] s C string literal
 * @return Number of characters without the null terminator
 *
 * @details @p s is evaluated twice.
 */
#define lengthof(s) (countof(s) - 1)

/** @} */
