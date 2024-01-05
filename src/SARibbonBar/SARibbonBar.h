#ifndef SARIBBONBAR_H
#define SARIBBONBAR_H
#include "SARibbonCategory.h"
#include "SARibbonContextCategory.h"
#include "SARibbonGlobal.h"
#include <QMenuBar>
#include <QScopedPointer>
#include <QVariant>

class QAbstractButton;
class SARibbonElementFactory;
class SARibbonTabBar;
class SARibbonButtonGroupWidget;
class SARibbonQuickAccessBar;
class SARibbonStackedWidget;

/**
  @brief SARibbonBar继承于QMenuBar,在SARibbonMainWindow中直接替换了原来的QMenuBar

  通过setRibbonStyle函数设置ribbon的风格:

  @code
  void setRibbonStyle(RibbonStyles v);
  @endcode

  SARibbonBar参考office和wps，提供了四种风格的Ribbon模式,@ref SARibbonBar::RibbonStyles

  如果想ribbon占用的空间足够小，WpsLiteStyleTwoRow模式能比OfficeStyle节省35%的高度空间

  如何生成ribbon?先看看一个传统的Menu/ToolBar是如何生成的：

  @code
  void MainWindow::MainWindow()
  {
     QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
     QToolBar *fileToolBar = addToolBar(tr("File"));
     //生成action
     QAction *newAct = new QAction(newIcon, tr("&New"), this);
     fileMenu->addAction(newAct);
     fileToolBar->addAction(newAct);

     QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
     fileMenu->addAction(openAct);
     fileToolBar->addAction(openAct);
  }
  @endcode

  传统的Menu/ToolBar主要通过QMenu的addMenu添加菜单,通过QMainWindow::addToolBar生成QToolBar,
  再把QAction设置进QMenu和QToolBar中

  SARibbonBar和传统方法相似，不过相对于传统的Menu/ToolBar QMenu和QToolBar是平级的，
  Ribbon是有明显的层级关系，SARibbonBar下面是 @ref SARibbonCategory，
  SARibbonCategory下面是@ref SARibbonPannel ，SARibbonPannel下面是@ref SARibbonToolButton ，
  SARibbonToolButton管理着QAction

  因此，生成一个ribbon只需以下几个函数：
  @code
  SARibbonCategory * SARibbonBar::addCategoryPage(const QString& title);
  SARibbonPannel * SARibbonCategory::addPannel(const QString& title);
  SARibbonToolButton * SARibbonPannel::addLargeAction(QAction *action);
  SARibbonToolButton * SARibbonPannel::addSmallAction(QAction *action);
  @endcode

  因此生成步骤如下：

  @code
  //成员变量
  SARibbonCategory* categoryMain;
  SARibbonPannel* FilePannel;

  //建立ui
  void setupRibbonUi()
  {
      ......
      //ribbonwindow为SARibbonMainWindow
      SARibbonBar* ribbon = ribbonwindow->ribbonBar();
      ribbon->setRibbonStyle(SARibbonBar::WpsLiteStyle);
      //添加一个Main标签
      categoryMain = ribbon->addCategoryPage(QStringLiteral("Main"));
      //Main标签下添加一个File Pannel
      FilePannel = categoryMain->addPannel(QStringLiteral("FilePannel"));
      //开始为File Pannel添加action
      FilePannel->addLargeAction(actionNew);
      FilePannel->addLargeAction(actionOpen);
      FilePannel->addLargeAction(actionSave);
      FilePannel->addSmallAction(actionImportMesh);
      FilePannel->addSmallAction(actionImportGeometry);
  }
  @endcode
 */
