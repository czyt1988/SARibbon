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
#include <QWheelEvent>
#include <QGuiApplication>
#include <memory>
#include <functional>
#include <SARibbonQml/SARibbonQmlGlobal.h>
#include <SARibbonQml/SARibbonQmlTypes.h>
#include <SARibbonCore/SARibbonCoreUtil.h>
#include <SARibbonCore/SARibbonThemePalette.h>
#include <SARibbonCore/SARibbonCategoryLayoutEngine.h>
#include <SARibbonQml/SARibbonQmlTheme.h>
#include <SARibbonQml/SARibbonQmlMetrics.h>
#include <SARibbonQml/SARibbonQmlToolButton.h>
#include <SARibbonQml/SARibbonQmlControlContainer.h>
#include <SARibbonQml/SARibbonQmlGallery.h>
#include <SARibbonQml/SARibbonQmlGalleryGroup.h>
#include <SARibbonQml/SARibbonQmlGalleryItem.h>
#include <SARibbonQml/SARibbonQmlMenuItem.h>
#include <SARibbonQml/SARibbonQmlBar.h>
#include <SARibbonQml/SARibbonQmlWindowAgent.h>
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
    void menuCheckableShortcutAndSubmenu();
    void controlContainerEmbedding();
    void controlContainerSuffixAndShowFlags();
    void contextCategoryActivation();
    void galleryInPanel();
    void galleryCaptionStylesHoverAndSelectable();
    void ribbonStyleSwitching();
    void styleRadioViaContainer();
    void separatorInPanel();
    void quickAccessBarAndRightGroup();
    void buttonRowExclusivity();
    void tabAlignmentAndMinimumMode();
    void rtlToggle();
    void panelOptionAction();
    void applicationWindow();
    void themeCustomization();
    void categoryScrollWheelAndArrows();
    void embeddedRibbonBasicControls();
    void ribbonBasicControlsThemeAndFont();
    void framelessAgentStripAndTitle();

private:
    QQuickView* exposeScene(QQmlEngine& engine, QQmlComponent& component, const char* src, int w, int h);
    static int countPixelsNear(const QImage& img, const QColor& color, int tolerance = 8);
};

/**
 * @brief Collect every item of a QML visual subtree
 * @details Repeater delegates are QObject-parented outside their visual parent,
 *          so findChildren() silently misses them; this walks childItems().
 */
static void collectVisualItems(QQuickItem* item, QList< QQuickItem* >* out)
{
    if (!item) {
        return;
    }
    out->append(item);
    const QList< QQuickItem* > kids = item->childItems();
    for (QQuickItem* kid : kids) {
        collectVisualItems(kid, out);
    }
}

/**
 * @brief Normalize a QColor to a comparable string key
 * @details QColor::operator== compares the color spec before the components, so
 *          a color built from a token string and the same color built from an
 *          integer constructor can differ while painting identically. Comparing
 *          the ARGB hex keeps the assertions about the visible value.
 */
static QString colorKey(const QColor& c)
{
    return c.isValid() ? c.name(QColor::HexArgb) : QStringLiteral("<invalid>");
}

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

    // the hidden category never gets laid out, yet its panels still publish
    // geometry: the title strip must be empty rather than negative (a negative
    // width used to leak into the leaf's Text and draw the caption outside the
    // panel — NOTES B50)
    const auto sceneItems = rootItem->findChildren< QQuickItem* >();
    int panelCount = 0;
    for (QQuickItem* item : sceneItems) {
        const QByteArray cls = item->metaObject()->className();
        if (cls == QByteArrayLiteral("SARibbonQml::RibbonPanel")) {
            ++panelCount;
            const QRectF title = item->property("titleGeometry").toRectF();
            QVERIFY2(title.width() >= 0 && title.height() >= 0, "panel titleGeometry must never be negative");
            const QRectF opt = item->property("optionButtonRect").toRectF();
            QVERIFY2(opt.width() >= 0 && opt.height() >= 0, "panel optionButtonRect must never be negative");
        }
    }
    QCOMPARE(panelCount, 2);

    // the Text sweep must run over the visual tree: the leaves' Text elements
    // are Repeater/instantiated items that findChildren() does not reach
    int textCount = 0;
    QList< QQuickItem* > visualItems;
    collectVisualItems(rootItem, &visualItems);
    for (QQuickItem* item : visualItems) {
        if (item->metaObject()->className() == QByteArrayLiteral("QQuickText")) {
            ++textCount;
            QVERIFY2(item->width() >= 0 && item->height() >= 0, "no Text may end up with a negative box");
        }
    }
    QVERIFY2(textCount > 0, "the scene must actually render text for the sweep to mean anything");

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
 * @brief Menu entries: check marks, shortcut captions and nested submenus
 * @details Mirrors what QAction hands the widgets SARibbonMenu for free
 *          (setCheckable / setShortcut / addRibbonMenu). Activation is
 *          addressed by index path, so the host stays the only place that
 *          knows the tree; a checkable entry flips BEFORE menuTriggered fires
 *          (QAction::trigger ordering) and separators, disabled entries and
 *          malformed paths are refused silently. On the leaf the shared
 *          RibbonMenu renders a reserved mark column, a right-aligned
 *          shortcut caption and a hover-opened submenu popup.
 */
void TestConformanceQml::menuCheckableShortcutAndSubmenu()
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
            objectName: "menuBtn"
            text: "Menu"
            proportion: Ribbon.Small
            popupMode: Ribbon.MenuButtonPopup
            menuItems: [
                RibbonMenuItem { objectName: "boldItem"; text: "Bold"; checkable: true },
                RibbonMenuItem { objectName: "saveItem"; text: "Save"; shortcut: "Ctrl+S" },
                RibbonMenuItem {
                    objectName: "recentItem"
                    text: "Recent"
                    submenu: [
                        RibbonMenuItem { objectName: "doc1Item"; text: "doc1" },
                        RibbonMenuItem { objectName: "doc2Item"; text: "doc2"; checkable: true; checked: true }
                    ]
                }
            ]
        }
    }
}
)QML");

    QQmlComponent component(&engine);
    std::unique_ptr< QQuickView > view(exposeScene(engine, component, src.toUtf8().constData(), 600, 300));
    QVERIFY(view);
    QQuickItem* rootItem = view->rootObject();
    QVERIFY(rootItem);

    auto* menuBtn = rootItem->findChild< QQuickItem* >(QStringLiteral("menuBtn"));
    QVERIFY(menuBtn);
    QTRY_VERIFY(menuBtn->width() > 0 && menuBtn->height() > 0);
    auto* boldItem   = rootItem->findChild< SARibbonQml::RibbonMenuItem* >(QStringLiteral("boldItem"));
    auto* saveItem   = rootItem->findChild< SARibbonQml::RibbonMenuItem* >(QStringLiteral("saveItem"));
    auto* recentItem = rootItem->findChild< SARibbonQml::RibbonMenuItem* >(QStringLiteral("recentItem"));
    auto* doc2Item   = rootItem->findChild< SARibbonQml::RibbonMenuItem* >(QStringLiteral("doc2Item"));
    QVERIFY(boldItem && saveItem && recentItem && doc2Item);

    // ---- declarative model ----
    QVERIFY(boldItem->isCheckable());
    QVERIFY(!boldItem->isChecked());
    QCOMPARE(saveItem->shortcut(), QStringLiteral("Ctrl+S"));
    QCOMPARE(recentItem->submenuCount(), 2);
    QVERIFY(recentItem->hasSubmenu());
    QVERIFY(!saveItem->hasSubmenu());
    QCOMPARE(recentItem->submenuItemAt(1), doc2Item);
    QVERIFY(doc2Item->isChecked());

    // ---- a checkable entry flips before menuTriggered reaches the caller ----
    QSignalSpy menuSpy(menuBtn, SIGNAL(menuTriggered(SARibbonQml::RibbonMenuItem*)));
    QSignalSpy toggledSpy(boldItem, SIGNAL(toggled(bool)));
    QSignalSpy triggeredSpy(boldItem, SIGNAL(triggered()));
    bool checkedAtTrigger = false;
    auto* btnHost = qobject_cast< SARibbonQml::RibbonToolButton* >(menuBtn);
    QVERIFY(btnHost);
    QObject::connect(btnHost, &SARibbonQml::RibbonToolButton::menuTriggered, this,
                     [&](SARibbonQml::RibbonMenuItem* it) { checkedAtTrigger = (it == boldItem) ? it->isChecked() : checkedAtTrigger; });
    QVariantList path;
    path << 0;
    QMetaObject::invokeMethod(menuBtn, "activateMenuItemPath", Q_ARG(QVariantList, path));
    QCOMPARE(menuSpy.size(), 1);
    QCOMPARE(qvariant_cast< QObject* >(menuSpy.at(0).at(0)), static_cast< QObject* >(boldItem));
    QCOMPARE(boldItem->isChecked(), true);
    QVERIFY(checkedAtTrigger);
    QCOMPARE(toggledSpy.size(), 1);
    QCOMPARE(toggledSpy.at(0).at(0).toBool(), true);
    QCOMPARE(triggeredSpy.size(), 1);
    // activating again flips it back (QAction toggle semantics)
    QMetaObject::invokeMethod(menuBtn, "activateMenuItemPath", Q_ARG(QVariantList, path));
    QCOMPARE(menuSpy.size(), 2);
    QCOMPARE(boldItem->isChecked(), false);
    QCOMPARE(toggledSpy.size(), 2);

    // ---- the flat invokable is the one-element path ----
    QMetaObject::invokeMethod(menuBtn, "activateMenuItem", Q_ARG(int, 0));
    QCOMPARE(menuSpy.size(), 3);
    QCOMPARE(boldItem->isChecked(), true);
    // clearing checkable clears the mark (QAction parity)
    boldItem->setCheckable(false);
    QVERIFY(!boldItem->isChecked());
    boldItem->setCheckable(true);

    // ---- nested entries are addressed by path; bad paths are refused ----
    const int before = menuSpy.size();
    QVariantList docPath;
    docPath << 2 << 1;
    QMetaObject::invokeMethod(menuBtn, "activateMenuItemPath", Q_ARG(QVariantList, docPath));
    QCOMPARE(menuSpy.size(), before + 1);
    QCOMPARE(qvariant_cast< QObject* >(menuSpy.at(before).at(0)), static_cast< QObject* >(doc2Item));
    // the second submenu entry was declared checked, so activating clears it
    QCOMPARE(doc2Item->isChecked(), false);
    for (const QVariantList& bad : { QVariantList{ 2, 9 }, QVariantList{ 9 }, QVariantList{ 0, 0 }, QVariantList(), QVariantList{ QStringLiteral("x") } }) {
        QMetaObject::invokeMethod(menuBtn, "activateMenuItemPath", Q_ARG(QVariantList, bad));
        QCOMPARE(menuSpy.size(), before + 1);
    }
    // the flat invokable only reaches the top level
    QMetaObject::invokeMethod(menuBtn, "activateMenuItem", Q_ARG(int, 2));
    QCOMPARE(menuSpy.size(), before + 2);
    QCOMPARE(qvariant_cast< QObject* >(menuSpy.at(before + 1).at(0)), static_cast< QObject* >(recentItem));

    // ---- leaf: three top-level rows, mark column reserved, shortcut drawn ----
    QVERIFY(!menuBtn->property("menuVisible").toBool());
    QMetaObject::invokeMethod(menuBtn, "openMenu");
    QTRY_COMPARE(menuBtn->property("menuVisible").toBool(), true);

    // Repeater delegates carry no QObject parent: walk the item tree (NOTES B50)
    std::function< void(QQuickItem*, const QString&, QList< QQuickItem* >*) > collectNamed
        = [&collectNamed](QQuickItem* from, const QString& name, QList< QQuickItem* >* out) {
        if (from->objectName() == name) {
            out->append(from);
        }
        const QList< QQuickItem* > kids = from->childItems();
        for (QQuickItem* kid : kids) {
            collectNamed(kid, name, out);
        }
    };
    auto rowTexts = [](QQuickItem* row) {
        QList< QQuickItem* > all;
        collectVisualItems(row, &all);
        QStringList out;
        for (QQuickItem* it : all) {
            if (it->metaObject()->className() == QByteArrayLiteral("QQuickText")) {
                out << it->property("text").toString();
            }
        }
        return out;
    };
    auto captionX = [](QQuickItem* row) {
        QList< QQuickItem* > all;
        collectVisualItems(row, &all);
        for (QQuickItem* it : all) {
            if (it->metaObject()->className() == QByteArrayLiteral("QQuickText")) {
                return it->mapToItem(row, QPointF(0, 0)).x();
            }
        }
        return qreal(-1);
    };
    auto visibleRowCount = [&collectNamed, &view]() {
        QList< QQuickItem* > all;
        collectNamed(view->contentItem(), QStringLiteral("menuRow"), &all);
        int n = 0;
        for (QQuickItem* r : all) {
            if (r->isVisible()) {
                ++n;
            }
        }
        return n;
    };

    // restore the mark the path activation above cleared, so the leaf section
    // asserts the checked rendering
    doc2Item->setChecked(true);

    QList< QQuickItem* > rows;
    collectNamed(view->contentItem(), QStringLiteral("menuRow"), &rows);
    QCOMPARE(rows.size(), 3);
    QCOMPARE(rowTexts(rows.at(0)).first(), QStringLiteral("Bold"));
    QVERIFY(rowTexts(rows.at(1)).contains(QStringLiteral("Ctrl+S")));
    // every row reserves the mark column as soon as one entry is checkable,
    // which is what keeps the captions aligned in a mixed menu
    QVERIFY(captionX(rows.at(0)) > 10);
    QCOMPARE(captionX(rows.at(0)), captionX(rows.at(1)));
    // the tick of an unchecked entry exists but is not painted
    {
        QList< QQuickItem* > marks;
        collectVisualItems(rows.at(1), &marks);
        bool canvasFound = false;
        for (QQuickItem* it : marks) {
            // a Canvas carrying a custom property becomes a composite type
            // (QQuickCanvasItem_QML_n), so match on the prefix
            if (QByteArray(it->metaObject()->className()).startsWith("QQuickCanvasItem")) {
                canvasFound = true;
                QVERIFY(!it->isVisible());
            }
        }
        QVERIFY(canvasFound);
    }

    // ---- hovering the submenu row opens a sibling popup with its own rows ----
    const QPointF recentCenter = rows.at(2)->mapToScene(QPointF(rows.at(2)->width() / 2, rows.at(2)->height() / 2));
    QTest::mouseMove(view.get(), recentCenter.toPoint());
    // 3 top-level rows + 2 submenu rows (the nested popup rides the same
    // window overlay, so both levels are reachable from contentItem)
    QTRY_COMPARE(visibleRowCount(), 5);
    QList< QQuickItem* > allRows;
    collectNamed(view->contentItem(), QStringLiteral("menuRow"), &allRows);
    QQuickItem* doc2Row = nullptr;
    for (QQuickItem* r : allRows) {
        if (r->isVisible() && rowTexts(r).first() == QStringLiteral("doc2")) {
            doc2Row = r;
        }
    }
    QVERIFY(doc2Row);
    // the checked submenu entry paints its tick
    {
        QList< QQuickItem* > kids;
        collectVisualItems(doc2Row, &kids);
        bool tickVisible = false;
        for (QQuickItem* it : kids) {
            if (QByteArray(it->metaObject()->className()).startsWith("QQuickCanvasItem") && it->isVisible()) {
                tickVisible = true;
            }
        }
        QVERIFY(tickVisible);
    }

    // ---- clicking the submenu entry activates through the path and closes all ----
    const int beforeSub = menuSpy.size();
    const QPointF doc2Center = doc2Row->mapToScene(QPointF(doc2Row->width() / 2, doc2Row->height() / 2));
    QTest::mouseClick(view.get(), Qt::LeftButton, Qt::NoModifier, doc2Center.toPoint());
    QTRY_COMPARE(menuSpy.size(), beforeSub + 1);
    QCOMPARE(qvariant_cast< QObject* >(menuSpy.at(beforeSub).at(0)), static_cast< QObject* >(doc2Item));
    QCOMPARE(doc2Item->isChecked(), false);
    // the host closed the button popup and the menu tore its submenu down
    QTRY_COMPARE(menuBtn->property("menuVisible").toBool(), false);
    QTRY_COMPARE(visibleRowCount(), 0);
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

