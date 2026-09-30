#include <QtTest>
#include <QApplication>
#include <QMenu>
#include <QWidgetAction>
#include "SARibbonPanel.h"
#include "SARibbonCategory.h"
#include "SARibbonBar.h"
#include "SARibbonToolButton.h"

/**
 * @brief Ribbon 按钮 automation 标识回归测试（GitHub #121）
 *
 * 约定：面板按钮的 objectName / accessibleName 自动从其承载的 action 继承；
 * 用户显式设置的 action objectName 优先，不被覆盖。
 */
namespace {
// 在 panel 的所有子控件中找到承载指定 action 的 SARibbonToolButton
SARibbonToolButton* buttonForAction(SARibbonPanel* panel, QAction* action)
{
    const auto buttons = panel->findChildren< SARibbonToolButton* >();
    for (SARibbonToolButton* b : buttons) {
        if (b->defaultAction() == action) {
            return b;
        }
    }
    return nullptr;
}
}  // namespace

class SARibbonAutomationIdTest : public QObject
{
    Q_OBJECT
private slots:
    void testButtonInheritsActionObjectName();
    void testButtonFallsBackToActionText();
    void testExplicitActionObjectNameNotOverwrittenByAddWidget();
    void testAccessibleNameNotEmpty();
};

void SARibbonAutomationIdTest::testButtonInheritsActionObjectName()
{
    SARibbonPanel panel(nullptr);
    QAction* action = new QAction(QStringLiteral("btn-text"), &panel);
    action->setObjectName("mySaveAction");
    panel.addLargeAction(action);
    panel.resize(400, 100);
    panel.show();
    QApplication::processEvents();

    SARibbonToolButton* button = buttonForAction(&panel, action);
    QVERIFY2(button != nullptr, "tool button not created for action");
    QCOMPARE(button->objectName(), QStringLiteral("mySaveAction"));
    panel.hide();
}

void SARibbonAutomationIdTest::testButtonFallsBackToActionText()
{
    SARibbonPanel panel(nullptr);
    QAction* action = new QAction(QStringLiteral("btn-fallback-text"), &panel);  // 未设 objectName
    panel.addLargeAction(action);
    panel.resize(400, 100);
    panel.show();
    QApplication::processEvents();

    SARibbonToolButton* button = buttonForAction(&panel, action);
    QVERIFY2(button != nullptr, "tool button not created for action");
    // action 没有 objectName 时按钮名回退到 action 文本
    QCOMPARE(button->objectName(), QStringLiteral("btn-fallback-text"));
    panel.hide();
}

void SARibbonAutomationIdTest::testExplicitActionObjectNameNotOverwrittenByAddWidget()
{
    // 复现 addWidget 无条件覆盖 action objectName 的不一致（对应 addMenu 已有 isEmpty 保护）
    SARibbonPanel panel(nullptr);
    QWidget* w = new QWidget(&panel);
    w->setObjectName("myWidget");
    QAction* action = panel.addSmallWidget(w);
    QVERIFY(action != nullptr);
    // action 未设 objectName 时才应生成 "action.xxx"
    QCOMPARE(action->objectName(), QStringLiteral("action.myWidget"));

    // 用户显式设置的 action objectName 必须保留
    QWidget* w2 = new QWidget(&panel);
    w2->setObjectName("myWidget2");
    QAction* action2 = panel.addSmallWidget(w2);
    action2->setObjectName("userDefinedActionName");
    QCOMPARE(action2->objectName(), QStringLiteral("userDefinedActionName"));

    // addMenu 路径：action objectName 不为空时不被覆盖
    QMenu* menu = new QMenu(&panel);
    menu->setObjectName("menuObj");
    QAction* menuAction = menu->menuAction();
    menuAction->setObjectName("userMenuAction");
    panel.addMenu(menu, SARibbonPanelItem::Large);
    QCOMPARE(menuAction->objectName(), QStringLiteral("userMenuAction"));
}

void SARibbonAutomationIdTest::testAccessibleNameNotEmpty()
{
    SARibbonPanel panel(nullptr);
    QAction* action = new QAction(QStringLiteral("accessible-test"), &panel);
    action->setObjectName("accAction");
    panel.addLargeAction(action);
    panel.resize(400, 100);
    panel.show();
    QApplication::processEvents();

    SARibbonToolButton* button = buttonForAction(&panel, action);
    QVERIFY2(button != nullptr, "tool button not created for action");
    QVERIFY2(!button->accessibleName().isEmpty(),
             qPrintable(QString("accessibleName empty, text=%1").arg(button->accessibleName())));
    panel.hide();
}

QTEST_MAIN(SARibbonAutomationIdTest)

#include "SARibbonAutomationIdTest.moc"
