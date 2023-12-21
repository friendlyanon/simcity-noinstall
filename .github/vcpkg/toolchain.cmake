cmake_minimum_required(VERSION 3.18)

find_program(clang clang REQUIRED)
find_program(rc rc REQUIRED)
find_program(ml ml REQUIRED)

set(file "${CMAKE_CURRENT_LIST_DIR}/x86-windows-clang-toolchain.cmake")
configure_file("${file}.in" "${file}" @ONLY)
