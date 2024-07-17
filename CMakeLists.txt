cmake_minimum_required(VERSION 3.10)

# Add color support to our messages.
# See: https://stackoverflow.com/questions/18968979/how-to-get-colorized-output-with-cmake
if(NOT WIN32)
  string(ASCII 27 Esc)
  set(ColorReset   "${Esc}[m")
  set(ColorBold    "${Esc}[1m")
  set(ColorRed     "${Esc}[31m")
  set(ColorGreen   "${Esc}[32m")
  set(ColorYellow  "${Esc}[33m")
  set(ColorBlue    "${Esc}[34m")
  set(ColorMagenta "${Esc}[35m")
  set(ColorCyan    "${Esc}[36m")
  set(ColorWhite   "${Esc}[37m")
endif()

set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_LINKER "/usr/bin/clang++")

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
endif(CCACHE_FOUND)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(BURDOCK)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -pedantic -Wall -Werror -Wfatal-errors -Wextra -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++20 -D_GLIBCXX_DEBUG -O0 -gdwarf-4")

set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${assignment_clean_rm}")

# find_package(Catch2 REQUIRED)

set(lib_dir ${CMAKE_SOURCE_DIR}/lib)
set(src_dir ${CMAKE_SOURCE_DIR}/src)
set(tests_dir ${CMAKE_SOURCE_DIR}/tests)
set(entry_dir ${CMAKE_SOURCE_DIR}/entry)

add_subdirectory(${lib_dir})
add_subdirectory(${src_dir})

set(entrypoints "main")

foreach(entrypoint IN LISTS entrypoints)
    add_executable(${entrypoint} ${entry_dir}/${entrypoint}.cpp)
    target_link_libraries(${entrypoint} PRIVATE libs src)
endforeach()
