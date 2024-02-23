
# Cmake的命令不区分大小写，例如message，set等命令；但Cmake的变量区分大小写
# 为统一风格，本项目的Cmake命令全部采用小写，变量全部采用大写加下划线组合。

cmake_minimum_required(VERSION 3.5)

set(SARIBBON_VERSION_MAJOR 2)
set(SARIBBON_VERSION_MINOR 0)
set(SARIBBON_VERSION_PATCH 0)
set(SARIBBON_VERSION "${SARIBBON_VERSION_MAJOR}.${SARIBBON_VERSION_MINOR}.${SARIBBON_VERSION_PATCH}")
message(STATUS "SARibbon v${SARIBBON_VERSION}")
project(SARibbon VERSION ${SARIBBON_VERSION} LANGUAGES CXX)

# option(BUILD_SHARED_LIBS "build the SARibbonBar in shared lib mode" ON)
option(SARIBBON_BUILD_EXAMPLES "build the examples" ON)
# frameless能提供windows的窗口特效，如边缘吸附，且对高分屏多屏幕的支持更好,默认开启
option(SARIBBON_USE_FRAMELESS_LIB "Using the QWindowKit library as a frameless solution" OFF)

# load Qt library, minimum version required is 5.8
# cn:Qt库加载,最低版本要求为5.8
set(SARIBBON_MIN_QT_VERSION 5.8)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} ${SARIBBON_MIN_QT_VERSION} COMPONENTS
    Core
    Gui
    Widgets
    REQUIRED
)
message(STATUS "current Qt version is Qt${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}")

# 根据qt版本确认能否使用frameless库，目前frameless库支持qt5.14,qt5.15,qt6.1+,除了上诉版本，都使用不了
# 上述版本的qt如果设置SARIBBON_USE_FRAMELESS_LIB=ON也会改为OFF
if(SARIBBON_USE_FRAMELESS_LIB)
    if(${QT_VERSION_MAJOR} EQUAL 5)
        # qt版本为5，判断是否小版本小于14，小于14无法使用
        if(${QT_VERSION_MINOR} LESS 14)
            #5.14和5.15可以使用frameless
            set(SARIBBON_USE_FRAMELESS_LIB OFF)
            message(WARNING "QT_VERSION = Qt${QT_VERSION} QT version minor is ${QT_VERSION_MINOR},less 14,will set SARIBBON_USE_FRAMELESS_LIB OFF")
        endif()
    elseif(${QT_VERSION_MAJOR} EQUAL 6)
        # qt版本为6，判断是否小版本大于6.1
        if(${QT_VERSION_MINOR} LESS_EQUAL 1)
            #6.0及bug多多无法使用frameless
            set(SARIBBON_USE_FRAMELESS_LIB OFF)
            message(WARNING "QT_VERSION = Qt${QT_VERSION} QT version minor is ${QT_VERSION_MINOR},less 1,will set SARIBBON_USE_FRAMELESS_LIB OFF")
        endif()
    endif()
endif()

# 如果是使用frameless库，需要c++17，否则c++14
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

set(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "add a postfix, usually d on windows")
set(CMAKE_RELEASE_POSTFIX "" CACHE STRING "add a postfix, usually empty on windows")
set(CMAKE_RELWITHDEBINFO_POSTFIX "rd" CACHE STRING "add a postfix, usually empty on windows")
set(CMAKE_MINSIZEREL_POSTFIX "s" CACHE STRING "add a postfix, usually empty on windows")
# Set the build postfix extension according to what configuration is being built.

if(CMAKE_BUILD_TYPE MATCHES "Release")
    set(CMAKE_BUILD_POSTFIX "${CMAKE_RELEASE_POSTFIX}")
elseif (CMAKE_BUILD_TYPE MATCHES "MinSizeRel")
    set(CMAKE_BUILD_POSTFIX "${CMAKE_MINSIZEREL_POSTFIX}")
elseif(CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
    set(CMAKE_BUILD_POSTFIX "${CMAKE_RELWITHDEBINFO_POSTFIX}")
elseif(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(CMAKE_BUILD_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
else()
    set(CMAKE_BUILD_POSTFIX "")
endif()

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4819")
endif()


# 平台判断
if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
    set(SARIBBON_PLATFORM "x86")
else()
    set(SARIBBON_PLATFORM "x64")
endif()

# The bin file directory is one level above the current directory
# cn:bin文件目录在当前目录的上上一级
# 安装目录默认名字为bin_qt5.14.2_msvc_x64
set(SARIBBON_BIN_NAME bin_qt${QT_VERSION}_${CMAKE_CXX_COMPILER_ID}_${SARIBBON_PLATFORM})
set(SARIBBON_BIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${SARIBBON_BIN_NAME})

# windows系统下，默认直接安装到当前文件夹下
if(WIN32)
    set(CMAKE_INSTALL_PREFIX "${SARIBBON_BIN_DIR}")
endif()

##################################
# subdirectory
##################################

if(SARIBBON_USE_FRAMELESS_LIB)
    if(NOT QWindowKit_DIR)
        message(STATUS "option SARIBBON_USE_FRAMELESS_LIB=ON,but QWindowKit_DIR not defined,will set QWindowKit_DIR=${SARIBBON_BIN_DIR}/lib/cmake/QWindowKit")
        set(__qwk_dir ${SARIBBON_BIN_DIR}/lib/cmake/QWindowKit)
        if(EXISTS ${__qwk_dir})
            set(QWindowKit_DIR ${__qwk_dir})
        else()
            message(WARNING "SARIBBON_USE_FRAMELESS_LIB=ON,but not defined QWindowKit_DIR path")
        endif()
        # 再次尝试加载QWindowKit
        find_package(QWindowKit QUIET)
        if(NOT QWindowKit_FOUND)
            set(SARIBBON_USE_FRAMELESS_LIB OFF)
            message(WARNING "can not find package QWindowKit at ${__qwk_dir},set SARIBBON_USE_FRAMELESS_LIB=OFF")
        endif()
    else()
        message(STATUS "QWindowKit_DIR=${QWindowKit_DIR}")
        find_package(QWindowKit QUIET)
        if(NOT QWindowKit_FOUND)
            set(SARIBBON_USE_FRAMELESS_LIB OFF)
            message(WARNING "can not find package QWindowKit at ${__qwk_dir},set SARIBBON_USE_FRAMELESS_LIB=OFF")
        endif()
    endif()
endif()

include(cmake/WinResource.cmake)
add_subdirectory(src)

##################################
# install
##################################

# document-文档
set(SARIBBON_DOC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/readme.md
    ${CMAKE_CURRENT_SOURCE_DIR}/readme-cn.md
    ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE
    )
# 复制doc目录到install目录
install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/doc
    DESTINATION doc/SARibbon
    COMPONENT document
)
#  把文档复制过去
install(FILES
    ${SARIBBON_DOC_FILES}
    DESTINATION doc/SARibbon
    COMPONENT document
)

# 把针对cmake安装的两个pri文件复制到根目录下
set(SARIBBON_QMAKE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/common.pri
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/SARibbonBar.pri
    )
install(FILES
    ${SARIBBON_QMAKE_FILES}
    DESTINATION lib/qmake/SARibbonBar
    COMPONENT qmake
)
