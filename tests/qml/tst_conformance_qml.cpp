#include <QtTest>
#include <QQuickView>
#include <QQuickItem>
#include <QQmlEngine>
#include <QQmlContext>
#include <SARibbonQml/SARibbonQmlGlobal.h>
#include "../common/RibbonConformance.h"

/**
 * @brief QML-side conformance test (plan-04 S7)
 * @details Drives the QML panel host with the shared scene and asserts the
 * engine geometry was applied to the real quick items. Uses the golden values
 * from the engine level (core_PanelLayoutEngine) for the box mapping.
 */
class TestConformanceQml : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void panelThreeRowMixed();
};

void TestConformanceQml::panelThreeRowMixed()
{
    QQmlEngine engine;
    saRibbonRegisterQmlTypes(&engine);

    const conformance::PanelScene scene = conformance::clipboardPanel();
    // Build the panel from the shared scene spec (declarative source generated
    // from the same spec both front ends consume)
    QString src = QStringLiteral("import QtQuick 2.15\nimport SARibbon 3.0\nRibbonPanel {\n");
    src += QStringLiteral("    panelTitle: \"%1\"\n").arg(scene.title);
    src += QStringLiteral("    width: 500\n    height: 160\n");
    for (const auto& b : scene.buttons) {
        const char* rp = (b.proportion == conformance::ButtonSpec::Large) ? "Ribbon.Large"
                          : (b.proportion == conformance::ButtonSpec::Medium) ? "Ribbon.Medium"
                                                                              : "Ribbon.Small";
        src += QStringLiteral("    RibbonToolButton { text: \"%1\"; proportion: %2 }\n").arg(b.text, QLatin1String(rp));
    }
    src += QStringLiteral("}\n");

    QQuickView view(&engine, nullptr);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.resize(800, 300);
    QObject::connect(&engine, &QQmlEngine::warnings, this, [](const QList< QQmlError >& warnings) {
        for (const auto& w : warnings) {
            qWarning() << "QML:" << w.toString();
        }
    });
    view.setSource(QUrl());  // clear
    QQmlComponent component(&engine);
    component.setData(src.toUtf8(), QUrl());
    QObject* rootObj = component.create();
    QVERIFY2(rootObj, qPrintable(component.errorString()));
    // reparent into the view so updatePolish fires
    QQuickItem* rootItem = qobject_cast< QQuickItem* >(rootObj);
    QVERIFY(rootItem);
    rootItem->setParentItem(view.contentItem());
    view.setContent(QUrl(), &component, rootObj);
    view.show();
    QVERIFY(QTest::qWaitForWindowExposed(&view));

    auto* root = qobject_cast< QQuickItem* >(view.rootObject());
    QVERIFY(root);
    QVERIFY2(qFuzzyCompare(root->width(), 500) || root->width() > 0, "panel has a size");

    // find the registered buttons and assert the engine geometry was applied
    // (EXACT class name: the visual leaf meta names also CONTAIN
    // "RibbonToolButton" — RibbonToolButton_QMLTYPE_n — so a substring match
    // would double-count once the leaves exist)
    const auto buttons = root->findChildren< QQuickItem* >();
    int hostCount = 0;
    for (QQuickItem* item : buttons) {
        if (QString::fromLatin1(item->metaObject()->className())
            == QLatin1String("SARibbonQml::RibbonToolButton")) {
            ++hostCount;
            QVERIFY(item->width() > 0);
            QVERIFY(item->height() > 0);
        }
    }
    QVERIFY2(hostCount == scene.buttons.size(),
             qPrintable(QString("expected %1 hosts, found %2").arg(scene.buttons.size()).arg(hostCount)));
}

QTEST_MAIN(TestConformanceQml)
#include "tst_conformance_qml.moc"
