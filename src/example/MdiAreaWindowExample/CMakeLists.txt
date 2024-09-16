cmake_minimum_required(VERSION 3.5)
SET(VERSION_SHORT 0.1)
project(MdiAreaWindowExample VERSION ${VERSION_SHORT})
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# qt库加载，最低要求5.8
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} 5.8 COMPONENTS Core Gui Widgets REQUIRED)
file(GLOB HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/*.h")
file(GLOB SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
file(GLOB UI_FILES "${CMAKE_CURRENT_LIST_DIR}/*.ui")
file(GLOB RESOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.qrc")
add_executable(MdiAreaWindowExample WIN32
    ${HEADER_FILES}
    ${SOURCE_FILES}
    ${UI_FILES}
    ${RESOURCE_FILES}
)

if(NOT TARGET SARibbonBar)
    # 说明这个例子是单独加载
    message(STATUS "NOT TARGET SARibbonBar find_package(SARibbonBar REQUIRED)")
    find_package(SARibbonBar REQUIRED)
endif()

target_link_libraries(MdiAreaWindowExample PUBLIC SARibbonBar::SARibbonBar)
target_link_libraries(MdiAreaWindowExample PUBLIC
                                       Qt${QT_VERSION_MAJOR}::Core 
                                       Qt${QT_VERSION_MAJOR}::Gui 
                                       Qt${QT_VERSION_MAJOR}::Widgets)
                                           
set_target_properties(MdiAreaWindowExample PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
    CXX_EXTENSIONS OFF
    DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
    VERSION ${SARIBBON_VERSION}
    EXPORT_NAME MdiAreaWindowExample
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

install(TARGETS MdiAreaWindowExample
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION bin
    ARCHIVE DESTINATION lib
)

