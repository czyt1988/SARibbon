#ifndef SARIBBONPANNEL_H
#define SARIBBONPANNEL_H
#include "SARibbonGlobal.h"
#include "SARibbonPannelItem.h"
#include "SARibbonPannelLayout.h"
#include "SARibbonToolButton.h"
#include <QLayout>
#include <QWidget>
#include <QLabel>
class SARibbonMenu;
class SARibbonGallery;
class QGridLayout;
class SARibbonPannelOptionButton;
class SARibbonPannelLayout;
class SARibbonCategory;
class SARibbonBar;

/**
 * @brief SARibbonPannel的标题label，此类用于qss
 */
class SA_RIBBON_EXPORT SARibbonPannelLabel : public QLabel
{
	Q_OBJECT
public:
	SARibbonPannelLabel(QWidget* parent = nullptr);
};

/**
 * @brief pannel页窗口，pannel是ribbon的面板用于承放控件 / Ribbon panel container, used to hold controls
 *
 * The `SARibbonPannel` is a core component of the Ribbon interface. It acts as a container for `QAction` objects,
 * automatically creating and managing `SARibbonToolButton` instances based on the specified layout proportion (Large, Medium, Small).
 * It supports dynamic layout modes (ThreeRowMode, TwoRowMode) and can display an optional title and an option button.
 *
 * `SARibbonPannel` 是 Ribbon 界面的核心组件之一。它作为 `QAction` 对象的容器，
 * 根据指定的布局比例（大、中、小）自动创建和管理 `SARibbonToolButton` 实例。
 * 它支持动态布局模式（三行模式、两行模式），并可显示可选的标题和选项按钮。
 *
 * ---
 *
 * ### 布局模式 / Layout Modes
 * The pannel has two layout modes: two-row and three-row.
 * - **ThreeRowMode**: The Ribbon in Microsoft Office is a representative.
 * - **TwoRowMode**: The "compact style" represented by WPS.
 * You can specify the mode via @ref SARibbonBar::RibbonStyle (using @ref SARibbonBar::setRibbonStyle).
 *
 * ribbon的pannel分为两行模式和三行模式。
 * - **三行模式**: 以 Microsoft Office 为代表。
 * - **两行模式**: 以 WPS 为代表的“紧凑派”。
 * 可通过 @ref SARibbonBar::RibbonStyle (使用 @ref SARibbonBar::setRibbonStyle 函数) 来指定模式。
 *
 * ---
 *
 * ### 扩展模式 / Expanding Mode
 * Within a pannel, you can use @ref setExpanding to specify horizontal expansion.
 * **Recommendation**: Only use this if the pannel contains expandable widgets (e.g., @ref SARibbonGallery).
 * Otherwise, blank space will be left.
 *
 * 在pannel中，可以通过@ref setExpanding 函数指定pannel水平扩展。
 * **建议**: 仅在面板内包含可扩展控件（如 @ref SARibbonGallery）时使用此函数。
 * 否则，将会留白。
 *
 * ---
 *
 * ### 自定义布局 / Custom Layout
 * The pannel's layout is implemented by @ref SARibbonPannelLayout.
 * To use a custom layout, inherit from @ref SARibbonElementCreateDelegate::createRibbonPannel.
 *
 * pannel的布局通过@ref SARibbonPannelLayout 来实现。
 * 如需自定义布局，请继承 @ref SARibbonElementCreateDelegate::createRibbonPannel 函数。
 *
 * ---
 *
 * ### 全局属性设置 / Global Property Settings
 * `SARibbonPannel` is a child of `SARibbonCategory`, which belongs to `SARibbonBar`.
 * **Important**: Setting properties directly on a standalone `SARibbonPannel` is often meaningless.
 * Global properties (like title height, spacing) are managed by `SARibbonBar`.
 * Use these functions in `SARibbonBar` instead:
 * - `pannelTitleHeight`
 * - `isEnableShowPannelTitle`
 * - `pannelSpacing`
 * - `pannelToolButtonIconSize`
 *
 * `SARibbonPannel`属于`SARibbonCategory`的子对象，`SARibbonCategory`又属于`SARibbonBar`的对象。
 * **重要**: 单独设置一个`SARibbonPannel`的属性很多情况是没有意义的。
 * 全局性属性（如标题高度、间距）由`SARibbonBar`统一管理。
 * 请改用`SARibbonBar`中的以下函数：
 * - `pannelTitleHeight`
 * - `isEnableShowPannelTitle`
 * - `pannelSpacing`
 * - `pannelToolButtonIconSize`
 */
