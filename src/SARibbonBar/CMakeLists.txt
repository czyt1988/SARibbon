cmake_minimum_required(VERSION 3.5)
project(SARibbonBar LANGUAGES CXX VERSION ${SARIBBON_VERSION})
set(SARIBBONBAR_LIB_NAME SARibbonBar)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

#################################################
# frameless库https://github.com/wangwenx190/framelesshelper
#################################################
function(setup_custom_moc_macros)
    cmake_parse_arguments(arg "" "" "TARGETS" ${ARGN})
    if(arg_UNPARSED_ARGUMENTS)
        message(AUTHOR_WARNING "setup_custom_moc_macros: Unrecognized arguments: ${arg_UNPARSED_ARGUMENTS}")
    endif()
    foreach(__target ${arg_TARGETS})
        if(NOT TARGET ${__target})
            message(AUTHOR_WARNING "${__target} is not a valid CMake target!")
            continue()
        endif()
        set_property(TARGET ${__target} APPEND PROPERTY AUTOMOC_MACRO_NAMES "FRAMELESSHELPER_QT_CLASS;FRAMELESSHELPER_PUBLIC_QT_CLASS;FRAMELESSHELPER_PRIVATE_QT_CLASS")
    endforeach()
endfunction()

if(SARIBBON_USE_FRAMELESS_LIB)
    if(UNIX AND NOT APPLE)
        # Qt X11Extras is only available in Qt5.
        if(QT_VERSION_MAJOR LESS 6)
            find_package(Qt5 QUIET COMPONENTS X11Extras)
        endif()
        find_package(X11 QUIET COMPONENTS xcb)
        if(TARGET X11::xcb)
            message("Found system XCB. The XCB wrapper will be disabled.")
        else()
            message("System XCB not found. The XCB wrapper will be used instead.")
        endif()
        find_package(PkgConfig QUIET)
        if(PkgConfig_FOUND)
            pkg_check_modules(GTK3 QUIET IMPORTED_TARGET gtk+-3.0)
        endif()
        if(TARGET PkgConfig::GTK3)
            message("Found system GTK. The GTK wrapper will be disabled.")
        else()
            message("System GTK not found. The GTK wrapper will be used instead.")
        endif()
    endif()

    # frameless core 相关路径设置
    SET(FRAMELESS_HEADER_BASE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/framelesshelper/include)
    SET(FRAMELESS_HEADER_DIR ${FRAMELESS_HEADER_BASE}/FramelessHelper)
    SET(FRAMELESS_HEADER_DIR_CORE ${FRAMELESS_HEADER_DIR}/Core)
    SET(FRAMELESS_HEADER_DIR_CORE_PRIVATE ${FRAMELESS_HEADER_DIR_CORE}/private)
    SET(FRAMELESS_HEADER_DIR_STATIC_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/framelesshelper/qmake/inc/core)

    SET(FRAMELESS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/framelesshelper/src)
    SET(FRAMELESS_SOURCE_DIR_CORE ${FRAMELESS_SOURCE_DIR}/core)

    # frameless core config file / cn：配置h文件
    SET(FRAMELESS_CORE_CONFIG_HEADER_FILES
        ${FRAMELESS_HEADER_DIR_STATIC_CONFIG}/framelesshelper.version
        ${FRAMELESS_HEADER_DIR_STATIC_CONFIG}/framelesshelper.config
    )
    # frameless core header files / cn：core对应h文件
    SET(FRAMELESS_CORE_HEADER_FILES
        ${FRAMELESS_HEADER_DIR_CORE}/chromepalette.h
        ${FRAMELESS_HEADER_DIR_CORE}/framelesshelper_qt.h
        ${FRAMELESS_HEADER_DIR_CORE}/framelesshelpercore_global.h
        ${FRAMELESS_HEADER_DIR_CORE}/framelessmanager.h
        ${FRAMELESS_HEADER_DIR_CORE}/micamaterial.h
        ${FRAMELESS_HEADER_DIR_CORE}/utils.h
        ${FRAMELESS_HEADER_DIR_CORE}/windowborderpainter.h
        ${FRAMELESS_HEADER_DIR_CORE_PRIVATE}/chromepalette_p.h
        ${FRAMELESS_HEADER_DIR_CORE_PRIVATE}/framelessconfig_p.h
        ${FRAMELESS_HEADER_DIR_CORE_PRIVATE}/framelesshelpercore_global_p.h
        ${FRAMELESS_HEADER_DIR_CORE_PRIVATE}/framelessmanager_p.h
        ${FRAMELESS_HEADER_DIR_CORE_PRIVATE}/micamaterial_p.h
        ${FRAMELESS_HEADER_DIR_CORE_PRIVATE}/scopeguard_p.h
        ${FRAMELESS_HEADER_DIR_CORE_PRIVATE}/sysapiloader_p.h
        ${FRAMELESS_HEADER_DIR_CORE_PRIVATE}/versionnumber_p.h
        ${FRAMELESS_HEADER_DIR_CORE_PRIVATE}/windowborderpainter_p.h
    )
    # frameless core cpp files / cn：core对应cpp文件
    SET(FRAMELESS_CORE_SOURCE_FILES
        ${FRAMELESS_SOURCE_DIR_CORE}/chromepalette.cpp
        ${FRAMELESS_SOURCE_DIR_CORE}/framelessconfig.cpp
        ${FRAMELESS_SOURCE_DIR_CORE}/framelesshelper_qt.cpp
        ${FRAMELESS_SOURCE_DIR_CORE}/framelesshelpercore_global.cpp
        ${FRAMELESS_SOURCE_DIR_CORE}/framelessmanager.cpp
        ${FRAMELESS_SOURCE_DIR_CORE}/micamaterial.cpp
        ${FRAMELESS_SOURCE_DIR_CORE}/sysapiloader.cpp
        ${FRAMELESS_SOURCE_DIR_CORE}/utils.cpp
        ${FRAMELESS_SOURCE_DIR_CORE}/windowborderpainter.cpp
    )
    if(WIN32)
        list(APPEND FRAMELESS_CORE_HEADER_FILES
            ${FRAMELESS_HEADER_DIR_CORE}/framelesshelper_windows.h
            ${FRAMELESS_HEADER_DIR_CORE}/framelesshelper_win.h
        )
        list(APPEND FRAMELESS_CORE_HEADER_FILES
            ${FRAMELESS_HEADER_DIR_CORE_PRIVATE}/registrykey_p.h
            ${FRAMELESS_HEADER_DIR_CORE_PRIVATE}/winverhelper_p.h
        )
        list(APPEND FRAMELESS_CORE_SOURCE_FILES
            ${FRAMELESS_SOURCE_DIR_CORE}/registrykey.cpp
            ${FRAMELESS_SOURCE_DIR_CORE}/utils_win.cpp
            ${FRAMELESS_SOURCE_DIR_CORE}/framelesshelper_win.cpp
            ${FRAMELESS_SOURCE_DIR_CORE}/winverhelper.cpp
            ${FRAMELESS_SOURCE_DIR_CORE}/platformsupport_win.cpp
        )
    elseif(APPLE)
        list(APPEND FRAMELESS_CORE_SOURCE_FILES
            ${FRAMELESS_SOURCE_DIR_CORE}/utils_mac.mm)
    elseif(UNIX)
        list(APPEND FRAMELESS_CORE_HEADER_FILES
            ${FRAMELESS_HEADER_DIR_CORE}/framelesshelper_linux.h
        )
        list(APPEND FRAMELESS_CORE_SOURCE_FILES
            ${FRAMELESS_SOURCE_DIR_CORE}/utils_linux.cpp
            ${FRAMELESS_SOURCE_DIR_CORE}/platformsupport_linux.cpp
        )
    endif()
    # resource files
    # cn:资源文件
    SET(FRAMELESS_CORE_RESOURCES_FILES
        ${FRAMELESS_SOURCE_DIR_CORE}/framelesshelpercore.qrc
    )

    # frameless widget 相关路径设置

    SET(FRAMELESS_HEADER_DIR_WIDGETS ${FRAMELESS_HEADER_DIR}/Widgets)
    SET(FRAMELESS_HEADER_DIR_WIDGETS_PRIVATE ${FRAMELESS_HEADER_DIR_WIDGETS}/private)
    SET(FRAMELESS_SOURCE_DIR_WIDGETS ${FRAMELESS_SOURCE_DIR}/widgets)
    # frameless widgets header files / cn：widgets对应h文件
    SET(FRAMELESS_WIDGETS_HEADER_FILES
        ${FRAMELESS_HEADER_DIR_WIDGETS}/framelesshelperwidgets_global.h
        ${FRAMELESS_HEADER_DIR_WIDGETS}/framelesswidget.h
        ${FRAMELESS_HEADER_DIR_WIDGETS}/framelessmainwindow.h
        ${FRAMELESS_HEADER_DIR_WIDGETS}/standardsystembutton.h
        ${FRAMELESS_HEADER_DIR_WIDGETS}/framelesswidgetshelper.h
        ${FRAMELESS_HEADER_DIR_WIDGETS}/standardtitlebar.h
        ${FRAMELESS_HEADER_DIR_WIDGETS}/framelessdialog.h
        ${FRAMELESS_HEADER_DIR_WIDGETS_PRIVATE}/framelesswidgetshelper_p.h
        ${FRAMELESS_HEADER_DIR_WIDGETS_PRIVATE}/standardsystembutton_p.h
        ${FRAMELESS_HEADER_DIR_WIDGETS_PRIVATE}/standardtitlebar_p.h
        ${FRAMELESS_HEADER_DIR_WIDGETS_PRIVATE}/framelesswidget_p.h
        ${FRAMELESS_HEADER_DIR_WIDGETS_PRIVATE}/framelessmainwindow_p.h
        ${FRAMELESS_HEADER_DIR_WIDGETS_PRIVATE}/widgetssharedhelper_p.h
        ${FRAMELESS_HEADER_DIR_WIDGETS_PRIVATE}/framelessdialog_p.h
    )
    # frameless widgets cpp files / cn：widgets对应cpp文件
    SET(FRAMELESS_WIDGETS_SOURCE_FILES
        ${FRAMELESS_SOURCE_DIR_WIDGETS}/framelessmainwindow.cpp
        ${FRAMELESS_SOURCE_DIR_WIDGETS}/framelesswidgetshelper.cpp
        ${FRAMELESS_SOURCE_DIR_WIDGETS}/framelesswidget.cpp
        ${FRAMELESS_SOURCE_DIR_WIDGETS}/standardsystembutton.cpp
        ${FRAMELESS_SOURCE_DIR_WIDGETS}/standardtitlebar.cpp
        ${FRAMELESS_SOURCE_DIR_WIDGETS}/widgetssharedhelper.cpp
        ${FRAMELESS_SOURCE_DIR_WIDGETS}/framelesshelperwidgets_global.cpp
        ${FRAMELESS_SOURCE_DIR_WIDGETS}/framelessdialog.cpp
    )
    # 把所有FRAMELESS相关的库合并到一起
    SET(FRAMELESS_HEADER_FILES ${FRAMELESS_CORE_CONFIG_HEADER_FILES})
    list(APPEND FRAMELESS_HEADER_FILES ${FRAMELESS_CORE_HEADER_FILES})
    list(APPEND FRAMELESS_HEADER_FILES ${FRAMELESS_WIDGETS_HEADER_FILES})
    SET(FRAMELESS_SOURCE_FILES ${FRAMELESS_CORE_SOURCE_FILES})
    list(APPEND FRAMELESS_SOURCE_FILES ${FRAMELESS_WIDGETS_SOURCE_FILES})
    SET(FRAMELESS_RESOURCE_FILES ${FRAMELESS_CORE_RESOURCES_FILES})
