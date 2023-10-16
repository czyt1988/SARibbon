#[[
  MIT License

  Copyright (C) 2021-2023 by wangwenx190 (Yuhang Zhao)

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
]]

if(FRAMELESSHELPER_ENABLE_UNIVERSAL_BUILD)
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "" FORCE)
endif()

if(UNIX AND NOT APPLE)
    if(FRAMELESSHELPER_NO_PRIVATE)
        # Qt X11Extras is only available in Qt5.
        if(QT_VERSION_MAJOR LESS 6)
            find_package(Qt5 QUIET COMPONENTS X11Extras)
        endif()
    endif()
    find_package(X11 QUIET COMPONENTS xcb)
    if(TARGET X11::xcb)
        message(STATUS "--- Found system XCB. The XCB wrapper will be disabled.")
    else()
        message(STATUS "--- System XCB not found. The XCB wrapper will be used instead.")
    endif()
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(GTK3 QUIET IMPORTED_TARGET gtk+-3.0)
    endif()
    if(TARGET PkgConfig::GTK3)
        message(STATUS "--- Found system GTK. The GTK wrapper will be disabled.")
    else()
        message(STATUS "--- System GTK not found. The GTK wrapper will be used instead.")
    endif()
endif()

set(SUB_MODULE Core)
set(SUB_MODULE_FULL_NAME ${PROJECT_NAME}${SUB_MODULE})
set(SUB_MODULE_TARGET ${SUB_MODULE_FULL_NAME})
set(SUB_MODULE_PATH ${PROJECT_NAME}/${SUB_MODULE})

set(INCLUDE_PREFIX ../../include/${SUB_MODULE_PATH})

set(PUBLIC_HEADERS
    ${FRAMELESSHELPER_VERSION_FILE}
    ${FRAMELESSHELPER_CONFIG_FILE}
    ${INCLUDE_PREFIX}/framelesshelpercore_global.h
    ${INCLUDE_PREFIX}/framelessmanager.h
    ${INCLUDE_PREFIX}/utils.h
)

set(PUBLIC_HEADERS_ALIAS
    ${INCLUDE_PREFIX}/Global
    ${INCLUDE_PREFIX}/FramelessManager
    ${INCLUDE_PREFIX}/Utils
)

set(PRIVATE_HEADERS
    ${INCLUDE_PREFIX}/private/framelessmanager_p.h
    ${INCLUDE_PREFIX}/private/framelessconfig_p.h
    ${INCLUDE_PREFIX}/private/sysapiloader_p.h
    ${INCLUDE_PREFIX}/private/framelesshelpercore_global_p.h
    ${INCLUDE_PREFIX}/private/versionnumber_p.h
    ${INCLUDE_PREFIX}/private/scopeguard_p.h
)

set(SOURCES
    utils.cpp
    framelessmanager.cpp
    framelessconfig.cpp
    sysapiloader.cpp
    framelesshelpercore_global.cpp
)

if(WIN32)
    list(APPEND PUBLIC_HEADERS ${INCLUDE_PREFIX}/framelesshelper_windows.h)
    list(APPEND PUBLIC_HEADERS_ALIAS ${INCLUDE_PREFIX}/FramelessHelper_Windows)
    list(APPEND PRIVATE_HEADERS
        ${INCLUDE_PREFIX}/private/registrykey_p.h
        ${INCLUDE_PREFIX}/private/winverhelper_p.h
    )
    list(APPEND SOURCES
        registrykey.cpp
        utils_win.cpp
        winverhelper.cpp
        platformsupport_win.cpp
    )
elseif(APPLE)
    list(APPEND SOURCES utils_mac.mm)
elseif(UNIX)
    list(APPEND PUBLIC_HEADERS ${INCLUDE_PREFIX}/framelesshelper_linux.h)
    list(APPEND PUBLIC_HEADERS_ALIAS ${INCLUDE_PREFIX}/FramelessHelper_Linux)
    list(APPEND SOURCES
        utils_linux.cpp
        platformsupport_linux.cpp
    )
endif()

if(FRAMELESSHELPER_NATIVE_IMPL)
    if(WIN32)
        list(APPEND PUBLIC_HEADERS ${INCLUDE_PREFIX}/framelesshelper_win.h)
        list(APPEND PUBLIC_HEADERS_ALIAS ${INCLUDE_PREFIX}/FramelessHelper_Win)
        list(APPEND SOURCES framelesshelper_win.cpp)
    elseif(APPLE)
    elseif(UNIX)
    else()
    endif()
