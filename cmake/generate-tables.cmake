# This file uses https://github.com/rurban/gperf/tree/intkeys with patches, but
# the output is still edited by hand due to being incomplete

cmake_minimum_required(VERSION 3.5)

find_program(GPERF gperf)
if(NOT GPERF)
  message(FATAL_ERROR "Couldn't find gperf (${GPERF})")
endif()

get_filename_component(root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

set(gperf gperf)
set(gperf_abs "${root}/gperf")
file(MAKE_DIRECTORY "${gperf_abs}")
set(out source/tables)
set(out_abs "${root}/${out}")
file(MAKE_DIRECTORY "${out_abs}")

function(gen_table name table header)
  file(WRITE "${gperf_abs}/${name}.gperf" "${table}")
  execute_process(
      COMMAND "${GPERF}" "--output-file=${out}/${name}_tbl.c" "${gperf}/${name}.gperf"
      WORKING_DIRECTORY "${root}"
      RESULT_VARIABLE result
  )
# if(NOT result EQUAL "0")
#   message(FATAL_ERROR "gperf returned with exit code ${result}")
# endif()
  file(READ "${out_abs}/${name}_tbl.c" content)
  string(REGEX REPLACE "^[^#]+" "" content "${content}")
  string(REPLACE register\  "" content "${content}")
  file(WRITE "${out_abs}/${name}_tbl.c" "${content}")
  file(WRITE "${out_abs}/${name}_tbl.h" "${header}")
endfunction()

macro(make_table name end)
  set(table "\
%language=ANSI-C
%define slot-name key
%define lookup-function-name ${name}_lookup
%readonly-tables
%global-table
%struct-type
%omit-struct-type
${end}%%
")
endmacro()

macro(append_table key value)
  string(APPEND table "${key}, ${value}\n")
endmacro()

set(extra "")
function(gen_str_table name value)
  set(struct "struct ${name}_entry { char const* key; ${value} value; };")
  set(header "\
#pragma once

#include <string.h>

${struct}

struct ${name}_entry const* ${name}_lookup(char const* str, size_t len);
")
  make_table("${name}" "\
%compare-strncmp
%null-strings
%{
#include \"tables/${name}_tbl.h\"
${extra}%}
${struct}
")
  foreach(line IN LISTS ARGN)
    append_table(${line})
  endforeach()
  gen_table("${name}" "${table}" "${header}")
endfunction()

gen_str_table(
    string "char const*"
    "Mayor Name\;\"MissingMayor\""
    "MAYOR NAME\;\"MissingMayor\""
    "Company Name\;\"MissingCompany\""
    "COMPANY NAME\;\"MissingCompany\""
    "Language\;\"USA\""
    "LANGUAGE\;\"USA\""
)

gen_str_table(
    dword "unsigned long"
    "AUTOBUDGET\;0"
    "AUTOGOTO\;1"
    "AUTOSAVE\;0"
    "DISASTERS\;1"
    "MUSIC\;1"
    "SOUND\;1"
    "SPEED\;1"
    "CycleColors\;1"
    "GridHeight\;2"
    "GridWidth\;2"
    "ShowClipRegion\;0"
    "ShowDrawGrid\;0"
    "SnapToGrid\;0"
    "Speed\;1"
    "Sound\;1"
    "SCURK\;256"
    "Color Check\;0"
    "Last Color Depth\;32"
)

set(extra "\
#include <stddef.h>

#include \"paths.h\"
")
gen_str_table(
    paths size_t
    "DATA\;offsetof(struct paths, data)"
    "GRAPHICS\;offsetof(struct paths, graphics)"
    "Music\;offsetof(struct paths, sounds)"
    "MUSIC\;offsetof(struct paths, sounds)"
    "SOUND\;offsetof(struct paths, sounds)"
    "SCENARIOS\;offsetof(struct paths, scenarios)"
    "HOME\;offsetof(struct paths, home)"
    "CITIES\;offsetof(struct paths, cities)"
    "Cities\;offsetof(struct paths, cities)"
    "SAVEGAME\;offsetof(struct paths, cities)"
    "TILESETS\;offsetof(struct paths, tilesets)"
    "TileSets\;offsetof(struct paths, tilesets)"
)

set(extra "\
#include \"reg_keys.h\"
")
gen_str_table(
    reg "unsigned long"
    "Maxis\;MAXIS_KEY"
    "SimCity 2000\;SC2K_KEY"
    "PATHS\;PATHS_KEY"
    "Paths\;PATHS_KEY"
    "Windows\;WINDOWS_KEY"
    "VERSION\;VERSION_KEY"
    "Version\;VERSION_KEY"
    "OPTIONS\;OPTIONS_KEY"
    "LOCALIZE\;LOCALIZE_KEY"
    "Localize\;LOCALIZE_KEY"
    "REGISTRATION\;REGISTRATION_KEY"
    "SCURK\;SCURK_KEY"
)

set(header "#pragma once\n\n")
set(value 0)
set(first "")
set(last "")
foreach(
    key
    MAXIS
    SC2K
    PATHS
    WINDOWS
    VERSION
    OPTIONS
    LOCALIZE
    REGISTRATION
    SCURK
)
  set(key "${key}_KEY")
  set("${key}" "${value}")
  if(first STREQUAL "")
    set(first "${value}")
  endif()
  set(last "${value}")
  string(APPEND header "#define ${key} ${value}\n")
  math(EXPR value "${value} + 1")
endforeach()
file(WRITE "${root}/source/reg_keys.h" "${header}
#define FIRST_KEY ${first}
#define LAST_KEY ${last}

#define KEY_BASE 0x80000040UL

#define IS_KEY(value, key) ((value) == (HKEY)(KEY_BASE + (key)))

#define IS_FAKE_KEY(key) \\
  ((KEY_BASE + FIRST_KEY) <= (key) && (key) <= (KEY_BASE + LAST_KEY))
")

set(header "\
#pragma once

#include \"reg_keys.h\"

struct section_entry { unsigned long key; char const* value; };

struct section_entry const* section_lookup(unsigned char num);

#define SECTION_LOOKUP(out, key) \\
  do { \\
    unsigned long _key = (unsigned long)key; \\
    if (_key < KEY_BASE || (KEY_BASE + LAST_KEY) < _key) { \\
      break; \\
    } \\
    _key -= KEY_BASE; \\
    { \\
      struct section_entry const* _entry = section_lookup((unsigned char)_key); \\
      if (_entry != 0) { \\
        out = _entry; \\
      } \\
    } \\
  } while (0)
")
make_table(section "\
%includes
%struct-type
%omit-struct-type
%{
#include \"tables/section_tbl.h\"

#define uint8_t unsigned char
%}
struct section_entry { uint32_t key; char const* value; };
")
macro(append_table_ key value)
  string(APPEND table "${${key}}, ${value}\n")
endmacro()
foreach(
    line
    [[WINDOWS_KEY;"Windows"]]
    [[VERSION_KEY;"VERSION"]]
    [[OPTIONS_KEY;"OPTIONS"]]
    [[LOCALIZE_KEY;"LOCALIZE"]]
    [[REGISTRATION_KEY;"REGISTRATION"]]
    [[SCURK_KEY;"SCURK"]]
)
  append_table_(${line})
endforeach()
gen_table(section "${table}" "${header}")
