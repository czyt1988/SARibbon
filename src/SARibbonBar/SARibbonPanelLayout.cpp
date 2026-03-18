#include "SARibbonPanelLayout.h"
#include "SARibbonPanelOptionButton.h"
#include "SARibbonSeparatorWidget.h"
#include "SARibbonElementManager.h"
#include <QWidgetAction>
#include <QQueue>
#include "SARibbonPanel.h"
#include "SARibbonPanelItem.h"
#include "SARibbonQt5Compat.hpp"

#ifndef SARibbonPanelLayout_DEBUG_PRINT
#define SARibbonPanelLayout_DEBUG_PRINT 0
#endif

#if SARibbonPanelLayout_DEBUG_PRINT
#ifndef SARibbonPanelLayout_HELP_DRAW_RECT
#define SARibbonPanelLayout_HELP_DRAW_RECT(p, rect)                                                                    \
    do {                                                                                                               \
        p.save();                                                                                                      \
        QPen _pen(Qt::DashLine);                                                                                       \
        _pen.setColor(Qt::blue);                                                                                       \
        p.setPen(_pen);                                                                                                \
        p.setBrush(QBrush());                                                                                          \
        p.drawRect(rect);                                                                                              \
        p.restore();                                                                                                   \
    } while (0)
#endif  // SARibbonPanelLayout_HELP_DRAW_RECT
#endif  // SARibbonPanelLayout_DEBUG_PRINT
/**
 * \if ENGLISH
 * @brief Constructs a SARibbonPanelLayout instance
 * @param p Parent widget
 * \endif
 *
 * \if CHINESE
 * @brief 构造一个 SARibbonPanelLayout 实例
 * @param p 父窗口部件
 * \endif
 */
SARibbonPanelLayout::SARibbonPanelLayout(QWidget* p) : QLayout(p), mColumnCount(0), mExpandFlag(false), mDirty(true)
{
    setSpacing(1);
    SARibbonPanel* tb = qobject_cast< SARibbonPanel* >(p);

    if (!tb) {
        return;
    }
}

/**
 * \if ENGLISH
 * @brief Destructor for SARibbonPanelLayout
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonPanelLayout析构函数
 * \endif
 */
SARibbonPanelLayout::~SARibbonPanelLayout()
{
    // 参考QToolBarLayout
    while (!mItems.isEmpty()) {
        SARibbonPanelItem* item = mItems.takeFirst();
        if (QWidgetAction* widgetAction = qobject_cast< QWidgetAction* >(item->action)) {
            if (item->customWidget) {
                widgetAction->releaseWidget(item->widget());
            }
        }
        delete item;
    }
}

/**
 * \if ENGLISH
 * @brief Finds the index of an action in the layout
 * @param action The action to find
 * @return The index of the action, or -1 if not found
 * \endif
 *
 * \if CHINESE
 * @brief 在布局中查找一个action的索引
 * @param action 要查找的action
 * @return action的索引，如果未找到则返回-1
 * \endif
 */
int SARibbonPanelLayout::indexByAction(QAction* action) const
{
    for (int i = 0; i < mItems.count(); ++i) {
        if (mItems.at(i)->action == action) {
            return (i);
        }
    }
    return (-1);
}

/**
 * \if ENGLISH
 * @brief Gets the SARibbonPanel that owns this layout
 * @return A pointer to the parent SARibbonPanel, or nullptr if not found
 * \endif
 *
 * \if CHINESE
 * @brief 获取拥有此布局的SARibbonPanel
 * @return 指向父SARibbonPanel的指针，如果未找到则返回nullptr
 * \endif
 */
SARibbonPanel* SARibbonPanelLayout::ribbonPanel() const
{
    return qobject_cast< SARibbonPanel* >(parentWidget());
}

/**
 * \if ENGLISH
 * @brief Adds an item to the layout (not supported)
 *
 * This layout only accepts items created from `QAction` via `insertAction`. Calling this function
 * directly will result in a warning.
 *
 * @param item The layout item to add
 * \endif
 *
 * \if CHINESE
 * @brief 向布局添加一个项目（不支持）
 *
 * 此布局仅接受通过 `insertAction` 从 `QAction` 创建的项目。直接调用此函数将导致警告。
 *
 * @param item 要添加的布局项目
 * \endif
 */
void SARibbonPanelLayout::addItem(QLayoutItem* item)
{
    Q_UNUSED(item);
    qWarning("SARibbonPanelLayout::addItem(): please use addAction() instead");
    return;
}

/**
 * \if ENGLISH
 * @brief Inserts an action at a specific index
 *
 * This is the primary method for adding content to the layout. It creates a `SARibbonPanelItem`
 * from the `QAction` and inserts it at the specified position.
 *
 * @param index The index at which to insert the action
 * @param act The action to insert
 * @param rp The row proportion for the action
 * \endif
 *
 * \if CHINESE
 * @brief 在指定索引处插入一个action
 *
 * 这是向布局添加内容的主要方法。它从 `QAction` 创建一个 `SARibbonPanelItem` 并将其插入到指定位置。
 *
 * @param index 插入action的索引
 * @param act 要插入的action
 * @param rp action的行占比
 * \endif
 */
void SARibbonPanelLayout::insertAction(int index, QAction* act, SARibbonPanelItem::RowProportion rp)
{
    index                   = qMax(0, index);
    index                   = qMin(mItems.count(), index);
    SARibbonPanelItem* item = createItem(act, rp);

    if (item) {
        mItems.insert(index, item);
        // 标记需要重新计算尺寸
        invalidate();
    }
}

/**
 * \if ENGLISH
 * @brief Sets the option action for the panel
 *
 * The option action is displayed as a button in the panel's title area.
 * Pass `nullptr` to remove the current option action.
 *
 * @param action The option action, or nullptr to remove
 * \endif
 *
 * \if CHINESE
 * @brief 为面板设置选项action
 *
 * 选项action显示在面板标题区域的一个按钮中。
 * 传入 `nullptr` 以移除当前的选项action。
 *
 * @param action 选项action，或传入nullptr以移除
 * \endif
 */