else()
    list(APPEND PUBLIC_HEADERS ${INCLUDE_PREFIX}/framelesshelper_qt.h)
    list(APPEND PUBLIC_HEADERS_ALIAS ${INCLUDE_PREFIX}/FramelessHelper_Qt)
    list(APPEND SOURCES framelesshelper_qt.cpp)
endif()

if(NOT FRAMELESSHELPER_NO_TITLEBAR)
    list(APPEND PUBLIC_HEADERS ${INCLUDE_PREFIX}/chromepalette.h)
    list(APPEND PUBLIC_HEADERS_ALIAS ${INCLUDE_PREFIX}/ChromePalette)
    list(APPEND PRIVATE_HEADERS ${INCLUDE_PREFIX}/private/chromepalette_p.h)
    list(APPEND SOURCES chromepalette.cpp)
endif()

if(NOT FRAMELESSHELPER_NO_MICA_MATERIAL)
    list(APPEND PUBLIC_HEADERS ${INCLUDE_PREFIX}/micamaterial.h)
    list(APPEND PUBLIC_HEADERS_ALIAS ${INCLUDE_PREFIX}/MicaMaterial)
    list(APPEND PRIVATE_HEADERS ${INCLUDE_PREFIX}/private/micamaterial_p.h)
    list(APPEND SOURCES micamaterial.cpp)
endif()

if(NOT FRAMELESSHELPER_NO_BORDER_PAINTER)
    list(APPEND PUBLIC_HEADERS ${INCLUDE_PREFIX}/windowborderpainter.h)
    list(APPEND PUBLIC_HEADERS_ALIAS ${INCLUDE_PREFIX}/WindowBorderPainter)
    list(APPEND PRIVATE_HEADERS ${INCLUDE_PREFIX}/private/windowborderpainter_p.h)
    list(APPEND SOURCES windowborderpainter.cpp)
endif()

if(WIN32 AND NOT FRAMELESSHELPER_BUILD_STATIC)
    set(__rc_path "${CMAKE_CURRENT_BINARY_DIR}/${SUB_MODULE_FULL_NAME}.rc")
    if(NOT EXISTS "${__rc_path}")
        generate_win32_rc_file(
            PATH "${__rc_path}"
            VERSION "${PROJECT_VERSION}"
            COMPANY "wangwenx190"
            DESCRIPTION "${PROJECT_NAME} ${SUB_MODULE} Module"
            COPYRIGHT "MIT License"
            ORIGINAL_FILENAME "${SUB_MODULE_FULL_NAME}.dll"
            PRODUCT "${PROJECT_NAME}"
            COMMENTS "Who don't love Raiden Shogun ?"
            LIBRARY
        )
    endif()
    list(APPEND SOURCES "${__rc_path}")
endif()

set(ALL_SOURCES ${PUBLIC_HEADERS} ${PRIVATE_HEADERS} ${SOURCES})

if(FRAMELESSHELPER_BUILD_STATIC)
    set(SUB_MOD_LIB_TYPE "STATIC")
else()
    set(SUB_MOD_LIB_TYPE "SHARED")
endif()
add_library(${SUB_MODULE_TARGET} ${SUB_MOD_LIB_TYPE} ${ALL_SOURCES})
add_library(${PROJECT_NAME}::${SUB_MODULE} ALIAS ${SUB_MODULE_TARGET})
add_library(${PROJECT_NAME}::${SUB_MODULE_FULL_NAME} ALIAS ${SUB_MODULE_TARGET})

set_target_properties(${SUB_MODULE_TARGET} PROPERTIES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    OUTPUT_NAME "${SUB_MODULE_TARGET}${FRAMELESSHELPER_64BIT_POSTFIX}"
)

set(__export_targets ${SUB_MODULE_TARGET})

if(NOT FRAMELESSHELPER_NO_BUNDLE_RESOURCE)
    if(QT_VERSION VERSION_GREATER_EQUAL "6.2")
        qt_add_resources(${SUB_MODULE_TARGET} framelesshelpercore
            PREFIX
                "/org.wangwenx190.${PROJECT_NAME}"
            FILES
                "resources/fonts/iconfont.ttf"
                "resources/images/noise.png"
            OUTPUT_TARGETS __qrc_targets
        )
        if(__qrc_targets)
            list(APPEND __export_targets ${__qrc_targets})
            if(FRAMELESSHELPER_BUILD_STATIC)
                foreach(__target ${__qrc_targets})
                    target_sources(${SUB_MODULE_TARGET} PRIVATE
                        $<TARGET_OBJECTS:${__target}>
                    )
                endforeach()
            endif()
        endif()
    else()
        target_sources(${SUB_MODULE_TARGET} PRIVATE
            framelesshelpercore.qrc
        )
    endif()
