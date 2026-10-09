#include "SARibbonQmlGalleryViewportWidget.h"

namespace SARibbonQml {

RibbonGalleryViewportWidget::RibbonGalleryViewportWidget(QQuickItem* parent) : QQuickItem(parent)
{
    // a width change can reflow the content (wrapped text), which changes the
    // childrenRect height without any child signalling a height change
    connect(this, &QQuickItem::widthChanged, this, &RibbonGalleryViewportWidget::updateImplicitHeight);
}

QString RibbonGalleryViewportWidget::sectionTitle() const
{
    return mSectionTitle;
}

void RibbonGalleryViewportWidget::setSectionTitle(const QString& t)
{
    if (mSectionTitle == t) {
        return;
    }
    mSectionTitle = t;
    Q_EMIT sectionTitleChanged();
}

void RibbonGalleryViewportWidget::itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData& data)
{
    QQuickItem::itemChange(change, data);
    // childrenRect only aggregates direct children, so watching join/leave
    // plus their geometry signals is enough (inner changes move the direct
    // child's geometry first, which fires here again)
    if (change == QQuickItem::ItemChildAddedChange) {
        watchChild(data.item, true);
        updateImplicitHeight();
    } else if (change == QQuickItem::ItemChildRemovedChange) {
        watchChild(data.item, false);
        updateImplicitHeight();
    }
}

void RibbonGalleryViewportWidget::watchChild(QQuickItem* child, bool on)
{
    if (!child) {
        return;
    }
    // disconnect-then-connect instead of Qt::UniqueConnection: the Qt 5 lane
    // does not honour UniqueConnection with the PMF connect syntax, so a
    // duplicate watch would silently not connect at all there
    disconnect(child, &QQuickItem::heightChanged, this, &RibbonGalleryViewportWidget::updateImplicitHeight);
    disconnect(child, &QQuickItem::implicitHeightChanged, this, &RibbonGalleryViewportWidget::updateImplicitHeight);
    disconnect(child, &QQuickItem::yChanged, this, &RibbonGalleryViewportWidget::updateImplicitHeight);
    if (on) {
        // height covers explicit resizes, implicitHeight covers the positioner
        // lane (a Qt 5 Column changing its content re-derives implicitHeight
        // without emitting heightChanged), y covers repositioned children
        connect(child, &QQuickItem::heightChanged, this, &RibbonGalleryViewportWidget::updateImplicitHeight);
        connect(child, &QQuickItem::implicitHeightChanged, this, &RibbonGalleryViewportWidget::updateImplicitHeight);
        connect(child, &QQuickItem::yChanged, this, &RibbonGalleryViewportWidget::updateImplicitHeight);
    }
}

void RibbonGalleryViewportWidget::updateImplicitHeight()
{
    // plain C++ property write: the leaf's slot height reads implicitHeight,
    // and this derivation lives outside the QML binding graph, so a
    // QML-side childrenRect binding loop cannot form around it
    const qreal h = childrenRect().height();
    if (!qFuzzyCompare(implicitHeight(), h)) {
        setImplicitHeight(h);
    }
}

}
