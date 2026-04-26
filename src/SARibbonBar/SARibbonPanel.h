#ifndef SARIBBONPANEL_H
#define SARIBBONPANEL_H
#include "SARibbonGlobal.h"
#include "SARibbonPanelItem.h"
#include "SARibbonToolButton.h"
#include <QLayout>
#include <QWidget>
#include <QLabel>
class SARibbonMenu;
class SARibbonGallery;
class QGridLayout;
class SARibbonPanelOptionButton;
class SARibbonPanelLayout;
class SARibbonCategory;
class SARibbonBar;

/**
 * \if ENGLISH
 * @brief Title label for SARibbonPanel, used for qss styling
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonPanel的标题label，此类用于qss
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonPanelLabel : public QLabel
{
    Q_OBJECT
public:
    /// Constructor for SARibbonPanelLabel
    SARibbonPanelLabel(QWidget* parent = nullptr);
};

/**
 * @brief panel页窗口，panel是ribbon的面板用于承放控件 / Ribbon panel container, used to hold controls
 *
 * The `SARibbonPanel` is a core component of the Ribbon interface. It acts as a container for `QAction` objects,
 * automatically creating and managing `SARibbonToolButton` instances based on the specified layout proportion (Large, Medium, Small).
 * It supports dynamic layout modes (ThreeRowMode, TwoRowMode) and can display an optional title and an option button.
 *
 * `SARibbonPanel` 是 Ribbon 界面的核心组件之一。它作为 `QAction` 对象的容器，
 * 根据指定的布局比例（大、中、小）自动创建和管理 `SARibbonToolButton` 实例。
 * 它支持动态布局模式（三行模式、两行模式），并可显示可选的标题和选项按钮。
 *
 * ---
 *
 * ### 布局模式 / Layout Modes
 * The panel has two layout modes: two-row and three-row.
 * - **ThreeRowMode**: The Ribbon in Microsoft Office is a representative.
 * - **TwoRowMode**: The "compact style" represented by WPS.
 * You can specify the mode via @ref SARibbonBar::RibbonStyle (using @ref SARibbonBar::setRibbonStyle).
 *
 * ribbon的panel分为两行模式和三行模式。
 * - **三行模式**: 以 Microsoft Office 为代表。
 * - **两行模式**: 以 WPS 为代表的“紧凑派”。
 * 可通过 @ref SARibbonBar::RibbonStyle (使用 @ref SARibbonBar::setRibbonStyle 函数) 来指定模式。
 *
 * ---
 *
 * ### 扩展模式 / Expanding Mode
 * Within a panel, you can use @ref setExpanding to specify horizontal expansion.
 * **Recommendation**: Only use this if the panel contains expandable widgets (e.g., @ref SARibbonGallery).
 * Otherwise, blank space will be left.
 *
 * 在panel中，可以通过@ref setExpanding 函数指定panel水平扩展。
 * **建议**: 仅在面板内包含可扩展控件（如 @ref SARibbonGallery）时使用此函数。
 * 否则，将会留白。
 *
 * ---
 *
 * ### 自定义布局 / Custom Layout
 * The panel's layout is implemented by @ref SARibbonPanelLayout.
 * To use a custom layout, inherit from @ref SARibbonElementCreateDelegate::createRibbonPanel.
 *
 * panel的布局通过@ref SARibbonPanelLayout 来实现。
 * 如需自定义布局，请继承 @ref SARibbonElementCreateDelegate::createRibbonPanel 函数。
 *
 * ---
 *
 * ### 全局属性设置 / Global Property Settings
 * `SARibbonPanel` is a child of `SARibbonCategory`, which belongs to `SARibbonBar`.
 * **Important**: Setting properties directly on a standalone `SARibbonPanel` is often meaningless.
 * Global properties (like title height, spacing) are managed by `SARibbonBar`.
 * Use these functions in `SARibbonBar` instead:
 * - `panelTitleHeight`
 * - `isEnableShowPanelTitle`
 * - `panelSpacing`
 * - `panelToolButtonIconSize`
 *
 * `SARibbonPanel`属于`SARibbonCategory`的子对象，`SARibbonCategory`又属于`SARibbonBar`的对象。
 * **重要**: 单独设置一个`SARibbonPanel`的属性很多情况是没有意义的。
 * 全局性属性（如标题高度、间距）由`SARibbonBar`统一管理。
 * 请改用`SARibbonBar`中的以下函数：
 * - `panelTitleHeight`
 * - `isEnableShowPanelTitle`
 * - `panelSpacing`
 * - `panelToolButtonIconSize`
 */