class SA_RIBBON_EXPORT SARibbonPannel : public QFrame
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonPannel)
	friend class SARibbonBar;
	friend class SARibbonCategory;
	friend class SARibbonCategoryPrivate;
	friend class SARibbonCustomizeWidgetPrivate;
	friend class SARibbonPannelLayout;
	Q_PROPERTY(bool isCanCustomize READ isCanCustomize WRITE setCanCustomize)
	Q_PROPERTY(bool isExpanding READ isExpanding WRITE setExpanding)
	Q_PROPERTY(QString pannelName READ pannelName WRITE setPannelName)
public:
    /**
     * @brief Layout mode for the panel / 面板的布局模式
     */
	enum PannelLayoutMode
	{
        /**
         * @brief Three-row layout mode / 三行布局模式
         *
         * In this mode, small buttons are arranged in three rows, while medium buttons occupy two rows.
         * Large buttons always occupy the full height.
         *
         * 在此模式下，小按钮排列成三行，中等按钮占据两行。
         * 大按钮始终占据整个高度。
         */
        ThreeRowMode,

        /**
         * @brief Two-row layout mode / 两行布局模式
         *
         * In this mode, both small and medium buttons are arranged in two rows.
         * Large buttons always occupy the full height.
         *
         * 在此模式下，小按钮和中等按钮都排列成两行。
         * 大按钮始终占据整个高度。
         */
        TwoRowMode
	};
	Q_ENUM(PannelLayoutMode)
public:
	using FpRibbonToolButtonIterate = std::function< bool(SARibbonToolButton*) >;

public:
	explicit SARibbonPannel(QWidget* parent = nullptr);
	explicit SARibbonPannel(const QString& name, QWidget* parent = nullptr);
	~SARibbonPannel() Q_DECL_OVERRIDE;
	using QWidget::addAction;

	// 把action加入到pannel
    void addAction(QAction* action, SARibbonPannelItem::RowProportion rowProportion);
	// 生成并添加一个action
	void addAction(QAction* act,
                   QToolButton::ToolButtonPopupMode popMode,
                   SARibbonPannelItem::RowProportion rowProportion = SARibbonPannelItem::Large);
	// 把action加入到pannel，并以大图标显示
	void addLargeAction(QAction* action);
	// 把action加入到pannel，在三行模式下会以中图标显示
	void addMediumAction(QAction* action);
	// 把action加入到pannel，并以小图标显示
	void addSmallAction(QAction* action);

	// 把action加入到pannel，并以小图标显示
	void addSmallAction(QAction* action, QToolButton::ToolButtonPopupMode popMode);
	// 把action加入到pannel，并以大图标显示
	void addLargeAction(QAction* action, QToolButton::ToolButtonPopupMode popMode);
	// 把action加入到pannel，在三行模式下会以中图标显示
	void addMediumAction(QAction* action, QToolButton::ToolButtonPopupMode popMode);
    // 生成并添加一个action
	QAction* addAction(const QString& text,
                       const QIcon& icon,
                       QToolButton::ToolButtonPopupMode popMode,
                       SARibbonPannelItem::RowProportion rowProportion = SARibbonPannelItem::Large);

	// 添加menu
    void addMenu(QMenu* menu,
                 SARibbonPannelItem::RowProportion rowProportion,
                 QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);

	// 添加普通大菜单
	void addLargeMenu(QMenu* menu, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);

	// 添加普通小按钮菜单
	void addSmallMenu(QMenu* menu, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);

	// 添加窗口
    QAction* addWidget(QWidget* w, SARibbonPannelItem::RowProportion rowProportion);

	// 添加窗口,占用ribbon的一行
	QAction* addSmallWidget(QWidget* w);

	// 添加窗口,占用ribbon的一行
	QAction* addMediumWidget(QWidget* w);

	// 添加窗口，占用所有行
	QAction* addLargeWidget(QWidget* w);

	// 添加一个Gallery
	SARibbonGallery* addGallery(bool expanding = true);

	// 添加分割线
	QAction* addSeparator();

	// 从pannel中把action对应的button提取出来，如果action没有对应的button，就返回nullptr
	SARibbonToolButton* actionToRibbonToolButton(QAction* action);

	// 设置操作action，如果要去除，传入nullptr指针即可，SARibbonPannel不会对QAction的所有权进行管理
	// OptionAction也会触发actionTriggered信号
	void setOptionAction(QAction* action);

	// 判断是否存在OptionAction
	bool isHaveOptionAction() const;

	// 获取所有的buttons
	QList< SARibbonToolButton* > ribbonToolButtons() const;

	// 获取PannelLayoutMode
	PannelLayoutMode pannelLayoutMode() const;
	void setPannelLayoutMode(PannelLayoutMode mode);

	// 更新按钮的尺寸，这个函数在pannel的布局状态变换后需要调用刷新
	void resetToolButtonSize();

	// 判断是否为2行模式
    bool isTwoRow() const;

	// 把pannel设置为扩展模式，此时会撑大水平区域
	void setExpanding(bool isExpanding = true);
	// 是否是扩展模式
	bool isExpanding() const;

	// 标题栏高度
	int titleHeight() const;

	// 是否显示标题，显示标题后，标题的高度需要设置，默认高度为15
	bool isEnableShowTitle() const;

	// action对应的布局index，此操作一般用于移动moveAction，其他意义不大
	int actionIndex(QAction* act) const;

	// 移动action
	void moveAction(int from, int to);

	// 判断是否可以自定义
	bool isCanCustomize() const;
	void setCanCustomize(bool b);

	// 标题
	QString pannelName() const;
	void setPannelName(const QString& title);

	// 大图标的高度
	int largeButtonHeight() const;

	// 获取布局对应的item,此函数目前仅仅在自定义过程中用到
	const QList< SARibbonPannelItem* >& ribbonPannelItem() const;

	// 获取pannel layout
	SARibbonPannelLayout* pannelLayout() const;

	// 更新布局
	void updateItemGeometry();

	// 获取category指针，如果没有parent，或者不在category管理，返回nullptr
	SARibbonCategory* category() const;

	// 获取ribbonBar指针，如果没有返回nullptr
	SARibbonBar* ribbonBar() const;

	// 按钮之间的间隔
	int spacing() const;

	// 设置按钮的icon尺寸，large action不受此尺寸影响
	void setToolButtonIconSize(const QSize& s);
	QSize toolButtonIconSize() const;

	// 获取最近添加的按钮
	SARibbonToolButton* lastAddActionButton();

	// 获取pannel标题的label控件
	SARibbonPannelLabel* titleLabel() const;

    // 是否允许换行
    bool isEnableWordWrap() const;

    // 按钮的最大宽高比，这个系数决定按钮的最大宽度
    qreal buttonMaximumAspectRatio() const;

    // 此函数会遍历SARibbonPannel下的所有RibbonToolButton，执行函数指针(bool(SARibbonRibbonToolButton*))，函数指针返回false则停止迭代
    bool iterateButton(FpRibbonToolButtonIterate fp) const;

