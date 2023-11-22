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

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS QuickTemplates2 QuickControls2)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS QuickTemplates2 QuickControls2)

set(SUB_MODULE Quick)
set(SUB_MODULE_FULL_NAME ${PROJECT_NAME}${SUB_MODULE})
set(SUB_MODULE_TARGET ${SUB_MODULE_FULL_NAME})
set(SUB_MODULE_PATH ${PROJECT_NAME}/${SUB_MODULE})

set(INCLUDE_PREFIX ../../include/${SUB_MODULE_PATH})

set(PUBLIC_HEADERS
    ${INCLUDE_PREFIX}/framelesshelperquick_global.h
    ${INCLUDE_PREFIX}/framelessquickmodule.h
    ${INCLUDE_PREFIX}/framelessquickhelper.h
    ${INCLUDE_PREFIX}/framelessquickutils.h
)

set(PUBLIC_HEADERS_ALIAS
    ${INCLUDE_PREFIX}/Global
    ${INCLUDE_PREFIX}/FramelessQuickModule
    ${INCLUDE_PREFIX}/FramelessQuickHelper
    ${INCLUDE_PREFIX}/FramelessQuickUtils
)

set(PRIVATE_HEADERS
    ${INCLUDE_PREFIX}/private/framelessquickhelper_p.h
    ${INCLUDE_PREFIX}/private/quickimageitem_p.h
)

set(SOURCES
    framelessquickutils.cpp
    framelessquickhelper.cpp
    framelesshelperquick_global.cpp
    quickimageitem.cpp
)

if(NOT FRAMELESSHELPER_NO_SYSTEM_BUTTON)
    list(APPEND PRIVATE_HEADERS ${INCLUDE_PREFIX}/private/quickstandardsystembutton_p.h)
    list(APPEND SOURCES quickstandardsystembutton.cpp)
endif()

if(NOT FRAMELESSHELPER_NO_TITLEBAR)
    list(APPEND PUBLIC_HEADERS ${INCLUDE_PREFIX}/quickchromepalette.h)
    list(APPEND PUBLIC_HEADERS_ALIAS ${INCLUDE_PREFIX}/QuickChromePalette)
    list(APPEND PRIVATE_HEADERS ${INCLUDE_PREFIX}/private/quickstandardtitlebar_p.h)
    list(APPEND SOURCES quickchromepalette.cpp quickstandardtitlebar.cpp)
endif()

if(NOT FRAMELESSHELPER_NO_WINDOW)
    list(APPEND PRIVATE_HEADERS
        ${INCLUDE_PREFIX}/private/framelessquickwindow_p.h
        ${INCLUDE_PREFIX}/private/framelessquickwindow_p_p.h
        ${INCLUDE_PREFIX}/private/framelessquickapplicationwindow_p.h
        ${INCLUDE_PREFIX}/private/framelessquickapplicationwindow_p_p.h
    )
    list(APPEND SOURCES framelessquickwindow.cpp framelessquickapplicationwindow.cpp)
endif()

if(NOT FRAMELESSHELPER_NO_MICA_MATERIAL)
    list(APPEND PUBLIC_HEADERS ${INCLUDE_PREFIX}/quickmicamaterial.h)
    list(APPEND PUBLIC_HEADERS_ALIAS ${INCLUDE_PREFIX}/QuickMicaMaterial)
    list(APPEND PRIVATE_HEADERS ${INCLUDE_PREFIX}/private/quickmicamaterial_p.h)
    list(APPEND SOURCES quickmicamaterial.cpp)
endif()

if(NOT FRAMELESSHELPER_NO_BORDER_PAINTER)
    list(APPEND PUBLIC_HEADERS ${INCLUDE_PREFIX}/quickwindowborder.h)
    list(APPEND PUBLIC_HEADERS_ALIAS ${INCLUDE_PREFIX}/QuickWindowBorder)
    list(APPEND PRIVATE_HEADERS ${INCLUDE_PREFIX}/private/quickwindowborder_p.h)
    list(APPEND SOURCES quickwindowborder.cpp)
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

set(__import_base_dir "${PROJECT_BINARY_DIR}/imports")
if(DEFINED FRAMELESSHELPER_IMPORT_DIR)
    set(__import_base_dir "${FRAMELESSHELPER_IMPORT_DIR}")
endif()