void SARibbonPanelLayout::setOptionAction(QAction* action)
{
    SARibbonPanel* p = ribbonPanel();
    if (!p) {
        return;
    }
    if (action) {
        // 创建option action
        if (nullptr == mOptionActionBtn) {
            mOptionActionBtn = RibbonSubElementFactory->createRibbonPanelOptionButton(p);
            QObject::connect(mOptionActionBtn, &SARibbonToolButton::triggered, p, &SARibbonPanel::actionTriggered);
            // 确保m_optionActionBtn在label之上
            if (mTitleLabel) {
                mTitleLabel->stackUnder(mOptionActionBtn);
            }
        }
        mOptionActionBtn->setDefaultAction(action);
        if (action->icon().isNull()) {
            mOptionActionBtn->setIcon(QIcon(":/SARibbon/image/resource/ribbonPanelOptionButton.png"));
        }
        // 标记需要重新计算尺寸
        invalidate();
    } else {
        // 取消option action
        if (mOptionActionBtn) {
            mOptionActionBtn->hide();
            mOptionActionBtn->deleteLater();
            mOptionActionBtn = nullptr;
            // 标记需要重新计算尺寸
            invalidate();
        }
    }
}

/**
 * \if ENGLISH
 * @brief Checks if an option action is set
 * @return true if an option action exists; otherwise false
 * \endif
 *
 * \if CHINESE
 * @brief 检查是否设置了选项action
 * @return 如果存在选项action则返回true；否则返回false
 * \endif
 */
bool SARibbonPanelLayout::isHaveOptionAction() const
{
    return (mOptionActionBtn != nullptr);
}

/**
 * \if ENGLISH
 * @brief Retrieves the item at the specified index
 * @param index The index of the item
 * @return The layout item, or nullptr if index is invalid
 * \endif
 *
 * \if CHINESE
 * @brief 获取指定索引处的项目
 * @param index 项目的索引
 * @return 布局项目，如果索引无效则返回nullptr
 * \endif
 */
QLayoutItem* SARibbonPanelLayout::itemAt(int index) const
{
    if ((index < 0) || (index >= mItems.count())) {
        return (nullptr);
    }
    return (mItems.at(index));
}

/**
 * \if ENGLISH
 * @brief Removes and returns the item at the specified index
 * @param index The index of the item to remove
 * @return The removed layout item, or nullptr if index is invalid
 * \endif
 *
 * \if CHINESE
 * @brief 移除并返回指定索引处的项目
 * @param index 要移除的项目的索引
 * @return 被移除的布局项目，如果索引无效则返回nullptr
 * \endif
 */
QLayoutItem* SARibbonPanelLayout::takeAt(int index)
{
    if ((index < 0) || (index >= mItems.count())) {
        return (nullptr);
    }
    SARibbonPanelItem* item = mItems.takeAt(index);

    QWidgetAction* widgetAction = qobject_cast< QWidgetAction* >(item->action);

    if ((widgetAction != 0) && item->customWidget) {
        widgetAction->releaseWidget(item->widget());
    } else {
        // destroy the QToolButton/QToolBarSeparator
        item->widget()->hide();
        item->widget()->deleteLater();
    }

    invalidate();
    return (item);
}

/**
 * \if ENGLISH
 * @brief Gets the number of items in the layout
 * @return The item count
 * \endif
 *
 * \if CHINESE
 * @brief 获取布局中项目的数量
 * @return 项目数量
 * \endif
 */
int SARibbonPanelLayout::count() const
{
    return (mItems.count());
}

/**
 * \if ENGLISH
 * @brief Checks if the layout is empty
 * @return true if the layout has no items; otherwise false
 * \endif
 *
 * \if CHINESE
 * @brief 检查布局是否为空
 * @return 如果布局没有项目则返回true；否则返回false
 * \endif
 */
bool SARibbonPanelLayout::isEmpty() const
{

    return (mItems.isEmpty());
}

/**
 * \if ENGLISH
 * @brief Invalidates the layout, marking it as dirty
 *
 * This forces a recalculation of the layout geometry on the next update.
 *
 * \endif
 *
 * \if CHINESE
 * @brief 使布局失效，将其标记为“脏”
 *
 * 这会强制在下次更新时重新计算布局几何形状。
 *
 * \endif
 */
void SARibbonPanelLayout::invalidate()
{
    mDirty = true;
    QLayout::invalidate();
}

/**
 * \if ENGLISH
 * @brief Returns the directions in which the layout can expand
 * @return The expanding directions (always Qt::Horizontal for this layout)
 * \endif
 *
 * \if CHINESE
 * @brief 返回布局可以扩展的方向
 * @return 扩展方向（此布局始终为Qt::Horizontal）
 * \endif
 */
Qt::Orientations SARibbonPanelLayout::expandingDirections() const
{
    return (Qt::Horizontal);
}

/**
 * \if ENGLISH
 * @brief Gets the minimum size of the layout
 * @return The minimum size
 * \endif
 *
 * \if CHINESE
 * @brief 获取布局的最小尺寸
 * @return 最小尺寸
 * \endif
 */
QSize SARibbonPanelLayout::minimumSize() const
{
    return (mSizeHint);
}

/**
 * \if ENGLISH
 * @brief Gets the size hint of the layout
 * @return The size hint
 * \endif
 *
 * \if CHINESE
 * @brief 获取布局的尺寸提示
 * @return 尺寸提示
 * \endif
 */
QSize SARibbonPanelLayout::sizeHint() const
{
#if SARibbonPanelLayout_DEBUG_PRINT
    if (SARibbonPanel* panel = ribbonPanel()) {
        qDebug() << "| |-SARibbonPanelLayout sizeHint,sizeHint = " << m_sizeHint;
    }
#endif
    return (mSizeHint);
}

/**
 * \if ENGLISH
 * @brief Retrieves the SARibbonPanelItem associated with an action
 * @param action The action to query
 * @return The associated item, or nullptr if not found
 * \endif
 *
 * \if CHINESE
 * @brief 获取与action关联的SARibbonPanelItem
 * @param action 要查询的action
 * @return 关联的项目，如果未找到则返回nullptr
 * \endif
 */
SARibbonPanelItem* SARibbonPanelLayout::panelItem(QAction* action) const
{
    int index = indexByAction(action);

    if (index >= 0) {
        return (mItems[ index ]);
    }
    return (nullptr);
}

/**
 * \if ENGLISH
 * @brief Gets the last item added to the layout
 * @return The last item, or nullptr if the layout is empty
 * \endif
 *
 * \if CHINESE
 * @brief 获取最后添加到布局的项目
 * @return 最后一个项目，如果布局为空则返回nullptr
 * \endif
 */
SARibbonPanelItem* SARibbonPanelLayout::lastItem() const
{
    if (mItems.isEmpty()) {
        return (nullptr);
    }
    return (mItems.last());
}