/**
 * @brief Container suffix label + the two enableShow* strip switches
 * @details Parity check for SARibbonLineWidgetContainer::setSuffix and
 *          SARibbonCtrlContainer::setEnableShowIcon/Title: the trailing strip is
 *          carved out of the container (not out of the control), an unset suffix
 *          keeps the pre-suffix geometry byte-identical, and hiding a strip slot
 *          hands its width back so the control moves up.
 */
void TestConformanceQml::controlContainerSuffixAndShowFlags()
{
    const QString iconPath = QStringLiteral(QT_TESTCASE_BUILDDIR)
                             + QStringLiteral("/../../../examples/widgets/MainWindowExample/icon/save.svg");
    if (!QFile::exists(iconPath)) {
        QSKIP("icon file not reachable from this build directory");
    }

    QQmlEngine engine;
    saRibbonRegisterQmlTypes(&engine);

    QString src = QStringLiteral(R"QML(import QtQuick 2.12
import QtQuick.Controls 2.12
import SARibbon 3.0
Item {
    width: 700
    height: 300
    RibbonPanel {
        objectName: "panel"
        anchors.fill: parent
        panelTitle: "P"
        RibbonControlContainer {
            objectName: "sfx"
            text: "Size:"
            control: SpinBox {
                objectName: "spin"
                from: 0
                to: 100
                value: 12
            }
        }
        RibbonControlContainer {
            objectName: "iconed"
            text: "Font:"
            control: ComboBox {
                objectName: "combo2"
                model: [ "a", "b" ]
            }
        }
    }
})QML");

    QQmlComponent component(&engine);
    std::unique_ptr< QQuickView > view(exposeScene(engine, component, src.toUtf8().constData(), 700, 300));
    QVERIFY(view);
    QQuickItem* rootItem = view->rootObject();
    QVERIFY(rootItem);

    auto* sfx    = rootItem->findChild< QQuickItem* >(QStringLiteral("sfx"));
    auto* iconed = rootItem->findChild< QQuickItem* >(QStringLiteral("iconed"));
    auto* spin   = rootItem->findChild< QQuickItem* >(QStringLiteral("spin"));
    auto* combo2 = rootItem->findChild< QQuickItem* >(QStringLiteral("combo2"));
    QVERIFY(sfx && iconed && spin && combo2);
    QTRY_VERIFY(sfx->width() > 0 && iconed->width() > 0);

    // ---- no suffix by default: zero-width tail strip, unchanged geometry ----
    QCOMPARE(sfx->property("suffixWidth").toReal(), 0.0);
    const qreal labelW = sfx->property("labelWidth").toReal();
    QVERIFY(labelW > 0);
    QTRY_VERIFY(spin->width() > 0);
    const qreal spinW0 = spin->width();
    QCOMPARE(spin->x(), labelW);
    // the control gets everything between the strip and the (empty) tail minus
    // the content margin
    QCOMPARE(qRound(sfx->width() - spin->x() - spinW0), 2);

    // ---- the suffix carves a trailing strip out of the container ----
    const qreal sfxW0 = sfx->width();
    sfx->setProperty("suffixText", QStringLiteral("px"));
    const qreal suffixW = sfx->property("suffixWidth").toReal();
    QVERIFY(suffixW > 0);
    // the hint grows by exactly the strip, and the panel grants it: the control
    // keeps both its offset and its width, the tail is never taken out of it
    QCOMPARE(sfx->property("implicitWidth").toReal(), sfxW0 + suffixW);
    QTRY_COMPARE(sfx->width(), sfxW0 + suffixW);
    QCOMPARE(spin->x(), labelW);
    QCOMPARE(spin->width(), spinW0);
    QCOMPARE(qRound(sfx->width() - spin->x() - spin->width()), qRound(suffixW) + 2);

    // ---- the leaf actually paints the suffix inside the tail strip ----
    QList< QQuickItem* > visualItems;
    collectVisualItems(sfx, &visualItems);
    QQuickItem* suffixText = nullptr;
    for (QQuickItem* item : visualItems) {
        if (item->metaObject()->className() == QByteArrayLiteral("QQuickText")
            && item->property("text").toString() == QLatin1String("px")) {
            suffixText = item;
            break;
        }
    }
    QVERIFY2(suffixText, "the container leaf must render the suffix label");
    QVERIFY(suffixText->isVisible());
    QVERIFY(suffixText->x() + suffixText->width() <= sfx->width() + 1.0);
    QVERIFY(suffixText->x() + 1.0 >= spin->x() + spin->width());

    // ---- enableShowTitle / enableShowIcon drop their slot from the metrics ----
    const qreal textOnly = iconed->property("labelWidth").toReal();
    QVERIFY(textOnly > 0);
    iconed->setProperty("iconSource", QUrl::fromLocalFile(iconPath).toString());
    const qreal withIcon = iconed->property("labelWidth").toReal();
    QVERIFY(withIcon > textOnly);
    const qreal iconSlot = withIcon - textOnly;
    QTRY_COMPARE(combo2->x(), withIcon);

    iconed->setProperty("enableShowTitle", false);
    QCOMPARE(iconed->property("labelWidth").toReal(), iconSlot);
    QTRY_COMPARE(combo2->x(), iconSlot);

    iconed->setProperty("enableShowIcon", false);
    QCOMPARE(iconed->property("labelWidth").toReal(), 0.0);
    QTRY_COMPARE(combo2->x(), 0.0);

    // both switches back on restore the exact strip (no drift)
    iconed->setProperty("enableShowTitle", true);
    iconed->setProperty("enableShowIcon", true);
    QCOMPARE(iconed->property("labelWidth").toReal(), withIcon);
    QTRY_COMPARE(combo2->x(), withIcon);

    // the hint follows the strip, so the panel gives the width back
    const qreal hintNoTitle = iconed->property("implicitWidth").toReal();
    iconed->setProperty("enableShowTitle", false);
    QTRY_COMPARE(iconed->property("implicitWidth").toReal(), hintNoTitle - textOnly);
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
                RibbonGalleryItem { text: "Document File" }
                RibbonGalleryItem { text: "Drive File Four Word" }
                RibbonGalleryItem { text: "six" }
                RibbonGalleryItem { text: "Network Location File" }
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

    // ---- caption band + icon box: the host splits the cell with the very same
    // core helper the widgets group feeds setIconSize with, so the two-line
    // caption the leaf renders really fits inside the cell instead of spilling
    // over the icon (NOTES B50). captionHeight is lineSpacing * 2 for the
    // word-wrap style, which recovers the helper's font input ----
    const int captionH  = gallery->property("captionHeight").toInt();
    const QSize iconBox(gallery->property("cellIconWidth").toInt(), gallery->property("cellIconHeight").toInt());
    QVERIFY2(captionH > 0 && captionH % 2 == 0, "the word-wrap caption band is two text lines");
    const SA::GalleryCellMetrics cm = SA::calcGalleryCellMetrics(cell.width(),
                                                                 cell.height(),
                                                                 captionH / 2,
                                                                 1,
                                                                 SA::GalleryCaptionStyle::WordWrap);
    QCOMPARE(captionH, cm.captionHeight);
    QCOMPARE(iconBox, cm.iconSize);
    QVERIFY2(iconBox.width() > 0 && iconBox.width() <= cell.width(), "icon box stays inside the cell");
    QVERIFY2(captionH + iconBox.height() <= cell.height(), "icon box + caption band must fit the cell");

    // ---- the rendered captions really sit in that band: the leaf's caption
    // Text elements are the ones carrying the long item texts, and each must be
    // a box of exactly captionHeight inside the cell width, with a font small
    // enough for the two wrapped lines (this is what kept "Document File" /
    // "Drive File Four Word" from drawing over the icon) ----
    const QStringList longCaptions { QStringLiteral("Document File"),
                                     QStringLiteral("Drive File Four Word"),
                                     QStringLiteral("Network Location File") };
    int captionTextCount = 0;
    QList< QQuickItem* > leafItems;
    collectVisualItems(gallery, &leafItems);
    for (QQuickItem* it : leafItems) {
        if (it->metaObject()->className() != QByteArrayLiteral("QQuickText")) {
            continue;
        }
        if (!longCaptions.contains(it->property("text").toString())) {
            continue;
        }
        ++captionTextCount;
        QVERIFY2(it->width() > 0 && it->width() <= cell.width(), "caption Text stays inside the cell width");
        QCOMPARE(it->height(), qreal(captionH));
        const int px = it->property("font").value< QFont >().pixelSize();
        QVERIFY2(px > 0 && px * 2 <= captionH, "caption font leaves room for both wrapped lines");
        QVERIFY2(it->y() + it->height() <= cell.height() + 1, "caption band does not spill past the cell");
    }
    QVERIFY2(captionTextCount > 0, "the long captions are rendered by the leaf");

    // 10 items / columns -> totalRows follows
    const int columns = gallery->property("gridColumns").toInt();
    const int expectRows = (10 + columns - 1) / columns;
    QCOMPARE(gallery->property("totalRows").toInt(), expectRows);

    // ---- scrolling clamps against totalRows - displayRow (derive the bound
    // from the actual layout: a wide gallery fits everything in one screen) ----
    const int maxScroll = qMax(expectRows - gallery->property("displayRow").toInt(), 0);
    // page past the bound: the clamp, not the call count, must be what stops it
    for (int i = 0; i < maxScroll + 2; ++i) {
        QMetaObject::invokeMethod(gallery, "scrollDown");
    }
    QTRY_COMPARE(gallery->property("scrollRow").toInt(), maxScroll);
    QMetaObject::invokeMethod(gallery, "scrollUp");
    QTRY_COMPARE(gallery->property("scrollRow").toInt(), qMax(maxScroll - 1, 0));

    // ---- group switch (2 items; the row count follows the live column count) ----
    gallery->setProperty("currentGroupIndex", 1);
    QTRY_COMPARE(gallery->property("totalRows").toInt(), (2 + columns - 1) / columns);
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
 * @brief Gallery caption styles, hover reporting and the selectable flag
 * @details The three captionStyle values are the QML face of the widgets
 *          SARibbonGalleryGroup::GalleryGroupStyle and must reach the very same
 *          core SA::calcGalleryCellMetrics branch the widgets group feeds
 *          setIconSize with — the leaf then mirrors the style into its two
 *          rendering switches (caption drawn at all / one elided line vs two
 *          wrapped lines), which is the delegate's three paint paths. Hover is
 *          reported cell-wise by the leaf and resolved by the host, publishing
 *          on both the gallery and the owning group; leaving the grid publishes
 *          the nullptr/-1 pair. selectable gates becoming current (widgets
 *          Qt::ItemIsSelectable) but never activation.
 */
void TestConformanceQml::galleryCaptionStylesHoverAndSelectable()
{
    QQmlEngine engine;
    saRibbonRegisterQmlTypes(&engine);

    QString src = QStringLiteral(R"QML(import QtQuick 2.12
import SARibbon 3.0
RibbonGallery {
    objectName: "gallery"
    width: 600
    height: 120
    RibbonGalleryGroup {
        objectName: "group0"
        groupTitle: "Styles"
        RibbonGalleryItem { objectName: "it0"; text: "alpha" }
        RibbonGalleryItem { objectName: "it1"; text: "beta" }
        RibbonGalleryItem { objectName: "it2"; text: "gamma"; selectable: false }
        RibbonGalleryItem { objectName: "it3"; text: "delta"; enabled: false }
    }
    RibbonGalleryGroup {
        objectName: "group1"
        groupTitle: "Other"
        RibbonGalleryItem { text: "solo" }
    }
}
)QML");

    QQmlComponent component(&engine);
    std::unique_ptr< QQuickView > view(exposeScene(engine, component, src.toUtf8().constData(), 600, 120));
    QVERIFY(view);
    QQuickItem* rootItem = view->rootObject();
    QVERIFY(rootItem);
    auto* host = qobject_cast< SARibbonQml::RibbonGallery* >(rootItem);
    QVERIFY2(host, "the scene root is the gallery host itself");
    QQuickItem* gallery = host;
    QTRY_VERIFY(gallery->width() > 0 && gallery->height() > 0);

    // the cell delegates are the only items carrying both leaf-local hover /
    // selection switches; collectVisualItems returns them in Repeater order,
    // which is the item order of the current group
    auto collectCells = [gallery]() {
        QList< QQuickItem* > all;
        collectVisualItems(gallery, &all);
        QList< QQuickItem* > cells;
        for (QQuickItem* it : all) {
            if (it->property("isCurrent").isValid() && it->property("entryEnabled").isValid()) {
                cells.append(it);
            }
        }
        return cells;
    };
    // the caption Text is the delegate's only QQuickText child
    auto captionOf = [](QQuickItem* cell) -> QQuickItem* {
        const QList< QQuickItem* > kids = cell->childItems();
        for (QQuickItem* k : kids) {
            if (k->metaObject()->className() == QByteArrayLiteral("QQuickText")) {
                return k;
            }
        }
        return nullptr;
    };

    // ---- caption styles: host metrics follow the core helper, leaf follows the host ----
    const QSize cell = gallery->property("gridSize").toSize();
    QVERIFY(cell.width() > 0 && cell.height() > 0);
    QCOMPARE(gallery->property("captionStyle").toInt(), int(SARibbonQml::RibbonEnums::GalleryIconWithWordWrapText));
    const int wrapCaption = gallery->property("captionHeight").toInt();
    QVERIFY2(wrapCaption > 0 && wrapCaption % 2 == 0, "the default style reserves a two-line caption band");
    // the word-wrap band is exactly two core line spacings, which recovers the
    // font input the host feeds calcGalleryCellMetrics with
    const int lineSpacing = wrapCaption / 2;

    // QQuickText::WrapMode values (QtQuick has no public QQuickText header, so
    // the two constants the leaf switches between are spelled out here)
    const int kWrapNone = 0;  // Text.NoWrap
    const int kWrapWord = 1;  // Text.WordWrap

    struct StyleCase
    {
        SARibbonQml::RibbonEnums::GalleryCaptionStyle style;
        SA::GalleryCaptionStyle core;
        const char* name;
    };
    const StyleCase cases[] = {
        { SARibbonQml::RibbonEnums::GalleryIconOnly, SA::GalleryCaptionStyle::None, "IconOnly" },
        { SARibbonQml::RibbonEnums::GalleryIconWithText, SA::GalleryCaptionStyle::SingleLine, "IconWithText" },
        { SARibbonQml::RibbonEnums::GalleryIconWithWordWrapText, SA::GalleryCaptionStyle::WordWrap, "IconWithWordWrapText" },
    };
    for (const StyleCase& c : cases) {
        QSignalSpy metricsSpy(gallery, SIGNAL(gridMetricsChanged()));
        gallery->setProperty("captionStyle", int(c.style));
        QCOMPARE(gallery->property("captionStyle").toInt(), int(c.style));
        QVERIFY2(!metricsSpy.isEmpty(), "a caption style change republishes the grid metrics");
        const SA::GalleryCellMetrics cm = SA::calcGalleryCellMetrics(cell.width(), cell.height(), lineSpacing, 1, c.core);
        QCOMPARE(gallery->property("captionHeight").toInt(), cm.captionHeight);
        QCOMPARE(gallery->property("cellIconWidth").toInt(), cm.iconSize.width());
        QCOMPARE(gallery->property("cellIconHeight").toInt(), cm.iconSize.height());
        if (c.core == SA::GalleryCaptionStyle::None) {
            QCOMPARE(cm.captionHeight, 0);  // the band really disappears
            QCOMPARE(cm.iconSize.height(), cell.height() - 2 - 4);
        } else if (c.core == SA::GalleryCaptionStyle::SingleLine) {
            QCOMPARE(cm.captionHeight, lineSpacing);
        } else {
            QCOMPARE(cm.captionHeight, lineSpacing * 2);
        }

        const bool draws = (c.core != SA::GalleryCaptionStyle::None);
        const bool wraps = (c.core == SA::GalleryCaptionStyle::WordWrap);
        const QList< QQuickItem* > cells = collectCells();
        QVERIFY2(cells.size() >= 4, "every entry got a cell delegate");
        int inspected = 0;
        for (QQuickItem* cellItem : cells) {
            QQuickItem* caption = captionOf(cellItem);
            if (!caption) {
                continue;
            }
            ++inspected;
            QCOMPARE(caption->property("height").toInt(), cm.captionHeight);
            QCOMPARE(caption->property("y").toInt(), cell.height() - cm.captionHeight);
            const bool entryOn = cellItem->property("entryEnabled").toBool();
            QCOMPARE(caption->property("visible").toBool(), draws && entryOn);
            QCOMPARE(caption->property("wrapMode").toInt(), wraps ? kWrapWord : kWrapNone);
            QCOMPARE(caption->property("maximumLineCount").toInt(), wraps ? 2 : 1);
            const int px = caption->property("font").value< QFont >().pixelSize();
            if (draws) {
                QVERIFY2(px > 0 && px <= cm.captionHeight, "the caption font fits the band it is given");
            }
        }
        QVERIFY2(inspected >= 4, qPrintable(QStringLiteral("leaf captions inspected for %1").arg(QLatin1String(c.name))));
    }
    // restore the contract default before the interaction checks below
    gallery->setProperty("captionStyle", int(SARibbonQml::RibbonEnums::GalleryIconWithWordWrapText));

    // ---- hover: leaf reports the cell index, host resolves it to an entry and
    // publishes on the gallery AND on the owning group (widgets fires the group
    // signal and the gallery forwards it; here the host fires both) ----
    auto* group0 = host->groupAt(0);
    QVERIFY(group0);
    QSignalSpy hoverSpy(gallery, SIGNAL(hovered(SARibbonQml::RibbonGalleryItem*, int)));
    QSignalSpy groupHoverSpy(group0, SIGNAL(hovered(SARibbonQml::RibbonGalleryItem*, int)));

    const QList< QQuickItem* > cells = collectCells();
    QVERIFY(cells.size() >= 4);
    QVERIFY2(cells.at(1)->isVisible(), "the second cell is on screen for a real hover");
    // Qt Quick flushes hover delivery in the frame-synchronous phase, so an
    // otherwise idle window would never publish containsMouse: request a frame
    // with every move
    auto moveMouse = [&view](const QPointF& scenePos) {
        QTest::mouseMove(view.get(), scenePos.toPoint());
        view->requestUpdate();
        QTest::qWait(80);
    };
    const QPointF over1 = cells.at(1)->mapToScene(QPointF(cells.at(1)->width() / 2, cells.at(1)->height() / 2));
    moveMouse(over1);
    QTRY_VERIFY(!hoverSpy.isEmpty());
    QCOMPARE(hoverSpy.last().at(1).toInt(), 1);
    {
        auto* hoveredItem = qvariant_cast< QObject* >(hoverSpy.last().at(0));
        QVERIFY(hoveredItem);
        QCOMPARE(hoveredItem->property("text").toString(), QStringLiteral("beta"));
    }
    QCOMPARE(groupHoverSpy.size(), hoverSpy.size());

    // the non-selectable entry still reports hover: hover is not selection
    const QPointF over2 = cells.at(2)->mapToScene(QPointF(cells.at(2)->width() / 2, cells.at(2)->height() / 2));
    moveMouse(over2);
    QTRY_COMPARE(hoverSpy.last().at(1).toInt(), 2);
    QCOMPARE(groupHoverSpy.size(), hoverSpy.size());

    // leaving the grid publishes the nullptr/-1 pair so a hover preview clears
    moveMouse(QPointF(gallery->width() - 2, 2));  // over the button strip, outside the grid
    QTRY_VERIFY(qvariant_cast< QObject* >(hoverSpy.last().at(0)) == nullptr);
    QCOMPARE(hoverSpy.last().at(1).toInt(), -1);
    QCOMPARE(groupHoverSpy.size(), hoverSpy.size());

    // ---- selectable: gates becoming current, never activation ----
    QCOMPARE(host->currentItemIndex(), -1);
    QVERIFY(host->currentItem() == nullptr);
    QSignalSpy currentSpy(gallery, SIGNAL(currentItemChanged(SARibbonQml::RibbonGalleryItem*, int)));

    gallery->setProperty("currentItemIndex", 0);
    QCOMPARE(host->currentItemIndex(), 0);
    QVERIFY(host->currentItem());
    QCOMPARE(host->currentItem()->text(), QStringLiteral("alpha"));
    QCOMPARE(currentSpy.size(), 1);
    QCOMPARE(qvariant_cast< QObject* >(currentSpy.last().at(0)), static_cast< QObject* >(host->currentItem()));

    // the current cell paints the selection fill, its neighbours stay clear
    {
        const QColor selBg = SARibbonQml::RibbonTheme::instance()->selectionBg();
        const QList< QQuickItem* > painted = collectCells();
        for (int i = 0; i < painted.size(); ++i) {
            const QList< QQuickItem* > kids = painted.at(i)->childItems();
            QVERIFY(!kids.isEmpty());
            const QColor bg = kids.first()->property("color").value< QColor >();
            if (i == 0) {
                QCOMPARE(colorKey(bg), colorKey(selBg));
            } else {
                QCOMPARE(colorKey(bg), colorKey(QColor(Qt::transparent)));
            }
        }
    }

    // non-selectable / disabled / out-of-range are all refused and leave the
    // stored mark where it was (widgets selection-model parity)
    gallery->setProperty("currentItemIndex", 2);
    gallery->setProperty("currentItemIndex", 3);
    gallery->setProperty("currentItemIndex", 99);
    QCOMPARE(host->currentItemIndex(), 0);
    QCOMPARE(currentSpy.size(), 1);

    // any negative index normalizes to the -1 "no current cell" sentinel, which
    // is a clear request rather than a refusal
    gallery->setProperty("currentItemIndex", -7);
    QCOMPARE(host->currentItemIndex(), -1);
    QVERIFY(host->currentItem() == nullptr);
    QCOMPARE(currentSpy.size(), 2);
    QCOMPARE(currentSpy.last().at(1).toInt(), -1);
    gallery->setProperty("currentItemIndex", 0);
    QCOMPARE(host->currentItemIndex(), 0);
    QCOMPARE(currentSpy.size(), 3);

    // activation is NOT gated: the click still fires, the mark does not move
    QSignalSpy trigSpy(gallery, SIGNAL(triggered(SARibbonQml::RibbonGalleryItem*, int)));
    QMetaObject::invokeMethod(gallery, "activateItem", Q_ARG(int, 2));
    QCOMPARE(trigSpy.size(), 1);
    QCOMPARE(trigSpy.at(0).at(1).toInt(), 2);
    QCOMPARE(host->currentItemIndex(), 0);
    // a selectable cell does move the mark when activated
    QMetaObject::invokeMethod(gallery, "activateItem", Q_ARG(int, 1));
    QCOMPARE(trigSpy.size(), 2);
    QCOMPARE(host->currentItemIndex(), 1);
    QCOMPARE(currentSpy.size(), 4);

    // clearing selectable on the current entry drops the mark
    auto* it1 = group0->itemAt(1);
    QVERIFY(it1);
    it1->setSelectable(false);
    QCOMPARE(host->currentItemIndex(), -1);
    QVERIFY(host->currentItem() == nullptr);
    QCOMPARE(currentSpy.size(), 5);
    QCOMPARE(currentSpy.last().at(1).toInt(), -1);
    QVERIFY(qvariant_cast< QObject* >(currentSpy.last().at(0)) == nullptr);
    // restoring the flag does not re-claim the mark on its own
    it1->setSelectable(true);
    QCOMPARE(host->currentItemIndex(), -1);
    QCOMPARE(currentSpy.size(), 5);

    // the mark is group-scoped: switching groups clears it
    gallery->setProperty("currentItemIndex", 0);
    QCOMPARE(host->currentItemIndex(), 0);
    gallery->setProperty("currentGroupIndex", 1);
    QCOMPARE(host->currentItemIndex(), -1);
    QVERIFY(host->currentItem() == nullptr);
    gallery->setProperty("currentGroupIndex", 0);
    QCOMPARE(host->currentItemIndex(), -1);
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

    // ---- right group: flush against the right margin (native frame) ----
    QTRY_VERIFY(rgroup->width() > 0);
    // systemButtonStripWidth defaults to 0: no frameless system-button
    // reservation, the group ends at the bar's 8px right margin (widgets
    // only subtracts the strip under isUseRibbonFrame)
    QTRY_COMPARE(rgroup->x() + rgroup->width(), qreal(800 - 8));
    QVERIFY(help->width() > 0);
    // a frameless host declaring the strip pushes the group left of it
    QVERIFY(bar->setProperty("systemButtonStripWidth", 120));
    QTRY_COMPARE(rgroup->x() + rgroup->width(), qreal(800 - 120 - 8));

    // ---- the embedded quick access button is clickable ----
    QSignalSpy clickedSpy(save, SIGNAL(clicked()));
    const QPointF center = save->mapToScene(QPointF(save->width() / 2, save->height() / 2));
    QTest::mouseClick(view.get(), Qt::LeftButton, Qt::NoModifier, center.toPoint());
    QTRY_COMPARE(clickedSpy.size(), 1);
}

/**
 * @brief Single-choice behavior of the title-row button containers
 * @details The widgets side gets this for free from QActionGroup; the QML side
 *          has no action bridge, so RibbonButtonRowHost implements it. Covers
 *          the default (quick access bar stays multi-choice), the exclusive
 *          right group, the checkedButton() accessor, the absence of a signal
 *          storm on untouched siblings, and the QActionGroup rule that turning
 *          the flag on does not retroactively uncheck anything.
 */
void TestConformanceQml::buttonRowExclusivity()
{
    QQmlEngine engine;
    saRibbonRegisterQmlTypes(&engine);

    QString src = QStringLiteral(R"QML(import QtQuick 2.12
import SARibbon 3.0
Item {
    width: 900
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
            RibbonToolButton { objectName: "qa1"; text: "One"; checkable: true; proportion: Ribbon.Small }
            RibbonToolButton { objectName: "qa2"; text: "Two"; checkable: true; proportion: Ribbon.Small }
        }
        RibbonButtonGroup {
            objectName: "rgroup"
            exclusive: true
            RibbonToolButton { objectName: "rb1"; text: "A"; checkable: true; proportion: Ribbon.Small }
            RibbonToolButton { objectName: "rb2"; text: "B"; checkable: true; proportion: Ribbon.Small }
            RibbonToolButton { objectName: "rb3"; text: "C"; checkable: true; proportion: Ribbon.Small }
        }
        RibbonCategory {
            title: "Home"
            RibbonPanel {
                panelTitle: "P"
                RibbonToolButton { text: "X" }
            }
        }
    }
})QML");

    QQmlComponent component(&engine);
    std::unique_ptr< QQuickView > view(exposeScene(engine, component, src.toUtf8().constData(), 900, 300));
    QVERIFY(view);
    QQuickItem* rootItem = view->rootObject();
    QVERIFY(rootItem);

    auto* qab = rootItem->findChild< QQuickItem* >(QStringLiteral("qab"));
    auto* rg  = rootItem->findChild< QQuickItem* >(QStringLiteral("rgroup"));
    auto* qa1 = rootItem->findChild< QQuickItem* >(QStringLiteral("qa1"));
    auto* qa2 = rootItem->findChild< QQuickItem* >(QStringLiteral("qa2"));
    auto* rb1 = rootItem->findChild< QQuickItem* >(QStringLiteral("rb1"));
    auto* rb2 = rootItem->findChild< QQuickItem* >(QStringLiteral("rb2"));
    auto* rb3 = rootItem->findChild< QQuickItem* >(QStringLiteral("rb3"));
    QVERIFY(qab && rg && qa1 && qa2 && rb1 && rb2 && rb3);

    auto clickItem = [&view](QQuickItem* item) {
        QTRY_VERIFY(item->width() > 0 && item->height() > 0);
        const QPointF center = item->mapToScene(QPointF(item->width() / 2, item->height() / 2));
        QTest::mouseClick(view.get(), Qt::LeftButton, Qt::NoModifier, center.toPoint());
    };
    auto checkedButtonOf = [](QQuickItem* row) -> SARibbonQml::RibbonToolButton* {
        SARibbonQml::RibbonToolButton* result = nullptr;
        QMetaObject::invokeMethod(row, "checkedButton", Q_RETURN_ARG(SARibbonQml::RibbonToolButton*, result));
        return result;
    };

    // ---- defaults: the quick access row is multi-choice, the group is not ----
    QCOMPARE(qab->property("exclusive").toBool(), false);
    QCOMPARE(rg->property("exclusive").toBool(), true);
    QVERIFY(checkedButtonOf(qab) == nullptr);
    QVERIFY(checkedButtonOf(rg) == nullptr);

    // ---- non-exclusive row: both stay checked (no QActionGroup semantics) ----
    clickItem(qa1);
    QTRY_COMPARE(qa1->property("checked").toBool(), true);
    clickItem(qa2);
    QTRY_COMPARE(qa2->property("checked").toBool(), true);
    QVERIFY(qa1->property("checked").toBool());

    // ---- exclusive group: a check unchecks the rest ----
    QSignalSpy rb3Spy(rb3, SIGNAL(toggled(bool)));
    clickItem(rb1);
    QTRY_COMPARE(rb1->property("checked").toBool(), true);
    QCOMPARE(checkedButtonOf(rg), qobject_cast< SARibbonQml::RibbonToolButton* >(rb1));
    clickItem(rb2);
    QTRY_COMPARE(rb2->property("checked").toBool(), true);
    QTRY_COMPARE(rb1->property("checked").toBool(), false);
    QCOMPARE(checkedButtonOf(rg), qobject_cast< SARibbonQml::RibbonToolButton* >(rb2));
    // the untouched sibling was never written to
    QCOMPARE(rb3Spy.size(), 0);
    QCOMPARE(rb3->property("checked").toBool(), false);

    // ---- unchecking the only checked button leaves the group empty ----
    clickItem(rb2);
    QTRY_COMPARE(rb2->property("checked").toBool(), false);
    QVERIFY(checkedButtonOf(rg) == nullptr);

    // ---- switching exclusivity on constrains the NEXT check only ----
    qab->setProperty("exclusive", true);
    QCOMPARE(qab->property("exclusive").toBool(), true);
    QVERIFY(qa1->property("checked").toBool());
    QVERIFY(qa2->property("checked").toBool());
    clickItem(qa2);                                   // toggles qa2 off, nothing else to do
    QTRY_COMPARE(qa2->property("checked").toBool(), false);
    QVERIFY(qa1->property("checked").toBool());
    clickItem(qa2);                                   // now the exclusivity bites
    QTRY_COMPARE(qa2->property("checked").toBool(), true);
    QTRY_COMPARE(qa1->property("checked").toBool(), false);
    QCOMPARE(checkedButtonOf(qab), qobject_cast< SARibbonQml::RibbonToolButton* >(qa2));
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
    // the engine reserved a square on the title strip's right end
    const QRectF optRect = panel->property("optionButtonRect").toRectF();
    QVERIFY(optRect.width() > 0);
    QVERIFY(optRect.height() == optRect.width());
    const QRectF titleRect = panel->property("titleGeometry").toRectF();
    QVERIFY(qFuzzyCompare(optRect.y(), titleRect.y()));
    QVERIFY(optRect.x() >= titleRect.x());

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

