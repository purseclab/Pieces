# Install script for directory: /home/sloan/dispatch/linux/retdec/src

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

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/sloan/dispatch/linux/retdec/build/src/ar-extractor/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/src/ar-extractortool/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/src/bin2llvmir/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/src/bin2llvmirtool/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/src/bin2pat/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/src/capstone2llvmir/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/src/config/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/src/configtool/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/src/cpdetect/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/src/crypto/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/src/ctypes/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/src/ctypesparser/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/src/debugformat/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/src/demangler/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/src/dwarfparser/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/src/fileformat/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/src/fileinfo/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/src/idr2pat/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/src/llvm-support/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/src/llvmir2hll/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/src/llvmir2hlltool/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/src/loader/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/src/macho-extractor/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/src/macho-extractortool/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/src/pat2yara/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/src/patterngen/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/src/pdbparser/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/src/rtti-finder/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/src/stacofin/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/src/stacofintool/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/src/unpacker/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/src/unpackertool/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/src/utils/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/src/getsig/cmake_install.cmake")

endif()

