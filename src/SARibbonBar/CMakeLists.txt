cmake_minimum_required(VERSION 3.5)
project(SARibbonBar LANGUAGES CXX VERSION ${SARIBBON_VERSION})
set(SARIBBON_LIB_NAME SARibbonBar)
message(STATUS "SARIBBON_LIB_NAME=${SARIBBON_LIB_NAME}")
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
    SARibbonBarVersionInfo.h
    SARibbonGlobal.h
    SAFramelessHelper.h
    SARibbonActionsManager.h
    SARibbonBar.h
    SARibbonCustomizeData.h
    SARibbonCustomizeDialog.h
    SARibbonCustomizeWidget.h
    SARibbonMainWindow.h
    SARibbonWidget.h
    SARibbonSystemButtonBar.h
    SARibbonApplicationButton.h
    SARibbonTabBar.h
    SARibbonCategory.h
    SARibbonContextCategory.h
    SARibbonPannel.h
    SARibbonToolButton.h
    SARibbonMenu.h
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
    SARibbonApplicationWidget.h
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
    SARibbonWidget.cpp
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
    SARibbonApplicationWidget.cpp
)

# resource files
# cn:资源文件
SET(SARIBBON_RESOURCE_FILES
    SARibbonResource.qrc
)

message(STATUS "SARibbon resource file:${SARIBBON_RESOURCE_FILES}")
################################################################

add_library(${SARIBBON_LIB_NAME} SHARED
    ${SARIBBON_HEADER_FILES}
    ${SARIBBON_SOURCE_FILES}
    ${SARIBBON_RESOURCE_FILES}
    ${SACOLOR_HEADER_FILES}
    ${SACOLOR_SOURCE_FILES}
)
add_library(${SARIBBON_LIB_NAME}::${SARIBBON_LIB_NAME} ALIAS ${SARIBBON_LIB_NAME})
#################################################
# Qt相关的依赖
#################################################


target_link_libraries(${SARIBBON_LIB_NAME} PUBLIC
                      Qt${QT_VERSION_MAJOR}::Core
                      Qt${QT_VERSION_MAJOR}::Gui
                      Qt${QT_VERSION_MAJOR}::Widgets)
message(STATUS "link ${SARIBBON_LIB_NAME} PUBLIC Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets")

#################################################
# frameless相关的依赖
# qwindowkit库 https://github.com/stdware/qwindowkit
#################################################
# Qt X11Extras was first introduced in 5.1 and got removed in 6.0
# But it was again brought back as a private feature of QtGui in 6.2
if(_SARIBBON_USE_FRAMELESS_LIB)
    find_package(QWindowKit REQUIRED)
    target_link_libraries(${SARIBBON_LIB_NAME} PRIVATE QWindowKit::Widgets)
    message(STATUS "link ${SARIBBON_LIB_NAME} PRIVATE QWindowKit::Widgets")
endif()
#################################################
include(GNUInstallDirs)
set_target_properties(${SARIBBON_LIB_NAME} PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
    CXX_EXTENSIONS OFF
    DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
    VERSION ${SARIBBON_VERSION}
    EXPORT_NAME ${SARIBBON_LIB_NAME}
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}"
)
# 构建库定义的宏,此宏必须为PRIVATE
target_compile_definitions(${SARIBBON_LIB_NAME} PRIVATE SA_RIBBON_BAR_MAKE_LIB)
message(STATUS "macros predefined:#define SA_RIBBON_BAR_MAKE_LIB")
# 使用SAColorWidgets但不作为库使用定义的宏,此宏必须为PRIVATE
target_compile_definitions(${SARIBBON_LIB_NAME} PRIVATE SA_COLOR_WIDGETS_MAKE_LIB)
message(STATUS "macros predefined:#define SA_COLOR_WIDGETS_MAKE_LIB")
if(_SARIBBON_USE_FRAMELESS_LIB)
    #使用frameless必须设置SARIBBON_USE_3RDPARTY_FRAMELESSHELPER宏为1,此宏必须为PUBLIC
    target_compile_definitions(${SARIBBON_LIB_NAME} PUBLIC SARIBBON_USE_3RDPARTY_FRAMELESSHELPER=1)
    message(STATUS "macros predefined:#define SARIBBON_USE_3RDPARTY_FRAMELESSHELPER 1")
    if(SARIBBON_ENABLE_SNAPLAYOUT)
        # 是否开启windows11的snap layout效果，目前这个在不同qt版本下有bug
        target_compile_definitions(${SARIBBON_LIB_NAME} PUBLIC SARIBBON_ENABLE_SNAP_LAYOUT=1)
        message(STATUS "macros predefined:#define SARIBBON_ENABLE_SNAP_LAYOUT 1")
    else()
        target_compile_definitions(${SARIBBON_LIB_NAME} PUBLIC SARIBBON_ENABLE_SNAP_LAYOUT=0)
        message(STATUS "macros predefined:#define SARIBBON_ENABLE_SNAP_LAYOUT 0")
    endif()
else()
    #不使用frameless必须设置SARIBBON_USE_3RDPARTY_FRAMELESSHELPER宏为0,此宏必须为PUBLIC
    target_compile_definitions(${SARIBBON_LIB_NAME} PUBLIC SARIBBON_USE_3RDPARTY_FRAMELESSHELPER=0)
    message(STATUS "macros predefined:#define SARIBBON_USE_3RDPARTY_FRAMELESSHELPER 0")
endif()

target_include_directories(${SARIBBON_LIB_NAME} PUBLIC
    $<INSTALL_INTERFACE:include/${SARIBBON_LIB_NAME}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
)
if(MSVC)
    # To unbreak std::min/max
    target_compile_definitions(${SARIBBON_LIB_NAME} PRIVATE NOMINMAX)
    message(STATUS "macros predefined:#define NOMINMAX")
endif()

# ------------------------------- install script -------------------------------

# 头文件
install(FILES
    ${SARIBBON_HEADER_FILES}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${SARIBBON_LIB_NAME}
    COMPONENT headers
)
# SAColorWidget的头文件
install(FILES
    ${SACOLOR_HEADER_FILES}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${SARIBBON_LIB_NAME}/colorWidgets
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



include(CMakePackageConfigHelpers)

# SARibbonBarConfig.cmake.in中，会让此变量和“${PACKAGE_PREFIX_DIR}/”进行拼接，也就是${PACKAGE_PREFIX_DIR}/@SARIBBON_LIB_INCLUDE_INSTALL_DIR@
# PACKAGE_PREFIX_DIR等于${CMAKE_CURRENT_LIST_DIR}/../..
# 最终变为${CMAKE_CURRENT_LIST_DIR}/../../include
# 注意此处是替换，不要出现引号
set(SARIBBON_LIB_INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR}/${SARIBBON_LIB_NAME})
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
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${SARIBBON_LIB_NAME}
)
install(EXPORT ${SARIBBON_LIB_NAME}Targets
    FILE ${SARIBBON_LIB_NAME}Targets.cmake
    NAMESPACE ${SARIBBON_LIB_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${SARIBBON_LIB_NAME}
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${SARIBBON_LIB_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${SARIBBON_LIB_NAME}ConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${SARIBBON_LIB_NAME}
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
