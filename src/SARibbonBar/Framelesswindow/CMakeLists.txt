project(Framelesswindow)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(FRAMELESSWINDOW_LIB_NAME Framelesswindow)

find_package(Qt5 REQUIRED COMPONENTS  Widgets)

file(GLOB _header "${PROJECT_SOURCE_DIR}/*.h")
file(GLOB _source "${PROJECT_SOURCE_DIR}/*.cpp")

add_library(${PROJECT_NAME}
	STATIC
		${_header}
		${_source}
)

target_include_directories(${PROJECT_NAME}
	PUBLIC
		${PROJECT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME}
	PUBLIC
		Qt5::Widgets
)

set(FRAMLESSWINDOW_INSTALL_DIR Framelesswindow)
#set(CMAKE_INSTALL_PREFIX ${SARIBBON_BIN_DIR})
# Generate library version info which will generate xxxConfigVersion.cmake,
# the ${PACKAGE_VERSION} is the version defined in project()
#include(CMakePackageConfigHelpers)
#write_basic_package_version_file(
#		"${FRAMELESSWINDOW_LIB_NAME}ConfigVersion.cmake"
#		VERSION ${VERSION_SHORT}
#		COMPATIBILITY SameMajorVersion
#)
install(FILES
		${_header}
		DESTINATION include/${FRAMLESSWINDOW_INSTALL_DIR}
		COMPONENT headers
)

install(TARGETS ${FRAMELESSWINDOW_LIB_NAME}
		EXPORT FramelesswindowTargets
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		INCLUDES DESTINATION include/${FRAMLESSWINDOW_INSTALL_DIR}
)
install(EXPORT FramelesswindowTargets
		FILE FramelesswindowTargets.cmake
		DESTINATION lib/cmake/${FRAMLESSWINDOW_INSTALL_DIR}
)

if(WIN32)
	create_win32_resource_version(
			TARGET Framelesswindow
			FILENAME Framelesswindow
			EXT "dll"
			DESCRIPTION "Ribbon control library for Qt"
	)
endif()
