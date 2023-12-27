#pragma once

#define MAXIS_KEY 0
#define SC2K_KEY 1
#define PATHS_KEY 2
#define WINDOWS_KEY 3
#define VERSION_KEY 4
#define OPTIONS_KEY 5
#define LOCALIZE_KEY 6
#define REGISTRATION_KEY 7
#define SCURK_KEY 8

#define FIRST_KEY 0
#define LAST_KEY 8

#define KEY_BASE 0x80000040UL

#define IS_KEY(value, key) ((value) == (HKEY)(KEY_BASE + (key)))

#define IS_FAKE_KEY(key) \
  ((KEY_BASE + FIRST_KEY) <= (key) && (key) <= (KEY_BASE + LAST_KEY))