# qt_add_qml_module() was introduced in Qt 6.2 but qt_query_qml_module()
# was introduced in 6.3, to simplify the CMake code, I decide to limit the
# version check to 6.3, otherwise we'll need a lot of code to query and
# calculate the generated files, which will also break Ninja Multi-Config
# builds.
if(QT_VERSION VERSION_GREATER_EQUAL "6.3")
    qt_add_qml_module(${SUB_MODULE_TARGET}
        URI "org.wangwenx190.${PROJECT_NAME}"
        VERSION "1.0"
        OUTPUT_DIRECTORY "${__import_base_dir}/org/wangwenx190/${PROJECT_NAME}"
        RESOURCE_PREFIX "/"
        TYPEINFO "plugins.qmltypes" # This is the default type info file name since at least Qt5, some tools won't recognize other names.
        #NO_RESOURCE_TARGET_PATH # Can't be used for non-executable targets.
        OUTPUT_TARGETS __qml_targets
        IMPORTS
            QtQml
            QtQuick
            QtQuick.Controls.Basic
    )
    if(__qml_targets)
        list(APPEND __export_targets ${__qml_targets})
        # We have some resources bundled into the library,
        # however, for static builds, the object files will
        # not be packed into our final static library file,
        # and thus it causes linker errors for our users,
        # so we need this hack here.
        if(FRAMELESSHELPER_BUILD_STATIC)
            foreach(__target ${__qml_targets})
                target_sources(${SUB_MODULE_TARGET} PRIVATE
                    $<TARGET_OBJECTS:${__target}>
                )
            endforeach()
        endif()
    endif()
    if(NOT FRAMELESSHELPER_NO_INSTALL)
        qt_query_qml_module(${SUB_MODULE_TARGET}
            URI module_uri
            VERSION module_version
            PLUGIN_TARGET module_plugin_target
            TARGET_PATH module_target_path
            QMLDIR module_qmldir
            TYPEINFO module_typeinfo
            #QML_FILES module_qml_files
            #QML_FILES_DEPLOY_PATHS module_qml_files_deploy_paths
            #RESOURCES module_resources
            #RESOURCES_DEPLOY_PATHS module_resources_deploy_paths
        )
        if(module_target_path)
            set(__qml_plugin_dir "qml/${module_target_path}")
            if(module_plugin_target)
                install(TARGETS ${module_plugin_target}
                    RUNTIME DESTINATION "${__qml_plugin_dir}"
                    LIBRARY DESTINATION "${__qml_plugin_dir}"
                    ARCHIVE DESTINATION "${__qml_plugin_dir}"
                )
            endif()
            if(module_qmldir)
                install(FILES "${module_qmldir}" DESTINATION "${__qml_plugin_dir}")
            endif()
            if(module_typeinfo)
                install(FILES "${module_typeinfo}" DESTINATION "${__qml_plugin_dir}")
            endif()
            if(module_qml_files)
                list(LENGTH module_qml_files num_files)
                math(EXPR last_index "${num_files} - 1")
                foreach(i RANGE 0 ${last_index})
                    list(GET module_qml_files ${i} src_file)
                    list(GET module_qml_files_deploy_paths ${i} deploy_path)
                    get_filename_component(dest_dir "${deploy_path}" DIRECTORY)
                    install(FILES "${src_file}" DESTINATION "${__qml_plugin_dir}/${dest_dir}")
                endforeach()
            endif()
            if(module_resources)
                list(LENGTH module_resources num_files)
                math(EXPR last_index "${num_files} - 1")
                foreach(i RANGE 0 ${last_index})
                    list(GET module_resources ${i} src_file)
                    list(GET module_resources_deploy_paths ${i} deploy_path)
                    get_filename_component(dest_dir "${deploy_path}" DIRECTORY)
                    install(FILES "${src_file}" DESTINATION "${__qml_plugin_dir}/${dest_dir}")
                endforeach()
            endif()
        endif()
    endif()
endif()

target_compile_definitions(${SUB_MODULE_TARGET} PRIVATE
    FRAMELESSHELPER_QUICK_LIBRARY
)

if(FRAMELESSHELPER_NO_PRIVATE)
    target_link_libraries(${SUB_MODULE_TARGET} PRIVATE
        Qt${QT_VERSION_MAJOR}::Quick
    )
else()
    target_link_libraries(${SUB_MODULE_TARGET} PRIVATE
        Qt${QT_VERSION_MAJOR}::QuickPrivate
        Qt${QT_VERSION_MAJOR}::QuickTemplates2Private
        Qt${QT_VERSION_MAJOR}::QuickControls2Private
    )
endif()

target_link_libraries(${SUB_MODULE_TARGET} PRIVATE
    ${PROJECT_NAME}::Core
)

include(GNUInstallDirs)
target_include_directories(${SUB_MODULE_TARGET} PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${INCLUDE_PREFIX}/../..>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${INCLUDE_PREFIX}>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${INCLUDE_PREFIX}/private>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${SUB_MODULE_PATH}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${SUB_MODULE_PATH}/private>"
)

if(NOT FRAMELESSHELPER_BUILD_STATIC)
    setup_target_rpaths(TARGETS ${SUB_MODULE_TARGET})
endif()

setup_qt_stuff(TARGETS ${SUB_MODULE_TARGET} ALLOW_KEYWORD)
setup_custom_moc_macros(TARGETS ${SUB_MODULE_TARGET})

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
