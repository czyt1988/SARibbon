#include "RibbonGalleryItem.h"

namespace SARibbonQml {

RibbonGalleryItem::RibbonGalleryItem(QObject* parent) : QObject(parent)
{
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

}
