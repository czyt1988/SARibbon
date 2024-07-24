#include "SARibbonGalleryItem.h"
#include "SARibbonGalleryGroup.h"

SARibbonGalleryItem::SARibbonGalleryItem() : mFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable), mAction(nullptr)
{
}

SARibbonGalleryItem::SARibbonGalleryItem(const QString& text, const QIcon& icon)
    : mFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable), mAction(nullptr)
{
    setText(text);
    setIcon(icon);
    setTextAlignment(Qt::AlignTop | Qt::AlignHCenter);
}

SARibbonGalleryItem::SARibbonGalleryItem(QAction* act) : mFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable)
{
    setTextAlignment(Qt::AlignTop | Qt::AlignHCenter);
    setAction(act);
}

SARibbonGalleryItem::~SARibbonGalleryItem()
{
}

void SARibbonGalleryItem::setData(int role, const QVariant& data)
{
    mDatas[ role ] = data;
}

QVariant SARibbonGalleryItem::data(int role) const
{
    if (mAction) {
        switch (role) {
        case Qt::DisplayRole:
            return (mAction->text());

        case Qt::ToolTipRole:
            return (mAction->toolTip());

        case Qt::DecorationRole:
            return (mAction->icon());
        default:
            break;
        }
    }
    return (mDatas.value(role));
}

void SARibbonGalleryItem::setText(const QString& text)
{
    setData(Qt::DisplayRole, text);
}

QString SARibbonGalleryItem::text() const
{
    if (mAction) {
        return (mAction->text());
    }
    return (data(Qt::DisplayRole).toString());
}

void SARibbonGalleryItem::setToolTip(const QString& text)
{
    setData(Qt::ToolTipRole, text);
}

QString SARibbonGalleryItem::toolTip() const
{
    if (mAction) {
        return (mAction->toolTip());
    }
    return (data(Qt::ToolTipRole).toString());
}

void SARibbonGalleryItem::setIcon(const QIcon& ico)
{
    setData(Qt::DecorationRole, ico);
}

QIcon SARibbonGalleryItem::icon() const
{
    if (mAction) {
        return (mAction->icon());
    }
    return (qvariant_cast< QIcon >(data(Qt::DecorationRole)));
}

bool SARibbonGalleryItem::isSelectable() const
{
    return (mFlags & Qt::ItemIsSelectable);
}

void SARibbonGalleryItem::setSelectable(bool isSelectable)
{
    if (isSelectable) {
        mFlags |= Qt::ItemIsSelectable;
    } else {
        mFlags = (mFlags & (~Qt::ItemIsSelectable));
    }
}

bool SARibbonGalleryItem::isEnable() const
{
    if (mAction) {
        return (mAction->isEnabled());
    }
    return (mFlags & Qt::ItemIsEnabled);
}

void SARibbonGalleryItem::setEnable(bool isEnable)
{
    if (mAction) {
        mAction->setEnabled(isEnable);
    }

    if (isEnable) {
        mFlags |= Qt::ItemIsEnabled;
    } else {
        mFlags = (mFlags & (~Qt::ItemIsEnabled));
    }
}

void SARibbonGalleryItem::setFlags(Qt::ItemFlags flag)
{
    mFlags = flag;
    if (mAction) {
        mAction->setEnabled(flag & Qt::ItemIsEnabled);
    }
}

Qt::ItemFlags SARibbonGalleryItem::flags() const
{
    return (mFlags);
}

void SARibbonGalleryItem::setAction(QAction* act)
{
    mAction = act;
    if (nullptr == mAction) {
        return;
    }
    if (act->isEnabled()) {
        mFlags |= Qt::ItemIsEnabled;
    } else {
        mFlags = (mFlags & (~Qt::ItemIsEnabled));
    }
}

QAction* SARibbonGalleryItem::action()
{
    return (mAction);
}

void SARibbonGalleryItem::setTextAlignment(Qt::Alignment a)
{
    setData(Qt::TextAlignmentRole, (int)a);
}

Qt::Alignment SARibbonGalleryItem::textAlignment() const
{
    return qvariant_cast< Qt::Alignment >(data(Qt::TextAlignmentRole));
}
