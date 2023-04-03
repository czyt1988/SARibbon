cmake_minimum_required(VERSION 3.5)

# 设置项目名称
set(EXPAMPLE_NAME UseDesginerMainWindowExample)
project(${EXPAMPLE_NAME})

FIND_PACKAGE(Qt6 6.3.1 COMPONENTS Core Gui Widgets REQUIRED)

set(UI_FILES mainwindow.ui)
qt_wrap_ui(UI_HEADERS ${UI_FILES})

add_executable(${EXPAMPLE_NAME}
    ${UI_HEADERS} # 包含了.ui文件生成的头文件
    MainWindow.h
    MainWindow.cpp
    main.cpp
)

target_link_libraries(${EXPAMPLE_NAME} SARibbonBar Qt::Widgets Qt::Gui Qt::Core)

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
