cmake_minimum_required(VERSION 3.5)
project(SARibbonBar LANGUAGES CXX VERSION ${SARIBBON_VERSION})
set(SARIBBON_LIB_NAME SARibbonBar)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

#################################################
# SAColorWidgets相关
# https://github.com/czyt1988/SAColorWidgets
#################################################
SET(SACOLOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/colorWidgets)
SET(SACOLOR_HEADER_FILES
    ${SACOLOR_DIR}/SAColorWidgetsGlobal.h
    ${SACOLOR_DIR}/SAColorToolButton.h
    ${SACOLOR_DIR}/SAColorGridWidget.h
    ${SACOLOR_DIR}/SAColorPaletteGridWidget.h
    ${SACOLOR_DIR}/SAColorMenu.h
)
SET(SACOLOR_SOURCE_FILES
    ${SACOLOR_DIR}/SAColorToolButton.cpp
    ${SACOLOR_DIR}/SAColorGridWidget.cpp
    ${SACOLOR_DIR}/SAColorPaletteGridWidget.cpp
    ${SACOLOR_DIR}/SAColorMenu.cpp
)

#################################################
# SARibbonBar相关
#################################################


# header files
# cn:头文件
SET(SARIBBON_HEADER_FILES
    SAFramelessHelper.h
    SARibbonActionsManager.h
    SARibbonBar.h
    SARibbonCustomizeData.h
    SARibbonCustomizeDialog.h
    SARibbonCustomizeWidget.h
    SARibbonMainWindow.h
    SARibbonSystemButtonBar.h
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
    SARibbonActionsManager.cpp
    SARibbonBar.cpp
    SARibbonCustomizeData.cpp
    SARibbonCustomizeDialog.cpp
    SARibbonCustomizeWidget.cpp
    SARibbonMainWindow.cpp
    SARibbonSystemButtonBar.cpp
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

# resource files
# cn:资源文件
SET(SARIBBON_RESOURCE_FILES
    resource.qrc
)

################################################################

add_library(${SARIBBON_LIB_NAME} SHARED
    ${SARIBBON_HEADER_FILES}
    ${SARIBBON_SOURCE_FILES}
    ${SARIBBON_RESOURCE_FILES}
    ${SACOLOR_HEADER_FILES}
    ${SACOLOR_SOURCE_FILES}
)
add_library(SA::${SARIBBON_LIB_NAME} ALIAS ${SARIBBON_LIB_NAME})
#################################################
# Qt相关的依赖
#################################################
target_link_libraries(${SARIBBON_LIB_NAME} PUBLIC
                      Qt${QT_VERSION_MAJOR}::Core
                      Qt${QT_VERSION_MAJOR}::Gui
                      Qt${QT_VERSION_MAJOR}::Widgets)


#################################################
# frameless相关的依赖
# qwindowkit库 https://github.com/stdware/qwindowkit
#################################################
# Qt X11Extras was first introduced in 5.1 and got removed in 6.0
# But it was again brought back as a private feature of QtGui in 6.2
if(SARIBBON_USE_FRAMELESS_LIB)
    find_package(QWindowKit REQUIRED)
    target_link_libraries(${SARIBBON_LIB_NAME} PRIVATE QWindowKit::Widgets)
endif()
#################################################

set_target_properties(${SARIBBON_LIB_NAME} PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
    CXX_EXTENSIONS OFF
    DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
    VERSION ${SARIBBON_VERSION}
    EXPORT_NAME ${SARIBBON_LIB_NAME}
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
# 构建库定义的宏,此宏必须为PRIVATE
target_compile_definitions(${SARIBBON_LIB_NAME} PRIVATE SA_RIBBON_BAR_MAKE_LIB)
# 使用SAColorWidgets但不作为库使用定义的宏,此宏必须为PRIVATE
target_compile_definitions(${SARIBBON_LIB_NAME} PRIVATE SA_COLOR_WIDGETS_MAKE_LIB)
if(SARIBBON_USE_FRAMELESS_LIB)
    #使用frameless必须设置SARIBBON_USE_3RDPARTY_FRAMELESSHELPER宏为1,此宏必须为PUBLIC
    target_compile_definitions(${SARIBBON_LIB_NAME} PUBLIC SARIBBON_USE_3RDPARTY_FRAMELESSHELPER=1)
else()
    #不使用frameless必须设置SARIBBON_USE_3RDPARTY_FRAMELESSHELPER宏为0,此宏必须为PUBLIC
    target_compile_definitions(${SARIBBON_LIB_NAME} PUBLIC SARIBBON_USE_3RDPARTY_FRAMELESSHELPER=0)
endif()

target_include_directories(${SARIBBON_LIB_NAME} PUBLIC
    $<INSTALL_INTERFACE:include/${SARIBBON_LIB_NAME}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)


# ------------------------------- install script -------------------------------
message(STATUS "install dir ${CMAKE_INSTALL_PREFIX}")

# 头文件
install(FILES
    ${SARIBBON_HEADER_FILES}
    DESTINATION include/${SARIBBON_LIB_NAME}
    COMPONENT headers
)
# SAColorWidget的头文件
install(FILES
    ${SACOLOR_HEADER_FILES}
    DESTINATION include/${SARIBBON_LIB_NAME}/colorWidgets
    COMPONENT headers
)

# 生成单一文件
# 单一文件是把整个SARibbon打包为一个h和一个cpp，方便集成
set(SARIBBON_AMALGAMATE_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/../SARibbon.h 
    ${CMAKE_CURRENT_SOURCE_DIR}/../SARibbon.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/../SARibbon.pri
)
install(FILES
    ${SARIBBON_AMALGAMATE_FILES}
    DESTINATION ${SARIBBON_LIB_NAME}_amalgamate
)
# 复制resource目录到install/amalgamate目录
install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/resource
    DESTINATION ${SARIBBON_LIB_NAME}_amalgamate/SARibbonBar
)
# resource.qrc
install(FILES
    ${SARIBBON_RESOURCE_FILES}
    DESTINATION ${SARIBBON_LIB_NAME}_amalgamate/SARibbonBar
)
# 复制3rdparty目录到install/amalgamate目录
install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty
    DESTINATION ${SARIBBON_LIB_NAME}_amalgamate/SARibbonBar
)


