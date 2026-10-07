#include "SARibbonQmlAction.h"
#include <QIcon>
#include <QKeySequence>
#include <QPixmap>
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

QVariantList RibbonAction::menuActions() const
{
    return mMenuActions;
}

void RibbonAction::setMenuActions(const QVariantList& actions)
{
    if (mMenuActions == actions) {
        return;
    }
    mMenuActions = actions;
    Q_EMIT menuActionsChanged();
}

void RibbonAction::syncNotifies()
{
    // shortcutText reads QAction::shortcut, so a C++ setShortcut write must
    // reach QML bindings through the convenience notify as well
    Q_EMIT shortcutTextChanged();
}

}
