#ifndef SARIBBONPANELITEM_H
#define SARIBBONPANELITEM_H
#include "SARibbonGlobal.h"
#include <QWidgetItem>
#include <QAction>
class SARibbonToolButton;
/**
 * \if ENGLISH
 * @brief Abstraction for all child windows of the panel, reference Qt's toolbar
 * @details Reference Qt's toolbar, all child window content of the panel is abstracted through QAction, including gallery windows, which are also abstracted through QAction
 * @details QAction will eventually be converted to SARibbonPanelItem, each SARibbonPanelItem contains a widget, and the layout of SARibbonPanel
 * @details is based on SARibbonPanelItem
 * @details Action without window will generate a SARibbonToolButton internally
 * \endif
 *
 * \if CHINESE
 * @brief 是对panel所有子窗口的抽象，参考qt的toolbar
 * @details 参考qt的toolbar，panel所有子窗口内容都通过QAction进行抽象，包括gallery这些窗口，也是通过QAction进行抽象
 * @details QAction最终会转换为SARibbonPanelItem，每个SARibbonPanelItem都含有一个widget，SARibbonPanel的布局
 * @details 就基于SARibbonPanelItem
 * @details 无窗口的action会在内部生成一个SARibbonToolButton
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonPanelItem : public QWidgetItem
{
public:
	/**
	 * \if ENGLISH
	 * @brief Defines the row proportion, there are three types of proportions in ribbon: large, medium and small
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 定义了行的占比，ribbon中有large，media和small三种占比
	 * \endif
	 */
	enum RowProportion
	{
		None,  ///< Undefined proportion, at this time it will be judged based on expandingDirections, if there is Qt::Vertical, it is equivalent to Large, otherwise it is Small
		Large,   ///< Large proportion, the height of a widget will fill the entire panel
		Medium,  ///< Medium proportion, only works when @ref SARibbonPanel::panelLayoutMode is @ref SARibbonPanel::ThreeRowMode, and when both in the same column are Medium, they will occupy two rows in three rows
		Small  ///< Small proportion, occupies one row of SARibbonPanel, Medium will also become Small when conditions are not met, but will not become Large
	};
	/// Constructor for SARibbonPanelItem
	explicit SARibbonPanelItem(QWidget* widget);
	/// Destructor for SARibbonPanelItem
	~SARibbonPanelItem();

	/// Check if the item is empty
	bool isEmpty() const Q_DECL_OVERRIDE;

	short rowIndex;             ///< Record which row the current item belongs to, -1 in hide mode
	int columnIndex;            ///< Record which column the current item belongs to, -1 in hide mode
	QRect itemWillSetGeometry;  ///< This will be updated when calling SARibbonPanelLayout::updateGeomArray, the actual setting will use QWidgetItem::setGeometry to set Geometry
	QAction* action;            /// < Record action, reference QToolBarLayoutItem
	bool customWidget;  ///< For action without window, there will actually be a SARibbonToolButton, which needs to be deleted during destruction
	SARibbonPanelItem::RowProportion rowProportion;  ///< Row proportion, there are three types of proportions in ribbon: large, medium and small, see @ref RowProportion
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
