

set(command "/usr/bin/cmake;-Dmake=${make};-Dconfig=${config};-P;/home/sloan/dispatch/linux/retdec/build/external/src/rapidjson-project-stamp/rapidjson-project-download-Release-impl.cmake")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "/home/sloan/dispatch/linux/retdec/build/external/src/rapidjson-project-stamp/rapidjson-project-download-out.log"
  ERROR_FILE "/home/sloan/dispatch/linux/retdec/build/external/src/rapidjson-project-stamp/rapidjson-project-download-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  /home/sloan/dispatch/linux/retdec/build/external/src/rapidjson-project-stamp/rapidjson-project-download-*.log")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "rapidjson-project download command succeeded.  See also /home/sloan/dispatch/linux/retdec/build/external/src/rapidjson-project-stamp/rapidjson-project-download-*.log")
  message(STATUS "${msg}")
endif()