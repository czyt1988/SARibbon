#ifndef SARIBBONPANELITEM_H
#define SARIBBONPANELITEM_H
#include "SARibbonGlobal.h"
#include <QWidgetItem>
#include <QAction>
class SARibbonToolButton;
/**
 * @brief 是对panel所有子窗口的抽象，参考qt的toolbar
 *
 * 参考qt的toolbar，panel所有子窗口内容都通过QAction进行抽象，包括gallery这些窗口，也是通过QAction进行抽象
 * QAction最终会转换为SARibbonPanelItem，每个SARibbonPanelItem都含有一个widget，SARibbonPanel的布局
 * 就基于SARibbonPanelItem
 *
 * 无窗口的action会在内部生成一个SARibbonToolButton，
 */
class SA_RIBBON_EXPORT SARibbonPanelItem : public QWidgetItem
{
public:
	/**
	 * @brief 定义了行的占比，ribbon中有large，media和small三种占比
	 */
	enum RowProportion
	{
		None,  ///< 为定义占比，这时候将会依据expandingDirections来判断，如果能有Qt::Vertical，就等同于Large，否则就是Small
		Large,   ///< 大占比，一个widget的高度会充满整个panel
		Medium,  ///< 中占比，在@ref SARibbonPanel::panelLayoutMode 为 @ref SARibbonPanel::ThreeRowMode 时才会起作用，且要同一列里两个都是Medium时，会在三行中占据两行
		Small  ///< 小占比，占SARibbonPanel的一行，Medium在不满足条件时也会变为Small，但不会变为Large
	};
	explicit SARibbonPanelItem(QWidget* widget);
	~SARibbonPanelItem();

	bool isEmpty() const Q_DECL_OVERRIDE;

	short rowIndex;             ///< 记录当前item属于第几行，hide模式下为-1
	int columnIndex;            ///< 记录当前item属于第几列，hide模式下为-1
	QRect itemWillSetGeometry;  ///< 在调用SARibbonPanelLayout::updateGeomArray会更新这个此处，实际设置的时候会QWidgetItem::setGeometry设置Geometry
	QAction* action;            /// < 记录action，参考QToolBarLayoutItem
	bool customWidget;  ///< 对于没有窗口的action，实际也会有一个SARibbonToolButton，在销毁时要delete掉
	SARibbonPanelItem::RowProportion rowProportion;  ///< 行的占比，ribbon中有large，media和small三种占比,见@ref RowProportion
};
#ifndef SA_ActionPropertyName_RowProportion
#define SA_ActionPropertyName_RowProportion "_sa_RowProportion"
#endif
#ifndef SA_ActionPropertyName_ToolButtonPopupMode
#define SA_ActionPropertyName_ToolButtonPopupMode "_sa_ToolButtonPopupMode"
#endif
#ifndef SA_ActionPropertyName_ToolButtonStyle
#define SA_ActionPropertyName_ToolButtonStyle "_sa_ToolButtonStyle"
#endif
#endif  // SARIBBONPANELITEM_H
