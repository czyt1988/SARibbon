#ifndef SARIBBONBAR_H
#define SARIBBONBAR_H
#include "SARibbonCategory.h"
#include "SARibbonContextCategory.h"
#include "SARibbonGlobal.h"
#include <QMenuBar>
#include <QScopedPointer>
#include <QVariant>

class QAction;
class QAbstractButton;
class SARibbonElementFactory;
class SARibbonTabBar;
class SARibbonButtonGroupWidget;
class SARibbonQuickAccessBar;
class SARibbonStackedWidget;
class SARibbonTitleIconWidget;

/**
 * \if ENGLISH
 * @brief SARibbonBar inherits from QMenuBar and directly replaces the original QMenuBar in SARibbonMainWindow
 *
 * Set the ribbon style using the setRibbonStyle function:
 *
 * @code
 * void setRibbonStyle(RibbonStyles v);
 * @endcode
 *
 * SARibbonBar references Office and WPS, providing four styles of Ribbon modes, @ref SARibbonBar::RibbonStyles
 *
 * If you want the ribbon to occupy minimal space, WpsLiteStyleTwoRow mode can save 35% height space compared to OfficeStyle
 *
 * How to create a ribbon? First, let's see how a traditional Menu/ToolBar is created:
 *
 * @code
 * void MainWindow::MainWindow()
 * {
 *  QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
 *  QToolBar *fileToolBar = addToolBar(tr("File"));
 *  // Create action
 *  QAction *newAct = new QAction(newIcon, tr("&New"), this);
 *  fileMenu->addAction(newAct);
 *  fileToolBar->addAction(newAct);
 *
 *  QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
 *  fileMenu->addAction(openAct);
 *  fileToolBar->addAction(openAct);
 * }
 * @endcode
 *
 * Traditional Menu/ToolBar mainly uses QMenu's addMenu to add menus, and QMainWindow::addToolBar to generate QToolBar,
 * then sets QAction into QMenu and QToolBar.
 *
 * SARibbonBar is similar to the traditional method, but compared to traditional Menu/ToolBar where QMenu and QToolBar are at the same level,
 * Ribbon has a clear hierarchical relationship: SARibbonBar is above @ref SARibbonCategory,
 * SARibbonCategory is above @ref SARibbonPanel, SARibbonPanel is above @ref SARibbonToolButton,
 * and SARibbonToolButton manages QAction.
 *
 * Therefore, creating a ribbon only requires the following functions:
 * @code
 * SARibbonCategory * SARibbonBar::addCategoryPage(const QString& title);
 * SARibbonPanel * SARibbonCategory::addPanel(const QString& title);
 * SARibbonToolButton * SARibbonPanel::addLargeAction(QAction *action);
 * SARibbonToolButton * SARibbonPanel::addSmallAction(QAction *action);
 * @endcode
 *
 * Therefore, the creation steps are as follows:
 *
 * @code
 * // Member variables
 * SARibbonCategory* categoryMain;
 * SARibbonPanel* FilePanel;
 *
 * // Build UI
 * void setupRibbonUi()
 * {
 *   ......
 *   // ribbonwindow is SARibbonMainWindow
 *   SARibbonBar* ribbon = ribbonwindow->ribbonBar();
 *   ribbon->setRibbonStyle(SARibbonBar::WpsLiteStyle);
 *   // Add a Main tab
 *   categoryMain = ribbon->addCategoryPage(QStringLiteral("Main"));
 *   // Add a File Panel under the Main tab
 *   FilePanel = categoryMain->addPanel(QStringLiteral("FilePanel"));
 *   // Start adding actions to File Panel
 *   FilePanel->addLargeAction(actionNew);
 *   FilePanel->addLargeAction(actionOpen);
 *   FilePanel->addLargeAction(actionSave);
 *   FilePanel->addSmallAction(actionImportMesh);
 *   FilePanel->addSmallAction(actionImportGeometry);
 * }
 * @endcode
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonBar继承于QMenuBar,在SARibbonMainWindow中直接替换了原来的QMenuBar
 *
 * 通过setRibbonStyle函数设置ribbon的风格:
 *
 * @code
 * void setRibbonStyle(RibbonStyles v);
 * @endcode
 *
 * SARibbonBar参考office和wps，提供了四种风格的Ribbon模式,@ref SARibbonBar::RibbonStyles
 *
 * 如果想ribbon占用的空间足够小，WpsLiteStyleTwoRow模式能比OfficeStyle节省35%的高度空间
 *
 * 如何生成ribbon?先看看一个传统的Menu/ToolBar是如何生成的：
 *
 * @code
 * void MainWindow::MainWindow()
 * {
 *  QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
 *  QToolBar *fileToolBar = addToolBar(tr("File"));
 *  //生成action
 *  QAction *newAct = new QAction(newIcon, tr("&New"), this);
 *  fileMenu->addAction(newAct);
 *  fileToolBar->addAction(newAct);
 *
 *  QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
 *  fileMenu->addAction(openAct);
 *  fileToolBar->addAction(openAct);
 * }
 * @endcode
 *
 * 传统的Menu/ToolBar主要通过QMenu的addMenu添加菜单,通过QMainWindow::addToolBar生成QToolBar,
 * 再把QAction设置进QMenu和QToolBar中
 *
 * SARibbonBar和传统方法相似，不过相对于传统的Menu/ToolBar QMenu和QToolBar是平级的，
 * Ribbon是有明显的层级关系，SARibbonBar下面是 @ref SARibbonCategory，
 * SARibbonCategory下面是@ref SARibbonPanel ，SARibbonPanel下面是@ref SARibbonToolButton ，
 * SARibbonToolButton管理着QAction
 *
 * 因此，生成一个ribbon只需以下几个函数：
 * @code
 * SARibbonCategory * SARibbonBar::addCategoryPage(const QString& title);
 * SARibbonPanel * SARibbonCategory::addPanel(const QString& title);
 * SARibbonToolButton * SARibbonPanel::addLargeAction(QAction *action);
 * SARibbonToolButton * SARibbonPanel::addSmallAction(QAction *action);
 * @endcode
 *
 * 因此生成步骤如下：
 *
 * @code
 * //成员变量
 * SARibbonCategory* categoryMain;
 * SARibbonPanel* FilePanel;
 *
 * //建立ui
 * void setupRibbonUi()
 * {
 *   ......
 *   //ribbonwindow为SARibbonMainWindow
 *   SARibbonBar* ribbon = ribbonwindow->ribbonBar();
 *   ribbon->setRibbonStyle(SARibbonBar::WpsLiteStyle);
 *   //添加一个Main标签
 *   categoryMain = ribbon->addCategoryPage(QStringLiteral("Main"));
 *   //Main标签下添加一个File Panel
 *   FilePanel = categoryMain->addPanel(QStringLiteral("FilePanel"));
 *   //开始为File Panel添加action
 *   FilePanel->addLargeAction(actionNew);
 *   FilePanel->addLargeAction(actionOpen);
 *   FilePanel->addLargeAction(actionSave);
 *   FilePanel->addSmallAction(actionImportMesh);
 *   FilePanel->addSmallAction(actionImportGeometry);
 * }
 * @endcode
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonBar : public QMenuBar
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonBar)
    friend class SARibbonMainWindow;
    friend class SARibbonSystemButtonBar;
    Q_PROPERTY(RibbonStyles ribbonStyle READ currentRibbonStyle WRITE setRibbonStyle)
    Q_PROPERTY(bool minimumMode READ isMinimumMode WRITE setMinimumMode)
    Q_PROPERTY(bool minimumModeButton READ haveShowMinimumModeButton WRITE showMinimumModeButton)
    Q_PROPERTY(QColor windowTitleTextColor READ windowTitleTextColor WRITE setWindowTitleTextColor)
    Q_PROPERTY(QColor tabBarBaseLineColor READ tabBarBaseLineColor WRITE setTabBarBaseLineColor)
    Q_PROPERTY(Qt::Alignment windowTitleAligment READ windowTitleAligment WRITE setWindowTitleAligment)
    Q_PROPERTY(bool enableWordWrap READ isEnableWordWrap WRITE setEnableWordWrap)
    Q_PROPERTY(bool enableShowPanelTitle READ isEnableShowPanelTitle WRITE setEnableShowPanelTitle)
    Q_PROPERTY(bool tabOnTitle READ isTabOnTitle WRITE setTabOnTitle)
    Q_PROPERTY(SARibbonPanel::PanelLayoutMode panelLayoutMode READ panelLayoutMode WRITE setPanelLayoutMode)

public:
    enum RibbonStyleFlag
    {
        RibbonStyleLoose    = 0x0001,  // bit:0000 0001
        RibbonStyleCompact  = 0x0002,  // bit:0000 0010
        RibbonStyleThreeRow = 0x0010,  // bit:0001 0000
        RibbonStyleTwoRow   = 0x0020,  // bit:0010 0000

        RibbonStyleLooseThreeRow   = RibbonStyleLoose | RibbonStyleThreeRow,    ///< 宽松结构，3行模式
        RibbonStyleCompactThreeRow = RibbonStyleCompact | RibbonStyleThreeRow,  ///< 紧凑结构，3行模式
        RibbonStyleLooseTwoRow     = RibbonStyleLoose | RibbonStyleTwoRow,      ///< 宽松结构，2行模式
        RibbonStyleCompactTwoRow   = RibbonStyleCompact | RibbonStyleTwoRow     ///< 紧凑结构，2行模式
    };
    Q_ENUM(RibbonStyleFlag)
    Q_DECLARE_FLAGS(RibbonStyles, RibbonStyleFlag)
    Q_FLAG(RibbonStyles)

    /**
     * @brief 定义当前ribbon 的状态
     */
    enum RibbonMode
    {
        MinimumRibbonMode,  ///< 缩小模式
        NormalRibbonMode    ///< 正常模式
    };
    Q_ENUM(RibbonMode)
