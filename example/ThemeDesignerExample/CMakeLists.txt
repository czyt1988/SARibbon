cmake_minimum_required(VERSION 3.5)
SET(VERSION_SHORT 0.1)
project(ThemeDesignerExample VERSION ${VERSION_SHORT})
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} 5.8 COMPONENTS Core Gui Widgets REQUIRED)
add_executable(ThemeDesignerExample
    mainwindow.h
    mainwindow.cpp
    helpdialog.h
    helpdialog.cpp
    main.cpp
)
if(WIN32)
    set_target_properties(ThemeDesignerExample PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
if(NOT TARGET SARibbonBar)
    message(STATUS "NOT TARGET SARibbonBar find_package(SARibbonBar REQUIRED)")
    find_package(SARibbonBar REQUIRED)
endif()
target_link_libraries(ThemeDesignerExample PUBLIC SARibbonBar::SARibbonBar)
target_link_libraries(ThemeDesignerExample PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(ThemeDesignerExample PROPERTIES
    AUTOMOC ON
    CXX_EXTENSIONS OFF
    DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
    VERSION ${SARIBBON_VERSION}
    EXPORT_NAME ThemeDesignerExample
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

include(GNUInstallDirs)
install(TARGETS ThemeDesignerExample
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ThemeDesignerExample)
endif()
