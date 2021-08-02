﻿#include "SARibbonGalleryGroup.h"
#include <QPainter>
#include <QDebug>
#include <QItemSelectionModel>

/**
 * @brief The SARibbonGalleryGroupPrivate class
 */
class SARibbonGalleryGroupPrivate
{
public:
    SARibbonGalleryGroup *Parent;
    bool enableIconText;
    QString groupTitle;
    SARibbonGalleryGroupPrivate(SARibbonGalleryGroup *p)
        : Parent(p)
        , enableIconText(false)
    {
    }
};


////////////////////////////////////////

SARibbonGalleryGroupItemDelegate::SARibbonGalleryGroupItemDelegate(SARibbonGalleryGroup *group, QObject *parent)
    : QStyledItemDelegate(parent)
    , m_group(group)
{
}


void SARibbonGalleryGroupItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (nullptr == m_group) {
        return;
    }
    if (m_group->enableIconText()) {
        paintIconWithText(painter, option, index);
    }else {
        paintIconOnly(painter, option, index);
    }
}


void SARibbonGalleryGroupItemDelegate::paintIconOnly(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyle *style = m_group->style();

    painter->save();
    painter->setClipRect(option.rect);
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, painter, m_group);
    // draw the icon
    QRect iconRect = option.rect;

    iconRect.adjust(3, 3, -3, -3);
    QIcon ico = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));

    ico.paint(painter, iconRect, Qt::AlignCenter, QIcon::Normal, QIcon::On);
    painter->restore();
}


void SARibbonGalleryGroupItemDelegate::paintIconWithText(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyledItemDelegate::paint(painter, option, index);
}


QSize SARibbonGalleryGroupItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    //option.rect对应grid size
    Q_UNUSED(index);
    return (QSize(option.rect.width(), option.rect.height()));
}


//////////////////////////////////////////

SARibbonGalleryGroupModel::SARibbonGalleryGroupModel(QObject *parent) : QAbstractListModel(parent)
{
}


SARibbonGalleryGroupModel::~SARibbonGalleryGroupModel()
{
    clear();
}


int SARibbonGalleryGroupModel::rowCount(const QModelIndex& parent) const
{
    return (parent.isValid() ? 0 : m_items.size());
}


Qt::ItemFlags SARibbonGalleryGroupModel::flags(const QModelIndex& index) const
{
    if (!index.isValid()
        ||
        (index.row() >= m_items.size())) {
        return (Qt::NoItemFlags);
    }
    return (m_items.at(index.row())->flags());
}


QVariant SARibbonGalleryGroupModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || (index.row() >= m_items.count())) {
        return (QVariant());
    }
    return (m_items.at(index.row())->data(role));
}


QModelIndex SARibbonGalleryGroupModel::index(int row, int column, const QModelIndex& parent) const
{
    if (hasIndex(row, column, parent)) {
        return (createIndex(row, column, m_items.at(row)));
    }
    return (QModelIndex());
}


bool SARibbonGalleryGroupModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid() || (index.row() >= m_items.count())) {
        return (false);
    }

    m_items.at(index.row())->setData(role, value);
    return (true);
}


void SARibbonGalleryGroupModel::clear()
{
    beginResetModel();
    for (int i = 0; i < m_items.count(); ++i)
    {
        if (m_items.at(i)) {
            delete m_items.at(i);
        }
    }
    m_items.clear();
    endResetModel();
}


SARibbonGalleryItem *SARibbonGalleryGroupModel::at(int row) const
{
    return (m_items.value(row));
}


void SARibbonGalleryGroupModel::insert(int row, SARibbonGalleryItem *item)
{
    beginInsertRows(QModelIndex(), row, row);
    m_items.insert(row, item);
    endInsertRows();
}


SARibbonGalleryItem *SARibbonGalleryGroupModel::take(int row)
{
    if ((row < 0) || (row >= m_items.count())) {
        return (0);
    }

    beginRemoveRows(QModelIndex(), row, row);
    SARibbonGalleryItem *item = m_items.takeAt(row);

    endRemoveRows();
    return (item);
}


void SARibbonGalleryGroupModel::append(SARibbonGalleryItem *item)
{
    beginInsertRows(QModelIndex(), m_items.count(), m_items.count()+1);
    m_items.append(item);
    endInsertRows();
}


