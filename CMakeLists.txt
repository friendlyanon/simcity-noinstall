cmake_minimum_required(VERSION 3.5...3.16)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "In-source builds are not supported")
endif()

project(simcity-noinstall C ASM_MASM)

if(NOT CMAKE_SIZEOF_VOID_P EQUAL "4")
  message(FATAL_ERROR "32-bit target required")
endif()

add_custom_command(
    OUTPUT stubs-generated.c stubs-generated.asm stubs-generated.def
    COMMAND "${CMAKE_COMMAND}"
    "-Doutput=${PROJECT_BINARY_DIR}"
    "-Dinput=${PROJECT_SOURCE_DIR}/source/winmm.txt"
    -P "${PROJECT_SOURCE_DIR}/cmake/generate-stubs.cmake"
    MAIN_DEPENDENCY source/winmm.txt
    DEPENDS cmake/generate-stubs.cmake
    COMMENT "Generating winmm.dll stubs"
    VERBATIM
)

add_library(
    simcity-noinstall MODULE
    source/float.asm
    source/float.h
    source/hooks.asm
    source/hooks.c
    source/hooks.h
    source/ini.c
    source/ini.h
    source/paths.c
    source/paths.h
    source/simcity-noinstall.c
    source/stubs.c
    source/stubs.h
    source/tables/dword_tbl.c
    source/tables/paths_tbl.c
    source/tables/reg_tbl.c
    source/tables/section_tbl.c
    source/tables/string_tbl.c
    "${PROJECT_BINARY_DIR}/stubs-generated.c"
    "${PROJECT_BINARY_DIR}/stubs-generated.asm"
    "${PROJECT_BINARY_DIR}/stubs-generated.def"
)

target_compile_definitions(simcity-noinstall PRIVATE _CRT_SECURE_NO_WARNINGS=1)

if(NOT CMAKE_VERSION VERSION_LESS "3.16")
  target_precompile_headers(simcity-noinstall PRIVATE [[<Windows.h>]])
endif()

target_include_directories(simcity-noinstall PRIVATE source)

find_package(minhook REQUIRED)
target_link_libraries(simcity-noinstall PRIVATE minhook::minhook shlwapi)

install(TARGETS simcity-noinstall DESTINATION .)
if(NOT CMAKE_VERSION VERSION_LESS "3.14")
  install(CODE [[
get_filename_component(install_prefix "${CMAKE_INSTALL_PREFIX}" ABSOLUTE)
message(STATUS "Renaming: ${install_prefix}/WINMM.DLL")
file(REMOVE "${install_prefix}/WINMM.DLL")
file(RENAME "${install_prefix}/$<TARGET_FILE_NAME:simcity-noinstall>" "${install_prefix}/WINMM.DLL")
]])
endif()
