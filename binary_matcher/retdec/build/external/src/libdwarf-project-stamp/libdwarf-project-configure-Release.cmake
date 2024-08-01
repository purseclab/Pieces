

set(command "/usr/bin/cmake;-DCMAKE_BUILD_TYPE=Release;-DCMAKE_INSTALL_PREFIX=/home/sloan/dispatch/linux/retdec/build/external/src/libdwarf-project-install;-DCMAKE_C_COMPILER=/usr/bin/cc;-DCMAKE_CXX_COMPILER=/usr/bin/c++;-GUnix Makefiles;/home/sloan/dispatch/linux/retdec/build/external/src/libdwarf-project")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "/home/sloan/dispatch/linux/retdec/build/external/src/libdwarf-project-stamp/libdwarf-project-configure-out.log"
  ERROR_FILE "/home/sloan/dispatch/linux/retdec/build/external/src/libdwarf-project-stamp/libdwarf-project-configure-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  /home/sloan/dispatch/linux/retdec/build/external/src/libdwarf-project-stamp/libdwarf-project-configure-*.log")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "libdwarf-project configure command succeeded.  See also /home/sloan/dispatch/linux/retdec/build/external/src/libdwarf-project-stamp/libdwarf-project-configure-*.log")
  message(STATUS "${msg}")
endif()