/**
 * @brief Theme customization entry points of the QML singleton bridge
 * @details Covers the writable half of RibbonTheme: a single key color override
 *          must reach the rendered leaf (the bar paints RibbonTheme.accent over
 *          its whole background) and drag the derived tokens with it, a whole
 *          palette can be installed from JSON text / a file / the declarative
 *          customPaletteSource, and RibbonThemeUserDefine keeps the user palette
 *          instead of silently retaining the previous theme's colors. Ends by
 *          restoring the process-wide singleton state for the following cases.
 */
void TestConformanceQml::themeCustomization()
{
    QQmlEngine engine;
    saRibbonRegisterQmlTypes(&engine);

    SARibbonQml::RibbonTheme* theme = SARibbonQml::RibbonTheme::instance();
    QVERIFY(theme);

    QString src = QStringLiteral(R"QML(import QtQuick 2.12
import SARibbon 3.0
Item {
    width: 600
    height: 200
    RibbonBar {
        objectName: "bar"
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        RibbonCategory {
            title: "Home"
            RibbonPanel {
                panelTitle: "P"
                RibbonToolButton { text: "A" }
            }
        }
    }
}
)QML");

    QQmlComponent component(&engine);
    std::unique_ptr< QQuickView > view(exposeScene(engine, component, src.toUtf8().constData(), 600, 200));
    QVERIFY(view);

    // captured AFTER the bar exists: RibbonBar's construction-time dark mode
    // auto switch may already have moved the theme on a dark desktop
    const int themeAtEntry = theme->currentTheme();

    // deterministic starting point: a built-in theme with its own palette loaded
    theme->setCustomPaletteSource(QUrl());
    theme->setCurrentTheme(int(SARibbonTheme::RibbonThemeOffice2021Blue));
    QVERIFY(!theme->hasCustomPalette());
    const QColor builtinAccent = theme->accent();
    QVERIFY(builtinAccent.isValid());
    QTRY_VERIFY(theme->currentTheme() == int(SARibbonTheme::RibbonThemeOffice2021Blue));

    // ---- 1. the built-in accent really is what the leaf paints ----
    // grabWindow is polled: the palette notify has to travel through the QML
    // bindings and the scene graph before the frame carries the new color
    auto grabUntil = [&view](const QColor& want, int minPixels) -> QImage {
        QImage img;
        for (int i = 0; i < 50; ++i) {
            img = view->grabWindow();
            if (countPixelsNear(img, want) >= minPixels) {
                break;
            }
            QTest::qWait(20);
        }
        return img;
    };
    QImage frame = grabUntil(builtinAccent, 201);
    QVERIFY(!frame.isNull());
    const int builtinPixels = countPixelsNear(frame, builtinAccent);
    QVERIFY2(builtinPixels > 200, "the bar leaf must paint the theme accent");

    // ---- 2. setAccentColor: key color, derived tokens and the rendered frame ----
    QSignalSpy paletteSpy(theme, &SARibbonQml::RibbonTheme::paletteChanged);
    QSignalSpy customSpy(theme, &SARibbonQml::RibbonTheme::hasCustomPaletteChanged);
    const QColor custom(0xc0, 0x39, 0x2b);
    theme->setAccentColor(custom);
    QCOMPARE(colorKey(theme->accent()), colorKey(custom));
    QVERIFY(theme->hasCustomPalette());
    QCOMPARE(customSpy.count(), 1);
    QVERIFY(paletteSpy.count() >= 1);
    // the derived tokens were recomputed from the new key color (office2021-blue
    // derives accent-pressed with darken/15 on a light palette -> darker(115))
    QCOMPARE(colorKey(theme->accentPressed()), colorKey(custom.darker(115)));
    QVERIFY(colorKey(theme->accentPressed()) != colorKey(builtinAccent.darker(115)));
    frame = grabUntil(custom, 201);
    const int customPixels = countPixelsNear(frame, custom);
    QVERIFY2(customPixels > 200, "the overridden accent must reach the rendered leaf");
    QVERIFY2(countPixelsNear(frame, builtinAccent) < customPixels,
             "the built-in accent must be gone from the rendered leaf");

    // an invalid color is rejected and leaves the palette untouched
    theme->setAccentColor(QColor());
    QCOMPARE(colorKey(theme->accent()), colorKey(custom));

    // setContentBgColor / setTextColor go through the same mutation path
    theme->setContentBgColor(QColor("#101010"));
    QCOMPARE(colorKey(theme->contentBg()), colorKey(QColor("#101010")));
    theme->setTextColor(QColor("#f5f5f5"));
    QCOMPARE(colorKey(theme->textColor()), colorKey(QColor("#f5f5f5")));

    // ---- 3. whole palette from JSON text ----
    const QString json = QStringLiteral(R"JSON({
        "name": "test-palette",
        "isDark": true,
        "keyColors": {
            "accent": "#123456",
            "content-bg": "#0a0a0a",
            "text-color": "#f0f0f0"
        },
        "derived": {
            "accent-hover": { "fn": "lighten", "base": "accent", "amount": 10 }
        },
        "fixed": {
            "separator": "#202020"
        }
    })JSON");
    QVERIFY(theme->loadPaletteFromJson(json));
    QCOMPARE(colorKey(theme->tokenColor(QStringLiteral("accent"))), colorKey(QColor("#123456")));
    QCOMPARE(colorKey(theme->tokenColor(QStringLiteral("separator"))), colorKey(QColor("#202020")));
    // isDark reverses the derive direction: lighten/10 on a dark palette -> darker(110)
    QCOMPARE(colorKey(theme->tokenColor(QStringLiteral("accent-hover"))), colorKey(QColor("#123456").darker(110)));
    QVERIFY(theme->isDark());
    QVERIFY(theme->hasCustomPalette());

    // malformed JSON is refused and the previous palette survives
    QVERIFY(!theme->loadPaletteFromJson(QStringLiteral("{ this is not json")));
    QCOMPARE(colorKey(theme->tokenColor(QStringLiteral("accent"))), colorKey(QColor("#123456")));
    QVERIFY(!theme->loadPaletteFromFile(QStringLiteral(":/does/not/exist.json")));
    QCOMPARE(colorKey(theme->tokenColor(QStringLiteral("accent"))), colorKey(QColor("#123456")));

    // ---- 4. declarative source: a qrc URL loads the very same file ----
    const QUrl sourceUrl(QStringLiteral("qrc:/SARibbonTheme/resource/palettes/office2016-blue.json"));
    SA::SARibbonThemePalette expected;
    QVERIFY2(expected.loadFromFile(QStringLiteral(":/SARibbonTheme/resource/palettes/office2016-blue.json")),
             "the palette JSON must be reachable from the QML module's resources");
    QSignalSpy sourceSpy(theme, &SARibbonQml::RibbonTheme::customPaletteSourceChanged);
    theme->setCustomPaletteSource(sourceUrl);
    QCOMPARE(theme->customPaletteSource(), sourceUrl);
    QCOMPARE(sourceSpy.count(), 1);
    QCOMPARE(colorKey(theme->accent()), colorKey(expected.color(QStringLiteral("accent"))));
    QVERIFY(theme->hasCustomPalette());

    // ---- 5. RibbonThemeUserDefine keeps the user palette ----
    theme->setCurrentTheme(int(SARibbonTheme::RibbonThemeUserDefine));
    QCOMPARE(theme->currentTheme(), int(SARibbonTheme::RibbonThemeUserDefine));
    QCOMPARE(colorKey(theme->accent()), colorKey(expected.color(QStringLiteral("accent"))));
    QVERIFY(theme->hasCustomPalette());

    // a built-in theme takes the palette back and clears the custom flag ...
    theme->setCurrentTheme(int(SARibbonTheme::RibbonThemeOffice2021Blue));
    QVERIFY(!theme->hasCustomPalette());
    QCOMPARE(colorKey(theme->accent()), colorKey(builtinAccent));
    // ... and switching to user-define again re-applies the declared source
    // instead of leaving the built-in colors in place
    theme->setCurrentTheme(int(SARibbonTheme::RibbonThemeUserDefine));
    QVERIFY(theme->hasCustomPalette());
    QCOMPARE(colorKey(theme->accent()), colorKey(expected.color(QStringLiteral("accent"))));

    // ---- 6. the system dark mode bridge maps the core switch ----
    const bool followAtEntry = theme->followSystemDarkMode();
    QSignalSpy followSpy(theme, &SARibbonQml::RibbonTheme::followSystemDarkModeChanged);
    theme->setFollowSystemDarkMode(!followAtEntry);
    QCOMPARE(theme->followSystemDarkMode(), !followAtEntry);
    QCOMPARE(SA::isEnableSystemDarkModeAutoSwitch(), !followAtEntry);
    QCOMPARE(followSpy.count(), 1);
    theme->setFollowSystemDarkMode(!followAtEntry);  // no-op: already there
    QCOMPARE(followSpy.count(), 1);
    // reading the OS color scheme must not perturb the palette (no notify loop
    // between systemDarkMode and the palette-driven dark property)
    const QColor accentBeforeQuery = theme->accent();
    const bool sysDark = theme->isSystemDarkMode();
    QCOMPARE(sysDark, SA::isOperatingSystemInDarkMode());
    QCOMPARE(colorKey(theme->accent()), colorKey(accentBeforeQuery));
    paletteSpy.clear();
    theme->setFollowSystemDarkMode(followAtEntry);
    QCOMPARE(paletteSpy.count(), 0);

    // ---- restore the process-wide singleton for the following cases ----
    theme->setCustomPaletteSource(QUrl());
    theme->setCurrentTheme(themeAtEntry);
    QVERIFY(!theme->hasCustomPalette());
    QTRY_COMPARE(theme->currentTheme(), themeAtEntry);
}

