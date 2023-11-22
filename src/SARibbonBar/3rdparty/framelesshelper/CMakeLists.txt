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

cmake_minimum_required(VERSION 3.20)

project(FramelessHelper
    VERSION "2.5.0"
    DESCRIPTION "Cross-platform window customization framework for Qt Widgets and Qt Quick."
    HOMEPAGE_URL "https://github.com/wangwenx190/framelesshelper/"
)

include(CMakeDependentOption)
include(cmake/utils.cmake)

# TODO: Use add_feature_info() for every option below? Is it worth doing?
option(FRAMELESSHELPER_BUILD_STATIC "Build FramelessHelper as a static library." OFF)
option(FRAMELESSHELPER_BUILD_WIDGETS "Build FramelessHelper's Widgets module." ON)
option(FRAMELESSHELPER_BUILD_QUICK "Build FramelessHelper's Quick module." ON)
option(FRAMELESSHELPER_BUILD_EXAMPLES "Build FramelessHelper demo applications." OFF)
option(FRAMELESSHELPER_EXAMPLES_DEPLOYQT "Deploy the Qt framework after building the demo projects." OFF)
option(FRAMELESSHELPER_NO_DEBUG_OUTPUT "Suppress the debug messages from FramelessHelper." ON)
option(FRAMELESSHELPER_NO_BUNDLE_RESOURCE "Do not bundle any resources within FramelessHelper." OFF)
option(FRAMELESSHELPER_NO_PRIVATE "Do not use any private functionalities from Qt." OFF)
option(FRAMELESSHELPER_ENABLE_VCLTL "MSVC only: link to the system MSVCRT/UCRT and get rid of API sets." OFF)
option(FRAMELESSHELPER_ENABLE_YYTHUNKS "MSVC only: dynamic load most Win32 APIs to give better compatibility for old Windows versions." OFF)
option(FRAMELESSHELPER_NO_PERMISSIVE_CHECKS "MSVC only: disable the additional permissive checks." OFF)
option(FRAMELESSHELPER_NO_INSTALL "Don't install any files." OFF)
option(FRAMELESSHELPER_NO_SUMMARY "Don't show CMake configure summary." ON)
option(FRAMELESSHELPER_ENABLE_SPECTRE "Mitigate Spectre security vulnerabilities." OFF)
option(FRAMELESSHELPER_ENABLE_EHCONTGUARD "MSVC only: Enable EH Continuation (EHCONT) Metadata." OFF)
option(FRAMELESSHELPER_ENABLE_INTELCET "Enable Intel CET." OFF)
#option(FRAMELESSHELPER_ENABLE_INTELJCC "Enable Intel JCC." OFF) # Always enabled now.
option(FRAMELESSHELPER_ENABLE_CFGUARD "Enable Control Flow Guard (CFG)." OFF)
option(FRAMELESSHELPER_EXAMPLES_STANDALONE "Build the demo projects as standalone CMake projects." OFF)
cmake_dependent_option(FRAMELESSHELPER_ENABLE_UNIVERSAL_BUILD "macOS only: build universal library/example for Mac." ON APPLE OFF)
option(FRAMELESSHELPER_FORCE_LTO "Force enable LTO/LTCG even when building static libraries." OFF)
#option(FRAMELESSHELPER_REPRODUCIBLE_OUTPUT "Don't update the build commit and date dynamically." ON) # Always enabled now.
option(FRAMELESSHELPER_NO_WINDOW "Disable the pre-defined FramelessWindow wrapper class." OFF)
option(FRAMELESSHELPER_NO_TITLEBAR "Disable the pre-defined StandardTitleBar control." OFF)
option(FRAMELESSHELPER_NO_TRANSLATION "Don't bundle the I18N translations into the library." OFF)
option(FRAMELESSHELPER_NO_MICA_MATERIAL "Disable the cross-platform homemade Mica Material." OFF)
option(FRAMELESSHELPER_NO_BORDER_PAINTER "Disable the cross-platform window frame border painter." OFF)
option(FRAMELESSHELPER_NO_SYSTEM_BUTTON "Disable the pre-defined StandardSystemButton control." OFF)
cmake_dependent_option(FRAMELESSHELPER_NATIVE_IMPL "Use platform native implementation instead of Qt to get best experience." ON WIN32 OFF)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui)

