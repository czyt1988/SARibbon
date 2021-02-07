#ifndef SARIBBONBAR_H
#define SARIBBONBAR_H
#include "SARibbonGlobal.h"
#include <QMenuBar>
#include <QVariant>
#include "SARibbonCategory.h"
#include "SARibbonContextCategory.h"
#include <QScopedPointer>

class QAbstractButton;
class SARibbonElementCreateDelegate;
class SARibbonBarPrivate;
class SARibbonTabBar;
class SARibbonButtonGroupWidget;
class SARibbonQuickAccessBar;

/**
 * @brief SARibbonBar继承于QMenuBar,在SARibbonMainWindow中直接替换了原来的QMenuBar
 *
 * 通过setRibbonStyle函数设置ribbon的风格:
 *
 * @code
 * void setRibbonStyle(RibbonStyle v);
 * @endcode
 *
 * SARibbonBar参考office和wps，提供了四种风格的Ribbon模式,@ref SARibbonBar::RibbonStyle
 *
 * 如果想ribbon占用的空间足够小，WpsLiteStyleTwoRow模式能比OfficeStyle节省35%的高度空间
 *
 * 如何生成ribbon?先看看一个传统的Menu/ToolBar是如何生成的：
 *
 * @code
 * void MainWindow::MainWindow()
 * {
 *    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
 *    QToolBar *fileToolBar = addToolBar(tr("File"));
 *    //生成action
 *    QAction *newAct = new QAction(newIcon, tr("&New"), this);
 *    fileMenu->addAction(newAct);
 *    fileToolBar->addAction(newAct);
 *
 *    QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
 *    fileMenu->addAction(openAct);
 *    fileToolBar->addAction(openAct);
 * }
 * @endcode
 *
 * 传统的Menu/ToolBar主要通过QMenu的addMenu添加菜单,通过QMainWindow::addToolBar生成QToolBar,
 * 再把QAction设置进QMenu和QToolBar中
 *
 * SARibbonBar和传统方法相似，不过相对于传统的Menu/ToolBar QMenu和QToolBar是平级的，
 * Ribbon是有明显的层级关系，SARibbonBar下面是 @ref SARibbonCategory，
 * SARibbonCategory下面是@ref SARibbonPannel ，SARibbonPannel下面是@ref SARibbonToolButton ，
 * SARibbonToolButton管理着QAction
 *
 * 因此，生成一个ribbon只需以下几个函数：
 * @code
 * SARibbonCategory * SARibbonBar::addCategoryPage(const QString& title);
 * SARibbonPannel * SARibbonCategory::addPannel(const QString& title);
 * SARibbonToolButton * SARibbonPannel::addLargeAction(QAction *action);
 * SARibbonToolButton * SARibbonPannel::addSmallAction(QAction *action);
 * @endcode
 *
 * 因此生成步骤如下：
 *
 * @code
 * //成员变量
 * SARibbonCategory* categoryMain;
 * SARibbonPannel* FilePannel;
 *
 * //建立ui
 * void setupRibbonUi()
 * {
 *     ......
 *     //ribbonwindow为SARibbonMainWindow
 *     SARibbonBar* ribbon = ribbonwindow->ribbonBar();
 *     ribbon->setRibbonStyle(SARibbonBar::WpsLiteStyle);
 *     //添加一个Main标签
 *     categoryMain = ribbon->addCategoryPage(QStringLiteral("Main"));
 *     //Main标签下添加一个File Pannel
 *     FilePannel = categoryMain->addPannel(QStringLiteral("FilePannel"));
 *     //开始为File Pannel添加action
 *     FilePannel->addLargeAction(actionNew);
 *     FilePannel->addLargeAction(actionOpen);
 *     FilePannel->addLargeAction(actionSave);
 *     FilePannel->addSmallAction(actionImportMesh);
 *     FilePannel->addSmallAction(actionImportGeometry);
 * }
 * @endcode
 */
class SA_RIBBON_EXPORT SARibbonBar : public QMenuBar
{
    Q_OBJECT
    friend class SARibbonBarPrivate;
public:

    /**
     * @brief 定义ribbon的风格,第一字节代表样式，第二字节代表是否是2行
     */
    enum RibbonStyle {
        OfficeStyle		= 0x0000        ///< 类似office 的ribbon风格
        , WpsLiteStyle		= 0x0001        ///< 类似wps的紧凑风格
        , OfficeStyleTwoRow	= 0x0100        ///< 类似office 的ribbon风格 2行工具栏 三行布局模式，office就是三行布局模式，pannel能布置3行小toolbutton，默认模式
        , WpsLiteStyleTwoRow	= 0x0101        ///< 类似wps的紧凑风格  2行工具栏
    };
    Q_FLAGS(RibbonStyle)

    /**
     * @brief 定义当前ribbon 的状态
     */
    enum RibbonState {
        MinimumRibbonMode       ///< 缩小模式
        , NormalRibbonMode      ///< 正常模式
    };
    //判断RibbonStyle是否为2行模式
    static bool isTwoRowStyle(RibbonStyle s);