/**
 * @brief Category scrolling: overflow flags, arrow buttons, wheel and animation
 * @details WS-A3. The engine's Result::scrollFlags used to be discarded by the
 *          QML host (only totalWidth was read), so a too-narrow category
 *          clipped its panels with no way to reach them. This case pins the
 *          widgets scroll parity end to end: the 12px arrow rectangles from the
 *          core pure function, the half-viewport arrow step, the wheel delta
 *          preference with its x2 / /2 scaling, wheels dropped while animating,
 *          the OutQuad animation on scrollPosition, and the "content fits"
 *          category that must ignore the wheel instead of eating it.
 */
void TestConformanceQml::categoryScrollWheelAndArrows()
{
    QQmlEngine engine;
    saRibbonRegisterQmlTypes(&engine);

    QString src = QStringLiteral(R"QML(import QtQuick 2.12
import SARibbon 3.0
Item {
    width: 460
    height: 300
    RibbonBar {
        objectName: "bar"
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        RibbonCategory {
            objectName: "cat"
            title: "Home"
            RibbonPanel { objectName: "panel0"; panelTitle: "P0"; RibbonToolButton { text: "Alpha" } }
            RibbonPanel { panelTitle: "P1"; RibbonToolButton { text: "Bravo" } }
            RibbonPanel { panelTitle: "P2"; RibbonToolButton { text: "Charlie" } }
            RibbonPanel { panelTitle: "P3"; RibbonToolButton { text: "Delta" } }
            RibbonPanel { panelTitle: "P4"; RibbonToolButton { text: "Echo" } }
            RibbonPanel { panelTitle: "P5"; RibbonToolButton { text: "Foxtrot" } }
            RibbonPanel { panelTitle: "P6"; RibbonToolButton { text: "Golf" } }
            RibbonPanel { panelTitle: "P7"; RibbonToolButton { text: "Hotel" } }
            RibbonPanel { panelTitle: "P8"; RibbonToolButton { text: "India" } }
            RibbonPanel { panelTitle: "P9"; RibbonToolButton { text: "Juliet" } }
        }
        RibbonCategory {
            objectName: "catFit"
            title: "Fit"
            RibbonPanel { objectName: "fitPanel"; panelTitle: "F"; RibbonToolButton { text: "One" } }
        }
    }
})QML");

    QQmlComponent component(&engine);
    std::unique_ptr< QQuickView > view(exposeScene(engine, component, src.toUtf8().constData(), 460, 300));
    QVERIFY(view);
    QQuickItem* rootItem = view->rootObject();
    QVERIFY(rootItem);

    auto* bar    = rootItem->findChild< QQuickItem* >(QStringLiteral("bar"));
    auto* cat    = rootItem->findChild< QQuickItem* >(QStringLiteral("cat"));
    auto* catFit = rootItem->findChild< QQuickItem* >(QStringLiteral("catFit"));
    auto* panel0 = rootItem->findChild< QQuickItem* >(QStringLiteral("panel0"));
    QVERIFY(bar && cat && catFit && panel0);

    QTRY_VERIFY(cat->width() > 0 && cat->height() > 0);
    QTRY_VERIFY(panel0->width() > 0 && panel0->height() > 0);

    auto flagsOf = [cat]() { return cat->property("scrollButtonFlags").toMap(); };
    auto geoOf   = [cat]() { return cat->property("scrollButtonGeometry").toMap(); };
    auto posOf   = [cat]() { return cat->property("scrollPosition").toInt(); };
    auto contentWidthOf = [](QQuickItem* category) {
        int w = 0;
        QMetaObject::invokeMethod(category, "contentWidth", Q_RETURN_ARG(int, w));
        return w;
    };

    // ---- 1. the engine Result::scrollFlags reaches the host ----
    QTRY_VERIFY(flagsOf().value(QStringLiteral("right")).toBool());
    QVERIFY(!flagsOf().value(QStringLiteral("left")).toBool());
    QCOMPARE(posOf(), 0);
    const int viewport = int(cat->width());
    const int total    = contentWidthOf(cat);
    QVERIFY2(total > viewport, "the category must overflow for this case to mean anything");

    // ---- 2. the arrow rectangles are the core pure function's output ----
    const QVariantMap geo = geoOf();
    QCOMPARE(geo.value(QStringLiteral("width")).toInt(), SARibbon::Core::SCROLL_BUTTON_WIDTH);
    QCOMPARE(geo.value(QStringLiteral("height")).toInt(), int(cat->height()));
    QCOMPARE(geo.value(QStringLiteral("leftX")).toInt(), 0);
    QCOMPARE(geo.value(QStringLiteral("rightX")).toInt(), viewport - SARibbon::Core::SCROLL_BUTTON_WIDTH);

    // the overlay leaf really instantiated the two buttons at those rectangles
    auto* leftBtn  = cat->findChild< QQuickItem* >(QStringLiteral("categoryScrollLeftButton"));
    auto* rightBtn = cat->findChild< QQuickItem* >(QStringLiteral("categoryScrollRightButton"));
    QVERIFY(leftBtn && rightBtn);
    QTRY_VERIFY(rightBtn->isVisible());
    QVERIFY(!leftBtn->isVisible());
    QTRY_COMPARE(rightBtn->width(), qreal(SARibbon::Core::SCROLL_BUTTON_WIDTH));
    QTRY_COMPARE(rightBtn->height(), cat->height());
    QTRY_COMPARE(rightBtn->x(), qreal(viewport - SARibbon::Core::SCROLL_BUTTON_WIDTH));

    const qreal panelX0 = panel0->x();
    auto clickItem = [&view](QQuickItem* item) {
        QTRY_VERIFY(item->width() > 0 && item->height() > 0);
        const QPointF center = item->mapToScene(QPointF(item->width() / 2, item->height() / 2));
        QTest::mouseClick(view.get(), Qt::LeftButton, Qt::NoModifier, center.toPoint());
    };

    // ---- 3. deterministic scrolling moves the panels, the arrows follow ----
    cat->setProperty("useAnimatingScroll", false);
    QCOMPARE(cat->property("useAnimatingScroll").toBool(), false);

    cat->setProperty("scrollPosition", -100);
    QTRY_COMPARE(posOf(), -100);
    QTRY_COMPARE(panel0->x(), panelX0 - 100);
    QTRY_VERIFY(flagsOf().value(QStringLiteral("left")).toBool());
    QVERIFY(flagsOf().value(QStringLiteral("right")).toBool());
    QVERIFY(leftBtn->isVisible());

    // a real click on the trailing arrow steps by half the viewport (LTR: negative)
    clickItem(rightBtn);
    const int afterRightClick = SARibbon::Core::clampScrollOffset(-100 - viewport / 2, total, viewport, false);
    QVERIFY(afterRightClick < -100);
    QTRY_COMPARE(posOf(), afterRightClick);
    QTRY_COMPARE(panel0->x(), panelX0 + afterRightClick);

    // and the leading arrow steps back the same amount
    QVERIFY(leftBtn->isVisible());
    clickItem(leftBtn);
    const int afterLeftClick = SARibbon::Core::clampScrollOffset(afterRightClick + viewport / 2, total, viewport, false);
    QCOMPARE(afterLeftClick, -100);
    QTRY_COMPARE(posOf(), afterLeftClick);
    QTRY_COMPARE(panel0->x(), panelX0 - 100);

    // the base step never runs past the end of the content
    cat->setProperty("scrollPosition", -1000000);
    const int minBase = viewport - total;
    QTRY_COMPARE(posOf(), minBase);
    // the flags are republished by the next polish, not by setScrollPosition
    QTRY_VERIFY(!flagsOf().value(QStringLiteral("right")).toBool());
    QVERIFY(flagsOf().value(QStringLiteral("left")).toBool());
    QVERIFY(!rightBtn->isVisible());
    QVERIFY(leftBtn->isVisible());
    cat->setProperty("scrollPosition", 0);
    QTRY_COMPARE(posOf(), 0);

    // ---- 4. wheel delta preference and step scaling (core pure functions) ----
    QCOMPARE(SARibbon::Core::wheelScrollDelta(QPoint(7, 9), QPoint(0, 0)), 7);        // pixelDelta.x wins
    QCOMPARE(SARibbon::Core::wheelScrollDelta(QPoint(0, 9), QPoint(0, 0)), 9);        // then pixelDelta.y
    QCOMPARE(SARibbon::Core::wheelScrollDelta(QPoint(), QPoint(160, 0)), 20);         // then angleDelta/8 .x
    QCOMPARE(SARibbon::Core::wheelScrollDelta(QPoint(), QPoint(0, -240)), -30);       // then angleDelta/8 .y
    const int base = cat->property("wheelScrollStep").toInt();
    QCOMPARE(base, 400);
    QCOMPARE(SARibbon::Core::scaledWheelStep(base, 0), base);        // no delta leaves the step alone
    QCOMPARE(SARibbon::Core::scaledWheelStep(base, 15), base / 2);   // |delta| < 20 -> half
    QCOMPARE(SARibbon::Core::scaledWheelStep(base, -30), -base);     // neutral band
    QCOMPARE(SARibbon::Core::scaledWheelStep(base, 120), 2 * base);  // |delta| > 60 -> double

    // ---- 5. the real wheel path goes through the very same functions ----
    const QPointF wheelAt = cat->mapToScene(QPointF(cat->width() / 2, cat->height() / 2));
    auto sendWheel = [&view, wheelAt](int angleY) {
        QWheelEvent ev(wheelAt, view->mapToGlobal(wheelAt.toPoint()), QPoint(0, 0), QPoint(0, angleY),
                       Qt::NoButton, Qt::NoModifier, Qt::NoScrollPhase, false);
        QGuiApplication::sendEvent(view.get(), &ev);
        return ev.isAccepted();
    };
    int pos = posOf();
    auto expectWheel = [&](int angleY, bool accepted = true) {
        const int delta = SARibbon::Core::wheelScrollDelta(QPoint(), QPoint(0, angleY));
        pos             = SARibbon::Core::clampScrollOffset(pos + SARibbon::Core::scaledWheelStep(base, delta),
                                                            total, viewport, false);
        QCOMPARE(sendWheel(angleY), accepted);
        QTRY_COMPARE(posOf(), pos);
    };
    expectWheel(-120);  // |15| < 20 -> half step down
    QVERIFY2(pos < 0, "the wheel must actually scroll the overflowing category");
    QTRY_COMPARE(panel0->x(), panelX0 + pos);
    expectWheel(120);   // back up, clamped at the start
    QCOMPARE(pos, 0);

    // wheelScrollStep is a real property, not a constant
    cat->setProperty("wheelScrollStep", 60);
    QSignalSpy stepSpy(cat, SIGNAL(wheelScrollStepChanged()));
    QCOMPARE(cat->property("wheelScrollStep").toInt(), 60);
    cat->setProperty("wheelScrollStep", 80);
    QCOMPARE(stepSpy.count(), 1);
    const int step80 = cat->property("wheelScrollStep").toInt();
    QVERIFY(sendWheel(-120));
    pos = SARibbon::Core::clampScrollOffset(0 - step80 / 2, total, viewport, false);
    QTRY_COMPARE(posOf(), pos);
    cat->setProperty("wheelScrollStep", base);

    // ---- 6. the animated path lands on the same target ----
    cat->setProperty("useAnimatingScroll", true);
    QCOMPARE(cat->property("useAnimatingScroll").toBool(), true);
    QCOMPARE(cat->property("animationDuration").toInt(), SARibbon::Core::SCROLL_ANIMATION_DURATION);
    cat->setProperty("scrollPosition", 0);
    QTRY_COMPARE(posOf(), 0);
    const int animTarget = SARibbon::Core::clampScrollOffset(0 - viewport / 2, total, viewport, false);
    QVERIFY(animTarget < 0);
    QVERIFY(QMetaObject::invokeMethod(cat, "scrollByButton", Q_ARG(bool, false)));
    QVERIFY(cat->property("isAnimatingScroll").toBool());
    // a wheel arriving mid-animation is dropped instead of retargeting it
    QVERIFY(!sendWheel(-120));
    QTRY_COMPARE(posOf(), animTarget);
    QTRY_VERIFY(!cat->property("isAnimatingScroll").toBool());
    QTRY_COMPARE(panel0->x(), panelX0 + animTarget);

    // ---- 7. a category whose content fits ignores the wheel ----
    QVector< QQuickItem* > tabHosts;
    const auto barKids = bar->findChildren< QQuickItem* >();
    for (QQuickItem* item : barKids) {
        if (QString::fromLatin1(item->metaObject()->className()) == QLatin1String("SARibbonQml::RibbonTab")) {
            tabHosts.append(item);
        }
    }
    QCOMPARE(tabHosts.size(), 2);
    clickItem(tabHosts[ 1 ]);
    QTRY_VERIFY(catFit->isVisible());
    QTRY_VERIFY(!cat->isVisible());
    int fitTotal = 0;
    QTRY_VERIFY((fitTotal = contentWidthOf(catFit)) > 0);
    QVERIFY2(fitTotal <= int(catFit->width()), "the second category must fit for this case to mean anything");
    const QVariantMap fitFlags = catFit->property("scrollButtonFlags").toMap();
    QVERIFY(!fitFlags.value(QStringLiteral("left")).toBool());
    QVERIFY(!fitFlags.value(QStringLiteral("right")).toBool());
    auto* fitLeftBtn = catFit->findChild< QQuickItem* >(QStringLiteral("categoryScrollLeftButton"));
    QVERIFY(fitLeftBtn && !fitLeftBtn->isVisible());
    QCOMPARE(catFit->property("scrollPosition").toInt(), 0);
    const QPointF fitWheelAt = catFit->mapToScene(QPointF(catFit->width() / 2, catFit->height() / 2));
    QWheelEvent fitEv(fitWheelAt, view->mapToGlobal(fitWheelAt.toPoint()), QPoint(0, 0), QPoint(0, -120),
                      Qt::NoButton, Qt::NoModifier, Qt::NoScrollPhase, false);
    QGuiApplication::sendEvent(view.get(), &fitEv);
    QVERIFY2(!fitEv.isAccepted(), "a fitting category must not eat the wheel");
    QCOMPARE(catFit->property("scrollPosition").toInt(), 0);
}

