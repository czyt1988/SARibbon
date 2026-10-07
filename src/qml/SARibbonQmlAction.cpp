#include "SARibbonQmlAction.h"
#include "SARibbonQmlIconProvider.h"
#include <QIcon>
#include <QKeySequence>
#include <QPixmap>
#include <QSet>
#include <QVariant>

namespace SARibbonQml {

RibbonAction::RibbonAction(QObject* parent) : QAction(parent)
{
    // QAction::changed carries no field hint; the derived convenience notifies
    // are re-emitted coarsely off it (correctness over notify granularity —
    // the plan-05 risk table accepts redundant refreshes)
    connect(this, &QAction::changed, this, &RibbonAction::syncNotifies);
}

QUrl RibbonAction::iconSource() const
{
    return mIconSource;
}

/**
 * \if ENGLISH
 * @brief Mirror the url into QAction::icon
 * @details The url is turned into a QIcon only when non-empty and resolvable
 *          to a local file; clearing it clears the icon. QAction::setIcon
 *          stays the single authority widgets consumers read.
 * \endif
 *
 * \if CHINESE
 * @brief 把 url 镜像进 QAction::icon
 * @details 仅当 url 非空且可解析为本地文件时构建 QIcon；清空 url 即清空图标。
 *          QAction::setIcon 保持 widgets 消费方读取的唯一权威。
 * \endif
 */
void RibbonAction::setIconSource(const QUrl& url)
{
    if (mIconSource == url) {
        return;
    }
    mIconSource = url;
    if (url.isEmpty()) {
        QAction::setIcon(QIcon());
    } else {
        // url forms the leaf Image understands directly; the QIcon mirror is
        // only possible for resources and local files (remote urls would need
        // async loading, out of scope for the widgets-world mirror)
        QString path;
        if (url.scheme() == QLatin1String("qrc")) {
            path = QLatin1Char(':') + url.path();
        } else if (url.isLocalFile()) {
            path = url.toLocalFile();
        } else {
            path = url.toString(QUrl::PreferLocalFile);
        }
        QAction::setIcon(QIcon(path));
    }
    Q_EMIT iconSourceChanged();
}

QString RibbonAction::shortcutText() const
{
    return QAction::shortcut().toString(QKeySequence::PortableText);
}

void RibbonAction::setShortcutText(const QString& text)
{
    const QKeySequence seq(text);
    if (QAction::shortcut() == seq) {
        return;
    }
    QAction::setShortcut(seq);
    Q_EMIT shortcutTextChanged();
}

bool RibbonAction::isSeparator() const
{
    return QAction::isSeparator();
}

void RibbonAction::setSeparator(bool on)
{
    if (QAction::isSeparator() == on) {
        return;
    }
    QAction::setSeparator(on);
    Q_EMIT separatorChanged();
}

QQmlListProperty< RibbonAction > RibbonAction::menuActions()
{
    return QQmlListProperty< RibbonAction >(this, this, &RibbonAction::appendMenuActionCb, &RibbonAction::menuActionCountCb,
                                            &RibbonAction::menuActionAtCb, &RibbonAction::clearMenuActionsCb);
}

int RibbonAction::menuActionCount() const
{
    return mMenuActions.size();
}

QAction* RibbonAction::menuActionAt(int index) const
{
    return (index >= 0 && index < mMenuActions.size()) ? mMenuActions[ index ] : nullptr;
}

void RibbonAction::addMenuAction(QAction* action)
{
    if (action && !mMenuActions.contains(action)) {
        mMenuActions.append(action);
        // review P1-1: a destroyed submenu command must leave the list
        // instead of leaving a dangling pointer menuActionAt would hand out
        connect(action, &QObject::destroyed, this, [this](QObject* gone) {
            if (mMenuActions.removeAll(static_cast< QAction* >(gone)) > 0) {
                Q_EMIT menuActionsChanged();
            }
        });
        Q_EMIT menuActionsChanged();
    }
}

QVariantList RibbonAction::menuActionList() const
{
    QVariantList res;
    for (QAction* a : mMenuActions) {
        res.append(QVariant::fromValue(a));
    }
    return res;
}

void RibbonAction::appendMenuActionCb(QQmlListProperty< RibbonAction >* prop, RibbonAction* action)
{
    auto* self = static_cast< RibbonAction* >(prop->data);
    if (self && action) {
        self->addMenuAction(action);
    }
}

RibbonAction::ListIndex RibbonAction::menuActionCountCb(QQmlListProperty< RibbonAction >* prop)
{
    auto* self = static_cast< RibbonAction* >(prop->data);
    return self ? self->mMenuActions.size() : ListIndex(0);
}

RibbonAction* RibbonAction::menuActionAtCb(QQmlListProperty< RibbonAction >* prop, ListIndex index)
{
    // AtFunction must return the element type; a C++-appended bare QAction is
    // not reachable through the QML view of the list (it stays reachable
    // through menuActionAt), nullptr is the honest answer for it
    auto* self = static_cast< RibbonAction* >(prop->data);
    return self ? qobject_cast< RibbonAction* >(self->menuActionAt(int(index))) : nullptr;
}

void RibbonAction::clearMenuActionsCb(QQmlListProperty< RibbonAction >* prop)
{
    auto* self = static_cast< RibbonAction* >(prop->data);
    if (self && !self->mMenuActions.isEmpty()) {
        self->mMenuActions.clear();
        Q_EMIT self->menuActionsChanged();
    }
}

void RibbonAction::syncNotifies()
{
    // shortcutText reads QAction::shortcut, so a C++ setShortcut write must
    // reach QML bindings through the convenience notify as well
    Q_EMIT shortcutTextChanged();
}

// ---- RibbonMenuModel ----
RibbonMenuModel::RibbonMenuModel(QObject* parent) : QObject(parent)
{
}

RibbonMenuModel::~RibbonMenuModel()
{
}

void RibbonMenuModel::setActions(const QVariantList& actions)
{
    if (mActions == actions) {
        return;
    }
    mActions = actions;
    rewatch();
    rebuild();
}

const QVariantList& RibbonMenuModel::actions() const
{
    return mActions;
}

QVariantList RibbonMenuModel::rows() const
{
    return mRows;
}

/**
 * \if ENGLISH
 * @brief Resolve an activation index path
 * @details The leaf publishes [row] at the top level and one more element per
 *          nesting level. The walk descends through RibbonAction.menuActions
 *          of the addressed entry; separators, disabled entries and bad
 *          indices resolve to nullptr — the old menu-entry refusals, now
 *          on the command objects themselves.
 * \endif
 *
 * \if CHINESE
 * @brief 解析激活索引路径
 * @details 叶子发布 [行]，嵌套层级每层追加一个元素。解析沿被指向条目的
 *          RibbonAction.menuActions 下行；分隔符、禁用条目与非法下标解析为
 *          nullptr——旧的菜单项拒绝逻辑，现在落在命令对象本身上。
 * \endif
 */
QAction* RibbonMenuModel::resolvePath(const QVariantList& indexPath) const
{
    if (indexPath.isEmpty()) {
        return nullptr;
    }
    QVariantList level = mActions;
    QAction* act = nullptr;
    for (int i = 0; i < indexPath.size(); ++i) {
        const QVariant& step = indexPath.at(i);
        // strict numeric acceptance: the 2.x path resolver refused
        // non-numeric elements, and QVariant::toInt would silently coerce
        // "x" to 0 — a malformed path must stay refused, not address the
        // first entry (the leaf only ever publishes numeric paths anyway)
        if (QMetaType::Type stepType = static_cast< QMetaType::Type >(step.type());
            stepType != QMetaType::Int && stepType != QMetaType::UInt && stepType != QMetaType::Double
            && stepType != QMetaType::LongLong && stepType != QMetaType::ULongLong) {
            return nullptr;
        }
        const int idx = step.toInt();
        if (idx < 0 || idx >= level.size()) {
            return nullptr;
        }
        act = level.at(idx).value< QAction* >();
        if (!act) {
            return nullptr;
        }
        if (i + 1 < indexPath.size()) {
            auto* ribbonAction = qobject_cast< RibbonAction* >(act);
            if (!ribbonAction) {
                return nullptr;  // a bare QAction has no submenu (contract §5)
            }
            level = ribbonAction->menuActionList();
        }
    }
    if (act->isSeparator() || !act->isEnabled()) {
        return nullptr;
    }
    return act;
}

void RibbonMenuModel::rewatch()
{
    for (const QMetaObject::Connection& c : mWatched) {
        QObject::disconnect(c);
    }
    mWatched.clear();
    // collect every action of the tree (top level + nested menuActions) and
    // rebuild on any change; a destroyed action drops out through the same
    // changed()-less path and the next rebuild skips it
    QVector< QAction* > pending;
    for (const QVariant& v : mActions) {
        if (QAction* a = v.value< QAction* >()) {
            pending.append(a);
        }
    }
    QSet< QAction* > seen;
    while (!pending.isEmpty()) {
        QAction* a = pending.takeFirst();
        if (!a || seen.contains(a)) {
            continue;
        }
        seen.insert(a);
        mWatched.append(connect(a, &QAction::changed, this, &RibbonMenuModel::rebuild));
        // review P1-1: a destroyed command must leave the list, not dangle in
        // it — the changed hook dies with the sender, so destroyed is the only
        // reliable cleanup signal (the Registry onActionDestroyed pattern)
        mWatched.append(connect(a, &QObject::destroyed, this, [this](QObject* gone) { onActionDestroyed(gone); }));
        if (auto* ribbonAction = qobject_cast< RibbonAction* >(a)) {
            for (const QVariant& v : ribbonAction->menuActionList()) {
                if (QAction* sub = v.value< QAction* >()) {
                    pending.append(sub);
                }
            }
        }
    }
}

void RibbonMenuModel::rebuild()
{
    mRows = saRibbonMenuRows(mActions);
    Q_EMIT rowsChanged();
}

void RibbonMenuModel::onActionDestroyed(QObject* gone)
{
    QVariantList kept;
    bool dropped = false;
    for (const QVariant& v : mActions) {
        if (v.value< QObject* >() == gone) {
            dropped = true;
            continue;
        }
        kept.append(v);
    }
    if (dropped) {
        mActions = kept;
        rewatch();
        rebuild();
    }
}

QVariantList saRibbonMenuRows(const QVariantList& actions)
{
    QVariantList rows;
    for (const QVariant& v : actions) {
        QAction* a = v.value< QAction* >();
        if (!a) {
            continue;
        }
        QVariantMap row;
        row.insert(QStringLiteral("text"), a->text());
        row.insert(QStringLiteral("enabled"), a->isEnabled());
        row.insert(QStringLiteral("separator"), a->isSeparator());
        row.insert(QStringLiteral("checkable"), a->isCheckable());
        row.insert(QStringLiteral("checked"), a->isChecked());
        // shortcut caption: derived from the REAL key sequence (the S5 matcher
        // fires it; the display can no longer lie, contract §7)
        row.insert(QStringLiteral("shortcut"), a->shortcut().toString(QKeySequence::PortableText));
        QString icon;
        if (auto* ribbonAction = qobject_cast< RibbonAction* >(a)) {
            icon = ribbonAction->iconSource().toString();
        } else if (!a->icon().isNull()) {
            icon = SAIconImageProvider::iconUrl(a).toString();
        }
        row.insert(QStringLiteral("iconSource"), icon);
        QVariantList submenu;
        if (auto* ribbonAction = qobject_cast< RibbonAction* >(a)) {
            submenu = saRibbonMenuRows(ribbonAction->menuActionList());
        }
        row.insert(QStringLiteral("submenu"), submenu);
        row.insert(QStringLiteral("hasSubmenu"), !submenu.isEmpty());
        rows.append(row);
    }
    return rows;
}

}
