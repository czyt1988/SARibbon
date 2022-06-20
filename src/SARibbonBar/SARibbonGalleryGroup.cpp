#include "SARibbonGalleryGroup.h"
#include <QPainter>
#include <QDebug>
#include <QActionGroup>
#include <QItemSelectionModel>
#include "SARibbonElementManager.h"
#include "SARibbonStyleOption.h"
/**
 * @brief The SARibbonGalleryGroupPrivate class
 */
class SARibbonGalleryGroupPrivate
{
public:
    SARibbonGalleryGroup* Parent;
    QString _groupTitle;
    SARibbonGalleryGroup::GalleryGroupStyle _preStyle;
    SARibbonGalleryGroup::DisplayRow _displayRow;
    bool _blockRecalc;
    int _gridMinimumWidth;       ///< grid最小宽度
    int _gridMaximumWidth;       ///< grid最大宽度
    QActionGroup* _actionGroup;  ///< 所有GalleryGroup管理的actions都由这个actiongroup管理
    SARibbonGalleryGroupPrivate(SARibbonGalleryGroup* p)
        : Parent(p)
        , _preStyle(SARibbonGalleryGroup::IconWithText)
        , _displayRow(SARibbonGalleryGroup::DisplayOneRow)
        , _blockRecalc(false)
        , _gridMinimumWidth(0)
        , _gridMaximumWidth(0)
    {
        _actionGroup = new QActionGroup(p);
        p->connect(_actionGroup, &QActionGroup::triggered, p, &SARibbonGalleryGroup::triggered);
        p->connect(_actionGroup, &QActionGroup::hovered, p, &SARibbonGalleryGroup::hovered);
    }
};

////////////////////////////////////////

SARibbonGalleryGroupItemDelegate::SARibbonGalleryGroupItemDelegate(SARibbonGalleryGroup* group, QObject* parent)
    : QStyledItemDelegate(parent), m_group(group)
{
}

void SARibbonGalleryGroupItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (nullptr == m_group) {
        return;
    }
    switch (m_group->getGalleryGroupStyle()) {
    case SARibbonGalleryGroup::IconWithText:
        paintIconWithText(painter, option, index);
        break;
    case SARibbonGalleryGroup::IconWithWordWrapText:
        paintIconWithTextWordWrap(painter, option, index);
        break;
    case SARibbonGalleryGroup::IconOnly:
        paintIconOnly(painter, option, index);
        break;
    default:
        paintIconWithText(painter, option, index);
        break;
    }
}

void SARibbonGalleryGroupItemDelegate::paintIconOnly(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyle* style = m_group->style();
    int sp        = m_group->spacing();
    sp += 3;
    painter->save();
    painter->setClipRect(option.rect);
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, painter, m_group);
    // draw the icon
    QRect iconRect = option.rect;

    iconRect.adjust(sp, sp, -sp, -sp);
    QIcon ico = qvariant_cast< QIcon >(index.data(Qt::DecorationRole));

    ico.paint(painter, iconRect, Qt::AlignCenter, QIcon::Normal, QIcon::On);
    painter->restore();
}

void SARibbonGalleryGroupItemDelegate::paintIconWithText(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyledItemDelegate::paint(painter, option, index);
}

void SARibbonGalleryGroupItemDelegate::paintIconWithTextWordWrap(QPainter* painter,
                                                                 const QStyleOptionViewItem& option,
                                                                 const QModelIndex& index) const
{
    QStyledItemDelegate::paint(painter, option, index);
}

QSize SARibbonGalleryGroupItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(index);
    Q_UNUSED(option);
    return m_group->gridSize();
}

//////////////////////////////////////////

SARibbonGalleryGroupModel::SARibbonGalleryGroupModel(QObject* parent) : QAbstractListModel(parent)
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
    if (!index.isValid() || (index.row() >= m_items.size())) {
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
    for (int i = 0; i < m_items.count(); ++i) {
        if (m_items.at(i)) {
            delete m_items.at(i);
        }
    }
    m_items.clear();
    endResetModel();
}

SARibbonGalleryItem* SARibbonGalleryGroupModel::at(int row) const
{
    return (m_items.value(row));
}

void SARibbonGalleryGroupModel::insert(int row, SARibbonGalleryItem* item)
{
    beginInsertRows(QModelIndex(), row, row);
    m_items.insert(row, item);
    endInsertRows();
}

SARibbonGalleryItem* SARibbonGalleryGroupModel::take(int row)
{
    if ((row < 0) || (row >= m_items.count())) {
        return (0);
    }

    beginRemoveRows(QModelIndex(), row, row);
    SARibbonGalleryItem* item = m_items.takeAt(row);

    endRemoveRows();
    return (item);
}