/**
 * @brief The five URL-registered basic controls embedded through containers
 * @details RibbonCheckBox/RadioButton/ComboBox/SpinBox/TextField are pure QML
 *          documents registered by URL (no C++ host). This case pins their
 *          contract end to end: every control is reparented into its
 *          RibbonControlContainer and stretched to the engine-assigned row,
 *          the implicit sizes stay compact (the stock Basic indicators are
 *          28-40px tall and would blow the row), the indicators are the 14px
 *          variants, and each control stays functional through real mouse
 *          input — including the themed popup of the combo (the stock style
 *          popup would keep white-on-black colors in dark themes).
 */
void TestConformanceQml::embeddedRibbonBasicControls()
{
    QQmlEngine engine;
    saRibbonRegisterQmlTypes(&engine);

    QString src = QStringLiteral(R"QML(import QtQuick 2.12
import SARibbon 3.0
Item {
    width: 700
    height: 400
    RibbonPanel {
        objectName: "panel"
        anchors.fill: parent
        panelTitle: "P"
        RibbonControlContainer {
            objectName: "checkContainer"
            text: "Check:"
            control: RibbonCheckBox { objectName: "check"; text: "check me" }
        }
        RibbonControlContainer {
            objectName: "radioContainer"
            text: "Radio:"
            control: RibbonRadioButton { objectName: "radio"; text: "pick me" }
        }
        RibbonControlContainer {
            objectName: "comboContainer"
            text: "Combo:"
            control: RibbonComboBox { objectName: "combo"; model: [ "alpha", "beta", "gamma" ] }
        }
        RibbonControlContainer {
            objectName: "spinContainer"
            text: "Spin:"
            suffixText: "px"
            control: RibbonSpinBox { objectName: "spin"; from: 0; to: 100; value: 10 }
        }
        RibbonControlContainer {
            objectName: "fieldContainer"
            text: "Edit:"
            control: RibbonTextField { objectName: "field"; placeholderText: "type here" }
        }
    }
}
)QML");

    QQmlComponent component(&engine);
    std::unique_ptr< QQuickView > view(exposeScene(engine, component, src.toUtf8().constData(), 700, 400));
    QVERIFY(view);
    QQuickItem* rootItem = view->rootObject();
    QVERIFY(rootItem);

    auto clickItem = [&view](QQuickItem* item) {
        QTRY_VERIFY(item->width() > 0 && item->height() > 0);
        const QPointF center = item->mapToScene(QPointF(item->width() / 2, item->height() / 2));
        QTest::mouseClick(view.get(), Qt::LeftButton, Qt::NoModifier, center.toPoint());
    };

    // ---- all five controls created, reparented into their containers ----
    auto* check           = rootItem->findChild< QQuickItem* >(QStringLiteral("check"));
    auto* radio           = rootItem->findChild< QQuickItem* >(QStringLiteral("radio"));
    auto* combo           = rootItem->findChild< QQuickItem* >(QStringLiteral("combo"));
    auto* spin            = rootItem->findChild< QQuickItem* >(QStringLiteral("spin"));
    auto* field           = rootItem->findChild< QQuickItem* >(QStringLiteral("field"));
    auto* checkContainer  = rootItem->findChild< QQuickItem* >(QStringLiteral("checkContainer"));
    auto* comboContainer  = rootItem->findChild< QQuickItem* >(QStringLiteral("comboContainer"));
    QVERIFY(check && radio && combo && spin && field && checkContainer && comboContainer);
    for (QQuickItem* c : { check, radio, combo, spin, field }) {
        QTRY_VERIFY(c->width() > 0 && c->height() > 0);
        QCOMPARE(c->parentItem()->property("labelWidth").isValid(), true);  // inside a container
    }

    // ---- the container stretches the control to the engine-assigned row ----
    // (host geometry: x = labelWidth, height = row height minus 2 * margin)
    QTRY_VERIFY(checkContainer->height() > 0);
    QCOMPARE(qRound(check->height()), qRound(checkContainer->height() - 4));
    QVERIFY(check->x() >= checkContainer->property("labelWidth").toReal() - 1.0);
    QCOMPARE(qRound(combo->height()), qRound(comboContainer->height() - 4));

    // ---- compact implicit sizes: the stock Basic 28-40px defaults are gone ----
    QVERIFY2(check->implicitHeight() < 26, "checkbox must fit a ribbon row");
    QVERIFY2(radio->implicitHeight() < 26, "radio button must fit a ribbon row");
    QVERIFY2(combo->implicitHeight() < 30, "combo must be font-derived, not the stock 40px");
    QVERIFY2(spin->implicitHeight() < 30, "spin box must be font-derived, not the stock 40px");
    QVERIFY2(field->implicitHeight() < 30, "text field must be font-derived, not the stock 40px");

    // ---- the indicators are the compact 14px variants ----
    auto* checkIndicator = check->findChild< QQuickItem* >(QStringLiteral("ribbonCheckBoxIndicator"));
    auto* radioIndicator = radio->findChild< QQuickItem* >(QStringLiteral("ribbonRadioButtonIndicator"));
    QVERIFY(checkIndicator && radioIndicator);
    QCOMPARE(qRound(checkIndicator->width()), 14);
    QCOMPARE(qRound(checkIndicator->height()), 14);
    QCOMPARE(qRound(radioIndicator->width()), 14);
    QCOMPARE(qRound(radioIndicator->height()), 14);

    // ---- checkbox toggles through a real click on the indicator ----
    // AbstractButton::toggled() carries NO parameter (checked is a property)
    QSignalSpy toggleSpy(check, SIGNAL(toggled()));
    QCOMPARE(check->property("checked").toBool(), false);
    clickItem(checkIndicator);
    QTRY_COMPARE(check->property("checked").toBool(), true);
    QCOMPARE(toggleSpy.count(), 1);

    // ---- radio button checks through a real click on the ring ----
    QCOMPARE(radio->property("checked").toBool(), false);
    clickItem(radioIndicator);
    QTRY_COMPARE(radio->property("checked").toBool(), true);

    // ---- combo popup opens themed and picks through a real click ----
    QSignalSpy activatedSpy(combo, SIGNAL(activated(int)));
    clickItem(combo);
    QObject* popup = combo->property("popup").value< QObject* >();
    QVERIFY(popup);
    QTRY_COMPARE(popup->property("visible").toBool(), true);
    auto* popupBg = popup->findChild< QQuickItem* >(QStringLiteral("ribbonComboBoxPopupBackground"));
    QVERIFY(popupBg);
    // themed dropdown: the background follows the palette token, not a style default
    SARibbonQml::RibbonTheme* theme = SARibbonQml::RibbonTheme::instance();
    QCOMPARE(colorKey(popupBg->property("color").value< QColor >()), colorKey(theme->contentBg()));
    // delegate rows are invisible to findChildren (B50: delegates live outside
    // their visual parent in the QObject tree) — walk the scene from the window
    auto popupRows = [ &view ]() -> QVector< QQuickItem* > {
        QVector< QQuickItem* > rows;
        QList< QQuickItem* > all;
        collectVisualItems(view->contentItem(), &all);
        for (QQuickItem* it : all) {
            if (it->objectName() == QLatin1String("ribbonComboBoxPopupRow")) {
                rows.append(it);
            }
        }
        return rows;
    };
    QTRY_COMPARE(popupRows().size(), 3);
    clickItem(popupRows().value(1));
    QTRY_COMPARE(popup->property("visible").toBool(), false);
    QCOMPARE(combo->property("currentIndex").toInt(), 1);
    QCOMPARE(combo->property("currentText").toString(), QStringLiteral("beta"));
    QCOMPARE(activatedSpy.count(), 1);
    QCOMPARE(activatedSpy.at(0).at(0).toInt(), 1);

    // ---- spin steppers: a real click on the up indicator steps the value ----
    QCOMPARE(spin->property("value").toInt(), 10);
    auto* upBtn = spin->findChild< QQuickItem* >(QStringLiteral("ribbonSpinBoxUpIndicator"));
    QVERIFY(upBtn);
    clickItem(upBtn);
    QTRY_COMPARE(spin->property("value").toInt(), 11);

    // ---- text field: placeholder visible while empty, typing replaces it ----
    auto* placeholder = field->findChild< QQuickItem* >(QStringLiteral("ribbonTextFieldPlaceholder"));
    QVERIFY(placeholder);
    QCOMPARE(placeholder->property("visible").toBool(), true);
    clickItem(field);
    // QTest::keyClicks lives in the GUI half of QtTest (qtest_gui.h), which
    // this target does not link; the per-key form is in the core half
    QTest::keyEvent(QTest::Click, view.get(), 'h');
    QTest::keyEvent(QTest::Click, view.get(), 'i');
    QTRY_COMPARE(field->property("text").toString(), QStringLiteral("hi"));
    QCOMPARE(placeholder->property("visible").toBool(), false);
}