public:
    using FpCategoryIterate = std::function< bool(SARibbonCategory*) >;
    using FpPanelIterate    = SARibbonCategory::FpPanelIterate;
    /**
     * @brief 这是针对上下文标签的高亮颜色绘制，用户可以设置一个函数指针，来针对上下文标签的高亮颜色进行调整
     */
    using FpContextCategoryHighlight = std::function< QColor(const QColor&) >;

public:
    /// Check if the ribbon style is two-row mode
    static bool isTwoRowStyle(RibbonStyles s);
    /// Check if the ribbon style is three-row mode
    static bool isThreeRowStyle(RibbonStyles s);
    /// Check if the ribbon style is loose (Office) style
    static bool isLooseStyle(RibbonStyles s);
    /// Check if the ribbon style is compact (WPS) style
    static bool isCompactStyle(RibbonStyles s);
    /// Get version information
    static QString versionString();

    /// Get default context category color list
    static QList< QColor > defaultContextCategoryColorList();

    /// Initialize high DPI support
    static void initHighDpi();

public:
    /// Constructor
    explicit SARibbonBar(QWidget* parent = nullptr);
    /// Destructor
    ~SARibbonBar();
    /// Get application button
    QAbstractButton* applicationButton();

    /// Set application button
    void setApplicationButton(QAbstractButton* btn);

    /// Get ribbon tab bar
    SARibbonTabBar* ribbonTabBar();

    /// Add a category page
    SARibbonCategory* addCategoryPage(const QString& title);
    /// Add a category page
    void addCategoryPage(SARibbonCategory* category);

    /// Overloaded function for Qt designer support
    Q_INVOKABLE void addCategoryPage(QWidget* category);

    /// Insert a category page at specified index
    SARibbonCategory* insertCategoryPage(const QString& title, int index);
    /// Insert a category page at specified index
    void insertCategoryPage(SARibbonCategory* category, int index);

    /// Find category by name
    SARibbonCategory* categoryByName(const QString& title) const;

    /// Find category by object name
    SARibbonCategory* categoryByObjectName(const QString& objname) const;

    /// Find category by index
    SARibbonCategory* categoryByIndex(int index) const;

    /// Hide category (not delete)
    void hideCategory(SARibbonCategory* category);

    /// Show hidden category
    void showCategory(SARibbonCategory* category);

    // Batch set categories visible or hidden, only triggers one relayout at the end
    void setCategoriesVisible(const QList< SARibbonCategory* >& categories, bool visible);

    /// Check if category is visible
    bool isCategoryVisible(const SARibbonCategory* c) const;

    /// Get category index
    int categoryIndex(const SARibbonCategory* c) const;

    /// Move category from one index to another
    void moveCategory(int from, int to);

    /// Get all category pages
    QList< SARibbonCategory* > categoryPages(bool getAll = true) const;

    /// Remove category
    void removeCategory(SARibbonCategory* category);

    /// Add a context category
    SARibbonContextCategory*
    addContextCategory(const QString& title, const QColor& color = QColor(), const QVariant& id = QVariant());
    /// Add a context category
    void addContextCategory(SARibbonContextCategory* context);

    /// Show context category
    void showContextCategory(SARibbonContextCategory* context);

    /// Hide context category
    void hideContextCategory(SARibbonContextCategory* context);

    /// Check if context category is visible
    bool isContextCategoryVisible(SARibbonContextCategory* context);

    /// Set context category visibility
    void setContextCategoryVisible(SARibbonContextCategory* context, bool visible);

    /// Get all context categories
    QList< SARibbonContextCategory* > contextCategoryList() const;

    /// Destroy context category
    void destroyContextCategory(SARibbonContextCategory* context);

    /// Get current visible context category tab indexes
    QList< int > currentVisibleContextCategoryTabIndexs() const;

    /// Set minimum mode
    void setMinimumMode(bool isHide);

    /// Check if in minimum mode
    bool isMinimumMode() const;

    /// Show minimum mode button
    void showMinimumModeButton(bool isShow = true);

    /// Check if minimum mode button is shown
    bool haveShowMinimumModeButton() const;

    /// Get minimum mode action
    QAction* minimumModeAction() const;

    /// Check if tab double click enables minimum mode
    bool isEnableTabDoubleClickToMinimumMode() const;
    /// Set tab double click to enable minimum mode
    void setTabDoubleClickToMinimumMode(bool on = true) const;

    /// Get current ribbon state
    RibbonMode currentRibbonState() const;

    /// Get tab bar height
    int tabBarHeight() const;
    /// Set tab bar height
    void setTabBarHeight(int h, bool resizeByNow = true);

    /// Get title bar height
    int titleBarHeight() const;
    /// Set title bar height
    void setTitleBarHeight(int h, bool resizeByNow = true);

    /// Get category height
    int categoryHeight() const;
    /// Set category height
    void setCategoryHeight(int h, bool resizeByNow = true);

    /// Get main bar height in normal mode
    int normalModeMainBarHeight() const;

    /// Get main bar height in minimum mode
    int minimumModeMainBarHeight() const;

    /// Get active right button group
    SARibbonButtonGroupWidget* activeRightButtonGroup();

    /// Get right button group
    SARibbonButtonGroupWidget* rightButtonGroup();

    /// Get active quick access bar
    SARibbonQuickAccessBar* activeQuickAccessBar();

    /// Get quick access bar
    SARibbonQuickAccessBar* quickAccessBar();

    /// Get title icon widget
    SARibbonTitleIconWidget* titleIconWidget() const;

    /// Set title icon visibility
    void setTitleIconVisible(bool on = true);
    /// Check if title icon is visible
    bool isTitleIconVisible() const;

    /// Set ribbon style
    void setRibbonStyle(RibbonStyles v);
    /// Get current ribbon style
    RibbonStyles currentRibbonStyle() const;

    /// Set current ribbon index
    void setCurrentIndex(int index);

    /// Get current tab index
    int currentIndex();

    /// Raise category to show it
    void raiseCategory(SARibbonCategory* category);

    /// Check if current style is two-row
    bool isTwoRowStyle() const;
    /// Check if current style is three-row
    bool isThreeRowStyle() const;

    /// Check if current style is loose
    bool isLooseStyle() const;
    /// Check if current style is compact
    bool isCompactStyle() const;

    /// Update ribbon geometry
    void updateRibbonGeometry();

    /// Get panel layout mode
    SARibbonPanel::PanelLayoutMode panelLayoutMode() const;
    /// Set panel layout mode
    void setPanelLayoutMode(SARibbonPanel::PanelLayoutMode m);

    /// Set tab on title
    void setTabOnTitle(bool on);
    /// Check if tab is on title
    bool isTabOnTitle() const;

    /// Set tab bar baseline color
    void setTabBarBaseLineColor(const QColor& clr);
    /// Get tab bar baseline color
    QColor tabBarBaseLineColor() const;

    /// Set window title text color
    void setWindowTitleTextColor(const QColor& clr);
    /// Get window title text color
    QColor windowTitleTextColor() const;

    /// Set title visibility
    void setTitleVisible(bool on = false);
    /// Check if title is visible
    bool isTitleVisible() const;

    /// Set window title background brush
    void setWindowTitleBackgroundBrush(const QBrush& bk);
    /// Get window title background brush
    QBrush windowTitleBackgroundBrush() const;

    /// Set window title alignment
    void setWindowTitleAligment(Qt::Alignment al);
    /// Get window title alignment
    Qt::Alignment windowTitleAligment() const;

    /// Set enable word wrap
    void setEnableWordWrap(bool on);
    /// Check if word wrap is enabled
    bool isEnableWordWrap() const;

    /// Set button maximum aspect ratio
    void setButtonMaximumAspectRatio(qreal fac = 1.4);
    /// Get button maximum aspect ratio
    qreal buttonMaximumAspectRatio() const;

    /// Set panel title height
    void setPanelTitleHeight(int h);
    /// Get panel title height
    int panelTitleHeight() const;

    /// Set enable show panel title
    void setEnableShowPanelTitle(bool on);
    /// Check if panel title is shown
    bool isEnableShowPanelTitle() const;

    /// Set panel spacing
    void setPanelSpacing(int n);
    /// Get panel spacing
    int panelSpacing() const;

    /// Set panel tool button icon sizes
    void setPanelToolButtonIconSize(const QSize& smallSize, const QSize& largeSize);
    /// Get panel tool button icon sizes
    QPair< QSize, QSize > panelToolButtonIconSize() const;

    /// Set panel large icon size
    void setPanelLargeIconSize(const QSize& largeSize);
    /// Get panel large icon size
    QSize panelLargeIconSize() const;

    /// Set panel small icon size
    void setPanelSmallIconSize(const QSize& smallSize);
    /// Get panel small icon size
    QSize panelSmallIconSize() const;

    /// Get ribbon stacked widget
    SARibbonStackedWidget* ribbonStackedWidget();

    /// Set context category color list
    void setContextCategoryColorList(const QList< QColor >& cls);
    /// Get context category color list
    QList< QColor > contextCategoryColorList() const;

    /// Set context category title text color
    void setContextCategoryTitleTextColor(const QColor& clr);
    /// Get context category title text color
    QColor contextCategoryTitleTextColor() const;

    /// Set context category color highlight function
    void setContextCategoryColorHighLight(FpContextCategoryHighlight fp);

    /// Set ribbon alignment
    void setRibbonAlignment(SARibbonAlignment al);
    /// Get ribbon alignment
    SARibbonAlignment ribbonAlignment() const;

    /// Iterate through all categories
    bool iterateCategory(FpCategoryIterate fp) const;
    /// Iterate through all panels
    bool iteratePanel(FpPanelIterate fp) const;

    /// Set corner widget visibility
    void setCornerWidgetVisible(bool on, Qt::Corner c = Qt::TopLeftCorner);

    /// Set application button vertical expansion
    void setApplicationButtonVerticalExpansion(bool on = true);
    /// Check if application button is vertically expanded
    bool isApplicationButtonVerticalExpansion() const;

    /// Get all actions in panels
    QList< QAction* > allActions() const;
    /// Check if ribbon frame is used
    bool isUseRibbonFrame() const;
