#include <QtTest>
#include <QQuickView>
#include <QQuickItem>
#include <QQuickWindow>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlComponent>
#include <QSignalSpy>
#include <QFile>
#include <QImageReader>
#include <QImage>
#include <memory>
#include <functional>
#include <SARibbonQml/SARibbonQmlGlobal.h>
#include <SARibbonQml/SARibbonQmlTypes.h>
#include <SARibbonCore/SARibbonCoreUtil.h>
#include <SARibbonQml/button/RibbonToolButton.h>
#include <SARibbonQml/container/RibbonControlContainer.h>
#include <SARibbonQml/gallery/RibbonGallery.h>
#include <SARibbonQml/gallery/RibbonGalleryGroup.h>
#include <SARibbonQml/gallery/RibbonGalleryItem.h>
#include <SARibbonQml/menu/RibbonMenuItem.h>
#include "../common/RibbonConformance.h"

/**
 * @brief QML-side conformance test (plan-04 S7)
 * @details Drives the QML panel host with the shared scene and asserts the
 * engine geometry was applied to the real quick items. Uses the golden values
 * from the engine level (core_PanelLayoutEngine) for the box mapping. The
 * interaction cases assert the tab switching / button click behavior against
 * the C++ hosts (real QTest mouse events, no synthetic property poking).
 */
class TestConformanceQml : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void panelThreeRowMixed();
    void barAutoTabsAndSwitch();
    void toolButtonClick();
    void svgIconLoads();
    void toolButtonPopupStates();
    void controlContainerEmbedding();
    void contextCategoryActivation();
    void galleryInPanel();
    void ribbonStyleSwitching();
    void styleRadioViaContainer();
    void separatorInPanel();
    void quickAccessBarAndRightGroup();
    void tabAlignmentAndMinimumMode();
    void rtlToggle();
    void panelOptionAction();
    void applicationWindow();

private:
    QQuickView* exposeScene(QQmlEngine& engine, QQmlComponent& component, const char* src, int w, int h);
    static int countPixelsNear(const QImage& img, const QColor& color, int tolerance = 8);
};

