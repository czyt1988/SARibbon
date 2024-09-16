cmake_minimum_required(VERSION 3.5)
SET(VERSION_SHORT 0.1)
project(WidgetWithRibbon VERSION ${VERSION_SHORT})
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# 显示定义FRAMELESSHELPER_FEATURE_static_build为-1 否则库引用会失败
add_definitions(-DFRAMELESSHELPER_FEATURE_static_build=-1)
# qt库加载，最低要求5.9
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} 5.9 COMPONENTS Core Gui Widgets REQUIRED)

add_executable(WidgetWithRibbon WIN32
    RibbonWidget.h
    RibbonWidget.cpp
    MainWidget.h
    MainWidget.cpp
    MainWidget.ui
    InnerWidget.h
    InnerWidget.cpp
    InnerWidget.ui
    main.cpp
    icon.qrc
)


if(NOT TARGET SARibbonBar)
    # 说明这个例子是单独加载
    message(STATUS "NOT TARGET SARibbonBar find_package(SARibbonBar REQUIRED)")
    find_package(SARibbonBar REQUIRED)
endif()

target_link_libraries(WidgetWithRibbon PUBLIC SARibbonBar::SARibbonBar)
target_link_libraries(WidgetWithRibbon PUBLIC
                                       Qt${QT_VERSION_MAJOR}::Core 
                                       Qt${QT_VERSION_MAJOR}::Gui 
                                       Qt${QT_VERSION_MAJOR}::Widgets
)

include(GNUInstallDirs)

set_target_properties(WidgetWithRibbon PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
    CXX_EXTENSIONS OFF
    DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
    VERSION ${SARIBBON_VERSION}
    EXPORT_NAME WidgetWithRibbon
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}"
)

install(TARGETS WidgetWithRibbon
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
