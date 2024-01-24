cmake_minimum_required(VERSION 3.5)
SET(VERSION_SHORT 0.1)
project(WidgetWithRibbon VERSION ${VERSION_SHORT}) 
set(SARIBBON_EXPAMPLE_NAME WidgetWithRibbon)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# 显示定义FRAMELESSHELPER_FEATURE_static_build为-1 否则库引用会失败
add_definitions(-DFRAMELESSHELPER_FEATURE_static_build=-1)
# qt库加载，最低要求5.9
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} 5.9 COMPONENTS Core Gui Widgets REQUIRED)

add_executable(${SARIBBON_EXPAMPLE_NAME} 
    Widget.h
    Widget.cpp
    main.cpp
    icon.qrc
    Widget.ui
)

target_include_directories(${SARIBBON_EXPAMPLE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../SARibbonBar")

target_link_libraries(${SARIBBON_EXPAMPLE_NAME} PRIVATE SARibbonBar)
target_link_libraries(${SARIBBON_EXPAMPLE_NAME} PUBLIC
                                       Qt${QT_VERSION_MAJOR}::Core 
                                       Qt${QT_VERSION_MAJOR}::Gui 
                                       Qt${QT_VERSION_MAJOR}::Widgets)
                                           
set_target_properties(${SARIBBON_EXPAMPLE_NAME} PROPERTIES 
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
    CXX_EXTENSIONS OFF
    DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
    VERSION ${SARIBBON_VERSION}
    EXPORT_NAME ${SARIBBON_EXPAMPLE_NAME}
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

install(TARGETS ${SARIBBON_EXPAMPLE_NAME} RUNTIME DESTINATION bin LIBRARY DESTINATION bin ARCHIVE DESTINATION lib)