/**
 * \if ENGLISH
 * @brief Gets the widget associated with the last item
 * @return The widget, or nullptr if not found
 * \endif
 *
 * \if CHINESE
 * @brief 获取与最后一个项目关联的窗口部件
 * @return 窗口部件，如果未找到则返回nullptr
 * \endif
 */
QWidget* SARibbonPanelLayout::lastWidget() const
{
    SARibbonPanelItem* item = lastItem();

    if (item) {
        return (item->widget());
    }
    return (nullptr);
}

/**
 * \if ENGLISH
 * @brief Moves an item from one index to another
 * @param from The current index of the item
 * @param to The new index for the item
 * \endif
 *
 * \if CHINESE
 * @brief 将一个项目从一个索引移动到另一个索引
 * @param from 项目的当前索引
 * @param to 项目的新索引
 * \endif
 */
void SARibbonPanelLayout::move(int from, int to)
{
    if (from == to) {
        return;
    }
    if (to < 0) {
        to = 0;
    }
    if (to >= count()) {
        to = count() - 1;
    }
    mItems.move(from, to);
    invalidate();
}

/**
 * \if ENGLISH
 * @brief Checks if the layout needs to be relayouted
 * @return true if the layout is dirty and needs relayouting, false otherwise
 * \endif
 *
 * \if CHINESE
 * @brief 判断是否需要重新布局
 * @return 如果布局需要重新布局则返回true，否则返回false
 * \endif
 */
bool SARibbonPanelLayout::isDirty() const
{
    return (mDirty);
}

/**
 * \if ENGLISH
 * @brief Updates the geometry array using the current geometry
 * \endif
 *
 * \if CHINESE
 * @brief 使用当前几何形状更新几何数组
 * \endif
 */
void SARibbonPanelLayout::updateGeomArray()
{
    updateGeomArray(geometry());
}

/**
 * \if ENGLISH
 * @brief Lays out all actions
 * \endif
 *
 * \if CHINESE
 * @brief 布局所有action
 * \endif
 */
void SARibbonPanelLayout::doLayout()
{
#if SARibbonPanelLayout_DEBUG_PRINT
    if (SARibbonPanel* panel = ribbonPanel()) {
        qDebug() << "| |-SARibbonPanelLayout layoutActions,panel name = " << panel->panelName();
    }
#endif
    if (isDirty()) {
        updateGeomArray();
    }
    QWidget* par = parentWidget();
    if (!par) {
        return;
    }
    QList< QWidget* > showWidgets, hideWidgets;
    for (SARibbonPanelItem* item : sa_as_const(mItems)) {
        if (item->isEmpty()) {
            hideWidgets << item->widget();
        } else {
            // 在category发现item->setGeometry有点奇怪的现象，这里统一使用item->widget->setgeo
            // item->setGeometry(item->itemWillSetGeometry);
            if (item->widget()) {
                item->widget()->setGeometry(item->itemWillSetGeometry);
            }
            showWidgets << item->widget();
        }
    }

    // 不在上面那里进行show和hide因为这会触发SARibbonPanelLayout的重绘，导致循环绘制，非常影响效率
    for (QWidget* w : sa_as_const(showWidgets)) {
        if (!w->isVisible())
            w->show();
    }
    for (QWidget* w : sa_as_const(hideWidgets)) {
        if (w->isVisible())
            w->hide();
    }

    // 布局label
    if (mTitleLabel) {
        if (isEnableShowPanelTitle()) {
            mTitleLabel->setGeometry(mTitleLabelGeometry);
            if (!mTitleLabel->isVisibleTo(par)) {
                mTitleLabel->show();
            }
        } else {
            if (mTitleLabel->isVisibleTo(par)) {
                mTitleLabel->hide();
            }
        }
    }
    // 布局m_optionActionBtn
    if (mOptionActionBtn) {
        if (!mOptionActionBtn->isVisibleTo(par)) {
            mOptionActionBtn->show();
        }
        mOptionActionBtn->setGeometry(mOptionActionBtnGeometry);
        mOptionActionBtn->setIconSize(QSize(mOptionActionBtnGeometry.width(), mOptionActionBtnGeometry.height()));
    }
}

/**
 * \if ENGLISH
 * @brief Converts an action to an item
 *
 * This function references QToolBarItem *QToolBarLayout::createItem(QAction *action)
 *
 * For regular QAction, this function creates a SARibbonToolButton. The type of SARibbonToolButton
 * is determined by SARibbonPanelItem::RowProportion.
 *
 * @param action The action to convert
 * @param rp Row proportion
 * @return The converted SARibbonPanelItem
 * @note Each SARibbonPanelItem will eventually carry a widget. If a QWidgetAction is passed,
 * it will directly use the widget carried by QWidgetAction. Otherwise, it will internally generate a SARibbonToolButton.
 * \endif
 *
 * \if CHINESE
 * @brief 把action转换为item
 *
 * 此函数参考QToolBarItem *QToolBarLayout::createItem(QAction *action)
 *
 * 对于普通QAction，此函数会创建SARibbonToolButton，SARibbonToolButton的类型参考SARibbonPanelItem::RowProportion，
 * @param action 要转换的action
 * @param rp 行高占比情况
 * @return 转换的SARibbonPanelItem
 * @note 每个SARibbonPanelItem最终都会携带一个widget，传入的是QWidgetAction的话，会直接使用QWidgetAction带的widget，
 * 否则会内部生成一个SARibbonToolButton
 * \endif
 */
