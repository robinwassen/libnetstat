cmake_minimum_required (VERSION 3.9)

function (DisableCompilingPlatformSpecificSources file_list)
  foreach(file ${file_list})
    if (
      ((${file} MATCHES "^(.*)win") AND NOT WIN32) OR
      ((${file} MATCHES "^(.*)unix") AND NOT UNIX)  OR
      ((${file} MATCHES "^(.*)mac") AND NOT APPLE)
    )
      set_source_files_properties(
        ${file}
        PROPERTIES HEADER_FILE_ONLY TRUE
      )
    endif()
  endforeach()
endfunction()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(all_sources
  src/cli.c
  src/libnetstat_win.c
  src/libnetstat_unix.c
)

DisableCompilingPlatformSpecificSources("${all_sources}")

project(clinetstat)
add_executable(clinetstat ${all_sources})

project(libnetstat)
project(libnetstat VERSION 0.0.1 DESCRIPTION "libnetstat description")
add_library(libnetstat SHARED
  ${all_sources}
)
set_target_properties(libnetstat PROPERTIES PUBLIC_HEADER src/libnetstat.h)