#include "RibbonTab.h"
#include "../SARibbonQmlTypes.h"

namespace SARibbonQml {

RibbonTab::RibbonTab(QQuickItem* parent) : RibbonQuickHost(parent)
{
    setAcceptedMouseButtons(Qt::LeftButton);
}

RibbonTab::~RibbonTab()
{
}

QUrl RibbonTab::leafUrl() const
{
    return SARibbonQmlLeafUrls::tabLeaf();
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

void RibbonTab::componentComplete()
{
    RibbonQuickHost::componentComplete();
    ensureQmlLeaf();
}

void RibbonTab::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    Q_EMIT clicked();
}

}
