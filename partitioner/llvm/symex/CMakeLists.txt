
if(DEFINED IN_SOURCE_BUILD)
    set(LLVM_LINK_COMPONENTS BitWriter Core IPO IrReader InstCombine Instrumentation Target Linker Analysis ScalarOpts Support Svf Cudd)
    add_llvm_tool( symex symex.cpp  util.cpp)
else()
    add_executable( symex symex.cpp util.cpp)

    target_link_libraries( symex Svf Cudd ${llvm_libs} )

    set_target_properties( symex PROPERTIES
                           RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
endif()