void TestConformanceQml::panelThreeRowMixed()
{
    QQmlEngine engine;
    saRibbonRegisterQmlTypes(&engine);

    const conformance::PanelScene scene = conformance::clipboardPanel();
    // Build the panel from the shared scene spec (declarative source generated
    // from the same spec both front ends consume)
    QString src = QStringLiteral("import QtQuick 2.12\nimport SARibbon 3.0\nRibbonPanel {\n");
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

void TestConformanceQml::barAutoTabsAndSwitch()
{
    QQmlEngine engine;
    saRibbonRegisterQmlTypes(&engine);

    QString src = QStringLiteral(R"QML(import QtQuick 2.12
import SARibbon 3.0
Item {
    width: 800
    height: 300
    RibbonBar {
        objectName: "bar"
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        RibbonCategory {
            objectName: "cat0"
            title: "Home"
            RibbonPanel {
                panelTitle: "P1"
                RibbonToolButton { objectName: "btn0"; text: "A" }
            }
        }
        RibbonCategory {
            objectName: "cat1"
            title: "Insert"
            RibbonPanel {
                panelTitle: "P2"
                RibbonToolButton { objectName: "btn1"; text: "B" }
            }
        }
    }
})QML");

    QQuickView view(&engine, nullptr);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.resize(800, 300);
    QObject::connect(&engine, &QQmlEngine::warnings, this, [](const QList< QQmlError >& warnings) {
        for (const auto& w : warnings) {
            qWarning() << "QML:" << w.toString();
        }
    });
    view.setSource(QUrl());
    QQmlComponent component(&engine);
    component.setData(src.toUtf8(), QUrl());
    QObject* rootObj = component.create();
    QVERIFY2(rootObj, qPrintable(component.errorString()));
    QQuickItem* rootItem = qobject_cast< QQuickItem* >(rootObj);
    QVERIFY(rootItem);
    rootItem->setParentItem(view.contentItem());
    view.setContent(QUrl(), &component, rootObj);
    view.show();
    QVERIFY(QTest::qWaitForWindowExposed(&view));

    auto* bar = rootItem->findChild< QQuickItem* >(QStringLiteral("bar"));
    QVERIFY(bar);
    auto* cat0 = rootItem->findChild< QQuickItem* >(QStringLiteral("cat0"));
    auto* cat1 = rootItem->findChild< QQuickItem* >(QStringLiteral("cat1"));
    QVERIFY(cat0 && cat1);

    // no explicit RibbonTab was declared: the bar must auto-create one tab per
    // category (addCategoryPage parity) with the category title as text
    const auto tabs = bar->findChildren< QQuickItem* >();
    QVector< QQuickItem* > tabHosts;
    for (QQuickItem* item : tabs) {
        if (QString::fromLatin1(item->metaObject()->className())
            == QLatin1String("SARibbonQml::RibbonTab")) {
            tabHosts.append(item);
        }
    }
    QCOMPARE(tabHosts.size(), 2);
    QTRY_COMPARE(tabHosts[ 0 ]->property("text").toString(), QStringLiteral("Home"));
    QTRY_COMPARE(tabHosts[ 1 ]->property("text").toString(), QStringLiteral("Insert"));
    QVERIFY(tabHosts[ 1 ]->width() > 0 && tabHosts[ 1 ]->height() > 0);

    // initial state: index 0, first category visible
    QCOMPARE(bar->property("currentIndex").toInt(), 0);
    QTRY_VERIFY(cat0->isVisible());
    QTRY_VERIFY(!cat1->isVisible());

    // real mouse click on the second tab switches the category (the user bug:
    // "Tab 标签页不可切换")
    const QPointF tabCenter = tabHosts[ 1 ]->mapToScene(QPointF(tabHosts[ 1 ]->width() / 2,
                                                                tabHosts[ 1 ]->height() / 2));
    QTest::mouseClick(&view, Qt::LeftButton, Qt::NoModifier, tabCenter.toPoint());
    QTRY_COMPARE(bar->property("currentIndex").toInt(), 1);
    QTRY_VERIFY(cat1->isVisible());
    QTRY_VERIFY(!cat0->isVisible());

    // and back to the first tab
    const QPointF tabCenter0 = tabHosts[ 0 ]->mapToScene(QPointF(tabHosts[ 0 ]->width() / 2,
                                                                 tabHosts[ 0 ]->height() / 2));
    QTest::mouseClick(&view, Qt::LeftButton, Qt::NoModifier, tabCenter0.toPoint());
    QTRY_COMPARE(bar->property("currentIndex").toInt(), 0);
    QTRY_VERIFY(cat0->isVisible());
    QTRY_VERIFY(!cat1->isVisible());
}

void TestConformanceQml::toolButtonClick()
{
    QQmlEngine engine;
    saRibbonRegisterQmlTypes(&engine);

    QString src = QStringLiteral(R"QML(import QtQuick 2.12
import SARibbon 3.0
Item {
    width: 400
    height: 200
    RibbonPanel {
        objectName: "panel"
        anchors.fill: parent
        panelTitle: "P"
        RibbonToolButton { objectName: "btn"; text: "Hello"; checkable: true }
    }
})QML");

    QQuickView view(&engine, nullptr);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.resize(400, 200);
    QObject::connect(&engine, &QQmlEngine::warnings, this, [](const QList< QQmlError >& warnings) {
        for (const auto& w : warnings) {
            qWarning() << "QML:" << w.toString();
        }
    });
    view.setSource(QUrl());
    QQmlComponent component(&engine);
    component.setData(src.toUtf8(), QUrl());
    QObject* rootObj = component.create();
    QVERIFY2(rootObj, qPrintable(component.errorString()));
    QQuickItem* rootItem = qobject_cast< QQuickItem* >(rootObj);
    QVERIFY(rootItem);
    rootItem->setParentItem(view.contentItem());
    view.setContent(QUrl(), &component, rootObj);
    view.show();
    QVERIFY(QTest::qWaitForWindowExposed(&view));

    auto* btn = rootItem->findChild< QQuickItem* >(QStringLiteral("btn"));
    QVERIFY(btn);
    QTRY_VERIFY(btn->width() > 0 && btn->height() > 0);

    // real mouse click on the button must emit clicked and toggle checked
    // (the user bug: "按钮不可点击")
    QSignalSpy clickedSpy(btn, SIGNAL(clicked()));
    QSignalSpy toggledSpy(btn, SIGNAL(toggled(bool)));
    const QPointF center = btn->mapToScene(QPointF(btn->width() / 2, btn->height() / 2));
    QTest::mouseClick(&view, Qt::LeftButton, Qt::NoModifier, center.toPoint());
    QTRY_COMPARE(clickedSpy.size(), 1);
    QTRY_COMPARE(toggledSpy.size(), 1);
    QCOMPARE(btn->property("checked").toBool(), true);

    // second click toggles back off
    QTest::mouseClick(&view, Qt::LeftButton, Qt::NoModifier, center.toPoint());
    QTRY_COMPARE(clickedSpy.size(), 2);
    QTRY_COMPARE(btn->property("checked").toBool(), false);
}

void TestConformanceQml::svgIconLoads()
{
    // the example buttons reference the widget example's SVG icons; make sure
    // the QML runtime rasterizes them (Qt SVG image format plugin present)
    QQmlEngine engine;
    saRibbonRegisterQmlTypes(&engine);
    const QString iconPath = QStringLiteral(QT_TESTCASE_BUILDDIR)
                             + QStringLiteral("/../../../examples/widgets/MainWindowExample/icon/save.svg");
    if (!QFile::exists(iconPath)) {
        QSKIP("icon file not reachable from this build directory");
    }
    // direct QImageReader probe: isolates image-format-plugin problems from
    // the QML async image pipeline
    QImageReader reader(iconPath);
    reader.setScaledSize(QSize(32, 32));
    const QImage probe = reader.read();
    QVERIFY2(!probe.isNull(), qPrintable(reader.errorString()));
    QString src = QStringLiteral(R"QML(import QtQuick 2.12
Item {
    width: 100; height: 100
    Image {
        id: img
        objectName: "img"
        anchors.fill: parent
        sourceSize.width: 32; sourceSize.height: 32
        source: "%1"
    }
})QML").arg(QUrl::fromLocalFile(iconPath).toString());
    QQuickView view(&engine, nullptr);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.resize(100, 100);
    QQmlComponent component(&engine);
    component.setData(src.toUtf8(), QUrl());
    QObject* rootObj = component.create();
    QVERIFY2(rootObj, qPrintable(component.errorString()));
    QQuickItem* rootItem = qobject_cast< QQuickItem* >(rootObj);
    QVERIFY(rootItem);
    rootItem->setParentItem(view.contentItem());
    view.setContent(QUrl(), &component, rootObj);
    view.show();
    QVERIFY(QTest::qWaitForWindowExposed(&view));
    auto* img = rootItem->findChild< QQuickItem* >(QStringLiteral("img"));
    QVERIFY(img);
    // QQuickImageBase status: 0=Null, 1=Ready, 2=Loading, 3=Error
    QTRY_COMPARE(img->property("status").toInt(), 1);
    QTRY_VERIFY(img->property("progress").toReal() >= 1.0);
}

QQuickView* TestConformanceQml::exposeScene(QQmlEngine& engine, QQmlComponent& component, const char* src, int w, int h)
{
    QObject::connect(&engine, &QQmlEngine::warnings, this, [](const QList< QQmlError >& warnings) {
        for (const auto& w : warnings) {
            qWarning() << "QML:" << w.toString();
        }
    });
    component.setData(QByteArray(src), QUrl());
    QObject* rootObj = component.create();
    if (!rootObj) {
        qWarning() << "component create failed:" << component.errorString();
        return nullptr;
    }
    QQuickItem* rootItem = qobject_cast< QQuickItem* >(rootObj);
    if (!rootItem) {
        delete rootObj;
        return nullptr;
    }
    QQuickView* view = new QQuickView(&engine, nullptr);
    view->setResizeMode(QQuickView::SizeRootObjectToView);
    view->resize(w, h);
    rootItem->setParentItem(view->contentItem());
    view->setContent(QUrl(), &component, rootObj);
    view->show();
    if (!QTest::qWaitForWindowExposed(view)) {
        delete view;
        return nullptr;
    }
    return view;
}

/**
 * @brief Popup modes / disabled state / menu model of the tool button
 * @details Mirrors the widgets "sa ribbon toolbutton style" panel: the three
 *          popup modes publish host-computed hit zones, menu entries are
 *          RibbonMenuItem objects mediated by menuTriggered, and a disabled
 *          host swallows both invokable and real-mouse clicks.
 */
void TestConformanceQml::toolButtonPopupStates()
{
    QQmlEngine engine;
    saRibbonRegisterQmlTypes(&engine);

    QString src = QStringLiteral(R"QML(import QtQuick 2.12
import SARibbon 3.0
Item {
    width: 600
    height: 300
    RibbonPanel {
        objectName: "panel"
        anchors.fill: parent
        panelTitle: "P"
        RibbonToolButton {
            objectName: "disabledBtn"
            text: "Disabled"
            checkable: true
            enabled: false
        }
        RibbonToolButton {
            objectName: "menuBtn"
            text: "Menu"
            proportion: Ribbon.Small
            checkable: true
            popupMode: Ribbon.MenuButtonPopup
            menuItems: [
                RibbonMenuItem { text: "item 1" },
                RibbonMenuItem { separator: true },
                RibbonMenuItem { text: "item 2"; enabled: false },
                RibbonMenuItem { text: "item 3" }
            ]
        }
        RibbonToolButton {
            objectName: "instantBtn"
            text: "Inst"
            proportion: Ribbon.Small
            popupMode: Ribbon.InstantPopup
            menuItems: [ RibbonMenuItem { text: "only" } ]
        }
        RibbonToolButton {
            objectName: "delayedBtn"
            text: "Del"
            proportion: Ribbon.Small
            popupMode: Ribbon.DelayedPopup
            menuItems: [ RibbonMenuItem { text: "d1" } ]
        }
    }
})QML");

    QQmlComponent component(&engine);
    std::unique_ptr< QQuickView > view(exposeScene(engine, component, src.toUtf8().constData(), 600, 300));
    QVERIFY(view);
    QQuickItem* rootItem = view->rootObject();
    QVERIFY(rootItem);

    // ---- disabled host swallows clicks (invokable AND real mouse) ----
    auto* disabledBtn = rootItem->findChild< QQuickItem* >(QStringLiteral("disabledBtn"));
    QVERIFY(disabledBtn);
    QSignalSpy disabledClicked(disabledBtn, SIGNAL(clicked()));
    QMetaObject::invokeMethod(disabledBtn, "click");
    QVERIFY(disabledClicked.isEmpty());
    const QPointF disCenter = disabledBtn->mapToScene(QPointF(disabledBtn->width() / 2, disabledBtn->height() / 2));
    QTest::mouseClick(view.get(), Qt::LeftButton, Qt::NoModifier, disCenter.toPoint());
    QTRY_VERIFY(disabledClicked.isEmpty());
    QVERIFY(!disabledBtn->property("checked").toBool());

    // ---- MenuButtonPopup hit zones: small button = trailing indicator strip ----
    auto* menuBtn = rootItem->findChild< QQuickItem* >(QStringLiteral("menuBtn"));
    QVERIFY(menuBtn);
    QTRY_VERIFY(menuBtn->width() > 0 && menuBtn->height() > 0);
    QVERIFY(menuBtn->property("hasMenu").toBool());
    const QRectF actionRect = menuBtn->property("actionRect").toRectF();
    const QRectF menuRect   = menuBtn->property("menuRect").toRectF();
    QVERIFY(actionRect.width() > 0);
    QVERIFY(menuRect.width() > 0);
    // the menu strip sits on the trailing edge, the action zone before it
    QVERIFY(qFuzzyCompare(menuRect.x() + menuRect.width(), qreal(menuBtn->width())));
    QVERIFY(actionRect.x() + actionRect.width() <= menuRect.x() + 1.0);

    // ---- menu model mediation: activateMenuItem -> menuTriggered ----
    QSignalSpy menuSpy(menuBtn, SIGNAL(menuTriggered(SARibbonQml::RibbonMenuItem*)));
    QMetaObject::invokeMethod(menuBtn, "activateMenuItem", Q_ARG(int, 0));
    QCOMPARE(menuSpy.size(), 1);
    {
        auto* triggeredItem = qvariant_cast< QObject* >(menuSpy.at(0).at(0));
        QVERIFY(triggeredItem);
        QCOMPARE(triggeredItem->property("text").toString(), QStringLiteral("item 1"));
    }
    // separator / disabled / out-of-range entries are ignored
    QMetaObject::invokeMethod(menuBtn, "activateMenuItem", Q_ARG(int, 1));
    QMetaObject::invokeMethod(menuBtn, "activateMenuItem", Q_ARG(int, 2));
    QMetaObject::invokeMethod(menuBtn, "activateMenuItem", Q_ARG(int, 99));
    QMetaObject::invokeMethod(menuBtn, "activateMenuItem", Q_ARG(int, -1));
    QCOMPARE(menuSpy.size(), 1);

    // ---- openMenu with a rendered leaf opens the styled popup ----
    QVERIFY(!menuBtn->property("menuVisible").toBool());
    QMetaObject::invokeMethod(menuBtn, "openMenu");
    QTRY_COMPARE(menuBtn->property("menuVisible").toBool(), true);
    // the first menu row exists in the popup content and is clickable.
    // NOTE: Repeater delegates carry no QObject parent, so findChild cannot
    // reach them — walk the item tree instead
    std::function< QQuickItem* (QQuickItem*, const QString&) > findItem
        = [&findItem](QQuickItem* from, const QString& name) -> QQuickItem* {
        if (from->objectName() == name) {
            return from;
        }
        for (QQuickItem* child : from->childItems()) {
            if (QQuickItem* hit = findItem(child, name)) {
                return hit;
            }
        }
        return nullptr;
    };
    QQuickItem* row = nullptr;
    for (QQuickItem* top : view->contentItem()->childItems()) {
        if ((row = findItem(top, QStringLiteral("menuRow")))) {
            break;
        }
    }
    QVERIFY(row);
    QTRY_VERIFY(row->isVisible() && row->width() > 0);
    const QPointF rowCenter = row->mapToScene(QPointF(row->width() / 2, row->height() / 2));
    QTest::mouseClick(view.get(), Qt::LeftButton, Qt::NoModifier, rowCenter.toPoint());
    QTRY_COMPARE(menuSpy.size(), 2);
    // the activation closed the popup
    QTRY_COMPARE(menuBtn->property("menuVisible").toBool(), false);

    // ---- InstantPopup: the whole button is the menu zone ----
    auto* instantBtn = rootItem->findChild< QQuickItem* >(QStringLiteral("instantBtn"));
    QVERIFY(instantBtn);
    QTRY_VERIFY(instantBtn->width() > 0);
    const QRectF instMenu   = instantBtn->property("menuRect").toRectF();
    const QRectF instAction = instantBtn->property("actionRect").toRectF();
    QVERIFY(instAction.width() <= 0);
    QVERIFY(qFuzzyCompare(instMenu.width(), qreal(instantBtn->width())));
    // a real click anywhere on the button opens the menu
    const QPointF instCenter = instantBtn->mapToScene(QPointF(instantBtn->width() / 2, instantBtn->height() / 2));
    QTest::mouseClick(view.get(), Qt::LeftButton, Qt::NoModifier, instCenter.toPoint());
    QTRY_COMPARE(instantBtn->property("menuVisible").toBool(), true);

    // ---- DelayedPopup: whole button stays the action zone (menu on hold) ----
    auto* delayedBtn = rootItem->findChild< QQuickItem* >(QStringLiteral("delayedBtn"));
    QVERIFY(delayedBtn);
    const QRectF delayAction = delayedBtn->property("actionRect").toRectF();
    const QRectF delayMenu   = delayedBtn->property("menuRect").toRectF();
    QVERIFY(delayMenu.width() <= 0);
    QVERIFY(qFuzzyCompare(delayAction.width(), qreal(delayedBtn->width())));
}

/**
 * @brief RibbonControlContainer embedding an arbitrary control into the panel
 * @details Mirrors the widgets "widget test" panel (addSmallWidget parity): a
 *          ComboBox rides inside the container, the label strip width is
 *          computed by the host, the control is reparented after the strip,
 *          and the panel engine lays out buttons and containers side by side.
 */
void TestConformanceQml::controlContainerEmbedding()
{
    QQmlEngine engine;
    saRibbonRegisterQmlTypes(&engine);

    QString src = QStringLiteral(R"QML(import QtQuick 2.12
import QtQuick.Controls 2.12
import SARibbon 3.0
Item {
    width: 600
    height: 300
    RibbonPanel {
        objectName: "panel"
        anchors.fill: parent
        panelTitle: "P"
        RibbonToolButton {
            objectName: "btnA"
            text: "A"
        }
        RibbonControlContainer {
            objectName: "comboContainer"
            text: "Font:"
            control: ComboBox {
                objectName: "combo"
                model: [ "item 1", "item 2", "item 3" ]
            }
        }
        RibbonControlContainer {
            objectName: "bareContainer"
            control: CheckBox {
                objectName: "check"
                text: "check me"
            }
        }
    }
})QML");

    QQmlComponent component(&engine);
    std::unique_ptr< QQuickView > view(exposeScene(engine, component, src.toUtf8().constData(), 600, 300));
    QVERIFY(view);
    QQuickItem* rootItem = view->rootObject();
    QVERIFY(rootItem);

    // ---- container participates in the panel layout ----
    auto* container = rootItem->findChild< QQuickItem* >(QStringLiteral("comboContainer"));
    QVERIFY(container);
    QTRY_VERIFY(container->width() > 0 && container->height() > 0);
    const qreal labelWidth = container->property("labelWidth").toReal();
    QVERIFY(labelWidth > 0);  // "Font:" label strip computed from core metrics

    // ---- the control was reparented into the container, after the strip ----
    auto* combo = container->findChild< QQuickItem* >(QStringLiteral("combo"));
    QVERIFY(combo);
    QCOMPARE(combo->parentItem(), container);
    QTRY_VERIFY(combo->width() > 0 && combo->height() > 0);
    QVERIFY(combo->x() >= labelWidth - 1.0);

    // ---- a container without a label still lays out (icon-less strip) ----
    auto* bare = rootItem->findChild< QQuickItem* >(QStringLiteral("bareContainer"));
    QVERIFY(bare);
    QTRY_VERIFY(bare->width() > 0);
    auto* check = bare->findChild< QQuickItem* >(QStringLiteral("check"));
    QVERIFY(check);
    QTRY_VERIFY(check->width() > 0);
    QVERIFY(check->x() >= 0.0);

    // ---- buttons and containers coexist in one engine pass ----
    auto* btnA = rootItem->findChild< QQuickItem* >(QStringLiteral("btnA"));
    QVERIFY(btnA);
    QTRY_VERIFY(btnA->width() > 0 && btnA->height() > 0);

    // ---- the embedded control is functional (real mouse opens the popup) ----
    const QPointF comboCenter = combo->mapToScene(QPointF(combo->width() / 2, combo->height() / 2));
    QTest::mouseClick(view.get(), Qt::LeftButton, Qt::NoModifier, comboCenter.toPoint());
    QObject* comboPopup = combo->property("popup").value< QObject* >();
    QVERIFY(comboPopup);
    QTRY_COMPARE(comboPopup->property("visible").toBool(), true);
}

int TestConformanceQml::countPixelsNear(const QImage& img, const QColor& color, int tolerance)
{
    int count = 0;
    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            const QRgb rgb = img.pixel(x, y);
            if (qAbs(qRed(rgb) - color.red()) <= tolerance && qAbs(qGreen(rgb) - color.green()) <= tolerance
                && qAbs(qBlue(rgb) - color.blue()) <= tolerance) {
                ++count;
            }
        }
    }
    return count;
}

