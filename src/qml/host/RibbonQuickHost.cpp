#include "RibbonQuickHost.h"
#include "../SARibbonQmlTypes.h"

namespace SARibbonQml {

RibbonQuickHost::RibbonQuickHost(QQuickItem* parent) : QQuickItem(parent)
{
}

RibbonQuickHost::~RibbonQuickHost()
{
    // Leaf teardown (round-5 root cause, NOTES B44): the leaf is a QObject
    // CHILD of this host (setParent in createVisualLeaf), so QObject's own
    // child cleanup deletes it synchronously right after this destructor
    // body — while the QML engine is still alive. Detaching via
    // setParent(nullptr) + deleteLater instead left the leaf pending past
    // the engine's teardown, and the deferred deletion then freed V4 heap
    // blocks of an already-destroyed engine (_CrtIsValidHeapPointer assert /
    // QV4::Value::fromHeapObject access violations — the round-4 optionAction
    // crash family). Only the scene-graph attachment is detached here.
    if (mQmlLeaf) {
        mQmlLeaf->setParentItem(nullptr);
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
