#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "LLVMDemangle" for configuration "Release"
set_property(TARGET LLVMDemangle APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMDemangle PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMDemangle.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMDemangle )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMDemangle "${_IMPORT_PREFIX}/lib/libLLVMDemangle.a" )

# Import target "LLVMSupport" for configuration "Release"
set_property(TARGET LLVMSupport APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMSupport PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C;CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMSupport.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMSupport )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMSupport "${_IMPORT_PREFIX}/lib/libLLVMSupport.a" )

# Import target "LLVMTableGen" for configuration "Release"
set_property(TARGET LLVMTableGen APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMTableGen PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMTableGen.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMTableGen )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMTableGen "${_IMPORT_PREFIX}/lib/libLLVMTableGen.a" )

# Import target "llvm-tblgen" for configuration "Release"
set_property(TARGET llvm-tblgen APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(llvm-tblgen PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/llvm-tblgen"
  )

list(APPEND _IMPORT_CHECK_TARGETS llvm-tblgen )
list(APPEND _IMPORT_CHECK_FILES_FOR_llvm-tblgen "${_IMPORT_PREFIX}/bin/llvm-tblgen" )

# Import target "LLVMCore" for configuration "Release"
set_property(TARGET LLVMCore APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMCore PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMCore.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMCore )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMCore "${_IMPORT_PREFIX}/lib/libLLVMCore.a" )

# Import target "LLVMIRReader" for configuration "Release"
set_property(TARGET LLVMIRReader APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMIRReader PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMIRReader.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMIRReader )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMIRReader "${_IMPORT_PREFIX}/lib/libLLVMIRReader.a" )

# Import target "LLVMCodeGen" for configuration "Release"
set_property(TARGET LLVMCodeGen APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMCodeGen PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMCodeGen.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMCodeGen )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMCodeGen "${_IMPORT_PREFIX}/lib/libLLVMCodeGen.a" )

# Import target "LLVMBinaryFormat" for configuration "Release"
set_property(TARGET LLVMBinaryFormat APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMBinaryFormat PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMBinaryFormat.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMBinaryFormat )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMBinaryFormat "${_IMPORT_PREFIX}/lib/libLLVMBinaryFormat.a" )

# Import target "LLVMBitReader" for configuration "Release"
set_property(TARGET LLVMBitReader APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMBitReader PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMBitReader.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMBitReader )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMBitReader "${_IMPORT_PREFIX}/lib/libLLVMBitReader.a" )

# Import target "LLVMBitWriter" for configuration "Release"
set_property(TARGET LLVMBitWriter APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMBitWriter PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMBitWriter.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMBitWriter )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMBitWriter "${_IMPORT_PREFIX}/lib/libLLVMBitWriter.a" )

# Import target "LLVMTransformUtils" for configuration "Release"
set_property(TARGET LLVMTransformUtils APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMTransformUtils PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMTransformUtils.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMTransformUtils )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMTransformUtils "${_IMPORT_PREFIX}/lib/libLLVMTransformUtils.a" )

# Import target "LLVMAggressiveInstCombine" for configuration "Release"
set_property(TARGET LLVMAggressiveInstCombine APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMAggressiveInstCombine PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMAggressiveInstCombine.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMAggressiveInstCombine )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMAggressiveInstCombine "${_IMPORT_PREFIX}/lib/libLLVMAggressiveInstCombine.a" )

# Import target "LLVMInstCombine" for configuration "Release"
set_property(TARGET LLVMInstCombine APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMInstCombine PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMInstCombine.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMInstCombine )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMInstCombine "${_IMPORT_PREFIX}/lib/libLLVMInstCombine.a" )

# Import target "LLVMScalarOpts" for configuration "Release"
set_property(TARGET LLVMScalarOpts APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMScalarOpts PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMScalarOpts.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMScalarOpts )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMScalarOpts "${_IMPORT_PREFIX}/lib/libLLVMScalarOpts.a" )

# Import target "LLVMipo" for configuration "Release"
set_property(TARGET LLVMipo APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMipo PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMipo.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMipo )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMipo "${_IMPORT_PREFIX}/lib/libLLVMipo.a" )

# Import target "LLVMAnalysis" for configuration "Release"
set_property(TARGET LLVMAnalysis APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMAnalysis PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMAnalysis.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMAnalysis )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMAnalysis "${_IMPORT_PREFIX}/lib/libLLVMAnalysis.a" )

# Import target "LLVMMC" for configuration "Release"
set_property(TARGET LLVMMC APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMMC PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMMC.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMMC )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMMC "${_IMPORT_PREFIX}/lib/libLLVMMC.a" )

# Import target "LLVMMCParser" for configuration "Release"
set_property(TARGET LLVMMCParser APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMMCParser PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMMCParser.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMMCParser )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMMCParser "${_IMPORT_PREFIX}/lib/libLLVMMCParser.a" )

# Import target "LLVMObject" for configuration "Release"
set_property(TARGET LLVMObject APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMObject PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMObject.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMObject )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMObject "${_IMPORT_PREFIX}/lib/libLLVMObject.a" )

# Import target "LLVMTarget" for configuration "Release"
set_property(TARGET LLVMTarget APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMTarget PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMTarget.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMTarget )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMTarget "${_IMPORT_PREFIX}/lib/libLLVMTarget.a" )

# Import target "LLVMAsmParser" for configuration "Release"
set_property(TARGET LLVMAsmParser APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMAsmParser PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMAsmParser.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMAsmParser )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMAsmParser "${_IMPORT_PREFIX}/lib/libLLVMAsmParser.a" )

# Import target "LLVMPasses" for configuration "Release"
set_property(TARGET LLVMPasses APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMPasses PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMPasses.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMPasses )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMPasses "${_IMPORT_PREFIX}/lib/libLLVMPasses.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
