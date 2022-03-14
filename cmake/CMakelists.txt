string (TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)
configure_file (project-config.cmake.in
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME_LOWER}-config.cmake" @ONLY)
configure_file (project-config-version.cmake.in
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME_LOWER}-config-version.cmake" @ONLY)

install (FILES
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME_LOWER}-config.cmake"
  DESTINATION "cmake")
install (FILES
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME_LOWER}-config-version.cmake"
  DESTINATION "cmake")