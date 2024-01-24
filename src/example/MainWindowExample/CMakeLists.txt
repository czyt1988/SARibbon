cmake_minimum_required(VERSION 3.5)
SET(VERSION_SHORT 0.1)
project(MainWindowExample VERSION ${VERSION_SHORT}) 
set(SARIBBON_EXPAMPLE_NAME MainWindowExample)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# qt库加载，最低要求5.8
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} 5.8 COMPONENTS Core Gui Widgets REQUIRED)

add_executable(${SARIBBON_EXPAMPLE_NAME} 
    mainwindow.h
    mainwindow.cpp
    main.cpp
    saribbonresouce.qrc
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

# 会有一个FAILED: src/example/MainWindowExample/CMakeFiles/MainWindowExample.dir/MainWindowExample.rc.res 
# 暂时注释
# 
# if(WIN32)
#     add_custom_command(TARGET ${SARIBBON_EXPAMPLE_NAME}
#         POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E
#                 copy_if_different 
#                     "$<TARGET_FILE:SARibbonBar>" 
#                     "$<TARGET_FILE_DIR:${SARIBBON_EXPAMPLE_NAME}>"
#     )
# 	create_win32_resource_version(
# 		TARGET ${SARIBBON_EXPAMPLE_NAME}
# 		FILENAME ${SARIBBON_EXPAMPLE_NAME}
# 		EXT "exe"
# 		DESCRIPTION "Example application for Qt Ribbon Control"
# 	)
# 	visual_studio_qt_helper(${SARIBBON_EXPAMPLE_NAME})
# endif()
