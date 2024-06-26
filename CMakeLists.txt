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
    source/tables.c
    source/tables.h
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

install(
    FILES "\$<TARGET_FILE:simcity-noinstall>"
    DESTINATION .
    RENAME WINMM.DLL
)

option(BUILD_WINMM_EXPORT_TOOL "Build tools/winmm-exports" OFF)
mark_as_advanced(BUILD_WINMM_EXPORT_TOOL)
if(BUILD_WINMM_EXPORT_TOOL)
  add_subdirectory(tools/winmm-exports winmm-exports)
endif()
