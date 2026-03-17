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

/**
 * \if ENGLISH
 * @brief Constructor for SARibbonGalleryGroupItemDelegate
 * @param group SARibbonGalleryGroup pointer
 * @param parent Parent object
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonGalleryGroupItemDelegate 构造函数
 * @param group SARibbonGalleryGroup指针
 * @param parent 父对象
 * \endif
 */
SARibbonGalleryGroupItemDelegate::SARibbonGalleryGroupItemDelegate(SARibbonGalleryGroup* group, QObject* parent)
    : QStyledItemDelegate(parent), mGroup(group)
{
}

/**
 * \if ENGLISH
 * @brief Destructor for SARibbonGalleryGroupItemDelegate
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonGalleryGroupItemDelegate 析构函数
 * \endif
 */
SARibbonGalleryGroupItemDelegate::~SARibbonGalleryGroupItemDelegate()
{
}

/**
 * \if ENGLISH
 * @brief Paint the item
 * @param painter Painter
 * @param option Style option
 * @param index Model index
 * \endif
 *
 * \if CHINESE
 * @brief 绘制项目
 * @param painter 画家
 * @param option 样式选项
 * @param index 模型索引
 * \endif
 */
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

/**
 * \if ENGLISH
 * @brief Paint only icon
 * @param painter Painter
 * @param option Style option
 * @param index Model index
 * \endif
 *
 * \if CHINESE
 * @brief 仅绘制图标
 * @param painter 画家
 * @param option 样式选项
 * @param index 模型索引
 * \endif
 */
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

/**
 * \if ENGLISH
 * @brief Paint icon with text
 * @param painter Painter
 * @param option Style option
 * @param index Model index
 * \endif
 *
 * \if CHINESE
 * @brief 绘制带文本的图标
 * @param painter 画家
 * @param option 样式选项
 * @param index 模型索引
 * \endif
 */
void SARibbonGalleryGroupItemDelegate::paintIconWithText(QPainter* painter,
                                                         const QStyleOptionViewItem& option,
                                                         const QModelIndex& index) const
{
    QStyledItemDelegate::paint(painter, option, index);
}

/**
 * \if ENGLISH
 * @brief Paint icon with word wrap text
 * @param painter Painter
 * @param option Style option
 * @param index Model index
 * \endif
 *
 * \if CHINESE
 * @brief 绘制带自动换行文本的图标
 * @param painter 画家
 * @param option 样式选项
 * @param index 模型索引
 * \endif
 */
void SARibbonGalleryGroupItemDelegate::paintIconWithTextWordWrap(QPainter* painter,
                                                                 const QStyleOptionViewItem& option,
                                                                 const QModelIndex& index) const
{
    QStyledItemDelegate::paint(painter, option, index);
}

/**
 * \if ENGLISH
 * @brief Get size hint
 * @param option Style option
 * @param index Model index
 * @return Size hint
 * \endif
 *
 * \if CHINESE
 * @brief 获取大小提示
 * @param option 样式选项
 * @param index 模型索引
 * @return 大小提示
 * \endif
 */
QSize SARibbonGalleryGroupItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(index);
    Q_UNUSED(option);
    return mGroup->gridSize();
}

//===================================================
// SARibbonGalleryGroupModel
//===================================================

/**
 * \if ENGLISH
 * @brief Constructor for SARibbonGalleryGroupModel
 * @param parent Parent object
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonGalleryGroupModel 构造函数
 * @param parent 父对象
 * \endif
 */
SARibbonGalleryGroupModel::SARibbonGalleryGroupModel(QObject* parent) : QAbstractListModel(parent)
{
}

/**
 * \if ENGLISH
 * @brief Destructor for SARibbonGalleryGroupModel
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonGalleryGroupModel 析构函数
 * \endif
 */
SARibbonGalleryGroupModel::~SARibbonGalleryGroupModel()
{
    clear();
}

/**
 * \if ENGLISH
 * @brief Get row count
 * @param parent Parent model index
 * @return Row count
 * \endif
 *
 * \if CHINESE
 * @brief 获取行数
 * @param parent 父模型索引
 * @return 行数
 * \endif
 */
