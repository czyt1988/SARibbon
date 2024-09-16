cmake_minimum_required(VERSION 3.5)
SET(VERSION_SHORT 0.1)
project(NormalMenuBarExample VERSION ${VERSION_SHORT})
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# qt库加载，最低要求5.9
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} 5.9 COMPONENTS Core Gui Widgets REQUIRED)

# linux下会报
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_LIST_DIR})
add_executable(NormalMenuBarExample WIN32
    MainWindow.h
    MainWindow.cpp
    MainWindow.ui
    main.cpp
)

if(NOT TARGET SARibbonBar)
    # 说明这个例子是单独加载
    message(STATUS "NOT TARGET SARibbonBar find_package(SARibbonBar REQUIRED)")
    find_package(SARibbonBar REQUIRED)
endif()

target_link_libraries(NormalMenuBarExample PUBLIC SARibbonBar::SARibbonBar)
target_link_libraries(NormalMenuBarExample PUBLIC
                                       Qt${QT_VERSION_MAJOR}::Core 
                                       Qt${QT_VERSION_MAJOR}::Gui 
                                       Qt${QT_VERSION_MAJOR}::Widgets)
                                           
set_target_properties(NormalMenuBarExample PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
    CXX_EXTENSIONS OFF
    DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
    VERSION ${SARIBBON_VERSION}
    EXPORT_NAME NormalMenuBarExample
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

install(TARGETS NormalMenuBarExample
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION bin
    ARCHIVE DESTINATION lib
)


