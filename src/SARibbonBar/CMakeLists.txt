set(LIB_NAME SARibbonBar)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

qt_add_resources(QRCS resource.qrc)

SET(HEADER_FILES
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

qt_wrap_cpp(MOC_HEADER_FILES ${HEADER_FILES})

SET(SOURCE_FILES
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

add_library(${LIB_NAME}
    ${QRCS}
    ${MOC_HEADER_FILES}
    ${HEADER_FILES}
    ${SOURCE_FILES}
)

if (BUILD_SHARED_LIBS)
    target_compile_definitions(${LIB_NAME} PRIVATE SA_RIBBON_BAR_MAKE_LIB)
else()
    target_compile_definitions(${LIB_NAME} PUBLIC SA_RIBBON_BAR_NO_EXPORT)    
endif()

target_link_libraries(${LIB_NAME} Qt::Widgets)

set_target_properties(${LIB_NAME} PROPERTIES
	AUTOMOC TRUE
	AUTOUIC TRUE
	AUTORCC TRUE
	DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
	VERSION ${PROJECT_VERSION}
	SOVERSION ${PROJECT_VERSION}
)
target_include_directories(${LIB_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
install(TARGETS ${LIB_NAME}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION bin
	ARCHIVE DESTINATION lib
)
install(FILES ${HEADER_FILES} DESTINATION include)

if(WIN32)
	create_win32_resource_version(
		TARGET ${LIB_NAME}
		FILENAME ${LIB_NAME}
		EXT "dll"
		DESCRIPTION "Ribbon control library for Qt"
	)
endif()
