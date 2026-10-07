#include "SARibbonQmlGalleryItem.h"
#include "SARibbonQmlAction.h"
#include "SARibbonQmlIconProvider.h"

namespace SARibbonQml {

RibbonGalleryItem::RibbonGalleryItem(QObject* parent) : QObject(parent)
{
}

QAction* RibbonGalleryItem::action() const
{
    return mAction;
}

void RibbonGalleryItem::setAction(QAction* act)
{
    if (mAction == act) {
        return;
    }
    if (mAction) {
        QObject::disconnect(mAction, &QAction::changed, this, &RibbonGalleryItem::onActionChanged);
    }
    mAction = act;
    if (mAction) {
        QObject::connect(mAction, &QAction::changed, this, &RibbonGalleryItem::onActionChanged);
        onActionChanged();
    }
    Q_EMIT actionChanged();
}

void RibbonGalleryItem::onActionChanged()
{
    if (!mAction) {
        return;
    }
    if (mText != mAction->text()) {
        mText = mAction->text();
        Q_EMIT textChanged();
    }
    QString icon;
    if (auto* ribbonAction = qobject_cast< RibbonAction* >(mAction)) {
        icon = ribbonAction->iconSource().toString();
    } else if (!mAction->icon().isNull()) {
        icon = SAIconImageProvider::iconUrl(mAction).toString();
    }
    if (mIconSource != icon) {
        mIconSource = icon;
        Q_EMIT iconSourceChanged();
    }
    if (mEnabled != mAction->isEnabled()) {
        mEnabled = mAction->isEnabled();
        Q_EMIT enabledChanged();
    }
}

QString RibbonGalleryItem::text() const
{
    return mText;
}

void RibbonGalleryItem::setText(const QString& t)
{
    if (mText == t) {
        return;
    }
    mText = t;
    Q_EMIT textChanged();
    if (mAction) {
        mAction->setText(t);  // write-through: the action is the authority
    }
}

QString RibbonGalleryItem::iconSource() const
{
    return mIconSource;
}

void RibbonGalleryItem::setIconSource(const QString& s)
{
    if (mIconSource == s) {
        return;
    }
    mIconSource = s;
    Q_EMIT iconSourceChanged();
}

bool RibbonGalleryItem::isEnabled() const
{
    return mEnabled;
}

void RibbonGalleryItem::setEnabled(bool on)
{
    if (mEnabled == on) {
        return;
    }
    mEnabled = on;
    Q_EMIT enabledChanged();
}

QString RibbonGalleryItem::toolTip() const
{
    return mToolTip;
}

void RibbonGalleryItem::setToolTip(const QString& t)
{
    if (mToolTip == t) {
        return;
    }
    mToolTip = t;
    Q_EMIT toolTipChanged();
}

bool RibbonGalleryItem::isSelectable() const
{
    return mSelectable;
}

/**
 * \if ENGLISH
 * @brief Set whether the cell may become the gallery's current item
 * @details Qt::ItemIsSelectable parity. Clearing it while the gallery marks
 *          this very item as current must also clear that mark, which is why
 *          the gallery host watches selectableChanged (the widgets side gets
 *          the same effect from its selection model refusing a non-selectable
 *          index). Activation is deliberately NOT gated here.
 * \endif
 *
 * \if CHINESE
 * @brief 设置该单元能否成为画廊的当前项
 * @details 对应 Qt::ItemIsSelectable。若画廊此刻正把该项标为当前项，清除可选择性
 *          必须连带清除该标记，因此画廊宿主监听 selectableChanged（widgets 侧由
 *          选择模型拒绝不可选择索引得到同样效果）。激活刻意不受此标志约束。
 * \endif
 */
void RibbonGalleryItem::setSelectable(bool on)
{
    if (mSelectable == on) {
        return;
    }
    mSelectable = on;
    Q_EMIT selectableChanged();
}

}