find_package(QT NAMES Qt6 Qt5 QUIET COMPONENTS Widgets Quick)
find_package(Qt${QT_VERSION_MAJOR} QUIET COMPONENTS Widgets Quick)

if(FRAMELESSHELPER_NATIVE_IMPL AND NOT WIN32)
    message(WARNING "FRAMELESSHELPER_NATIVE_IMPL currently only supports the Windows platform!")
    set(FRAMELESSHELPER_NATIVE_IMPL OFF)
endif()

if(FRAMELESSHELPER_NO_WINDOW AND FRAMELESSHELPER_BUILD_EXAMPLES)
    message(WARNING "You can't build the examples when the FramelessWindow class is disabled at the same time!")
    set(FRAMELESSHELPER_BUILD_EXAMPLES OFF)
endif()

if(FRAMELESSHELPER_ENABLE_UNIVERSAL_BUILD AND NOT APPLE)
    message(WARNING "Universal build is a macOS only feature, it will be disabled on current platform.")
    set(FRAMELESSHELPER_ENABLE_UNIVERSAL_BUILD OFF)
elseif(APPLE AND ((QT_VERSION VERSION_LESS "6.2" AND QT_VERSION VERSION_GREATER_EQUAL "6.0") OR (QT_VERSION VERSION_LESS "5.15.9")))
    message(WARNING "Your Qt version ${QT_VERSION} doesn't support universal build, it will be disabled.")
    set(FRAMELESSHELPER_ENABLE_UNIVERSAL_BUILD OFF)
endif()

if(FRAMELESSHELPER_NO_BUNDLE_RESOURCE)
    message(WARNING "Nothing will be embeded into the FramelessHelper library, the chrome buttons will have no icon.")
endif()

if(FRAMELESSHELPER_ENABLE_VCLTL AND NOT MSVC)
    message(WARNING "VC-LTL is only available for the MSVC toolchain.")
    set(FRAMELESSHELPER_ENABLE_VCLTL OFF)
endif()

if(FRAMELESSHELPER_ENABLE_YYTHUNKS AND NOT MSVC)
    message(WARNING "YY-Thunks is only available for the MSVC toolchain.")
    set(FRAMELESSHELPER_ENABLE_YYTHUNKS OFF)
endif()

if(NOT TARGET Qt${QT_VERSION_MAJOR}::Core OR NOT TARGET Qt${QT_VERSION_MAJOR}::Gui)
    message(WARNING "Can't find the QtCore and/or QtGui module. Nothing will be built.")
    set(FRAMELESSHELPER_BUILD_WIDGETS OFF)
    set(FRAMELESSHELPER_BUILD_QUICK OFF)
    set(FRAMELESSHELPER_BUILD_EXAMPLES OFF)
endif()

if(FRAMELESSHELPER_BUILD_QUICK AND NOT TARGET Qt${QT_VERSION_MAJOR}::Quick)
    message(WARNING "Can't find the QtQuick module. FramelessHelper's QtQuick implementation and the QtQuick demo won't be built.")
    set(FRAMELESSHELPER_BUILD_QUICK OFF)
endif()

set(FRAMELESSHELPER_64BIT_POSTFIX "")
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(FRAMELESSHELPER_64BIT_POSTFIX "64")
endif()

set(FRAMELESSHELPER_LICENSE_HEADER "/*
 * MIT License
 *
 * Copyright (C) 2021-2023 by wangwenx190 (Yuhang Zhao)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the \"Software\"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */")

set(__extra_flags "")
if(NOT FRAMELESSHELPER_BUILD_STATIC)
    list(APPEND __extra_flags ENABLE_LTO)
