#ifndef SARIBBONPANELITEM_H
#define SARIBBONPANELITEM_H
#include "SARibbonGlobal.h"
#include <SARibbonCore/SARibbonAbstractLayoutItem.h>
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
class SA_RIBBON_EXPORT SARibbonPanelItem : public QWidgetItem, public SARibbon::Core::SARibbonAbstractLayoutItem
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
	// 计划 02 S1：枚举本体已提升至 SARibbon::Core::SARibbonRowProportion（core/global/SARibbonEnums.h）。
	// 类作用域的 using 声明无法引入命名空间枚举符（MSVC C2886/标准 [namespace.udecl]，
	// 计划 round3 断言有误，见 NOTES B21），改用类型别名 + static constexpr 成员，
	// SARibbonPanelItem::Large / 类内裸名 / 隐式 int 转换三类存量用法全部保持可编译。
	using RowProportion = SARibbon::Core::SARibbonRowProportion;
	static constexpr RowProportion None   = SARibbon::Core::None;   ///< Undefined proportion, judged by expandingDirections
	static constexpr RowProportion Large  = SARibbon::Core::Large;  ///< Large proportion, fills the entire panel height
	static constexpr RowProportion Medium = SARibbon::Core::Medium; ///< Medium proportion, only works in ThreeRowMode
	static constexpr RowProportion Small  = SARibbon::Core::Small;  ///< Small proportion, occupies one row
	// Constructor for SARibbonPanelItem
	explicit SARibbonPanelItem(QWidget* widget);
	// Destructor for SARibbonPanelItem
	~SARibbonPanelItem();

	// Check if the item is empty
	bool isEmpty() const Q_DECL_OVERRIDE;

	// Contract + QLayoutItem same-signature virtuals: explicit override disambiguates
	// the two base declarations (no unique final overrider otherwise), plan-02 S5.1-1
	QSize sizeHint() const Q_DECL_OVERRIDE;
	Qt::Orientations expandingDirections() const Q_DECL_OVERRIDE;
	// Contract: Panel-side isHidden == action not visible (2.x isEmpty semantics, plan-02 S5.1-1)
	bool isHidden() const Q_DECL_OVERRIDE;
	// Contract: engine geometry application (widgets: QWidgetItem::setGeometry path)
	void applyGeometry(const QRect& rect) Q_DECL_OVERRIDE;

	// Step A（计划 02 S5.1）：本类同时实现 core 契约（SARibbonAbstractLayoutItem）。
	// rowIndex/columnIndex/itemWillSetGeometry/isExpandItem/rowProportion 与契约的同名字段
	// 构成遮蔽（2.x 拼写保持，算法读写这五个名字零改动）；Step B 搬移时统一为契约字段。
	short rowIndex;             ///< Record which row the current item belongs to, -1 in hide mode
	int columnIndex;            ///< Record which column the current item belongs to, -1 in hide mode
	QRect itemWillSetGeometry;  ///< This will be updated when calling SARibbonPanelLayout::updateGeomArray, the actual setting will use QWidgetItem::setGeometry to set Geometry
	QAction* action;            /// < Record action, reference QToolBarLayoutItem
	bool customWidget;  ///< For action without window, there will actually be a SARibbonToolButton, which needs to be deleted during destruction
	SARibbonPanelItem::RowProportion rowProportion;  ///< Row proportion, there are three types of proportions in ribbon: large, medium and small, see @ref RowProportion
	bool isExpandItem { false };  ///< Temporary flag used by recalcExpandGeomArray to mark expandable items
};
#endif  // SARIBBONPANELITEM_H
