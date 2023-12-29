cmake_minimum_required(VERSION 3.5)
SET(VERSION_SHORT 0.1)
project(MainWindowExample VERSION ${VERSION_SHORT}) 
set(SARIBBON_EXPAMPLE_NAME MainWindowExample)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# qt库加载，最低要求5.8
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} 5.8 COMPONENTS Core Gui Widgets REQUIRED)

# 在加载qt版本后，需要下面这段开判断是否使用framelss
# 根据qt版本选择是否使用frameless库，目前frameless库支持qt5.14,qt5.15,qt6.4+,除了上诉版本，都使用不了
function(setup_custom_moc_macros)
    cmake_parse_arguments(arg "" "" "TARGETS" ${ARGN})
    if(arg_UNPARSED_ARGUMENTS)
        message(AUTHOR_WARNING "setup_custom_moc_macros: Unrecognized arguments: ${arg_UNPARSED_ARGUMENTS}")
    endif()
    foreach(__target ${arg_TARGETS})
        if(NOT TARGET ${__target})
            message(AUTHOR_WARNING "${__target} is not a valid CMake target!")
            continue()
        endif()
        set_property(TARGET ${__target} APPEND PROPERTY AUTOMOC_MACRO_NAMES "FRAMELESSHELPER_QT_CLASS;FRAMELESSHELPER_PUBLIC_QT_CLASS;FRAMELESSHELPER_PRIVATE_QT_CLASS")
    endforeach()
endfunction()
if(${QT_VERSION_MAJOR} EQUAL 5)
    # qt版本为5，判断是否小版本大于14
    if(${QT_VERSION_MINOR} GREATER_EQUAL 14)
        #5.14和5.15可以使用frameless
        #set(SARIBBON_USE_FRAMELESS_LIB ON)
    else()
        set(SARIBBON_USE_FRAMELESS_LIB OFF)
    endif()
elseif(${QT_VERSION_MAJOR} EQUAL 6)
    # qt版本为6，判断是否小版本大于4
    if(${QT_VERSION_MINOR} GREATER_EQUAL 4)
        #6.4及以上可以使用frameless
        #set(SARIBBON_USE_FRAMELESS_LIB ON)
    else()
        set(SARIBBON_USE_FRAMELESS_LIB OFF)
    endif()
endif()
if(SARIBBON_USE_FRAMELESS_LIB)
    #使用frameless必须设置SARIBBON_USE_3RDPARTY_FRAMELESSHELPER宏为1
    add_definitions(-DSARIBBON_USE_3RDPARTY_FRAMELESSHELPER=1)
    # 显示定义FRAMELESSHELPER_FEATURE_static_build为-1 否则库引用会失败
    add_definitions(-DFRAMELESSHELPER_FEATURE_static_build=-1)

    setup_custom_moc_macros(TARGETS ${SARIBBON_EXPAMPLE_NAME})
else()
    #不使用frameless必须设置SARIBBON_USE_3RDPARTY_FRAMELESSHELPER宏为0
    add_definitions(-DSARIBBON_USE_3RDPARTY_FRAMELESSHELPER=0)
endif()




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
