#ifndef SARIBBONPANELLAYOUT_H
#define SARIBBONPANELLAYOUT_H
#include "SARibbonGlobal.h"
#include <QLayout>
#include "SARibbonPanelItem.h"
class QToolButton;
class SARibbonPanel;
class SARibbonPanelLabel;

/**
 * @brief Layout manager for SARibbonPanel / SARibbonPanel的布局管理器
 *
 * `SARibbonPanelLayout` is a custom `QLayout` subclass responsible for arranging `SARibbonToolButton`
 * and other widgets within a `SARibbonPanel`. It handles the complex Ribbon-specific layout logic,
 * including Large, Medium, and Small button proportions, and the optional title and option button.
 *
 * `SARibbonPanelLayout` 是一个自定义的 `QLayout` 子类，负责在 `SARibbonPanel` 内排列 `SARibbonToolButton`
 * 和其他窗口部件。它处理复杂的Ribbon特定布局逻辑，包括大、中、小按钮比例，以及可选的标题和选项按钮。
 */
class SA_RIBBON_EXPORT SARibbonPanelLayout : public QLayout
{
	Q_OBJECT
	friend class SARibbonPanel;

public:
	explicit SARibbonPanelLayout(QWidget* p = nullptr);
	~SARibbonPanelLayout();
	// Gets the SARibbonPanel that owns this layout / 获取拥有此布局的SARibbonPanel
	SARibbonPanel* ribbonPanel() const;

	// Inserts an action at a specific index / 在指定索引处插入一个action
	void insertAction(int index, QAction* act, SARibbonPanelItem::RowProportion rp = SARibbonPanelItem::None);

	// Sets the option action for the panel / 为面板设置选项action
	void setOptionAction(QAction* action);

	// Checks if an option action is set / 检查是否设置了选项action
	bool isHaveOptionAction() const;

	// Retrieves the SARibbonPanelItem associated with an action / 获取与action关联的SARibbonPanelItem
	SARibbonPanelItem* panelItem(QAction* action) const;

	// Gets the last item added to the layout / 获取最后添加到布局的项目
	SARibbonPanelItem* lastItem() const;

	// Gets the widget associated with the last item / 获取与最后一个项目关联的窗口部件
	QWidget* lastWidget() const;

	// Moves an item from one index to another / 将一个项目从一个索引移动到另一个索引
	void move(int from, int to);
	// 判断是否需要重新布局
	bool isDirty() const;
	// 更新尺寸
	void updateGeomArray();

	// Finds the index of an action in the layout / 在布局中查找一个action的索引
	int indexByAction(QAction* action) const;

	// Gets the height of the panel's title / 获取面板标题的高度
	int panelTitleHeight() const;

	// Sets the height of the panel's title / 设置面板标题的高度
	void setPanelTitleHeight(int newTitleHeight);

	// Checks if the panel's title is enabled for display / 检查面板标题是否启用显示
	bool isEnableShowPanelTitle() const;

	// Sets whether the panel's title is enabled for display / 设置面板标题是否启用显示
	void setEnableShowPanelTitle(bool on);

	// Gets the height of large buttons / 获取大按钮的高度
	int largeButtonHeight() const;

	// Gets the spacing between the title and the buttons / 获取标题与按钮之间的间距
	int panelTitleSpace() const;

	// Sets the spacing between the title and the buttons / 设置标题与按钮之间的间距
	void setPanelTitleSpace(int newTitleSpace);

	// Gets the panel's title label / 获取面板的标题标签
	SARibbonPanelLabel* panelTitleLabel() const;

	// Sets the default icon size for tool buttons / 设置工具按钮的默认图标尺寸
	void setToolButtonIconSize(const QSize& s);

	// Gets the default icon size for tool buttons / 获取工具按钮的默认图标尺寸
	QSize toolButtonIconSize() const;

