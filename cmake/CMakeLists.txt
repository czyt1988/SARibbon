#string (TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)
set(SA_LIB_NAME SARibbonBar)
string(TOLOWER ${SA_LIB_NAME} SA_LIB_NAME_LOWER)
configure_file (project-config.cmake.in
  "${PROJECT_BINARY_DIR}/${SA_LIB_NAME_LOWER}-config.cmake" @ONLY)

configure_file (project-config-version.cmake.in
  "${PROJECT_BINARY_DIR}/${SA_LIB_NAME_LOWER}-config-version.cmake" @ONLY)

install (FILES
  "${PROJECT_BINARY_DIR}/${SA_LIB_NAME_LOWER}-config.cmake"
  DESTINATION lib/cmake/${SA_LIB_NAME}
)

install (FILES
  "${PROJECT_BINARY_DIR}/${SA_LIB_NAME_LOWER}-config-version.cmake"
  DESTINATION lib/cmake/${SA_LIB_NAME}
)