#pragma once

/**
 * @file
 * Floating point operations
 *
 * Compilers insert a lot of instructions to make sure that the values returned
 * by x87 instructions conform to the IEEE 754 standard and don't convert
 * floating point numbers to integers as efficiently when the bounds are known
 * as a programmer can in assembly. For this reason, all floating point
 * operations are written using assembly to cut down on unnecessary cruft. The
 * x87 instruction set is a reasonable target, since it is period correct for
 * the game and modern CPUs support the instruction set all the same.
 */

/**
 * @defgroup float_ops Floating point operations
 * @{
 */

/**
 * Return value of ::scale
 */
struct scaled
{
  int a;
  int b;
};

#ifndef FLOATAPI
#  define FLOATAPI __fastcall
#endif

/**
 * Calculate the growth factor needed to fit videos in the game to the screen
 *
 * This function calculates and stores a growth factor in a global variable in
 * the assembly implementation for use by the ::scale function.
 * @param [in] width Width of the desktop monitor
 * @param [in] height Height of the desktop monitor
 *
 * @details Desktop monitor is assumed to be the one where the game will be
 * displayed. Multi-monitor setups have not been tested yet.
 */
void FLOATAPI calculate_growth(int const* width, int const* height);

/**
 * Scale values by the calculated growth factor
 *
 * @param [in] a
 * @param [in] b
 * @return @p a and @p b scaled up by the growth factor
 *
 * @details The arguments are pointers to simplify the assembly implementation.
 */
struct scaled FLOATAPI scale(int const* a, int const* b);

/** @} */
