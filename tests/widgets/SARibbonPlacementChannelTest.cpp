#include <QtTest>
#include <QApplication>
#include <QToolBar>
#include "SARibbonActionsManager.h"
#include "SARibbonBar.h"
#include "SARibbonCategory.h"
#include "SARibbonPanel.h"
#include "SARibbonPanelLayout.h"
#include "SARibbonPanelItem.h"
#include "SARibbonToolButton.h"

/**
 * \if ENGLISH
 * @brief Regression test for the plan-07 placement channel
 * @details The 2.x channel smuggled placement (row proportion / popup mode) through
 *          `_sa_*` dynamic properties on the QAction, which turned placement into
 *          shared mutable state on the command. These cases lock the fixed structure:
 *          placement is carried by the panel's pending table into the per-panel
 *          SARibbonPanelItem, so the same action can live with different proportions
 *          in different panels.
 * \endif
 *
 * \if CHINESE
 * @brief 计划 07 放置通道的回归测试
 * @details 2.x 通道把放置参数（行占比/弹出模式）经 `_sa_*` 动态属性走私到 QAction 上，
 *          使放置变成命令上的共享可变状态。本用例固化修复后的结构：放置参数经面板
 *          pending 表进入每面板独立的 SARibbonPanelItem，同一action可以在不同面板
 *          以不同比例共存。
 * \endif
 */
class SARibbonPlacementChannelTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void sameActionTwoPanelsDifferentProportion();
    void popupModeIsPlacementParam();
    void bareAddActionFallsBackToDefaults();
    void addThenRemoveKeepsPanelConsistent();
    void runtimePropertyChangeDoesNotMovePlacement();
    void actionsManagerCanCustomizeFlag();
};

namespace {
// 取面板中该 action 对应的 SARibbonPanelItem（无则 nullptr）
SARibbonPanelItem* itemOfAction(SARibbonPanel* panel, QAction* act)
{
    const QList< SARibbonPanelItem* >& items = panel->ribbonPanelItem();
    for (SARibbonPanelItem* i : items) {
        if (i->action == act) {
            return i;
        }
    }
    return nullptr;
}
}  // namespace

/**
 * \if ENGLISH
 * @brief The same QAction added as Large in one panel and Small in another must not
 *        interfere: each panel's item keeps its own row proportion (the old dynamic
 *        property channel overwrote whichever panel wrote last).
 * \endif
 *
 * \if CHINESE
 * @brief 同一QAction在一个面板按Large、另一个面板按Small添加时互不干扰：
 *        各面板的item持有各自的行占比（旧动态属性通道下后写覆盖先写）。
 * \endif
 */
void SARibbonPlacementChannelTest::sameActionTwoPanelsDifferentProportion()
{
    SARibbonPanel panelLarge;
    SARibbonPanel panelSmall;
    QAction act("shared");
    act.setObjectName("shared");

    panelLarge.addAction(&act, SARibbonPanelItem::Large);
    panelSmall.addAction(&act, SARibbonPanelItem::Small);

    SARibbonPanelItem* itemLarge = itemOfAction(&panelLarge, &act);
    SARibbonPanelItem* itemSmall = itemOfAction(&panelSmall, &act);
    QVERIFY(itemLarge != nullptr);
    QVERIFY(itemSmall != nullptr);
    QCOMPARE(int(itemLarge->rowProportion), int(SARibbonPanelItem::Large));
    QCOMPARE(int(itemSmall->rowProportion), int(SARibbonPanelItem::Small));

    // Large 渲染为大按钮类型，Small 渲染为小按钮类型
    SARibbonToolButton* btnLarge = qobject_cast< SARibbonToolButton* >(itemLarge->widget());
    SARibbonToolButton* btnSmall = qobject_cast< SARibbonToolButton* >(itemSmall->widget());
    QVERIFY(btnLarge != nullptr);
    QVERIFY(btnSmall != nullptr);
    QCOMPARE(int(btnLarge->buttonType()), int(SARibbonToolButton::LargeButton));
    QCOMPARE(int(btnSmall->buttonType()), int(SARibbonToolButton::SmallButton));

    // action 上不再残留任何 _sa_* 放置属性
    QVERIFY(!act.property("_sa_RowProportion").isValid());
    QVERIFY(!act.property("_sa_ToolButtonPopupMode").isValid());
    QVERIFY(!act.property("_sa_ToolButtonStyle").isValid());
}

