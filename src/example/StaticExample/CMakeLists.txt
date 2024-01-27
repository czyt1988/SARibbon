cmake_minimum_required(VERSION 3.5)

project(StaticExample LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# frameless能提供windows的窗口特效，如边缘吸附，且对高分屏多屏幕的支持更好,默认开启
option(SARIBBON_USE_FRAMELESS_LIB "Using the QWindowKit library as a frameless solution" OFF)

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


# 注意后面所有SARIBBON_USE_FRAMELESS_LIB的判断，都是和framelss有关
# 如果是使用frameless库，需要c++17，否则c++11足以
if(SARIBBON_USE_FRAMELESS_LIB)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    if(MSVC)
        # CMAKE_CXX_STANDARD对有些版本的msvc无效
        set(CMAKE_CXX_FLAGS"${CMAKE_CXX_FLAGS} /std:c++17")
    endif()
    message(STATUS "The current QT version can use the frameless library and enable C++17")
else()
    set(CMAKE_CXX_STANDARD 14)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    if(MSVC)
        # CMAKE_CXX_STANDARD对有些版本的msvc无效
        set(CMAKE_CXX_FLAGS"${CMAKE_CXX_FLAGS} /std:c++14")
    endif()
    message(STATUS "The current qt version cannot use the frameless library, enable C++11")
endif()
#################################################
# SARibbon相关的文件只要3个
#################################################
# 这里展示如何简单的使用SARibbon在CMake文件里
# Here is an example of how to simply use SARibbon in a CMake file
SET(SARIBBON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set(SARIBBON_SIMPLE
    ${SARIBBON_DIR}/SARibbon.h
    ${SARIBBON_DIR}/SARibbon.cpp
    ${SARIBBON_DIR}/SARibbonBar/resource.qrc
)


if(SARIBBON_USE_FRAMELESS_LIB)
    #使用frameless必须设置SARIBBON_USE_3RDPARTY_FRAMELESSHELPER宏为1
    add_definitions(-DSARIBBON_USE_3RDPARTY_FRAMELESSHELPER=1)
else()
    #不使用frameless必须设置SARIBBON_USE_3RDPARTY_FRAMELESSHELPER宏为0
    add_definitions(-DSARIBBON_USE_3RDPARTY_FRAMELESSHELPER=0)
endif()



set(PROJECT_SOURCES
        main.cpp
        MainWindow.cpp
        MainWindow.h
        icon.qrc
        ${SARIBBON_SIMPLE}
)


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(StaticExample
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(StaticExample SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(StaticExample
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(StaticExample PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_include_directories(StaticExample PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${SARIBBON_DIR}>
)



