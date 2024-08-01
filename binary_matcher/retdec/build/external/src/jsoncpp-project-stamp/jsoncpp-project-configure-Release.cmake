

set(command "/usr/bin/cmake;-DCMAKE_BUILD_TYPE=Release;-DCMAKE_C_COMPILER=/usr/bin/cc;-DCMAKE_CXX_COMPILER=/usr/bin/c++;-DJSONCPP_WITH_TESTS=OFF;-DJSONCPP_WITH_POST_BUILD_UNITTEST=OFF;-DJSONCPP_WITH_PKGCONFIG_SUPPORT=OFF;-DBUILD_SHARED_LIBS=OFF;-GUnix Makefiles;/home/sloan/dispatch/linux/retdec/build/external/src/jsoncpp-project")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "/home/sloan/dispatch/linux/retdec/build/external/src/jsoncpp-project-stamp/jsoncpp-project-configure-out.log"
  ERROR_FILE "/home/sloan/dispatch/linux/retdec/build/external/src/jsoncpp-project-stamp/jsoncpp-project-configure-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  /home/sloan/dispatch/linux/retdec/build/external/src/jsoncpp-project-stamp/jsoncpp-project-configure-*.log")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "jsoncpp-project configure command succeeded.  See also /home/sloan/dispatch/linux/retdec/build/external/src/jsoncpp-project-stamp/jsoncpp-project-configure-*.log")
  message(STATUS "${msg}")
endif()