Q_SIGNALS:

    /**
     * \if ENGLISH
     * @brief Application button click response - the button in the top-left corner, connect to this signal to trigger application button click effect
     *
     * For example, clicking the button can pop up a full-screen window (like in Office)
     * \endif
     *
     * \if CHINESE
     * @brief 应用按钮点击响应 - 左上角的按钮，通过关联此信号触发应用按钮点击的效果
     *
     * 例如想点击按钮后弹出一个全屏的窗口（如office这些）
     * \endif
     */
    void applicationButtonClicked();

    /**
     * \if ENGLISH
     * @brief Signal triggered when tab page changes
     * @param index New tab index
     * \endif
     *
     * \if CHINESE
     * @brief 标签页变化触发的信号
     * @param index 新的标签页索引
     * \endif
     */
    void currentRibbonTabChanged(int index);

    /**
     * \if ENGLISH
     * @brief Signal triggered when ribbon state changes
     * @param nowState New ribbon state after change
     * \endif
     *
     * \if CHINESE
     * @brief ribbon的状态发生了变化后触发此信号
     * @param nowState 变更之后的ribbon状态
     * \endif
     */
    void ribbonModeChanged(SARibbonBar::RibbonMode nowState);

    /**
     * \if ENGLISH
     * @brief Signal triggered when ribbon style changes
     * @param nowStyle New ribbon style after change
     * \endif
     *
     * \if CHINESE
     * @brief ribbon的状态发生了变化后触发此信号
     * @param nowStyle 变更之后的ribbon样式
     * \endif
     */
    void ribbonStyleChanged(SARibbonBar::RibbonStyles nowStyle);

    /**
     * \if ENGLISH
     * @brief Signal triggered when title bar height changes
     * @param oldHeight Old title bar height
     * @param newHeight New title bar height
     * \endif
     *
     * \if CHINESE
     * @brief 标题栏高度发生了变化的信号
     * @param oldHeight 旧的标题栏高度
     * @param newHeight 新的标题栏高度
     * \endif
     */
    void titleBarHeightChanged(int oldHeight, int newHeight);

    /**
     * \if ENGLISH
     * @brief Signal similar to QToolBar::actionTriggered
     * @param action Triggered action
     * \endif
     *
     * \if CHINESE
     * @brief 参考QToolBar::actionTriggered的信号
     * @param action 触发的action
     * \endif
     */
    void actionTriggered(QAction* action);