SARibbonPanelItem* SARibbonPanelLayout::createItem(QAction* action, SARibbonPanelItem::RowProportion rp)
{
    bool customWidget    = false;
    QWidget* widget      = nullptr;
    SARibbonPanel* panel = qobject_cast< SARibbonPanel* >(parentWidget());

    if (!panel) {
        // 在没有panel这个函数会返回nullptr
        return (nullptr);
    }
    if (QWidgetAction* widgetAction = qobject_cast< QWidgetAction* >(action)) {
        widget = widgetAction->requestWidget(panel);
        if (widget != nullptr) {
            widget->setAttribute(Qt::WA_LayoutUsesWidgetRect);
            customWidget =
                true;  // 标记为true，在移除的时候是不会对这个窗口进行删除，false默认会进行删除如SARibbonSeparatorWidget和SARibbonToolButton
            // 明确设置父对象，确保 releaseWidget 时能正确工作
            widget->setParent(panel);
        }
    } else if (action->isSeparator()) {
        SARibbonSeparatorWidget* sep = RibbonSubElementFactory->createRibbonSeparatorWidget(panel);
        widget                       = sep;
    }
    // 不是widget，自动生成SARibbonToolbutton
    if (!widget) {
        SARibbonToolButton::RibbonButtonType buttonType =
            ((rp == SARibbonPanelItem::Large) ? SARibbonToolButton::LargeButton : SARibbonToolButton::SmallButton);

        SARibbonToolButton* button = RibbonSubElementFactory->createRibbonToolButton(panel);
        button->setFocusPolicy(Qt::NoFocus);
        button->setButtonType(buttonType);
        button->setDefaultAction(action);
        button->setIconSize(mSmallToolButtonIconSize);
        button->setLargeIconSize(mLargeToolButtonIconSize);
        button->setEnableWordWrap(isEnableWordWrap());
        button->setButtonMaximumAspectRatio(mButtonMaximumAspectRatio);
        // 属性设置
        QToolButton::ToolButtonPopupMode popMode = SARibbonPanel::getActionToolButtonPopupModeProperty(action);
        button->setPopupMode(popMode);
        // 根据QAction的属性设置按钮的大小

        QObject::connect(button, &SARibbonToolButton::triggered, panel, &SARibbonPanel::actionTriggered);
        widget = button;
    }
    // 这时总会有widget
    widget->hide();
    SARibbonPanelItem* result = new SARibbonPanelItem(widget);

    result->rowProportion = rp;
    result->customWidget  = customWidget;
    result->action        = action;
    return (result);
}

/**
 * \if ENGLISH
 * @brief Updates the geometry array
 * @param setrect The rectangle to update
 * \endif
 *
 * \if CHINESE
 * @brief 更新尺寸
 * @param setrect 要更新的矩形
 * \endif
 */