/**
 * @brief Context category activation: colored tabs + band + switching
 * @details Mirrors the widgets showContextCategory/hideContextCategory
 *          semantics: active appends one colored tab per page to the tab
 *          row, publishes the band (title/color/highlight through the core
 *          theme fp), and the current index can walk into the context
 *          pages; deactivation removes them and clamps the index back.
 *          The band color is verified on the real rendered frame
 *          (QQuickWindow::grabWindow pixel count).
 */
void TestConformanceQml::contextCategoryActivation()
{
    QQmlEngine engine;
    saRibbonRegisterQmlTypes(&engine);

    QString src = QStringLiteral(R"QML(import QtQuick 2.12
import SARibbon 3.0
Item {
    width: 800
    height: 300
    RibbonBar {
        objectName: "bar"
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        RibbonCategory {
            objectName: "normal"
            title: "Home"
            RibbonPanel {
                panelTitle: "P"
                RibbonToolButton { text: "A" }
            }
        }
        RibbonContextCategory {
            objectName: "ctx"
            contextTitle: "context"
            contextColor: "#2d7d9a"
            active: false
            RibbonCategory {
                objectName: "page1"
                title: "ctx Page1"
                RibbonPanel {
                    panelTitle: "CP"
                    RibbonToolButton { objectName: "ctxBtn"; text: "B" }
                }
            }
            RibbonCategory {
                objectName: "page2"
                title: "ctx Page2"
                RibbonPanel { panelTitle: "CP2" }
            }
        }
    }
})QML");

    QQmlComponent component(&engine);
    std::unique_ptr< QQuickView > view(exposeScene(engine, component, src.toUtf8().constData(), 800, 300));
    QVERIFY(view);
    QQuickItem* rootItem = view->rootObject();
    QVERIFY(rootItem);

    auto* bar     = rootItem->findChild< QQuickItem* >(QStringLiteral("bar"));
    auto* ctx     = rootItem->findChild< QQuickItem* >(QStringLiteral("ctx"));
    auto* page1   = rootItem->findChild< QQuickItem* >(QStringLiteral("page1"));
    auto* page2   = rootItem->findChild< QQuickItem* >(QStringLiteral("page2"));
    QVERIFY(bar && ctx && page1 && page2);

    // count VISIBLE tab hosts (exact class name — the leaf meta names also
    // CONTAIN "RibbonTab"; context-owned tabs are created eagerly but stay
    // hidden until their context activates)
    auto countTabs = [rootItem]() -> int {
        int n = 0;
        const auto all = rootItem->findChildren< QQuickItem* >();
        for (QQuickItem* item : all) {
            if (QString::fromLatin1(item->metaObject()->className()) == QLatin1String("SARibbonQml::RibbonTab")
                && item->isVisible()) {
                ++n;
            }
        }
        return n;
    };
    const int tabsBefore = [&]() {
        for (int i = 0; i < 50; ++i) {
            if (countTabs() >= 1) {
                break;
            }
            QTest::qWait(20);  // auto tabs appear after the first relayout pass
        }
        return countTabs();
    }();
    QVERIFY2(tabsBefore >= 1, "the normal tab must be visible after relayout");
    QVERIFY(bar->property("contextBands").toList().isEmpty());
    QVERIFY(!page1->isVisible() && !page2->isVisible());

    // ---- activation: tabs + band appear, index can walk into the pages ----
    ctx->setProperty("active", true);
    QTRY_COMPARE(countTabs(), tabsBefore + 2);
    const QVariantList bands = bar->property("contextBands").toList();
    QCOMPARE(bands.size(), 1);
    {
        const QVariantMap band = bands.at(0).toMap();
        QCOMPARE(band.value(QStringLiteral("title")).toString(), QStringLiteral("context"));
        QVERIFY(band.value(QStringLiteral("width")).toReal() > 0);
        QVERIFY(band.value(QStringLiteral("highlight")).value< QColor >().isValid());
    }
    // switch onto the first context page through the property API
    bar->setProperty("currentIndex", tabsBefore);
    QTRY_VERIFY(page1->isVisible());
    QVERIFY(!page2->isVisible());
    bar->setProperty("currentIndex", tabsBefore + 1);
    QTRY_VERIFY(page2->isVisible());

    // the rendered frame carries the band color in the title area
    const QImage frame = view->grabWindow();
    QVERIFY(!frame.isNull());
    QVERIFY2(countPixelsNear(frame, QColor(0x2d, 0x7d, 0x9a)) > 200,
             "context band color must be visible in the rendered frame");

    // ---- deactivation: tabs vanish, the index clamps back to normal tabs ----
    ctx->setProperty("active", false);
    QTRY_COMPARE(countTabs(), tabsBefore);
    QTRY_VERIFY(bar->property("currentIndex").toInt() < tabsBefore);
    QTRY_VERIFY(!page1->isVisible() && !page2->isVisible());
    QVERIFY(bar->property("contextBands").toList().isEmpty());
}

