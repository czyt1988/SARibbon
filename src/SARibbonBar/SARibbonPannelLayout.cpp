#include "SARibbonPannelLayout.h"
#include "SARibbonCategory.h"
#include "SARibbonPannelOptionButton.h"
#include "SARibbonSeparatorWidget.h"
#include "SARibbonGallery.h"
#include "SARibbonElementManager.h"
#include "SARibbonMenu.h"
#include <QWidgetAction>
#include <QQueue>
#include <set>
#include "SARibbonPannel.h"
#include "SARibbonPannelItem.h"

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

QMargins SARibbonPannelLayout::s_contentsMargins = QMargins(1, 1, 1, 1);

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
int SARibbonPannelLayout::indexOf(QAction* action) const
{
    for (int i = 0; i < m_items.count(); ++i) {
        if (m_items.at(i)->action == action) {
            return (i);
        }
    }
    return (-1);
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
        //标记需要重新计算尺寸
        invalidate();
    }
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
    return (m_sizeHint);
}

/**
 * @brief 通过action获取SARibbonPannelItem
 * @param action
 * @return 如果没有返回nullptr
 */
SARibbonPannelItem* SARibbonPannelLayout::pannelItem(QAction* action) const
{
    int index = indexOf(action);

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

/**
 * @brief 计算大图标的高度
 * @param setrect
 * @param pannel
 * @return
 */
int SARibbonPannelLayout::calcLargeHeight(const QRect& setrect, const SARibbonPannel* pannel)
{
    const QMargins& mag = pannelContentsMargins();
    return setrect.height() - mag.top() - mag.bottom() - pannel->titleHeight();
}

/**
 * @brief 全局的contentsMargins
 *
 * 由于ribbonbar的高度估算需要预先知道pannel的contentsMargins，所有需要设置为全局，
 * 另外，设置为全局也是为了风格的统一
 *
 * @return
 */
const QMargins& SARibbonPannelLayout::pannelContentsMargins()
{
    return s_contentsMargins;
}

/**
 * @brief 全局的contentsMargins
 * @note SARibbonStyleOption会用到此函数，调用设置函数后需要手动重新计算SARibbonStyleOption的内容,@sa SARibbonStyleOption::recalc
 * @sa SARibbonStyleOption
 * @param m
 */
void SARibbonPannelLayout::setPannelContentsMargins(const QMargins& m)
{
    s_contentsMargins = m;
}

/**
 * @brief 布局所有action
 */
void SARibbonPannelLayout::layoutActions()
{
    if (m_dirty) {
        updateGeomArray(geometry());
    }
    QList< QWidget* > showWidgets, hideWidgets;

#ifdef SA_RIBBON_DEBUG_HELP_DRAW
    qDebug() << "\r\n\r\n =============================================="
                "\r\n SARibbonPannelLayout::layoutActions"
             << " \r\n name:" << parentWidget()->windowTitle() << " sizehint:" << this->sizeHint();
#endif
    for (SARibbonPannelItem* item : qAsConst(m_items)) {
        if (item->isEmpty()) {
            hideWidgets << item->widget();
        } else {
            item->setGeometry(item->itemWillSetGeometry);
            //            item->widget()->setFixedSize(item->itemWillSetGeometry.size());
            //            item->widget()->move(item->itemWillSetGeometry.topLeft());
            showWidgets << item->widget();
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
            qDebug() << "[" << item->rowIndex << "," << item->columnIndex << "]"
                     << " -> " << item->itemWillSetGeometry << ":" << item->widget()->metaObject()->className();
#endif
        }
    }

    // 不在上面那里进行show和hide因为这会触发SARibbonPannelLayout的重绘，导致循环绘制，非常影响效率
    for (QWidget* w : qAsConst(showWidgets)) {
        w->show();
    }
    for (QWidget* w : qAsConst(hideWidgets)) {
        w->hide();
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
        return (nullptr);
    }
    if (QWidgetAction* widgetAction = qobject_cast< QWidgetAction* >(action)) {
        widget = widgetAction->requestWidget(pannel);
        if (widget != nullptr) {
            widget->setAttribute(Qt::WA_LayoutUsesWidgetRect);
            customWidget = true;  //标记为true，在移除的时候是不会对这个窗口进行删除，false默认会进行删除如SARibbonSeparatorWidget和SARibbonToolButton
        }
    } else if (action->isSeparator()) {
        SARibbonSeparatorWidget* sep = RibbonSubElementDelegate->createRibbonSeparatorWidget(pannel);
        widget                       = sep;
    }
    //不是widget，自动生成SARibbonToolbutton
    if (!widget) {
        SARibbonToolButton::RibbonButtonType buttonType = ((rp == SARibbonPannelItem::Large) ? SARibbonToolButton::LargeButton
                                                                                             : SARibbonToolButton::SmallButton);

        SARibbonToolButton* button = RibbonSubElementDelegate->createRibbonToolButton(pannel);
        button->setAutoRaise(true);
        button->setFocusPolicy(Qt::NoFocus);
        button->setButtonType(buttonType);
        if (SARibbonToolButton::LargeButton == buttonType) {
            //根据pannel的模式设置button样式
            button->setLargeButtonType((pannel->isTwoRow()) ? SARibbonToolButton::Lite : SARibbonToolButton::Normal);
        }
        button->setDefaultAction(action);
        //根据QAction的属性设置按钮的大小

        QObject::connect(button, &SARibbonToolButton::triggered, pannel, &SARibbonPannel::actionTriggered);
        widget = button;
    }
    //这时总会有widget
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

    int height          = setrect.height();
    const QMargins& mag = pannelContentsMargins();
    const int spacing   = this->spacing();
    int x               = mag.left();
    //获取pannel的布局模式 3行或者2行
    // rowcount 是ribbon的行，有2行和3行两种
    const short rowCount = (pannel->pannelLayoutMode() == SARibbonPannel::ThreeRowMode) ? 3 : 2;
    // largeHeight是对应large占比的高度,pannel->titleHeight()在两行模式返回0
    const int largeHeight = calcLargeHeight(setrect, pannel);

    m_largeHeight = largeHeight;
    //计算smallHeight的高度
    const int smallHeight = (largeHeight - (rowCount - 1) * spacing) / rowCount;
    // Medium行的y位置
    const int yMediumRow0 = (2 == rowCount) ? mag.top() : (mag.top() + ((largeHeight - 2 * smallHeight) / 3));
    const int yMediumRow1 = (2 == rowCount) ? (mag.top() + smallHeight + spacing)
                                            : (mag.top() + ((largeHeight - 2 * smallHeight) / 3) * 2 + smallHeight);
    // Small行的y位置
    const int ySmallRow0 = mag.top();
    const int ySmallRow1 = mag.top() + smallHeight + spacing;
    const int ySmallRow2 = mag.top() + 2 * (smallHeight + spacing);
    // row用于记录下个item应该属于第几行，item->rowIndex用于记录当前处于第几行，
    // item->rowIndex主要用于SARibbonPannelItem::Medium
    short row  = 0;
    int column = 0;
    //记录每列最大的宽度
    int columMaxWidth = 0;
    //记录总宽度
    int totalWidth = 0;

    int itemCount = m_items.count();

#ifdef SA_RIBBON_DEBUG_HELP_DRAW
    qDebug() << "\r\n\r\n============================================="
             << "\r\nSARibbonPannelLayout::updateGeomArray()"
             << " setrect:" << setrect << "\r\npannel name:" << pannel->windowTitle()
             << "\r\n largeHeight:" << largeHeight << "\r\n smallHeight:" << smallHeight << "\r\n rowCount:" << rowCount;
#endif

    //本列第一、二行占比
    SARibbonPannelItem::RowProportion thisColumnRP0 = SARibbonPannelItem::None;
    SARibbonPannelItem* lastGeomItem                = nullptr;  //记录最后一个设置位置的item
    for (int i = 0; i < itemCount; ++i) {
        SARibbonPannelItem* item = m_items.at(i);
        if (item->isEmpty()) {
            //如果是hide就直接跳过
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
            qDebug() << item->widget()->metaObject()->className() << "is hide"
                     << " row:" << row << " col:" << column;
#endif
            item->rowIndex    = -1;
            item->columnIndex = -1;
            continue;
        }

        QSize hint           = item->sizeHint();
        Qt::Orientations exp = item->expandingDirections();
        if (item->widget()) {
            //有窗口是水平扩展，则标记为扩展
            if ((item->widget()->sizePolicy().horizontalPolicy() & QSizePolicy::ExpandFlag)) {
                m_expandFlag = true;
            }
        }
        SARibbonPannelItem::RowProportion rp = item->rowProportion;
        if (SARibbonPannelItem::None == rp) {
            //为定义行占比但是垂直扩展，就定义为Large占比，否则就是small占比
            if (exp & Qt::Vertical) {
                rp = SARibbonPannelItem::Large;
            } else {
                rp = SARibbonPannelItem::Small;
            }
        }
        //开始根据占比和layoutmode来布局
        switch (rp) {
        case SARibbonPannelItem::Large: {
            // ！！在Large，如果不是处于新列的第一行，就需要进行换列处理
            // 把large一直设置在下一列的开始
            if (row != 0) {
                x += (columMaxWidth + spacing);
                ++column;
                row           = 0;
                columMaxWidth = 0;
            }
            //
            item->rowIndex            = 0;
            item->columnIndex         = column;
            item->itemWillSetGeometry = QRect(x, mag.top(), hint.width(), largeHeight);
            columMaxWidth             = hint.width();
            //换列，x自动递增到下个坐标，列数增加，行数归零，最大列宽归零
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
                    //下个row为1
                    row = 1;
                    // x不变
                } else {
                    item->rowIndex            = 1;
                    item->columnIndex         = column;
                    item->itemWillSetGeometry = QRect(x, yMediumRow1, hint.width(), smallHeight);
                    //和上个进行比较得到最长宽度
                    columMaxWidth = qMax(columMaxWidth, hint.width());
                    //换列，x自动递增到下个坐标，列数增加，行数归零，最大列宽归零
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
                    //换列，x自动递增到下个坐标，列数增加，行数归零，最大列宽归零
                    x += (columMaxWidth + spacing);
                    row           = 0;
                    columMaxWidth = 0;
                    ++column;
                } else {
                    //这种模式一般情况会发生在当前列前两行是Small，添加了一个Medium
                    //这时需要先换列
                    //换列，x自动递增到下个坐标，列数增加，行数归零，最大列宽归零
                    x += (columMaxWidth + spacing);
                    ++column;
                    row           = 0;
                    columMaxWidth = 0;
                    //换列后此时等价于0 == row
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
                //第一行
                item->rowIndex            = 0;
                item->columnIndex         = column;
                item->itemWillSetGeometry = QRect(x, ySmallRow0, hint.width(), smallHeight);
                thisColumnRP0             = SARibbonPannelItem::Small;
                columMaxWidth             = hint.width();
                //下个row为1
                row = 1;
                // x不变
            } else if (1 == row) {
                //第二行
                item->rowIndex            = 1;
                item->columnIndex         = column;
                item->itemWillSetGeometry = QRect(x, ySmallRow1, hint.width(), smallHeight);
                if ((3 == rowCount) && (SARibbonPannelItem::Medium == thisColumnRP0)) {
                    //三行模式，并且第一行是Medium
                    item->itemWillSetGeometry = QRect(x, yMediumRow1, hint.width(), smallHeight);
                }
                //和上个进行比较得到最长宽度
                columMaxWidth = qMax(columMaxWidth, hint.width());
                //这里要看两行还是三行，确定是否要换列
                if (2 == rowCount) {
                    //两行模式，换列
                    //换列，x自动递增到下个坐标，列数增加，行数归零，最大列宽归零
                    x += (columMaxWidth + spacing);
                    row           = 0;
                    columMaxWidth = 0;
                    ++column;
                } else {
                    //三行模式，继续增加行数
                    row = 2;
                    // x不变
                }
                if ((3 == rowCount) && (SARibbonPannelItem::Medium == thisColumnRP0)) {
                    //三行模式，并且第一行是Medium，换列
                    //换列，x自动递增到下个坐标，列数增加，行数归零，最大列宽归零
                    x += (columMaxWidth + spacing);
                    row           = 0;
                    columMaxWidth = 0;
                    ++column;
                }
            } else {
                //第三行
                item->rowIndex            = 2;
                item->columnIndex         = column;
                item->itemWillSetGeometry = QRect(x, ySmallRow2, hint.width(), smallHeight);
                //和上个进行比较得到最长宽度
                columMaxWidth = qMax(columMaxWidth, hint.width());
                //换列，x自动递增到下个坐标，列数增加，行数归零，最大列宽归零
                x += (columMaxWidth + spacing);
                row           = 0;
                columMaxWidth = 0;
                ++column;
            }
        } break;

        default:
            //不可能出现
            break;
        }
        lastGeomItem = item;
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
        qDebug() << item->widget()->metaObject()->className() << " rp:" << rp << " row:" << item->rowIndex
                 << " col:" << item->columnIndex << " new row:" << row << " new column:" << column
                 << " itemWillSetGeometry:" << item->itemWillSetGeometry << " sizeHint:" << hint << " x:" << x;
#endif
    }
    //最后一个元素，更新列数
    // 2022-06-20 此句本来在循环里面，如果最后一个元素隐藏，会导致无法到达此判断导致异常
    if (lastGeomItem) {  //最后一个元素，更新totalWidth
        if (lastGeomItem->columnIndex != column) {
            //说明最后一个元素处于最后位置，触发了换列，此时真实列数需要减1，直接等于column索引
            m_columnCount = column;
            //由于最后一个元素触发了换列，x值是新一列的位置，直接作为totalWidth
            totalWidth = x + mag.right();
        } else {
            //说明最后一个元素处于非最后位置，没有触发下一个换列，此时真实列数等于column索引+1
            m_columnCount = column + 1;
            //由于最后一个元素未触发换列，需要计算totalWidth
            totalWidth = x + columMaxWidth + spacing + mag.right();
        }
    }
    //在有optionButton情况下，的2行模式，需要调整totalWidth
    if (pannel->isTwoRow()) {
        if (pannel->isHaveOptionAction()) {
            totalWidth += pannel->optionActionButtonSize().width();
        }
    }
    //在设置完所有窗口后，再设置扩展属性的窗口
    if (totalWidth < setrect.width()) {
        //说明可以设置扩展属性的窗口
        recalcExpandGeomArray(setrect);
    }
    this->m_sizeHint = QSize(totalWidth, height);
}