endif()
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

# resource files
# cn:资源文件
SET(SARIBBON_RESOURCE_FILES
    resource.qrc
)

################################################################
if(SARIBBON_USE_FRAMELESS_LIB)
    add_library(${SARIBBONBAR_LIB_NAME} SHARED
        ${SARIBBON_HEADER_FILES}
        ${SARIBBON_SOURCE_FILES}
        ${SARIBBON_RESOURCE_FILES}
        ${SACOLOR_HEADER_FILES}
        ${SACOLOR_SOURCE_FILES}
        ${FRAMELESS_HEADER_FILES}
        ${FRAMELESS_SOURCE_FILES}
        ${FRAMELESS_RESOURCE_FILES}
    )
    setup_custom_moc_macros(TARGETS ${SARIBBONBAR_LIB_NAME})
else()
    add_library(${SARIBBONBAR_LIB_NAME} SHARED
        ${SARIBBON_HEADER_FILES}
        ${SARIBBON_SOURCE_FILES}
        ${SARIBBON_RESOURCE_FILES}
        ${SACOLOR_HEADER_FILES}
        ${SACOLOR_SOURCE_FILES}
    )
endif()

#################################################
# Qt相关的依赖
#################################################
target_link_libraries(${SARIBBONBAR_LIB_NAME} PUBLIC
                      Qt${QT_VERSION_MAJOR}::Core
                      Qt${QT_VERSION_MAJOR}::Gui
                      Qt${QT_VERSION_MAJOR}::Widgets)


