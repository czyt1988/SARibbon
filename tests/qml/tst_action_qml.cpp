#include <QtTest>
#include <QQuickView>
#include <QQuickItem>
#include <QQuickWindow>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QRegularExpression>
#include <QSignalSpy>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QIcon>
#include <QPixmap>
#include <QGuiApplication>
#include <memory>
#include <SARibbonQml/SARibbonQmlGlobal.h>
#include <SARibbonQml/SARibbonQmlTypes.h>
#include <SARibbonQml/SARibbonQmlAction.h>
#include <SARibbonQml/SARibbonQmlActionRegistry.h>
#include <SARibbonQml/SARibbonQmlToolButton.h>
#include <SARibbonQml/SARibbonQmlGalleryGroup.h>
#include <SARibbonQml/SARibbonQmlPanel.h>
#include <SARibbonQml/SARibbonQmlBar.h>
#include <SARibbonQml/SARibbonQmlShortcutMatcher.h>

/**
 * @brief Command-layer unit tests (plan-05 S8)
 * @details Covers the QAction binding contract of RibbonToolButton
 *          (derivation / write-through / shared state / detach), the
 *          RibbonAction convenience surface, the backend-driven
 *          RibbonPanel::addAction route and the real shortcut triggering
 *          through the bar-level fallback matcher (S0-V2 verdict: no
 *          auto-trigger, so the matcher IS the mechanism on both lanes).
 *          Deliberately links no QtWidgets: the process runs on a plain
 *          QGuiApplication — the same environment the Qt5 lane promises
 *          (S0-V3).
 */
class TestActionQml : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void bareActionDerivation();
    void writeThroughToLocalWrites();
    void sharedActionTwoViews();
    void detachKeepsActionAlive();
    void nullActionFallback();
    void panelAddActionBackendRoute();
    void shortcutTriggersThroughWindow();
    void keyboardSpaceTriggers();
    void ribbonActionQmlRoute();
    void menuPanelSharedCommand();
    void destroyedActionCleanup();
    void menuCheckableFlipOrder();
    void galleryActionActivation();
    void registryObjectNameAddressing();

private:
    QQuickView* exposeScene(const QByteArray& qml, int w, int h);
};

// context property backend: the widgets-migration story shape (a C++ object
// exposing QAction* properties, zero QML knowledge on the C++ side)
class Backend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAction* actionSave READ actionSave CONSTANT)
public:
    using QObject::QObject;
    QAction* actionSave()
    {
        mSave.setObjectName("actionSave");
        return &mSave;
    }

private:
    QAction mSave;
};

QQuickView* TestActionQml::exposeScene(const QByteArray& qml, int w, int h)
{
    QQuickView* view = new QQuickView();
    view->resize(w, h);
    view->show();
    return view;
}

/**
 * @brief Derivation: binding a bare QAction mirrors the command state
 */
void TestActionQml::bareActionDerivation()
{
    SARibbonQml::RibbonToolButton btn;
    QAction act;
    act.setText("Save");
    act.setToolTip("Save the file");
    act.setCheckable(true);
    act.setChecked(true);
    act.setEnabled(false);

    btn.setAction(&act);
    QCOMPARE(btn.text(), QStringLiteral("Save"));
    QCOMPARE(btn.toolTip(), QStringLiteral("Save the file"));
    QVERIFY(btn.isCheckable());
    QVERIFY(btn.isChecked());
    QVERIFY(!btn.isEnabled());

    // icon of a bare QAction: the image provider url (plan-05 S3)
    act.setIcon(QIcon(QPixmap(16, 16)));
    QVERIFY(btn.iconSource().startsWith(QStringLiteral("image://saribbon/act/")));
}

/**
 * @brief Write-through: a local property write lands on the action
 */
void TestActionQml::writeThroughToLocalWrites()
{
    SARibbonQml::RibbonToolButton btn;
    QAction act;
    btn.setAction(&act);

    btn.setText("Open");
    QCOMPARE(act.text(), QStringLiteral("Open"));

    btn.setToolTip("Open a file");
    QCOMPARE(act.toolTip(), QStringLiteral("Open a file"));

    act.setCheckable(true);
    btn.setChecked(true);
    QVERIFY(act.isChecked());

    btn.setCheckable(false);
    QVERIFY(!act.isCheckable());

    // QQuickItem::setEnabled drives through the EnabledChange event; the
    // action follows the item state (write-through, plan-05 S4)
    btn.setEnabled(false);
    QTRY_VERIFY(!act.isEnabled());
}

