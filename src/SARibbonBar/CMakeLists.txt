cmake_minimum_required(VERSION 3.5)
project(SARibbonBar LANGUAGES CXX VERSION ${SARIBBON_VERSION})
set(LIB_NAME SARibbonBar)

set(CMAKE_INCLUDE_CURRENT_DIR ON)


# header files
# cn:头文件
SET(SARIBBON_HEADER_FILES
    SAFramelessHelper.h
    SARibbonStyleOption.h
    SARibbonActionsManager.h
    SARibbonBar.h
    SARibbonCustomizeData.h
    SARibbonCustomizeDialog.h
    SARibbonCustomizeWidget.h
    SARibbonMainWindow.h
    SAWindowButtonGroup.h
    SARibbonApplicationButton.h
    SARibbonTabBar.h
    SARibbonCategory.h
    SARibbonContextCategory.h
    SARibbonPannel.h
    SARibbonToolButton.h
    SARibbonMenu.h
    SARibbonGlobal.h
    SARibbonPannelOptionButton.h
    SARibbonSeparatorWidget.h
    SARibbonCategoryLayout.h
    SARibbonGallery.h
    SARibbonControlButton.h
    SARibbonGalleryGroup.h
    SARibbonGalleryItem.h
    SARibbonComboBox.h
    SARibbonElementCreateDelegate.h
    SARibbonElementManager.h
    SARibbonDrawHelper.h
    SARibbonLineEdit.h
    SARibbonCheckBox.h
    SARibbonButtonGroupWidget.h
    SARibbonStackedWidget.h
    SARibbonQuickAccessBar.h
    SARibbonCtrlContainer.h
    SARibbonPannelLayout.h
    SARibbonPannelItem.h
    SARibbonLineWidgetContainer.h
)

# source files
# cn:cpp文件
SET(SARIBBON_SOURCE_FILES
    SAFramelessHelper.cpp
    SARibbonStyleOption.cpp
    SARibbonActionsManager.cpp
    SARibbonBar.cpp
    SARibbonCustomizeData.cpp
    SARibbonCustomizeDialog.cpp
    SARibbonCustomizeWidget.cpp
    SARibbonMainWindow.cpp
    SAWindowButtonGroup.cpp
    SARibbonApplicationButton.cpp
    SARibbonTabBar.cpp
    SARibbonCategory.cpp
    SARibbonContextCategory.cpp
    SARibbonPannel.cpp
    SARibbonToolButton.cpp
    SARibbonMenu.cpp
    SARibbonPannelOptionButton.cpp
    SARibbonSeparatorWidget.cpp
    SARibbonCategoryLayout.cpp
    SARibbonGallery.cpp
    SARibbonControlButton.cpp
    SARibbonGalleryGroup.cpp
    SARibbonGalleryItem.cpp
    SARibbonComboBox.cpp
    SARibbonElementCreateDelegate.cpp
    SARibbonElementManager.cpp
    SARibbonDrawHelper.cpp
    SARibbonLineEdit.cpp
    SARibbonCheckBox.cpp
    SARibbonButtonGroupWidget.cpp
    SARibbonStackedWidget.cpp
    SARibbonQuickAccessBar.cpp
    SARibbonCtrlContainer.cpp
    SARibbonPannelLayout.cpp
    SARibbonPannelItem.cpp
    SARibbonLineWidgetContainer.cpp
)

# resource files
# cn:资源文件
SET(SARIBBON_RESOURCE_FILES
    resource.qrc
)

add_library(${LIB_NAME} SHARED
    ${SARIBBON_HEADER_FILES}
    ${SARIBBON_SOURCE_FILES}
    ${SARIBBON_RESOURCE_FILES}
)


target_link_libraries(${LIB_NAME} PUBLIC
                      Qt${QT_VERSION_MAJOR}::Core
                      Qt${QT_VERSION_MAJOR}::Gui
                      Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(${LIB_NAME} PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
    CXX_EXTENSIONS OFF
    DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
    VERSION ${SARIBBON_VERSION}
    EXPORT_NAME ${LIB_NAME}
    ARCHIVE_OUTPUT_DIRECTORY "${SARIBBON_BIN_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${SARIBBON_BIN_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${SARIBBON_BIN_DIR}"
)

target_compile_definitions(${LIB_NAME} PRIVATE SA_RIBBON_BAR_MAKE_LIB)




target_include_directories(${LIB_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
install(TARGETS ${LIB_NAME}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION bin
	ARCHIVE DESTINATION lib
)
install(FILES ${SARIBBON_HEADER_FILES} DESTINATION include)

if(WIN32)
	create_win32_resource_version(
		TARGET ${LIB_NAME}
		FILENAME ${LIB_NAME}
		EXT "dll"
		DESCRIPTION "Ribbon control library for Qt"
	)
endif()
