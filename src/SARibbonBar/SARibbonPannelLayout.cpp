#include "SARibbonPannelLayout.h"
#include "SARibbonPannelOptionButton.h"
#include "SARibbonSeparatorWidget.h"
#include "SARibbonElementManager.h"
#include <QWidgetAction>
#include <QQueue>
#include "SARibbonPannel.h"
#include "SARibbonPannelItem.h"
#define SARibbonPannelLayout_DEBUG_PRINT 1
#define HELP_DRAW_RECT(p, rect)                                                                                        \
    do {                                                                                                               \
        p.save();                                                                                                      \
        QPen _pen(Qt::DashLine);                                                                                       \
        _pen.setColor(Qt::blue);                                                                                       \
        p.setPen(_pen);                                                                                                \
        p.setBrush(QBrush());                                                                                          \
        p.drawRect(rect);                                                                                              \
        p.restore();                                                                                                   \
    } while (0)

SARibbonPannelLayout::SARibbonPannelLayout(QWidget* p)
    : QLayout(p), m_columnCount(0), m_expandFlag(false), m_dirty(true)
{
    setSpacing(1);
    SARibbonPannel* tb = qobject_cast< SARibbonPannel* >(p);

    if (!tb) {
        return;
    }
}

SARibbonPannelLayout::~SARibbonPannelLayout()
{
    // 参考QToolBarLayout
    while (!m_items.isEmpty()) {
        SARibbonPannelItem* item = m_items.takeFirst();
        if (QWidgetAction* widgetAction = qobject_cast< QWidgetAction* >(item->action)) {
            if (item->customWidget) {
                widgetAction->releaseWidget(item->widget());
            }
        }
        delete item;
    }
}

/**
 * @brief 通过action查找索引，用于actionEvent添加action用
 * @param action
 * @return 没有查到返回-1
 */
int SARibbonPannelLayout::indexByAction(QAction* action) const
{
    for (int i = 0; i < m_items.count(); ++i) {
        if (m_items.at(i)->action == action) {
            return (i);
        }
    }
    return (-1);
}

/**
 * @brief 获取ribbonpannel
 * @return
 */
SARibbonPannel* SARibbonPannelLayout::ribbonPannel() const
{
    return qobject_cast< SARibbonPannel* >(parentWidget());
}

void SARibbonPannelLayout::addItem(QLayoutItem* item)
{
    Q_UNUSED(item);
    qWarning("SARibbonPannelLayout::addItem(): please use addAction() instead");
    return;
}

/**
 * @brief SARibbonPannel主要通过此函数来添加action
 * @param act
 * @param rp 布局策略
 */
void SARibbonPannelLayout::insertAction(int index, QAction* act, SARibbonPannelItem::RowProportion rp)
{
    index                    = qMax(0, index);
    index                    = qMin(m_items.count(), index);
    SARibbonPannelItem* item = createItem(act, rp);

    if (item) {
        m_items.insert(index, item);
        // 标记需要重新计算尺寸
        invalidate();
    }
}

/**
 * @brief 添加操作action，如果要去除，传入nullptr指针即可，SARibbonPannel不会对QAction的所有权进行管理
 * @param action
 * @note 要去除OptionAction直接传入nullptr即可
 * @note SARibbonPannel不对QAction的destroy进行关联，如果外部对action进行delete，需要先传入nullptr给addOptionAction
 */
void SARibbonPannelLayout::setOptionAction(QAction* action)
{
    SARibbonPannel* p = ribbonPannel();
    if (!p) {
        return;
    }
    if (action) {
        // 创建option action
        if (nullptr == m_optionActionBtn) {
            m_optionActionBtn = RibbonSubElementFactory->createRibbonPannelOptionButton(p);
            QObject::connect(m_optionActionBtn, &SARibbonToolButton::triggered, p, &SARibbonPannel::actionTriggered);
            // 确保m_optionActionBtn在label之上
            if (m_titleLabel) {
                m_titleLabel->stackUnder(m_optionActionBtn);
            }
        }
        m_optionActionBtn->setDefaultAction(action);
        if (action->icon().isNull()) {
            m_optionActionBtn->setIcon(QIcon(":/image/resource/ribbonPannelOptionButton.png"));
        }
        // 标记需要重新计算尺寸
        invalidate();
    } else {
        // 取消option action
        if (m_optionActionBtn) {
            m_optionActionBtn->hide();
            m_optionActionBtn->deleteLater();
            m_optionActionBtn = nullptr;
            // 标记需要重新计算尺寸
            invalidate();
        }
    }
}

