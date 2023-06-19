cmake_minimum_required(VERSION 3.5)

# 设置项目名称
set(EXPAMPLE_NAME UseDesginerMainWindowExample)
project(${EXPAMPLE_NAME})

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} 5.8 COMPONENTS Core Gui Widgets REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(UI_FILES mainwindow.ui)
#qt_wrap_ui(UI_HEADERS ${UI_FILES})

add_executable(${EXPAMPLE_NAME}
    ${UI_FILES} # 包含了.ui文件
    MainWindow.h
    MainWindow.cpp
    main.cpp
)


target_link_libraries(${EXPAMPLE_NAME} PUBLIC
                      Qt${QT_VERSION_MAJOR}::Core
                      Qt${QT_VERSION_MAJOR}::Gui
                      Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(${EXPAMPLE_NAME} SARibbonBar)

set_target_properties(${EXPAMPLE_NAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

install(TARGETS ${EXPAMPLE_NAME} RUNTIME DESTINATION bin LIBRARY DESTINATION bin ARCHIVE DESTINATION lib)

if(WIN32)
    add_custom_command(TARGET ${EXPAMPLE_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
                copy_if_different 
                    "$<TARGET_FILE:SARibbonBar>" 
                    "$<TARGET_FILE_DIR:${EXPAMPLE_NAME}>"
    )
	create_win32_resource_version(
		TARGET ${EXPAMPLE_NAME}
		FILENAME ${EXPAMPLE_NAME}
		EXT "exe"
		DESCRIPTION "Example application for Qt Ribbon Control"
	)
	visual_studio_qt_helper(${EXPAMPLE_NAME})
endif()