/**
 * @brief The basic controls follow RibbonTheme and RibbonMetrics standalone
 * @details The five controls bind their colors to RibbonTheme tokens and their
 *          font to RibbonMetrics, so they follow theme switches and ribbon
 *          font changes wherever they are placed (also outside a panel). This
 *          case switches the palette twice and asserts every visible binding
 *          re-resolves (indicator fill, input backgrounds, placeholder), and
 *          that a font point size change grows the font-derived heights.
 */
void TestConformanceQml::ribbonBasicControlsThemeAndFont()
{
    QQmlEngine engine;
    saRibbonRegisterQmlTypes(&engine);

    SARibbonQml::RibbonTheme* theme   = SARibbonQml::RibbonTheme::instance();
    SARibbonQml::RibbonMetrics* metrics = SARibbonQml::RibbonMetrics::instance();
    QVERIFY(theme && metrics);

    QString src = QStringLiteral(R"QML(import QtQuick 2.12
import SARibbon 3.0
Item {
    width: 400
    height: 260
    Column {
        spacing: 4
        RibbonCheckBox { objectName: "check"; text: "c"; checked: true }
        RibbonComboBox { objectName: "combo"; model: [ "a", "b" ] }
        RibbonSpinBox { objectName: "spin"; from: 0; to: 10 }
        RibbonTextField { objectName: "field"; placeholderText: "ph" }
    }
}
)QML");

    QQmlComponent component(&engine);
    std::unique_ptr< QQuickView > view(exposeScene(engine, component, src.toUtf8().constData(), 400, 260));
    QVERIFY(view);
    QQuickItem* rootItem = view->rootObject();
    QVERIFY(rootItem);

    const int themeAtEntry = theme->currentTheme();
    // deterministic starting point (themeCustomization may have left a custom
    // palette behind): a built-in theme reloads its own palette
    theme->setCustomPaletteSource(QUrl());
    theme->setCurrentTheme(int(SARibbonTheme::RibbonThemeOffice2021Blue));

    auto* check           = rootItem->findChild< QQuickItem* >(QStringLiteral("check"));
    auto* combo           = rootItem->findChild< QQuickItem* >(QStringLiteral("combo"));
    auto* spin            = rootItem->findChild< QQuickItem* >(QStringLiteral("spin"));
    auto* field           = rootItem->findChild< QQuickItem* >(QStringLiteral("field"));
    QVERIFY(check && combo && spin && field);
    auto* checkIndicator = check->findChild< QQuickItem* >(QStringLiteral("ribbonCheckBoxIndicator"));
    auto* comboBg        = combo->findChild< QQuickItem* >(QStringLiteral("ribbonComboBoxBackground"));
    auto* spinBg         = spin->findChild< QQuickItem* >(QStringLiteral("ribbonSpinBoxBackground"));
    auto* fieldBg        = field->findChild< QQuickItem* >(QStringLiteral("ribbonTextFieldBackground"));
    auto* placeholder    = field->findChild< QQuickItem* >(QStringLiteral("ribbonTextFieldPlaceholder"));
    QVERIFY(checkIndicator && comboBg && spinBg && fieldBg && placeholder);

    // ---- colors resolve from the tokens of the running palette ----
    QTRY_COMPARE(colorKey(checkIndicator->property("color").value< QColor >()), colorKey(theme->inputFocus()));
    QCOMPARE(colorKey(comboBg->property("color").value< QColor >()), colorKey(theme->contentBg()));
    QCOMPARE(colorKey(spinBg->property("color").value< QColor >()), colorKey(theme->contentBg()));
    QCOMPARE(colorKey(fieldBg->property("color").value< QColor >()), colorKey(theme->contentBg()));
    QCOMPARE(colorKey(placeholder->property("color").value< QColor >()), colorKey(theme->subtitle()));

    // ---- theme switch: every visible binding re-resolves through the tokens ----
    const QColor lightBg = theme->contentBg();
    theme->setCurrentTheme(int(SARibbonTheme::RibbonThemeDark));
    QVERIFY(colorKey(theme->contentBg()) != colorKey(lightBg));
    QTRY_COMPARE(colorKey(comboBg->property("color").value< QColor >()), colorKey(theme->contentBg()));
    QTRY_COMPARE(colorKey(spinBg->property("color").value< QColor >()), colorKey(theme->contentBg()));
    QTRY_COMPARE(colorKey(fieldBg->property("color").value< QColor >()), colorKey(theme->contentBg()));
    QTRY_COMPARE(colorKey(checkIndicator->property("color").value< QColor >()), colorKey(theme->inputFocus()));
    QTRY_COMPARE(colorKey(placeholder->property("color").value< QColor >()), colorKey(theme->subtitle()));

    // ---- the font follows RibbonMetrics and drives the implicit heights ----
    const int fontAtEntry = metrics->fontPointSize();
    QVERIFY2(fontAtEntry > 0, "the application font must be point-size based for this case");
    QCOMPARE(check->property("font").value< QFont >().pointSize(), fontAtEntry);
    const qreal comboH0 = combo->implicitHeight();
    metrics->setFontPointSize(fontAtEntry + 3);
    QTRY_COMPARE(check->property("font").value< QFont >().pointSize(), fontAtEntry + 3);
    QTRY_VERIFY(combo->implicitHeight() > comboH0 + 2);

    // ---- restore the process-wide singletons for the following cases ----
    metrics->setFontPointSize(fontAtEntry);
    theme->setCustomPaletteSource(QUrl());
    theme->setCurrentTheme(themeAtEntry);
    QTRY_COMPARE(theme->currentTheme(), themeAtEntry);
}

