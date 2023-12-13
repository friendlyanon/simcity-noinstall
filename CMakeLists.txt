cmake_minimum_required(VERSION 3.18)

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
    source/hooks.c
    source/hooks.h
    source/ini.c
    source/ini.h
    source/paths.c
    source/paths.h
    source/simcity-noinstall.c
    source/stubs.c
    source/stubs.h
    "${PROJECT_BINARY_DIR}/stubs-generated.c"
    "${PROJECT_BINARY_DIR}/stubs-generated.asm"
    "${PROJECT_BINARY_DIR}/stubs-generated.def"
)

target_compile_definitions(
    simcity-noinstall PRIVATE
    NTDDI_VERSION=0x0A000007
    _WIN32_WINNT=0x0A00
    UNICODE=1
    _UNICODE=1
    _CRT_SECURE_NO_WARNINGS=1
)

target_precompile_headers(simcity-noinstall PRIVATE [[<Windows.h>]])

target_compile_features(simcity-noinstall PRIVATE c_std_99)

target_include_directories(simcity-noinstall PRIVATE source)

find_package(minhook REQUIRED)
target_link_libraries(simcity-noinstall PRIVATE minhook::minhook winmm)

find_path(STB_DS_INCLUDE_DIR stb_ds.h REQUIRED)
target_include_directories(simcity-noinstall SYSTEM PRIVATE "${STB_DS_INCLUDE_DIR}")
set_property(SOURCE source/hooks.c PROPERTY COMPILE_DEFINITIONS STB_DS_IMPLEMENTATION=1)
target_compile_definitions(simcity-noinstall PRIVATE STBDS_NO_SHORT_NAMES=1)

install(TARGETS simcity-noinstall DESTINATION .)