class SA_RIBBON_EXPORT SARibbonPanel : public QFrame
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonPanel)
    friend class SARibbonBar;
    friend class SARibbonCategory;
    friend class SARibbonCategoryPrivate;
    friend class SARibbonCustomizeWidgetPrivate;
    friend class SARibbonPanelLayout;
    Q_PROPERTY(bool isCanCustomize READ isCanCustomize WRITE setCanCustomize)
    Q_PROPERTY(bool isExpanding READ isExpanding WRITE setExpanding)
    Q_PROPERTY(QString panelName READ panelName WRITE setPanelName)
public:
    /**
     * @brief Layout mode for the panel / 面板的布局模式
     */
    enum PanelLayoutMode
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
        TwoRowMode,

        /**
         * \if ENGLISH
         * @brief Single-row layout mode
         *
         * In this mode, all buttons are arranged in a single horizontal row.
         * Large, Medium and Small row proportions all behave as Small.
         * Panel titles are hidden by default.
         * \endif
         *
         * \if CHINESE
         * @brief 单行布局模式
         *
         * 在此模式下，所有按钮排列在单行水平行中。
         * Large、Medium 和 Small 行占比都等效为 Small。
         * Panel 标题默认隐藏。
         * \endif
         */
        SingleRowMode
    };
    Q_ENUM(PanelLayoutMode)
public:
    using FpRibbonToolButtonIterate = std::function< bool(SARibbonToolButton*) >;