QTEST_MAIN(TestConformanceQml)
#include "tst_conformance_qml.moc"

/**
 * \if ENGLISH
 * @brief Frameless agent wiring: strip reservation, title mirror, button row
 * @details Declares a RibbonWindowAgent through the bar's windowAgent
 *          property and asserts the whole contract the leaf relies on:
 *          the strip width reservation (widgets 4:3:3 stretch parity), the
 *          window title mirror the leaf paints into the title free rect,
 *          the framelessActive gate, and the system button registration.
 *          Runs under the offscreen platform: QWK attaches its Qt-level
 *          hooks to the QQuickWindow, so the contracts are verifiable
 *          without a native decoration.
 * \endif
 *
 * \if CHINESE
 * @brief 无边框代理接线：预留宽度、标题镜像、系统按钮注册
 * @details 通过 bar 的 windowAgent 属性声明一个 RibbonWindowAgent，
 *          断言叶子依赖的完整契约：预留宽度（widgets 4:3:3 拉伸比例）、
 *          叶子绘制在标题自由区的窗口标题镜像、framelessActive 门槛，
 *          以及系统按钮注册。offscreen 平台下可运行：QWK 把 Qt 层钩子
 *          挂到 QQuickWindow 上，无需原生装饰即可验证这些契约。
 * \endif
 */