void SARibbonPanelLayout::updateGeomArray(const QRect& setrect)
{
    SARibbonPanel* panel = qobject_cast< SARibbonPanel* >(parentWidget());

    if (!panel) {
        return;
    }

    const int height     = setrect.height();
    const QMargins& mag  = contentsMargins();
    const int spacing    = this->spacing();
    const int spacingRow = 1;  // 高度间距，也就是行间距，不同行之间的距离
    int x                = mag.left();
    const int yBegin     = mag.top();
    int titleH           = (mTitleHeight >= 0) ? mTitleHeight : 0;  // 防止负数影响
    int titleSpace       = (mTitleHeight >= 0) ? mTitleSpace : 0;   // 对于没有标题的情况，spacing就不生效
    if (!isEnableShowPanelTitle()) {
        titleH     = 0;
        titleSpace = 0;
    }
    // 获取panel的布局模式 3行或者2行
    //  rowcount 是ribbon的行，有2行和3行两种
    const short rowCount = (panel->panelLayoutMode() == SARibbonPanel::ThreeRowMode) ? 3 : 2;
    // largeHeight是对应large占比的高度
    const int largeHeight = qMax(height - mag.bottom() - mag.top() - titleH - titleSpace, 2);  // 大按钮高度不小于2

    mLargeHeight = largeHeight;
    // 计算smallHeight的高度
    const int smallHeight = qMax((largeHeight - (rowCount - 1) * spacingRow) / rowCount, 1);
    // Medium行的y位置
    const int yMediumRow0 = (2 == rowCount) ? yBegin : (yBegin + ((largeHeight - 2 * smallHeight) / 3));
    const int yMediumRow1 = (2 == rowCount) ? (yBegin + smallHeight + spacingRow)
                                            : (yBegin + ((largeHeight - 2 * smallHeight) / 3) * 2 + smallHeight);
    // Small行的y位置
    const int ySmallRow0 = yBegin;
    const int ySmallRow1 = yBegin + smallHeight + spacingRow;
    const int ySmallRow2 = yBegin + 2 * (smallHeight + spacingRow);
    // row用于记录下个item应该属于第几行，item->rowIndex用于记录当前处于第几行，
    // item->rowIndex主要用于SARibbonPanelItem::Medium
    short row  = 0;
    int column = 0;
    // 记录每列最大的宽度
    int columMaxWidth = 0;
    // 记录总宽度
    int totalWidth = 0;

    int itemCount = mItems.count();

#if SARibbonPanelLayout_DEBUG_PRINT
    QString debug_print__log__;
#endif
    // 本列第一、二行占比
    SARibbonPanelItem::RowProportion thisColumnRP0 = SARibbonPanelItem::None;
    SARibbonPanelItem* lastGeomItem                = nullptr;  // 记录最后一个设置位置的item
    for (int i = 0; i < itemCount; ++i) {
        SARibbonPanelItem* item = mItems.at(i);
        if (item->isEmpty()) {
            // 如果是hide就直接跳过
            item->rowIndex    = -1;
            item->columnIndex = -1;
            continue;
        }
        // 当开始新的一列时，重置列宽
        if (row == 0) {
            columMaxWidth = 0;
        }
        QSize hint = item->sizeHint();
#if SARibbonPanelLayout_DEBUG_PRINT
        if (SARibbonToolButton* tb = qobject_cast< SARibbonToolButton* >(item->widget())) {
            auto ss__ = tb->sizeHint();
            debug_print__log__ += QString("| | |-[%1]SARibbonToolButton.sizeHint=(%2,%3),ButtonText=%4\n")
                                      .arg(i)
                                      .arg(ss__.width())
                                      .arg(ss__.height())
                                      .arg(tb->text());
        }
#endif
        Qt::Orientations exp = item->expandingDirections();
        if (item->widget()) {
            // 有窗口是水平扩展，则标记为扩展
            if ((item->widget()->sizePolicy().horizontalPolicy() & QSizePolicy::ExpandFlag)) {
                mExpandFlag = true;
            }
        }
        SARibbonPanelItem::RowProportion rp = item->rowProportion;
        if (SARibbonPanelItem::None == rp) {
            // 为定义行占比但是垂直扩展，就定义为Large占比，否则就是small占比
            if (exp & Qt::Vertical) {
                rp = SARibbonPanelItem::Large;
            } else {
                rp = SARibbonPanelItem::Small;
            }
        }
        // 开始根据占比和layoutmode来布局
        switch (rp) {
        case SARibbonPanelItem::Large: {
            // ！！在Large，如果不是处于新列的第一行，就需要进行换列处理
            // 把large一直设置在下一列的开始
            if (row != 0) {
                x += (columMaxWidth + spacing);
                ++column;
            }
            //
            item->rowIndex            = 0;
            item->columnIndex         = column;
            item->itemWillSetGeometry = QRect(x, yBegin, hint.width(), largeHeight);
            columMaxWidth             = hint.width();
            // 换列，x自动递增到下个坐标，列数增加，行数归零，最大列宽归零
            x += (columMaxWidth + spacing);
            row           = 0;
            columMaxWidth = 0;
            ++column;
        } break;

        case SARibbonPanelItem::Medium: {
            // 2行模式下Medium和small等价
            if (2 == rowCount) {
                if (0 == row) {
                    item->rowIndex            = 0;
                    item->columnIndex         = column;
                    item->itemWillSetGeometry = QRect(x, yMediumRow0, hint.width(), smallHeight);
                    thisColumnRP0             = SARibbonPanelItem::Medium;
                    columMaxWidth             = hint.width();
                    // 下个row为1
                    row = 1;
                    // x不变
                } else {
                    item->rowIndex            = 1;
                    item->columnIndex         = column;
                    item->itemWillSetGeometry = QRect(x, yMediumRow1, hint.width(), smallHeight);
                    // 和上个进行比较得到最长宽度
                    columMaxWidth = qMax(columMaxWidth, hint.width());
                    // 换列，x自动递增到下个坐标，列数增加，行数归零，最大列宽归零
                    x += (columMaxWidth + spacing);
                    row           = 0;
                    columMaxWidth = 0;
                    ++column;
                }
            } else {
                // 3行模式
                if (0 == row) {
                    item->rowIndex            = 0;
                    item->columnIndex         = column;
                    item->itemWillSetGeometry = QRect(x, yMediumRow0, hint.width(), smallHeight);
                    thisColumnRP0             = SARibbonPanelItem::Medium;
                    columMaxWidth             = hint.width();
                    row                       = 1;
                    // x不变
                } else if (1 == row) {
                    item->rowIndex            = 1;
                    item->columnIndex         = column;
                    item->itemWillSetGeometry = QRect(x, yMediumRow1, hint.width(), smallHeight);
                    columMaxWidth             = qMax(columMaxWidth, hint.width());
                    // 换列，x自动递增到下个坐标，列数增加，行数归零，最大列宽归零
                    x += (columMaxWidth + spacing);
                    row           = 0;
                    columMaxWidth = 0;
                    ++column;
                } else {
                    // 这种模式一般情况会发生在当前列前两行是Small，添加了一个Medium
                    // 这时需要先换列
                    // 换列，x自动递增到下个坐标，列数增加，行数归零，最大列宽归零
                    x += (columMaxWidth + spacing);
                    ++column;
                    // 换列后此时等价于0 == row
                    item->rowIndex            = 0;
                    item->columnIndex         = column;
                    item->itemWillSetGeometry = QRect(x, yMediumRow0, hint.width(), smallHeight);
                    thisColumnRP0             = SARibbonPanelItem::Medium;
                    columMaxWidth             = hint.width();
                    row                       = 1;
                }
            }
        } break;

        case SARibbonPanelItem::Small: {
            if (0 == row) {
                // 第一行
                item->rowIndex            = 0;
                item->columnIndex         = column;
                item->itemWillSetGeometry = QRect(x, ySmallRow0, hint.width(), smallHeight);
                thisColumnRP0             = SARibbonPanelItem::Small;
                columMaxWidth             = hint.width();
                // 下个row为1
                row = 1;
                // x不变
            } else if (1 == row) {
                // 第二行
                item->rowIndex            = 1;
                item->columnIndex         = column;
                item->itemWillSetGeometry = QRect(x, ySmallRow1, hint.width(), smallHeight);
                if ((3 == rowCount) && (SARibbonPanelItem::Medium == thisColumnRP0)) {
                    // 三行模式，并且第一行是Medium
                    item->itemWillSetGeometry = QRect(x, yMediumRow1, hint.width(), smallHeight);
                }
                // 和上个进行比较得到最长宽度
                columMaxWidth = qMax(columMaxWidth, hint.width());
                // 这里要看两行还是三行，确定是否要换列
                if (2 == rowCount) {
                    // 两行模式，换列
                    // 换列，x自动递增到下个坐标，列数增加，行数归零，最大列宽归零
                    x += (columMaxWidth + spacing);
                    row           = 0;
                    columMaxWidth = 0;
                    ++column;
                } else {
                    // 三行模式，继续增加行数
                    row = 2;
                    // x不变
                }
                if ((3 == rowCount) && (SARibbonPanelItem::Medium == thisColumnRP0)) {
                    // 三行模式，并且第一行是Medium，换列
                    // 换列，x自动递增到下个坐标，列数增加，行数归零，最大列宽归零
                    x += (columMaxWidth + spacing);
                    row           = 0;
                    columMaxWidth = 0;
                    ++column;
                }
            } else {
                // 第三行
                item->rowIndex            = 2;
                item->columnIndex         = column;
                item->itemWillSetGeometry = QRect(x, ySmallRow2, hint.width(), smallHeight);
                // 和上个进行比较得到最长宽度
                columMaxWidth = qMax(columMaxWidth, hint.width());
                // 换列，x自动递增到下个坐标，列数增加，行数归零，最大列宽归零
                x += (columMaxWidth + spacing);
                row           = 0;
                columMaxWidth = 0;
                ++column;
            }
        } break;

        default:
            // 不可能出现
            break;
        }
        lastGeomItem = item;
    }
    // 最后一个元素，更新列数
    //  2022-06-20 此句本来在循环里面，如果最后一个元素隐藏，会导致无法到达此判断导致异常
    if (lastGeomItem) {  // 最后一个元素，更新totalWidth
        if (lastGeomItem->columnIndex != column) {
            // 说明最后一个元素处于最后位置，触发了换列，此时真实列数需要减1，直接等于column索引
            mColumnCount = column;
            // 由于最后一个元素触发了换列，x值是新一列的位置，直接作为totalWidth要减去已经加入的spacing
            totalWidth = x - spacing + mag.right();
        } else {
            // 说明最后一个元素处于非最后位置，没有触发下一个换列，此时真实列数等于column索引+1
            mColumnCount = column + 1;
            // 由于最后一个元素未触发换列，需要计算totalWidth
            totalWidth = x + columMaxWidth + mag.right();
        }
    }

    // 在设置完所有窗口后，再设置扩展属性的窗口
    if (totalWidth < setrect.width() && (setrect.width() - totalWidth) > 10) {
        // 说明可以设置扩展属性的窗口
        recalcExpandGeomArray(setrect);
    }
    // 布局label
    const int yTitleBegin      = qMax(height - mag.bottom() - titleH, 1);
    bool isTitleWidthThanPanel = false;
    if (isEnableShowPanelTitle()) {
        mTitleLabelGeometry.setRect(mag.left(), yTitleBegin, setrect.width() - mag.left() - mag.right(), titleH);
        // 这里要确认标题宽度是否大于totalWidth，如果大于，则要把标题的宽度作为totalwidth
        QFontMetrics fm = mTitleLabel->fontMetrics();
        int textWidth   = SA::compat::horizontalAdvance(fm, panel->panelName());
        textWidth += 4;
        if (totalWidth < textWidth) {
            totalWidth            = textWidth;
            isTitleWidthThanPanel = true;  // 说明标题的长度大于按钮布局的长度
        }
    }
    // 布局optionActionButton

    if (isHaveOptionAction()) {
        QSize optBtnSize = optionActionButtonSize();
        if (isEnableShowPanelTitle()) {
            // 有标题
            mOptionActionBtnGeometry.setRect(
                mTitleLabelGeometry.right() - mTitleLabelGeometry.height(),
                mTitleLabelGeometry.y(),
                mTitleLabelGeometry.height(),
                mTitleLabelGeometry.height()
            );

            // 特殊情况，如果panel的标题长度大于totalWidth，那么说明totalWidth比较短
            // 这时候，optionActionBtn的宽度要加上到标题宽度上
            if (isTitleWidthThanPanel) {
                // 由于文字是居中对齐，因此要扩展2个按钮的宽度
                totalWidth += (2 * titleH);
            }
        } else {
            // 无标题
            mOptionActionBtnGeometry.setRect(
                setrect.right() - optBtnSize.width() - mag.right(),
                setrect.bottom() - optBtnSize.height() - mag.bottom(),
                optBtnSize.width(),
                optBtnSize.height()
            );
            totalWidth += optBtnSize.width();
        }
    }
    // 刷新sizeHint
    int heightHint  = SARibbonPanel::panelHeightHint(panel->fontMetrics(), panel->panelLayoutMode(), titleH);
    this->mSizeHint = QSize(totalWidth, heightHint);
#if SARibbonPanelLayout_DEBUG_PRINT
    qDebug() << "| |-SARibbonPanelLayout updateGeomArray(" << setrect << "),panel name = " << panel->panelName()
             << "\n| | |-size hint =" << this->m_sizeHint  //
             << "\n| | |-totalWidth=" << totalWidth        //
             << "\n| | |-last x=" << x                     //
             << "\n| | |-columMaxWidth=" << columMaxWidth  //
             << "\n| | |-spacing=" << spacing              //
             << "\n| | |-mag=" << mag                      //
             << "\n| | |-largeHeight=" << largeHeight      //
             << "\n| | |-smallHeight=" << smallHeight      //
        ;
    qDebug().noquote() << debug_print__log__;
#endif
}

