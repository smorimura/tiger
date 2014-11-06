################################################################################
# Project: TigerScript
# File   : CMakeLists.txt
# Legal  : Copyright Trent Reed 2014, All rights reserved.
# About  : A file for setting max warnings.
# Usage  : include(FullWarnings.cmake) set max warnings/errors.
################################################################################

message(STATUS "Setting warnings for \"${CMAKE_C_COMPILER_ID}\"")
if("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic -std=c11 -Wall -Werror -Wextra")
elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4 /WX /D_CRT_SECURE_NO_WARNINGS")
else()
  message(FATAL_ERROR "No warnings configuration found for this compiler!")
endif()
