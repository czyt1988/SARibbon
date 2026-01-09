#include "SARibbonGalleryGroup.h"
#include <QPainter>
#include <QDebug>
#include <QActionGroup>
#include <QItemSelectionModel>
#include "SARibbonElementManager.h"
/**
 * @brief The SARibbonGalleryGroupPrivate class
 */
class SARibbonGalleryGroup::PrivateData
{
public:
    SARibbonGalleryGroup* q_ptr;
    QString mGroupTitle;
    SARibbonGalleryGroup::GalleryGroupStyle mPreStyle { SARibbonGalleryGroup::IconWithText };
    SARibbonGalleryGroup::DisplayRow mDisplayRow { SARibbonGalleryGroup::DisplayOneRow };
    bool mBlockRecalc { false };
    int mGridMinimumWidth { 0 };             ///< grid最小宽度
    int mGridMaximumWidth { 0 };             ///< grid最大宽度
    QActionGroup* mActionGroup { nullptr };  ///< 所有GalleryGroup管理的actions都由这个actiongroup管理
public:
    PrivateData(SARibbonGalleryGroup* p) : q_ptr(p)
    {
        mActionGroup = new QActionGroup(p);
        p->connect(mActionGroup, &QActionGroup::triggered, p, &SARibbonGalleryGroup::triggered);
        p->connect(mActionGroup, &QActionGroup::hovered, p, &SARibbonGalleryGroup::hovered);
    }
};

//===================================================
// SARibbonGalleryGroupItemDelegate
//===================================================

SARibbonGalleryGroupItemDelegate::SARibbonGalleryGroupItemDelegate(SARibbonGalleryGroup* group, QObject* parent)
    : QStyledItemDelegate(parent), mGroup(group)
{
}

SARibbonGalleryGroupItemDelegate::~SARibbonGalleryGroupItemDelegate()
{
}

void SARibbonGalleryGroupItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (nullptr == mGroup) {
        return;
    }
    switch (mGroup->galleryGroupStyle()) {
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

void SARibbonGalleryGroupItemDelegate::paintIconOnly(QPainter* painter,
                                                     const QStyleOptionViewItem& option,
                                                     const QModelIndex& index) const
{
    QStyle* style = mGroup->style();
    int sp        = mGroup->spacing();
    sp += 3;
    painter->save();
    painter->setClipRect(option.rect);
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, painter, mGroup);
    // draw the icon
    QRect iconRect = option.rect;

    iconRect.adjust(sp, sp, -sp, -sp);
    QIcon ico = qvariant_cast< QIcon >(index.data(Qt::DecorationRole));

    ico.paint(painter, iconRect, Qt::AlignCenter, QIcon::Normal, QIcon::On);
    painter->restore();
}

void SARibbonGalleryGroupItemDelegate::paintIconWithText(QPainter* painter,
                                                         const QStyleOptionViewItem& option,
                                                         const QModelIndex& index) const
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
    return mGroup->gridSize();
}

//===================================================
// SARibbonGalleryGroupModel
//===================================================

SARibbonGalleryGroupModel::SARibbonGalleryGroupModel(QObject* parent) : QAbstractListModel(parent)
{
}

SARibbonGalleryGroupModel::~SARibbonGalleryGroupModel()
{
    clear();
}

int SARibbonGalleryGroupModel::rowCount(const QModelIndex& parent) const
{
    return (parent.isValid() ? 0 : mItems.size());
}

Qt::ItemFlags SARibbonGalleryGroupModel::flags(const QModelIndex& index) const
{
    if (!index.isValid() || (index.row() >= mItems.size())) {
        return (Qt::NoItemFlags);
    }
    return (mItems.at(index.row())->flags());
}

QVariant SARibbonGalleryGroupModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || (index.row() >= mItems.count())) {
        return (QVariant());
    }
    return (mItems.at(index.row())->data(role));
}

QModelIndex SARibbonGalleryGroupModel::index(int row, int column, const QModelIndex& parent) const
{
    if (hasIndex(row, column, parent)) {
        return (createIndex(row, column, mItems.at(row)));
    }
    return (QModelIndex());
}

bool SARibbonGalleryGroupModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid() || (index.row() >= mItems.count())) {
        return (false);
    }

    mItems.at(index.row())->setData(role, value);
    return (true);
}

