#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlError>
#include <cstdio>
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
    // a GUI-subsystem app has no stderr by default; route Qt messages there
    // so a failed QML load is diagnosable under headless verification
    qInstallMessageHandler([](QtMsgType, const QMessageLogContext&, const QString& msg) {
        std::fputs(qPrintable(msg), stderr);
        std::fputc('\n', stderr);
        std::fflush(stderr);
    });
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
    // surface QML load diagnostics: a silent -1 exit helps nobody (headless
    // verification reads stderr through redirections)
    QObject::connect(&engine, &QQmlApplicationEngine::warnings, &engine,
                     [](const QList< QQmlError >& warnings) {
                         for (const QQmlError& w : warnings) {
                             std::fputs(qPrintable(w.toString()), stderr);
                             std::fputc('\n', stderr);
                         }
                         std::fflush(stderr);
                     });
    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }
    return app.exec();
}
