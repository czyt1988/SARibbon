# SARibbonQml module (plan-04): imperative registration, Qt5/Qt6 same code
find_package(Qt${QT_VERSION_MAJOR} ${SARIBBON_MIN_QT_VERSION} REQUIRED
    COMPONENTS Core Gui Qml Quick QuickControls2)

sa_add_library(SARibbonQml
    SOURCES
        SARibbonQmlGlobal.h
        SARibbonQmlTypes.h
        SARibbonQmlTypes.cpp
        SARibbonQmlQuickHost.h
        SARibbonQmlQuickHost.cpp
        SARibbonQmlLayoutItemHost.h
        SARibbonQmlLayoutItemHost.cpp
        SARibbonQmlButtonRowHost.h
        SARibbonQmlButtonRowHost.cpp
        SARibbonQmlTheme.h
        SARibbonQmlTheme.cpp
        SARibbonQmlMetrics.h
        SARibbonQmlMetrics.cpp
        SARibbonQmlMenuItem.h
        SARibbonQmlMenuItem.cpp
        SARibbonQmlContextCategory.h
        SARibbonQmlContextCategory.cpp
        SARibbonQmlPanel.h
        SARibbonQmlPanel.cpp
        SARibbonQmlToolButton.h
        SARibbonQmlToolButton.cpp
        SARibbonQmlControlContainer.h
        SARibbonQmlControlContainer.cpp
        SARibbonQmlGallery.h
        SARibbonQmlGallery.cpp
        SARibbonQmlGalleryGroup.h
        SARibbonQmlGalleryGroup.cpp
        SARibbonQmlGalleryItem.h
        SARibbonQmlGalleryItem.cpp
        SARibbonQmlColorGrid.h
        SARibbonQmlColorGrid.cpp
        SARibbonQmlColorMenu.h
        SARibbonQmlColorMenu.cpp
        SARibbonQmlColorToolButton.h
        SARibbonQmlColorToolButton.cpp
        SARibbonQmlSeparator.h
        SARibbonQmlSeparator.cpp
        SARibbonQmlQuickAccessBar.h
        SARibbonQmlQuickAccessBar.cpp
        SARibbonQmlButtonGroup.h
        SARibbonQmlButtonGroup.cpp
        SARibbonQmlApplicationWindow.h
        SARibbonQmlApplicationWindow.cpp
        SARibbonQmlCategory.h
        SARibbonQmlCategory.cpp
        SARibbonQmlTab.h
        SARibbonQmlTab.cpp
        SARibbonQmlBar.h
        SARibbonQmlBar.cpp
        SARibbonQmlActionRegistry.h
        SARibbonQmlActionRegistry.cpp
        SARibbonQmlActionRegistryModel.h
        SARibbonQmlActionRegistryModel.cpp
        SARibbonQmlCustomizer.h
        SARibbonQmlCustomizer.cpp
        SARibbonQmlCustomizeTreeModel.h
        SARibbonQmlCustomizeTreeModel.cpp
        qml/saribbon_qml.qrc
    PREFIX SA_RIBBON_QML
    QT_LINKS Core Gui Quick Qml QuickControls2
    LINKS SARibbonCore
)
sa_sync_include(SARibbonQml SARibbonQml)