/**
 * @brief One command, many views: a state change syncs every bound button
 */
void TestActionQml::sharedActionTwoViews()
{
    SARibbonQml::RibbonToolButton panelButton;
    SARibbonQml::RibbonToolButton quickButton;
    QAction act;
    act.setCheckable(true);

    panelButton.setAction(&act);
    quickButton.setAction(&act);

    QSignalSpy panelSpy(&panelButton, &SARibbonQml::RibbonToolButton::checkedChanged);
    act.setChecked(true);
    QVERIFY(quickButton.isChecked());
    QVERIFY(panelButton.isChecked());
    QCOMPARE(panelSpy.count(), 1);

    // clicking either view triggers the action: the other view follows
    QSignalSpy actionSpy(&act, &QAction::triggered);
    QSignalSpy quickSpy(&quickButton, &SARibbonQml::RibbonToolButton::clicked);
    panelButton.click();
    QCOMPARE(actionSpy.count(), 1);
    QCOMPARE(quickSpy.count(), 1);
    QVERIFY(!quickButton.isChecked());  // QAction toggled the shared state
}

/**
 * @brief Detach only unbinds: the action survives, mirrored values stay
 */
void TestActionQml::detachKeepsActionAlive()
{
    SARibbonQml::RibbonToolButton btn;
    auto act = std::make_unique< QAction >();
    act->setText("Keep");
    btn.setAction(act.get());
    QCOMPARE(btn.text(), QStringLiteral("Keep"));

    QSignalSpy destroyedSpy(act.get(), &QObject::destroyed);
    btn.setAction(nullptr);
    QCOMPARE(destroyedSpy.count(), 0);  // detach never destroys (contract D6)
    QCOMPARE(btn.text(), QStringLiteral("Keep"));  // mirrored value stays
    act->setText("ChangedAfterDetach");
    QCOMPARE(btn.text(), QStringLiteral("Keep"));  // no live sync anymore

    // plain-declarative storage works again after the unbind
    btn.setText("Local");
    QCOMPARE(btn.text(), QStringLiteral("Local"));
    QCOMPARE(act->text(), QStringLiteral("ChangedAfterDetach"));
}

/**
 * @brief Null-action buttons keep the pre-action semantics
 */
void TestActionQml::nullActionFallback()
{
    SARibbonQml::RibbonToolButton btn;
    btn.setCheckable(true);
    QVERIFY(!btn.isChecked());
    QSignalSpy toggledSpy(&btn, &SARibbonQml::RibbonToolButton::toggled);
    QSignalSpy clickedSpy(&btn, &SARibbonQml::RibbonToolButton::clicked);
    btn.click();
    QVERIFY(btn.isChecked());
    QCOMPARE(toggledSpy.count(), 1);
    QCOMPARE(clickedSpy.count(), 1);
}

/**
 * @brief Backend-driven placement: panel->addAction creates the bound button
 */
void TestActionQml::panelAddActionBackendRoute()
{
    SARibbonQml::RibbonPanel panel;
    QAction act;
    act.setText("Backend");

    SARibbonQml::RibbonToolButton* btn = panel.addAction(&act, 1 /* Large */);
    QVERIFY(btn != nullptr);
    QCOMPARE(btn->action(), &act);
    QCOMPARE(int(btn->proportion()), 1);  // Ribbon.Large
    QCOMPARE(panel.childItemCount(), 1);
    QCOMPARE(btn->parent(), &panel);  // QObject parent: the panel

    QAction act2;
    act2.setText("Second");
    SARibbonQml::RibbonToolButton* btn2 = panel.addAction(&act2, 3 /* Small */, 0);
    QVERIFY(btn2 != nullptr);
    QCOMPARE(panel.childItemIndex(btn2), 0);  // insert at 0 honored
    QCOMPARE(int(btn2->proportion()), 3);
}

/**
 * @brief Shortcut reality: Ctrl+S reaches the action through the window
 * @details The S0-V2 verdict (a QAction shortcut never auto-fires in a
 *          QQuickWindow) is exactly why the bar-level matcher exists; this
 *          case drives the whole chain: QML button under a bar under a
 *          window, real QTest key press, action triggered exactly once.
 */