/**
 * @brief 判断是否存在OptionAction
 * @return 存在返回true
 */
bool SARibbonPannelLayout::isHaveOptionAction() const
{
    return (m_optionActionBtn != nullptr);
}

QLayoutItem* SARibbonPannelLayout::itemAt(int index) const
{
    if ((index < 0) || (index >= m_items.count())) {
        return (nullptr);
    }
    return (m_items.at(index));
}

QLayoutItem* SARibbonPannelLayout::takeAt(int index)
{
    if ((index < 0) || (index >= m_items.count())) {
        return (nullptr);
    }
    SARibbonPannelItem* item = m_items.takeAt(index);

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

int SARibbonPannelLayout::count() const
{
    return (m_items.count());
}

bool SARibbonPannelLayout::isEmpty() const
{

    return (m_items.isEmpty());
}

void SARibbonPannelLayout::invalidate()
{
    m_dirty = true;
    QLayout::invalidate();
}

Qt::Orientations SARibbonPannelLayout::expandingDirections() const
{
    return (Qt::Horizontal);
}

QSize SARibbonPannelLayout::minimumSize() const
{
    return (m_sizeHint);
}

QSize SARibbonPannelLayout::sizeHint() const
{
#if SARibbonPannelLayout_DEBUG_PRINT && SA_DEBUG_PRINT_SIZE_HINT
    if (SARibbonPannel* pannel = ribbonPannel()) {
        qDebug() << "| |-SARibbonPannelLayout sizeHint,sizeHint = " << m_sizeHint;
    }
#endif
    return (m_sizeHint);
}

/**
 * @brief 通过action获取SARibbonPannelItem
 * @param action
 * @return 如果没有返回nullptr
 */
SARibbonPannelItem* SARibbonPannelLayout::pannelItem(QAction* action) const
{
    int index = indexByAction(action);

    if (index >= 0) {
        return (m_items[ index ]);
    }
    return (nullptr);
}

/**
 * @brief 获取最后一个添加的item
 * @return 如果没有返回nullptr
 */
SARibbonPannelItem* SARibbonPannelLayout::lastItem() const
{
    if (m_items.isEmpty()) {
        return (nullptr);
    }
    return (m_items.last());
}

/**
 * @brief 获取最后生成的窗口
 * @return 如果无窗口或者item为空，返回nullptr
 */
QWidget* SARibbonPannelLayout::lastWidget() const
{
    SARibbonPannelItem* item = lastItem();

    if (item) {
        return (item->widget());
    }
    return (nullptr);
}

/**
 * @brief 移动两个item
 * @param from
 * @param to
 * @note 移动完后所有都失效，需要重新布局
 */
void SARibbonPannelLayout::move(int from, int to)
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
    m_items.move(from, to);
    invalidate();
}

/**
 * @brief 判断是否需要重新布局
 * @return
 */
bool SARibbonPannelLayout::isDirty() const
{
    return (m_dirty);
}

void SARibbonPannelLayout::updateGeomArray()
{
    updateGeomArray(geometry());
}

/**
 * @brief 布局所有action
 */