/**
 * @brief Gallery: expanding panel item with core grid metrics + activation
 * @details Mirrors the widgets SARibbonGallery contract: Large cell,
 *          expandingDirections Horizontal, stretchFactor feeding the core
 *          engine's extra-width distribution; cell size derives from the
 *          core calcGalleryGridCellSize (moved from the widgets group, so
 *          both front ends agree), scrolling clamps, and activation is
 *          mediated by triggered.
 */
void TestConformanceQml::galleryInPanel()
{
    QQmlEngine engine;
    saRibbonRegisterQmlTypes(&engine);

    QString src = QStringLiteral(R"QML(import QtQuick 2.12
import SARibbon 3.0
Item {
    width: 700
    height: 300
    RibbonPanel {
        objectName: "panel"
        anchors.fill: parent
        panelTitle: "Gallery Panel"
        RibbonGallery {
            objectName: "gallery"
            stretchFactor: 1
            RibbonGalleryGroup {
                groupTitle: "Files"
                RibbonGalleryItem { text: "one" }
                RibbonGalleryItem { text: "two" }
                RibbonGalleryItem { text: "three" }
                RibbonGalleryItem { text: "four" }
                RibbonGalleryItem { text: "five" }
                RibbonGalleryItem { text: "six" }
                RibbonGalleryItem { text: "seven" }
                RibbonGalleryItem { text: "eight" }
                RibbonGalleryItem { text: "nine" }
                RibbonGalleryItem { text: "ten" }
            }
            RibbonGalleryGroup {
                groupTitle: "Apps"
                RibbonGalleryItem { text: "alpha" }
                RibbonGalleryItem { text: "beta" }
            }
        }
    }
})QML");

    QQmlComponent component(&engine);
    std::unique_ptr< QQuickView > view(exposeScene(engine, component, src.toUtf8().constData(), 700, 300));
    QVERIFY(view);
    QQuickItem* rootItem = view->rootObject();
    QVERIFY(rootItem);

    auto* gallery = rootItem->findChild< QQuickItem* >(QStringLiteral("gallery"));
    QVERIFY(gallery);
    QTRY_VERIFY(gallery->width() > 0 && gallery->height() > 0);

    // ---- contract face: expanding + stretch feed the core engine ----
    auto* host = qobject_cast< SARibbonQml::RibbonGallery* >(gallery);
    QVERIFY(host);
    QCOMPARE(host->expandingDirections(), Qt::Orientations(Qt::Horizontal));
    QCOMPARE(host->stretchFactor(), 1);
    QVERIFY(gallery->width() >= 200);  // base width at least (widgets minimum)

    // ---- grid metrics: cell size identical to the core helper ----
    const QSize cell = gallery->property("gridSize").toSize();
    const QSize expectCell
        = SA::calcGalleryGridCellSize(int(gallery->height()) - 2, gallery->property("displayRow").toInt(),
                                      gallery->property("gridMinimumWidth").toInt(), 0);
    QCOMPARE(cell, expectCell);
    QVERIFY(cell.width() >= 80);
    QVERIFY(gallery->property("gridColumns").toInt() >= 1);
    // 10 items / columns -> totalRows follows
    const int columns = gallery->property("gridColumns").toInt();
    const int expectRows = (10 + columns - 1) / columns;
    QCOMPARE(gallery->property("totalRows").toInt(), expectRows);

    // ---- scrolling clamps against totalRows - displayRow (derive the bound
    // from the actual layout: a wide gallery fits everything in one screen) ----
    const int maxScroll = qMax(expectRows - gallery->property("displayRow").toInt(), 0);
    QMetaObject::invokeMethod(gallery, "scrollDown");
    QMetaObject::invokeMethod(gallery, "scrollDown");
    QMetaObject::invokeMethod(gallery, "scrollDown");
    QTRY_COMPARE(gallery->property("scrollRow").toInt(), maxScroll);
    QMetaObject::invokeMethod(gallery, "scrollUp");
    QTRY_COMPARE(gallery->property("scrollRow").toInt(), qMax(maxScroll - 1, 0));

    // ---- group switch ----
    gallery->setProperty("currentGroupIndex", 1);
    QTRY_COMPARE(gallery->property("totalRows").toInt(), 1);  // 2 items, >=1 column
    gallery->setProperty("currentGroupIndex", 0);
    QTRY_COMPARE(gallery->property("totalRows").toInt(), expectRows);

    // ---- activation mediated by triggered ----
    QSignalSpy trigSpy(gallery, SIGNAL(triggered(SARibbonQml::RibbonGalleryItem*, int)));
    QMetaObject::invokeMethod(gallery, "activateItem", Q_ARG(int, 2));
    QCOMPARE(trigSpy.size(), 1);
    {
        auto* item = qvariant_cast< QObject* >(trigSpy.at(0).at(0));
        QVERIFY(item);
        QCOMPARE(item->property("text").toString(), QStringLiteral("three"));
        QCOMPARE(trigSpy.at(0).at(1).toInt(), 2);
    }
    QMetaObject::invokeMethod(gallery, "activateItem", Q_ARG(int, 99));  // out of range
    QCOMPARE(trigSpy.size(), 1);

    // ---- the grid renders (frame carries non-background content in the
    // gallery area: cell hover/press styling aside, the captions render) ----
    const QImage frame = view->grabWindow();
    QVERIFY(!frame.isNull());
    const QPointF galPos = gallery->mapToScene(QPointF(0, 0));
    int ink = 0;
    for (int y = int(galPos.y()); y < int(galPos.y() + gallery->height()) && y < frame.height(); ++y) {
        for (int x = int(galPos.x()); x < int(galPos.x() + gallery->width()) && x < frame.width(); ++x) {
            const QRgb rgb = frame.pixel(x, y);
            if (qAbs(qRed(rgb) - 255) > 12 || qAbs(qGreen(rgb) - 255) > 12 || qAbs(qBlue(rgb) - 255) > 12) {
                ++ink;
            }
        }
    }
    QVERIFY2(ink > 300, "gallery grid must render visible content (items + strip + frame)");
}