public:
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;
    virtual QSize minimumSizeHint() const Q_DECL_OVERRIDE;

public:
    // pannel高度推荐
    static int pannelHeightHint(const QFontMetrics& fm, PannelLayoutMode layMode, int pannelTitleHeight);
    // 把action的行属性设置进action中，action自身携带了行属性
    static void setActionRowProportionProperty(QAction* action, SARibbonPannelItem::RowProportion rp);
    // 获取action的行属性
    static SARibbonPannelItem::RowProportion getActionRowProportionProperty(QAction* action);
    // 把action的PopupMode属性设置进action中，action自身携带了PopupMode属性
    static void setActionToolButtonPopupModeProperty(QAction* action, QToolButton::ToolButtonPopupMode popMode);
    // 获取action的PopupMode属性
    static QToolButton::ToolButtonPopupMode getActionToolButtonPopupModeProperty(QAction* action);
    // 把action的ToolButtonStyle属性设置进action中，action自身携带了ToolButtonStyle属性
    static void setActionToolButtonStyleProperty(QAction* action, Qt::ToolButtonStyle buttonStyle);
    // 获取action的ToolButtonStyle属性
    static Qt::ToolButtonStyle getActionToolButtonStyleProperty(QAction* action);

Q_SIGNALS:

	/**
	 * @brief 等同于QToolBar::actionTriggered
	 * @param action
	 */
	void actionTriggered(QAction* action);

    /**
     * @brief Emitted when the panel's name changes / 当面板名称改变时发出
     * @param name The new name / 新的名称
     */
	void pannelNameChanged(const QString& n);

protected:
	virtual void actionEvent(QActionEvent* e) Q_DECL_OVERRIDE;
	virtual void changeEvent(QEvent* e) Q_DECL_OVERRIDE;

protected:
    // 以下这些方法是为了接收SARibbonBar的对应接口的参数，用户在操作SARibbonPannel的时候不需要进行设置
    // SARibbonPannel在被设置到SARibbonCategory的时候，SARibbonCategory会重新把自身的这些参数同步到pannel中
    // 同理，SARibbonCategory加入SARibbonBar的时候，SARibbonBar会重新把自身的这些参数同步到category

	// 设置标题栏高度
	void setTitleHeight(int h);

	// 设置按钮之间的间隔
	void setSpacing(int n);

	// 是否显示标题，显示标题后，标题的高度需要设置，默认高度为15
	void setEnableShowTitle(bool on);

    // 设置按钮允许换行，注意图标大小是由文字决定的，两行文字会让图标变小，如果想图标变大，文字不换行是最好的
    void setEnableWordWrap(bool on);

    // 按钮的最大宽高比，这个系数决定按钮的最大宽度
    void setButtonMaximumAspectRatio(qreal fac = 1.4);
};

#endif  // SARIBBONPANNEL_H
