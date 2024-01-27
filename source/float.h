#pragma once

struct scaled
{
  unsigned int width;
  unsigned int height;
};

#ifndef FLOATAPI
#  define FLOATAPI __fastcall
#endif

void FLOATAPI calculate_growth(void const* width, void const* height);

struct scaled FLOATAPI scale(void const* width, void const* height);