    //判断是否是office样式
    static bool isOfficeStyle(RibbonStyle s);

    //构造函数
    SARibbonBar(QWidget *parent = nullptr);
    ~SARibbonBar();
    //获取applitionButton
    QAbstractButton *applicationButton();

    //设置applitionButton
    void setApplicationButton(QAbstractButton *btn);

    //获取tabbar
    SARibbonTabBar *ribbonTabBar();

    //添加一个标签
    SARibbonCategory *addCategoryPage(const QString& title);

    //获取当前显示的所有的SARibbonCategory，不含未显示的SARibbonContextCategory的SARibbonCategory
    QList<SARibbonCategory *> categoryPages() const;

    //移除SARibbonCategory
    void removeCategory(SARibbonCategory *category);

    //添加一个上下文标签
    SARibbonContextCategory *addContextCategory(const QString& title, const QColor& color, const QVariant& id = QVariant());

    //显示一个上下文标签
    void showContextCategory(SARibbonContextCategory *context);

    //隐藏一个上下文标签
    void hideContextCategory(SARibbonContextCategory *context);

    //判断上下文是否是在显示状态
    bool isContextCategoryVisible(SARibbonContextCategory *context);

    //设置上下文标签的显示或隐藏
    void setContextCategoryVisible(SARibbonContextCategory *context, bool visible);

    //获取所有的上下文标签
    QList<SARibbonContextCategory *> contextCategoryList() const;

    //移除ContextCategory
    void destroyContextCategory(SARibbonContextCategory *context);

    //设置为隐藏模式
    void setMinimumMode(bool isHide);

    //当前Ribbon是否是隐藏模式
    bool isMinimumMode() const;

    //设置显示隐藏ribbon按钮
    void showMinimumModeButton(bool isShow = true);

    //是否显示隐藏ribbon按钮
    bool haveShowMinimumModeButton() const;

    //ribbon tab的高度
    int tabBarHeight() const;

    //标题栏的高度
    int titleBarHeight() const;

    //激活tabbar右边的按钮群
    SARibbonButtonGroupWidget *activeTabBarRightButtonGroup();

    //快速响应栏
    SARibbonQuickAccessBar *quickAccessBar();

    //设置ribbon的风格
    void setRibbonStyle(RibbonStyle v);

    //当前ribbon的风格
    RibbonStyle currentRibbonStyle() const;

    //当前的模式
    RibbonState currentRibbonState() const;

	//设置当前ribbon的index
	void setCurrentIndex(int index);

	//确保标签显示出来
    void raiseCategory(SARibbonCategory *category);

    //判断当前的样式是否为两行
    bool isTwoRowStyle() const;

    //判断当前的样式是否为office样式
    bool isOfficeStyle() const;

    //告诉saribbonbar，window button的尺寸
    void setWindowButtonSize(const QSize& size);

signals:

    /**
     * @brief 应用按钮点击响应 - 左上角的按钮，通过关联此信号触发应用按钮点击的效果
     */
    void applitionButtonClicked();

    /**
     * @brief 标签页变化触发的信号
     * @param index
     */
    void currentRibbonTabChanged(int index);

protected:
    bool eventFilter(QObject *obj, QEvent *e) override;

    //根据情况重置tabbar的宽度，主要针对wps模式
    int calcMinTabBarWidth() const;

    //根据currentRibbonStyle计算mainBar的高度
    virtual int mainBarHeight() const;

    //应用按钮的宽度
    virtual int applitionButtonWidth() const;

protected slots:
    void onWindowTitleChanged(const QString& title);
    void onWindowIconChanged(const QIcon& icon);
    void onCategoryWindowTitleChanged(const QString& title);
    void onStackWidgetHided();
    virtual void onCurrentRibbonTabChanged(int index);
    virtual void onCurrentRibbonTabClicked(int index);
    virtual void onCurrentRibbonTabDoubleClicked(int index);
    void onContextsCategoryPageAdded(SARibbonCategory *category);

private:
    int tabIndex(SARibbonCategory *obj);
    void updateRibbonElementGeometry();
    void resizeInOfficeStyle();
    void resizeInWpsLiteStyle();
    void paintInNormalStyle();
    void paintInWpsLiteStyle();
    void resizeStackedContainerWidget();

    //刷新所有ContextCategoryManagerData，这个在单独一个Category删除时调用
    void updateContextCategoryManagerData();

protected:
    void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;
    void moveEvent(QMoveEvent *event) Q_DECL_OVERRIDE;
    virtual void paintBackground(QPainter& painter);
    virtual void paintWindowTitle(QPainter& painter, const QString& title, const QRect& titleRegion);
    virtual void paintWindowIcon(QPainter& painter, const QIcon& icon);
    virtual void paintContextCategoryTab(QPainter& painter, const QString& title, QRect contextRect, const QColor& color);

private:
    SARibbonBarPrivate *m_d;
};



#endif // SARIBBONBAR_H