	// 是否允许文字换行
	bool isEnableWordWrap() const;
	// 按钮的最大宽高比，这个系数决定按钮的最大宽度
	qreal buttonMaximumAspectRatio() const;

public:
	// Adds an item to the layout (SARibbonPanelLayout not supported) / 向布局添加一个项目（SARibbonPanelLayout不支持）
	void addItem(QLayoutItem* item) Q_DECL_OVERRIDE;

	// QLayout 所必须的重载函数
	QLayoutItem* itemAt(int index) const Q_DECL_OVERRIDE;

	// Removes and returns the item at the specified index / 移除并返回指定索引处的项目
	QLayoutItem* takeAt(int index) Q_DECL_OVERRIDE;

	// Gets the number of items in the layout / 获取布局中项目的数量
	int count() const Q_DECL_OVERRIDE;

	// Checks if the layout is empty / 检查布局是否为空
	bool isEmpty() const Q_DECL_OVERRIDE;

	// Invalidates the layout, marking it as dirty / 使布局失效，将其标记为“脏”
	void invalidate() Q_DECL_OVERRIDE;

	// Returns the directions in which the layout can expand / 返回布局可以扩展的方向
	Qt::Orientations expandingDirections() const Q_DECL_OVERRIDE;
	void setGeometry(const QRect& rect) Q_DECL_OVERRIDE;
	QSize minimumSize() const Q_DECL_OVERRIDE;
	QSize sizeHint() const Q_DECL_OVERRIDE;

protected:
	// Gets the size of the option action button / 获取选项action按钮的尺寸
	QSize optionActionButtonSize() const;
	// 布局action
	void doLayout();
	// 把action转换为item，对于纯Action，此函数会创建SARibbonToolButton,
	//  rp用于告诉Layout生成什么样的窗口，详细见SARibbonPanelItem::RowProportion
	SARibbonPanelItem* createItem(QAction* action, SARibbonPanelItem::RowProportion rp = SARibbonPanelItem::None);
	void updateGeomArray(const QRect& setrect);
	// 重新计算扩展条码，此函数必须在updateGeomArray函数之后调用
	void recalcExpandGeomArray(const QRect& setrect);
	// 设置文字允许换行
	void setEnableWordWrap(bool on);
	// 按钮的最大宽高比，这个系数决定按钮的最大宽度
	void setButtonMaximumAspectRatio(qreal fac = 1.4);

private:
	// 根据列数，计算窗口的宽度，以及最大宽度
	void columnWidthInfo(int colindex, int& width, int& maximum) const;

	//  Sets the panel's title label / 设置面板的标题标签
	void setPanelTitleLabel(SARibbonPanelLabel* newTitleLabel);

private:
	QList< SARibbonPanelItem* > mItems;
	int mColumnCount { 0 };                       ///< 记录有多少列
	bool mExpandFlag { false };                   ///< 标记是否是会扩展的
	QSize mSizeHint;                              ///< sizeHint返回的尺寸
	QSize mDefaultToolButtonIconSize { 22, 22 };  ///< 记录默认的按钮图标尺寸
	bool mDirty { true };                         ///< 用于标记是否需要刷新元素，参考QToolBarLayout源码
	int mLargeHeight { 0 };                       ///< 记录大图标的高度
	int mTitleHeight { 15 };                      ///< 标题区域高度
	int mTitleSpace { 2 };                        ///< 标题区域和按钮的间隔
	bool mEnableShowTitle { true };               ///< 是否运行显示panel标题
	SARibbonPanelLabel* mTitleLabel { nullptr };  ///< titlelabel指针
	QRect mTitleLabelGeometry;                    ///< titlelabel的位置
	QToolButton* mOptionActionBtn { nullptr };    ///< optionAction对应的button
	QRect mOptionActionBtnGeometry;               ///< optionAction的位置
	bool mEnableWordWrap { true };                ///< 是否允许文字换行
	qreal mButtonMaximumAspectRatio { 1.4 };      ///< 按钮的宽高比
};

#endif  // SARIBBONPANELLAYOUT_H