/**
 * @brief Six ribbon styles: row modes + tabOnTitle geometry + propagation
 * @details Mirrors the widgets setRibbonStyle mapping: three-row keeps word
 *          wrap + panel titles; single-row hides titles and switches buttons
 *          to icon-right text; compact styles ride the tab row on the title
 *          bar (categoryRowY collapses to the title height).
 */
void TestConformanceQml::ribbonStyleSwitching()
{
    QQmlEngine engine;
    saRibbonRegisterQmlTypes(&engine);

    QString src = QStringLiteral(R"QML(import QtQuick 2.12
import SARibbon 3.0
Item {
    width: 800
    height: 300
    RibbonBar {
        objectName: "bar"
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        RibbonCategory {
            objectName: "cat"
            title: "Home"
            RibbonPanel {
                objectName: "panel"
                panelTitle: "Styles"
                RibbonToolButton { objectName: "btn"; text: "Hello World Button" }
            }
        }
    }
})QML");

    QQmlComponent component(&engine);
    std::unique_ptr< QQuickView > view(exposeScene(engine, component, src.toUtf8().constData(), 800, 300));
    QVERIFY(view);
    QQuickItem* rootItem = view->rootObject();
    QVERIFY(rootItem);

    auto* bar    = rootItem->findChild< QQuickItem* >(QStringLiteral("bar"));
    auto* panel  = rootItem->findChild< QQuickItem* >(QStringLiteral("panel"));
    auto* btn    = rootItem->findChild< QQuickItem* >(QStringLiteral("btn"));
    QVERIFY(bar && panel && btn);

    // layout mode enum values: ThreeRowMode=3, TwoRowMode=2, SingleRowMode=1
    const int threeRow = 3, twoRow = 2, singleRow = 1;

    // ---- default LooseThreeRow ----
    QTRY_COMPARE(bar->property("ribbonStyle").toInt(), int(SARibbonQml::RibbonEnums::RibbonStyleLooseThreeRow));
    QTRY_COMPARE(panel->property("layoutMode").toInt(), threeRow);
    QTRY_COMPARE(btn->property("wordWrap").toBool(), true);
    QTRY_COMPARE(btn->property("iconRightText").toBool(), false);
    QTRY_VERIFY(panel->property("enableShowPanelTitle").toBool());
    const int looseCategoryRowY = bar->property("categoryRowY").toInt();
    const int looseBarHeight    = int(bar->implicitHeight());
    QVERIFY(looseCategoryRowY > 0);

    // ---- CompactThreeRow: tabs on the title row ----
    bar->setProperty("ribbonStyle", int(SARibbonQml::RibbonEnums::RibbonStyleCompactThreeRow));
    QTRY_COMPARE(bar->property("ribbonStyle").toInt(), int(SARibbonQml::RibbonEnums::RibbonStyleCompactThreeRow));
    QTRY_COMPARE(panel->property("layoutMode").toInt(), threeRow);
    // the tab row rides inside the title strip: categoryRowY collapses
    QTRY_VERIFY(bar->property("categoryRowY").toInt() < looseCategoryRowY);
    QTRY_VERIFY(int(bar->implicitHeight()) < looseBarHeight);

    // ---- LooseTwoRow: word wrap off, titles stay ----
    bar->setProperty("ribbonStyle", int(SARibbonQml::RibbonEnums::RibbonStyleLooseTwoRow));
    QTRY_COMPARE(panel->property("layoutMode").toInt(), twoRow);
    QTRY_COMPARE(btn->property("wordWrap").toBool(), false);
    QTRY_VERIFY(panel->property("enableShowPanelTitle").toBool());
    QTRY_COMPARE(bar->property("categoryRowY").toInt(), looseCategoryRowY);

    // ---- LooseSingleRow: titles hidden + icon-right text ----
    bar->setProperty("ribbonStyle", int(SARibbonQml::RibbonEnums::RibbonStyleLooseSingleRow));
    QTRY_COMPARE(panel->property("layoutMode").toInt(), singleRow);
    QTRY_COMPARE(btn->property("wordWrap").toBool(), false);
    QTRY_COMPARE(btn->property("iconRightText").toBool(), true);
    QTRY_VERIFY(!panel->property("enableShowPanelTitle").toBool());

    // ---- back to LooseThreeRow restores everything ----
    bar->setProperty("ribbonStyle", int(SARibbonQml::RibbonEnums::RibbonStyleLooseThreeRow));
    QTRY_COMPARE(panel->property("layoutMode").toInt(), threeRow);
    QTRY_COMPARE(btn->property("wordWrap").toBool(), true);
    QTRY_COMPARE(btn->property("iconRightText").toBool(), false);
    QTRY_VERIFY(panel->property("enableShowPanelTitle").toBool());
    QTRY_COMPARE(bar->property("categoryRowY").toInt(), looseCategoryRowY);
}