/**
 * \if ENGLISH
 * @brief Recalculates the expand geometry array
 * @param setrect The rectangle to recalculate
 * \endif
 *
 * \if CHINESE
 * @brief 重新计算扩展几何数组
 * @param setrect 要重新计算的矩形
 * \endif
 */
void SARibbonPanelLayout::recalcExpandGeomArray(const QRect& setrect)
{
    // 计算能扩展的尺寸
    int expandwidth = setrect.width() - this->mSizeHint.width();

    if (expandwidth <= 1) {
        // 没有必要设置
        return;
    }
    // 列扩展信息
    struct _columnExpandInfo
    {
        int oldColumnWidth      = 0;   ///< 原来的列宽
        int columnMaximumWidth  = -1;  ///< 列的最大宽度
        int columnExpandedWidth = 0;   ///< 扩展后列的宽度
        QList< SARibbonPanelItem* > expandItems;
    };
    // 此变量用于记录可以水平扩展的列和控件，在布局结束后，如果还有空间，就把水平扩展的控件进行扩展
    QMap< int, _columnExpandInfo > columnExpandInfo;

    for (SARibbonPanelItem* item : sa_as_const(mItems)) {
        if ((!item->isEmpty()) && item->expandingDirections() & Qt::Horizontal) {
            // 只获取可见的
            QMap< int, _columnExpandInfo >::iterator i = columnExpandInfo.find(item->columnIndex);
            if (i == columnExpandInfo.end()) {
                i = columnExpandInfo.insert(item->columnIndex, _columnExpandInfo());
            }
            i.value().expandItems.append(item);
        }
    }
    if (columnExpandInfo.size() <= 0) {
        // 没有需要扩展的就退出
        return;
    }
    // 获取完可扩展的列和控件后，计算对应的列的尺寸
    // 计算能扩展的尺寸
    int oneColCanexpandWidth = expandwidth / columnExpandInfo.size();

    for (QMap< int, _columnExpandInfo >::iterator i = columnExpandInfo.begin(); i != columnExpandInfo.end();) {
        int& oldColumnWidth     = i.value().oldColumnWidth;
        int& columnMaximumWidth = i.value().columnMaximumWidth;
        this->columnWidthInfo(i.key(), oldColumnWidth, columnMaximumWidth);
        if ((oldColumnWidth <= 0) || (oldColumnWidth > columnMaximumWidth)) {
            // 如果小于0说明没有这个列，这种属于异常，删除继续
            //  oldColumnWidth > columnMaximumWidth也是异常
            i = columnExpandInfo.erase(i);
            continue;
        }
        // 开始调整
        int colwidth = oneColCanexpandWidth + oldColumnWidth;  // 先扩展了
        if (colwidth >= columnMaximumWidth) {
            // 过最大宽度要求
            i.value().columnExpandedWidth = columnMaximumWidth;
        } else {
            i.value().columnExpandedWidth = colwidth;
        }
        ++i;
    }
    // 从新调整尺寸
    // 由于会涉及其他列的变更，因此需要所有都遍历一下
    for (auto i = columnExpandInfo.begin(); i != columnExpandInfo.end(); ++i) {
        int moveXLen = i.value().columnExpandedWidth - i.value().oldColumnWidth;
        for (SARibbonPanelItem* item : sa_as_const(mItems)) {
            if (item->isEmpty() || (item->columnIndex < i.key())) {
                // 之前的列不用管
                continue;
            }
            if (item->columnIndex == i.key()) {
                // 此列的扩展
                if (i.value().expandItems.contains(item)) {
                    // 此列需要扩展的item才扩展尺寸
                    item->itemWillSetGeometry.setWidth(i.value().columnExpandedWidth);
                } else {
                    // 此列不扩展的模块保持原来的尺寸
                    continue;
                }
            } else {
                // 后面的移动
                item->itemWillSetGeometry.moveLeft(item->itemWillSetGeometry.x() + moveXLen);
            }
        }
    }
#if SARibbonPanelLayout_DEBUG_PRINT
    qDebug() << "| |-SARibbonPanelLayout recalcExpandGeomArray(" << setrect
             << ") panelName=" << ribbonPanel()->panelName()  //
             << ",expandwidth=" << expandwidth                //
        ;
#endif
}

