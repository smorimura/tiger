################################################################################
# Project: TigerScript
# Legal  : All content 2014 Trent Reed, all rights reserved.
# Author : Trent Reed
# About  : The main CMake script
# Usage  : mkdir build
#          cd build
#          cmake ..
#          make
################################################################################

cmake_minimum_required(VERSION 2.8)
project(tiger C)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Options for configuring Tiger
option(TIGER_PROFILING "Whether or not debug mode will be profiled." OFF)
option(TIGER_TESTING "Whether or not Tiger should compile test binaries." OFF)

# Setting up profiling defines
if(TIGER_PROFILING)
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -o0 -Wall -W -fprofile-arcs -ftest-coverage")
  set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage")
endif()

# Add a Tiger Library
add_library(tiger
  tiger/topt.c
  tiger/talloc.c
  tiger/tbuffer.c
  tiger/tcode.c
  tiger/tenv.c
  tiger/tlex.c
  tiger/tparse.c
  tiger/tstate.c
)

# Add Tiger Compiler
add_executable(tc tiger/tc.c)
target_link_libraries(tc tiger)
install(TARGETS tc DESTINATION bin)

# Add Tiger Interpreter
# add_executable(tl tiger/tl.c)
# target_link_libraries(tl tiger)
# install(TARGETS tl DESTINATION bin)

# Linking to gcov for coverage
if(TIGER_PROFILING)
  target_link_libraries(tl gcov)
endif()

# Add the test that Tiger relies on
if(TIGER_TESTING)
  enable_testing()
  include(CTest)
  add_subdirectory(tests)
endif()

configure_file(${CMAKE_SOURCE_DIR}/docs/Doxyfile.in ${CMAKE_BINARY_DIR}/Doxyfile @ONLY)