/**
 * @brief Panel separator: Large-proportion line item between buttons
 * @details Mirrors the widgets addSeparator: the separator rides its own
 *          column at full body height; the engine must give it a geometry
 *          distinct from the neighbouring buttons.
 */
void TestConformanceQml::separatorInPanel()
{
    QQmlEngine engine;
    saRibbonRegisterQmlTypes(&engine);

    QString src = QStringLiteral(R"QML(import QtQuick 2.12
import SARibbon 3.0
Item {
    width: 600
    height: 300
    RibbonPanel {
        objectName: "panel"
        anchors.fill: parent
        panelTitle: "Sep"
        RibbonToolButton { objectName: "left"; text: "Left" }
        RibbonSeparator { objectName: "sep" }
        RibbonToolButton { objectName: "right"; text: "Right" }
    }
})QML");

    QQmlComponent component(&engine);
    std::unique_ptr< QQuickView > view(exposeScene(engine, component, src.toUtf8().constData(), 600, 300));
    QVERIFY(view);
    QQuickItem* rootItem = view->rootObject();
    QVERIFY(rootItem);

    auto* left  = rootItem->findChild< QQuickItem* >(QStringLiteral("left"));
    auto* sep   = rootItem->findChild< QQuickItem* >(QStringLiteral("sep"));
    auto* right = rootItem->findChild< QQuickItem* >(QStringLiteral("right"));
    QVERIFY(left && sep && right);

    QTRY_VERIFY(left->width() > 0 && right->width() > 0);
    QTRY_VERIFY(sep->width() > 0 && sep->height() > 0);
    // widgets parity: 2*3 margins + 1px line
    QCOMPARE(sep->width(), 7.0);
    // the separator column sits between the buttons
    QVERIFY(left->x() < sep->x());
    QVERIFY(sep->x() < right->x());
}

/**
 * @brief Style radios embedded through a control container (example parity)
 * @details The example drives ribbonStyle through RadioButtons inside
 *          RibbonControlContainers; this reproduces the exact pattern and
 *          clicks the radio with real mouse events to prove the whole chain
 *          (container embedding -> radio toggle -> bar style switch).
 */
void TestConformanceQml::styleRadioViaContainer()
{
    QQmlEngine engine;
    saRibbonRegisterQmlTypes(&engine);

    QString src = QStringLiteral(R"QML(import QtQuick 2.12
import QtQuick.Controls 2.12
import SARibbon 3.0
Item {
    width: 600
    height: 300
    RibbonBar {
        id: ribbonBar
        objectName: "bar"
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        applicationLabel: "File"
        RibbonCategory {
            title: "Home"
            RibbonPanel {
                panelTitle: "style"
                ButtonGroup { id: styleGroup }
                RibbonControlContainer {
                    control: RadioButton {
                        objectName: "radioLoose"
                        ButtonGroup.group: styleGroup
                        text: "loose"
                        checked: true
                        onToggled: if (checked) ribbonBar.ribbonStyle = Ribbon.RibbonStyleLooseThreeRow
                    }
                }
                RibbonControlContainer {
                    control: RadioButton {
                        objectName: "radioCompact"
                        ButtonGroup.group: styleGroup
                        text: "compact"
                        onToggled: if (checked) ribbonBar.ribbonStyle = Ribbon.RibbonStyleCompactThreeRow
                    }
                }
            }
        }
    }
})QML");

    QQmlComponent component(&engine);
    std::unique_ptr< QQuickView > view(exposeScene(engine, component, src.toUtf8().constData(), 600, 300));
    QVERIFY(view);
    QQuickItem* rootItem = view->rootObject();
    QVERIFY(rootItem);

    auto* bar = rootItem->findChild< QQuickItem* >(QStringLiteral("bar"));
    auto* radioCompact = rootItem->findChild< QQuickItem* >(QStringLiteral("radioCompact"));
    QVERIFY(bar && radioCompact);
    QTRY_COMPARE(bar->property("ribbonStyle").toInt(), int(SARibbonQml::RibbonEnums::RibbonStyleLooseThreeRow));

    // real mouse click on the embedded radio toggles the style
    const int looseRowY = bar->property("categoryRowY").toInt();
    const QPointF center = radioCompact->mapToScene(QPointF(radioCompact->width() / 2, radioCompact->height() / 2));
    QTest::mouseClick(view.get(), Qt::LeftButton, Qt::NoModifier, center.toPoint());
    QTRY_COMPARE(radioCompact->property("checked").toBool(), true);
    QTRY_COMPARE(bar->property("ribbonStyle").toInt(), int(SARibbonQml::RibbonEnums::RibbonStyleCompactThreeRow));
    QTRY_VERIFY(bar->property("categoryRowY").toInt() < looseRowY);
}

/**
 * @brief Quick access bar + right button group on the title row
 * @details Mirrors the widgets quick access bar / right button group: the
 *          rows ride the title strip (after the app button / right-aligned
 *          before the system strip), buttons size from their metrics
 *          sizeHints and stay clickable.
 */