#################################################
# frameless相关的依赖
#################################################
# Qt X11Extras was first introduced in 5.1 and got removed in 6.0
# But it was again brought back as a private feature of QtGui in 6.2
if(SARIBBON_USE_FRAMELESS_LIB)
    if(TARGET Qt5::X11Extras)
        target_link_libraries(${SARIBBONBAR_LIB_NAME} PRIVATE
            Qt5::X11Extras
        )
    endif()

    # frameless 依赖了qt-private相关的库
    target_link_libraries(${SARIBBONBAR_LIB_NAME} PRIVATE
            Qt${QT_VERSION_MAJOR}::CorePrivate
            Qt${QT_VERSION_MAJOR}::GuiPrivate
        )

    if(APPLE)
      target_link_libraries(${SARIBBONBAR_LIB_NAME} PRIVATE
          "-framework Foundation"
          "-framework Cocoa"
          "-framework AppKit"
      )
    elseif(UNIX)
      if(TARGET X11::xcb)
          target_link_libraries(${SARIBBONBAR_LIB_NAME} PRIVATE
              X11::xcb
          )
      endif()
      if(TARGET PkgConfig::GTK3)
          target_link_libraries(${SARIBBONBAR_LIB_NAME} PRIVATE
              PkgConfig::GTK3
          )
          target_compile_definitions(${SARIBBONBAR_LIB_NAME} PRIVATE
              GDK_VERSION_MIN_REQUIRED=GDK_VERSION_3_6
          )
      endif()
    endif()