void SARibbonGalleryGroupModel::clear()
{
    beginResetModel();
    for (int i = 0; i < mItems.count(); ++i) {
        if (mItems.at(i)) {
            delete mItems.at(i);
        }
    }
    mItems.clear();
    endResetModel();
}

SARibbonGalleryItem* SARibbonGalleryGroupModel::at(int row) const
{
    return (mItems.value(row));
}

void SARibbonGalleryGroupModel::insert(int row, SARibbonGalleryItem* item)
{
    beginInsertRows(QModelIndex(), row, row);
    mItems.insert(row, item);
    endInsertRows();
}

SARibbonGalleryItem* SARibbonGalleryGroupModel::take(int row)
{
    if ((row < 0) || (row >= mItems.count())) {
        return (0);
    }

    beginRemoveRows(QModelIndex(), row, row);
    SARibbonGalleryItem* item = mItems.takeAt(row);

    endRemoveRows();
    return (item);
}

/**
 * @brief 插入条目，item的内存由SARibbonGalleryGroupModel管理
 * @param item
 */
void SARibbonGalleryGroupModel::append(SARibbonGalleryItem* item)
{
    beginInsertRows(QModelIndex(), mItems.count(), mItems.count() + 1);

    mItems.append(item);
    endInsertRows();
}

/**
 * @brief remove the inner GalleryItem by the object name of QAction
 * @param act_object_name [const QString&]
 * @return the result of removal operation
 */
bool SARibbonGalleryGroupModel::remove(const QString& act_object_name)
{
    for (size_t index = 0; index < mItems.size(); ++index)
    {
        if (mItems.at(index)->action()->objectName() == act_object_name)
        {
            beginRemoveRows(QModelIndex(), index, index);
            SARibbonGalleryItem* item = mItems.takeAt(index);
            endRemoveRows();

            delete item;

            return true;
        }
    }

    return false;
}

/**
 * @brief 获取条目数量
 * @return
 */
int SARibbonGalleryGroupModel::itemSize() const
{
    return mItems.size();
}

//===================================================
// SARibbonGalleryGroup
//===================================================
SARibbonGalleryGroup::SARibbonGalleryGroup(QWidget* w)
    : QListView(w), d_ptr(new SARibbonGalleryGroup::PrivateData(this))
{
    setViewMode(QListView::IconMode);
    setResizeMode(QListView::Adjust);
    setFlow(QListView::LeftToRight);
    setSelectionRectVisible(true);
    setUniformItemSizes(true);
    setSpacing(1);
    setItemDelegate(new SARibbonGalleryGroupItemDelegate(this, this));
    connect(this, &QAbstractItemView::clicked, this, &SARibbonGalleryGroup::onItemClicked);
    SARibbonGalleryGroupModel* m = setupGroupModel();
    setModel(m);
}