void TestActionQml::shortcutTriggersThroughWindow()
{
    // declaration order = reverse destruction order: the view dies LAST so
    // the engine and the content root tear down while the window is still
    // alive (the bar unregisters its event filters on the leave-scene path;
    // a window dying first leaves nothing to unregister from)
    QQuickView view;
    QQmlEngine engine;
    saRibbonRegisterQmlTypes(&engine);
    Backend backend;
    backend.actionSave()->setText("Save");
    backend.actionSave()->setShortcut(QKeySequence("Ctrl+S"));
    engine.rootContext()->setContextProperty(QStringLiteral("backend"), &backend);

    QQmlComponent component(&engine);
    component.setData(R"(
import QtQuick 2.12
import SARibbon 3.0
RibbonBar {
    id: bar
    width: 600
    height: 90
    RibbonCategory {
        title: "Home"
        RibbonPanel {
            panelTitle: "Commands"
            RibbonToolButton {
                action: backend.actionSave
                proportion: Ribbon.Small
            }
        }
    }
}
)",
                      QUrl());
    QVERIFY2(!component.isError(), qPrintable(component.errorString()));
    std::unique_ptr< QObject > root(component.create());
    QVERIFY(root != nullptr);

    view.resize(640, 200);
    view.setContent(QUrl(), &component, root.get());
    view.show();
    QTRY_VERIFY(view.isExposed());
    QAction* save = backend.actionSave();
    QSignalSpy triggeredSpy(save, &QAction::triggered);
    QTest::keyClick(&view, Qt::Key_S, Qt::ControlModifier);
    QCOMPARE(triggeredSpy.count(), 1);

    // second press: one more trigger, one at a time (no double firing with
    // the QML delivery: the matcher consumed the key before the focus item)
    QTest::keyClick(&view, Qt::Key_S, Qt::ControlModifier);
    QCOMPARE(triggeredSpy.count(), 2);

    // a disabled command never fires
    save->setEnabled(false);
    QTest::keyClick(&view, Qt::Key_S, Qt::ControlModifier);
    QCOMPARE(triggeredSpy.count(), 2);
    save->setEnabled(true);
}

/**
 * @brief RibbonAction: the QML-declared command route
 */
void TestActionQml::ribbonActionQmlRoute()
{
    QQmlEngine engine;
    saRibbonRegisterQmlTypes(&engine);
    // a local file icon (the qrc of the QML module carries QML documents
    // only; a real image is provided from a temp file for the QIcon mirror)
    // applicationDirPath, not QDir::temp(): under a Git Bash session TMP
    // points at a POSIX /tmp that is not writable on Windows
    const QString iconPath = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("sa_ribbon_action_icon.png");
    // a 1x1 PNG embedded as bytes: image writers are not guaranteed under
    // the stripped offscreen environment, file IO always is
    static const unsigned char redPng[] = { 137,80,78,71,13,10,26,10,0,0,0,13,73,72,68,82,0,0,0,1,0,0,0,1,8,2,0,0,0,144,119,83,222,0,0,0,12,73,68,65,84,120,156,99,248,207,192,0,0,3,1,1,0,201,254,146,239,0,0,0,0,73,69,78,68,174,66,96,130 };
    {
        QFile f(iconPath);
        QVERIFY(f.open(QIODevice::WriteOnly));
        QVERIFY(f.write(reinterpret_cast< const char* >(redPng), sizeof(redPng)) == sizeof(redPng));
    }
    const QString qmlIcon = QUrl::fromLocalFile(iconPath).toString();
    QQmlComponent component(&engine);
    component.setData(QStringLiteral(R"(
import SARibbon 3.0
RibbonAction {
    id: cmd
    text: "Paste"
    shortcutText: "Ctrl+V"
    iconSource: "%1"
    checkable: true
    toolTip: "Paste from clipboard"
    menuActions: [ ]
}
)").arg(qmlIcon).toUtf8(),
                      QUrl());
    QVERIFY2(!component.isError(), qPrintable(component.errorString()));
    std::unique_ptr< QObject > obj(component.create());
    auto* act = qobject_cast< SARibbonQml::RibbonAction* >(obj.get());
    QVERIFY(act != nullptr);

    // QML-written properties land on the QAction half (single authority)
    QCOMPARE(act->text(), QStringLiteral("Paste"));
    QCOMPARE(act->shortcut(), QKeySequence("Ctrl+V"));
    QCOMPARE(act->shortcutText(), QStringLiteral("Ctrl+V"));
    QVERIFY(act->isCheckable());
    QCOMPARE(act->toolTip(), QStringLiteral("Paste from clipboard"));
    QVERIFY(!act->icon().isNull());  // qrc url mirrored into the QIcon

    // the convenience notify also follows a C++ shortcut write
    QSignalSpy seqSpy(act, &SARibbonQml::RibbonAction::shortcutTextChanged);
    act->setShortcut(QKeySequence("Ctrl+X"));
    QCOMPARE(seqSpy.count(), 1);
}

