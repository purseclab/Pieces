

set(command "/usr/bin/cmake;-Dmake=${make};-Dconfig=${config};-P;/home/sloan/dispatch/linux/retdec/build/external/src/elfio-project-stamp/elfio-project-download-Release-impl.cmake")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "/home/sloan/dispatch/linux/retdec/build/external/src/elfio-project-stamp/elfio-project-download-out.log"
  ERROR_FILE "/home/sloan/dispatch/linux/retdec/build/external/src/elfio-project-stamp/elfio-project-download-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  /home/sloan/dispatch/linux/retdec/build/external/src/elfio-project-stamp/elfio-project-download-*.log")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "elfio-project download command succeeded.  See also /home/sloan/dispatch/linux/retdec/build/external/src/elfio-project-stamp/elfio-project-download-*.log")
  message(STATUS "${msg}")
endif()