int SARibbonGalleryGroupModel::rowCount(const QModelIndex& parent) const
{
    return (parent.isValid() ? 0 : mItems.size());
}

/**
 * \if ENGLISH
 * @brief Get item flags
 * @param index Model index
 * @return Item flags
 * \endif
 *
 * \if CHINESE
 * @brief 获取项目标志
 * @param index 模型索引
 * @return 项目标志
 * \endif
 */
Qt::ItemFlags SARibbonGalleryGroupModel::flags(const QModelIndex& index) const
{
    if (!index.isValid() || (index.row() >= mItems.size())) {
        return (Qt::NoItemFlags);
    }
    return (mItems.at(index.row())->flags());
}

/**
 * \if ENGLISH
 * @brief Get data at index
 * @param index Model index
 * @param role Data role
 * @return Data value
 * \endif
 *
 * \if CHINESE
 * @brief 获取索引处的数据
 * @param index 模型索引
 * @param role 数据角色
 * @return 数据值
 * \endif
 */
QVariant SARibbonGalleryGroupModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || (index.row() >= mItems.count())) {
        return (QVariant());
    }
    return (mItems.at(index.row())->data(role));
}

/**
 * \if ENGLISH
 * @brief Create model index
 * @param row Row
 * @param column Column
 * @param parent Parent model index
 * @return Model index
 * \endif
 *
 * \if CHINESE
 * @brief 创建模型索引
 * @param row 行
 * @param column 列
 * @param parent 父模型索引
 * @return 模型索引
 * \endif
 */
QModelIndex SARibbonGalleryGroupModel::index(int row, int column, const QModelIndex& parent) const
{
    if (hasIndex(row, column, parent)) {
        return (createIndex(row, column, mItems.at(row)));
    }
    return (QModelIndex());
}

/**
 * \if ENGLISH
 * @brief Set data at index
 * @param index Model index
 * @param value Data value
 * @param role Data role
 * @return True if successful
 * \endif
 *
 * \if CHINESE
 * @brief 设置索引处的数据
 * @param index 模型索引
 * @param value 数据值
 * @param role 数据角色
 * @return 成功返回true
 * \endif
 */
bool SARibbonGalleryGroupModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid() || (index.row() >= mItems.count())) {
        return (false);
    }

    mItems.at(index.row())->setData(role, value);
    return (true);
}

/**
 * \if ENGLISH
 * @brief Clear all items
 * \endif
 *
 * \if CHINESE
 * @brief 清空所有项目
 * \endif
 */
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

/**
 * \if ENGLISH
 * @brief Get item at row
 * @param row Row index
 * @return SARibbonGalleryItem pointer
 * \endif
 *
 * \if CHINESE
 * @brief 获取行处的项目
 * @param row 行索引
 * @return SARibbonGalleryItem指针
 * \endif
 */
SARibbonGalleryItem* SARibbonGalleryGroupModel::at(int row) const
{
    return (mItems.value(row));
}

/**
 * \if ENGLISH
 * @brief Insert item at row
 * @param row Insert position
 * @param item SARibbonGalleryItem pointer
 * \endif
 *
 * \if CHINESE
 * @brief 在指定行插入项目
 * @param row 插入位置
 * @param item SARibbonGalleryItem指针
 * \endif
 */
void SARibbonGalleryGroupModel::insert(int row, SARibbonGalleryItem* item)
{
    beginInsertRows(QModelIndex(), row, row);
    mItems.insert(row, item);
    endInsertRows();
}

/**
 * \if ENGLISH
 * @brief Take item at row
 * @param row Row index
 * @return SARibbonGalleryItem pointer
 * \endif
 *
 * \if CHINESE
 * @brief 取出指定行的项目
 * @param row 行索引
 * @return SARibbonGalleryItem指针
 * \endif
 */
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
 * \if ENGLISH
 * @brief Append item
 * @param item SARibbonGalleryItem pointer (memory managed by SARibbonGalleryGroupModel)
 * \endif
 *
 * \if CHINESE
 * @brief 插入条目，item的内存由SARibbonGalleryGroupModel管理
 * @param item SARibbonGalleryItem指针
 * \endif
 */
