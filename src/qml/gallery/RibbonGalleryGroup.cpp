#include "RibbonGalleryGroup.h"

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
