#include "SARibbonGalleryItem.h"
#include "SARibbonGalleryGroup.h"

SARibbonGalleryItem::SARibbonGalleryItem()
    :m_flsgs(Qt::ItemIsEnabled|Qt::ItemIsSelectable)
{

}

SARibbonGalleryItem::SARibbonGalleryItem(const QIcon &icon)
    :m_flsgs(Qt::ItemIsEnabled|Qt::ItemIsSelectable)
{
    setIcon(icon);
}

SARibbonGalleryItem::~SARibbonGalleryItem()
{

}



void SARibbonGalleryItem::setData(int role, const QVariant &data)
{
    m_datas[role] = data;
}

QVariant SARibbonGalleryItem::data(int role) const
{
    return m_datas.value(role);
}

void SARibbonGalleryItem::setText(const QString &text)
{
    setData(Qt::DisplayRole,text);
}

QString SARibbonGalleryItem::text() const
{
    return data(Qt::DisplayRole).toString();
}

void SARibbonGalleryItem::setToolTip(const QString &text)
{
    setData(Qt::ToolTipRole, text);
}

QString SARibbonGalleryItem::toolTip() const
{
    return data(Qt::ToolTipRole).toString();
}

void SARibbonGalleryItem::setIcon(const QIcon &ico)
{
    setData(Qt::DecorationRole, ico);
}

QIcon SARibbonGalleryItem::icon() const
{
    return qvariant_cast<QIcon>(data(Qt::DecorationRole));
}

bool SARibbonGalleryItem::isSelectable() const
{
    return (m_flsgs & Qt::ItemIsSelectable);
}

void SARibbonGalleryItem::setSelectable(bool isSelectable)
{
    if(isSelectable)
    {
        m_flsgs |= Qt::ItemIsSelectable;
    }
    else
    {
        m_flsgs = (m_flsgs & (~Qt::ItemIsSelectable));
    }
}

bool SARibbonGalleryItem::isEnable() const
{
    return (m_flsgs & Qt::ItemIsEnabled);
}

void SARibbonGalleryItem::setEnable(bool isEnable)
{
    if(isEnable)
    {
        m_flsgs |= Qt::ItemIsEnabled;
    }
    else
    {
        m_flsgs = (m_flsgs & (~Qt::ItemIsEnabled));
    }
}

void SARibbonGalleryItem::setFlags(Qt::ItemFlags flag)
{
    m_flsgs = flag;
}

Qt::ItemFlags SARibbonGalleryItem::flags() const
{
    return m_flsgs;
}
