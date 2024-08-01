file(REMOVE_RECURSE
  "CMakeFiles/yara"
  "CMakeFiles/yara-complete"
  "yara/src/yara-stamp/yara-install"
  "yara/src/yara-stamp/yara-mkdir"
  "yara/src/yara-stamp/yara-download"
  "yara/src/yara-stamp/yara-update"
  "yara/src/yara-stamp/yara-patch"
  "yara/src/yara-stamp/yara-configure"
  "yara/src/yara-stamp/yara-build"
  "yara/src/yara-stamp/yara-configure-linux"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/yara.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
