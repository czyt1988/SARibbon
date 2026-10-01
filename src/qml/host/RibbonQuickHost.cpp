#include "RibbonQuickHost.h"
#include "../SARibbonQmlTypes.h"

namespace SARibbonQml {

RibbonQuickHost::RibbonQuickHost(QQuickItem* parent) : QQuickItem(parent)
{
}

RibbonQuickHost::~RibbonQuickHost()
{
    // leaf destruction: unparent + deleteLater, NEVER direct delete (a QML item may
    // sit inside its own mouse-handling call stack, KDDW Group.cpp same rule)
    if (mQmlLeaf) {
        mQmlLeaf->setParentItem(nullptr);
        mQmlLeaf->setParent(nullptr);
        mQmlLeaf->deleteLater();
        mQmlLeaf = nullptr;
    }
}

QQuickItem* RibbonQuickHost::qmlLeaf() const
{
    return mQmlLeaf;
}

void RibbonQuickHost::setQmlLeaf(QQuickItem* item)
{
    if (mQmlLeaf == item) {
        return;
    }
    mQmlLeaf = item;
    Q_EMIT qmlLeafChanged();
}

void RibbonQuickHost::ensureQmlLeaf()
{
    if (mQmlLeaf) {
        return;
    }
    QQuickItem* leaf = createVisualLeaf(this, leafUrl());
    if (leaf && !mQmlLeaf) {
        setQmlLeaf(leaf);  // handshake assigns it; fallback keeps the pair intact
    }
}

}
