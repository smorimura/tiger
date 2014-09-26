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

file(GLOB TIGER_TEST_INPUT "${CMAKE_CURRENT_SOURCE_DIR}/*.in")
foreach(TIGER_TEST_IN ${TIGER_TEST_INPUT})
  get_filename_component(TIGER_TEST_NAME ${TIGER_TEST_IN} NAME_WE)
  message("Added test: ${TIGER_TEST_NAME}")
  add_test(NAME TIGER_TEST_NAME COMMAND tl < ${TIGER_TEST_IN})
endforeach()