/**
 * \if ENGLISH
 * @brief Calculates the width and maximum width of the window based on the column index
 * @param colindex Column index
 * @param width Returns -1 if the column does not exist
 * @param maximum Returns -1 if the column does not exist
 * \endif
 *
 * \if CHINESE
 * @brief 根据列数，计算窗口的宽度，以及最大宽度
 * @param colindex 列索引
 * @param width 如果传入没有这个列，返回-1
 * @param maximum 如果传入没有这个列，返回-1
 * \endif
 */
void SARibbonPanelLayout::columnWidthInfo(int colindex, int& width, int& maximum) const
{
    width   = -1;
    maximum = -1;
    for (SARibbonPanelItem* item : mItems) {
        if (!item->isEmpty() && (item->columnIndex == colindex)) {
            width   = qMax(width, item->itemWillSetGeometry.width());
            maximum = qMax(maximum, item->widget()->maximumWidth());
        }
    }
}

/**
 * \if ENGLISH
 * @brief Gets the panel's title label
 * @return A pointer to the SARibbonPanelLabel
 * \endif
 *
 * \if CHINESE
 * @brief 获取面板的标题标签
 * @return 指向SARibbonPanelLabel的指针
 * \endif
 */
SARibbonPanelLabel* SARibbonPanelLayout::panelTitleLabel() const
{
    return mTitleLabel;
}

/**
 * \if ENGLISH
 * @brief Sets the default icon size for tool buttons
 * @param smallSize Small icon size
 * @param largeSize Large icon size
 * @sa toolButtonIconSize
 * \endif
 *
 * \if CHINESE
 * @brief 设置工具按钮的默认图标尺寸
 * @param smallSize 小图标尺寸
 * @param largeSize 大图标尺寸
 * @sa toolButtonIconSize
 * \endif
 */
void SARibbonPanelLayout::setToolButtonIconSize(const QSize& smallSize, const QSize& largeSize)
{
    mSmallToolButtonIconSize = smallSize;
    mLargeToolButtonIconSize = largeSize;
}

/**
 * \if ENGLISH
 * @brief Gets the default icon size for tool buttons
 * @return The current icon size
 * @sa setToolButtonIconSize
 * \endif
 *
 * \if CHINESE
 * @brief 获取工具按钮的默认图标尺寸
 * @return 当前的图标尺寸
 * @sa setToolButtonIconSize
 * \endif
 */
QPair< QSize, QSize > SARibbonPanelLayout::toolButtonIconSize() const
{
    return qMakePair(mSmallToolButtonIconSize, mLargeToolButtonIconSize);
}

/**
 * \if ENGLISH
 * @brief Sets the large button icon size
 * @param largeSize Large icon size
 * \endif
 *
 * \if CHINESE
 * @brief 设置大按钮图标尺寸
 * @param largeSize 大图标尺寸
 * \endif
 */
void SARibbonPanelLayout::setLargeIconSize(const QSize& largeSize)
{
    mLargeToolButtonIconSize = largeSize;
}

/**
 * \if ENGLISH
 * @brief Gets the large button icon size
 * @return Large icon size
 * \endif
 *
 * \if CHINESE
 * @brief 大按钮图标尺寸
 * @return 大图标尺寸
 * \endif
 */
QSize SARibbonPanelLayout::largeIconSize() const
{
    return mLargeToolButtonIconSize;
}

/**
 * \if ENGLISH
 * @brief Sets the small button icon size
 * @param largeSize Small icon size
 * \endif
 *
 * \if CHINESE
 * @brief 设置小按钮图标尺寸
 * @param largeSize 小图标尺寸
 * \endif
 */
void SARibbonPanelLayout::setSmallIconSize(const QSize& largeSize)
{
    mSmallToolButtonIconSize = largeSize;
}

/**
 * \if ENGLISH
 * @brief Gets the small button icon size
 * @return Small icon size
 * \endif
 *
 * \if CHINESE
 * @brief 小按钮图标尺寸
 * @return 小图标尺寸
 * \endif
 */
QSize SARibbonPanelLayout::smallIconSize() const
{
    return mSmallToolButtonIconSize;
}

/**
 * \if ENGLISH
 * @brief Gets the size of the option action button
 * @return The button's size
 * \endif
 *
 * \if CHINESE
 * @brief 获取选项action按钮的尺寸
 * @return 按钮的尺寸
 * \endif
 */
QSize SARibbonPanelLayout::optionActionButtonSize() const
{
    return (isEnableShowPanelTitle() ? QSize(12, 12) : QSize(mTitleHeight, mTitleHeight));
}

/**
 * \if ENGLISH
 * @brief Sets the panel's title label
 * @param newTitleLabel The new title label widget
 * \endif
 *
 * \if CHINESE
 * @brief 设置面板的标题标签
 * @param newTitleLabel 新的标题标签窗口部件
 * \endif
 */
void SARibbonPanelLayout::setPanelTitleLabel(SARibbonPanelLabel* newTitleLabel)
{
    mTitleLabel = newTitleLabel;
    // 确保m_optionActionBtn在label之上
    if (mOptionActionBtn) {
        if (mTitleLabel) {
            mTitleLabel->stackUnder(mOptionActionBtn);
        }
    }
}

/**
 * \if ENGLISH
 * @brief Checks if word wrap is enabled
 * @return true if word wrap is enabled, false otherwise
 * \endif
 *
 * \if CHINESE
 * @brief 是否允许文字换行
 * @return 如果允许文字换行则返回true，否则返回false
 * \endif
 */
bool SARibbonPanelLayout::isEnableWordWrap() const
{
    return mEnableWordWrap;
}

/**
 * \if ENGLISH
 * @brief Sets whether word wrap is enabled
 * @param on If true, word wrap is enabled
 * \endif
 *
 * \if CHINESE
 * @brief 设置文字允许换行
 * @param on 如果为true，则允许文字换行
 * \endif
 */