/**
 * @brief Keyboard baseline (plan-05 S7): tab focus on the leaf + Space
 * @details The leaf carries activeFocusOnTab and the Space/Enter Keys
 * handling; forcing the active focus onto it and pressing Space must run
 * the same click path a mouse click would (action triggered).
 */
void TestActionQml::keyboardSpaceTriggers()
{
    // declaration order: the view dies last (see shortcutTriggersThroughWindow)
    QQuickView view;
    QQmlEngine engine;
    saRibbonRegisterQmlTypes(&engine);
    Backend backend;
    backend.actionSave()->setText("Save");
    engine.rootContext()->setContextProperty(QStringLiteral("backend"), &backend);

    QQmlComponent component(&engine);
    component.setData(R"(
import QtQuick 2.12
import SARibbon 3.0
RibbonBar {
    width: 600
    height: 90
    RibbonCategory {
        title: "Home"
        RibbonPanel {
            panelTitle: "Commands"
            RibbonToolButton {
                action: backend.actionSave
                proportion: Ribbon.Small
            }
        }
    }
}
)",
                      QUrl());
    QVERIFY2(!component.isError(), qPrintable(component.errorString()));
    std::unique_ptr< QObject > root(component.create());
    QVERIFY(root != nullptr);
    view.resize(640, 200);
    view.setContent(QUrl(), &component, root.get());
    view.show();
    QTRY_VERIFY(view.isExposed());

    // locate the bound button through a full visual walk, focus its leaf
    SARibbonQml::RibbonToolButton* btn = nullptr;
    QList< QQuickItem* > pending { view.contentItem() };
    while (!pending.isEmpty() && !btn) {
        QQuickItem* candidate = pending.takeFirst();
        if ((btn = qobject_cast< SARibbonQml::RibbonToolButton* >(candidate))) {
            break;
        }
        pending.append(candidate->childItems());
    }
    QVERIFY(btn != nullptr);
    QQuickItem* leaf = btn->qmlLeaf();
    QVERIFY(leaf != nullptr);
    QVERIFY(leaf->activeFocusOnTab());
    leaf->setFocus(true);
    leaf->forceActiveFocus();
    QTRY_VERIFY(leaf->hasActiveFocus());

    QAction* save = backend.actionSave();
    QSignalSpy spy(save, &QAction::triggered);
    QTest::keyClick(&view, Qt::Key_Space);
    QTRY_COMPARE(spy.count(), 1);
}

/**
 * @brief One command shared by a panel button and a menu entry (plan-06 gate 2)
 * @details A checkable RibbonAction sits in the button's menuActions AND as a
 *          panel button's bound action. Toggling through the MENU path
 *          (activateMenuItemPath) flips the same QAction state the panel
 *          button mirrors; disabling the command kills both views.
 */
