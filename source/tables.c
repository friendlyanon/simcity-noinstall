#include "tables.h"

#include "array_size.h"
#include "paths.h"
#include "reg_keys.h"
#include "string.h"

struct string_view
{
  char const* data;
  size_t size;
};

#define SV(str) \
  { \
    str, lengthof(str) \
  }

struct sv_to_cstr_entry
{
  struct string_view key;
  char const* value;
};

struct sv_to_dword_entry
{
  struct string_view key;
  unsigned long value;
};

struct sv_to_size_entry
{
  struct string_view key;
  size_t value;
};

#define LOOKUP_STR(ret, name) \
  ret const* name##_lookup(char const* key, size_t size) \
  { \
    size_t i = 0; \
    for (; i != countof(name##_table); ++i) { \
      if (size == name##_table[i].key.size \
          && key[0] == name##_table[i].key.data[0] \
          && memcmp(key + 1, name##_table[i].key.data + 1, size - 1) == 0) \
      { \
        return &name##_table[i].value; \
      } \
    } \
    return NULL; \
  }

struct sv_to_cstr_entry string_table[] = {
    {SV("Mayor Name"), "MissingMayor"},
    {SV("MAYOR NAME"), "MissingMayor"},
    {SV("Company Name"), "MissingCompany"},
    {SV("COMPANY NAME"), "MissingCompany"},
    {SV("Language"), "USA"},
    {SV("LANGUAGE"), "USA"},
};

LOOKUP_STR(char const*, string)

struct sv_to_dword_entry dword_table[] = {
    {SV("AUTOBUDGET"), 0},
    {SV("AUTOGOTO"), 1},
    {SV("AUTOSAVE"), 0},
    {SV("DISASTERS"), 1},
    {SV("MUSIC"), 1},
    {SV("SOUND"), 1},
    {SV("SPEED"), 1},
    {SV("CycleColors"), 1},
    {SV("GridHeight"), 2},
    {SV("GridWidth"), 2},
    {SV("ShowClipRegion"), 0},
    {SV("ShowDrawGrid"), 0},
    {SV("SnapToGrid"), 0},
    {SV("Speed"), 1},
    {SV("Sound"), 1},
    {SV("SCURK"), 256},
    {SV("Color Check"), 0},
    {SV("Last Color Depth"), 32},
};

LOOKUP_STR(unsigned long, dword)

struct sv_to_size_entry paths_table[] = {
    {SV("DATA"), offsetof(struct paths, data)},
    {SV("GRAPHICS"), offsetof(struct paths, graphics)},
    {SV("Music"), offsetof(struct paths, sounds)},
    {SV("MUSIC"), offsetof(struct paths, sounds)},
    {SV("SOUND"), offsetof(struct paths, sounds)},
    {SV("SCENARIOS"), offsetof(struct paths, scenarios)},
    {SV("HOME"), offsetof(struct paths, home)},
    {SV("CITIES"), offsetof(struct paths, cities)},
    {SV("Cities"), offsetof(struct paths, cities)},
    {SV("SAVEGAME"), offsetof(struct paths, cities)},
    {SV("TILESETS"), offsetof(struct paths, tilesets)},
    {SV("TileSets"), offsetof(struct paths, tilesets)},
};

LOOKUP_STR(size_t, paths)

struct sv_to_dword_entry reg_table[] = {
    {SV("Maxis"), MAXIS_KEY},
    {SV("SimCity 2000"), SC2K_KEY},
    {SV("PATHS"), PATHS_KEY},
    {SV("Paths"), PATHS_KEY},
    {SV("Windows"), WINDOWS_KEY},
    {SV("VERSION"), VERSION_KEY},
    {SV("Version"), VERSION_KEY},
    {SV("OPTIONS"), OPTIONS_KEY},
    {SV("LOCALIZE"), LOCALIZE_KEY},
    {SV("Localize"), LOCALIZE_KEY},
    {SV("REGISTRATION"), REGISTRATION_KEY},
    {SV("SCURK"), SCURK_KEY},
};

LOOKUP_STR(unsigned long, reg)

struct section_entry
{
  unsigned long key;
  char const* value;
};

struct section_entry section_table[] = {
    {WINDOWS_KEY, "Windows"},
    {VERSION_KEY, "VERSION"},
    {OPTIONS_KEY, "OPTIONS"},
    {LOCALIZE_KEY, "LOCALIZE"},
    {REGISTRATION_KEY, "REGISTRATION"},
    {SCURK_KEY, "SCURK"},
};

char const* const* section_lookup(unsigned long key)
{
  if (key < KEY_BASE || (KEY_BASE + LAST_KEY) < key) {
    return NULL;
  }

  key -= KEY_BASE;

  {
    size_t i = 0
    for (; i != countof(section_table); ++i) {
      if (section_table[i].key == key) {
        return &section_table[i].value;
      }
    }
  }

  return NULL;
}