class SA_RIBBON_EXPORT SARibbonBar : public QMenuBar
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonBar)
    Q_PROPERTY(RibbonStyles ribbonStyle READ currentRibbonStyle WRITE setRibbonStyle)
    Q_PROPERTY(bool minimumMode READ isMinimumMode WRITE setMinimumMode)
    Q_PROPERTY(bool minimumModeButton READ haveShowMinimumModeButton WRITE showMinimumModeButton)
    Q_PROPERTY(QColor windowTitleTextColor READ windowTitleTextColor WRITE setWindowTitleTextColor)
    Q_PROPERTY(QColor tabBarBaseLineColor READ tabBarBaseLineColor WRITE setTabBarBaseLineColor)
    Q_PROPERTY(Qt::Alignment windowTitleAligment READ windowTitleAligment WRITE setWindowTitleAligment)
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

    using FpCategoryIterate = std::function< bool(SARibbonCategory*) >;
    using FpPannelIterate   = SARibbonCategory::FpPannelIterate;

public:
    // 判断RibbonStyle是否为2行模式
    static bool isTwoRowStyle(RibbonStyles s);
    static bool isThreeRowStyle(RibbonStyles s);
    // 判断是否是office样式
    static bool isLooseStyle(RibbonStyles s);
    static bool isCompactStyle(RibbonStyles s);
    // 获取版本信息
    static QString versionString();

    // 获取默认的上下文标签颜色列表
    static QList< QColor > defaultContextCategoryColorList();

