#include "SARibbonQmlGlobal.h"
#include "SARibbonQmlTypes.h"
#include "theme/RibbonTheme.h"
#include "metrics/RibbonMetrics.h"
#include "panel/RibbonPanel.h"
#include "button/RibbonToolButton.h"
#include "category/RibbonCategory.h"
#include "tab/RibbonTab.h"
#include "bar/RibbonBar.h"
#include <QQmlEngine>
#include <QQmlContext>
#include <QtQml>

void saRibbonRegisterQmlTypes(QQmlEngine* engine)
{
    Q_UNUSED(engine);  // registration goes into the global QQmlMetaType registry
    static bool once = false;
    if (once) {
        return;
    }
    once = true;

#if defined(SA_RIBBON_QML_STATIC) || defined(QT_STATIC)
    Q_INIT_RESOURCE(saribbon_qml);  // static library qrc does not auto-load
#endif

    // ---- singletons (callback form; CppOwnership set inside, plan-04 S1-3) ----
    qmlRegisterSingletonType< SARibbonQml::RibbonTheme >(
        "SARibbon", 3, 0, "RibbonTheme",
        [](QQmlEngine* qmlEngine, QJSEngine*) -> QObject* {
            QObject* o = SARibbonQml::RibbonTheme::instance();
            QQmlEngine::setObjectOwnership(o, QQmlEngine::CppOwnership);
            return o;
        });
    qmlRegisterSingletonType< SARibbonQml::RibbonMetrics >(
        "SARibbon", 3, 0, "RibbonMetrics",
        [](QQmlEngine* qmlEngine, QJSEngine*) -> QObject* {
            QObject* o = SARibbonQml::RibbonMetrics::instance();
            QQmlEngine::setObjectOwnership(o, QQmlEngine::CppOwnership);
            return o;
        });

    // ---- types (filled in plan-04 S3-S5) ----
    qmlRegisterType< SARibbonQml::RibbonBar >("SARibbon", 3, 0, "RibbonBar");
    qmlRegisterType< SARibbonQml::RibbonCategory >("SARibbon", 3, 0, "RibbonCategory");
    qmlRegisterType< SARibbonQml::RibbonTab >("SARibbon", 3, 0, "RibbonTab");
    qmlRegisterType< SARibbonQml::RibbonPanel >("SARibbon", 3, 0, "RibbonPanel");
    qmlRegisterType< SARibbonQml::RibbonToolButton >("SARibbon", 3, 0, "RibbonToolButton");

    // Uncreatable enum holder (enums mirrored via Q_ENUM on registered classes;
    // Q_NAMESPACE route is closed: 3.0 enums stay in the global namespace, plan-02 S1)
    qmlRegisterUncreatableType< SARibbonQml::RibbonEnums >(
        "SARibbon", 3, 0, "Ribbon", "Enum access only");

    qmlRegisterModule("SARibbon", 3, 0);
}