protected:
    /// Event filter
    bool eventFilter(QObject* obj, QEvent* e) override;
    /// Calculate minimum tab bar width
    int calcMinTabBarWidth() const;
    /// Update category title to tab name
    void updateCategoryTitleToTabName();
    /// Set system button group size
    void setSystemButtonGroupSize(const QSize& s);
    /// Set main window styles
    void setMainWindowStyles(SARibbonMainWindowStyles s);
protected Q_SLOTS:
    /// Slot for window title changed
    void onWindowTitleChanged(const QString& title);
    /// Slot for window icon changed
    void onWindowIconChanged(const QIcon& i);
    /// Slot for category window title changed
    void onCategoryWindowTitleChanged(const QString& title);
    /// Slot for stack widget hided
    void onStackWidgetHided();
    /// Slot for current ribbon tab changed
    virtual void onCurrentRibbonTabChanged(int index);
    /// Slot for current ribbon tab clicked
    virtual void onCurrentRibbonTabClicked(int index);
    /// Slot for current ribbon tab double clicked
    virtual void onCurrentRibbonTabDoubleClicked(int index);
    /// Slot for contexts category page added
    void onContextsCategoryPageAdded(SARibbonCategory* category);
    /// Slot for contexts category name changed
    void onContextsCategoryCategoryNameChanged(SARibbonCategory* category, const QString& title);
    /// Slot for tab moved
    void onTabMoved(int from, int to);

