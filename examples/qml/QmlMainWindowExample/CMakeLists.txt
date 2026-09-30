cmake_minimum_required(VERSION 3.21)
project(QmlMainWindowExample VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt${QT_VERSION_MAJOR} ${SARIBBON_MIN_QT_VERSION} REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2)

add_executable(QmlMainWindowExample main.cpp qml.qrc)
if(WIN32)
    set_target_properties(QmlMainWindowExample PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

if(NOT TARGET SARibbonQml)
    find_package(SARibbon 3.0 REQUIRED COMPONENTS Qml)
endif()
target_link_libraries(QmlMainWindowExample PRIVATE SARibbon::Qml Qt${QT_VERSION_MAJOR}::QuickControls2)