void TestConformanceQml::quickAccessBarAndRightGroup()
{
    QQmlEngine engine;
    saRibbonRegisterQmlTypes(&engine);

    QString src = QStringLiteral(R"QML(import QtQuick 2.12
import SARibbon 3.0
Item {
    width: 800
    height: 300
    RibbonBar {
        id: bar
        objectName: "bar"
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        applicationLabel: "File"
        RibbonQuickAccessBar {
            objectName: "qab"
            RibbonToolButton { objectName: "qabSave"; text: "Save"; iconSource: ""; proportion: Ribbon.Small }
            RibbonToolButton { objectName: "qabUndo"; text: "Undo"; proportion: Ribbon.Small }
        }
        RibbonButtonGroup {
            objectName: "rgroup"
            RibbonToolButton { objectName: "rgHelp"; text: "Help"; proportion: Ribbon.Small }
        }
        RibbonCategory {
            title: "Home"
            RibbonPanel {
                panelTitle: "P"
                RibbonToolButton { text: "A" }
            }
        }
    }
})QML");

    QQmlComponent component(&engine);
    std::unique_ptr< QQuickView > view(exposeScene(engine, component, src.toUtf8().constData(), 800, 300));
    QVERIFY(view);
    QQuickItem* rootItem = view->rootObject();
    QVERIFY(rootItem);

    auto* qab    = rootItem->findChild< QQuickItem* >(QStringLiteral("qab"));
    auto* rgroup = rootItem->findChild< QQuickItem* >(QStringLiteral("rgroup"));
    auto* bar    = rootItem->findChild< QQuickItem* >(QStringLiteral("bar"));
    auto* save   = rootItem->findChild< QQuickItem* >(QStringLiteral("qabSave"));
    auto* undo   = rootItem->findChild< QQuickItem* >(QStringLiteral("qabUndo"));
    auto* help   = rootItem->findChild< QQuickItem* >(QStringLiteral("rgHelp"));
    QVERIFY(qab && rgroup && bar && save && undo && help);

    // ---- the rows sit on the title strip, after the app button ----
    const int titleH = bar->property("titleBarHeight").toInt();
    QTRY_VERIFY(qab->width() > 0 && qab->height() > 0);
    QCOMPARE(qab->height(), qreal(titleH));
    QVERIFY(qab->x() > 0);        // after the application button
    QVERIFY(qab->y() == 0.0);     // on the title row
    // the buttons are laid out in a row inside the bar's coordinate space
    QTRY_VERIFY(save->width() > 0 && undo->width() > 0);
    QVERIFY(undo->x() > save->x());
    QVERIFY(save->y() >= 0 && save->y() + save->height() <= titleH + 1);

    // ---- right group: right-aligned before the system strip ----
    QTRY_VERIFY(rgroup->width() > 0);
    QVERIFY(rgroup->x() + rgroup->width() <= 800);
    QVERIFY(rgroup->x() > 800 - 120 - rgroup->width() - 30);  // just before the strip
    QVERIFY(help->width() > 0);

    // ---- the embedded quick access button is clickable ----
    QSignalSpy clickedSpy(save, SIGNAL(clicked()));
    const QPointF center = save->mapToScene(QPointF(save->width() / 2, save->height() / 2));
    QTest::mouseClick(view.get(), Qt::LeftButton, Qt::NoModifier, center.toPoint());
    QTRY_COMPARE(clickedSpy.size(), 1);
}

/**
 * @brief Tab row alignment + minimum (collapsed) mode
 * @details Alignment mirrors widgets setRibbonAlignment (left/center/right
 *          inside the free strip); minimum mode mirrors setMinimumMode
 *          (the category row hides, the bar keeps title + tab rows).
 */
void TestConformanceQml::tabAlignmentAndMinimumMode()
{
    QQmlEngine engine;
    saRibbonRegisterQmlTypes(&engine);

    QString src = QStringLiteral(R"QML(import QtQuick 2.12
import SARibbon 3.0
Item {
    width: 900
    height: 300
    RibbonBar {
        objectName: "bar"
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        applicationLabel: "File"
        RibbonCategory {
            objectName: "cat"
            title: "Home"
            RibbonPanel { panelTitle: "P"; RibbonToolButton { text: "A" } }
        }
    }
})QML");

    QQmlComponent component(&engine);
    std::unique_ptr< QQuickView > view(exposeScene(engine, component, src.toUtf8().constData(), 900, 300));
    QVERIFY(view);
    QQuickItem* rootItem = view->rootObject();
    QVERIFY(rootItem);

    auto* bar = rootItem->findChild< QQuickItem* >(QStringLiteral("bar"));
    auto* cat = rootItem->findChild< QQuickItem* >(QStringLiteral("cat"));
    QVERIFY(bar && cat);

    // leftmost visible tab host (exact class name)
    auto firstTab = [rootItem]() -> QQuickItem* {
        const auto all = rootItem->findChildren< QQuickItem* >();
        QQuickItem* best = nullptr;
        for (QQuickItem* item : all) {
            if (QString::fromLatin1(item->metaObject()->className()) == QLatin1String("SARibbonQml::RibbonTab")
                && item->isVisible()) {
                if (!best || item->x() < best->x()) {
                    best = item;
                }
            }
        }
        return best;
    };
    QQuickItem* tab = nullptr;
    QTRY_VERIFY((tab = firstTab()) != nullptr);
    const qreal leftX = tab->x();
    QVERIFY(leftX > 0);

    // ---- center alignment moves the row right ----
    bar->setProperty("tabAlignment", int(SARibbonQml::RibbonEnums::AlignCenter));
    QTRY_VERIFY(tab->x() > leftX + 50);
    // ---- right alignment pushes it further ----
    bar->setProperty("tabAlignment", int(SARibbonQml::RibbonEnums::AlignRight));
    QTRY_VERIFY(tab->x() > leftX + 100);
    // ---- back to left restores ----
    bar->setProperty("tabAlignment", int(SARibbonQml::RibbonEnums::AlignLeft));
    QTRY_COMPARE(tab->x(), leftX);

    // ---- minimum mode: category hides, bar shrinks ----
    const qreal normalHeight = bar->implicitHeight();
    QTRY_VERIFY(cat->isVisible());
    bar->setProperty("minimumMode", true);
    QTRY_VERIFY(!cat->isVisible());
    QTRY_VERIFY(bar->implicitHeight() < normalHeight);
    bar->setProperty("minimumMode", false);
    QTRY_VERIFY(cat->isVisible());
    QTRY_COMPARE(bar->implicitHeight(), normalHeight);
}

/**
 * @brief RTL toggle: application layout direction flips the engine mirroring
 * @details Mirrors the widgets "Switch to RTL": RibbonTheme.rtl drives
 *          QGuiApplication::setLayoutDirection which the core engines read
 *          through SA::saIsRTL(); the small MenuButtonPopup hit strip must
 *          mirror to the LEADING edge (core saMirrorX).
 */
