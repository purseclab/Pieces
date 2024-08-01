# Install script for directory: /home/sloan/dispatch/linux/retdec/deps

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
  include("/home/sloan/dispatch/linux/retdec/build/deps/capstone/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/deps/jsoncpp/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/deps/llvm/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/deps/openssl/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/deps/rapidjson/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/deps/tinyxml2/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/deps/elfio/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/deps/libdwarf/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/deps/pelib/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/deps/yaracpp/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/deps/yaramod/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/deps/whereami/cmake_install.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/deps/variant/cmake_install.cmake")

endif()

