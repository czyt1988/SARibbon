cmake_minimum_required(VERSION 3.5)
project(SARibbonBar LANGUAGES CXX VERSION ${SARIBBON_VERSION})
set(LIB_NAME SARibbonBar)

set(CMAKE_INCLUDE_CURRENT_DIR ON)


# header files
# cn:头文件
SET(SARIBBON_HEADER_FILES
    SAFramelessHelper.h
    SARibbonStyleOption.h
    SARibbonActionsManager.h
    SARibbonBar.h
    SARibbonCustomizeData.h
    SARibbonCustomizeDialog.h
    SARibbonCustomizeWidget.h
    SARibbonMainWindow.h
    SAWindowButtonGroup.h
    SARibbonApplicationButton.h
    SARibbonTabBar.h
    SARibbonCategory.h
    SARibbonContextCategory.h
    SARibbonPannel.h
    SARibbonToolButton.h
    SARibbonMenu.h
    SARibbonGlobal.h
    SARibbonPannelOptionButton.h
    SARibbonSeparatorWidget.h
    SARibbonCategoryLayout.h
    SARibbonGallery.h
    SARibbonControlButton.h
    SARibbonGalleryGroup.h
    SARibbonGalleryItem.h
    SARibbonComboBox.h
    SARibbonElementFactory.h
    SARibbonElementManager.h
    SARibbonDrawHelper.h
    SARibbonLineEdit.h
    SARibbonCheckBox.h
    SARibbonButtonGroupWidget.h
    SARibbonStackedWidget.h
    SARibbonQuickAccessBar.h
    SARibbonCtrlContainer.h
    SARibbonPannelLayout.h
    SARibbonPannelItem.h
    SARibbonLineWidgetContainer.h
    SARibbonColorToolButton.h
)

# source files
# cn:cpp文件
SET(SARIBBON_SOURCE_FILES
    SAFramelessHelper.cpp
    SARibbonStyleOption.cpp
    SARibbonActionsManager.cpp
    SARibbonBar.cpp
    SARibbonCustomizeData.cpp
    SARibbonCustomizeDialog.cpp
    SARibbonCustomizeWidget.cpp
    SARibbonMainWindow.cpp
    SAWindowButtonGroup.cpp
    SARibbonApplicationButton.cpp
    SARibbonTabBar.cpp
    SARibbonCategory.cpp
    SARibbonContextCategory.cpp
    SARibbonPannel.cpp
    SARibbonToolButton.cpp
    SARibbonMenu.cpp
    SARibbonPannelOptionButton.cpp
    SARibbonSeparatorWidget.cpp
    SARibbonCategoryLayout.cpp
    SARibbonGallery.cpp
    SARibbonControlButton.cpp
    SARibbonGalleryGroup.cpp
    SARibbonGalleryItem.cpp
    SARibbonComboBox.cpp
    SARibbonElementFactory.cpp
    SARibbonElementManager.cpp
    SARibbonDrawHelper.cpp
    SARibbonLineEdit.cpp
    SARibbonCheckBox.cpp
    SARibbonButtonGroupWidget.cpp
    SARibbonStackedWidget.cpp
    SARibbonQuickAccessBar.cpp
    SARibbonCtrlContainer.cpp
    SARibbonPannelLayout.cpp
    SARibbonPannelItem.cpp
    SARibbonLineWidgetContainer.cpp
    SARibbonColorToolButton.cpp
)
# 颜色组件SAColorWidgets
# https://github.com/czyt1988/SAColorWidgets
SET(SACOLOR_HEADER_FILES
    ./colorWidgets/SAColorWidgetsGlobal.h
    ./colorWidgets/SAColorToolButton.h
    ./colorWidgets/SAColorGridWidget.h
    ./colorWidgets/SAColorPaletteGridWidget.h
    ./colorWidgets/SAColorMenu.h
)
SET(SACOLOR_SOURCE_FILES
    ./colorWidgets/SAColorToolButton.cpp
    ./colorWidgets/SAColorGridWidget.cpp
    ./colorWidgets/SAColorPaletteGridWidget.cpp
    ./colorWidgets/SAColorMenu.cpp
)
# resource files
# cn:资源文件
SET(SARIBBON_RESOURCE_FILES
    resource.qrc
)

add_library(${LIB_NAME} SHARED
    ${SARIBBON_HEADER_FILES}
    ${SARIBBON_SOURCE_FILES}
    ${SACOLOR_HEADER_FILES}
    ${SACOLOR_SOURCE_FILES}
    ${SARIBBON_RESOURCE_FILES}
)


target_link_libraries(${LIB_NAME} PUBLIC
                      Qt${QT_VERSION_MAJOR}::Core
                      Qt${QT_VERSION_MAJOR}::Gui
                      Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(${LIB_NAME} PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
    CXX_EXTENSIONS OFF
    DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
    VERSION ${SARIBBON_VERSION}
    EXPORT_NAME ${LIB_NAME}
    ARCHIVE_OUTPUT_DIRECTORY "${SARIBBON_BIN_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${SARIBBON_BIN_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${SARIBBON_BIN_DIR}"
)
# 构建库定义的宏
target_compile_definitions(${LIB_NAME} PRIVATE SA_RIBBON_BAR_MAKE_LIB)
# 使用SAColorWidgets但不作为库使用定义的宏
target_compile_definitions(${LIB_NAME} PRIVATE SA_COLOR_WIDGETS_NO_DLL)


target_include_directories(${LIB_NAME} PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
# ------------------------------- install script -------------------------------
set(SARIBBON_INSTALL_DIR SARibbon)
set(CMAKE_INSTALL_PREFIX ${SARIBBON_BIN_DIR})
# Generate library version info which will generate xxxConfigVersion.cmake,
# the ${PACKAGE_VERSION} is the version defined in project()
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${LIB_NAME}ConfigVersion.cmake"
    VERSION ${VERSION_SHORT}
    COMPATIBILITY SameMajorVersion
)
install(FILES
    ${SARIBBON_HEADER_FILES}
    DESTINATION include/${SARIBBON_INSTALL_DIR}
    COMPONENT headers
)
install(FILES
    ${SACOLOR_HEADER_FILES}
    DESTINATION include/${SARIBBON_INSTALL_DIR}/colorWidgets
    COMPONENT headers
)
install(TARGETS ${LIB_NAME}
    EXPORT SARibbonTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include/${SARIBBON_INSTALL_DIR}
)
install(EXPORT SARibbonTargets
    FILE SARibbonTargets.cmake
    DESTINATION lib/cmake/${SARIBBON_INSTALL_DIR}
)

if(WIN32)
	create_win32_resource_version(
		TARGET ${LIB_NAME}
		FILENAME ${LIB_NAME}
		EXT "dll"
		DESCRIPTION "Ribbon control library for Qt"
	)
endif()