public:
    /// Constructor for SARibbonPanel
    explicit SARibbonPanel(QWidget* parent = nullptr);
    /// Constructor for SARibbonPanel with name
    explicit SARibbonPanel(const QString& name, QWidget* parent = nullptr);
    /// Destructor for SARibbonPanel
    ~SARibbonPanel() Q_DECL_OVERRIDE;
    using QWidget::addAction;

    /// Add action to panel with specified row proportion
    void addAction(QAction* action, SARibbonPanelItem::RowProportion rowProportion);
    /// Generate and add an action with specified popup mode and row proportion
    void addAction(QAction* act,
                   QToolButton::ToolButtonPopupMode popMode,
                   SARibbonPanelItem::RowProportion rowProportion = SARibbonPanelItem::Large);
    /// Add action to panel with large icon display
    void addLargeAction(QAction* action);
    /// Add action to panel with medium icon display in three-row mode
    void addMediumAction(QAction* action);
    /// Add action to panel with small icon display
    void addSmallAction(QAction* action);

    /// Add action to panel with small icon display and specified popup mode
    void addSmallAction(QAction* action, QToolButton::ToolButtonPopupMode popMode);
    /// Add action to panel with large icon display and specified popup mode
    void addLargeAction(QAction* action, QToolButton::ToolButtonPopupMode popMode);
    /// Add action to panel with medium icon display in three-row mode and specified popup mode
    void addMediumAction(QAction* action, QToolButton::ToolButtonPopupMode popMode);
    /// Generate and add an action with text, icon, popup mode, and row proportion
    QAction* addAction(const QString& text,
                       const QIcon& icon,
                       QToolButton::ToolButtonPopupMode popMode,
                       SARibbonPanelItem::RowProportion rowProportion = SARibbonPanelItem::Large);

    /// Add menu with specified row proportion and popup mode
    void addMenu(QMenu* menu,
                 SARibbonPanelItem::RowProportion rowProportion,
                 QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);
    /// Add large menu with specified popup mode
    void addLargeMenu(QMenu* menu, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);
    /// Add medium menu with specified popup mode
    void addMediumMenu(QMenu* menu, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);
    /// Add small menu with specified popup mode
    void addSmallMenu(QMenu* menu, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);

    /// Add widget with specified row proportion
    QAction* addWidget(QWidget* w, SARibbonPanelItem::RowProportion rowProportion);

    /// Add small widget (occupies one row of ribbon)
    QAction* addSmallWidget(QWidget* w);

    /// Add medium widget (occupies one row of ribbon)
    QAction* addMediumWidget(QWidget* w);

    /// Add large widget (occupies all rows)
    QAction* addLargeWidget(QWidget* w);

    /// Add a gallery
    SARibbonGallery* addGallery(bool expanding = true);

    /// Add separator
    QAction* addSeparator();

    /// Extract the button corresponding to the action from the panel, returns nullptr if no corresponding button
    SARibbonToolButton* actionToRibbonToolButton(QAction* action);

    /// Set option action, pass nullptr to remove, SARibbonPanel does not manage QAction ownership
    /// OptionAction also triggers actionTriggered signal
    void setOptionAction(QAction* action);

    /// Check if option action exists
    bool isHaveOptionAction() const;

    /// Get all buttons
    QList< SARibbonToolButton* > ribbonToolButtons() const;

    /// Get PanelLayoutMode
    PanelLayoutMode panelLayoutMode() const;
    /// Set PanelLayoutMode
    void setPanelLayoutMode(PanelLayoutMode mode);

    /// Update button sizes, this function needs to be called after panel layout state changes
    void resetToolButtonSize();

    /// Check if it is two-row mode
    bool isTwoRow() const;

    /// Set panel to expanding mode, which will expand the horizontal area
    void setExpanding(bool isExpanding = true);
    /// Check if it is expanding mode
    bool isExpanding() const;

    /// Title bar height
    int titleHeight() const;

    /// Check if title is enabled, after enabling title, the title height needs to be set, default height is 15
    bool isEnableShowTitle() const;

    /// Layout index corresponding to action, this operation is generally used for moveAction, other meanings are not significant
    int actionIndex(QAction* act) const;

    /// Move action
    void moveAction(int from, int to);

    /// Check if customization is allowed
    bool isCanCustomize() const;
    /// Set customization allowed
    void setCanCustomize(bool b);

    /// Panel name
    QString panelName() const;
    /// Set panel name
    void setPanelName(const QString& title);

    /// Large button height
    int largeButtonHeight() const;

    /// Get layout corresponding items, this function is currently only used in the customization process
    const QList< SARibbonPanelItem* >& ribbonPanelItem() const;

    /// Get panel layout
    SARibbonPanelLayout* panelLayout() const;

    /// Update layout
    void updateItemGeometry();

    /// Get category pointer, returns nullptr if no parent or not managed by category
    SARibbonCategory* category() const;

    /// Get ribbonBar pointer, returns nullptr if none
    SARibbonBar* ribbonBar() const;

    /// Spacing between buttons
    int spacing() const;

    /// Set button icon sizes
    void setToolButtonIconSize(const QSize& smallSize, const QSize& largeSize);
    /// Get button icon sizes
    QPair< QSize, QSize > toolButtonIconSize() const;

    /// Large button icon size
    void setLargeIconSize(const QSize& largeSize);
    /// Get large icon size
    QSize largeIconSize() const;

    /// Small button icon size
    void setSmallIconSize(const QSize& smallSize);
    /// Get small icon size
    QSize smallIconSize() const;

    /// Get last added button
    SARibbonToolButton* lastAddActionButton();

    /// Get panel title label widget
    SARibbonPanelLabel* titleLabel() const;

    /// Check if word wrap is enabled
    bool isEnableWordWrap() const;

    /// Set whether button text is displayed to the right of the icon
    void setEnableIconRightText(bool on);
    /// Check if icon-right-text mode is enabled
    bool isEnableIconRightText() const;

    /// Maximum aspect ratio of buttons, this coefficient determines the maximum width of buttons
    qreal buttonMaximumAspectRatio() const;

    /// This function will iterate through all RibbonToolButton under SARibbonPanel, execute function pointer
    /// (bool(SARibbonRibbonToolButton*)), function pointer returns false to stop iteration
    bool iterateButton(FpRibbonToolButtonIterate fp) const;