public:
    // 构造函数
    SARibbonBar(QWidget* parent = nullptr);
    ~SARibbonBar() Q_DECL_OVERRIDE;
    // 获取applicationButton
    QAbstractButton* applicationButton();

    // 设置applicationButton
    void setApplicationButton(QAbstractButton* btn);

    // 获取tabbar
    SARibbonTabBar* ribbonTabBar();

    // 添加一个标签
    SARibbonCategory* addCategoryPage(const QString& title);
    void addCategoryPage(SARibbonCategory* category);

    // 为了支持Qt designer,添加的一个重载函数
    Q_INVOKABLE void addCategoryPage(QWidget* category);

    // 添加一个category，category的位置在index，如果当前category数量少于index，将插入到最后
    SARibbonCategory* insertCategoryPage(const QString& title, int index);
    void insertCategoryPage(SARibbonCategory* category, int index);

    // 通过名字查找Category
    SARibbonCategory* categoryByName(const QString& title) const;

    // 通过ObjectName查找Category
    SARibbonCategory* categoryByObjectName(const QString& objname) const;

    // 通过索引找到category，如果超过索引范围，会返回nullptr
    SARibbonCategory* categoryByIndex(int index) const;

    // 隐藏category,并不会删除或者取走，只是隐藏
    void hideCategory(SARibbonCategory* category);

    // 显示被隐藏的category
    void showCategory(SARibbonCategory* category);

    // 判断这个category是否在显示状态，也就是tabbar有这个category
    bool isCategoryVisible(const SARibbonCategory* c) const;

    // 获取category的索引
    int categoryIndex(const SARibbonCategory* c) const;

    // 移动一个Category从from index到to index
    void moveCategory(int from, int to);

    // 获取当前显示的所有的SARibbonCategory，包含未显示的SARibbonContextCategory的SARibbonCategory也一并返回
    QList< SARibbonCategory* > categoryPages(bool getAll = true) const;

    // 移除SARibbonCategory
    void removeCategory(SARibbonCategory* category);

    // 添加一个上下文标签
    SARibbonContextCategory* addContextCategory(const QString& title, const QColor& color = QColor(), const QVariant& id = QVariant());
    void addContextCategory(SARibbonContextCategory* context);

    // 显示一个上下文标签
    void showContextCategory(SARibbonContextCategory* context);

    // 隐藏一个上下文标签
    void hideContextCategory(SARibbonContextCategory* context);

    // 判断上下文是否是在显示状态
    bool isContextCategoryVisible(SARibbonContextCategory* context);

    // 设置上下文标签的显示或隐藏
    void setContextCategoryVisible(SARibbonContextCategory* context, bool visible);

    // 获取所有的上下文标签
    QList< SARibbonContextCategory* > contextCategoryList() const;

    // 移除ContextCategory
    void destroyContextCategory(SARibbonContextCategory* context);

    // 设置为隐藏模式
    void setMinimumMode(bool isHide);

    // 当前Ribbon是否是隐藏模式
    bool isMinimumMode() const;

    // 设置显示隐藏ribbon按钮
    void showMinimumModeButton(bool isShow = true);

    // 是否显示隐藏ribbon按钮
    bool haveShowMinimumModeButton() const;

    // 隐藏ribbon对应的action
    QAction* minimumModeAction() const;

    // 当前的模式
    RibbonMode currentRibbonState() const;

    // ribbon tab的高度
    int tabBarHeight() const;
    void setTabBarHeight(int h, bool resizeByNow = true);

    // 标题栏的高度
    int titleBarHeight() const;
    void setTitleBarHeight(int h, bool resizeByNow = true);

    // category的高度
    int categoryHeight() const;
    void setCategoryHeight(int h, bool resizeByNow = true);

    // 获取正常模式下的mainBar的高度
    int normalModeMainBarHeight() const;

    // 最小模式下的MainBar高度
    int minimumModeMainBarHeight() const;

    // 激活tabbar右边的按钮群
    SARibbonButtonGroupWidget* activeRightButtonGroup();

    // 右侧按钮群
    SARibbonButtonGroupWidget* rightButtonGroup();

    // 激活QuickAccessBar
    SARibbonQuickAccessBar* activeQuickAccessBar();

    // 快速响应栏
    SARibbonQuickAccessBar* quickAccessBar();

    // 设置ribbon的风格
    void setRibbonStyle(RibbonStyles v);
    RibbonStyles currentRibbonStyle() const;

    // 设置当前ribbon的index
    void setCurrentIndex(int index);

    // 返回当前的tab索引
    int currentIndex();

    // 确保标签显示出来
    void raiseCategory(SARibbonCategory* category);

    // 判断当前的行数
    bool isTwoRowStyle() const;
    bool isThreeRowStyle() const;

    // 判断当前的样式
    bool isLooseStyle() const;
    bool isCompactStyle() const;

    // 告诉saribbonbar，window button的尺寸
    void setWindowButtonSize(const QSize& size);

    // 更新ribbon的布局数据，此函数适用于一些关键性尺寸变化，换起ribbon下面元素的布局,在发现刷新问题时，可以调用此函数
    void updateRibbonGeometry();

    // tabbar 底部会绘制一条线条，此接口定义线条颜色
    void setTabBarBaseLineColor(const QColor& clr);
    QColor tabBarBaseLineColor() const;

    // 设置标题颜色,如果不设置标题颜色，默认是SARibbonBar的qss的color属性
    void setWindowTitleTextColor(const QColor& clr);
    QColor windowTitleTextColor() const;

    // 设置标题的对齐方式
    void setWindowTitleAligment(Qt::Alignment al);
    Qt::Alignment windowTitleAligment() const;

    // 设置按钮允许换行，注意图标大小是由文字决定的，两行文字会让图标变小，如果想图标变大，文字不换行是最好的
    void setEnableWordWrap(bool on);
    bool isEnableWordWrap() const;

    // 设置pannel的标题栏高度
    int pannelTitleHeight() const;
    void setPannelTitleHeight(int h);

    // 设置pannel是否显示标题栏
    bool isEnableShowPannelTitle() const;
    void setEnableShowPannelTitle(bool on);

    // 获取SARibbonStackedWidget，谨慎使用此函数
    SARibbonStackedWidget* ribbonStackedWidget();

    // 设置是否显示标题
    void setTitleVisible(bool on = false);
    bool isTitleVisible() const;

    // 允许用户自定义AccessBar图标尺寸，默认为false
    void setEnableUserDefineAccessBarIconSize(bool on = true);
    bool isEnableUserDefineAccessBarIconSize() const;

    // 允许用户自定义RightBar图标尺寸，默认为false
    void setEnableUserDefineRightBarIconSize(bool on = true);
    bool isEnableUserDefineRightBarIconSize() const;

    // 上下文标签的颜色列表，上下文标签显示的时候，会从颜色列表中取颜色进行标签的渲染
    void setContextCategoryColorList(const QList< QColor >& cls);
    QList< QColor > contextCategoryColorList() const;

    // 设置ribbon的对齐方式
    void setRibbonAlignment(SARibbonAlignment al);
    SARibbonAlignment ribbonAlignment() const;

    // 此函数会遍历SARibbonBar下的所有Category，执行函数指针(bool(SARibbonCategory*))，函数指针返回false则停止迭代
    bool iterate(FpCategoryIterate fp);
    // 此函数会遍历SARibbonBar下的所有Category,并迭代所有的pannel，执行函数指针(bool(SARibbonPannel*))，函数指针返回false则停止迭代
    bool iterate(FpPannelIterate fp);