/**
 * \if ENGLISH
 * @brief The popup mode passed to addAction is applied to the created button and kept
 *        on the item; it is not written to the action.
 * \endif
 *
 * \if CHINESE
 * @brief addAction 传入的弹出模式作用于所创建的按钮并保存在item上；不会写到action上。
 * \endif
 */
void SARibbonPlacementChannelTest::popupModeIsPlacementParam()
{
    SARibbonPanel panelA;
    SARibbonPanel panelB;
    QAction act("popup");
    act.setObjectName("popup");

    panelA.addAction(&act, QToolButton::MenuButtonPopup, SARibbonPanelItem::Small);
    panelB.addAction(&act);  // 无 popupMode 重载：InstantPopup 默认

    SARibbonPanelItem* itemA = itemOfAction(&panelA, &act);
    SARibbonPanelItem* itemB = itemOfAction(&panelB, &act);
    QVERIFY(itemA != nullptr);
    QVERIFY(itemB != nullptr);
    QCOMPARE(int(itemA->popupMode), int(QToolButton::MenuButtonPopup));
    QCOMPARE(int(itemB->popupMode), int(QToolButton::InstantPopup));

    SARibbonToolButton* btnA = qobject_cast< SARibbonToolButton* >(itemA->widget());
    QVERIFY(btnA != nullptr);
    QCOMPARE(int(btnA->popupMode()), int(QToolButton::MenuButtonPopup));
    QVERIFY(!act.property("_sa_ToolButtonPopupMode").isValid());
}

/**
 * \if ENGLISH
 * @brief An action entering the panel through a bare QWidget::addAction (not via the
 *        convenience calls) gets the documented defaults: Large / InstantPopup.
 * \endif
 *
 * \if CHINESE
 * @brief 经裸 QWidget::addAction（非便捷方法）进入面板的action获得文档化默认值：
 *        Large / InstantPopup。
 * \endif
 */
void SARibbonPlacementChannelTest::bareAddActionFallsBackToDefaults()
{
    SARibbonPanel panel;
    QAction act("bare");
    act.setObjectName("bare");

    panel.QWidget::addAction(&act);

    SARibbonPanelItem* item = itemOfAction(&panel, &act);
    QVERIFY(item != nullptr);
    QCOMPARE(int(item->rowProportion), int(SARibbonPanelItem::Large));
    QCOMPARE(int(item->popupMode), int(QToolButton::InstantPopup));
}

/**
 * \if ENGLISH
 * @brief "add then immediately remove" and "consecutive convenience adds" keep the
 *        panel consistent (pending table entries are consumed exactly once and do
 *        not leak to later adds).
 * \endif
 *
 * \if CHINESE
 * @brief "添加后立刻移除"与"连续便捷添加"保持面板一致（pending 表条目恰好消费一次，
 *        不泄漏到后续添加）。
 * \endif
 */
void SARibbonPlacementChannelTest::addThenRemoveKeepsPanelConsistent()
{
    SARibbonPanel panel;
    QAction doomed("doomed");
    doomed.setObjectName("doomed");
    QAction survivor("survivor");
    survivor.setObjectName("survivor");

    panel.addAction(&doomed, SARibbonPanelItem::Small);
    panel.QWidget::removeAction(&doomed);
    QCOMPARE(itemOfAction(&panel, &doomed), nullptr);
    // pending 表里 doomed 的记录已随 ActionAdded 消费，不会影响后续添加
    panel.addAction(&survivor, SARibbonPanelItem::Medium);
    SARibbonPanelItem* item = itemOfAction(&panel, &survivor);
    QVERIFY(item != nullptr);
    QCOMPARE(int(item->rowProportion), int(SARibbonPanelItem::Medium));
    QCOMPARE(panel.ribbonPanelItem().count(), 1);
}