public:
    /// Get size hint
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;
    /// Get minimum size hint
    virtual QSize minimumSizeHint() const Q_DECL_OVERRIDE;

public:
    /// Recommended panel height
    static int panelHeightHint(const QFontMetrics& fm, PanelLayoutMode layMode, int panelTitleHeight);
    /// Set action row proportion property into action, action itself carries row property
    static void setActionRowProportionProperty(QAction* action, SARibbonPanelItem::RowProportion rp);
    /// Get action row proportion property
    static SARibbonPanelItem::RowProportion getActionRowProportionProperty(QAction* action);
    /// Set action PopupMode property into action, action itself carries PopupMode property
    static void setActionToolButtonPopupModeProperty(QAction* action, QToolButton::ToolButtonPopupMode popMode);
    /// Get action PopupMode property
    static QToolButton::ToolButtonPopupMode getActionToolButtonPopupModeProperty(QAction* action);
    /// Set action ToolButtonStyle property into action, action itself carries ToolButtonStyle property
    static void setActionToolButtonStyleProperty(QAction* action, Qt::ToolButtonStyle buttonStyle);
    /// Get action ToolButtonStyle property
    static Qt::ToolButtonStyle getActionToolButtonStyleProperty(QAction* action);

Q_SIGNALS:

    /**
     * \if ENGLISH
     * @brief Equivalent to QToolBar::actionTriggered
     * @param action
     * \endif
     *
     * \if CHINESE
     * @brief 等同于QToolBar::actionTriggered
     * @param action
     * \endif
     */
    void actionTriggered(QAction* action);

    /**
     * \if ENGLISH
     * @brief Emitted when the panel's name changes
     * @param n The new name
     * \endif
     *
     * \if CHINESE
     * @brief 当面板名称改变时发出
     * @param n 新的名称
     * \endif
     */
    void panelNameChanged(const QString& n);

protected:
    /// Handle action events
    virtual void actionEvent(QActionEvent* e) Q_DECL_OVERRIDE;
    /// Handle change events
    virtual void changeEvent(QEvent* e) Q_DECL_OVERRIDE;

protected:
    // The following methods are for receiving parameters from the corresponding interfaces of SARibbonBar, users do not
    // need to set them when operating SARibbonPanel When SARibbonPanel is set to SARibbonCategory, SARibbonCategory
    // will resynchronize its own parameters to the panel Similarly, when SARibbonCategory is added to SARibbonBar,
    // SARibbonBar will resynchronize its own parameters to the category

    /// Set title bar height
    void setTitleHeight(int h);

    /// Set spacing between buttons
    void setSpacing(int n);

    /// Set whether to show title, after showing title, the title height needs to be set, default height is 15
    void setEnableShowTitle(bool on);

    /// Set whether buttons are allowed to wrap, note that the icon size is determined by the text, two lines of text
    /// will make the icon smaller, if you want the icon to be larger, it is best not to wrap the text
    void setEnableWordWrap(bool on);

    /// Set the maximum aspect ratio of buttons, this coefficient determines the maximum width of buttons
    void setButtonMaximumAspectRatio(qreal fac = 1.4);
};

#endif  // SARIBBONPANEL_H