endif()
#################################################

set_target_properties(${SARIBBONBAR_LIB_NAME} PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
    CXX_EXTENSIONS OFF
    DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
    VERSION ${SARIBBON_VERSION}
    EXPORT_NAME ${SARIBBONBAR_LIB_NAME}
    ARCHIVE_OUTPUT_DIRECTORY "${SARIBBON_BIN_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${SARIBBON_BIN_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${SARIBBON_BIN_DIR}"
)
# 构建库定义的宏
target_compile_definitions(${SARIBBONBAR_LIB_NAME} PRIVATE SA_RIBBON_BAR_MAKE_LIB)
# 使用SAColorWidgets但不作为库使用定义的宏
target_compile_definitions(${SARIBBONBAR_LIB_NAME} PRIVATE SA_COLOR_WIDGETS_MAKE_LIB)
if(SARIBBON_USE_FRAMELESS_LIB)
    # 显示定义FRAMELESSHELPER_FEATURE_static_build为-1
    target_compile_definitions(${SARIBBONBAR_LIB_NAME} PRIVATE FRAMELESSHELPER_FEATURE_static_build=-1)
    # 使用frameless,但也要导出符号，因此要定义导出宏
    target_compile_definitions(${SARIBBONBAR_LIB_NAME} PRIVATE FRAMELESSHELPER_CORE_LIBRARY)
    target_compile_definitions(${SARIBBONBAR_LIB_NAME} PRIVATE FRAMELESSHELPER_WIDGETS_LIBRARY)
    #使用frameless必须设置SARIBBON_USE_3RDPARTY_FRAMELESSHELPER宏为1
    target_compile_definitions(${SARIBBONBAR_LIB_NAME} PUBLIC SARIBBON_USE_3RDPARTY_FRAMELESSHELPER=1)
else()
    #不使用frameless必须设置SARIBBON_USE_3RDPARTY_FRAMELESSHELPER宏为0
    target_compile_definitions(${SARIBBONBAR_LIB_NAME} PUBLIC SARIBBON_USE_3RDPARTY_FRAMELESSHELPER=0)
endif()