//////////////////////////
/// \brief SARibbonGalleryGroup::SARibbonGalleryGroup
/// \param w
///////////////////////

SARibbonGalleryGroup::SARibbonGalleryGroup(QWidget *w) : QListView(w)
    , m_d(new SARibbonGalleryGroupPrivate(this))
{
    setViewMode(QListView::IconMode);
    setResizeMode(QListView::Adjust);
    setSelectionRectVisible(true);
    setUniformItemSizes(true);
    setPreinstallStyle(LargeIconWithText);
    setItemDelegate(new SARibbonGalleryGroupItemDelegate(this, this));

    connect(this, &QAbstractItemView::clicked
        , this, &SARibbonGalleryGroup::onItemClicked);
}


SARibbonGalleryGroup::~SARibbonGalleryGroup()
{
    delete m_d;
}


///
/// \brief 设置默认的预设样式
/// \param style
///
void SARibbonGalleryGroup::setPreinstallStyle(SARibbonGalleryGroup::PreinstallStyle style)
{
    switch (style)
    {
    case LargeIconWithText:
    {
        setIconSize(QSize(72, 36));
        setGridSize(QSize(72, 56));
        setEnableIconText(true);
        break;
    }

    case LargeIconOnly:
    {
        setIconSize(QSize(72, 56));
        setGridSize(QSize(72, 56));
        setEnableIconText(false);
        break;
    }

    default:
    {
        setIconSize(QSize(72, 36));
        setGridSize(QSize(72, 56));
        setEnableIconText(true);
    }
    }
}


void SARibbonGalleryGroup::addItem(const QString& text, const QIcon& icon)
{
    if (nullptr == groupModel()) {
        return;
    }
    addItem(new SARibbonGalleryItem(text, icon));
}


void SARibbonGalleryGroup::addItem(SARibbonGalleryItem *item)
{
    if (nullptr == groupModel()) {
        return;
    }
    groupModel()->append(item);
}


void SARibbonGalleryGroup::addActionItem(QAction *act)
{
    if (nullptr == groupModel()) {
        return;
    }
    groupModel()->append(new SARibbonGalleryItem(act));
}


void SARibbonGalleryGroup::addActionItemList(const QList<QAction *>& acts)
{
    SARibbonGalleryGroupModel *model = groupModel();

    if (nullptr == model) {
        return;
    }
    for (int i = 0; i < acts.size(); ++i)
    {
        model->append(new SARibbonGalleryItem(acts[i]));
    }
}


///
/// \brief 构建一个model，这个model的父类是SARibbonGalleryGroup，如果要共享model，需要手动处理model的父类
///
void SARibbonGalleryGroup::setupGroupModel()
{
    setModel(new SARibbonGalleryGroupModel(this));
}


SARibbonGalleryGroupModel *SARibbonGalleryGroup::groupModel()
{
    return (qobject_cast<SARibbonGalleryGroupModel *>(model()));
}


void SARibbonGalleryGroup::setEnableIconText(bool enable)
{
    m_d->enableIconText = enable;
}


bool SARibbonGalleryGroup::enableIconText() const
{
    return (m_d->enableIconText);
}


void SARibbonGalleryGroup::setGroupTitle(const QString& title)
{
    m_d->groupTitle = title;
    emit groupTitleChanged(m_d->groupTitle);
}


QString SARibbonGalleryGroup::groupTitle() const
{
    return (m_d->groupTitle);
}


void SARibbonGalleryGroup::selectByIndex(int i)
{
    SARibbonGalleryGroupModel *model = groupModel();

    if (nullptr == model) {
        return;
    }
    QModelIndex ind = model->index(i, 0, QModelIndex());
    QItemSelectionModel *selmodel = selectionModel();

    if (selmodel) {
        selmodel->select(ind, QItemSelectionModel::SelectCurrent);
    }
}


void SARibbonGalleryGroup::onItemClicked(const QModelIndex& index)
{
    if (index.isValid()) {
        SARibbonGalleryItem *item = (SARibbonGalleryItem *)index.internalPointer();
        if (item) {
            QAction *act = item->action();
            if (act) {
                act->activate(QAction::Trigger);
            }
        }
    }
}