void SARibbonPannelLayout::doLayout()
{
#if SARibbonPannelLayout_DEBUG_PRINT && SA_DEBUG_PRINT_SIZE_HINT
    if (SARibbonPannel* pannel = ribbonPannel()) {
        qDebug() << "| |-SARibbonPannelLayout layoutActions,pannel name = " << pannel->pannelName();
    }
#endif
    if (isDirty()) {
        updateGeomArray();
    }
    QList< QWidget* > showWidgets, hideWidgets;
    SARibbonPannel* pannel = ribbonPannel();
    for (SARibbonPannelItem* item : qAsConst(m_items)) {
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

    // 不在上面那里进行show和hide因为这会触发SARibbonPannelLayout的重绘，导致循环绘制，非常影响效率
    for (QWidget* w : qAsConst(showWidgets)) {
        if (!w->isVisible())
            w->show();
    }
    for (QWidget* w : qAsConst(hideWidgets)) {
        if (w->isVisible())
            w->hide();
    }
    // 布局label
    if (m_titleLabel) {
        if (isEnableShowPannelTitle()) {
            m_titleLabel->setGeometry(m_titleLabelGeometry);
            if (!m_titleLabel->isVisibleTo(pannel)) {
                m_titleLabel->show();
            }
        } else {
            if (m_titleLabel->isVisibleTo(pannel)) {
                m_titleLabel->hide();
            }
        }
    }
    // 布局m_optionActionBtn
    if (m_optionActionBtn) {
        m_optionActionBtn->setGeometry(m_optionActionBtnGeometry);
        m_optionActionBtn->setIconSize(QSize(m_optionActionBtnGeometry.width(), m_optionActionBtnGeometry.height()));
    }
}

/**
 * @brief 把action转换为item
 *
 * 此函数参考QToolBarItem *QToolBarLayout::createItem(QAction *action)
 *
 * 对于普通QAction，此函数会创建SARibbonToolButton，SARibbonToolButton的类型参考SARibbonPannelItem::RowProportion，
 * @param action
 * @param rp 行高占比情况
 * @return 转换的SARibbonPannelItem
 * @note 每个SARibbonPannelItem最终都会携带一个widget，传入的是QWidgetAction的话，会直接使用QWidgetAction带的widget，
 * 否则会内部生成一个SARibbonToolButton
 *
 */
SARibbonPannelItem* SARibbonPannelLayout::createItem(QAction* action, SARibbonPannelItem::RowProportion rp)
{
    bool customWidget      = false;
    QWidget* widget        = nullptr;
    SARibbonPannel* pannel = qobject_cast< SARibbonPannel* >(parentWidget());

    if (!pannel) {
        // 在没有pannel这个函数会返回nullptr
        return (nullptr);
    }
    if (QWidgetAction* widgetAction = qobject_cast< QWidgetAction* >(action)) {
        widget = widgetAction->requestWidget(pannel);
        if (widget != nullptr) {
            widget->setAttribute(Qt::WA_LayoutUsesWidgetRect);
            customWidget = true;  // 标记为true，在移除的时候是不会对这个窗口进行删除，false默认会进行删除如SARibbonSeparatorWidget和SARibbonToolButton
        }
    } else if (action->isSeparator()) {
        SARibbonSeparatorWidget* sep = RibbonSubElementFactory->createRibbonSeparatorWidget(pannel);
        widget                       = sep;
    }
    // 不是widget，自动生成SARibbonToolbutton
    if (!widget) {
        SARibbonToolButton::RibbonButtonType buttonType = ((rp == SARibbonPannelItem::Large)
                                                               ? SARibbonToolButton::LargeButton
                                                               : SARibbonToolButton::SmallButton);

        SARibbonToolButton* button = RibbonSubElementFactory->createRibbonToolButton(pannel);
        button->setFocusPolicy(Qt::NoFocus);
        button->setButtonType(buttonType);
        button->setDefaultAction(action);
        // 属性设置
        QToolButton::ToolButtonPopupMode popMode = SARibbonPannel::getActionToolButtonPopupModeProperty(action);
        button->setPopupMode(popMode);
        // 根据QAction的属性设置按钮的大小

        QObject::connect(button, &SARibbonToolButton::triggered, pannel, &SARibbonPannel::actionTriggered);
        widget = button;
    }
    // 这时总会有widget
    widget->hide();
    SARibbonPannelItem* result = new SARibbonPannelItem(widget);

    result->rowProportion = rp;
    result->customWidget  = customWidget;
    result->action        = action;
    return (result);
}

/**
 * @brief 更新尺寸
 */
void SARibbonPannelLayout::updateGeomArray(const QRect& setrect)
{
    SARibbonPannel* pannel = qobject_cast< SARibbonPannel* >(parentWidget());

    if (!pannel) {
        return;
    }

    const int height    = setrect.height();
    const QMargins& mag = contentsMargins();
    const int spacing   = this->spacing();
    int x               = mag.left();
    const int yBegin    = mag.top();
    int titleH          = (m_titleHeight >= 0) ? m_titleHeight : 0;  // 防止负数影响
    int titleSpace      = (m_titleHeight >= 0) ? m_titleSpace : 0;  // 对于没有标题的情况，spacing就不生效
    if (!isEnableShowPannelTitle()) {
        titleH     = 0;
        titleSpace = 0;
    }
    // 获取pannel的布局模式 3行或者2行
    //  rowcount 是ribbon的行，有2行和3行两种
    const short rowCount = (pannel->pannelLayoutMode() == SARibbonPannel::ThreeRowMode) ? 3 : 2;
    // largeHeight是对应large占比的高度
    const int largeHeight = height - mag.bottom() - mag.top() - titleH - titleSpace;
    const int yTitleBegin = height - mag.bottom() - titleH;
    m_largeHeight         = largeHeight;
    // 计算smallHeight的高度
    const int smallHeight = (largeHeight - (rowCount - 1) * spacing) / rowCount;
    // Medium行的y位置
    const int yMediumRow0 = (2 == rowCount) ? yBegin : (yBegin + ((largeHeight - 2 * smallHeight) / 3));
    const int yMediumRow1 = (2 == rowCount) ? (yBegin + smallHeight + spacing)
                                            : (yBegin + ((largeHeight - 2 * smallHeight) / 3) * 2 + smallHeight);
    // Small行的y位置
    const int ySmallRow0 = yBegin;
    const int ySmallRow1 = yBegin + smallHeight + spacing;
    const int ySmallRow2 = yBegin + 2 * (smallHeight + spacing);
    // row用于记录下个item应该属于第几行，item->rowIndex用于记录当前处于第几行，
    // item->rowIndex主要用于SARibbonPannelItem::Medium
    short row  = 0;
    int column = 0;
    // 记录每列最大的宽度
    int columMaxWidth = 0;
    // 记录总宽度
    int totalWidth = 0;

    int itemCount = m_items.count();

#if SARibbonPannelLayout_DEBUG_PRINT && SA_DEBUG_PRINT_SIZE_HINT
    QString debug_print__log__;
#endif
    // 本列第一、二行占比
    SARibbonPannelItem::RowProportion thisColumnRP0 = SARibbonPannelItem::None;
    SARibbonPannelItem* lastGeomItem                = nullptr;  // 记录最后一个设置位置的item
    for (int i = 0; i < itemCount; ++i) {
        SARibbonPannelItem* item = m_items.at(i);
        if (item->isEmpty()) {
            // 如果是hide就直接跳过
            item->rowIndex    = -1;
            item->columnIndex = -1;
            continue;
        }

        QSize hint = item->sizeHint();
#if SARibbonPannelLayout_DEBUG_PRINT && SA_DEBUG_PRINT_SIZE_HINT
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
                m_expandFlag = true;
            }
        }
        SARibbonPannelItem::RowProportion rp = item->rowProportion;
        if (SARibbonPannelItem::None == rp) {
            // 为定义行占比但是垂直扩展，就定义为Large占比，否则就是small占比
            if (exp & Qt::Vertical) {
                rp = SARibbonPannelItem::Large;
            } else {
                rp = SARibbonPannelItem::Small;
            }
        }
        // 开始根据占比和layoutmode来布局
        switch (rp) {
        case SARibbonPannelItem::Large: {
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

        case SARibbonPannelItem::Medium: {
            // 2行模式下Medium和small等价
            if (2 == rowCount) {
                if (0 == row) {
                    item->rowIndex            = 0;
                    item->columnIndex         = column;
                    item->itemWillSetGeometry = QRect(x, yMediumRow0, hint.width(), smallHeight);
                    thisColumnRP0             = SARibbonPannelItem::Medium;
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
                    thisColumnRP0             = SARibbonPannelItem::Medium;
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
                    thisColumnRP0             = SARibbonPannelItem::Medium;
                    columMaxWidth             = hint.width();
                    row                       = 1;
                }
            }
        } break;

        case SARibbonPannelItem::Small: {
            if (0 == row) {
                // 第一行
                item->rowIndex            = 0;
                item->columnIndex         = column;
                item->itemWillSetGeometry = QRect(x, ySmallRow0, hint.width(), smallHeight);
                thisColumnRP0             = SARibbonPannelItem::Small;
                columMaxWidth             = hint.width();
                // 下个row为1
                row = 1;
                // x不变
            } else if (1 == row) {
                // 第二行
                item->rowIndex            = 1;
                item->columnIndex         = column;
                item->itemWillSetGeometry = QRect(x, ySmallRow1, hint.width(), smallHeight);
                if ((3 == rowCount) && (SARibbonPannelItem::Medium == thisColumnRP0)) {
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
                if ((3 == rowCount) && (SARibbonPannelItem::Medium == thisColumnRP0)) {
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
            m_columnCount = column;
            // 由于最后一个元素触发了换列，x值是新一列的位置，直接作为totalWidth要减去已经加入的spacing
            totalWidth = x - spacing + mag.right();
        } else {
            // 说明最后一个元素处于非最后位置，没有触发下一个换列，此时真实列数等于column索引+1
            m_columnCount = column + 1;
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
    bool isTitleWidthThanPannel = false;
    if (isEnableShowPannelTitle()) {
        m_titleLabelGeometry.setRect(mag.left(), yTitleBegin, setrect.width() - mag.left() - mag.right(), titleH);
        // 这里要确认标题宽度是否大于totalWidth，如果大于，则要把标题的宽度作为totalwidth
        QFontMetrics fm = m_titleLabel->fontMetrics();
        int textWidth   = SA_FONTMETRICS_WIDTH(fm, pannel->pannelName());
        textWidth += 4;
        if (totalWidth < textWidth) {
            totalWidth             = textWidth;
            isTitleWidthThanPannel = true;  // 说明标题的长度大于按钮布局的长度
        }
    }
    // 布局optionActionButton

    if (isHaveOptionAction()) {
        QSize optBtnSize = optionActionButtonSize();
        if (isEnableShowPannelTitle()) {
            // 有标题
            m_optionActionBtnGeometry.setRect(m_titleLabelGeometry.right() - m_titleLabelGeometry.height(),
                                              m_titleLabelGeometry.y(),
                                              m_titleLabelGeometry.height(),
                                              m_titleLabelGeometry.height());

            // 特殊情况，如果pannel的标题长度大于totalWidth，那么说明totalWidth比较短
            // 这时候，optionActionBtn的宽度要加上到标题宽度上
            if (isTitleWidthThanPannel) {
                // 由于文字是居中对齐，因此要扩展2个按钮的宽度
                totalWidth += (2 * titleH);
            }
        } else {
            // 无标题
            m_optionActionBtnGeometry.setRect(setrect.right() - optBtnSize.width() - mag.right(),
                                              setrect.bottom() - optBtnSize.height() - mag.bottom(),
                                              optBtnSize.width(),
                                              optBtnSize.height());
            totalWidth += optBtnSize.width();
        }
    }
    // 刷新sizeHint
    int heightHint   = SARibbonPannel::pannelHeightHint(pannel->fontMetrics(), pannel->pannelLayoutMode(), titleH);
    this->m_sizeHint = QSize(totalWidth, heightHint);
#if SARibbonPannelLayout_DEBUG_PRINT && SA_DEBUG_PRINT_SIZE_HINT
    qDebug() << "| |-SARibbonPannelLayout updateGeomArray(" << setrect << "),pannel name = " << pannel->pannelName()
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

void SARibbonPannelLayout::recalcExpandGeomArray(const QRect& setrect)
{
    // 计算能扩展的尺寸
    int expandwidth = setrect.width() - this->m_sizeHint.width();

    if (expandwidth <= 0) {
        // 没有必要设置
        return;
    }
    // 列扩展信息
    struct _columnExpandInfo
    {
        int oldColumnWidth      = 0;   ///< 原来的列宽
        int columnMaximumWidth  = -1;  ///< 列的最大宽度
        int columnExpandedWidth = 0;   ///< 扩展后列的宽度
        QList< SARibbonPannelItem* > expandItems;
    };
    // 此变量用于记录可以水平扩展的列和控件，在布局结束后，如果还有空间，就把水平扩展的控件进行扩展
    QMap< int, _columnExpandInfo > columnExpandInfo;

    for (SARibbonPannelItem* item : qAsConst(m_items)) {
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
        for (SARibbonPannelItem* item : qAsConst(m_items)) {
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
#if SARibbonPannelLayout_DEBUG_PRINT && SA_DEBUG_PRINT_SIZE_HINT
    qDebug() << "| |-SARibbonPannelLayout recalcExpandGeomArray(" << setrect
             << ") pannelName=" << ribbonPannel()->pannelName()  //
             << ",expandwidth=" << expandwidth                   //
        ;
#endif
}

/**
 * @brief 根据列数，计算窗口的宽度，以及最大宽度
 * @param colindex
 * @param width 如果传入没有这个列，返回-1
 * @param maximum 如果传入没有这个列，返回-1
 */
void SARibbonPannelLayout::columnWidthInfo(int colindex, int& width, int& maximum) const
{
    width   = -1;
    maximum = -1;
    for (SARibbonPannelItem* item : m_items) {
        if (!item->isEmpty() && (item->columnIndex == colindex)) {
            width   = qMax(width, item->itemWillSetGeometry.width());
            maximum = qMax(maximum, item->widget()->maximumWidth());
        }
    }
}

SARibbonPannelLabel* SARibbonPannelLayout::pannelTitleLabel() const
{
    return m_titleLabel;
}

/**
 * @brief 获取optionAction 按钮尺寸
 * @return
 */
QSize SARibbonPannelLayout::optionActionButtonSize() const
{
    return (isEnableShowPannelTitle() ? QSize(12, 12) : QSize(m_titleHeight, m_titleHeight));
}

void SARibbonPannelLayout::setPannelTitleLabel(SARibbonPannelLabel* newTitleLabel)
{
    m_titleLabel = newTitleLabel;
    // 确保m_optionActionBtn在label之上
    if (m_optionActionBtn) {
        if (m_titleLabel) {
            m_titleLabel->stackUnder(m_optionActionBtn);
        }
    }
}

/**
 * @brief 标题区域和按钮的间隔
 * @return
 */
int SARibbonPannelLayout::pannelTitleSpace() const
{
    return m_titleSpace;
}

/**
 * @brief 设置标题区域和按钮的间隔
 * @param newTitleSpace
 */
void SARibbonPannelLayout::setPannelTitleSpace(int newTitleSpace)
{
    if (m_titleSpace == newTitleSpace) {
        return;
    }
    m_titleSpace = newTitleSpace;
    invalidate();
}

/**
 * @brief 标题高度
 * @return
 */
int SARibbonPannelLayout::pannelTitleHeight() const
{
    return m_titleHeight;
}

/**
 * @brief 设置标题高度
 * @param newTitleHeight
 */
void SARibbonPannelLayout::setPannelTitleHeight(int newTitleHeight)
{
    if (m_titleHeight == newTitleHeight) {
        return;
    }
    m_titleHeight = newTitleHeight;
    invalidate();
}

/**
 * @brief 判断是否存在标题
 * @return
 */
bool SARibbonPannelLayout::isEnableShowPannelTitle() const
{
    return m_enableShowTitle;
}

/**
 * @brief 设置显示标题
 * @param on
 */
void SARibbonPannelLayout::setEnableShowPannelTitle(bool on)
{
    if (m_enableShowTitle == on) {
        return;
    }
    m_enableShowTitle = on;
    invalidate();
}

/**
 * @brief 大按钮的高度
 * @return
 */
int SARibbonPannelLayout::largeButtonHeight() const
{
    return m_largeHeight;
}

void SARibbonPannelLayout::setGeometry(const QRect& rect)
{
    QRect old = geometry();
    if (old == rect) {
        return;
    }
#if SARibbonPannelLayout_DEBUG_PRINT && SA_DEBUG_PRINT_SIZE_HINT
    qDebug() << "| |----->SARibbonPannelLayout.setGeometry(" << rect << "(" << ribbonPannel()->pannelName() << ")=======";
#endif
    QLayout::setGeometry(rect);
    m_dirty = false;
    updateGeomArray(rect);
    doLayout();
}
