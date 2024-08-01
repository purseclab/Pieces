

set(command "/usr/bin/cmake;-DCMAKE_BUILD_TYPE=Release;-DCMAKE_C_COMPILER=/usr/bin/cc;-DCMAKE_CXX_COMPILER=/usr/bin/c++;-GUnix Makefiles;/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-stamp/yaramod-project-configure-out.log"
  ERROR_FILE "/home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-stamp/yaramod-project-configure-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  /home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-stamp/yaramod-project-configure-*.log")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "yaramod-project configure command succeeded.  See also /home/sloan/dispatch/linux/retdec/build/external/src/yaramod-project-stamp/yaramod-project-configure-*.log")
  message(STATUS "${msg}")
endif()
