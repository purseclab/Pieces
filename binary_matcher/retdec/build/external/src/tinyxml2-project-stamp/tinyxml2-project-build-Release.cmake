

set(command "${make}")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "/home/sloan/dispatch/linux/retdec/build/external/src/tinyxml2-project-stamp/tinyxml2-project-build-out.log"
  ERROR_FILE "/home/sloan/dispatch/linux/retdec/build/external/src/tinyxml2-project-stamp/tinyxml2-project-build-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  /home/sloan/dispatch/linux/retdec/build/external/src/tinyxml2-project-stamp/tinyxml2-project-build-*.log")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "tinyxml2-project build command succeeded.  See also /home/sloan/dispatch/linux/retdec/build/external/src/tinyxml2-project-stamp/tinyxml2-project-build-*.log")
  message(STATUS "${msg}")
endif()
