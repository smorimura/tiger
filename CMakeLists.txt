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

# Options for configuring Tiger
option(TIGER_PROFILING "Whether or not debug mode will be profiled." OFF)
option(TIGER_TESTING "Whether or not Tiger should compile test binaries." OFF)

# Setting up profiling defines
if(TIGER_PROFILING)
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -o0 -Wall -W -fprofile-arcs -ftest-coverage")
  set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage")
endif()

# A sample application
add_executable(sample tiger/main.c)
install(TARGETS sample DESTINATION bin)

# Linking to gcov for coverage
if(TIGER_PROFILING)
  target_link_libraries(sample gcov)
endif()

# Add the test that Tiger relies on
if(TIGER_TESTING)
  enable_testing()
  include(CTest)
  add_test(NAME Sample COMMAND sample)
endif()

configure_file(${CMAKE_SOURCE_DIR}/docs/Doxyfile.in ${CMAKE_BINARY_DIR}/Doxyfile @ONLY)
