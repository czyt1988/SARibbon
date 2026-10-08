cmake_minimum_required(VERSION 3.21)
project(QmlMainWindowExample VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt${QT_VERSION_MAJOR} ${SARIBBON_MIN_QT_VERSION} REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2 Svg)

add_executable(QmlMainWindowExample main.cpp ribbonbackend.h ribbonbackend.cpp qml.qrc)
if(WIN32)
    set_target_properties(QmlMainWindowExample PROPERTIES WIN32_EXECUTABLE TRUE)
endif()
# exe must land in ${CMAKE_BINARY_DIR}/bin next to the SARibbonQml dlls,
# otherwise the Windows loader cannot find them at startup (same as widgets examples)
set_target_properties(QmlMainWindowExample PROPERTIES
    DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

if(NOT TARGET SARibbonQml)
    find_package(SARibbon 3.0 REQUIRED COMPONENTS Qml)
endif()
target_link_libraries(QmlMainWindowExample PRIVATE SARibbon::Qml Qt${QT_VERSION_MAJOR}::QuickControls2 Qt${QT_VERSION_MAJOR}::Svg)
# Qt5 lane: the backend constructs QAction objects, which live in QtWidgets
# there (contract D5 R1 — one extra Qt5Widgets.dll on the deployment side)
if(QT_VERSION_MAJOR EQUAL 5)
    target_link_libraries(QmlMainWindowExample PRIVATE Qt5::Widgets)
endif()
