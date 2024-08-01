

set(command "/usr/bin/cmake;-DCMAKE_BUILD_TYPE=Release;-DCAPSTONE_BUILD_STATIC=ON;-DCAPSTONE_BUILD_SHARED=OFF;-DCAPSTONE_BUILD_STATIC_RUNTIME=;-DCAPSTONE_BUILD_TESTS=OFF;-DCAPSTONE_X86_ATT_DISABLE=OFF;-DCAPSTONE_ARM_SUPPORT=ON;-DCAPSTONE_MIPS_SUPPORT=ON;-DCAPSTONE_PPC_SUPPORT=ON;-DCAPSTONE_X86_SUPPORT=ON;-DCAPSTONE_ARM64_SUPPORT=ON;-DCAPSTONE_M68K_SUPPORT=OFF;-DCAPSTONE_SPARC_SUPPORT=OFF;-DCAPSTONE_SYSZ_SUPPORT=OFF;-DCAPSTONE_XCORE_SUPPORT=OFF;-DCAPSTONE_TMS320C64X_SUPPORT=OFF;-DCAPSTONE_M680X_SUPPORT=OFF;-DCMAKE_C_COMPILER=/usr/bin/cc;-DCMAKE_CXX_COMPILER=/usr/bin/c++;-GUnix Makefiles;/home/sloan/dispatch/linux/retdec/build/external/src/capstone-project")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "/home/sloan/dispatch/linux/retdec/build/external/src/capstone-project-stamp/capstone-project-configure-out.log"
  ERROR_FILE "/home/sloan/dispatch/linux/retdec/build/external/src/capstone-project-stamp/capstone-project-configure-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  /home/sloan/dispatch/linux/retdec/build/external/src/capstone-project-stamp/capstone-project-configure-*.log")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "capstone-project configure command succeeded.  See also /home/sloan/dispatch/linux/retdec/build/external/src/capstone-project-stamp/capstone-project-configure-*.log")
  message(STATUS "${msg}")
endif()
