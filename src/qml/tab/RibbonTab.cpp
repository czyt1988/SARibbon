#include "RibbonTab.h"

namespace SARibbonQml {

RibbonTab::RibbonTab(QQuickItem* parent) : QQuickItem(parent)
{
    setAcceptedMouseButtons(Qt::LeftButton);
}

QString RibbonTab::text() const
{
    return mText;
}

void RibbonTab::setText(const QString& t)
{
    if (mText == t) {
        return;
    }
    mText = t;
    Q_EMIT textChanged();
}

bool RibbonTab::isCurrent() const
{
    return mCurrent;
}

void RibbonTab::setCurrent(bool c)
{
    if (mCurrent == c) {
        return;
    }
    mCurrent = c;
    Q_EMIT currentChanged();
}

QColor RibbonTab::contextColor() const
{
    return mContextColor;
}

void RibbonTab::setContextColor(const QColor& c)
{
    if (mContextColor == c) {
        return;
    }
    mContextColor = c;
    Q_EMIT contextColorChanged();
}

QQuickItem* RibbonTab::tabQmlItem() const
{
    return mTabQmlItem;
}

void RibbonTab::setTabQmlItem(QQuickItem* item)
{
    if (mTabQmlItem == item) {
        return;
    }
    mTabQmlItem = item;
    Q_EMIT tabQmlItemChanged();
}

void RibbonTab::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    Q_EMIT clicked();
}

}