SARibbonGalleryGroup::~SARibbonGalleryGroup()
{
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
    // 首先通过DisplayRow计算GridSize
    int dr = static_cast< int >(displayRow());
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
    if (gridMinimumWidth() > 0) {
        if (w < gridMinimumWidth()) {
            w = gridMinimumWidth();
        }
    }
    if (gridMaximumWidth() > 0) {
        if (w > gridMaximumWidth()) {
            w = gridMaximumWidth();
        }
    }
    setGridSize(QSize(w, h));
    // 在通过GalleryGroupStyle确定icon的尺寸
    const int shiftpix =
        4;  // 这个是移动像素，qt在鼠标移动到图标上时会移动一下，给用户明确的动态，导致如果布局很满会超出显示范围，因此要在此基础上缩放一点
    switch (galleryGroupStyle()) {
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
    d_ptr->mPreStyle = style;
    setWordWrap(style == IconWithWordWrapText);
    recalcGridSize();
}

SARibbonGalleryGroup::GalleryGroupStyle SARibbonGalleryGroup::galleryGroupStyle() const
{
    return d_ptr->mPreStyle;
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
    d_ptr->mActionGroup->addAction(act);
    groupModel()->append(new SARibbonGalleryItem(act));
}

void SARibbonGalleryGroup::addActionItemList(const QList< QAction* >& acts)
{
    SARibbonGalleryGroupModel* model = groupModel();

    if (nullptr == model) {
        return;
    }
    for (QAction* a : acts) {
        d_ptr->mActionGroup->addAction(a);
    }
    for (int i = 0; i < acts.size(); ++i) {
        model->append(new SARibbonGalleryItem(acts[ i ]));
    }
}

/**
 * @brief remove one QAction in current GalleryGroup, and this function can't be replace by removeAction.
 * @param act [QAction*] the pointer for the QAction which will be removed.
 * @return the result of removal operation
 */
bool SARibbonGalleryGroup::removeActionItem(QAction* act)
{
    if (nullptr == groupModel()) {
        return false;
    }else {
        d_ptr->mActionGroup->removeAction(act);
        return groupModel()->remove(act->objectName());
    }
}


/**
 * @brief 构建一个model，这个model的父类是SARibbonGalleryGroup，如果要共享model，需要手动处理model的父类
 * @return
 */
SARibbonGalleryGroupModel* SARibbonGalleryGroup::setupGroupModel()
{
    SARibbonGalleryGroupModel* m = new SARibbonGalleryGroupModel(this);
    setModel(m);
    return m;
}

SARibbonGalleryGroupModel* SARibbonGalleryGroup::groupModel() const
{
    return (qobject_cast< SARibbonGalleryGroupModel* >(model()));
}

void SARibbonGalleryGroup::setGroupTitle(const QString& title)
{
    d_ptr->mGroupTitle = title;
    Q_EMIT groupTitleChanged(d_ptr->mGroupTitle);
}

QString SARibbonGalleryGroup::groupTitle() const
{
    return (d_ptr->mGroupTitle);
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
    d_ptr->mDisplayRow = r;
    recalcGridSize();
}

/**
 * @brief Gallery显示的行数
 * @return
 */
SARibbonGalleryGroup::DisplayRow SARibbonGalleryGroup::displayRow() const
{
    return d_ptr->mDisplayRow;
}

/**
 * @brief 设置grid最小的宽度，默认为0（不限制）
 * @param w
 */
void SARibbonGalleryGroup::setGridMinimumWidth(int w)
{
    d_ptr->mGridMinimumWidth = w;
}

/**
 * @brief grid最小的宽度，默认为0（不限制）
 * @return
 */
int SARibbonGalleryGroup::gridMinimumWidth() const
{
    return d_ptr->mGridMinimumWidth;
}

/**
 * @brief 设置grid最大的宽度，默认为0（不限制）
 * @param w
 */
void SARibbonGalleryGroup::setGridMaximumWidth(int w)
{
    d_ptr->mGridMaximumWidth = w;
}

/**
 * @brief grid最大的的宽度，默认为0（不限制）
 * @param w
 */
int SARibbonGalleryGroup::gridMaximumWidth() const
{
    return d_ptr->mGridMaximumWidth;
}

/**
 * @brief 获取SARibbonGalleryGroup管理的actiongroup
 * @return
 */
QActionGroup* SARibbonGalleryGroup::actionGroup() const
{
    return d_ptr->mActionGroup;
}

/**
 * @brief 计算行数
 * @return
 */
int SARibbonGalleryGroup::gridRowCount() const
{
    SARibbonGalleryGroupModel* model = groupModel();
    if (nullptr == model) {
        return 0;
    }
    int gcol = gridColumnCount();
    if (gcol == 0) {
        return 0;
    }
    // 这里的rowcount是item的数量
    int itemCnt = model->rowCount(QModelIndex());
    return (itemCnt / gcol) + 1;
}

int SARibbonGalleryGroup::gridColumnCount() const
{
    auto vp = viewport();
    if (!vp) {
        return 0;
    }
    int w    = viewport()->width();
    QSize gs = gridSize();
    return w / gs.width();
}

/**
 * @brief 计算最紧凑的高度
 * @return
 */
int SARibbonGalleryGroup::preferredHeightForWidth(int w) const
{
    SARibbonGalleryGroupModel* model = groupModel();
    if (nullptr == model) {
        return -1;
    }
    int viewWidth = w - 2 * frameWidth();
    QSize gs      = gridSize();
    int gcol      = viewWidth / gs.width();
    if (gcol == 0) {
        return -1;
    }
    int itemCnt = model->rowCount(QModelIndex());
    int grow    = (itemCnt / gcol) + 1;
    return grow * gs.height() + 2 * frameWidth() + 5;  // 这里加上5是留下一定余量，避免刚好触发滚动条
}

bool SARibbonGalleryGroup::hasHeightForWidth() const
{
    return true;
}

int SARibbonGalleryGroup::heightForWidth(int w) const
{
    return preferredHeightForWidth(w);
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
