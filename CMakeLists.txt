cmake_minimum_required(VERSION 3.5)

set(SARIBBON_VERSION_MAJOR 0)
set(SARIBBON_VERSION_MINOR 4)
set(SARIBBON_VERSION_PATCH 0)
set(SARIBBON_VERSION "${SARIBBON_VERSION_MAJOR}.${SARIBBON_VERSION_MINOR}.${SARIBBON_VERSION_PATCH}")
project(SARibbon VERSION ${SARIBBON_VERSION} LANGUAGES CXX)

# option(BUILD_SHARED_LIBS "build the SARibbonBar in shared lib mode" ON)
option(BUILD_EXAMPLES "build the examples" OFF)
option(BUILD_DESIGNERPLUGIN "build the Designer Plugin" OFF)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "add a postfix, usually d on windows")
set(CMAKE_RELEASE_POSTFIX "" CACHE STRING "add a postfix, usually empty on windows")
set(CMAKE_RELWITHDEBINFO_POSTFIX "rd" CACHE STRING "add a postfix, usually empty on windows")
set(CMAKE_MINSIZEREL_POSTFIX "s" CACHE STRING "add a postfix, usually empty on windows")

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4819")
endif()

include(CPack)

message(STATUS "SARibbon ${SARIBBON_VERSION}")
# 平台判断
if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
    set(SARIBBON_PLATFORM "86")
else()
    set(SARIBBON_PLATFORM "64")
endif()

# load Qt library, minimum version required is 5.7
# cn:Qt库加载,最低版本要求为5.7
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} 5.8 COMPONENTS Core Gui Widgets REQUIRED)
message(STATUS "current Qt version is Qt${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}")



include(cmake/WinResource.cmake)

# The bin file directory is one level above the current directory
# cn:bin文件目录在当前目录的上上一级
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(SARIBBON_BUILD_TYPE debug)
elseif(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    set(SARIBBON_BUILD_TYPE release)
else()
    set(SARIBBON_BUILD_TYPE unknow)
endif()
set(SARIBBON_BIN_NAME bin_qt${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}_${SARIBBON_BUILD_TYPE}_${SARIBBON_PLATFORM})
set(SARIBBON_BIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${SARIBBON_BIN_NAME})


add_subdirectory(src)
add_subdirectory(cmake)
