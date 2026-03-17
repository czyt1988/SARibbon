#ifndef SARIBBONPANELLAYOUT_H
#define SARIBBONPANELLAYOUT_H
#include "SARibbonGlobal.h"
#include <QLayout>
#include "SARibbonPanelItem.h"
class QToolButton;
class SARibbonPanel;
class SARibbonPanelLabel;

/**
 * \if ENGLISH
 * @brief Layout manager for SARibbonPanel
 * @details `SARibbonPanelLayout` is a custom `QLayout` subclass responsible for arranging `SARibbonToolButton`
 * @details and other widgets within a `SARibbonPanel`. It handles the complex Ribbon-specific layout logic,
 * @details including Large, Medium, and Small button proportions, and the optional title and option button.
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonPanel的布局管理器
 * @details `SARibbonPanelLayout` 是一个自定义的 `QLayout` 子类，负责在 `SARibbonPanel` 内排列 `SARibbonToolButton`
 * @details 和其他窗口部件。它处理复杂的Ribbon特定布局逻辑，包括大、中、小按钮比例，以及可选的标题和选项按钮。
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonPanelLayout : public QLayout
{
    Q_OBJECT
    friend class SARibbonPanel;

public:
    /// Constructor for SARibbonPanelLayout
    explicit SARibbonPanelLayout(QWidget* p = nullptr);
    /// Destructor for SARibbonPanelLayout
    ~SARibbonPanelLayout();
    /// Gets the SARibbonPanel that owns this layout
    SARibbonPanel* ribbonPanel() const;

    /// Inserts an action at a specific index
    void insertAction(int index, QAction* act, SARibbonPanelItem::RowProportion rp = SARibbonPanelItem::None);

    /// Sets the option action for the panel
    void setOptionAction(QAction* action);

    /// Checks if an option action is set
    bool isHaveOptionAction() const;

    /// Retrieves the SARibbonPanelItem associated with an action
    SARibbonPanelItem* panelItem(QAction* action) const;

    /// Gets the last item added to the layout
    SARibbonPanelItem* lastItem() const;

    /// Gets the widget associated with the last item
    QWidget* lastWidget() const;

    /// Moves an item from one index to another
    void move(int from, int to);
    /// Checks if layout needs to be reorganized
    bool isDirty() const;
    /// Updates size
    void updateGeomArray();

    /// Finds the index of an action in the layout
    int indexByAction(QAction* action) const;

    /// Gets the height of the panel's title
    int panelTitleHeight() const;

    /// Sets the height of the panel's title
    void setPanelTitleHeight(int newTitleHeight);

    /// Checks if the panel's title is enabled for display
    bool isEnableShowPanelTitle() const;

    /// Sets whether the panel's title is enabled for display
    void setEnableShowPanelTitle(bool on);

    /// Gets the height of large buttons
    int largeButtonHeight() const;

    /// Gets the spacing between the title and the buttons
    int panelTitleSpace() const;

    /// Sets the spacing between the title and the buttons
    void setPanelTitleSpace(int newTitleSpace);

    /// Gets the panel's title label
    SARibbonPanelLabel* panelTitleLabel() const;

    /// Sets the default icon size for tool buttons
    void setToolButtonIconSize(const QSize& smallSize, const QSize& largeSize);
    /// Gets the default icon size for tool buttons
    QPair< QSize, QSize > toolButtonIconSize() const;

    /// Large button icon size
    void setLargeIconSize(const QSize& largeSize);
    /// Get large icon size
    QSize largeIconSize() const;

    /// Small button icon size
    void setSmallIconSize(const QSize& largeSize);
    /// Get small icon size
    QSize smallIconSize() const;

    /// Check if word wrap is enabled
    bool isEnableWordWrap() const;
    /// Maximum aspect ratio of buttons, this coefficient determines the maximum width of buttons
    qreal buttonMaximumAspectRatio() const;

public:
    /// Adds an item to the layout (SARibbonPanelLayout not supported)
    void addItem(QLayoutItem* item) Q_DECL_OVERRIDE;

    /// QLayout required override functions
    QLayoutItem* itemAt(int index) const Q_DECL_OVERRIDE;

    /// Removes and returns the item at the specified index
    QLayoutItem* takeAt(int index) Q_DECL_OVERRIDE;

    /// Gets the number of items in the layout
    int count() const Q_DECL_OVERRIDE;

    /// Checks if the layout is empty
    bool isEmpty() const Q_DECL_OVERRIDE;

    /// Invalidates the layout, marking it as dirty
    void invalidate() Q_DECL_OVERRIDE;

    /// Returns the directions in which the layout can expand
    Qt::Orientations expandingDirections() const Q_DECL_OVERRIDE;
    /// Set geometry
    void setGeometry(const QRect& rect) Q_DECL_OVERRIDE;
    /// Get minimum size
    QSize minimumSize() const Q_DECL_OVERRIDE;
    /// Get size hint
    QSize sizeHint() const Q_DECL_OVERRIDE;

protected:
    /// Gets the size of the option action button
    QSize optionActionButtonSize() const;
    /// Layout action
    void doLayout();
    /// Convert action to item, for pure Action, this function will create SARibbonToolButton
    /// rp is used to tell Layout what kind of window to generate, see SARibbonPanelItem::RowProportion for details
    SARibbonPanelItem* createItem(QAction* action, SARibbonPanelItem::RowProportion rp = SARibbonPanelItem::None);
    /// Update geometry array
    void updateGeomArray(const QRect& setrect);
    /// Recalculate expansion bar code, this function must be called after updateGeomArray function
    void recalcExpandGeomArray(const QRect& setrect);
    /// Set text wrap enabled
    void setEnableWordWrap(bool on);
    /// Set maximum aspect ratio of buttons, this coefficient determines the maximum width of buttons
    void setButtonMaximumAspectRatio(qreal fac = 1.4);

private:
    /// Calculate window width and maximum width based on column count
    void columnWidthInfo(int colindex, int& width, int& maximum) const;

    /// Sets the panel's title label
    void setPanelTitleLabel(SARibbonPanelLabel* newTitleLabel);

private:
    QList< SARibbonPanelItem* > mItems;
    int mColumnCount { 0 };                       ///< 记录有多少列
    bool mExpandFlag { false };                   ///< 标记是否是会扩展的
    QSize mSizeHint;                              ///< sizeHint返回的尺寸
    QSize mSmallToolButtonIconSize { 22, 22 };    ///< 记录小按钮图标尺寸
    QSize mLargeToolButtonIconSize { 32, 32 };    ///< 记录大按钮图标尺寸
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