void SARibbonGalleryGroupModel::append(SARibbonGalleryItem* item)
{
    beginInsertRows(QModelIndex(), m_items.count(), m_items.count() + 1);
    m_items.append(item);
    endInsertRows();
}

//////////////////////////
/// \brief SARibbonGalleryGroup::SARibbonGalleryGroup
/// \param w

SARibbonGalleryGroup::SARibbonGalleryGroup(QWidget* w) : QListView(w), m_d(new SARibbonGalleryGroupPrivate(this))
{
    setViewMode(QListView::IconMode);
    setResizeMode(QListView::Adjust);
    setSelectionRectVisible(true);
    setUniformItemSizes(true);
    setSpacing(1);
    setItemDelegate(new SARibbonGalleryGroupItemDelegate(this, this));
    connect(this, &QAbstractItemView::clicked, this, &SARibbonGalleryGroup::onItemClicked);
    SARibbonGalleryGroupModel* m = new SARibbonGalleryGroupModel(this);
    setModel(m);
}

SARibbonGalleryGroup::~SARibbonGalleryGroup()
{
    delete m_d;
}

/**
 * @brief 是否禁止计算
 * @param on
 */
void SARibbonGalleryGroup::setRecalcGridSizeBlock(bool on)
{
    m_d->_blockRecalc = on;
}

bool SARibbonGalleryGroup::isRecalcGridSizeBlock() const
{
    return m_d->_blockRecalc;
}

/**
 * @brief 重新计算grid和icon的尺寸
 */
void SARibbonGalleryGroup::recalcGridSize()
{
    recalcGridSize(height());
}

void SARibbonGalleryGroup::recalcGridSize(int galleryHeight)
{
    if (isRecalcGridSizeBlock()) {
        return;
    }
    // 首先通过DisplayRow计算GridSize
    int dr = static_cast< int >(getDisplayRow());
    if (dr < 1) {
        dr = 1;
    } else if (dr > 3) {
        dr = 3;
    }
    int h = galleryHeight / dr;
    if (h <= 1) {
        h = galleryHeight;
    }
    int w = h;
    if (getGridMinimumWidth() > 0) {
        if (w < getGridMinimumWidth()) {
            w = getGridMinimumWidth();
        }
    }
    if (getGridMaximumWidth() > 0) {
        if (w > getGridMaximumWidth()) {
            w = getGridMaximumWidth();
        }
    }
    setGridSize(QSize(w, h));
    //在通过GalleryGroupStyle确定icon的尺寸
    const int shiftpix = 4;  // 这个是移动像素，qt在鼠标移动到图标上时会移动一下，给用户明确的动态，导致如果布局很满会超出显示范围，因此要在此基础上缩放一点
    switch (getGalleryGroupStyle()) {
    case IconWithText: {
        int textHeight = fontMetrics().lineSpacing();
        int iconHeight = h - textHeight - 2 * spacing() - shiftpix;
        if (iconHeight > 0) {
            setIconSize(QSize(w - 2 * spacing() - shiftpix, iconHeight));
        } else {
            setIconSize(QSize(w - 2 * spacing() - shiftpix, h - 2 * spacing() - shiftpix));
        }
        break;
    }
    case IconWithWordWrapText: {
        int textHeight = fontMetrics().lineSpacing() * 2;
        int iconHeight = h - textHeight;
        if (iconHeight > 0) {
            setIconSize(QSize(w - 2 * spacing() - shiftpix, iconHeight - 2 * spacing() - shiftpix));
        } else {
            setIconSize(QSize(w - 2 * spacing() - shiftpix, h - 2 * spacing() - shiftpix));
        }
        break;
    }
    case IconOnly: {
        setIconSize(QSize(w - 2 * spacing() - shiftpix, h - 2 * spacing() - shiftpix));
        break;
    }
    default: {
        setIconSize(QSize(w - 2 * spacing() - shiftpix, h - 2 * spacing() - shiftpix));
        break;
    }
    }
#if 0
    qDebug() << "SARibbonGalleryGroup::recalcGridSize(" << galleryHeight << "): gridSize=" << gridSize()
             << " iconSize=" << iconSize();
#endif
}

///
/// \brief 设置默认的预设样式
/// \param style
///
void SARibbonGalleryGroup::setGalleryGroupStyle(SARibbonGalleryGroup::GalleryGroupStyle style)
{
    m_d->_preStyle = style;
    if (style == IconWithWordWrapText) {
        setWordWrap(true);
    }
    recalcGridSize();
}

SARibbonGalleryGroup::GalleryGroupStyle SARibbonGalleryGroup::getGalleryGroupStyle() const
{
    return m_d->_preStyle;
}

