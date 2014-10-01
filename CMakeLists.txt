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

set(TIGER_VERSION_MAJOR 0)
set(TIGER_VERSION_MINOR 0)
set(TIGER_VERSION_PATCH 0)
set(TIGER_VERSION
 "${TIGER_VERSION_MAJOR}.${TIGER_VERSION_MINOR}.${TIGER_VERSION_PATCH}"
)

################################################################################
# Project Initialization
################################################################################
project(tiger C)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

################################################################################
# Project Prerequisites
################################################################################
find_package(GMP REQUIRED)

################################################################################
# Project Options
################################################################################
option(TIGER_PROFILING "Whether or not debug mode will be profiled." OFF)
option(TIGER_TESTING "Whether or not Tiger should compile test binaries." OFF)

################################################################################
# Tiger Library
################################################################################
set(TIGER_SOURCE
  tiger/talloc.c
  tiger/tenv.c
  tiger/tlex.c
  tiger/tparse.c
)
set(TIGER_HEADERS
  tiger/talloc.h
  tiger/tenv.h
  tiger/tlex.h
  tiger/tparse.h
)
add_library(tiger ${TIGER_HEADERS} ${TIGER_SOURCE})
target_link_libraries(tiger ${GMP_LIBRARIES})
install(FILES ${TIGER_HEADERS} DESTINATION include)
install(TARGETS tiger ARCHIVE DESTINATION lib)

################################################################################
# Tiger Interpreter
################################################################################
set(TIGER_INTERPRETER_SOURCE
  tiger/tl.c
)
set(TIGER_INTERPRETER_HEADER
)
add_executable(tl ${TIGER_INTERPRETER_SOURCE} ${TIGER_INTERPRETER_HEADER})
target_link_libraries(tl tiger)
install(TARGETS tl DESTINATION bin)

################################################################################
# Profiling Configuration
################################################################################
if(TIGER_PROFILING)
  target_link_libraries(tl gcov)
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -o0 -Wall -W -fprofile-arcs -ftest-coverage")
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
