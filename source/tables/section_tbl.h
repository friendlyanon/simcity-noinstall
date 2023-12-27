#pragma once

#include "reg_keys.h"

struct section_entry { unsigned long key; char const* value; };

struct section_entry const* section_lookup(unsigned char num);

#define SECTION_LOOKUP(out, key) \
  do { \
    unsigned long _key = (unsigned long)(key); \
    if (_key < KEY_BASE || (KEY_BASE + LAST_KEY) < _key) { \
      break; \
    } \
    _key -= KEY_BASE; \
    { \
      struct section_entry const* _entry = section_lookup((unsigned char)_key); \
      if (_entry != 0) { \
        out = _entry; \
      } \
    } \
  } while (0)
