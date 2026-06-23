cmake_minimum_required(VERSION 3.22)

project(PySideSARibbon VERSION 2.8.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =====================================================================
# Find Qt6 (development package required — PySide6 pip package alone
# does not include Qt6 C++ headers like qglobal.h)
# =====================================================================
find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

# =====================================================================
# Find Shiboken6 tools (provides shiboken_generator_create_binding)
# Set CMAKE_PREFIX_PATH to include the shiboken6_generator cmake dir:
#   -DShiboken6Tools_DIR=".../shiboken6_generator/lib/cmake/Shiboken6Tools"
# Or set CMAKE_PREFIX_PATH to include:
#   .../shiboken6_generator/lib/cmake
#   .../shiboken6/lib/cmake
#   .../PySide6/lib/cmake
# =====================================================================
find_package(Shiboken6Tools REQUIRED)

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
qt_add_resources(SARIBBON_RESOURCE_SOURCES ${SARIBBON_SOURCE_DIR}/SARibbonResource.qrc)

add_library(saribbon_lib STATIC ${SARIBBON_SOURCES} ${SARIBBON_RESOURCE_SOURCES})

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
# Generate and build the binding extension module using shiboken6
# =====================================================================
set(GENERATED_SOURCE ${CMAKE_CURRENT_BINARY_DIR}/saribbon_binding.cpp)

shiboken_generator_create_binding(
    EXTENSION_TARGET saribbon
    TYPESYSTEM_FILE ${TYPESYSTEM_FILE}
    GENERATED_SOURCES ${GENERATED_SOURCE}
    HEADERS ${SARIBBON_HEADERS}
    LIBRARY_TARGET saribbon_lib
    QT_MODULES Core Gui Widgets
)

target_include_directories(saribbon PRIVATE
    ${SARIBBON_SOURCE_DIR}
    ${SARIBBON_COLORWIDGETS_DIR}
)

# =====================================================================
# Install
# =====================================================================
find_package(Python COMPONENTS Interpreter REQUIRED)

execute_process(
    COMMAND ${Python_EXECUTABLE} -c
    "import site; print(next(p for p in site.getsitepackages() if 'site-packages' in p))"
    OUTPUT_VARIABLE PYTHON_SITE_PACKAGES
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

install(TARGETS saribbon
    LIBRARY DESTINATION "${PYTHON_SITE_PACKAGES}/PySideSARibbon"
    RUNTIME DESTINATION "${PYTHON_SITE_PACKAGES}/PySideSARibbon"
)

install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/PySideSARibbon/__init__.py
    DESTINATION "${PYTHON_SITE_PACKAGES}/PySideSARibbon"
)
