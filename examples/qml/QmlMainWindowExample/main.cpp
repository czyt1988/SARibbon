#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QFont>
#include <SARibbonQml/SARibbonQmlGlobal.h>
#include "ribbonbackend.h"

int main(int argc, char* argv[])
{
    // A flat built-in style on both Qt5/Qt6 keeps screenshots comparable
    // (plan-04 S6); "Basic" is the Qt6 name, "Default" its Qt5 spelling
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    qputenv("QT_QUICK_CONTROLS_STYLE", "Default");
#else
    qputenv("QT_QUICK_CONTROLS_STYLE", "Basic");
#endif

    QGuiApplication app(argc, argv);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QQuickStyle::setStyle(QStringLiteral("Default"));
#else
    QQuickStyle::setStyle(QStringLiteral("Basic"));
#endif
    // same default font family as the widgets MainWindowExample so the
    // same-screen comparison stays honest (metrics are font-derived)
    QFont f = app.font();
    f.setFamily(QStringLiteral("Microsoft YaHei"));
    f.setPointSize(9);
    app.setFont(f);

    QQmlApplicationEngine engine;
    saRibbonRegisterQmlTypes(&engine);  // imperative single-track: before load()
    // command-layer backend (plan-05): plain QAction objects owned by C++,
    // bound by the QML views through their `action` properties
    RibbonBackend backend;
    engine.rootContext()->setContextProperty(QStringLiteral("backend"), &backend);
    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }
    return app.exec();
}