void SARibbonPannelLayout::recalcExpandGeomArray(const QRect& setrect)
{
    //计算能扩展的尺寸
    int expandwidth = setrect.width() - this->m_sizeHint.width();

    if (expandwidth <= 0) {
        //没有必要设置
        return;
    }
    //列扩展信息
    struct _columnExpandInfo
    {
        int oldColumnWidth      = 0;   ///< 原来的列宽
        int columnMaximumWidth  = -1;  ///< 列的最大宽度
        int columnExpandedWidth = 0;   ///< 扩展后列的宽度
        QList< SARibbonPannelItem* > expandItems;
    };
    //此变量用于记录可以水平扩展的列和控件，在布局结束后，如果还有空间，就把水平扩展的控件进行扩展
    QMap< int, _columnExpandInfo > columnExpandInfo;

    for (SARibbonPannelItem* item : qAsConst(m_items)) {
        if ((!item->isEmpty()) && item->expandingDirections() & Qt::Horizontal) {
            //只获取可见的
            QMap< int, _columnExpandInfo >::iterator i = columnExpandInfo.find(item->columnIndex);
            if (i == columnExpandInfo.end()) {
                i = columnExpandInfo.insert(item->columnIndex, _columnExpandInfo());
            }
            i.value().expandItems.append(item);
        }
    }
    if (columnExpandInfo.size() <= 0) {
        //没有需要扩展的就退出
        return;
    }
    //获取完可扩展的列和控件后，计算对应的列的尺寸
    //计算能扩展的尺寸
    int oneColCanexpandWidth = expandwidth / columnExpandInfo.size();

    for (QMap< int, _columnExpandInfo >::iterator i = columnExpandInfo.begin(); i != columnExpandInfo.end();) {
        int& oldColumnWidth     = i.value().oldColumnWidth;
        int& columnMaximumWidth = i.value().columnMaximumWidth;
        this->columnWidthInfo(i.key(), oldColumnWidth, columnMaximumWidth);
        if ((oldColumnWidth <= 0) || (oldColumnWidth > columnMaximumWidth)) {
            //如果小于0说明没有这个列，这种属于异常，删除继续
            // oldColumnWidth > columnMaximumWidth也是异常
            i = columnExpandInfo.erase(i);
            continue;
        }
        //开始调整
        int colwidth = oneColCanexpandWidth + oldColumnWidth;  //先扩展了
        if (colwidth >= columnMaximumWidth) {
            //过最大宽度要求
            i.value().columnExpandedWidth = columnMaximumWidth;
        } else {
            i.value().columnExpandedWidth = colwidth;
        }
        ++i;
    }
    //从新调整尺寸
    //由于会涉及其他列的变更，因此需要所有都遍历一下
    for (auto i = columnExpandInfo.begin(); i != columnExpandInfo.end(); ++i) {
        int moveXLen = i.value().columnExpandedWidth - i.value().oldColumnWidth;
        for (SARibbonPannelItem* item : this->m_items) {
            if (item->isEmpty() || (item->columnIndex < i.key())) {
                //之前的列不用管
                continue;
            }
            if (item->columnIndex == i.key()) {
                //此列的扩展
                if (i.value().expandItems.contains(item)) {
                    //此列需要扩展的item才扩展尺寸
                    item->itemWillSetGeometry.setWidth(i.value().columnExpandedWidth);
                } else {
                    //此列不扩展的模块保持原来的尺寸
                    continue;
                }
            } else {
                //后面的移动
                item->itemWillSetGeometry.moveLeft(item->itemWillSetGeometry.x() + moveXLen);
            }
        }
    }
}

/**
 * @brief 返回所有列的区域
 * @return <列索引，列区域>
 */
// QMap<int, QRect> SARibbonPannelLayout::columnsGeometry() const
//{
//    QMap<int, QRect> res;
//    for (SARibbonPannelItem *item:m_items){
//        if(item->isEmpty()){
//           continue;
//        }
//        QMap<int, QRect>::iterator i = res.find(item->columnIndex);
//        if(i == res.end())
//        {
//            QRect r = item->geometry();
//            r.setY(this->contentsMargins().top());
//            r.setHeight(m_largeHeight);
//            i = res.insert(item->columnIndex,r);
//        }
//        if(item->itemWillSetGeometry.width() > i.value().width()){
//            i.value().setWidth(item->itemWillSetGeometry.width());
//        }
//    }
//    return res;
//}

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

void SARibbonPannelLayout::setGeometry(const QRect& rect)
{
    m_dirty = false;
    updateGeomArray(rect);
    QLayout::setGeometry(rect);
    layoutActions();
}
