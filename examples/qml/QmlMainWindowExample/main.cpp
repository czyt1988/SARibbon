#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <SARibbonQml/SARibbonQmlGlobal.h>

int main(int argc, char* argv[])
{
    // Basic style on both Qt5/Qt6 keeps screenshots comparable (plan-04 S6)
    qputenv("QT_QUICK_CONTROLS_STYLE", "Basic");

    QGuiApplication app(argc, argv);
    QQuickStyle::setStyle(QStringLiteral("Basic"));

    QQmlApplicationEngine engine;
    saRibbonRegisterQmlTypes(&engine);  // imperative single-track: before load()
    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }
    return app.exec();
}
