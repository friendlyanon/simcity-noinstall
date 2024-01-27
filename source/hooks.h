#pragma once

#include "paths.h"

struct screen
{
  int half_width;
  int half_height;
};

int hooks_ctor(  //
    struct paths const* paths,
    char drive,
    struct screen const* screen);

int hooks_dtor(void);
