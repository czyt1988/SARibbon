cmake_minimum_required(VERSION 3.15)
SET(VERSION_SHORT 0.1)
project(Qt3DWindowExample VERSION ${VERSION_SHORT})
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
# qt库加载，最低要求5.8
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} 5.12 COMPONENTS Core Gui Widgets REQUIRED)
# Qt3D 依赖（Qt5: 3DCore/3DRender/3DInput/3DLogic/3DExtras/3DAnimation；Qt6: 模块更名）
if(QT_VERSION_MAJOR EQUAL 5)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS 3DCore 3DRender 3DInput 3DLogic 3DExtras 3DAnimation REQUIRED)
else()
    find_package(Qt6 COMPONENTS 3DCore 3DRender 3DInput 3DLogic 3DExtras 3DAnimation REQUIRED)
endif()
add_executable(Qt3DWindowExample
    mainwindow.h
    mainwindow.cpp
    main.cpp
)
if(WIN32)
    set_target_properties(Qt3DWindowExample PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
if(NOT TARGET SARibbonBar)
    # 说明这个例子是单独加载
    message(STATUS "NOT TARGET SARibbonBar find_package(SARibbonBar REQUIRED)")
    find_package(SARibbonBar REQUIRED)
endif()
target_link_libraries(Qt3DWindowExample PRIVATE SARibbonBar::SARibbonBar)
target_link_libraries(Qt3DWindowExample PRIVATE
                                       Qt${QT_VERSION_MAJOR}::Core
                                       Qt${QT_VERSION_MAJOR}::Gui
                                       Qt${QT_VERSION_MAJOR}::Widgets)
if(QT_VERSION_MAJOR EQUAL 5)
    target_link_libraries(Qt3DWindowExample PRIVATE
                           Qt5::3DCore
                           Qt5::3DRender
                           Qt5::3DInput
                           Qt5::3DLogic
                           Qt5::3DExtras
                           Qt5::3DAnimation)
else()
    target_link_libraries(Qt3DWindowExample PRIVATE
                           Qt6::3DCore
                           Qt6::3DRender
                           Qt6::3DInput
                           Qt6::3DLogic
                           Qt6::3DExtras
                           Qt6::3DAnimation)
endif()

set_target_properties(Qt3DWindowExample PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
    CXX_EXTENSIONS OFF
    DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
    VERSION ${SARIBBON_VERSION}
    EXPORT_NAME Qt3DWindowExample
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

include(GNUInstallDirs)
install(TARGETS Qt3DWindowExample
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Qt3DWindowExample)
endif()
