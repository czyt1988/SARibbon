#include "SARibbonQmlGalleryGroup.h"

namespace SARibbonQml {

RibbonGalleryGroup::RibbonGalleryGroup(QObject* parent) : QObject(parent)
{
}

QString RibbonGalleryGroup::groupTitle() const
{
    return mGroupTitle;
}

void RibbonGalleryGroup::setGroupTitle(const QString& t)
{
    if (mGroupTitle == t) {
        return;
    }
    mGroupTitle = t;
    Q_EMIT groupTitleChanged();
}

QQmlListProperty< RibbonGalleryItem > RibbonGalleryGroup::items()
{
    return QQmlListProperty< RibbonGalleryItem >(this, this, &RibbonGalleryGroup::appendItemCb, &RibbonGalleryGroup::itemCountCb,
                                                 &RibbonGalleryGroup::itemAtCb, &RibbonGalleryGroup::clearItemsCb);
}

int RibbonGalleryGroup::itemCount() const
{
    return mItems.size();
}

RibbonGalleryItem* RibbonGalleryGroup::itemAt(int index) const
{
    return (index >= 0 && index < mItems.size()) ? mItems[ index ] : nullptr;
}

RibbonGalleryItem* RibbonGalleryGroup::addAction(QAction* action)
{
    if (nullptr == action) {
        return nullptr;
    }
    auto* item = new RibbonGalleryItem(this);
    item->setAction(action);
    appendItem(item);
    return item;
}

void RibbonGalleryGroup::appendItem(RibbonGalleryItem* item)
{
    if (item && !mItems.contains(item)) {
        mItems.append(item);
        item->setParent(this);
        Q_EMIT itemsChanged();
    }
}

void RibbonGalleryGroup::clearItems()
{
    if (!mItems.isEmpty()) {
        mItems.clear();
        Q_EMIT itemsChanged();
    }
}

SARibbonQml::RibbonGalleryItem* RibbonGalleryGroup::addItem(const QString& text,
                                                            const QString& iconSource,
                                                            const QString& toolTip,
                                                            const QVariant& userData)
{
    auto* item = new RibbonGalleryItem(this);
    item->setText(text);
    if (!iconSource.isEmpty()) {
        item->setIconSource(iconSource);
    }
    if (!toolTip.isEmpty()) {
        item->setToolTip(toolTip);
    }
    item->setUserData(userData);
    appendItem(item);
    return item;
}

void RibbonGalleryGroup::clear()
{
    // the items are QObject children of this group (appendItem reparents them),
    // so detaching the vector and deferring the deletion is ownership-safe both
    // ways: nothing dangles before the event loop runs, and a deletion racing
    // the group's own destruction is settled by the parent-child link
    const QVector< RibbonGalleryItem* > doomed = mItems;
    clearItems();
    for (RibbonGalleryItem* item : doomed) {
        item->deleteLater();
    }
}

void RibbonGalleryGroup::appendItemCb(QQmlListProperty< RibbonGalleryItem >* prop, RibbonGalleryItem* item)
{
    auto* self = static_cast< RibbonGalleryGroup* >(prop->data);
    if (self) {
        self->appendItem(item);
    }
}

RibbonGalleryGroup::ListIndex RibbonGalleryGroup::itemCountCb(QQmlListProperty< RibbonGalleryItem >* prop)
{
    auto* self = static_cast< RibbonGalleryGroup* >(prop->data);
    return self ? self->mItems.size() : ListIndex(0);
}

RibbonGalleryItem* RibbonGalleryGroup::itemAtCb(QQmlListProperty< RibbonGalleryItem >* prop, ListIndex index)
{
    auto* self = static_cast< RibbonGalleryGroup* >(prop->data);
    return self ? self->itemAt(int(index)) : nullptr;
}

void RibbonGalleryGroup::clearItemsCb(QQmlListProperty< RibbonGalleryItem >* prop)
{
    auto* self = static_cast< RibbonGalleryGroup* >(prop->data);
    if (self) {
        self->clearItems();
    }
}

}
