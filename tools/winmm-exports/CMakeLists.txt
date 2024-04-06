cmake_minimum_required(VERSION 3.5...3.16)

project(winmm-exports C)

add_executable(winmm-exports main.c)
target_include_directories(winmm-exports SYSTEM PRIVATE "${PROJECT_SOURCE_DIR}/../../source")

if(NOT CMAKE_VERSION VERSION_LESS "3.16")
  target_precompile_headers(winmm-exports PRIVATE [[<Windows.h>]])
endif()

include(GNUInstallDirs)
install(TARGETS winmm-exports DESTINATION .)