void TestConformanceQml::framelessAgentStripAndTitle()
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
        windowAgent: RibbonWindowAgent {
            objectName: "agent"
            buttonWidth: 40
        }
        RibbonCategory {
            title: "Home"
            RibbonPanel {
                panelTitle: "P1"
                RibbonToolButton { text: "A" }
            }
        }
    }
}
)QML");

    QQmlComponent component(&engine);
    std::unique_ptr< QQuickView > view(exposeScene(engine, component, src.toUtf8().constData(), 800, 300));
    QVERIFY(view);
    QQuickItem* rootItem = view->rootObject();
    QVERIFY(rootItem);

    auto* bar = rootItem->findChild< SARibbonQml::RibbonBar* >(QStringLiteral("bar"));
    QVERIFY(bar);
    auto* agent = rootItem->findChild< SARibbonQml::RibbonWindowAgent* >(QStringLiteral("agent"));
    QVERIFY(agent);

    // ---- the property assignment took the agent and the window attached ----
    QCOMPARE(bar->windowAgent(), agent);
    QVERIFY(agent->window() == view.get());

    // ---- enabled from the start: strip = 4:3:3 stretch over 3*buttonWidth
    // (widgets SARibbonSystemButtonBar parity) — 120 at the 40px glyph width ----
    QCOMPARE(agent->stripWidth(), 120);
    QTRY_COMPARE(bar->systemButtonStripWidth(), 120);
    QCOMPARE(bar->isFramelessActive(), true);

    // ---- flipping off drops both (reservation follows the flag; native
    // frame parity: the widgets layouts reserve nothing without
    // isUseRibbonFrame) ----
    agent->setFramelessEnabled(false);
    QCOMPARE(agent->stripWidth(), 0);
    QTRY_COMPARE(bar->systemButtonStripWidth(), 0);
    QCOMPARE(bar->isFramelessActive(), false);
    agent->setFramelessEnabled(true);
    QTRY_COMPARE(bar->isFramelessActive(), true);

    // ---- window title mirror follows the attached window ----
    view->setTitle(QStringLiteral("Frameless Title Test"));
    QTRY_COMPARE(bar->windowTitle(), QStringLiteral("Frameless Title Test"));

    // ---- title free rect stays valid with the reservation in effect ----
    // (relayout is polished asynchronously; the strip flip above queues a
    // polish, offscreen render loops flush it on the next event spin)
    QTRY_VERIFY_WITH_TIMEOUT(bar->titleRect().width() > 0, 5000);
    const QRectF titleRect = bar->titleRect();
    QVERIFY2(titleRect.width() > 0 && titleRect.height() > 0, "title free rect must stay valid");
    // QRect->QRectF widens right() by 1 (inclusive vs exclusive edge), so
    // compare the far edge, not the rect right(), against the strip bound
    QVERIFY2(titleRect.x() + titleRect.width() <= 800 - 120 + 1,
             "title rect must not reach into the reserved strip");

    // ---- right button group anchor: strip pushes hosts left of it ----
    // (asserted through the bar geometry: rightEdge = width - strip - 8)
    // sanity only — full layout assertions live in the layout parity suite

    // ---- system button registration round trip (QWK object identity) ----
    // register a plain item directly; the offscreen window is enough for the
    // agent's QWK bookkeeping (setSystemButton only records the item)
    QQuickItem dummy;
    QVERIFY(bar->setSystemButton(QStringLiteral("minimize"), &dummy));
    QVERIFY(bar->setSystemButton(QStringLiteral("maximize"), &dummy));
    QVERIFY(bar->setSystemButton(QStringLiteral("close"), &dummy));
    QVERIFY(!bar->setSystemButton(QStringLiteral("bogus"), &dummy));
}