#include "SARibbonGalleryItem.h"
#include "SARibbonGalleryGroup.h"

SARibbonGalleryItem::SARibbonGalleryItem() : m_flags(Qt::ItemIsEnabled | Qt::ItemIsSelectable), m_action(nullptr)
{
}

SARibbonGalleryItem::SARibbonGalleryItem(const QString& text, const QIcon& icon)
    : m_flags(Qt::ItemIsEnabled | Qt::ItemIsSelectable), m_action(nullptr)
{
    setText(text);
    setIcon(icon);
    setTextAlignment(Qt::AlignTop | Qt::AlignHCenter);
}

SARibbonGalleryItem::SARibbonGalleryItem(QAction* act) : m_flags(Qt::ItemIsEnabled | Qt::ItemIsSelectable)
{
    setTextAlignment(Qt::AlignTop | Qt::AlignHCenter);
    setAction(act);
}

SARibbonGalleryItem::~SARibbonGalleryItem()
{
}

void SARibbonGalleryItem::setData(int role, const QVariant& data)
{
    m_datas[ role ] = data;
}

QVariant SARibbonGalleryItem::data(int role) const
{
    if (m_action) {
        switch (role) {
        case Qt::DisplayRole:
            return (m_action->text());

        case Qt::ToolTipRole:
            return (m_action->toolTip());

        case Qt::DecorationRole:
            return (m_action->icon());
        default:
            break;
        }
    }
    return (m_datas.value(role));
}

void SARibbonGalleryItem::setText(const QString& text)
{
    setData(Qt::DisplayRole, text);
}

QString SARibbonGalleryItem::text() const
{
    if (m_action) {
        return (m_action->text());
    }
    return (data(Qt::DisplayRole).toString());
}

void SARibbonGalleryItem::setToolTip(const QString& text)
{
    setData(Qt::ToolTipRole, text);
}

QString SARibbonGalleryItem::toolTip() const
{
    if (m_action) {
        return (m_action->toolTip());
    }
    return (data(Qt::ToolTipRole).toString());
}

void SARibbonGalleryItem::setIcon(const QIcon& ico)
{
    setData(Qt::DecorationRole, ico);
}

QIcon SARibbonGalleryItem::icon() const
{
    if (m_action) {
        return (m_action->icon());
    }
    return (qvariant_cast< QIcon >(data(Qt::DecorationRole)));
}

bool SARibbonGalleryItem::isSelectable() const
{
    return (m_flags & Qt::ItemIsSelectable);
}

void SARibbonGalleryItem::setSelectable(bool isSelectable)
{
    if (isSelectable) {
        m_flags |= Qt::ItemIsSelectable;
    } else {
        m_flags = (m_flags & (~Qt::ItemIsSelectable));
    }
}

bool SARibbonGalleryItem::isEnable() const
{
    if (m_action) {
        return (m_action->isEnabled());
    }
    return (m_flags & Qt::ItemIsEnabled);
}

void SARibbonGalleryItem::setEnable(bool isEnable)
{
    if (m_action) {
        m_action->setEnabled(isEnable);
    }

    if (isEnable) {
        m_flags |= Qt::ItemIsEnabled;
    } else {
        m_flags = (m_flags & (~Qt::ItemIsEnabled));
    }
}

void SARibbonGalleryItem::setFlags(Qt::ItemFlags flag)
{
    m_flags = flag;
    if (m_action) {
        m_action->setEnabled(flag & Qt::ItemIsEnabled);
    }
}

Qt::ItemFlags SARibbonGalleryItem::flags() const
{
    return (m_flags);
}

void SARibbonGalleryItem::setAction(QAction* act)
{
    m_action = act;
    if (nullptr == m_action) {
        return;
    }
    if (act->isEnabled()) {
        m_flags |= Qt::ItemIsEnabled;
    } else {
        m_flags = (m_flags & (~Qt::ItemIsEnabled));
    }
}

QAction* SARibbonGalleryItem::action()
{
    return (m_action);
}

void SARibbonGalleryItem::setTextAlignment(Qt::Alignment a)
{
    setData(Qt::TextAlignmentRole, (int)a);
}

Qt::Alignment SARibbonGalleryItem::getTextAlignment() const
{
    return qvariant_cast< Qt::Alignment >(data(Qt::TextAlignmentRole));
}