include(CMakePackageConfigHelpers)
# SARibbonBarConfig.cmake.in中，会让此变量和“${PACKAGE_PREFIX_DIR}/”进行拼接，也就是${PACKAGE_PREFIX_DIR}/@SARIBBON_LIB_INCLUDE_INSTALL_DIR@
# PACKAGE_PREFIX_DIR等于${CMAKE_CURRENT_LIST_DIR}/../..
# 最终变为${CMAKE_CURRENT_LIST_DIR}/../../include
# 注意此处是替换，不要出现引号
set(SARIBBON_LIB_INCLUDE_INSTALL_DIR include/${SARIBBON_LIB_NAME})
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${SARIBBON_LIB_NAME}ConfigVersion.cmake
    VERSION ${SARIBBON_VERSION}
    COMPATIBILITY SameMajorVersion
)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/SARibbonBarConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${SARIBBON_LIB_NAME}Config.cmake"
    INSTALL_DESTINATION lib/cmake/${SARIBBON_LIB_NAME}
    PATH_VARS SARIBBON_LIB_INCLUDE_INSTALL_DIR
)

install(TARGETS ${SARIBBON_LIB_NAME}
    EXPORT ${SARIBBON_LIB_NAME}Targets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include/${SARIBBON_LIB_NAME}
)
install(EXPORT ${SARIBBON_LIB_NAME}Targets
    FILE ${SARIBBON_LIB_NAME}Targets.cmake
    DESTINATION lib/cmake/${SARIBBON_LIB_NAME}
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${SARIBBON_LIB_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${SARIBBON_LIB_NAME}ConfigVersion.cmake"
    DESTINATION lib/cmake/${SARIBBON_LIB_NAME}
)

# 资源文件生成

if(WIN32)
	create_win32_resource_version(
                TARGET ${SARIBBON_LIB_NAME}
                FILENAME ${SARIBBON_LIB_NAME}
		EXT "dll"
		DESCRIPTION "Ribbon control library for Qt"
	)
endif()
