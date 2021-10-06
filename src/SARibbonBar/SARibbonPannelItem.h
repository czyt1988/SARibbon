#ifndef SARIBBONPANNELITEM_H
#define SARIBBONPANNELITEM_H
#include "SARibbonGlobal.h"
#include <QWidgetItem>
#include <QAction>

/**
 * @brief 是对pannel所有子窗口的抽象，参考qt的toolbar
 *
 * 参考qt的toolbar，pannel所有子窗口内容都通过QAction进行抽象，包括gallery这些窗口，也是通过QAction进行抽象
 * QAction最终会转换为SARibbonPannelItem，每个SARibbonPannelItem都含有一个widget，SARibbonPannel的布局
 * 就基于SARibbonPannelItem
 *
 * 无窗口的action会在内部生成一个SARibbonToolButton，
 */
class SA_RIBBON_EXPORT SARibbonPannelItem : public QWidgetItem
{
public:

    /**
     * @brief 定义了行的占比，ribbon中有large，media和small三种占比
     */
    enum RowProportion {
        None            ///< 为定义占比，这时候将会依据expandingDirections来判断，如果能有Qt::Vertical，就等同于Large，否则就是Small
        , Large         ///< 大占比，一个widget的高度会充满整个pannel
        , Medium        ///< 中占比，在@ref SARibbonPannel::pannelLayoutMode 为 @ref SARibbonPannel::ThreeRowMode 时才会起作用，且要同一列里两个都是Medium时，会在三行中占据两行
        , Small         ///< 小占比，占SARibbonPannel的一行，Medium在不满足条件时也会变为Small，但不会变为Large
    };
    SARibbonPannelItem(QWidget *widget);
    bool isEmpty() const Q_DECL_OVERRIDE;

    short rowIndex;                                         ///< 记录当前item属于第几行，hide模式下为-1
    int columnIndex;                                        ///< 记录当前item属于第几列，hide模式下为-1
    QRect itemWillSetGeometry;                              ///< 在调用SARibbonPannelLayout::updateGeomArray会更新这个此处，实际设置的时候会QWidgetItem::setGeometry设置Geometry
    QAction *action;                                        /// < 记录action，参考QToolBarLayoutItem
    bool customWidget;                                      ///< 对于没有窗口的action，实际也会有一个SARibbonToolButton，在销毁时要delete掉
    SARibbonPannelItem::RowProportion rowProportion;        ///< 行的占比，ribbon中有large，media和small三种占比,见@ref RowProportion
};
#ifndef SARibbonPannelItemRowProportionPropertyName
#define SARibbonPannelItemRowProportionPropertyName    "SARibbonPannelItem_RowProportion"
#endif
#endif // SARIBBONPANNELITEM_H