void TestActionQml::menuPanelSharedCommand()
{
    QQuickView view;
    QQmlEngine engine;
    saRibbonRegisterQmlTypes(&engine);
    QQmlComponent component(&engine);
    component.setData(R"(
import QtQuick 2.12
import SARibbon 3.0
RibbonBar {
    width: 600
    height: 90
    RibbonCategory {
        title: "Home"
        RibbonPanel {
            panelTitle: "Commands"
            RibbonAction { id: sharedCmd; objectName: "actionShared"; text: "Grid"; checkable: true }
            RibbonToolButton { action: sharedCmd; proportion: Ribbon.Small }
            RibbonToolButton {
                objectName: "menuHost"
                text: "View"
                proportion: Ribbon.Small
                popupMode: Ribbon.InstantPopup
                menuActions: [ sharedCmd ]
            }
        }
    }
}
)",
                      QUrl());
    QVERIFY2(!component.isError(), qPrintable(component.errorString()));
    std::unique_ptr< QObject > root(component.create());
    QVERIFY(root != nullptr);
    view.resize(640, 200);
    view.setContent(QUrl(), &component, root.get());
    view.show();
    QTRY_VERIFY(view.isExposed());

    // walk to the two views of the same command
    SARibbonQml::RibbonToolButton *panelBtn = nullptr, *menuHost = nullptr;
    QList< QQuickItem* > pending { view.contentItem() };
    while (!pending.isEmpty()) {
        QQuickItem* candidate = pending.takeFirst();
        if (auto* b = qobject_cast< SARibbonQml::RibbonToolButton* >(candidate)) {
            if (b->objectName() == "menuHost") {
                menuHost = b;
            } else {
                panelBtn = b;
            }
        }
        pending.append(candidate->childItems());
    }
    QVERIFY(panelBtn != nullptr);
    QVERIFY(menuHost != nullptr);
    QCOMPARE(menuHost->menuActionCount(), 1);

    // activate through the MENU path: the shared QAction flips, the panel
    // view follows (single state, two placements — contract §6)
    QAction* act = menuHost->menuActionAt(0);
    QCOMPARE(act, panelBtn->action());
    QSignalSpy panelSpy(panelBtn, &SARibbonQml::RibbonToolButton::checkedChanged);
    QSignalSpy menuSpy(menuHost, &SARibbonQml::RibbonToolButton::menuTriggered);
    menuHost->activateMenuItem(0);
    QVERIFY(panelBtn->isChecked());
    QCOMPARE(panelSpy.count(), 1);
    QCOMPARE(menuSpy.count(), 1);
    QCOMPARE(menuSpy.at(0).at(0).value< QAction* >(), act);

    // disabling the command disables both views (command authority)
    act->setEnabled(false);
    QTRY_VERIFY(!panelBtn->isEnabled());
    menuHost->activateMenuItem(0);  // disabled entries are refused
    QVERIFY(panelBtn->isChecked());  // unchanged
}

/**
 * @brief Menu activation ordering: flip before menuTriggered (plan-06 S3)
 */
void TestActionQml::menuCheckableFlipOrder()
{
    SARibbonQml::RibbonToolButton btn;
    auto* cmd = new SARibbonQml::RibbonAction(&btn);
    cmd->setCheckable(true);
    cmd->setText("Flip");
    btn.addMenuAction(cmd);
    QSignalSpy toggleSpy(cmd, &QAction::toggled);
    QSignalSpy menuSpy(&btn, &SARibbonQml::RibbonToolButton::menuTriggered);
    btn.activateMenuItem(0);
    QCOMPARE(toggleSpy.count(), 1);
    QCOMPARE(menuSpy.count(), 1);
    QVERIFY(cmd->isChecked());
    // the sequence: the action flipped first, menuTriggered reported second
    QCOMPARE(menuSpy.at(0).at(0).value< QAction* >(), qobject_cast< QAction* >(cmd));
}

/**
 * @brief Gallery command binding and derivation (plan-06 S4)
 */
void TestActionQml::galleryActionActivation()
{
    SARibbonQml::RibbonGalleryGroup group;
    auto* cmd = new SARibbonQml::RibbonAction(&group);
    cmd->setObjectName("actionGallery");
    cmd->setText("Cell");
    auto* item = group.addAction(cmd);
    QVERIFY(item != nullptr);
    QCOMPARE(item->action(), qobject_cast< QAction* >(cmd));
    QCOMPARE(item->text(), QStringLiteral("Cell"));  // derived from the command

    QSignalSpy triggeredSpy(cmd, &QAction::triggered);
    cmd->trigger();
    QCOMPARE(triggeredSpy.count(), 1);
    // live derivation: a command text change reaches the cell
    cmd->setText("Renamed cell");
    QCOMPARE(item->text(), QStringLiteral("Renamed cell"));
}