private:
    int tabIndex(SARibbonCategory* obj);
    void paintInLooseStyle();
    void paintInCompactStyle();

    // 刷新所有ContextCategoryManagerData，这个在单独一个Category删除时调用
    void updateContextCategoryManagerData();
    void synchronousCategoryData(bool autoUpdate = true);
    // 把窗口标题文字转换为显示的文字，这里主要针对[*]占位符进行替换
    QString toDisplayTitleText(const QString& title) const;
    // 获取标题显示的区域
    QRect getWindowTitleRect() const;

protected:
    /// Set ribbon main window style
    void setRibbonMainwindowStyle();
    /// Paint event handler
    virtual void paintEvent(QPaintEvent* e) override;
    /// Move event handler
    virtual void moveEvent(QMoveEvent* e) override;
    /// Change event handler
    virtual void changeEvent(QEvent* e) override;

    /// Paint tab bar baseline
    virtual void paintTabbarBaseLine(QPainter& painter);
    /// Paint window title
    virtual void paintWindowTitle(QPainter& painter, const QString& title, const QRect& titleRegion);
    /// Paint context category tab
    virtual void
    paintContextCategoryTab(QPainter& painter, const QString& title, const QRect& contextRect, const QColor& color);
#if SA_DEBUG_PRINT_SARIBBONBAR
    SA_RIBBON_EXPORT friend QDebug operator<<(QDebug debug, const SARibbonBar& ribbon);
#endif
};
Q_DECLARE_OPERATORS_FOR_FLAGS(SARibbonBar::RibbonStyles)

#endif  // SARIBBONBAR_H
