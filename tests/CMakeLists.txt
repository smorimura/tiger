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
  get_filename_component(TIGER_TEST_DIR ${TIGER_TEST_IN} DIRECTORY)
  get_filename_component(TIGER_TEST_NAME ${TIGER_TEST_IN} NAME_WE)
  set(TIGER_TEST_OUT "${TIGER_TEST_DIR}/${TIGER_TEST_NAME}.out")
  if(EXISTS ${TIGER_TEST_OUT})
    message("Added test: ${TIGER_TEST_NAME}")
    add_test( ${TIGER_TEST_NAME}
      ${CMAKE_COMMAND}
        -Dcommand=${CMAKE_BINARY_DIR}/tl
        -Dblessed=${TIGER_TEST_OUT}
        -Dinput=${TIGER_TEST_IN}
        -Doutput=${CMAKE_BINARY_DIR}/tests/${TIGER_TEST_NAME}.my
        -P ${CMAKE_SOURCE_DIR}/cmake/diff_test.cmake
      )
  endif()
endforeach()
