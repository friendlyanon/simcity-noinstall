#include "paths.h"

#include <string.h>

static struct paths paths = {0};

static char* find_reverse(char* begin, char* end, char value)
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

static void sc2k_paths(char* dll_path, char* slash)
{
  size_t size = (size_t)(slash - dll_path + 1);
  (void)memcpy(paths.ini, dll_path, size);
  (void)memcpy(paths.data, dll_path, size);
  (void)memcpy(paths.graphics, dll_path, size);
  (void)memcpy(paths.sounds, dll_path, size);
  (void)memcpy(paths.scenarios, dll_path, size);
  (void)memcpy(paths.cities, dll_path, size);
  (void)memcpy(paths.savegame, dll_path, size);
  (void)memcpy(paths.tilesets, dll_path, size);

  size_t count = PATHS_LENGTH - size - 1;
  (void)strncat(paths.ini + size, "simcity-noinstall.ini", count);
  (void)strncat(paths.data + size, "DATA", count);
  (void)strncat(paths.graphics + size, "BITMAPS", count);
  (void)strncat(paths.sounds + size, "SOUNDS", count);
  (void)strncat(paths.scenarios + size, "SCENARIO", count);
  (void)strncat(paths.cities + size, "CITIES", count);
  (void)strncat(paths.savegame + size, "CITIES", count);
  (void)strncat(paths.tilesets + size, "SCURKART", count);
}

static void root_paths(char* dll_path, char* slash)
{
  size_t size = (size_t)(slash - dll_path);
  (void)memcpy(paths.home,
               dll_path,
               size == sizeof("X:") - 1 ? sizeof("X:\\") - 1 : size);
  (void)memcpy(paths.movies, dll_path, size + 1);

  size_t count = PATHS_LENGTH - size;
  (void)strncat(paths.movies + size, "DATA\\", count);
}

struct paths* paths_ctor(char const* dll_path)
{
  char* last_slash = find_reverse(dll_path, dll_path + strlen(dll_path), '\\');
  if (last_slash == NULL) {
    return NULL;
  }

  char* second_last_slash = find_reverse(dll_path, last_slash, '\\');
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
