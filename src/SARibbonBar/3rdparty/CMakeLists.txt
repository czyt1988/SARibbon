
# Cmake的命令不区分打下写，例如message，set等命令；但Cmake的变量区分大小写
# 为统一风格，本项目的Cmake命令全部采用小写，变量全部采用大写加下划线组合。

cmake_minimum_required(VERSION 3.5)
project(SARibbon-3rdparty-build
        LANGUAGES CXX
        DESCRIPTION "SARibbon : 3rdparty build"
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(MSVC)
    # CMAKE_CXX_STANDARD对有些版本的msvc无效
    set(CMAKE_CXX_FLAGS"${CMAKE_CXX_FLAGS} /std:c++17")
endif()
# 编译错误error C2039: “max”: 不是“std”的成员
# 避免和min/max宏冲突
add_definitions(-DNOMINMAX)

########################################################
# 安装路径设置
########################################################
# load Qt library, minimum version required is 5.12
# cn:Qt库加载,最低版本要求为5.12
set(SARIBBON_MIN_QT_VERSION 5.12)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} ${SARIBBON_MIN_QT_VERSION} COMPONENTS
    Core
    Gui
    Widgets
    REQUIRED
)
# 平台判断
if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
    set(SARIBBON_PLATFORM "x86")
else()
    set(SARIBBON_PLATFORM "x64")
endif()
# The bin file directory is one level above the current directory
# cn:bin文件目录在当前目录的上上一级
set(SARIBBON_BIN_NAME bin_qt${QT_VERSION}_${CMAKE_CXX_COMPILER_ID}_${SARIBBON_PLATFORM})
set(SARIBBON_BIN_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../${SARIBBON_BIN_NAME})
# windows系统下，默认直接安装到当前文件夹下
if(WIN32)
    set(CMAKE_INSTALL_PREFIX "${SARIBBON_BIN_DIR}")
endif()
########################################################
# 第三方库参数设置
########################################################
# 
set(QWINDOWKIT_BUILD_WIDGETS ON)
set(QWINDOWKIT_BUILD_EXAMPLES OFF)
set(QWINDOWKIT_BUILD_QUICK OFF)
set(QWINDOWKIT_INSTALL ON)
########################################################
# 安装第三方库
########################################################
add_subdirectory(qwindowkit)
