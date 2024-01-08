#include "paths.h"

#include <string.h>

#include "array_size.h"

static struct paths paths = {0};

static char const* find_reverse(char const* begin, char const* end, char value)
{
  if (begin == end) {
    return NULL;
  }

  do {
    --end;
    if (*end == value) {
      return end;
    }
  } while (begin != end);

  return NULL;
}

static void sc2k_paths(char const* dll_path, char const* slash)
{
  size_t size = (size_t)(slash - dll_path + 1);
  size_t count = PATHS_LENGTH - size - 1;

  (void)memcpy(paths.ini, dll_path, size);
  (void)memcpy(paths.data, dll_path, size);
  (void)memcpy(paths.graphics, dll_path, size);
  (void)memcpy(paths.sounds, dll_path, size);
  (void)memcpy(paths.scenarios, dll_path, size);
  (void)memcpy(paths.cities, dll_path, size);
  (void)memcpy(paths.savegame, dll_path, size);
  (void)memcpy(paths.tilesets, dll_path, size);

  (void)strncat(paths.ini + size, "simcity-noinstall.ini", count);
  (void)strncat(paths.data + size, "DATA", count);
  (void)strncat(paths.graphics + size, "BITMAPS", count);
  (void)strncat(paths.sounds + size, "SOUNDS", count);
  (void)strncat(paths.scenarios + size, "SCENARIO", count);
  (void)strncat(paths.cities + size, "CITIES", count);
  (void)strncat(paths.savegame + size, "CITIES", count);
  (void)strncat(paths.tilesets + size, "SCURKART", count);
}

static void root_paths(char const* dll_path, char const* slash)
{
  size_t size = (size_t)(slash - dll_path);
  (void)memcpy(
      paths.home, dll_path, size == lengthof("X:") ? lengthof("X:\\") : size);
  (void)memcpy(paths.movies, dll_path, size + 1);

  (void)strncat(paths.movies + size, "DATA\\", PATHS_LENGTH - size);
}

struct paths* paths_ctor(char const* dll_path)
{
  char const* last_slash =
      find_reverse(dll_path, dll_path + strlen(dll_path), '\\');
  char const* second_last_slash = NULL;
  if (last_slash == NULL) {
    return NULL;
  }

  second_last_slash = find_reverse(dll_path, last_slash, '\\');
  if (second_last_slash == NULL) {
    return NULL;
  }

  if ((size_t)(last_slash - dll_path) - 1 >= PATHS_LENGTH) {
    return NULL;
  }

  sc2k_paths(dll_path, last_slash);
  root_paths(dll_path, second_last_slash);

  return &paths;
}