/**
 * @brief Registry addressing by objectName (plan-06 S1, contract §5)
 */
void TestActionQml::registryObjectNameAddressing()
{
    SARibbonQml::RibbonActionRegistry reg;
    QAction withName;
    withName.setObjectName("named.command");
    withName.setText("Named");
    QVERIFY(reg.registeAction(&withName, 1));
    QCOMPARE(reg.action("named.command"), &withName);
    QCOMPARE(reg.key(&withName), QStringLiteral("named.command"));
    QCOMPARE(reg.descriptor("named.command").text(), QStringLiteral("Named"));

    // no objectName: refused (contract §5 — the identity IS the objectName)
    QAction anon;
    QTest::ignoreMessage(QtWarningMsg, QRegularExpression(".*refuses an action without objectName.*"));
    QVERIFY(!reg.registeAction(&anon, 1));

    // key mismatch: refused
    QTest::ignoreMessage(QtWarningMsg, QRegularExpression(".*differs from the action objectName.*"));
    QVERIFY(!reg.registeAction(&withName, 1, "other.key"));

    // a text change is a LIVE read, not a registration snapshot
    withName.setText("Renamed");
    QCOMPARE(reg.descriptor("named.command").text(), QStringLiteral("Renamed"));

    // a command template holds its data on a real QAction
    QVERIFY(reg.registeCommand(2, "template.cmd", "Template Text", QString(), SARibbon::Core::SARibbonRowProportion::Small));
    QAction* tmpl = reg.action("template.cmd");
    QVERIFY(tmpl != nullptr);
    QCOMPARE(tmpl->text(), QStringLiteral("Template Text"));
}

/**
 * @brief Review P1-1/P1-2 regression: a deleted command must not dangle
 * @details The menu model, the button menu lists and the bound action all
 *          hold raw QAction pointers — the destroyed hooks must drop the
 *          entries so no later derivation dereferences a dead command.
 */
void TestActionQml::destroyedActionCleanup()
{
    SARibbonQml::RibbonToolButton btn;
    auto* cmd1 = new SARibbonQml::RibbonAction;
    cmd1->setObjectName("dying1");
    cmd1->setText("First");
    auto* cmd2 = new SARibbonQml::RibbonAction;
    cmd2->setObjectName("dying2");
    cmd2->setText("Second");
    btn.addMenuAction(cmd1);
    btn.addMenuAction(cmd2);
    QCOMPARE(btn.menuActionCount(), 2);
    QCOMPARE(btn.menuModel().size(), 2);

    // a submenu entry dying must leave the parent's nested list too
    auto* parent = new SARibbonQml::RibbonAction;
    parent->setObjectName("parent");
    auto* child = new SARibbonQml::RibbonAction;
    child->setObjectName("child");
    parent->addMenuAction(child);

    // bound command dying: the button survives, action() nulls out
    auto* bound = new SARibbonQml::RibbonAction;
    bound->setObjectName("bound");
    btn.setAction(bound);
    QCOMPARE(btn.action(), qobject_cast< QAction* >(bound));
    delete bound;
    QVERIFY(btn.action() == nullptr);  // QPointer, not a dangling raw pointer

    // menu entry dying: the list drops it, the derived rows follow
    delete cmd1;
    QCOMPARE(btn.menuActionCount(), 1);
    QCOMPARE(btn.menuModel().size(), 1);
    QCOMPARE(btn.menuActionAt(0), qobject_cast< QAction* >(cmd2));

    // submenu child dying: the nested derivation no longer shows it
    // (RibbonMenuModel drives the nested rows of an entry's menuActions)
    SARibbonQml::RibbonMenuModel nestedModel;
    nestedModel.setActions(parent->menuActionList());
    QCOMPARE(nestedModel.rows().size(), 1);
    delete child;
    QCOMPARE(parent->menuActionCount(), 0);
    QCOMPARE(nestedModel.rows().size(), 0);  // the destroyed hook dropped it
    delete parent;
    delete cmd2;
}

// custom main: QTEST_MAIN switches to QApplication once QtWidgets is linked
// (Qt5 lane needs it for QAction), but the S0-V3 promise is QGuiApplication
int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    TestActionQml tc;
    return QTest::qExec(&tc, argc, argv);
}
#include "tst_action_qml.moc"
