file(REMOVE_RECURSE
  "CMakeFiles/llvm-project"
  "CMakeFiles/llvm-project-complete"
  "../../external/src/llvm-project-stamp/llvm-project-install"
  "../../external/src/llvm-project-stamp/llvm-project-mkdir"
  "../../external/src/llvm-project-stamp/llvm-project-download"
  "../../external/src/llvm-project-stamp/llvm-project-update"
  "../../external/src/llvm-project-stamp/llvm-project-patch"
  "../../external/src/llvm-project-stamp/llvm-project-configure"
  "../../external/src/llvm-project-stamp/llvm-project-build"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/llvm-project.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
