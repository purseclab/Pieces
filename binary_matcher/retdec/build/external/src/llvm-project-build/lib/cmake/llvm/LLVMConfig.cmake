# This file provides information and services to the final user.


# LLVM_BUILD_* values available only from LLVM build tree.
set(LLVM_BUILD_BINARY_DIR "/home/sloan/dispatch/linux/retdec/build/external/src/llvm-project-build")
set(LLVM_BUILD_LIBRARY_DIR "/home/sloan/dispatch/linux/retdec/build/external/src/llvm-project-build/./lib")
set(LLVM_BUILD_MAIN_INCLUDE_DIR "/home/sloan/dispatch/linux/retdec/build/external/src/llvm-project/include")
set(LLVM_BUILD_MAIN_SRC_DIR "/home/sloan/dispatch/linux/retdec/build/external/src/llvm-project")


set(LLVM_VERSION_MAJOR 8)
set(LLVM_VERSION_MINOR 0)
set(LLVM_VERSION_PATCH 0)
set(LLVM_VERSION_SUFFIX )
set(LLVM_PACKAGE_VERSION 8.0.0)

set(LLVM_BUILD_TYPE Release)

set(LLVM_COMMON_DEPENDS )

set(LLVM_AVAILABLE_LIBS LLVMDemangle;LLVMSupport;LLVMTableGen;LLVMCore;LLVMIRReader;LLVMCodeGen;LLVMBinaryFormat;LLVMBitReader;LLVMBitWriter;LLVMTransformUtils;LLVMAggressiveInstCombine;LLVMInstCombine;LLVMScalarOpts;LLVMipo;LLVMAnalysis;LLVMMC;LLVMMCParser;LLVMObject;LLVMTarget;LLVMAsmParser;LLVMPasses)



set(LLVM_DYLIB_COMPONENTS all)

set(LLVM_ALL_TARGETS AArch64;AMDGPU;ARM;BPF;Hexagon;Lanai;Mips;MSP430;NVPTX;PowerPC;Sparc;SystemZ;WebAssembly;X86;XCore)

set(LLVM_TARGETS_TO_BUILD X86)

set(LLVM_TARGETS_WITH_JIT X86;PowerPC;AArch64;ARM;Mips;SystemZ)


set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMDemangle )
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMSupport LLVMDemangle)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMTableGen LLVMSupport)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMCore LLVMBinaryFormat;LLVMSupport)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMIRReader LLVMAsmParser;LLVMBitReader;LLVMCore;LLVMSupport)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMCodeGen LLVMAnalysis;LLVMBitReader;LLVMBitWriter;LLVMCore;LLVMMC;LLVMScalarOpts;LLVMSupport;LLVMTarget;LLVMTransformUtils)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMBinaryFormat LLVMSupport)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMBitReader LLVMCore;LLVMSupport)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMBitWriter LLVMAnalysis;LLVMCore;LLVMMC;LLVMObject;LLVMSupport)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMTransformUtils LLVMAnalysis;LLVMCore;LLVMSupport)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMAggressiveInstCombine LLVMAnalysis;LLVMCore;LLVMSupport;LLVMTransformUtils)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMInstCombine LLVMAnalysis;LLVMCore;LLVMSupport;LLVMTransformUtils)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMScalarOpts LLVMAggressiveInstCombine;LLVMAnalysis;LLVMCore;LLVMInstCombine;LLVMSupport;LLVMTransformUtils)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMipo LLVMAggressiveInstCombine;LLVMAnalysis;LLVMBitReader;LLVMBitWriter;LLVMCore;LLVMIRReader;LLVMInstCombine;LLVMObject;LLVMScalarOpts;LLVMSupport;LLVMTransformUtils)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMAnalysis LLVMBinaryFormat;LLVMCore;LLVMObject;LLVMSupport)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMMC LLVMBinaryFormat;LLVMSupport)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMMCParser LLVMMC;LLVMSupport)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMObject LLVMBinaryFormat;LLVMBitReader;LLVMCore;LLVMMC;LLVMMCParser;LLVMSupport)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMTarget LLVMAnalysis;LLVMCore;LLVMMC;LLVMSupport)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMAsmParser LLVMBinaryFormat;LLVMCore;LLVMSupport)
set_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_LLVMPasses LLVMAggressiveInstCombine;LLVMAnalysis;LLVMCodeGen;LLVMCore;LLVMInstCombine;LLVMScalarOpts;LLVMSupport;LLVMTarget;LLVMTransformUtils;LLVMipo)

