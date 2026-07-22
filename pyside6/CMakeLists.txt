cmake_minimum_required(VERSION 3.22)

project(PySideSARibbon VERSION 2.8.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt MOC/RCC/UIC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# =====================================================================
# Find Qt6 (development package required — PySide6 pip package alone
# does not include Qt6 C++ headers like qglobal.h)
# =====================================================================
find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

# =====================================================================
# Find Python and Shiboken6 / PySide6
#
# PySide6 6.8.x does not ship Shiboken6ToolsConfig.cmake, so we locate
# shiboken6, shiboken6 include dirs, and PySide6 include dirs manually
# via Python's site-packages.
# =====================================================================
find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)

# --- Locate site-packages ---
execute_process(
    COMMAND ${Python_EXECUTABLE} -c
    "import site; print(next(p for p in site.getsitepackages() if 'site-packages' in p))"
    OUTPUT_VARIABLE PYTHON_SITE_PACKAGES
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# --- Shiboken6 include and lib ---
# Shiboken headers ship in the shiboken6 runtime package, not in shiboken6_generator
set(SHIBOKEN6_INCLUDE_DIR "${PYTHON_SITE_PACKAGES}/shiboken6/include")
set(SHIBOKEN6_LIB_DIR "${PYTHON_SITE_PACKAGES}/shiboken6")

# Find shiboken6 generator executable
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(SHIBOKEN_LIB_NAME "shiboken6d")
else()
    set(SHIBOKEN_LIB_NAME "shiboken6")
endif()

# --- Find shiboken6.exe ---
find_program(SHIBOKEN_EXECUTABLE shiboken6
    HINTS "${PYTHON_SITE_PACKAGES}/shiboken6_generator"
    REQUIRED)

# --- PySide6 include and lib ---
set(PYSIDE_INCLUDE_DIR "${PYTHON_SITE_PACKAGES}/PySide6/include")
set(PYSIDE_LIB_DIR "${PYTHON_SITE_PACKAGES}/PySide6")
set(PYSIDE_TYPESYSTEMS "${PYTHON_SITE_PACKAGES}/PySide6/typesystems")

# --- Locate shiboken6.lib ---
# The wheels ship only versioned libraries (libshiboken6.abi3.so.6.11) with no
# unversioned symlink, so find_library() cannot see them.
file(GLOB SHIBOKEN6_LIBRARY_CANDIDATES
    "${SHIBOKEN6_LIB_DIR}/libshiboken6.abi3.so*"
    "${SHIBOKEN6_LIB_DIR}/libshiboken6*.dylib"
    "${SHIBOKEN6_LIB_DIR}/shiboken6.abi3.lib"
    "${SHIBOKEN6_LIB_DIR}/shiboken6.lib")
list(GET SHIBOKEN6_LIBRARY_CANDIDATES 0 SHIBOKEN6_LIBRARY)
if(NOT SHIBOKEN6_LIBRARY)
    message(FATAL_ERROR "shiboken6 library not found in ${SHIBOKEN6_LIB_DIR}")
endif()
message(STATUS "SHIBOKEN6_LIBRARY: ${SHIBOKEN6_LIBRARY}")

# --- Locate pyside6.lib ---
file(GLOB PYSIDE6_LIBRARY_CANDIDATES
    "${PYSIDE_LIB_DIR}/libpyside6.abi3.so*"
    "${PYSIDE_LIB_DIR}/libpyside6*.dylib"
    "${PYSIDE_LIB_DIR}/pyside6.abi3.lib"
    "${PYSIDE_LIB_DIR}/pyside6.lib")
list(GET PYSIDE6_LIBRARY_CANDIDATES 0 PYSIDE6_LIBRARY)
if(NOT PYSIDE6_LIBRARY)
    message(FATAL_ERROR "pyside6 library not found in ${PYSIDE_LIB_DIR}")
endif()
message(STATUS "PYSIDE6_LIBRARY: ${PYSIDE6_LIBRARY}")

# =====================================================================
# Paths
# =====================================================================
set(SARIBBON_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src/SARibbonBar")
set(SARIBBON_COLORWIDGETS_DIR "${SARIBBON_SOURCE_DIR}/colorWidgets")

# =====================================================================
# Header files for shiboken
# =====================================================================
set(SARIBBON_HEADERS
    ${SARIBBON_SOURCE_DIR}/SARibbonGlobal.h
    ${SARIBBON_SOURCE_DIR}/SARibbonBarVersionInfo.h
    ${SARIBBON_SOURCE_DIR}/SARibbonQt5Compat.hpp
    ${SARIBBON_SOURCE_DIR}/SARibbonMainWindow.h
    ${SARIBBON_SOURCE_DIR}/SARibbonBar.h
    ${SARIBBON_SOURCE_DIR}/SARibbonBarLayout.h
    ${SARIBBON_SOURCE_DIR}/SARibbonCategory.h
    ${SARIBBON_SOURCE_DIR}/SARibbonCategoryLayout.h
    ${SARIBBON_SOURCE_DIR}/SARibbonPanel.h
    ${SARIBBON_SOURCE_DIR}/SARibbonPanelLayout.h
    ${SARIBBON_SOURCE_DIR}/SARibbonPanelItem.h
    ${SARIBBON_SOURCE_DIR}/SARibbonPanelOptionButton.h
    ${SARIBBON_SOURCE_DIR}/SARibbonToolButton.h
    ${SARIBBON_SOURCE_DIR}/SARibbonButtonLayoutStrategy.h
    ${SARIBBON_SOURCE_DIR}/SARibbonContextCategory.h
    ${SARIBBON_SOURCE_DIR}/SARibbonGallery.h
    ${SARIBBON_SOURCE_DIR}/SARibbonGalleryGroup.h
    ${SARIBBON_SOURCE_DIR}/SARibbonGalleryItem.h
    ${SARIBBON_SOURCE_DIR}/SARibbonTabBar.h
    ${SARIBBON_SOURCE_DIR}/SARibbonMenu.h
    ${SARIBBON_SOURCE_DIR}/SARibbonApplicationButton.h
    ${SARIBBON_SOURCE_DIR}/SARibbonApplicationWidget.h
    ${SARIBBON_SOURCE_DIR}/SARibbonButtonGroupWidget.h
    ${SARIBBON_SOURCE_DIR}/SARibbonStackedWidget.h
    ${SARIBBON_SOURCE_DIR}/SARibbonQuickAccessBar.h
    ${SARIBBON_SOURCE_DIR}/SARibbonSystemButtonBar.h
    ${SARIBBON_SOURCE_DIR}/SARibbonCtrlContainer.h
    ${SARIBBON_SOURCE_DIR}/SARibbonLineWidgetContainer.h
    ${SARIBBON_SOURCE_DIR}/SARibbonSeparatorWidget.h
    ${SARIBBON_SOURCE_DIR}/SARibbonTitleIconWidget.h
    ${SARIBBON_SOURCE_DIR}/SARibbonElementFactory.h
    ${SARIBBON_SOURCE_DIR}/SARibbonElementManager.h
    ${SARIBBON_SOURCE_DIR}/SARibbonActionsManager.h
    ${SARIBBON_SOURCE_DIR}/SARibbonCustomizeData.h
    ${SARIBBON_SOURCE_DIR}/SARibbonCustomizeDialog.h
    ${SARIBBON_SOURCE_DIR}/SARibbonCustomizeWidget.h
    ${SARIBBON_SOURCE_DIR}/SARibbonUtil.h
    ${SARIBBON_SOURCE_DIR}/SAFramelessHelper.h
    ${SARIBBON_SOURCE_DIR}/SARibbonWidget.h
    ${SARIBBON_SOURCE_DIR}/SARibbonColorToolButton.h
    ${SARIBBON_COLORWIDGETS_DIR}/SAColorWidgetsGlobal.h
    ${SARIBBON_COLORWIDGETS_DIR}/SAColorToolButton.h
    ${SARIBBON_COLORWIDGETS_DIR}/SAColorGridWidget.h
    ${SARIBBON_COLORWIDGETS_DIR}/SAColorPaletteGridWidget.h
    ${SARIBBON_COLORWIDGETS_DIR}/SAColorMenu.h
)

# =====================================================================
# C++ source files
# =====================================================================
set(SARIBBON_SOURCES
    ${SARIBBON_SOURCE_DIR}/SARibbonMainWindow.cpp
    ${SARIBBON_SOURCE_DIR}/SARibbonBar.cpp
    ${SARIBBON_SOURCE_DIR}/SARibbonBarLayout.cpp
    ${SARIBBON_SOURCE_DIR}/SARibbonCategory.cpp
    ${SARIBBON_SOURCE_DIR}/SARibbonCategoryLayout.cpp
    ${SARIBBON_SOURCE_DIR}/SARibbonPanel.cpp
    ${SARIBBON_SOURCE_DIR}/SARibbonPanelLayout.cpp
    ${SARIBBON_SOURCE_DIR}/SARibbonPanelItem.cpp
    ${SARIBBON_SOURCE_DIR}/SARibbonPanelOptionButton.cpp
    ${SARIBBON_SOURCE_DIR}/SARibbonToolButton.cpp
    ${SARIBBON_SOURCE_DIR}/SARibbonButtonLayoutStrategy.cpp
    ${SARIBBON_SOURCE_DIR}/SARibbonContextCategory.cpp
    ${SARIBBON_SOURCE_DIR}/SARibbonGallery.cpp
    ${SARIBBON_SOURCE_DIR}/SARibbonGalleryGroup.cpp
    ${SARIBBON_SOURCE_DIR}/SARibbonGalleryItem.cpp
    ${SARIBBON_SOURCE_DIR}/SARibbonTabBar.cpp
    ${SARIBBON_SOURCE_DIR}/SARibbonMenu.cpp
    ${SARIBBON_SOURCE_DIR}/SARibbonApplicationButton.cpp
    ${SARIBBON_SOURCE_DIR}/SARibbonApplicationWidget.cpp
    ${SARIBBON_SOURCE_DIR}/SARibbonButtonGroupWidget.cpp
    ${SARIBBON_SOURCE_DIR}/SARibbonStackedWidget.cpp
    ${SARIBBON_SOURCE_DIR}/SARibbonQuickAccessBar.cpp
    ${SARIBBON_SOURCE_DIR}/SARibbonSystemButtonBar.cpp
    ${SARIBBON_SOURCE_DIR}/SARibbonCtrlContainer.cpp
    ${SARIBBON_SOURCE_DIR}/SARibbonLineWidgetContainer.cpp
    ${SARIBBON_SOURCE_DIR}/SARibbonSeparatorWidget.cpp
    ${SARIBBON_SOURCE_DIR}/SARibbonTitleIconWidget.cpp
    ${SARIBBON_SOURCE_DIR}/SARibbonElementFactory.cpp
    ${SARIBBON_SOURCE_DIR}/SARibbonElementManager.cpp
    ${SARIBBON_SOURCE_DIR}/SARibbonActionsManager.cpp
    ${SARIBBON_SOURCE_DIR}/SARibbonCustomizeData.cpp
    ${SARIBBON_SOURCE_DIR}/SARibbonCustomizeDialog.cpp
    ${SARIBBON_SOURCE_DIR}/SARibbonCustomizeWidget.cpp
    ${SARIBBON_SOURCE_DIR}/SARibbonUtil.cpp
    ${SARIBBON_SOURCE_DIR}/SARibbonThemeManager.cpp
    ${SARIBBON_SOURCE_DIR}/SARibbonThemePalette.cpp
    ${SARIBBON_SOURCE_DIR}/SAFramelessHelper.cpp
    ${SARIBBON_SOURCE_DIR}/SARibbonWidget.cpp
    ${SARIBBON_SOURCE_DIR}/SARibbonColorToolButton.cpp
    ${SARIBBON_COLORWIDGETS_DIR}/SAColorToolButton.cpp
    ${SARIBBON_COLORWIDGETS_DIR}/SAColorGridWidget.cpp
    ${SARIBBON_COLORWIDGETS_DIR}/SAColorPaletteGridWidget.cpp
    ${SARIBBON_COLORWIDGETS_DIR}/SAColorMenu.cpp
)

# =====================================================================
# Build SARibbon C++ static library
# =====================================================================
# NOTE: qrc resources are NOT added to saribbon_lib. They are compiled
# directly into the final .pyd module (see below) to ensure the RCC
# static initializer is included. When qrc code lives in a static lib,
# MSVC's /OPT:REF strips the unreferenced initializer, and resources
# (QSS themes, icons) are never registered at runtime.
add_library(saribbon_lib STATIC ${SARIBBON_SOURCES})

target_compile_definitions(saribbon_lib PRIVATE
    SA_RIBBON_BAR_NO_EXPORT
    SARIBBON_USE_3RDPARTY_FRAMELESSHELPER=0
    NOMINMAX
    SA_COLOR_WIDGETS_NO_DLL
    SA_RIBBON_BAR_MAKE_LIB
)

target_include_directories(saribbon_lib PUBLIC
    ${SARIBBON_SOURCE_DIR}
    ${SARIBBON_COLORWIDGETS_DIR}
)

target_link_libraries(saribbon_lib PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# =====================================================================
# Typesystem file
# =====================================================================
set(TYPESYSTEM_FILE ${CMAKE_CURRENT_SOURCE_DIR}/typesystem_saribbon.xml)

# =====================================================================
# Shiboken6 setup — build options
# =====================================================================
set(SHIBOKEN_OPTIONS
    --generator-set=shiboken
    --enable-parent-ctor-heuristic
    --enable-return-value-heuristic
    --use-isnull-as-nb_nonzero
    --avoid-protected-hack
    -I${CMAKE_CURRENT_SOURCE_DIR}
    -T${CMAKE_CURRENT_SOURCE_DIR}
    --output-directory=${CMAKE_CURRENT_BINARY_DIR}
)

# Add Qt include directories
foreach(module Core Gui Widgets)
    get_property(module_includes TARGET Qt6::${module} PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
    foreach(inc ${module_includes})
        list(APPEND SHIBOKEN_OPTIONS "-I${inc}")
    endforeach()
endforeach()

# Add PySide typesystems path
list(APPEND SHIBOKEN_OPTIONS "-T${PYSIDE_TYPESYSTEMS}")
list(APPEND SHIBOKEN_OPTIONS "--enable-pyside-extensions")

# =====================================================================
# Generate binding sources — shiboken6 generates multiple .cpp files
# =====================================================================
set(GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/PySideSARibbon/saribbon")

set(GENERATED_SOURCES
    ${GENERATED_DIR}/saribbon_module_wrapper.cpp
    ${GENERATED_DIR}/saribbonbar_wrapper.cpp
    ${GENERATED_DIR}/saribbonmainwindow_wrapper.cpp
    ${GENERATED_DIR}/saribboncategory_wrapper.cpp
    ${GENERATED_DIR}/saribbonpanel_wrapper.cpp
    ${GENERATED_DIR}/saribbonpanellabel_wrapper.cpp
    ${GENERATED_DIR}/saribbonpanelitem_wrapper.cpp
    ${GENERATED_DIR}/saribbontoolbutton_wrapper.cpp
    ${GENERATED_DIR}/saribbontabbar_wrapper.cpp
    ${GENERATED_DIR}/saribbonbuttongroupwidget_wrapper.cpp
    ${GENERATED_DIR}/saribbonquickaccessbar_wrapper.cpp
    ${GENERATED_DIR}/saribboncontextcategory_wrapper.cpp
    ${GENERATED_DIR}/saribbongallery_wrapper.cpp
    ${GENERATED_DIR}/saribbongallerygroup_wrapper.cpp
    ${GENERATED_DIR}/saribbongalleryitem_wrapper.cpp
    ${GENERATED_DIR}/saribbongallerybutton_wrapper.cpp
    ${GENERATED_DIR}/saribbongalleryviewport_wrapper.cpp
    ${GENERATED_DIR}/saribbonwidget_wrapper.cpp
    ${GENERATED_DIR}/saribbonmenu_wrapper.cpp
    ${GENERATED_DIR}/saribbonapplicationbutton_wrapper.cpp
    ${GENERATED_DIR}/saribboncolortoolbutton_wrapper.cpp
    ${GENERATED_DIR}/saribbonactionsmanager_wrapper.cpp
    ${GENERATED_DIR}/saribbonactionsmanagermodel_wrapper.cpp
    ${GENERATED_DIR}/saribboncustomizedata_wrapper.cpp
    ${GENERATED_DIR}/saribboncustomizewidget_wrapper.cpp
    ${GENERATED_DIR}/saribboncustomizedialog_wrapper.cpp
)

add_custom_command(
    OUTPUT ${GENERATED_SOURCES}
    COMMAND "${SHIBOKEN_EXECUTABLE}"
    ${SHIBOKEN_OPTIONS} ${SARIBBON_HEADERS} "${TYPESYSTEM_FILE}"
    DEPENDS ${SARIBBON_HEADERS} ${TYPESYSTEM_FILE}
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "Generating bindings for saribbon"
)

# =====================================================================
# Build the binding extension module
# =====================================================================
# Compile qrc resources directly into the .pyd so the RCC static
# initializer runs on module load, registering all QSS themes and icons.
qt_add_resources(SARIBBON_RESOURCE_SOURCES ${SARIBBON_SOURCE_DIR}/SARibbonResource.qrc)

add_library(saribbon MODULE ${GENERATED_SOURCES} ${SARIBBON_RESOURCE_SOURCES})

target_compile_definitions(saribbon PRIVATE
    Py_LIMITED_API=0x030a0000
    SA_RIBBON_BAR_NO_EXPORT
    SARIBBON_USE_3RDPARTY_FRAMELESSHELPER=0
    NOMINMAX
    SA_COLOR_WIDGETS_NO_DLL
)

target_include_directories(saribbon PRIVATE
    ${SHIBOKEN6_INCLUDE_DIR}
    ${PYSIDE_INCLUDE_DIR}
    ${PYSIDE_INCLUDE_DIR}/QtCore
    ${PYSIDE_INCLUDE_DIR}/QtGui
    ${PYSIDE_INCLUDE_DIR}/QtWidgets
    ${SARIBBON_SOURCE_DIR}
    ${SARIBBON_COLORWIDGETS_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Python_INCLUDE_DIRS}
)

target_link_libraries(saribbon PRIVATE
    ${SHIBOKEN6_LIBRARY}
    ${PYSIDE6_LIBRARY}
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    saribbon_lib
)

# Python libraries (Windows)
if(WIN32)
    target_link_directories(saribbon PRIVATE "${Python_LIBRARY_DIRS}")
    target_link_libraries(saribbon PRIVATE python3)
endif()

# Target properties
set_target_properties(saribbon PROPERTIES
    PREFIX ""
    OUTPUT_NAME "saribbon"
)

if(WIN32)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set_target_properties(saribbon PROPERTIES SUFFIX "_d.pyd")
    else()
        set_target_properties(saribbon PROPERTIES SUFFIX ".pyd")
    endif()
endif()

# =====================================================================
# Install
# Use SKBUILD_PLATLIB_DIR (provided by scikit-build-core) when building
# wheels, or fall back to PYTHON_SITE_PACKAGES for standalone CMake builds.
# =====================================================================
if(DEFINED SKBUILD_PLATLIB_DIR)
    set(_INSTALL_LIBDIR "${SKBUILD_PLATLIB_DIR}/PySideSARibbon")
else()
    set(_INSTALL_LIBDIR "${PYTHON_SITE_PACKAGES}/PySideSARibbon")
endif()

install(TARGETS saribbon
    LIBRARY DESTINATION "${_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${_INSTALL_LIBDIR}"
)

install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/PySideSARibbon/__init__.py
    DESTINATION "${_INSTALL_LIBDIR}"
)