void SARibbonPanelLayout::setEnableWordWrap(bool on)
{
    mEnableWordWrap = on;
    // 遍历所有SARibbonToolButton
    for (SARibbonPanelItem* item : sa_as_const(mItems)) {
        if (!item) {
            continue;
        }
        if (SARibbonToolButton* toolbtn = qobject_cast< SARibbonToolButton* >(item->widget())) {
            toolbtn->setEnableWordWrap(on);
        }
    }
}

/**
 * \if ENGLISH
 * @brief Sets the maximum button aspect ratio, which determines the maximum width of the button
 *
 * The maximum width of the button is button height * this coefficient. For example, if the button height is h, then the maximum button width is maxw = h * buttonMaximumAspectRatio
 * If the text cannot be fully displayed at this width, the button will not continue to expand horizontally, and ... will be used to replace the text that is not fully displayed
 *
 * @param fac Aspect ratio factor
 * @see buttonMaximumAspectRatio
 *
 * @note Users should not call @ref SARibbonPanelLayout::setButtonMaximumAspectRatio to set it,
 * but instead call @ref SARibbonBar::setButtonMaximumAspectRatio to set the aspect ratio
 * \endif
 *
 * \if CHINESE
 * @brief 设置按钮最大宽高比，这个系数决定按钮的最大宽度
 *
 * 按钮的最大宽度为按钮高度*此系数，例如按钮高度为h，那么按钮最大宽度maxw=h*buttonMaximumAspectRatio
 * 如果在此宽度下文字还无法完全显示，那么按钮将不会继续横向扩展，将使用...替代未完全显示的文字
 *
 * @param fac 宽高比系数
 * @see buttonMaximumAspectRatio
 *
 * @note 用户不应该调用@ref SARibbonPanelLayout::setButtonMaximumAspectRatio 来设置，
 * 而是调用@ref SARibbonBar::setButtonMaximumAspectRatio 设置宽高比
 * \endif
 */
void SARibbonPanelLayout::setButtonMaximumAspectRatio(qreal fac)
{
    mButtonMaximumAspectRatio = fac;
    // 遍历所有SARibbonToolButton
    for (SARibbonPanelItem* item : sa_as_const(mItems)) {
        if (!item) {
            continue;
        }
        if (SARibbonToolButton* toolbtn = qobject_cast< SARibbonToolButton* >(item->widget())) {
            toolbtn->setButtonMaximumAspectRatio(fac);
        }
    }
}

/**
 * \if ENGLISH
 * @brief Gets the maximum button aspect ratio, which determines the maximum width of the button
 * @return The maximum button aspect ratio
 * @see setButtonMaximumAspectRatio
 * \endif
 *
 * \if CHINESE
 * @brief 按钮最大宽高比，这个系数决定按钮的最大宽度
 * @return 按钮最大宽高比
 * @see setButtonMaximumAspectRatio
 * \endif
 */
qreal SARibbonPanelLayout::buttonMaximumAspectRatio() const
{
    return mButtonMaximumAspectRatio;
}

/**
 * \if ENGLISH
 * @brief Gets the spacing between the title and the buttons
 * @return The current spacing value
 * @sa setPanelTitleSpace
 * \endif
 *
 * \if CHINESE
 * @brief 获取标题与按钮之间的间距
 * @return 当前的间距值
 * @sa setPanelTitleSpace
 * \endif
 */
int SARibbonPanelLayout::panelTitleSpace() const
{
    return mTitleSpace;
}

/**
 * \if ENGLISH
 * @brief Sets the spacing between the title and the buttons
 * @param newTitleSpace The new spacing value
 * @sa panelTitleSpace
 * \endif
 *
 * \if CHINESE
 * @brief 设置标题与按钮之间的间距
 * @param newTitleSpace 新的间距值
 * @sa panelTitleSpace
 * \endif
 */
void SARibbonPanelLayout::setPanelTitleSpace(int newTitleSpace)
{
    if (mTitleSpace == newTitleSpace) {
        return;
    }
    mTitleSpace = newTitleSpace;
    invalidate();
}

/**
 * \if ENGLISH
 * @brief Gets the height of the panel's title
 * @return The current title height
 * @sa setPanelTitleHeight
 * \endif
 *
 * \if CHINESE
 * @brief 获取面板标题的高度
 * @return 当前的标题高度
 * @sa setPanelTitleHeight
 * \endif
 */
int SARibbonPanelLayout::panelTitleHeight() const
{
    return mTitleHeight;
}

/**
 * \if ENGLISH
 * @brief Sets the height of the panel's title
 * @param newTitleHeight The new title height
 * @sa panelTitleHeight
 * \endif
 *
 * \if CHINESE
 * @brief 设置面板标题的高度
 * @param newTitleHeight 新的标题高度
 * @sa panelTitleHeight
 * \endif
 */
void SARibbonPanelLayout::setPanelTitleHeight(int newTitleHeight)
{
    if (mTitleHeight == newTitleHeight) {
        return;
    }
    mTitleHeight = newTitleHeight;
    invalidate();
}

/**
 * @brief Checks if the panel's title is enabled for display / 检查面板标题是否启用显示
 * @return true if the title is enabled; otherwise false / 如果标题启用则返回true；否则返回false
 * @sa setEnableShowPanelTitle
 */
bool SARibbonPanelLayout::isEnableShowPanelTitle() const
{
    return mEnableShowTitle;
}

/**
 * @brief Sets whether the panel's title is enabled for display / 设置面板标题是否启用显示
 * @param on If true, the title is enabled / 如果为true，则启用标题
 * @sa isEnableShowPanelTitle
 */
void SARibbonPanelLayout::setEnableShowPanelTitle(bool on)
{
    if (mEnableShowTitle == on) {
        return;
    }
    mEnableShowTitle = on;
    invalidate();
}

/**
 * @brief Gets the height of large buttons / 获取大按钮的高度
 * @return The large button height / 大按钮高度
 */
int SARibbonPanelLayout::largeButtonHeight() const
{
    return mLargeHeight;
}

void SARibbonPanelLayout::setGeometry(const QRect& rect)
{
    QRect old = geometry();
    if (old == rect) {
        return;
    }
    if (rect.width() <= 0 || rect.height() <= 0) {
        return;
    }
#if SARibbonPanelLayout_DEBUG_PRINT
    qDebug() << "| |----->SARibbonPanelLayout.setGeometry(" << rect << "(" << ribbonPanel()->panelName() << ")=======";
#endif
    QLayout::setGeometry(rect);
    mDirty = false;
    updateGeomArray(rect);
    doLayout();
}
