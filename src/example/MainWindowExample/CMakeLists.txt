set(EXPAMPLE_NAME MainWindowExample)

qt_add_resources(QRCS saribbonresouce.qrc)
qt_wrap_cpp(Moced "mainwindow.h")

add_executable(${EXPAMPLE_NAME} 
    ${QRCS}
    mainwindow.h
    mainwindow.cpp
    main.cpp
	${Moced}
)

target_link_libraries(${EXPAMPLE_NAME} SARibbonBar Qt::Widgets)

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