signals:

    /**
     @brief 应用按钮点击响应 - 左上角的按钮，通过关联此信号触发应用按钮点击的效果

     例如想点击按钮后弹出一个全屏的窗口（如office这些）
     */
    void applicationButtonClicked();

    /**
     @brief 标签页变化触发的信号
     @param index
     */
    void currentRibbonTabChanged(int index);

    /**
     @brief ribbon的状态发生了变化后触发此信号
     @param nowState 变更之后的ribbon状态
     */
    void ribbonModeChanged(SARibbonBar::RibbonMode nowState);

    /**
     @brief ribbon的状态发生了变化后触发此信号
     @param nowStyle 变更之后的ribbon样式
     */
    void ribbonStyleChanged(SARibbonBar::RibbonStyles nowStyle);

    /**
       @brief 标题栏高度发生了变化的信号
       @param oldHeight
       @param newHeight
     */
    void titleBarHeightChanged(int oldHeight, int newHeight);

protected:
    bool eventFilter(QObject* obj, QEvent* e) override;

    // 根据情况重置tabbar的宽度，主要针对wps模式
    int calcMinTabBarWidth() const;
    // 更新
    void updateCategoryTitleToTabName();
protected slots:
    void onWindowTitleChanged(const QString& title);
    void onWindowIconChanged(const QIcon& i);
    void onCategoryWindowTitleChanged(const QString& title);
    void onStackWidgetHided();
    virtual void onCurrentRibbonTabChanged(int index);
    virtual void onCurrentRibbonTabClicked(int index);
    virtual void onCurrentRibbonTabDoubleClicked(int index);
    void onContextsCategoryPageAdded(SARibbonCategory* category);
    void onContextsCategoryCategoryNameChanged(SARibbonCategory* category, const QString& title);
    void onTabMoved(int from, int to);

private:
    int tabIndex(SARibbonCategory* obj);
    void resizeAll();
    void resizeInLooseStyle();
    void resizeInCompactStyle();
    void paintInLooseStyle();
    void paintInCompactStyle();
    void resizeStackedContainerWidget();

    // 刷新所有ContextCategoryManagerData，这个在单独一个Category删除时调用
    void updateContextCategoryManagerData();
    void synchronousCategoryData(bool autoUpdate = true);

protected:
    virtual void paintEvent(QPaintEvent* e) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent* e) Q_DECL_OVERRIDE;
    virtual void moveEvent(QMoveEvent* e) Q_DECL_OVERRIDE;
    virtual void changeEvent(QEvent* e) Q_DECL_OVERRIDE;
    virtual bool event(QEvent* e) Q_DECL_OVERRIDE;
    virtual void paintTabbarBaseLine(QPainter& painter);
    virtual void paintWindowTitle(QPainter& painter, const QString& title, const QRect& titleRegion);
    virtual void paintContextCategoryTab(QPainter& painter, const QString& title, QRect contextRect, const QColor& color);
};
Q_DECLARE_OPERATORS_FOR_FLAGS(SARibbonBar::RibbonStyles)
#endif  // SARIBBONBAR_H
