# Install script for directory: /home/sloan/dispatch/linux/retdec/build/external/src/capstone-project

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/capstone" TYPE FILE FILES
    "/home/sloan/dispatch/linux/retdec/build/external/src/capstone-project/include/capstone/arm64.h"
    "/home/sloan/dispatch/linux/retdec/build/external/src/capstone-project/include/capstone/arm.h"
    "/home/sloan/dispatch/linux/retdec/build/external/src/capstone-project/include/capstone/capstone.h"
    "/home/sloan/dispatch/linux/retdec/build/external/src/capstone-project/include/capstone/evm.h"
    "/home/sloan/dispatch/linux/retdec/build/external/src/capstone-project/include/capstone/mips.h"
    "/home/sloan/dispatch/linux/retdec/build/external/src/capstone-project/include/capstone/ppc.h"
    "/home/sloan/dispatch/linux/retdec/build/external/src/capstone-project/include/capstone/x86.h"
    "/home/sloan/dispatch/linux/retdec/build/external/src/capstone-project/include/capstone/sparc.h"
    "/home/sloan/dispatch/linux/retdec/build/external/src/capstone-project/include/capstone/systemz.h"
    "/home/sloan/dispatch/linux/retdec/build/external/src/capstone-project/include/capstone/xcore.h"
    "/home/sloan/dispatch/linux/retdec/build/external/src/capstone-project/include/capstone/m68k.h"
    "/home/sloan/dispatch/linux/retdec/build/external/src/capstone-project/include/capstone/tms320c64x.h"
    "/home/sloan/dispatch/linux/retdec/build/external/src/capstone-project/include/capstone/m680x.h"
    "/home/sloan/dispatch/linux/retdec/build/external/src/capstone-project/include/capstone/platform.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/sloan/dispatch/linux/retdec/build/external/src/capstone-project-build/libcapstone.a")
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/sloan/dispatch/linux/retdec/build/external/src/capstone-project-build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