void SARibbonGalleryGroup::addItem(const QString& text, const QIcon& icon)
{
    if (nullptr == groupModel()) {
        return;
    }
    addItem(new SARibbonGalleryItem(text, icon));
}

/**
 * @brief 添加一个条目
 *
 * @param item 条目的内存所有权归属SARibbonGalleryGroup管理
 */
void SARibbonGalleryGroup::addItem(SARibbonGalleryItem* item)
{
    if (nullptr == groupModel()) {
        return;
    }
    groupModel()->append(item);
}

void SARibbonGalleryGroup::addActionItem(QAction* act)
{
    if (nullptr == groupModel()) {
        return;
    }
    m_d->_actionGroup->addAction(act);
    groupModel()->append(new SARibbonGalleryItem(act));
}

void SARibbonGalleryGroup::addActionItemList(const QList< QAction* >& acts)
{
    SARibbonGalleryGroupModel* model = groupModel();

    if (nullptr == model) {
        return;
    }
    for (QAction* a : acts) {
        m_d->_actionGroup->addAction(a);
    }
    for (int i = 0; i < acts.size(); ++i) {
        model->append(new SARibbonGalleryItem(acts[ i ]));
    }
}

///
/// \brief 构建一个model，这个model的父类是SARibbonGalleryGroup，如果要共享model，需要手动处理model的父类
///
void SARibbonGalleryGroup::setupGroupModel()
{
    setModel(new SARibbonGalleryGroupModel(this));
}

SARibbonGalleryGroupModel* SARibbonGalleryGroup::groupModel()
{
    return (qobject_cast< SARibbonGalleryGroupModel* >(model()));
}

void SARibbonGalleryGroup::setGroupTitle(const QString& title)
{
    m_d->_groupTitle = title;
    emit groupTitleChanged(m_d->_groupTitle);
}

QString SARibbonGalleryGroup::getGroupTitle() const
{
    return (m_d->_groupTitle);
}

void SARibbonGalleryGroup::selectByIndex(int i)
{
    SARibbonGalleryGroupModel* model = groupModel();

    if (nullptr == model) {
        return;
    }
    QModelIndex ind               = model->index(i, 0, QModelIndex());
    QItemSelectionModel* selmodel = selectionModel();

    if (selmodel) {
        selmodel->select(ind, QItemSelectionModel::SelectCurrent);
    }
}

/**
 * @brief 设置显示的行数
 * @param r
 */
void SARibbonGalleryGroup::setDisplayRow(DisplayRow r)
{
    m_d->_displayRow = r;
    recalcGridSize();
}

/**
 * @brief Gallery显示的行数
 * @return
 */
SARibbonGalleryGroup::DisplayRow SARibbonGalleryGroup::getDisplayRow() const
{
    return m_d->_displayRow;
}

/**
 * @brief 设置grid最小的宽度，默认为0（不限制）
 * @param w
 */
void SARibbonGalleryGroup::setGridMinimumWidth(int w)
{
    m_d->_gridMinimumWidth = w;
}

/**
 * @brief grid最小的宽度，默认为0（不限制）
 * @return
 */
int SARibbonGalleryGroup::getGridMinimumWidth() const
{
    return m_d->_gridMinimumWidth;
}

/**
 * @brief 设置grid最大的宽度，默认为0（不限制）
 * @param w
 */
void SARibbonGalleryGroup::setGridMaximumWidth(int w)
{
    m_d->_gridMaximumWidth = w;
}

/**
 * @brief grid最大的的宽度，默认为0（不限制）
 * @param w
 */
int SARibbonGalleryGroup::getGridMaximumWidth() const
{
    return m_d->_gridMaximumWidth;
}

/**
 * @brief 获取SARibbonGalleryGroup管理的actiongroup
 * @return
 */
QActionGroup* SARibbonGalleryGroup::getActionGroup() const
{
    return m_d->_actionGroup;
}

void SARibbonGalleryGroup::onItemClicked(const QModelIndex& index)
{
    if (index.isValid()) {
        SARibbonGalleryItem* item = (SARibbonGalleryItem*)index.internalPointer();
        if (item) {
            QAction* act = item->action();
            if (act) {
                act->activate(QAction::Trigger);
            }
        }
    }
}

void SARibbonGalleryGroup::onItemEntered(const QModelIndex& index)
{
    if (index.isValid()) {
        SARibbonGalleryItem* item = (SARibbonGalleryItem*)index.internalPointer();
        if (item) {
            QAction* act = item->action();
            if (act) {
                act->activate(QAction::Hover);
            }
        }
    }
}
