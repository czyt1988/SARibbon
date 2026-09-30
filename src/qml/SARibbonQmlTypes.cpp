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
#include <QQmlComponent>
#include <QQuickItem>
#include <QFile>
#include <QDebug>
#include <QtQml>

namespace SARibbonQml {

QQuickItem* createVisualLeaf(QQuickItem* host, const QUrl& leafUrl, const char* handshakeProperty)
{
    if (!host || !handshakeProperty) {
        return nullptr;
    }
    QQmlEngine* engine = qmlEngine(host);
    if (!engine) {
        qWarning() << "SARibbonQml: no QML engine reachable from host, cannot create leaf" << leafUrl;
        return nullptr;
    }
    // QFile understands the ":/..." form but not "qrc:/..." URLs (exists() would
    // always report false for the URL form) — convert for the existence check
    QString resourcePath = leafUrl.toString();
    if (leafUrl.scheme() == QLatin1String("qrc")) {
        resourcePath = QLatin1Char(':') + leafUrl.path();
    }
    if (!QFile::exists(resourcePath)) {
        qWarning() << "SARibbonQml: leaf resource missing (static build without Q_INIT_RESOURCE?)" << leafUrl;
        return nullptr;
    }
    QQmlComponent component(engine, leafUrl);
    QObject* obj = component.create();
    if (!obj) {
        qWarning() << "SARibbonQml: leaf create() failed:" << leafUrl << component.errorString();
        return nullptr;
    }
    QQuickItem* leaf = qobject_cast< QQuickItem* >(obj);
    if (!leaf) {
        qWarning() << "SARibbonQml: leaf root is not a QQuickItem:" << leafUrl;
        obj->deleteLater();
        return nullptr;
    }
    // trilogy step 2: handshake injection — the leaf's onXxxCppChanged handler
    // assigns itself back into the host's xxxQmlItem property
    leaf->setProperty(handshakeProperty, QVariant::fromValue(host));
    // trilogy step 3: reparent onto the host (both parents, KDDW Group.cpp rule)
    leaf->setParentItem(host);
    leaf->setParent(host);
    leaf->setZ(-1);  // background layer under any structural children
    return leaf;
}

}  // namespace SARibbonQml

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
