#include "RibbonMenuItem.h"

namespace SARibbonQml {

RibbonMenuItem::RibbonMenuItem(QObject* parent) : QObject(parent)
{
}

QString RibbonMenuItem::text() const
{
    return mText;
}

void RibbonMenuItem::setText(const QString& t)
{
    if (mText == t) {
        return;
    }
    mText = t;
    Q_EMIT textChanged();
}

QString RibbonMenuItem::iconSource() const
{
    return mIconSource;
}

void RibbonMenuItem::setIconSource(const QString& s)
{
    if (mIconSource == s) {
        return;
    }
    mIconSource = s;
    Q_EMIT iconSourceChanged();
}

bool RibbonMenuItem::isEnabled() const
{
    return mEnabled;
}

void RibbonMenuItem::setEnabled(bool on)
{
    if (mEnabled == on) {
        return;
    }
    mEnabled = on;
    Q_EMIT enabledChanged();
}

bool RibbonMenuItem::isSeparator() const
{
    return mSeparator;
}

void RibbonMenuItem::setSeparator(bool on)
{
    if (mSeparator == on) {
        return;
    }
    mSeparator = on;
    Q_EMIT separatorChanged();
}

bool RibbonMenuItem::isCheckable() const
{
    return mCheckable;
}

/**
 * \if ENGLISH
 * @brief Set the checkable flag
 * @details QAction::setCheckable parity: dropping the flag also drops the
 *          checked state, so an entry that stops being checkable cannot keep a
 *          mark the leaf would still render.
 * \endif
 *
 * \if CHINESE
 * @brief 设置可勾选标志
 * @details 对照 QAction::setCheckable：取消可勾选的同时清掉勾选状态，避免不再
 *          可勾选的菜单项还留着一个叶子仍会绘制的标记。
 * \endif
 */
void RibbonMenuItem::setCheckable(bool on)
{
    if (mCheckable == on) {
        return;
    }
    mCheckable = on;
    Q_EMIT checkableChanged();
    if (!on && mChecked) {
        setChecked(false);
    }
}

bool RibbonMenuItem::isChecked() const
{
    return mChecked;
}

void RibbonMenuItem::setChecked(bool on)
{
    if (mChecked == on) {
        return;
    }
    mChecked = on;
    Q_EMIT checkedChanged();
    Q_EMIT toggled(mChecked);
}

QString RibbonMenuItem::shortcut() const
{
    return mShortcut;
}

void RibbonMenuItem::setShortcut(const QString& s)
{
    if (mShortcut == s) {
        return;
    }
    mShortcut = s;
    Q_EMIT shortcutChanged();
}

QQmlListProperty< RibbonMenuItem > RibbonMenuItem::submenu()
{
    return QQmlListProperty< RibbonMenuItem >(this, this, &RibbonMenuItem::appendSubmenuEntry, &RibbonMenuItem::submenuCountCb,
                                              &RibbonMenuItem::submenuItemAtCb, &RibbonMenuItem::clearSubmenu);
}

int RibbonMenuItem::submenuCount() const
{
    return mSubmenu.size();
}

RibbonMenuItem* RibbonMenuItem::submenuItemAt(int index) const
{
    return (index >= 0 && index < mSubmenu.size()) ? mSubmenu[ index ] : nullptr;
}

bool RibbonMenuItem::hasSubmenu() const
{
    return !mSubmenu.isEmpty();
}

/**
 * \if ENGLISH
 * @brief Run the activation side effects of this entry
 * @details A checkable entry flips its own state here — the widgets front end
 *          gets that from QAction::trigger, the QML front end has no action
 *          bridge so the host calls this while resolving the activation path.
 *          Guarding on isEnabled()/isSeparator() stays with the caller, which
 *          owns the tree and therefore the refusal semantics.
 * \endif
 *
 * \if CHINESE
 * @brief 执行本菜单项被激活时的副作用
 * @details 可勾选的菜单项在此翻转自身状态——widgets 前端从 QAction::trigger 白拿到
 *          这一行为，QML 前端没有 action 桥，故由宿主在解析激活路径时调用本函数。
 *          isEnabled()/isSeparator() 的拦截留给调用方：调用方持有整棵树，也因而
 *          持有拒绝语义。
 * \endif
 */
void RibbonMenuItem::activate()
{
    if (mCheckable) {
        setChecked(!mChecked);
    }
    Q_EMIT triggered();
}

/**
 * \if ENGLISH
 * @brief Walk an index path down to the entry it addresses
 * @details The visual leaf publishes one path per activation ([row] for a
 *          top-level entry, [row, subrow, ...] for a nested one) so the host
 *          stays the single place that knows the tree. Every step must be an
 *          integer convertible index inside the current level and the level
 *          itself must be a submenu of the previous entry; anything else yields
 *          nullptr, which the callers already treat as "ignore" (the flat
 *          activateMenuItem(int) out-of-range contract).
 * \endif
 *
 * \if CHINESE
 * @brief 按索引路径解析到目标菜单项
 * @details 视觉叶子每次激活只发布一条路径（顶层项为 [row]，嵌套项为
 *          [row, subrow, ...]），因此"认识整棵树"的地方仍然只有宿主一个。每一步
 *          都必须是可转成整数的下标、落在当前层范围内，且该层必须是上一步菜单项的
 *          submenu；否则返回 nullptr——调用方本就把它当"忽略"处理（与
 *          activateMenuItem(int) 的越界契约一致）。
 * \endif
 */
RibbonMenuItem* RibbonMenuItem::resolvePath(const QVector< RibbonMenuItem* >& roots, const QVariantList& indexPath)
{
    if (indexPath.isEmpty()) {
        return nullptr;
    }
    const QVector< RibbonMenuItem* >* level = &roots;
    RibbonMenuItem* current                 = nullptr;
    for (int i = 0; i < indexPath.size(); ++i) {
        bool ok   = false;
        const int idx = indexPath.at(i).toInt(&ok);
        if (!ok || !level || idx < 0 || idx >= level->size()) {
            return nullptr;
        }
        current = level->at(idx);
        if (!current) {
            return nullptr;
        }
        // the next level is this entry's submenu; owning the vector keeps the
        // pointer valid for the following iteration
        level = &current->mSubmenu;
    }
    return current;
}

// ---- submenu list property callbacks ----
void RibbonMenuItem::appendSubmenuEntry(QQmlListProperty< RibbonMenuItem >* prop, RibbonMenuItem* item)
{
    auto* self = static_cast< RibbonMenuItem* >(prop->data);
    if (self && item && item != self && !self->mSubmenu.contains(item)) {
        self->mSubmenu.append(item);
        item->setParent(self);
        self->emitSubmenuChanged();
    }
}

RibbonMenuItem::ListIndex RibbonMenuItem::submenuCountCb(QQmlListProperty< RibbonMenuItem >* prop)
{
    auto* self = static_cast< RibbonMenuItem* >(prop->data);
    return self ? self->mSubmenu.size() : RibbonMenuItem::ListIndex(0);
}

RibbonMenuItem* RibbonMenuItem::submenuItemAtCb(QQmlListProperty< RibbonMenuItem >* prop, ListIndex index)
{
    auto* self = static_cast< RibbonMenuItem* >(prop->data);
    return self ? self->submenuItemAt(int(index)) : nullptr;
}

void RibbonMenuItem::clearSubmenu(QQmlListProperty< RibbonMenuItem >* prop)
{
    auto* self = static_cast< RibbonMenuItem* >(prop->data);
    if (self && !self->mSubmenu.isEmpty()) {
        self->mSubmenu.clear();
        self->emitSubmenuChanged();
    }
}

void RibbonMenuItem::emitSubmenuChanged()
{
    Q_EMIT submenuChanged();
}

}
