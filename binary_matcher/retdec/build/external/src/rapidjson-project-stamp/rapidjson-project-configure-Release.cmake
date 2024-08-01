

set(command "/usr/bin/cmake;-DCMAKE_BUILD_TYPE=Release;-DRAPIDJSON_BUILD_DOC=OFF;-DRAPIDJSON_BUILD_EXAMPLES=OFF;-DRAPIDJSON_BUILD_TESTS=OFF;-DCMAKE_C_COMPILER=/usr/bin/cc;-DCMAKE_CXX_COMPILER=/usr/bin/c++;-GUnix Makefiles;/home/sloan/dispatch/linux/retdec/build/external/src/rapidjson-project")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "/home/sloan/dispatch/linux/retdec/build/external/src/rapidjson-project-stamp/rapidjson-project-configure-out.log"
  ERROR_FILE "/home/sloan/dispatch/linux/retdec/build/external/src/rapidjson-project-stamp/rapidjson-project-configure-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  /home/sloan/dispatch/linux/retdec/build/external/src/rapidjson-project-stamp/rapidjson-project-configure-*.log")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "rapidjson-project configure command succeeded.  See also /home/sloan/dispatch/linux/retdec/build/external/src/rapidjson-project-stamp/rapidjson-project-configure-*.log")
  message(STATUS "${msg}")
endif()
