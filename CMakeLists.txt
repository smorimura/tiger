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
set(TIGER_VERSION "${TIGER_VERSION_MAJOR}.${TIGER_VERSION_MINOR}${TIGER_VERSION_PATCH}")
include("${CMAKE_SOURCE_DIR}/cmake/FullWarnings.cmake")
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################################################################
# Project Options
################################################################################
option(TIGER_PROFILING "Whether or not debug mode will be profiled." OFF)
option(TIGER_TESTING "Whether or not Tiger should compile test binaries." OFF)

################################################################################
# libtiger
################################################################################
add_library(tgr
  tiger/topt.c tiger/topt.h
  tiger/talloc.c tiger/talloc.h
  tiger/tbuffer.c tiger/tbuffer.h
  tiger/tcode.c tiger/tcode.h
  tiger/tenv.c tiger/tenv.h
  tiger/tlex.c tiger/tlex.h
  tiger/tparse.c tiger/tparse.h
  tiger/tstate.c tiger/tstate.h
)
install(TARGETS tgr DESTINATION lib)
set_property( TARGET tgr PROPERTY FOLDER "tiger" )

################################################################################
# tc - TigerScript Compiler
################################################################################
add_executable(tc tiger/tc.c)
target_link_libraries(tc tgr)
install(TARGETS tc DESTINATION bin)
set_property( TARGET tc PROPERTY FOLDER "tiger" )

################################################################################
# tl - TigerScript Command-Line Interpreter
################################################################################
add_executable(tl tiger/tl.c)
target_link_libraries(tl tgr)
install(TARGETS tl DESTINATION bin)
set_property( TARGET tl PROPERTY FOLDER "tiger" )

################################################################################
# td - TigerScript Disassembler
################################################################################
add_executable(td tiger/td.c)
target_link_libraries(td tgr)
install(TARGETS td DESTINATION bin)
set_property( TARGET td PROPERTY FOLDER "tiger" )

################################################################################
# Profiling Flags and Definitions
################################################################################
if(TIGER_PROFILING)
  target_link_libraries(tl gcov)
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -o0 -fprofile-arcs -ftest-coverage")
  set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage")
endif()

################################################################################
# Testing Configuration
################################################################################
if(TIGER_TESTING)
  enable_testing()
  include(CTest)
  add_subdirectory(tests)
endif()

################################################################################
# Doxygen Configuration
################################################################################
configure_file(${CMAKE_SOURCE_DIR}/docs/Doxyfile.in ${CMAKE_BINARY_DIR}/Doxyfile @ONLY)

################################################################################
# Packaging Configuration
################################################################################
set(CPACK_PACKAGE_NAME "Tiger")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "An ANSI-C scripting language research project.")
set(CPACK_PACKAGE_VENDOR "Trent Reed")
set(CPACK_PACKAGE_VERSION_MAJOR "${TIGER_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${TIGER_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${TIGER_VERSION_PATCH}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "tiger-${TIGER_VERSION}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "tiger")
include (CPack)
