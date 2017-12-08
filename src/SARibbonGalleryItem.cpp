#include "SARibbonGalleryItem.h"
#include "SARibbonGalleryGroup.h"

SARibbonGalleryItem::SARibbonGalleryItem()
    :m_flsgs(Qt::ItemIsEnabled|Qt::ItemIsSelectable)
    ,m_action(nullptr)
{

}

SARibbonGalleryItem::SARibbonGalleryItem(const QIcon &icon)
    :m_flsgs(Qt::ItemIsEnabled|Qt::ItemIsSelectable)
    ,m_action(nullptr)
{
    setIcon(icon);
}

SARibbonGalleryItem::SARibbonGalleryItem(QAction *act)
    :m_flsgs(Qt::ItemIsEnabled|Qt::ItemIsSelectable)
{
    setAction(act);
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
    if(m_action)
    {
        switch (role) {
        case Qt::DisplayRole:
            return m_action->text();
        case Qt::ToolTipRole:
            return m_action->toolTip();
        case Qt::DecorationRole:
            return m_action->icon();
        default:
            break;
        }
    }
    return m_datas.value(role);
}

void SARibbonGalleryItem::setText(const QString &text)
{
    setData(Qt::DisplayRole,text);
}

QString SARibbonGalleryItem::text() const
{
    if(m_action)
    {
        return m_action->text();
    }
    return data(Qt::DisplayRole).toString();
}

void SARibbonGalleryItem::setToolTip(const QString &text)
{
    setData(Qt::ToolTipRole, text);
}

QString SARibbonGalleryItem::toolTip() const
{
    if(m_action)
    {
        return m_action->toolTip();
    }
    return data(Qt::ToolTipRole).toString();
}

void SARibbonGalleryItem::setIcon(const QIcon &ico)
{
    setData(Qt::DecorationRole, ico);
}

QIcon SARibbonGalleryItem::icon() const
{
    if(m_action)
    {
        return m_action->icon();
    }
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
    if(m_action)
    {
        return m_action->isEnabled();
    }
    return (m_flsgs & Qt::ItemIsEnabled);
}

void SARibbonGalleryItem::setEnable(bool isEnable)
{
    if(m_action)
    {
        m_action->setEnabled(isEnable);
    }

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
    if(m_action)
    {
        m_action->setEnabled(flag & Qt::ItemIsEnabled);
    }
}

Qt::ItemFlags SARibbonGalleryItem::flags() const
{
    return m_flsgs;
}

void SARibbonGalleryItem::setAction(QAction *act)
{
    m_action = act;
    if(act->isEnabled())
    {
        m_flsgs |= Qt::ItemIsEnabled;
    }
    else
    {
        m_flsgs = (m_flsgs & (~Qt::ItemIsEnabled));
    }
}

QAction *SARibbonGalleryItem::action()
{
    return m_action;
}
