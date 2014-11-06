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

################################################################################
# Project Initialization
################################################################################
project(tiger C)
set(TIGER_VERSION_MAJOR 0)
set(TIGER_VERSION_MINOR 0)
set(TIGER_VERSION_PATCH 2)
include("${CMAKE_SOURCE_DIR}/cmake/FullWarnings.cmake")
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

################################################################################
# Options
################################################################################
option(TIGER_PROFILING "Whether or not debug mode will be profiled." OFF)
option(TIGER_TESTING "Whether or not Tiger should compile test binaries." OFF)

################################################################################
# libtiger
################################################################################
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

################################################################################
# tc - TigerScript Compiler
################################################################################
add_executable(tc tiger/tc.c)
target_link_libraries(tc tiger)
install(TARGETS tc DESTINATION bin)

################################################################################
# tl - TigerScript Command-Line Interpreter
################################################################################
add_executable(tl tiger/tl.c)
target_link_libraries(tl tiger)
install(TARGETS tl DESTINATION bin)

################################################################################
# td - TigerScript Disassembler
################################################################################
add_executable(td tiger/td.c)
target_link_libraries(td tiger)
install(TARGETS td DESTINATION bin)

################################################################################
# Profiling Flags and Definitions
################################################################################
if(TIGER_PROFILING)
  target_link_libraries(tl gcov)
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -o0 -fprofile-arcs -ftest-coverage")
  set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage")
endif()

################################################################################
# Testing
################################################################################
if(TIGER_TESTING)
  enable_testing()
  include(CTest)
  add_subdirectory(tests)
endif()

# Configure Doxyfile for Nightlies
configure_file(${CMAKE_SOURCE_DIR}/docs/Doxyfile.in ${CMAKE_BINARY_DIR}/Doxyfile @ONLY)