set(TARGET_TRIPLE "x86_64-unknown-linux-gnu")

set(LLVM_ABI_BREAKING_CHECKS WITH_ASSERTS)

set(LLVM_ENABLE_ASSERTIONS NO)

set(LLVM_ENABLE_EH OFF)

set(LLVM_ENABLE_RTTI OFF)

set(LLVM_ENABLE_TERMINFO OFF)

set(LLVM_ENABLE_THREADS ON)

set(LLVM_ENABLE_ZLIB ON)

set(LLVM_LIBXML2_ENABLED 0)

set(LLVM_ENABLE_DIA_SDK 0)

set(LLVM_NATIVE_ARCH X86)

set(LLVM_ENABLE_PIC ON)

set(LLVM_BUILD_32_BITS OFF)

if (NOT "-lpthread" STREQUAL "")
  set(LLVM_PTHREAD_LIB "-lpthread")
endif()

set(LLVM_ENABLE_PLUGINS ON)
set(LLVM_EXPORT_SYMBOLS_FOR_PLUGINS OFF)
set(LLVM_PLUGIN_EXT .so)

set(LLVM_ON_UNIX 1)

set(LLVM_LIBDIR_SUFFIX )

set(LLVM_INCLUDE_DIRS "/home/sloan/dispatch/linux/retdec/build/external/src/llvm-project/include;/home/sloan/dispatch/linux/retdec/build/external/src/llvm-project-build/include")
set(LLVM_LIBRARY_DIRS "/home/sloan/dispatch/linux/retdec/build/external/src/llvm-project-build/./lib")

# These variables are duplicated, but they must match the LLVM variables of the
# same name. The variables ending in "S" could some day become lists, and are
# preserved for convention and compatibility.
set(LLVM_INCLUDE_DIR "/home/sloan/dispatch/linux/retdec/build/external/src/llvm-project/include;/home/sloan/dispatch/linux/retdec/build/external/src/llvm-project-build/include")
set(LLVM_LIBRARY_DIR "/home/sloan/dispatch/linux/retdec/build/external/src/llvm-project-build/./lib")

set(LLVM_DEFINITIONS "-D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS")
set(LLVM_CMAKE_DIR "/home/sloan/dispatch/linux/retdec/build/external/src/llvm-project/cmake/modules")
set(LLVM_BINARY_DIR "/home/sloan/dispatch/linux/retdec/build/external/src/llvm-project-build")
set(LLVM_TOOLS_BINARY_DIR "/home/sloan/dispatch/linux/retdec/build/external/src/llvm-project-build/./bin")
set(LLVM_TOOLS_INSTALL_DIR "bin")
set(LLVM_HAVE_OPT_VIEWER_MODULES 0)

if(NOT TARGET LLVMSupport)
  set(LLVM_EXPORTED_TARGETS "LLVMDemangle;LLVMSupport;LLVMTableGen;llvm-tblgen;LLVMCore;LLVMIRReader;LLVMCodeGen;LLVMBinaryFormat;LLVMBitReader;LLVMBitWriter;LLVMTransformUtils;LLVMAggressiveInstCombine;LLVMInstCombine;LLVMScalarOpts;LLVMipo;LLVMAnalysis;LLVMMC;LLVMMCParser;LLVMObject;LLVMTarget;LLVMAsmParser;LLVMPasses;")
  include("/home/sloan/dispatch/linux/retdec/build/external/src/llvm-project-build/lib/cmake/llvm/LLVMExports.cmake")
  include("/home/sloan/dispatch/linux/retdec/build/external/src/llvm-project-build/lib/cmake/llvm/LLVMBuildTreeOnlyTargets.cmake")
endif()

# By creating intrinsics_gen here, subprojects that depend on LLVM's
# tablegen-generated headers can always depend on this target whether building
# in-tree with LLVM or not.
if(NOT TARGET intrinsics_gen)
  add_custom_target(intrinsics_gen)
endif()

set_property(GLOBAL PROPERTY LLVM_TARGETS_CONFIGURED On)
include(${LLVM_CMAKE_DIR}/LLVM-Config.cmake)
