cmake_minimum_required(VERSION 3.5)
project(MainWindowExample VERSION ${VERSION_SHORT}) 
set(SARIBBON_EXPAMPLE_NAME MainWindowExample)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# qt库加载，最低
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} 5.7 COMPONENTS Core Gui Widgets REQUIRED)
add_executable(${SARIBBON_EXPAMPLE_NAME} 
    mainwindow.h
    mainwindow.cpp
    main.cpp
    saribbonresouce.qrc
)

target_include_directories(${SARIBBON_EXPAMPLE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../SARibbonBar")
message(STATUS "SARIBBON_BIN_DIR=${SARIBBON_BIN_DIR}")
find_library(SARIBBONBAR_LIB NAMES SARibbonBar HINTS ${SARIBBON_BIN_DIR})
target_link_libraries(${SARIBBON_EXPAMPLE_NAME} PRIVATE ${SARIBBONBAR_LIB})
target_link_libraries(${SARIBBON_EXPAMPLE_NAME} PUBLIC
                                       Qt${QT_VERSION_MAJOR}::Core 
                                       Qt${QT_VERSION_MAJOR}::Gui 
                                       Qt${QT_VERSION_MAJOR}::Widgets)
                                           
set_target_properties(${SARIBBON_EXPAMPLE_NAME} PROPERTIES 
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    VERSION ${VERSION_SHORT}
    EXPORT_NAME ${SARIBBON_EXPAMPLE_NAME}
    ARCHIVE_OUTPUT_DIRECTORY "${SARIBBON_BIN_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${SARIBBON_BIN_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${SARIBBON_BIN_DIR}"
)

install(TARGETS ${SARIBBON_EXPAMPLE_NAME} RUNTIME DESTINATION bin LIBRARY DESTINATION bin ARCHIVE DESTINATION lib)

if(WIN32)
    add_custom_command(TARGET ${SARIBBON_EXPAMPLE_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
                copy_if_different 
                    "$<TARGET_FILE:SARibbonBar>" 
                    "$<TARGET_FILE_DIR:${SARIBBON_EXPAMPLE_NAME}>"
    )
	create_win32_resource_version(
		TARGET ${SARIBBON_EXPAMPLE_NAME}
		FILENAME ${SARIBBON_EXPAMPLE_NAME}
		EXT "exe"
		DESCRIPTION "Example application for Qt Ribbon Control"
	)
	visual_studio_qt_helper(${SARIBBON_EXPAMPLE_NAME})
endif()