void SARibbonGalleryGroupModel::append(SARibbonGalleryItem* item)
{
    beginInsertRows(QModelIndex(), mItems.count(), mItems.count() + 1);

    mItems.append(item);
    endInsertRows();
}

/**
 * \if ENGLISH
 * @brief Remove item by action object name
 * @param act_object_name Action object name
 * @return Result of removal operation
 * \endif
 *
 * \if CHINESE
 * @brief 通过QAction的对象名删除内部GalleryItem
 * @param act_object_name 动作对象名
 * @return 删除操作的结果
 * \endif
 */
bool SARibbonGalleryGroupModel::remove(const QString& act_object_name)
{
    for (int index = 0; index < mItems.size(); ++index) {
        if (mItems.at(index)->action()->objectName() == act_object_name) {
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
 * \if ENGLISH
 * @brief Get item count
 * @return Item count
 * \endif
 *
 * \if CHINESE
 * @brief 获取条目数量
 * @return 条目数量
 * \endif
 */
int SARibbonGalleryGroupModel::itemSize() const
{
    return mItems.size();
}

//===================================================
// SARibbonGalleryGroup
//===================================================
/**
 * \if ENGLISH
 * @brief Constructor for SARibbonGalleryGroup
 * @param w Parent widget
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonGalleryGroup 构造函数
 * @param w 父窗口
 * \endif
 */
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

/**
 * \if ENGLISH
 * @brief Destructor for SARibbonGalleryGroup
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonGalleryGroup 析构函数
 * \endif
 */
SARibbonGalleryGroup::~SARibbonGalleryGroup()
{
}

/**
 * \if ENGLISH
 * @brief Recalculate grid and icon sizes
 * \endif
 *
 * \if CHINESE
 * @brief 重新计算grid和icon的尺寸
 * \endif
 */
void SARibbonGalleryGroup::recalcGridSize()
{
    recalcGridSize(height());
}

/**
 * \if ENGLISH
 * @brief Recalculate grid and icon sizes
 * @param galleryHeight Gallery height
 * \endif
 *
 * \if CHINESE
 * @brief 重新计算grid和icon的尺寸
 * @param galleryHeight 画廊高度
 * \endif
 */
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

/**
 * \if ENGLISH
 * @brief Set the default preset style
 * @param style Gallery group style
 * \endif
 *
 * \if CHINESE
 * @brief 设置默认的预设样式
 * @param style 画廊组样式
 * \endif
 */
void SARibbonGalleryGroup::setGalleryGroupStyle(SARibbonGalleryGroup::GalleryGroupStyle style)
{
    d_ptr->mPreStyle = style;
    setWordWrap(style == IconWithWordWrapText);
    recalcGridSize();
}

/**
 * \if ENGLISH
 * @brief Get the gallery group style
 * @return Gallery group style
 * \endif
 *
 * \if CHINESE
 * @brief 获取画廊组样式
 * @return 画廊组样式
 * \endif
 */
SARibbonGalleryGroup::GalleryGroupStyle SARibbonGalleryGroup::galleryGroupStyle() const
{
    return d_ptr->mPreStyle;
}

/**
 * \if ENGLISH
 * @brief Add item with text and icon
 * @param text Item text
 * @param icon Item icon
 * \endif
 *
 * \if CHINESE
 * @brief 添加带文本和图标的项目
 * @param text 项目文本
 * @param icon 项目图标
 * \endif
 */
void SARibbonGalleryGroup::addItem(const QString& text, const QIcon& icon)
{
    if (nullptr == groupModel()) {
        return;
    }
    addItem(new SARibbonGalleryItem(text, icon));
}

/**
 * \if ENGLISH
 * @brief Add item
 * @param item SARibbonGalleryItem pointer (memory managed by SARibbonGalleryGroup)
 * \endif
 *
 * \if CHINESE
 * @brief 添加一个条目
 * @param item 条目的内存所有权归属SARibbonGalleryGroup管理
 * \endif
 */
void SARibbonGalleryGroup::addItem(SARibbonGalleryItem* item)
{
    if (nullptr == groupModel()) {
        return;
    }
    groupModel()->append(item);
}

/**
 * \if ENGLISH
 * @brief Add action item
 * @param act QAction pointer
 * \endif
 *
 * \if CHINESE
 * @brief 添加动作项目
 * @param act QAction指针
 * \endif
 */
void SARibbonGalleryGroup::addActionItem(QAction* act)
{
    if (nullptr == groupModel()) {
        return;
    }
    d_ptr->mActionGroup->addAction(act);
    groupModel()->append(new SARibbonGalleryItem(act));
}

/**
 * \if ENGLISH
 * @brief Add action item list
 * @param acts QAction list
 * \endif
 *
 * \if CHINESE
 * @brief 添加动作项目列表
 * @param acts QAction列表
 * \endif
 */
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
 * \if ENGLISH
 * @brief Remove QAction from current GalleryGroup
 * @param act QAction pointer
 * @return Result of removal operation
 * \endif
 *
 * \if CHINESE
 * @brief 从当前GalleryGroup中移除QAction
 * @param act QAction指针
 * @return 删除操作的结果
 * \endif
 */
bool SARibbonGalleryGroup::removeActionItem(QAction* act)
{
    if (nullptr == groupModel()) {
        return false;
    } else {
        d_ptr->mActionGroup->removeAction(act);
        return groupModel()->remove(act->objectName());
    }
}

/**
 * \if ENGLISH
 * @brief Setup group model
 * @return SARibbonGalleryGroupModel pointer
 * \endif
 *
 * \if CHINESE
 * @brief 构建一个model，这个model的父类是SARibbonGalleryGroup，如果要共享model，需要手动处理model的父类
 * @return SARibbonGalleryGroupModel指针
 * \endif
 */
SARibbonGalleryGroupModel* SARibbonGalleryGroup::setupGroupModel()
{
    SARibbonGalleryGroupModel* m = new SARibbonGalleryGroupModel(this);
    setModel(m);
    return m;
}

/**
 * \if ENGLISH
 * @brief Get group model
 * @return SARibbonGalleryGroupModel pointer
 * \endif
 *
 * \if CHINESE
 * @brief 获取组模型
 * @return SARibbonGalleryGroupModel指针
 * \endif
 */
SARibbonGalleryGroupModel* SARibbonGalleryGroup::groupModel() const
{
    return (qobject_cast< SARibbonGalleryGroupModel* >(model()));
}

/**
 * \if ENGLISH
 * @brief Set group title
 * @param title Group title
 * \endif
 *
 * \if CHINESE
 * @brief 设置组标题
 * @param title 组标题
 * \endif
 */
void SARibbonGalleryGroup::setGroupTitle(const QString& title)
{
    d_ptr->mGroupTitle = title;
    Q_EMIT groupTitleChanged(d_ptr->mGroupTitle);
}

/**
 * \if ENGLISH
 * @brief Get group title
 * @return Group title
 * \endif
 *
 * \if CHINESE
 * @brief 获取组标题
 * @return 组标题
 * \endif
 */
QString SARibbonGalleryGroup::groupTitle() const
{
    return (d_ptr->mGroupTitle);
}

/**
 * \if ENGLISH
 * @brief Select item by index
 * @param i Item index
 * \endif
 *
 * \if CHINESE
 * @brief 通过索引选择项目
 * @param i 项目索引
 * \endif
 */
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
 * \if ENGLISH
 * @brief Set display row
 * @param r Display row
 * \endif
 *
 * \if CHINESE
 * @brief 设置显示的行数
 * @param r 显示行数
 * \endif
 */
void SARibbonGalleryGroup::setDisplayRow(DisplayRow r)
{
    d_ptr->mDisplayRow = r;
    recalcGridSize();
}

/**
 * \if ENGLISH
 * @brief Get display row
 * @return Display row
 * \endif
 *
 * \if CHINESE
 * @brief Gallery显示的行数
 * @return 显示行数
 * \endif
 */
SARibbonGalleryGroup::DisplayRow SARibbonGalleryGroup::displayRow() const
{
    return d_ptr->mDisplayRow;
}

/**
 * \if ENGLISH
 * @brief Set grid minimum width
 * @param w Minimum width
 * \endif
 *
 * \if CHINESE
 * @brief 设置grid最小的宽度，默认为0（不限制）
 * @param w 最小宽度
 * \endif
 */
void SARibbonGalleryGroup::setGridMinimumWidth(int w)
{
    d_ptr->mGridMinimumWidth = w;
}

/**
 * \if ENGLISH
 * @brief Get grid minimum width
 * @return Minimum width
 * \endif
 *
 * \if CHINESE
 * @brief grid最小的宽度，默认为0（不限制）
 * @return 最小宽度
 * \endif
 */
int SARibbonGalleryGroup::gridMinimumWidth() const
{
    return d_ptr->mGridMinimumWidth;
}

/**
 * \if ENGLISH
 * @brief Set grid maximum width
 * @param w Maximum width
 * \endif
 *
 * \if CHINESE
 * @brief 设置grid最大的宽度，默认为0（不限制）
 * @param w 最大宽度
 * \endif
 */
void SARibbonGalleryGroup::setGridMaximumWidth(int w)
{
    d_ptr->mGridMaximumWidth = w;
}

/**
 * \if ENGLISH
 * @brief Get grid maximum width
 * @return Maximum width
 * \endif
 *
 * \if CHINESE
 * @brief grid最大的的宽度，默认为0（不限制）
 * @return 最大宽度
 * \endif
 */
int SARibbonGalleryGroup::gridMaximumWidth() const
{
    return d_ptr->mGridMaximumWidth;
}

/**
 * \if ENGLISH
 * @brief Get action group
 * @return QActionGroup pointer
 * \endif
 *
 * \if CHINESE
 * @brief 获取SARibbonGalleryGroup管理的actiongroup
 * @return QActionGroup指针
 * \endif
 */
QActionGroup* SARibbonGalleryGroup::actionGroup() const
{
    return d_ptr->mActionGroup;
}

/**
 * \if ENGLISH
 * @brief Calculate grid row count
 * @return Row count
 * \endif
 *
 * \if CHINESE
 * @brief 计算行数
 * @return 行数
 * \endif
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

/**
 * \if ENGLISH
 * @brief Calculate grid column count
 * @return Column count
 * \endif
 *
 * \if CHINESE
 * @brief 计算列数
 * @return 列数
 * \endif
 */
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
 * \if ENGLISH
 * @brief Calculate preferred height for width
 * @param w Width
 * @return Preferred height
 * \endif
 *
 * \if CHINESE
 * @brief 计算最紧凑的高度
 * @param w 宽度
 * @return 首选高度
 * \endif
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

/**
 * \if ENGLISH
 * @brief Check if has height for width
 * @return True if has height for width
 * \endif
 *
 * \if CHINESE
 * @brief 检查是否具有宽度对应的高度
 * @return 如果具有宽度对应的高度返回true
 * \endif
 */
bool SARibbonGalleryGroup::hasHeightForWidth() const
{
    return true;
}

/**
 * \if ENGLISH
 * @brief Get height for width
 * @param w Width
 * @return Height
 * \endif
 *
 * \if CHINESE
 * @brief 获取宽度对应的高度
 * @param w 宽度
 * @return 高度
 * \endif
 */
int SARibbonGalleryGroup::heightForWidth(int w) const
{
    return preferredHeightForWidth(w);
}

/**
 * \if ENGLISH
 * @brief Handle item clicked event
 * @param index Model index
 * \endif
 *
 * \if CHINESE
 * @brief 处理项目点击事件
 * @param index 模型索引
 * \endif
 */
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

/**
 * \if ENGLISH
 * @brief Handle item entered event
 * @param index Model index
 * \endif
 *
 * \if CHINESE
 * @brief 处理项目进入事件
 * @param index 模型索引
 * \endif
 */
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
