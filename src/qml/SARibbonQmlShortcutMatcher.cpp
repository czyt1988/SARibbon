#include "SARibbonQmlShortcutMatcher.h"
#include "SARibbonQmlActionCompat.h"
#include "SARibbonQmlQuickHost.h"
#include <QDebug>
#include <QEvent>
#include <QKeyEvent>
#include <QQuickWindow>
#include <QKeySequence>
#include <QPointer>

namespace SARibbonQml {

RibbonShortcutMatcher::RibbonShortcutMatcher(QObject* parent) : QObject(parent)
{
}

RibbonShortcutMatcher::~RibbonShortcutMatcher()
{
    unwatch();
}

void RibbonShortcutMatcher::watch(RibbonQuickHost* bar)
{
    if (nullptr == bar) {
        return;
    }
    QQuickWindow* win = bar->window();
    if (win == mWindow) {
        return;
    }
    unwatch();
    if (nullptr == win) {
        return;
    }
    mWindow = win;
    // QQuickWindow receives the key press before its delivery agent runs, so
    // consuming the event here preempts every later stage (focus item, QML
    // Shortcut) — the "no double trigger with QML Shortcut" rule is simply
    // "the action wins on a match; everything else flows untouched"
    win->installEventFilter(this);
}

void RibbonShortcutMatcher::unwatch()
{
    // only stops the event filtering — the button collection is independent
    // of the window: buttons fill it during componentComplete, which happens
    // BEFORE the bar enters a window, so clearing here would wipe everything
    // exactly when the bar is about to start watching
    if (mWindow) {
        mWindow->removeEventFilter(this);
        mWindow = nullptr;
    }
}

void RibbonShortcutMatcher::collectAction(QObject* button, QAction* action)
{
    if (nullptr == button || nullptr == action) {
        return;
    }
    QPointer< QAction > ptr(action);
    auto it = mButtonToAction.find(button);
    if (it != mButtonToAction.end() && it.value() == ptr) {
        return;
    }
    if (it != mButtonToAction.end()) {
        it.value() = ptr;
    } else {
        mButtonToAction.insert(button, ptr);
        // a dead button must not keep the collection alive
        QObject::connect(button, &QObject::destroyed, this, [this](QObject* dead) {
            forgetButton(dead);
        });
    }
    // a shortcut rebind on the action must reach the match table
    QObject::connect(action, &QAction::changed, this, [this]() { rebuildMap(); });
    rebuildMap();
}

void RibbonShortcutMatcher::forgetButton(QObject* button)
{
    // Qt5 remove() returns int, Qt6 returns bool — the plain truth test
    // compiles warning-free on both lanes
    if (mButtonToAction.remove(button)) {
        rebuildMap();
    }
}

QList< QAction* > RibbonShortcutMatcher::collectedActions() const
{
    QList< QAction* > res;
    for (auto it = mButtonToAction.cbegin(); it != mButtonToAction.cend(); ++it) {
        if (it.value()) {
            res.append(it.value().data());
        }
    }
    return res;
}

bool RibbonShortcutMatcher::eventFilter(QObject* watched, QEvent* event)
{
    if (QEvent::KeyPress != event->type() || watched != mWindow) {
        return QObject::eventFilter(watched, event);
    }
    auto keyEvent = static_cast< QKeyEvent* >(event);
    if (keyEvent->isAutoRepeat()) {
        return false;  // QShortcut parity: auto-repeat does not re-trigger
    }
    // drop dead collections first so a destroyed action cannot match
    if (mSeqToActions.isEmpty()) {
        return false;
    }
    const int key = keyEvent->key() | (int(keyEvent->modifiers()) & ~int(Qt::KeypadModifier));
    if (key == 0) {
        return false;
    }
    const QString seqText = QKeySequence(key).toString();
    if (seqText.isEmpty()) {
        return false;
    }
    const bool matchedAll = mSeqToActions.contains(seqText);
    if (!matchedAll) {
        return false;
    }
    QAction* triggered = nullptr;
    for (auto it = mSeqToActions.constFind(seqText); it != mSeqToActions.cend() && it.key() == seqText; ++it) {
        QAction* candidate = it.value().data();
        if (nullptr == candidate) {
            continue;
        }
        const Qt::ShortcutContext ctx = candidate->shortcutContext();
        if (Qt::WidgetShortcut == ctx || Qt::WidgetWithChildrenShortcut == ctx) {
            continue;  // widget-scoped semantics have no Quick meaning here
        }
        if (!candidate->isEnabled() || !candidate->isVisible()) {
            continue;
        }
        if (nullptr == triggered) {
            triggered = candidate;  // first live match wins (no ambiguity engine)
        }
    }
    if (nullptr == triggered) {
        return false;
    }
    triggered->trigger();
    keyEvent->accept();  // consume: neither items nor QML Shortcut see this key
    return true;
}

void RibbonShortcutMatcher::rebuildMap()
{
    mSeqToActions.clear();
    for (auto it = mButtonToAction.cbegin(); it != mButtonToAction.cend(); ++it) {
        QAction* action = it.value().data();
        if (nullptr == action) {
            continue;
        }
        const QKeySequence seq = action->shortcut();
        if (seq.count() != 1) {
            continue;  // multi-step sequences are out of the fallback scope
        }
        const QString text = seq.toString();
        if (!text.isEmpty()) {
            mSeqToActions.insert(text, it.value());
        }
    }
}

}  // namespace SARibbonQml
