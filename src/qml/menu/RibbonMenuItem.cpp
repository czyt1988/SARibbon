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

}
