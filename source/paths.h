#pragma once

#define PATHS_LENGTH 260

struct paths
{
  char ini[PATHS_LENGTH];

  char data[PATHS_LENGTH];
  char graphics[PATHS_LENGTH];
  char sounds[PATHS_LENGTH];
  char scenarios[PATHS_LENGTH];
  char cities[PATHS_LENGTH];
  char savegame[PATHS_LENGTH];
  char tilesets[PATHS_LENGTH];

  char home[PATHS_LENGTH];
  char movies[PATHS_LENGTH];
};

struct paths* paths_ctor(char const* dll_path);
