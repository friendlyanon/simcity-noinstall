#pragma once

#include "paths.h"

int hooks_ctor(struct paths const* paths, char drive);

int hooks_dtor(void);