/**
 * \if ENGLISH
 * @brief Writing `_sa_RowProportion` on the action at runtime and firing ActionChanged
 *        must not move the button's row: the item owns its placement now.
 * \endif
 *
 * \if CHINESE
 * @brief 运行期在action上写 `_sa_RowProportion` 并触发 ActionChanged 不得移动按钮行位：
 *        放置归item所有。
 * \endif
 */
void SARibbonPlacementChannelTest::runtimePropertyChangeDoesNotMovePlacement()
{
    SARibbonPanel panel;
    QAction act("frozen");
    act.setObjectName("frozen");

    panel.addAction(&act, SARibbonPanelItem::Small);
    // 先强制一次布局，拿到基准行位
    panel.resize(QSize(120, 90));
    panel.show();
    QApplication::processEvents();
    SARibbonPanelItem* item = itemOfAction(&panel, &act);
    QVERIFY(item != nullptr);
    const int rowBefore      = item->rowIndex;
    const int rowProportionBefore = int(item->rowProportion);
    QVERIFY(rowBefore >= 0);

    // 旧通道语义：外部改属性 + action 变更会改变后续布局读取结果；新通道忽略之
    act.setProperty("_sa_RowProportion", int(SARibbonPanelItem::Large));
    act.setText("frozen-2");  // triggers ActionChanged path
    QApplication::processEvents();

    item = itemOfAction(&panel, &act);
    QVERIFY(item != nullptr);
    QCOMPARE(int(item->rowProportion), rowProportionBefore);
    QCOMPARE(item->rowIndex, rowBefore);
    panel.hide();
}

/**
 * \if ENGLISH
 * @brief The command-level canCustomize flag lives in SARibbonActionsManager
 *        (plan-07 S3) and follows the registration lifecycle.
 * \endif
 *
 * \if CHINESE
 * @brief 命令级 canCustomize 标记由 SARibbonActionsManager 持有（计划 07 S3），
 *        并随注册生命周期存亡。
 * \endif
 */
void SARibbonPlacementChannelTest::actionsManagerCanCustomizeFlag()
{
    SARibbonActionsManager mgr(nullptr);
    QAction act("flag");
    act.setObjectName("flag");

    QVERIFY(!mgr.isCanCustomize(&act));
    QVERIFY(mgr.registeAction(&act, SARibbonActionsManager::CommonlyUsedActionTag));
    QVERIFY(!mgr.isCanCustomize(&act));
    mgr.setCanCustomize(&act);
    QVERIFY(mgr.isCanCustomize(&act));
    mgr.setCanCustomize(&act, false);
    QVERIFY(!mgr.isCanCustomize(&act));

    // 未注册的action也可标记（如"面板内可移除但不进添加列表"的用法），
    // 但标记不等于注册——不进入管理器的 tag/key 体系
    QAction stranger("stranger");
    stranger.setObjectName("stranger");
    QVERIFY(!mgr.isCanCustomize(&stranger));
    mgr.setCanCustomize(&stranger);
    QVERIFY(mgr.isCanCustomize(&stranger));
    QVERIFY(mgr.key(&stranger).isEmpty());

    // 取消注册后标记一并清除
    mgr.setCanCustomize(&act);
    mgr.unregisteAction(&act);
    QVERIFY(!mgr.isCanCustomize(&act));
}

QTEST_MAIN(SARibbonPlacementChannelTest)

#include "SARibbonPlacementChannelTest.moc"