endif()

if(DEFINED FRAMELESSHELPER_NAMESPACE)
    if("x${FRAMELESSHELPER_NAMESPACE}" STREQUAL "x")
        message(FATAL_ERROR "FRAMELESSHELPER_NAMESPACE can't be empty!")
    endif()
    target_compile_definitions(${SUB_MODULE_TARGET} PUBLIC FRAMELESSHELPER_NAMESPACE=${FRAMELESSHELPER_NAMESPACE})
endif()

target_compile_definitions(${SUB_MODULE_TARGET} PRIVATE
    FRAMELESSHELPER_CORE_LIBRARY
)

if(APPLE)
    target_link_libraries(${SUB_MODULE_TARGET} PRIVATE
        "-framework Foundation"
        "-framework Cocoa"
        "-framework AppKit"
    )
elseif(UNIX)
    if(TARGET X11::xcb)
        target_link_libraries(${SUB_MODULE_TARGET} PRIVATE
            X11::xcb
        )
    endif()
    if(TARGET PkgConfig::GTK3)
        target_link_libraries(${SUB_MODULE_TARGET} PRIVATE
            PkgConfig::GTK3
        )
        target_compile_definitions(${SUB_MODULE_TARGET} PRIVATE
            GDK_VERSION_MIN_REQUIRED=GDK_VERSION_3_6
        )
    endif()
endif()

if(FRAMELESSHELPER_NO_PRIVATE)
    target_link_libraries(${SUB_MODULE_TARGET} PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
    )
    # Qt X11Extras was first introduced in 5.1 and got removed in 6.0
    # But it was again brought back as a private feature of QtGui in 6.2
    if(TARGET Qt5::X11Extras)
        target_link_libraries(${SUB_MODULE_TARGET} PRIVATE
            Qt5::X11Extras
        )
    endif()
else()
    target_link_libraries(${SUB_MODULE_TARGET} PRIVATE
        Qt${QT_VERSION_MAJOR}::CorePrivate
        Qt${QT_VERSION_MAJOR}::GuiPrivate
    )
endif()

include(GNUInstallDirs)
target_include_directories(${SUB_MODULE_TARGET} PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${INCLUDE_PREFIX}/../..>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${INCLUDE_PREFIX}>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${INCLUDE_PREFIX}/private>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
    "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${SUB_MODULE_PATH}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${SUB_MODULE_PATH}/private>"
)

if(NOT FRAMELESSHELPER_BUILD_STATIC)
    setup_target_rpaths(TARGETS ${SUB_MODULE_TARGET})
endif()

setup_qt_stuff(TARGETS ${SUB_MODULE_TARGET})

set(__extra_flags "")
if(NOT FRAMELESSHELPER_NO_PERMISSIVE_CHECKS)
    list(APPEND __extra_flags PERMISSIVE)
endif()
if(FRAMELESSHELPER_ENABLE_SPECTRE)
    list(APPEND __extra_flags SPECTRE)
endif()
if(FRAMELESSHELPER_ENABLE_EHCONTGUARD)
    list(APPEND __extra_flags EHCONTGUARD)
endif()
if(FRAMELESSHELPER_ENABLE_INTELCET)
    list(APPEND __extra_flags INTELCET)
endif()
if(FRAMELESSHELPER_ENABLE_CFGUARD)
    list(APPEND __extra_flags CFGUARD)
endif()
if(FRAMELESSHELPER_FORCE_LTO)
    list(APPEND __extra_flags FORCE_LTO)
endif()
setup_compile_params(TARGETS ${SUB_MODULE_TARGET} ${__extra_flags})

if(NOT FRAMELESSHELPER_NO_INSTALL)
    setup_package_export(
        TARGETS ${__export_targets}
        NAMESPACE ${PROJECT_NAME}
        PACKAGE_NAME ${PROJECT_NAME}
        COMPONENT ${SUB_MODULE}
        PUBLIC_HEADERS ${PUBLIC_HEADERS}
        ALIAS_HEADERS ${PUBLIC_HEADERS_ALIAS}
        PRIVATE_HEADERS ${PRIVATE_HEADERS}
    )
endif()

if(NOT FRAMELESSHELPER_NO_SUMMARY)
    dump_target_info(TARGETS ${SUB_MODULE_TARGET})
endif()
