cmake_minimum_required(VERSION 3.15)

# Find Qt Test
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test Core Gui Widgets REQUIRED)

# tests/widgets requires the widgets target; guarded so the core-only
# combination (SARIBBON_BUILD_WIDGETS=OFF + SARIBBON_BUILD_TESTS=ON) can
# still configure and run the core golden tests (plan-02 S5.0-1).
if(TARGET SARibbonWidgets)
    add_subdirectory(widgets)
endif()
add_subdirectory(core)
