cmake_minimum_required (VERSION 3.9)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

project(clinetstat)
add_executable(clinetstat
	src/cli.c
    src/libnetstat_win.c
)

project(libnetstat)
project(libnetstat VERSION 0.0.1 DESCRIPTION "libnetstat description")
add_library(libnetstat SHARED
    src/libnetstat_win.c
)
set_target_properties(libnetstat PROPERTIES PUBLIC_HEADER src/libnetstat.h)