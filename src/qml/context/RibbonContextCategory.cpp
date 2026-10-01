#include "RibbonContextCategory.h"
#include "../category/RibbonCategory.h"

namespace SARibbonQml {

RibbonContextCategory::RibbonContextCategory(QQuickItem* parent) : QQuickItem(parent)
{
    // structural container: no visuals of its own, pages render through the
    // bar layout; keep it out of hit testing
    setAcceptedMouseButtons(Qt::NoButton);
}

RibbonContextCategory::~RibbonContextCategory()
{
}

QString RibbonContextCategory::contextTitle() const
{
    return mContextTitle;
}

void RibbonContextCategory::setContextTitle(const QString& t)
{
    if (mContextTitle == t) {
        return;
    }
    mContextTitle = t;
    Q_EMIT contextTitleChanged();
}

QColor RibbonContextCategory::contextColor() const
{
    return mContextColor;
}

void RibbonContextCategory::setContextColor(const QColor& c)
{
    if (mContextColor == c) {
        return;
    }
    mContextColor = c;
    Q_EMIT contextColorChanged();
}

bool RibbonContextCategory::isActive() const
{
    return mActive;
}

void RibbonContextCategory::setActive(bool on)
{
    if (mActive == on) {
        return;
    }
    mActive = on;
    Q_EMIT activeChanged();
}

QVector< RibbonCategory* > RibbonContextCategory::categories() const
{
    return mCategories;
}

void RibbonContextCategory::registerCategory(RibbonCategory* c)
{
    if (c && !mCategories.contains(c)) {
        mCategories.append(c);
        Q_EMIT categoryPagesChanged();
    }
}

void RibbonContextCategory::unregisterCategory(RibbonCategory* c)
{
    if (mCategories.removeOne(c)) {
        Q_EMIT categoryPagesChanged();
    }
}

void RibbonContextCategory::itemChange(ItemChange change, const ItemChangeData& data)
{
    if (change == QQuickItem::ItemChildAddedChange) {
        // declaration order (sibling componentComplete runs reversed)
        if (RibbonCategory* c = qobject_cast< RibbonCategory* >(data.item)) {
            registerCategory(c);
        }
    } else if (change == QQuickItem::ItemChildRemovedChange) {
        if (RibbonCategory* c = qobject_cast< RibbonCategory* >(data.item)) {
            unregisterCategory(c);
        }
    }
    QQuickItem::itemChange(change, data);
}

}