if(SARIBBON_USE_FRAMELESS_LIB)
    target_include_directories(${SARIBBONBAR_LIB_NAME} PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${FRAMELESS_HEADER_BASE}>
        $<BUILD_INTERFACE:${FRAMELESS_HEADER_DIR}>
        $<BUILD_INTERFACE:${FRAMELESS_HEADER_DIR_CORE}>
        $<BUILD_INTERFACE:${FRAMELESS_HEADER_DIR_CORE_PRIVATE}>
        $<BUILD_INTERFACE:${FRAMELESS_HEADER_DIR_STATIC_CONFIG}>
        $<BUILD_INTERFACE:${FRAMELESS_HEADER_DIR_WIDGETS}>
        $<BUILD_INTERFACE:${FRAMELESS_HEADER_DIR_WIDGETS_PRIVATE}>
    )
else()
    target_include_directories(${SARIBBONBAR_LIB_NAME} PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )
endif()
# ------------------------------- install script -------------------------------
set(SARIBBON_INSTALL_DIR SARibbon)
set(CMAKE_INSTALL_PREFIX ${SARIBBON_BIN_DIR})
message(STATUS "install dir ${SARIBBON_BIN_DIR}")
# Generate library version info which will generate xxxConfigVersion.cmake,
# the ${PACKAGE_VERSION} is the version defined in project()
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${SARIBBONBAR_LIB_NAME}ConfigVersion.cmake"
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

# 生成单一文件
# 单一文件是把整个SARibbon打包为一个h和一个cpp，方便集成
set(SARIBBON_AMALGAMATE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../SARibbon.h 
	${CMAKE_CURRENT_SOURCE_DIR}/../SARibbon.cpp 
	${CMAKE_CURRENT_SOURCE_DIR}/../SARibbon.pri)
install(FILES
    ${SARIBBON_AMALGAMATE_FILES}
    DESTINATION amalgamate
    COMPONENT amalgamate
)
install(FILES
    ${SARIBBON_RESOURCE_FILES}
    DESTINATION amalgamate/SARibbonBar
    COMPONENT amalgamate
)
set(SARIBBON_RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resource)
set(SARIBBON_RESOURCE_RAW_FILES 
	${SARIBBON_RESOURCE_DIR}/theme-win7.qss
	${SARIBBON_RESOURCE_DIR}/theme-office2013.qss
	${SARIBBON_RESOURCE_DIR}/theme-dark.qss
	${SARIBBON_RESOURCE_DIR}/ArrowDown.png
	${SARIBBON_RESOURCE_DIR}/ArrowMore.png
	${SARIBBON_RESOURCE_DIR}/ArrowUp.png
	${SARIBBON_RESOURCE_DIR}/ribbonPannelOptionButton.png
	${SARIBBON_RESOURCE_DIR}/define-color.svg
	${SARIBBON_RESOURCE_DIR}/Titlebar_Close.png
	${SARIBBON_RESOURCE_DIR}/Titlebar_Close_Hover.png
	${SARIBBON_RESOURCE_DIR}/Titlebar_Max.png
	${SARIBBON_RESOURCE_DIR}/Titlebar_Min.png
	${SARIBBON_RESOURCE_DIR}/Titlebar_Normal.png
	${SARIBBON_RESOURCE_DIR}/Titlebar_Shade.png
	${SARIBBON_RESOURCE_DIR}/Titlebar_Unshade.png
	)
install(FILES
    ${SARIBBON_RESOURCE_RAW_FILES}
    DESTINATION amalgamate/SARibbonBar/resource
    COMPONENT amalgamate
)

# 如果开启了frameless，要把frameless的文件也拷贝过去
if(SARIBBON_USE_FRAMELESS_LIB)
	#复制目录src/SARibbonBar/3rdparty/framelesshelper/include/
	install(DIRECTORY
        ${FRAMELESS_HEADER_DIR}
		DESTINATION include/${SARIBBON_INSTALL_DIR}/3rdparty/framelesshelper/include
		COMPONENT headers
	)
	#复制目录src/SARibbonBar/3rdparty/framelesshelper/qmake/
	SET(FRAMELESS_QMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/framelesshelper/qmake)
	install(DIRECTORY
        ${FRAMELESS_QMAKE_DIR}
		DESTINATION include/${SARIBBON_INSTALL_DIR}/3rdparty/framelesshelper/
		COMPONENT headers
	)
endif()

install(TARGETS ${SARIBBONBAR_LIB_NAME}
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
                TARGET ${SARIBBONBAR_LIB_NAME}
                FILENAME ${SARIBBONBAR_LIB_NAME}
		EXT "dll"
		DESCRIPTION "Ribbon control library for Qt"
	)
endif()
