cmake_minimum_required(VERSION 3.5)

project(SimpleExample LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package( ...) calls below.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)


#################################################
# SARibbon相关的文件只要3个
#################################################
# 这里展示如何简单的使用SARibbon在CMake文件里
# Here is an example of how to simply use SARibbon in a CMake file
SET(SARIBBON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set(SARIBBON_SIMPLE ${SARIBBON_DIR}/SARibbon.h ${SARIBBON_DIR}/SARibbon.cpp ${SARIBBON_DIR}/SARibbonBar/resource.qrc)

#由于加入了frameless库，这里要把frameless的文件添加进来
#################################################
# frameless库https://github.com/wangwenx190/framelesshelper
#################################################

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
# 显示定义FRAMELESSHELPER_FEATURE_static_build为1
add_definitions(-DFRAMELESSHELPER_FEATURE_static_build=1)
SET(SARIBBON_3RDPARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../SARibbonBar/3rdparty)
SET(FRAMELESS_LIB_DIR ${SARIBBON_3RDPARTY_DIR}/framelesshelper)
SET(FRAMELESS_HEADER_BASE ${FRAMELESS_LIB_DIR}/include)
SET(FRAMELESS_HEADER_DIR ${FRAMELESS_HEADER_BASE}/FramelessHelper)
SET(FRAMELESS_HEADER_DIR_CORE ${FRAMELESS_HEADER_DIR}/Core)
SET(FRAMELESS_HEADER_DIR_CORE_PRIVATE ${FRAMELESS_HEADER_DIR_CORE}/private)
SET(FRAMELESS_HEADER_DIR_STATIC_CONFIG ${FRAMELESS_LIB_DIR}/qmake/inc/core)

SET(FRAMELESS_SOURCE_DIR ${FRAMELESS_LIB_DIR}/src)
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


set(PROJECT_SOURCES
        main.cpp
        MainWindow.cpp
        MainWindow.h
        ${SARIBBON_SIMPLE}
        ${FRAMELESS_CORE_CONFIG_HEADER_FILES}
        ${FRAMELESS_CORE_HEADER_FILES}
        ${FRAMELESS_CORE_SOURCE_FILES}
        ${FRAMELESS_CORE_RESOURCES_FILES}
        ${FRAMELESS_WIDGETS_HEADER_FILES}
        ${FRAMELESS_WIDGETS_SOURCE_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(SimpleExample
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(SimpleExample SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(SimpleExample
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(SimpleExample PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# frameless 依赖了qt-private相关的库
target_link_libraries(SimpleExample PRIVATE
        Qt${QT_VERSION_MAJOR}::CorePrivate
        Qt${QT_VERSION_MAJOR}::GuiPrivate
    )

target_include_directories(SimpleExample PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${SARIBBON_DIR}>
    $<BUILD_INTERFACE:${FRAMELESS_HEADER_BASE}>
    $<BUILD_INTERFACE:${FRAMELESS_HEADER_DIR}>
    $<BUILD_INTERFACE:${FRAMELESS_HEADER_DIR_CORE}>
    $<BUILD_INTERFACE:${FRAMELESS_HEADER_DIR_CORE_PRIVATE}>
    $<BUILD_INTERFACE:${FRAMELESS_HEADER_DIR_STATIC_CONFIG}>
    $<BUILD_INTERFACE:${FRAMELESS_HEADER_DIR_WIDGETS}>
    $<BUILD_INTERFACE:${FRAMELESS_HEADER_DIR_WIDGETS_PRIVATE}>
)
