# SARibbonQml module (plan-04): imperative registration, Qt5/Qt6 same code
find_package(Qt${QT_VERSION_MAJOR} ${SARIBBON_MIN_QT_VERSION} REQUIRED
    COMPONENTS Core Gui Qml Quick)

sa_add_library(SARibbonQml
    SOURCES
        SARibbonQmlGlobal.h
        SARibbonQmlTypes.h
        SARibbonQmlTypes.cpp
        theme/RibbonTheme.h
        theme/RibbonTheme.cpp
        metrics/RibbonMetrics.h
        metrics/RibbonMetrics.cpp
        panel/RibbonPanel.h
        panel/RibbonPanel.cpp
        button/RibbonToolButton.h
        button/RibbonToolButton.cpp
        category/RibbonCategory.h
        category/RibbonCategory.cpp
        tab/RibbonTab.h
        tab/RibbonTab.cpp
        bar/RibbonBar.h
        bar/RibbonBar.cpp
        qml/saribbon_qml.qrc
    PREFIX SA_RIBBON_QML
    QT_LINKS Core Gui Quick Qml
    LINKS SARibbonCore
)
sa_sync_include(SARibbonQml SARibbonQml)
