# SARibbonQml module (plan-04): imperative registration, Qt5/Qt6 same code
find_package(Qt${QT_VERSION_MAJOR} ${SARIBBON_MIN_QT_VERSION} REQUIRED
    COMPONENTS Core Gui Qml Quick QuickControls2)

sa_add_library(SARibbonQml
    SOURCES
        SARibbonQmlGlobal.h
        SARibbonQmlTypes.h
        SARibbonQmlTypes.cpp
        host/RibbonQuickHost.h
        host/RibbonQuickHost.cpp
        host/RibbonLayoutItemHost.h
        host/RibbonLayoutItemHost.cpp
        host/RibbonButtonRowHost.h
        host/RibbonButtonRowHost.cpp
        theme/RibbonTheme.h
        theme/RibbonTheme.cpp
        metrics/RibbonMetrics.h
        metrics/RibbonMetrics.cpp
        menu/RibbonMenuItem.h
        menu/RibbonMenuItem.cpp
        context/RibbonContextCategory.h
        context/RibbonContextCategory.cpp
        panel/RibbonPanel.h
        panel/RibbonPanel.cpp
        button/RibbonToolButton.h
        button/RibbonToolButton.cpp
        container/RibbonControlContainer.h
        container/RibbonControlContainer.cpp
        gallery/RibbonGallery.h
        gallery/RibbonGallery.cpp
        gallery/RibbonGalleryGroup.h
        gallery/RibbonGalleryGroup.cpp
        gallery/RibbonGalleryItem.h
        gallery/RibbonGalleryItem.cpp
        separator/RibbonSeparator.h
        separator/RibbonSeparator.cpp
        quickaccess/RibbonQuickAccessBar.h
        quickaccess/RibbonQuickAccessBar.cpp
        group/RibbonButtonGroup.h
        group/RibbonButtonGroup.cpp
        category/RibbonCategory.h
        category/RibbonCategory.cpp
        tab/RibbonTab.h
        tab/RibbonTab.cpp
        bar/RibbonBar.h
        bar/RibbonBar.cpp
        qml/saribbon_qml.qrc
    PREFIX SA_RIBBON_QML
    QT_LINKS Core Gui Quick Qml QuickControls2
    LINKS SARibbonCore
)
sa_sync_include(SARibbonQml SARibbonQml)