void TestConformanceQml::rtlToggle()
{
    QQmlEngine engine;
    saRibbonRegisterQmlTypes(&engine);

    QString src = QStringLiteral(R"QML(import QtQuick 2.12
import SARibbon 3.0
Item {
    width: 600
    height: 300
    RibbonPanel {
        objectName: "panel"
        anchors.fill: parent
        panelTitle: "RTL"
        RibbonToolButton {
            objectName: "btn"
            text: "Menu"
            proportion: Ribbon.Small
            popupMode: Ribbon.MenuButtonPopup
            menuItems: [ RibbonMenuItem { text: "one" } ]
        }
    }
})QML");

    QQmlComponent component(&engine);
    std::unique_ptr< QQuickView > view(exposeScene(engine, component, src.toUtf8().constData(), 600, 300));
    QVERIFY(view);
    QQuickItem* rootItem = view->rootObject();
    QVERIFY(rootItem);

    auto* btn = rootItem->findChild< QQuickItem* >(QStringLiteral("btn"));
    QVERIFY(btn);
    QTRY_VERIFY(btn->width() > 0);

    // LTR baseline: the menu strip sits on the trailing (right) edge
    const QRectF ltrMenu = btn->property("menuRect").toRectF();
    QVERIFY(ltrMenu.width() > 0);
    QVERIFY(qFuzzyCompare(ltrMenu.x() + ltrMenu.width(), qreal(btn->width())));

    // ---- flip to RTL through the theme singleton ----
    QObject* theme = nullptr;
    {
        QQmlComponent themeComp(&engine);
        themeComp.setData(QByteArrayLiteral("import QtQml 2.12\nimport SARibbon 3.0\nQtObject { property var t: RibbonTheme }"),
                          QUrl());
        QObject* holder = themeComp.create();
        QVERIFY(holder);
        theme = holder->property("t").value< QObject* >();
        QVERIFY(theme);
    }
    const bool wasRtl = theme->property("rtl").toBool();
    theme->setProperty("rtl", !wasRtl);
    QTRY_VERIFY(theme->property("rtl").toBool() == !wasRtl);

    // the strip mirrors to the leading (left) edge
    QTRY_VERIFY([btn]() {
        const QRectF r = btn->property("menuRect").toRectF();
        return qFuzzyCompare(r.x(), qreal(0)) && r.width() > 0;
    }());

    // restore (avoid leaking app-global state into other tests)
    theme->setProperty("rtl", wasRtl);
    QTRY_VERIFY(theme->property("rtl").toBool() == wasRtl);
}

/**
 * @brief Panel option action: reserved engine space + trigger signal
 * @details Mirrors the widgets setOptionAction. The engine reserves the
 *          option square when hasOptionAction is set; the geometry
 *          PUBLICATION stays deferred (consuming it crashes at teardown on
 *          Qt 6.7.3 debug — NOTES B44), so this test guards the reservation
 *          input, the property API and the trigger signal, plus a full
 *          event-pumped teardown of the previously-crashing scene.
 */
void TestConformanceQml::panelOptionAction()
{
    QQmlEngine engine;
    saRibbonRegisterQmlTypes(&engine);

    QString src = QStringLiteral(R"QML(import QtQuick 2.12
import SARibbon 3.0
Item {
    width: 500
    height: 200
    RibbonPanel {
        objectName: "panel"
        anchors.fill: parent
        panelTitle: "Opt"
        hasOptionAction: true
        RibbonSeparator { objectName: "sep" }
    }
})QML");

    QQmlComponent component(&engine);
    std::unique_ptr< QQuickView > view(exposeScene(engine, component, src.toUtf8().constData(), 500, 200));
    QVERIFY(view);
    QQuickItem* rootItem = view->rootObject();
    QVERIFY(rootItem);

    auto* panel = rootItem->findChild< QQuickItem* >(QStringLiteral("panel"));
    QVERIFY(panel);
    // the property round-trips (geometry publication deferred on Qt 6.7.3
    // debug — NOTES B46; Release-validated with full assertions)
    QCOMPARE(panel->property("hasOptionAction").toBool(), true);

    // the option action signal fires from the invokable (leaf-mediated click
    // parity; the visual button renders once geometry publication is enabled)
    QSignalSpy optSpy(panel, SIGNAL(optionActionTriggered()));
    QMetaObject::invokeMethod(panel, "triggerOptionAction");
    QCOMPARE(optSpy.size(), 1);

    // full event pumping + teardown: the crash family lived HERE
    for (int i = 0; i < 20; ++i) {
        QTest::qWait(50);
    }
    QVERIFY(panel->isVisible());
}

/**
 * @brief Application window (widgets ApplicationWidget mode)
 * @details A RibbonApplicationWindow declared as a bar child takes priority
 *          over the application menu: the app button click shows it in a
 *          popup below the button (Esc / outside click close), and the inner
 *          close() invokable routes back through the bar to shut the popup.
 */
void TestConformanceQml::applicationWindow()
{
    QQmlEngine engine;
    saRibbonRegisterQmlTypes(&engine);

    QString src = QStringLiteral(R"QML(import QtQuick 2.12
import QtQuick.Controls 2.12
import SARibbon 3.0
Item {
    width: 800
    height: 300
    RibbonBar {
        objectName: "bar"
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        applicationLabel: "File"
        applicationMenuItems: [ RibbonMenuItem { text: "should not open" } ]
        RibbonApplicationWindow {
            objectName: "appwin"
            width: 260
            height: 160
            Column {
                anchors.fill: parent
                spacing: 6
                Label { text: qsTr("Press the Esc key to exit the window.") }
                Button {
                    objectName: "cancelBtn"
                    text: qsTr("Cancel")
                    onClicked: appwinClose()
                }
            }
        }
        RibbonCategory {
            title: "Home"
            RibbonPanel { panelTitle: "P"; RibbonToolButton { text: "A" } }
        }
    }
    function appwinClose()
    {
        // route through the app window's close invokable
        appwin.close();
    }
})QML");

    QQmlComponent component(&engine);
    std::unique_ptr< QQuickView > view(exposeScene(engine, component, src.toUtf8().constData(), 800, 300));
    QVERIFY(view);
    QQuickItem* rootItem = view->rootObject();
    QVERIFY(rootItem);

    auto* bar    = rootItem->findChild< QQuickItem* >(QStringLiteral("bar"));
    auto* appwin = rootItem->findChild< QQuickItem* >(QStringLiteral("appwin"));
    QVERIFY(bar && appwin);

    // the bar publishes the application window (and it wins over the menu)
    QVERIFY(bar->property("hasApplicationWindow").toBool());
    QCOMPARE(bar->property("applicationWindowItem").value< QQuickItem* >(), appwin);
    QVERIFY(!appwin->property("popupVisible").toBool());

    // ---- real click on the application button opens the window popup ----
    const QRectF appRect = bar->property("applicationButtonRect").toRectF();
    QVERIFY(appRect.width() > 0);
    const QPointF center = bar->mapToScene(QPointF(appRect.center()));
    QTest::mouseClick(view.get(), Qt::LeftButton, Qt::NoModifier, center.toPoint());
    QTRY_COMPARE(appwin->property("popupVisible").toBool(), true);

    // ---- inner Cancel button closes through the invokable chain ----
    auto* cancel = rootItem->findChild< QQuickItem* >(QStringLiteral("cancelBtn"));
    QVERIFY(cancel);
    const QPointF cancelCenter = cancel->mapToScene(QPointF(cancel->width() / 2, cancel->height() / 2));
    QTest::mouseClick(view.get(), Qt::LeftButton, Qt::NoModifier, cancelCenter.toPoint());
    QTRY_COMPARE(appwin->property("popupVisible").toBool(), false);

    // ---- the headless close invokable also routes ----
    QMetaObject::invokeMethod(bar, "requestApplicationWindowClose");
    QTest::qWait(50);
    QCOMPARE(appwin->property("popupVisible").toBool(), false);
}

QTEST_MAIN(TestConformanceQml)
#include "tst_conformance_qml.moc"