endif()
setup_project(
    QT_PROJECT
    QML_IMPORT_DIR "${PROJECT_BINARY_DIR}/imports"
    LICENSE_HEADER "${FRAMELESSHELPER_LICENSE_HEADER}"
    LANGUAGES CXX RC
    MAX_WARNING
    RTTI
    EXCEPTIONS
    ${__extra_flags}
)
unset(__extra_flags)

if(MINGW AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(FRAMELESSHELPER_ENABLE_SPECTRE OFF)
    set(FRAMELESSHELPER_ENABLE_EHCONTGUARD OFF)
    set(FRAMELESSHELPER_ENABLE_INTELCET OFF)
    #set(FRAMELESSHELPER_ENABLE_INTELJCC OFF)
    set(FRAMELESSHELPER_ENABLE_CFGUARD OFF)
endif()

if(FRAMELESSHELPER_ENABLE_VCLTL)
    include(cmake/VC-LTL.cmake)
    if("x${SupportLTL}" STREQUAL "xtrue")
        # Make sure we will always overwrite the previous settings.
        unset(CMAKE_MSVC_RUNTIME_LIBRARY)
        unset(CMAKE_MSVC_RUNTIME_LIBRARY CACHE)
        #unset(CMAKE_MSVC_RUNTIME_LIBRARY PARENT_SCOPE)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE STRING "" FORCE)
    endif()
endif()
if(FRAMELESSHELPER_ENABLE_YYTHUNKS)
    unset(YYTHUNKS_TARGET_OS)
    unset(YYTHUNKS_TARGET_OS CACHE)
    #unset(YYTHUNKS_TARGET_OS PARENT_SCOPE)
    set(YYTHUNKS_TARGET_OS "WinXP" CACHE STRING "" FORCE)
    include(cmake/YY-Thunks.cmake)
endif()

set(__extra_flags "")
if(FRAMELESSHELPER_NO_INSTALL)
    list(APPEND __extra_flags NO_INSTALL)
endif()
prepare_package_export(
    PACKAGE_NAME "${PROJECT_NAME}"
    PACKAGE_VERSION "${PROJECT_VERSION}"
    ${__extra_flags}
)
unset(__extra_flags)

set(FRAMELESSHELPER_VERSION_FILE "${PROJECT_BINARY_DIR}/framelesshelper.version")
generate_project_version(
    PATH "${FRAMELESSHELPER_VERSION_FILE}"
    COMMIT_HASH COMMIT_SUBJECT COMMIT_AUTHOR
    COMMIT_DATETIME COMMIT_BRANCH COMPILER_NAME
    COMPILER_VENDOR COMPILER_VERSION BUILD_DATETIME
    ARCHITECTURE CMAKE_VERSION GENERATOR
)

set(FRAMELESSHELPER_CONFIG_FILE "${PROJECT_BINARY_DIR}/framelesshelper.config")
add_project_config(KEY "static_build" CONDITION FRAMELESSHELPER_BUILD_STATIC)
add_project_config(KEY "widgets" CONDITION FRAMELESSHELPER_BUILD_WIDGETS)
add_project_config(KEY "quick" CONDITION FRAMELESSHELPER_BUILD_QUICK)
add_project_config(KEY "debug_output" CONDITION NOT FRAMELESSHELPER_NO_DEBUG_OUTPUT)
add_project_config(KEY "bundle_resource" CONDITION NOT FRAMELESSHELPER_NO_BUNDLE_RESOURCE)
add_project_config(KEY "private_qt" CONDITION NOT FRAMELESSHELPER_NO_PRIVATE)
add_project_config(KEY "window" CONDITION NOT FRAMELESSHELPER_NO_WINDOW)
add_project_config(KEY "titlebar" CONDITION NOT FRAMELESSHELPER_NO_TITLEBAR)
add_project_config(KEY "translation" CONDITION NOT FRAMELESSHELPER_NO_TRANSLATION)
add_project_config(KEY "mica_material" CONDITION NOT FRAMELESSHELPER_NO_MICA_MATERIAL)
add_project_config(KEY "border_painter" CONDITION NOT FRAMELESSHELPER_NO_BORDER_PAINTER)
add_project_config(KEY "system_button" CONDITION NOT FRAMELESSHELPER_NO_SYSTEM_BUTTON)
add_project_config(KEY "native_impl" CONDITION FRAMELESSHELPER_NATIVE_IMPL)
generate_project_config(PATH "${FRAMELESSHELPER_CONFIG_FILE}")

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

if(FRAMELESSHELPER_BUILD_WIDGETS OR FRAMELESSHELPER_BUILD_QUICK)
    add_subdirectory(src)
endif()

if(FRAMELESSHELPER_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(WIN32 AND NOT FRAMELESSHELPER_NO_INSTALL)
    set(__data_dir ".")
    compute_install_dir(DATA_DIR __data_dir)
    install(FILES "msbuild/FramelessHelper.props" DESTINATION "${__data_dir}")
endif()

if(NOT FRAMELESSHELPER_NO_SUMMARY)
    message("--------------------------- Toolchain ----------------------------")
    message("CMake version: ${CMAKE_VERSION} (${CMAKE_COMMAND})")
    message("Host system: ${CMAKE_HOST_SYSTEM}")
    message("Host processor: ${CMAKE_HOST_SYSTEM_PROCESSOR}")
    #[[message("C compiler: ${CMAKE_C_COMPILER_ID} (${CMAKE_C_COMPILER})") # Currently we are not using any C compilers.
    message("C compiler version: ${CMAKE_C_COMPILER_VERSION}")
    message("C common flags: ${CMAKE_C_FLAGS}")
    message("C debug flags: ${CMAKE_C_FLAGS_DEBUG}")
    message("C release flags: ${CMAKE_C_FLAGS_RELEASE}")
    message("C minsizerel flags: ${CMAKE_C_FLAGS_MINSIZEREL}")
    message("C relwithdebinfo flags: ${CMAKE_C_FLAGS_RELWITHDEBINFO}")]]
    message("C++ compiler: ${CMAKE_CXX_COMPILER_ID} (${CMAKE_CXX_COMPILER})")
    message("C++ compiler version: ${CMAKE_CXX_COMPILER_VERSION}")
    message("C++ common flags: ${CMAKE_CXX_FLAGS}")
    message("C++ debug flags: ${CMAKE_CXX_FLAGS_DEBUG}")
    message("C++ release flags: ${CMAKE_CXX_FLAGS_RELEASE}")
    message("C++ minsizerel flags: ${CMAKE_CXX_FLAGS_MINSIZEREL}")
    message("C++ relwithdebinfo flags: ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
    message("Linker: ${CMAKE_LINKER}")
    message("Linker exe common flags: ${CMAKE_EXE_LINKER_FLAGS}")
    message("Linker exe debug flags: ${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
    message("Linker exe release flags: ${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
    message("Linker exe minsizerel flags: ${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL}")
    message("Linker exe relwithdebinfo flags: ${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO}")
    message("Linker dll common flags: ${CMAKE_SHARED_LINKER_FLAGS}")
    message("Linker dll debug flags: ${CMAKE_SHARED_LINKER_FLAGS_DEBUG}")
    message("Linker dll release flags: ${CMAKE_SHARED_LINKER_FLAGS_RELEASE}")
    message("Linker dll minsizerel flags: ${CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL}")
    message("Linker dll relwithdebinfo flags: ${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO}")
    message("Make program: ${CMAKE_MAKE_PROGRAM}")
    message("Generator: ${CMAKE_GENERATOR}")
    message("Build type: ${CMAKE_BUILD_TYPE}")
    message("Configuration types: ${CMAKE_CONFIGURATION_TYPES}")
    message("Install prefix: ${CMAKE_INSTALL_PREFIX}")
    message("Prefix paths: ${CMAKE_PREFIX_PATH}")
    message("Toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
    message("------------------------------ Qt -------------------------------")
    query_qt_paths(SDK_DIR __qt_inst_dir)
    query_qt_library_info(STATIC __qt_static_lib)
    message("Qt SDK installation directory: ${__qt_inst_dir}")
    message("Qt SDK version: ${QT_VERSION}")
    if(__qt_static_lib)
        message("Qt SDK library type: static")
    else()
        message("Qt SDK library type: shared")
    endif()
    message("------------------------ FramelessHelper ------------------------")
    message("FramelessHelper version: ${PROJECT_VERSION}")
    message("Build the static version of FramelessHelper: ${FRAMELESSHELPER_BUILD_STATIC}")
    message("Build the FramelessHelper::Widgets module: ${FRAMELESSHELPER_BUILD_WIDGETS}")
    message("Build the FramelessHelper::Quick module: ${FRAMELESSHELPER_BUILD_QUICK}")
    message("Build the FramelessHelper demo applications: ${FRAMELESSHELPER_BUILD_EXAMPLES}")
    message("Deploy Qt libraries after compilation: ${FRAMELESSHELPER_EXAMPLES_DEPLOYQT}")
    message("Suppress debug messages from FramelessHelper: ${FRAMELESSHELPER_NO_DEBUG_OUTPUT}")
    message("Do not bundle any resources within FramelessHelper: ${FRAMELESSHELPER_NO_BUNDLE_RESOURCE}")
    message("Do not use any private functionalities from Qt: ${FRAMELESSHELPER_NO_PRIVATE}")
    message("[MSVC] Link to system C runtime library: ${FRAMELESSHELPER_ENABLE_VCLTL}")
    message("[MSVC] Thunk system APIs for old system: ${FRAMELESSHELPER_ENABLE_YYTHUNKS}")
    message("[MSVC] Disable permissive checks: ${FRAMELESSHELPER_NO_PERMISSIVE_CHECKS}")
    message("Do not install anything for CMake install: ${FRAMELESSHELPER_NO_INSTALL}")
    message("Mitigate Spectre security vulnerabilities: ${FRAMELESSHELPER_ENABLE_SPECTRE}")
    message("[MSVC] Enable EH Continuation (EHCONT) Metadata: ${FRAMELESSHELPER_ENABLE_EHCONTGUARD}")
    message("Enable Intel CET: ${FRAMELESSHELPER_ENABLE_INTELCET}")
    #message("Enable Intel JCC: ${FRAMELESSHELPER_ENABLE_INTELJCC}")
    message("Enable Control Flow Guard (CFG): ${FRAMELESSHELPER_ENABLE_CFGUARD}")
    message("Build standalone demo projects: ${FRAMELESSHELPER_EXAMPLES_STANDALONE}")
    message("[macOS]: Build universal library/example: ${FRAMELESSHELPER_ENABLE_UNIVERSAL_BUILD}")
    message("Force enable LTO: ${FRAMELESSHELPER_FORCE_LTO}")
    #message("Make output reproducible: ${FRAMELESSHELPER_REPRODUCIBLE_OUTPUT}")
    message("Disable the FramelessWindow class (to reduce file size): ${FRAMELESSHELPER_NO_WINDOW}")
    message("Disable the StandardTitleBar class (to reduce file size): ${FRAMELESSHELPER_NO_TITLEBAR}")
    message("Don't embed the I18N resources (to reduce file size): ${FRAMELESSHELPER_NO_TRANSLATION}")
    message("Disable the MicaMaterial class (to reduce file size): ${FRAMELESSHELPER_NO_MICA_MATERIAL}")
    message("Disable the WindowBorderPainter class (to reduce file size): ${FRAMELESSHELPER_NO_BORDER_PAINTER}")
    message("Disable the StandardSystemButton class (to reduce file size): ${FRAMELESSHELPER_NO_SYSTEM_BUTTON}")
    message("-----------------------------------------------------------------")
endif()
