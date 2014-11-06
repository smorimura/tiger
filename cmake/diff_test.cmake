################################################################################
# Project: TigerScript
# Legal  : Copyright Trent Reed 2014, All rights reserved.
# About  : A script for add_diff_test which will run a command and diff output.
################################################################################

# Check input variables
if( NOT command )
  message( FATAL_ERROR "Variable command not defined" )
endif()
if( NOT blessed )
  message( FATAL_ERROR "Variable blessed not defined" )
endif()
if( NOT input )
  message( FATAL_ERROR "Variable input not defined" )
endif()
if( NOT output )
  message( FATAL_ERROR "Variable output not defined" )
endif()

# Run the command
execute_process(
  COMMAND ${command} -f ${input} > ${output}
  RESULT_VARIABLE run_result
)

if( run_result )
   message( SEND_ERROR "Trouble running script: ${input}" )
else()

  execute_process(
    COMMAND ${CMAKE_COMMAND} "-E" "compare_files" ${output} ${blessed}
    RESULT_VARIABLE diff_result
  )
  if( diff_result )
    message( SEND_ERROR "Differences found between files: ${output} ${blessed}")
  endif()

endif()
