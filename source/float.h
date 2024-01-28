#pragma once

struct scaled
{
  int a;
  int b;
};

#ifndef FLOATAPI
#  define FLOATAPI __fastcall
#endif

void FLOATAPI calculate_growth(int const* width, int const* height);

struct scaled FLOATAPI scale(int const* a, int const* b);
