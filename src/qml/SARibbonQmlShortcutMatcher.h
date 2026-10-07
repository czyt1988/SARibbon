#ifndef SARIBBONQMLSHORTCUTMATCHER_H
#define SARIBBONQMLSHORTCUTMATCHER_H
#include "SARibbonQmlGlobal.h"
#include <QPointer>
#include <QQuickItem>
#include <QUrl>

class QQuickWindow;
class QAction;

namespace SARibbonQml {

/**
 * \if ENGLISH
 * @brief Bar-level shortcut fallback (plan-05 S5, contract D8-1)
 * @details S0-V2 verdict: a QAction shortcut does not auto-trigger inside a
 *          QQuickWindow scene (the key was confirmed delivered to the focused
 *          item; the orphan action never fired), and it never does on the
 *          Qt5 lane either — so the matcher is the single code path on both
 *          lanes (no behavior fork, contract D5 discipline 1). It filters the
 *          bar's QQuickWindow: a key press matching a collected
 *          (action, QKeySequence) triggers the action — enabled and visible
 *          ones only, single-step sequences only. The window receives the key
 *          before the delivery agent runs, so consuming it on a match also
 *          preempts the focus item and any QML `Shortcut` on the same key
 *          (the action wins; unmatched keys flow untouched). Actions not
 *          bound to any collected button are ignored — the Quick-scope
 *          approximation of Qt::WindowShortcut semantics.
 * \endif
 *
 * \if CHINESE
 * @brief bar 级快捷键回退（计划 05 S5，契约 D8-1）
 * @details S0-V2 结论：QAction 快捷键在 QQuickWindow 场景不会自动触发（已确认
 *          按键送达了焦点 item，孤儿 action 从未发射），Qt5 车道同样不触发——
 *          因此匹配器是两条车道共用的同一条代码路径（无行为分叉，契约 D5 纪律
 *          1）。它经事件过滤盯住窗口 contentItem：ShortcutOverride 按键命中收集
 *          到的 (action, QKeySequence) 即触发该 action——仅限启用且可见者。bar
 *          在按钮 attach/detach 与 action 属性变化时维护收集表；未绑定到任何
 *          可见按钮的 action 被忽略，这是 Qt::WindowShortcut 语义的 Quick 域
 *          近似，同时避免与 QML `Shortcut` 类型双触发（仅在命中时消费按键）。
 * \endif
 */
class SA_RIBBON_QML_EXPORT RibbonShortcutMatcher : public QObject
{
    Q_OBJECT
public:
    explicit RibbonShortcutMatcher(QObject* parent = nullptr);
    ~RibbonShortcutMatcher() override;

    // Watch the window of the bar item (idempotent; no-op without a window)
    void watch(RibbonQuickHost* bar);

    // Stop watching and clear the collected table
    void unwatch();

    // ---- collection maintenance (called by the action-bound buttons) ----
    // An action with an empty/ambiguous shortcut is skipped. Idempotent per
    // (button, action); the sequence is re-read from the action on every
    // QAction::changed so a runtime rebind reaches the table
    void collectAction(QObject* button, QAction* action);
    // Drop the button's collection (button detach/destroy/action clear)
    void forgetButton(QObject* button);

    // All currently collected actions (registry/serializer aid)
    QList< QAction* > collectedActions() const;

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    void rebuildMap();

    // QPointer: the window may die before the bar (e.g. a QQuickView stack
    // local destroyed ahead of the content root) and itemChange does not
    // notify on window destruction — the guard makes the teardown order-safe
    QPointer< QQuickWindow > mWindow;
    // owner button -> watched action (QPointer pair: either side may die)
    QHash< QObject*, QPointer< QAction > > mButtonToAction;
    // shortcut match table: QKeySequence::toString -> live actions (rebuilt)
    QMultiHash< QString, QPointer< QAction > > mSeqToActions;
};

}
#endif  // SARIBBONQMLSHORTCUTMATCHER_H
