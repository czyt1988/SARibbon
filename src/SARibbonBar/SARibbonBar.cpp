#include "SARibbonBar.h"
#include <QPointer>
#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QHoverEvent>
#include <QLinearGradient>
#include <QPainter>
#include <QResizeEvent>
#include <QSet>
#include <QStyleOptionMenuItem>
#include <QTimer>
#include <QVariant>
#include <QDateTime>
#include "SARibbonUtil.h"
#include "SARibbonButtonGroupWidget.h"
#include "SARibbonElementManager.h"
#include "SARibbonQuickAccessBar.h"
#include "SARibbonStackedWidget.h"
#include "SARibbonTabBar.h"
#include "SARibbonApplicationButton.h"
#include "SARibbonBarLayout.h"
#include "SARibbonTitleIconWidget.h"
#include "SARibbonMainWindow.h"

#ifndef SARIBBONBAR_DEBUG_PRINT
#define SARIBBONBAR_DEBUG_PRINT 0
#endif
#if SARIBBONBAR_DEBUG_PRINT
#ifndef SARIBBONBAR_HELP_DRAW_RECT
#define SARIBBONBAR_HELP_DRAW_RECT(p, rect)                                                                            \
    do {                                                                                                               \
        p.save();                                                                                                      \
        QPen _pen(Qt::DashDotDotLine);                                                                                 \
        _pen.setColor(QColor(219, 26, 59));                                                                            \
        p.setPen(_pen);                                                                                                \
        p.setBrush(QBrush());                                                                                          \
        p.drawRect(rect);                                                                                              \
        p.restore();                                                                                                   \
    } while (0)
#endif
#endif
class _SAContextCategoryManagerData
{
public:
    SARibbonContextCategory* contextCategory;
    QList< int > tabPageIndex;
    bool operator==(const SARibbonContextCategory* contextPage)
    {
        return (this->contextCategory == contextPage);
    }
};

/**
 * @todo 此处要修改，此方式容易异常
 */
class _SARibbonTabData
{
public:
    SARibbonCategory* category;
    int index;
    _SARibbonTabData() : category(nullptr), index(-1)
    {
    }
};
Q_DECLARE_METATYPE(_SARibbonTabData)

class SARibbonBar::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonBar)
public:
    QPointer< QAbstractButton > mApplicationButton;
    QPointer< SARibbonTabBar > mRibbonTabBar;
    QPointer< SARibbonStackedWidget > mStackedContainerWidget;
    QPointer< SARibbonButtonGroupWidget > mRightButtonGroup;  ///< 在tab bar右边的按钮群
    QPointer< SARibbonQuickAccessBar > mQuickAccessBar;       ///< 快速响应栏
    QPointer< SARibbonTitleIconWidget > mTitleIconWidget;     ///< 标题栏图标

    QAction* mMinimumCategoryButtonAction { nullptr };  ///< 隐藏面板按钮action
    QList< _SAContextCategoryManagerData > mCurrentShowingContextCategory;
    QList< SARibbonContextCategory* > mContextCategoryList;  ///< 存放所有的上下文标签
    QList< _SARibbonTabData > mHidedCategory;
    qint64 mTabBarLastClickTime { 0 };  ///< 记录tabbar点击的上次点击时间戳，tabbar无法区分双击单击，双击必定触发单击，因此需要此把快速单击去掉
    SARibbonBar::RibbonStyles mRibbonStyle { SARibbonBar::RibbonStyleLooseThreeRow };  ///< ribbon的风格
    SARibbonBar::RibbonMode mCurrentRibbonMode { SARibbonBar::NormalRibbonMode };      ///< 记录当前模式
    QList< QColor > mContextCategoryColorList;                                         ///< contextCategory的色系
    int mContextCategoryColorListIndex { -1 };  ///< 记录contextCategory色系索引
    QColor mTitleTextColor;  ///< 标题文字颜色,默认无效，无效的情况下和SARibbonBar的qss:color属性一致
    QColor mTabBarBaseLineColor { QColor(186, 201, 219) };  ///< tabbar 底部会绘制一条线条，定义线条颜色
    QColor mContextCategoryTitleTextColor { Qt::black };    ///< 记录Context category的标题字体颜色
    Qt::Alignment mTitleAligment { Qt::AlignCenter };       ///< 标题对齐方式
    bool mIsTitleVisible { true };                          ///< 标题是否显示
    QBrush mTitleBackgroundBrush { Qt::NoBrush };           ///< 标题的背景颜色
    SARibbonAlignment mRibbonAlignment { SARibbonAlignment::AlignLeft };                     ///< 对齐方式
    SARibbonPanel::PanelLayoutMode mDefaulePanelLayoutMode { SARibbonPanel::ThreeRowMode };  ///< 默认的PanelLayoutMode
    bool mEnableShowPanelTitle { true };                         ///< 是否允许panel的标题栏显示
    int mPanelSpacing { 0 };                                     ///< panel的spacing
    QSize mPanelSmallToolButtonSize { 20, 20 };                  ///< 记录panel的默认图标大小
    QSize mPanelLargeToolButtonSize { 32, 32 };                  ///< 记录panel的大图标尺寸
    SARibbonMainWindowStyles mMainWindowStyle;                   ///< 记录MainWindow的样式
    FpContextCategoryHighlight mFpContextHighlight { nullptr };  ///< 上下文标签高亮
    bool mEnableTabDoubleClickToMinimumMode { true };  ///< 是否允许tab双击激活ribbon的最小化模式
    bool mEnableWordWrap { true };                     ///< 是否允许文字换行
    qreal buttonMaximumAspectRatio { 1.4 };            ///< 按钮的最大宽高比
public:
    PrivateData(SARibbonBar* par) : q_ptr(par)
    {
        mContextCategoryColorList = SARibbonBar::defaultContextCategoryColorList();
        mFpContextHighlight       = [](const QColor& c) -> QColor { return SA::makeColorVibrant(c); };
    }
    void init();
    // 创建QuickAccessBar
    SARibbonQuickAccessBar* createQuickAccessBar();
    // 创建右边工具栏
    SARibbonButtonGroupWidget* createRightButton();
    // 创建标题栏图标
    SARibbonTitleIconWidget* createTitleIconWidget();
    // 创建一个默认的ApplicationButton
    void createDefaultApplicationButton();
    // 设置一个ApplicationButton
    void setApplicationButton(QAbstractButton* btn);

    bool isContainContextCategoryInList(SARibbonContextCategory* contextCategory);

    void setMinimumMode();

    void setNormalMode();

    QColor getContextCategoryColor();

    void updateTabData();
    // 是否使用ribbon边框，如果不是，将影响布局方式
    bool isUseRibbonFrame() const;
    // 计算上下文标签的标题矩形，矩形位置相对于SARibbonBar
    QRect calcContextCategoryTitleRect(const _SAContextCategoryManagerData& contextData);

    // 重新布局
    void relayout();

    // 初始化新的父对象
    void initNewParent(QWidget* par);
};

void SARibbonBar::PrivateData::init()
{
    q_ptr->setNativeMenuBar(false);

    createDefaultApplicationButton();
    mRibbonTabBar = RibbonSubElementFactory->createRibbonTabBar(q_ptr);
    mRibbonTabBar->setObjectName(QStringLiteral("objSARibbonTabBar"));
    mRibbonTabBar->setDrawBase(false);
    q_ptr->connect(mRibbonTabBar.data(), &QTabBar::currentChanged, q_ptr, &SARibbonBar::onCurrentRibbonTabChanged);
    q_ptr->connect(mRibbonTabBar.data(), &QTabBar::tabBarClicked, q_ptr, &SARibbonBar::onCurrentRibbonTabClicked);
    q_ptr->connect(mRibbonTabBar.data(), &QTabBar::tabBarDoubleClicked, q_ptr, &SARibbonBar::onCurrentRibbonTabDoubleClicked);
    q_ptr->connect(mRibbonTabBar.data(), &QTabBar::tabMoved, q_ptr, &SARibbonBar::onTabMoved);
    //
    mStackedContainerWidget = RibbonSubElementFactory->createRibbonStackedWidget(q_ptr);
    mStackedContainerWidget->setObjectName(QStringLiteral("objSARibbonStackedContainerWidget"));
    q_ptr->connect(mStackedContainerWidget.data(), &SARibbonStackedWidget::hidWindow, q_ptr, &SARibbonBar::onStackWidgetHided);
    // 捕获事件，在popmode时必须用到
    mStackedContainerWidget->installEventFilter(q_ptr);
    // 快速工具栏
    createQuickAccessBar();
    // 右侧工具栏
    createRightButton();
    // 创建图标窗口
    createTitleIconWidget();
    setNormalMode();

    SARibbonBarLayout* lay = new SARibbonBarLayout(q_ptr);
    q_ptr->setLayout(lay);
    q_ptr->setRibbonStyle(RibbonStyleLooseThreeRow);
    q_ptr->ensurePolished();
}

SARibbonQuickAccessBar* SARibbonBar::PrivateData::createQuickAccessBar()
{
    mQuickAccessBar = RibbonSubElementFactory->createQuickAccessBar(q_ptr);
    mQuickAccessBar->setObjectName(QStringLiteral("objSARibbonQuickAccessBar"));
    mQuickAccessBar->setIconSize(QSize(22, 22));
    return mQuickAccessBar.data();
}

SARibbonButtonGroupWidget* SARibbonBar::PrivateData::createRightButton()
{
    mRightButtonGroup = RibbonSubElementFactory->createButtonGroupWidget(q_ptr);
    mRightButtonGroup->setObjectName(QStringLiteral("objSARibbonRightButtonGroup"));
    mRightButtonGroup->setIconSize(QSize(22, 22));
    return mRightButtonGroup.data();
}

SARibbonTitleIconWidget* SARibbonBar::PrivateData::createTitleIconWidget()
{
    mTitleIconWidget = RibbonSubElementFactory->createRibbonTitleIconWidget(q_ptr);
    return mTitleIconWidget.data();
}

void SARibbonBar::PrivateData::createDefaultApplicationButton()
{
    setApplicationButton(RibbonSubElementFactory->createRibbonApplicationButton(q_ptr));
}

void SARibbonBar::PrivateData::setApplicationButton(QAbstractButton* btn)
{
    if (mApplicationButton) {
        mApplicationButton->hide();
        mApplicationButton->deleteLater();
    }
    if (btn) {
        if (btn->parent() != q_ptr) {
            btn->setParent(q_ptr);
        }
        if (btn->objectName().isEmpty()) {
            btn->setObjectName(QStringLiteral("SARibbonApplicationButton"));
        }
        btn->setVisible(true);
        btn->show();
        q_ptr->connect(btn, &QAbstractButton::clicked, q_ptr, &SARibbonBar::applicationButtonClicked);
    }
    mApplicationButton = btn;
}

bool SARibbonBar::PrivateData::isContainContextCategoryInList(SARibbonContextCategory* contextCategory)
{
    for (int i = 0; i < mCurrentShowingContextCategory.size(); ++i) {
        if (mCurrentShowingContextCategory[ i ] == contextCategory) {
            return (true);
        }
    }
    return (false);
}

/**
 * @brief 设置最小模式
 */
void SARibbonBar::PrivateData::setMinimumMode()
{
    mCurrentRibbonMode = SARibbonBar::MinimumRibbonMode;
    mStackedContainerWidget->setPopupMode();
    mStackedContainerWidget->setFocusPolicy(Qt::NoFocus);
    mStackedContainerWidget->clearFocus();
    mRibbonTabBar->setFocus();
    mStackedContainerWidget->hide();
    if (SARibbonBarLayout* lay = qobject_cast< SARibbonBarLayout* >(q_ptr->layout())) {
        lay->resetSize();
        lay->layoutStackedContainerWidget();
    }
}

/**
 * @brief 设置正常模式
 */
void SARibbonBar::PrivateData::setNormalMode()
{
    mCurrentRibbonMode = SARibbonBar::NormalRibbonMode;
    mStackedContainerWidget->setNormalMode();
    mStackedContainerWidget->setFocus();
    mStackedContainerWidget->show();
    if (SARibbonBarLayout* lay = qobject_cast< SARibbonBarLayout* >(q_ptr->layout())) {
        lay->resetSize();
        lay->layoutStackedContainerWidget();
    }
}

/**
 * @brief 获取ContextCategory的color
 * @return
 */
QColor SARibbonBar::PrivateData::getContextCategoryColor()
{
    if (mContextCategoryColorList.isEmpty()) {
        mContextCategoryColorListIndex = -1;
        return (QColor());
    }
    ++mContextCategoryColorListIndex;
    if ((mContextCategoryColorListIndex >= mContextCategoryColorList.size()) || (mContextCategoryColorListIndex < 0)) {
        mContextCategoryColorListIndex = 0;
    }
    return (mContextCategoryColorList.at(mContextCategoryColorListIndex));
}

/**
 * @brief 更新tab数据
 */
void SARibbonBar::PrivateData::updateTabData()
{
    int tabcount = mRibbonTabBar->count();

    for (int i = 0; i < tabcount; ++i) {
        QVariant var = mRibbonTabBar->tabData(i);
        if (var.isValid()) {
            _SARibbonTabData p = var.value< _SARibbonTabData >();
            p.index            = i;
            mRibbonTabBar->setTabData(i, QVariant::fromValue(p));
        }
    }
    // 刷新完tabdata信息也要接着刷新ContextCategory信息
    for (_SAContextCategoryManagerData& cd : mCurrentShowingContextCategory) {
        cd.tabPageIndex.clear();
        for (int i = 0; i < cd.contextCategory->categoryCount(); ++i) {
            SARibbonCategory* category = cd.contextCategory->categoryPage(i);
            for (int t = 0; t < tabcount; ++t) {
                QVariant v = mRibbonTabBar->tabData(t);
                if (v.isValid()) {
                    _SARibbonTabData d = v.value< _SARibbonTabData >();
                    if (d.category == category) {
                        cd.tabPageIndex.append(t);
                    }
                } else {
                    cd.tabPageIndex.append(-1);
                }
            }
        }
    }
}

/**
 * @brief 获取当前tabbar的样式
 */
bool SARibbonBar::PrivateData::isUseRibbonFrame() const
{
    return mMainWindowStyle.testFlag(SARibbonMainWindowStyleFlag::UseRibbonFrame);
}

QRect SARibbonBar::PrivateData::calcContextCategoryTitleRect(const _SAContextCategoryManagerData& contextData)
{
    QRect contextTitleRect;
    SARibbonTabBar* ribbonTabBar = mRibbonTabBar.data();
    const QMargins border        = q_ptr->contentsMargins();
    const QList< int >& indexs   = contextData.tabPageIndex;
    if (!indexs.isEmpty()) {
        contextTitleRect = ribbonTabBar->tabRect(indexs.first());
        QRect endRect    = ribbonTabBar->tabRect(indexs.last());
        contextTitleRect.setRight(endRect.right());
        contextTitleRect.translate(ribbonTabBar->x(), ribbonTabBar->y());
        contextTitleRect.setHeight(ribbonTabBar->height() - 1);  // 减1像素，避免tabbar基线覆盖
        contextTitleRect -= ribbonTabBar->tabMargin();
        // 把区域顶部扩展到窗口顶部
        contextTitleRect.setTop(border.top());
    }
    return contextTitleRect;
}

void SARibbonBar::PrivateData::relayout()
{
    if (auto lay = q_ptr->layout()) {
        lay->invalidate();
        lay->activate();
#if SARIBBONBAR_DEBUG_PRINT
        qDebug() << "SARibbonBar relayout";
#endif
    }
}

void SARibbonBar::PrivateData::initNewParent(QWidget* par)
{
    if (!par) {
        return;
    }
    q_ptr->connect(par, &QWidget::windowTitleChanged, q_ptr, &SARibbonBar::onWindowTitleChanged);
    q_ptr->connect(par, &QWidget::windowIconChanged, q_ptr, &SARibbonBar::onWindowIconChanged);
    // 父窗口发生了改变，一般就是加入到了MainWindow中，这时要同步父窗口的信息到图标
    SARibbonTitleIconWidget* titleIcon = q_ptr->titleIconWidget();
    if (!titleIcon) {
        return;
    }

    if (q_ptr->isApplicationButtonVerticalExpansion()) {
        titleIcon->hide();
        return;
    }

    // 非垂直展开模式：尝试设置为 MainWindow 的图标
    if (auto* mainwindow = qobject_cast< SARibbonMainWindow* >(par)) {
        int th = q_ptr->titleBarHeight();
        titleIcon->setWidget(mainwindow);
        titleIcon->setIcon(mainwindow->windowIcon());
        titleIcon->setIconSize(QSize(th - 2, th - 2));
        titleIcon->show();
        titleIcon->raise();
    } else {
        titleIcon->hide();
    }
}

//===================================================
// SARibbonBar
//===================================================

/**
 * @brief SARibbonBar构造函数
 * @param parent
 */
SARibbonBar::SARibbonBar(QWidget* parent) : QMenuBar(parent), d_ptr(new SARibbonBar::PrivateData(this))
{
    d_ptr->init();
    d_ptr->initNewParent(parent);
}

SARibbonBar::~SARibbonBar()
{
}

/**
 * @brief 判断样式是否为2行
 * @param s
 * @return 2行返回true，返回false代表当前是3行
 */

/**
 * @brief 判断样式是否为3行
 * @param s
 * @return 3行返回true，返回false代表当前是2行
 */
bool SARibbonBar::isTwoRowStyle(RibbonStyles s)
{
    return s.testFlag(SARibbonBar::RibbonStyleTwoRow);
}

bool SARibbonBar::isThreeRowStyle(RibbonStyles s)
{
    return s.testFlag(SARibbonBar::RibbonStyleThreeRow);
}

/**
 * @brief 判断是否是宽松样式
 * @param s
 * @return 宽松样式（带标题栏）返回true，否则就是紧凑样式
 */
bool SARibbonBar::isLooseStyle(SARibbonBar::RibbonStyles s)
{
    return s.testFlag(SARibbonBar::RibbonStyleLoose);
}

/**
 * @brief 判断是否是紧凑样式
 * @param s
 * @return 紧凑样式（不带标题栏）返回true，否则就是宽松样式
 */
bool SARibbonBar::isCompactStyle(RibbonStyles s)
{
    return s.testFlag(SARibbonBar::RibbonStyleCompact);
}

/**
 * @brief 获取版本信息
 * @return {SA_RIBBON_BAR_VERSION_MAJ}.{SA_RIBBON_BAR_VERSION_MIN}.{SA_RIBBON_BAR_VERSION_PAT}
 */
QString SARibbonBar::versionString()
{
    return QString("%1.%2.%3").arg(SA_RIBBON_BAR_VERSION_MAJ).arg(SA_RIBBON_BAR_VERSION_MIN).arg(SA_RIBBON_BAR_VERSION_PAT);
}

/**
@brief 获取默认的上下文标签颜色列表
@return
*/
QList< QColor > SARibbonBar::defaultContextCategoryColorList()
{
    QList< QColor > res;
    res                           //
        << QColor(206, 232, 252)  // 蓝
        << QColor(253, 238, 179)  // 黄
        << QColor(212, 255, 174)  // 绿
        << QColor(255, 196, 214)  // 红
        << QColor(255, 216, 153)  // 橙
        << QColor(255, 224, 243)  // 玫红
        ;
    return res;
}

/**
 * @brief 提供高分屏的支持静态函数
 *
 * @note 此函数需要在main函数，QApplication生成之前调用
 * @code
 * int main(int argc, char* argv[]){
 *    SARibbonBar::initHighDpi();
 *    QApplication a(argc, argv);
 *    ...
 * }
 * @endcode
 */
void SARibbonBar::initHighDpi()
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)) && (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    // 在有些时候，无法显示分割线，是由于PassThrough导致的
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
}

/**
 * @brief 返回applicationButton
 * @return 默认的applicationButton由 @ref SARibbonApplicationButton 生成，通过@ref setApplicationButton 可设置为其他button
 */
QAbstractButton* SARibbonBar::applicationButton()
{
    return (d_ptr->mApplicationButton.data());
}

/**
 * @brief 设置applicationButton,如果想隐藏，可以传入nullptr
 *
 * 默认会有一个SARibbonApplicationButton，如果想取消，可传入nullptr，或者自定义的button也可以传入
 *
 * @note applicationButton的所有权归SARibbonBar所有，不要在外部对applicationButton进行delete操作
 * @param btn applicationButton指针，可以传入@ref SARibbonApplicationButton，
 * SA已经对SARibbonApplicationButton进行了样式设置
 */
void SARibbonBar::setApplicationButton(QAbstractButton* btn)
{
    d_ptr->setApplicationButton(btn);
    d_ptr->relayout();
}

/**
 * @brief 返回tabbar
 * @return SARibbonTabBar指针
 * @sa SARibbonTabBar
 */
SARibbonTabBar* SARibbonBar::ribbonTabBar()
{
    return (d_ptr->mRibbonTabBar);
}

/**
 * @brief 添加一个标签
 * 如果需要删除，直接delete即可，SARibbonBar会对其进行处理
 * @param title 标签名字，默认情况下SARibbonCategory的object name也被设置为title
 * @return 返回一个窗口容器，在Category里可以添加其他控件
 * @sa SARibbonCategory
 */
SARibbonCategory* SARibbonBar::addCategoryPage(const QString& title)
{
    SARibbonCategory* category = RibbonSubElementFactory->createRibbonCategory(this);

    category->setObjectName(title);
    category->setCategoryName(title);

    addCategoryPage(category);
    return (category);
}

/**
 * @brief 添加一个标签
 * @param category
 */
void SARibbonBar::addCategoryPage(SARibbonCategory* category)
{
    if (nullptr == category) {
        return;
    }
    int index = d_ptr->mRibbonTabBar->count();
    insertCategoryPage(category, index);
}

/**
 * @brief qtdesigner专用
 * @param category
 */
void SARibbonBar::addCategoryPage(QWidget* category)
{
    SARibbonCategory* c = qobject_cast< SARibbonCategory* >(category);

    if (c) {
        addCategoryPage(c);
    }
}

/**
 * @brief 添加一个category，category的位置在index，如果当前category数量少于index，将插入到最后
 * @param title category的标题
 * @param index category的位置
 * @return
 */
SARibbonCategory* SARibbonBar::insertCategoryPage(const QString& title, int index)
{
    SARibbonCategory* category = RibbonSubElementFactory->createRibbonCategory(this);

    category->setObjectName(title);
    category->setCategoryName(title);
    insertCategoryPage(category, index);
    return (category);
}

/**
 * @brief 插入一个category
 * @param category SARibbonCategory指针
 * @param index 插入的位置，如果超出范围，将默认插入到最后
 */
void SARibbonBar::insertCategoryPage(SARibbonCategory* category, int index)
{
    if (nullptr == category) {
        return;
    }
    category->setPanelLayoutMode(panelLayoutMode());
    category->setPanelSpacing(panelSpacing());
    category->setPanelLargeIconSize(panelLargeIconSize());
    category->setPanelSmallIconSize(panelSmallIconSize());
    category->setEnableWordWrap(isEnableWordWrap());

    int i = d_ptr->mRibbonTabBar->insertTab(index, category->categoryName());

    _SARibbonTabData tabdata;

    tabdata.category = category;
    tabdata.index    = i;
    d_ptr->mRibbonTabBar->setTabData(i, QVariant::fromValue(tabdata));
    d_ptr->mStackedContainerWidget->insertWidget(index, category);
    // 更新index信息
    d_ptr->updateTabData();
    connect(category, &QWidget::windowTitleChanged, this, &SARibbonBar::onCategoryWindowTitleChanged);
    connect(category, &SARibbonCategory::actionTriggered, this, &SARibbonBar::actionTriggered);
    d_ptr->relayout();
}

/**
 * @brief 通过名字查找Category
 * @param title Category的名字，既标签的标题
 * @return 如果没有找到，将返回nullptr，如果有重名，将返回第一个查询到的名字，因此，尽量避免重名标签
 * @note 由于翻译等原因，可能title会变化，因此如果想通过固定内容查找category，应该使用 @ref categoryByObjectName
 * @see categoryByObjectName
 */
SARibbonCategory* SARibbonBar::categoryByName(const QString& title) const
{
    int c = d_ptr->mStackedContainerWidget->count();

    for (int i = 0; i < c; ++i) {
        SARibbonCategory* w = qobject_cast< SARibbonCategory* >(d_ptr->mStackedContainerWidget->widget(i));
        if (w) {
            if (w->categoryName() == title) {
                return (w);
            }
        }
    }
    return (nullptr);
}

/**
 * @brief 通过ObjectName查找Category
 * @param objname
 * @return 如果没有找到，将返回nullptr，如果有同样的ObjectName，将返回第一个查询到的名字，因此，尽量避免ObjectName重名
 * @see categoryByName
 */
SARibbonCategory* SARibbonBar::categoryByObjectName(const QString& objname) const
{
    int c = d_ptr->mStackedContainerWidget->count();

    for (int i = 0; i < c; ++i) {
        SARibbonCategory* w = qobject_cast< SARibbonCategory* >(d_ptr->mStackedContainerWidget->widget(i));
        if (w) {
            if (w->objectName() == objname) {
                return (w);
            }
        }
    }
    return (nullptr);
}

/**
 * @brief 通过索引找到category，如果超过索引范围，会返回nullptr
 * @param index 索引
 * @return 如果超过索引范围，会返回nullptr
 * @note 如果此时有上下文标签，上下文的标签也会返回
 * @note 通过索引查找的category必须是visible状态的category，如果通过@ref hideCategory 隐藏的标签，通过索引是找不到的
 * @note 通过@ref categoryByObjectName 可以找到所有加入过的标签，
 * 如果想得到ribbonbar管理的所有标签，可以通过函数@ref categoryPages 得到
 * @see categoryIndex categoryByObjectName categoryByName
 */
SARibbonCategory* SARibbonBar::categoryByIndex(int index) const
{
    QVariant var = d_ptr->mRibbonTabBar->tabData(index);

    if (var.isValid()) {
        _SARibbonTabData p = var.value< _SARibbonTabData >();
        return (p.category);
    }
    return (nullptr);
}

/**
 * @brief 隐藏category,并不会删除或者取走，只是隐藏
 * @param category
 */
void SARibbonBar::hideCategory(SARibbonCategory* category)
{
    int tabcount = d_ptr->mRibbonTabBar->count();

    for (int i = 0; i < tabcount; ++i) {
        QVariant var = d_ptr->mRibbonTabBar->tabData(i);
        if (var.isValid()) {
            _SARibbonTabData p = var.value< _SARibbonTabData >();
            if (p.category == category) {
                d_ptr->mHidedCategory.append(p);
                d_ptr->mRibbonTabBar->removeTab(i);  // 仅仅把tab移除
                // 注意Category隐藏后，contex的位置就会发生变化，需要更新
                d_ptr->updateTabData();
                return;
            }
        }
    }
}

/**
 * @brief 显示被隐藏的category
 * @param category
 */
void SARibbonBar::showCategory(SARibbonCategory* category)
{
    for (auto i = d_ptr->mHidedCategory.begin(); i != d_ptr->mHidedCategory.end(); ++i) {
        if (i->category == category) {
            // 说明要显示
            int index = d_ptr->mRibbonTabBar->insertTab(i->index, i->category->categoryName());
            i->index  = index;
            d_ptr->mRibbonTabBar->setTabData(index, QVariant::fromValue(*i));
            d_ptr->mHidedCategory.erase(i);  // 移除
            // 更新index信息
            d_ptr->updateTabData();
            raiseCategory(category);
            return;
        }
    }
    raiseCategory(category);
}

/**
 * @brief 判断这个category是否在显示状态，也就是tabbar有这个category
 * @param category
 * @return
 */
bool SARibbonBar::isCategoryVisible(const SARibbonCategory* c) const
{
    int tabindex = categoryIndex(c);

    return (tabindex >= 0);
}

/**
 * @brief 获取category的索引
 * @param c
 * @return 如果找不到，返回-1
 */
int SARibbonBar::categoryIndex(const SARibbonCategory* c) const
{
    // category的顺序不能以stackedwidget为准，因为存在contextcategory，contextcategory正常是不显示的
    int tabcount = d_ptr->mRibbonTabBar->count();

    for (int i = 0; i < tabcount; ++i) {
        QVariant var = d_ptr->mRibbonTabBar->tabData(i);
        if (var.isValid()) {
            _SARibbonTabData p = var.value< _SARibbonTabData >();
            if (p.category == c) {
                return (i);
            }
        }
    }
    return (-1);
}

/**
 * @brief 移动一个Category从from index到to index
 * @param from
 * @param to
 */
void SARibbonBar::moveCategory(int from, int to)
{
    d_ptr->mRibbonTabBar->moveTab(from, to);
    // 这时要刷新所有tabdata的index信息
    d_ptr->updateTabData();
    // 这里会触发tabMoved信号，在tabMoved信号中调整stacked里窗口的位置
}

/**
 * @brief 获取当前显示的所有的SARibbonCategory，包含未显示的SARibbonContextCategory的SARibbonCategory也一并返回
 *
 * @return
 */
QList< SARibbonCategory* > SARibbonBar::categoryPages(bool getAll) const
{
    int c = d_ptr->mStackedContainerWidget->count();
    QList< SARibbonCategory* > res;

    for (int i = 0; i < c; ++i) {
        SARibbonCategory* w = qobject_cast< SARibbonCategory* >(d_ptr->mStackedContainerWidget->widget(i));
        if (w) {
            if (!getAll && w->isContextCategory()) {
                // 不是getall且是上下文时跳过
                continue;
            }
            res.append(w);
        }
    }
    return (res);
}

/**
 * @brief 移除SARibbonCategory
 *
 * SARibbonBar不会delete SARibbonCategory*，但这个SARibbonCategory会脱离SARibbonBar的管理
 * 表现在tabbar会移除，面板会移除，使用此函数后可以对SARibbonCategory进行delete
 * @param category
 */
void SARibbonBar::removeCategory(SARibbonCategory* category)
{
    int index     = tabIndex(category);
    bool isupdate = false;
    if (index >= 0) {
        d_ptr->mRibbonTabBar->removeTab(index);
        isupdate = true;
    }
    d_ptr->mStackedContainerWidget->removeWidget(category);
    // 同时验证这个category是否是contexcategory里的

    for (SARibbonContextCategory* c : qAsConst(d_ptr->mContextCategoryList)) {
        c->takeCategory(category);
    }
    // 这时要刷新所有tabdata的index信息
    if (isupdate) {
        d_ptr->updateTabData();
    }
    // 移除完后需要重新布局
    layout()->activate();
}

/**
 * @brief 添加上下文标签
 *
 * 上下文标签是特殊时候触发的标签，需要用户手动触发
 *
 * 调用@ref SARibbonContextCategory::addCategoryPage 可在上下文标签中添加SARibbonCategory，
 * 在上下文标签添加的SARibbonCategory，只有在上下文标签显示的时候才会显示
 * @param title 上下文标签的标题，在Office模式下会显示，在wps模式下不显示。默认情况下SARibbonContextCategory的object name也被设置为title
 * @param color 上下文标签的颜色，如果指定为空QColor(),将会使用SARibbonBar的默认色系
 * @param id 上下文标签的id，以便进行查找
 * @return 返回上下文标签指针
 * @note SARibbonBar拥有SARibbonContextCategory的管理权，用户避免在外部直接delete,如果要删除，调用@ref destroyContextCategory 函数
 */
SARibbonContextCategory* SARibbonBar::addContextCategory(const QString& title, const QColor& color, const QVariant& id)
{
    SARibbonContextCategory* context = RibbonSubElementFactory->createRibbonContextCategory(this);

    context->setObjectName(title);
    context->setContextTitle(title);
    context->setId(id);
    context->setContextColor(color.isValid() ? color : d_ptr->getContextCategoryColor());
    addContextCategory(context);
    return (context);
}

/**
 * @brief 添加上下文标签
 * @param context
 */
void SARibbonBar::addContextCategory(SARibbonContextCategory* context)
{
    if (nullptr == context) {
        return;
    }
    connect(context, &SARibbonContextCategory::categoryPageAdded, this, &SARibbonBar::onContextsCategoryPageAdded);
    connect(context, &SARibbonContextCategory::categoryTitleChanged, this, &SARibbonBar::onContextsCategoryCategoryNameChanged);
    // remove并没有绑定，主要是remove后在stacked里也不会显示，remove且delete的话，stacked里也会删除
    // 如果当前SARibbonContextCategory有category，先插入stackwidget中
    for (int i = 0; i < context->categoryCount(); ++i) {
        if (SARibbonCategory* category = context->categoryPage(i)) {
            d_ptr->mStackedContainerWidget->addWidget(category);
        }
    }
    d_ptr->mContextCategoryList.append(context);
}

/**
 * @brief 显示上下文标签
 * @param context 上下文标签指针
 */
void SARibbonBar::showContextCategory(SARibbonContextCategory* context)
{
    if (isContextCategoryVisible(context)) {
        return;
    }
    _SAContextCategoryManagerData contextCategoryData;

    contextCategoryData.contextCategory = context;
    for (int i = 0; i < context->categoryCount(); ++i) {
        SARibbonCategory* category = context->categoryPage(i);
        // 此句如果模式重复设置不会进行多余操作
        category->setPanelLayoutMode(d_ptr->mDefaulePanelLayoutMode);
        // 切换模式后会改变高度，上下文标签显示时要保证显示出来
        int index = d_ptr->mRibbonTabBar->addTab(category->categoryName());
        contextCategoryData.tabPageIndex.append(index);

        _SARibbonTabData tabdata;
        tabdata.category = category;
        tabdata.index    = index;
        d_ptr->mRibbonTabBar->setTabData(index, QVariant::fromValue(tabdata));
    }
    d_ptr->mCurrentShowingContextCategory.append(contextCategoryData);
    // 由于上下文都是在最后追加，不需要调用updateTabData();

    d_ptr->relayout();

    // 重新布局完后需要重绘,因为上下文标签涉及窗口的绘制
    update();
}

/**
 * @brief 隐藏上下文标签
 * @param context 上下文标签指针
 */
void SARibbonBar::hideContextCategory(SARibbonContextCategory* context)
{
    bool needResize = false;

    for (int i = 0; i < d_ptr->mCurrentShowingContextCategory.size(); ++i) {
        if (d_ptr->mCurrentShowingContextCategory[ i ].contextCategory == context) {
            const QList< int >& indexs = d_ptr->mCurrentShowingContextCategory[ i ].tabPageIndex;
            for (int j = indexs.size() - 1; j >= 0; --j) {
                d_ptr->mRibbonTabBar->removeTab(indexs[ j ]);
            }
            // 注意，再删除ContextCategory后，tab的序号就会改变，这时，这个tab后面的都要调整它的序号
            needResize = true;
            d_ptr->mCurrentShowingContextCategory.removeAt(i);
            break;
        }
    }
    if (needResize) {
        d_ptr->updateTabData();
        d_ptr->relayout();
        // 重新布局完后需要重绘
        update();
    }
}

/**
 * @brief 判断上下文是否在显示状态
 * @param context
 * @return 在显示状态返回true
 * @sa setContextCategoryVisible
 */
bool SARibbonBar::isContextCategoryVisible(SARibbonContextCategory* context)
{
    return (d_ptr->isContainContextCategoryInList(context));
}

/**
 * @brief 设置上下文标签的显示状态
 *
 * 上下文标签的当前显示状态可通过 @ref isContextCategoryVisible 进行判断
 * @param context 上下文标签
 * @param visible 显示状态，true为显示
 */
void SARibbonBar::setContextCategoryVisible(SARibbonContextCategory* context, bool visible)
{
    if (nullptr == context) {
        return;
    }
    if (visible) {
        showContextCategory(context);
    } else {
        hideContextCategory(context);
    }
}

/**
 * @brief 获取所有的上下文标签
 * @return 返回上下文标签列表
 */
QList< SARibbonContextCategory* > SARibbonBar::contextCategoryList() const
{
    return (d_ptr->mContextCategoryList);
}

/**
 * @brief 销毁上下文标签，上下文标签的SARibbonCategory也会随之销毁
 * @param context 需要销毁的上下文标签指针
 */
void SARibbonBar::destroyContextCategory(SARibbonContextCategory* context)
{
    if (nullptr == context) {
        return;
    }
    //! 1、如果上下文标签显示中，先隐藏
    if (isContextCategoryVisible(context)) {
        hideContextCategory(context);
    }
    //! 2、删除上下文标签的相关内容
    d_ptr->mContextCategoryList.removeAll(context);
    //!
    QList< SARibbonCategory* > res = context->categoryList();

    for (SARibbonCategory* c : qAsConst(res)) {
        c->hide();
        c->deleteLater();
    }
    context->deleteLater();
    d_ptr->relayout();
}

/**
 * @brief 获取当前可见的上下文标签的tab索引
 * @return 返回的索引是经过排序且去重的
 */
QList< int > SARibbonBar::currentVisibleContextCategoryTabIndexs() const
{
    QList< int > res;
    for (const _SAContextCategoryManagerData& data : qAsConst(d_ptr->mCurrentShowingContextCategory)) {
        res += data.tabPageIndex;
    }
    if (res.size() > 1) {
        // 1. 排序
        std::sort(res.begin(), res.end());
        // 2. 去重
        auto last = std::unique(res.begin(), res.end());
        res.erase(last, res.end());
    }
    return res;
}

/**
 * @brief 设置为最小/正常模式
 *
 * 隐藏模式下，只会显示tabbar，不会显示内容，默认状态是显示模式
 *
 * 默认下双击tabbar会切换隐藏显示模式，如果想禁用此功能，可以重载 @ref onCurrentRibbonTabDoubleClicked
 * 函数，不对函数进行任何处理即可
 *
 * @param isMinimum 参数为true时，切换为Minimum模式
 * @see 此函数会改变@ref RibbonState 状态，通过@ref currentRibbonState 函数可以查看当前状态
 */
void SARibbonBar::setMinimumMode(bool isMinimum)
{
#if SARIBBONBAR_DEBUG_PRINT
    qDebug() << "SARibbonBar::setHideMode " << isMinimum;
#endif
    if (isMinimum) {
        d_ptr->setMinimumMode();
    } else {
        d_ptr->setNormalMode();
    }
    // 发射信号
    Q_EMIT ribbonModeChanged(isMinimum ? MinimumRibbonMode : NormalRibbonMode);
}

///
/// \brief 当前ribbon是否是隐藏模式
/// \return
///
bool SARibbonBar::isMinimumMode() const
{
    return (d_ptr->mStackedContainerWidget->isPopupMode());
}

/**
 * @brief 显示隐藏ribbon的按钮
 * @param isShow
 */
void SARibbonBar::showMinimumModeButton(bool isShow)
{
    SA_D(d);
    if (isShow && !(d->mMinimumCategoryButtonAction)) {
        activeRightButtonGroup();

        d->mMinimumCategoryButtonAction = new QAction(this);
        d->mMinimumCategoryButtonAction->setIcon(style()->standardIcon(
            isMinimumMode() ? QStyle::SP_TitleBarUnshadeButton : QStyle::SP_TitleBarShadeButton, nullptr));
        connect(d->mMinimumCategoryButtonAction, &QAction::triggered, this, [ this, d ]() {
            this->setMinimumMode(!isMinimumMode());
            d->mMinimumCategoryButtonAction->setIcon(style()->standardIcon(
                isMinimumMode() ? QStyle::SP_TitleBarUnshadeButton : QStyle::SP_TitleBarShadeButton, nullptr));
        });
        if (d->mRightButtonGroup) {
            d->mRightButtonGroup->addAction(d->mMinimumCategoryButtonAction);
        }
    }

    d->mMinimumCategoryButtonAction->setVisible(isShow);
}

/**
 * @brief 是否显示隐藏ribbon按钮
 * @return
 */
bool SARibbonBar::haveShowMinimumModeButton() const
{
    return (nullptr != d_ptr->mMinimumCategoryButtonAction);
}

/**
@brief 隐藏ribbon对应的action
@return
*/
QAction* SARibbonBar::minimumModeAction() const
{
    return d_ptr->mMinimumCategoryButtonAction;
}

/**
 * @brief 是否允许tab双击后进入ribbon的最小模式
 * @return
 */
bool SARibbonBar::isEnableTabDoubleClickToMinimumMode() const
{
    return d_ptr->mEnableTabDoubleClickToMinimumMode;
}

/**
 * @brief 设置是否允许tab双击后，ribbon进入最小化模式，此属性默认开启
 * @param on
 */
void SARibbonBar::setTabDoubleClickToMinimumMode(bool on) const
{
    d_ptr->mEnableTabDoubleClickToMinimumMode = on;
}

/**
 * @brief 当前ribbon的状态（正常|最小化）
 * @return
 */
SARibbonBar::RibbonMode SARibbonBar::currentRibbonState() const
{
    return (d_ptr->mCurrentRibbonMode);
}

/**
@brief tabBar的高度
@return
*/
int SARibbonBar::tabBarHeight() const
{
    if (SARibbonBarLayout* lay = qobject_cast< SARibbonBarLayout* >(layout())) {
        return lay->tabBarHeight();
    }
    return -1;
}

/**
 * @brief 设置tabbar的高度
 *
 * 用户调用setTabBarHeight后，将使用用户设定的高度，而不使用自动计算的高度，这时tabbar高度不会跟随字体等信息重新计算
 *
 * @note 注意，在RibbonStyleCompact**模式下，tabbar高度要保证小于等于titlebar高度，否则会显示异常
 * @note 此函数不会自动刷新，如果需要刷新调用此函数后需要调用@ref updateRibbonGeometry
 * @param h
 */
void SARibbonBar::setTabBarHeight(int h, bool resizeByNow)
{
    if (SARibbonBarLayout* lay = qobject_cast< SARibbonBarLayout* >(layout())) {
        int oldHeight = lay->tabBarHeight();
        if (oldHeight == h) {
            return;
        }
        lay->setTabBarHeight(h);
        if (resizeByNow) {
            lay->resetSize();
        }
    }
}

/**
@brief 返回标题栏高度
@sa setTitleBarHeight
@return
*/
int SARibbonBar::titleBarHeight() const
{
    if (SARibbonBarLayout* lay = qobject_cast< SARibbonBarLayout* >(layout())) {
        return lay->titleBarHeight();
    }
    return -1;
}

/**
@brief 设置标题栏的高度
@sa titleBarHeight
@note 此操作会发射@ref titleBarHeightChanged 信号
@param h
*/
void SARibbonBar::setTitleBarHeight(int h, bool resizeByNow)
{
    if (SARibbonBarLayout* lay = qobject_cast< SARibbonBarLayout* >(layout())) {
        int oldHeight = lay->titleBarHeight();
        if (oldHeight == h) {
            return;
        }
        lay->setTitleBarHeight(h);
        if (resizeByNow) {
            lay->resetSize();
        }
        Q_EMIT titleBarHeightChanged(oldHeight, h);
    }
}

/**
 * @brief category的高度
 * @return
 */
int SARibbonBar::categoryHeight() const
{
    return d_ptr->mStackedContainerWidget->height();
}

/**
 * @brief 设置category的高度
 * @param h
 * @param resizeByNow
 */
void SARibbonBar::setCategoryHeight(int h, bool resizeByNow)
{
    if (SARibbonBarLayout* lay = qobject_cast< SARibbonBarLayout* >(layout())) {
        int oldHeight = lay->categoryHeight();
        if (oldHeight == h) {
            return;
        }
        lay->setCategoryHeight(h);
        if (resizeByNow) {
            lay->resetSize();
        }
    }
}

void SARibbonBar::onWindowTitleChanged(const QString& title)
{
    Q_UNUSED(title);
    update();
}

void SARibbonBar::onWindowIconChanged(const QIcon& i)
{
    if (SARibbonBarLayout* lay = qobject_cast< SARibbonBarLayout* >(layout())) {
        lay->setWindowIcon(i);
    }
}

/**
 * @brief category的名字发生改变触发
 * @param title
 */
void SARibbonBar::onCategoryWindowTitleChanged(const QString& title)
{
    // 全部更新一遍
    Q_UNUSED(title);
    updateCategoryTitleToTabName();
}

///
/// \brief ribbon的显示界面隐藏
///
void SARibbonBar::onStackWidgetHided()
{
}

/**
 * @brief 标签切换触发槽函数
 * @param index
 */
void SARibbonBar::onCurrentRibbonTabChanged(int index)
{
    QVariant var               = d_ptr->mRibbonTabBar->tabData(index);
    SARibbonCategory* category = nullptr;

    if (var.isValid()) {
        _SARibbonTabData p = var.value< _SARibbonTabData >();
        category           = p.category;
    }
    if (!category) {
        return;
    }
    if (d_ptr->mStackedContainerWidget->currentWidget() != category) {
        d_ptr->mStackedContainerWidget->setCurrentWidget(category);
    }
    Q_EMIT currentRibbonTabChanged(index);
    if (isMinimumMode()) {
        d_ptr->mRibbonTabBar->clearFocus();
        if (!d_ptr->mStackedContainerWidget->isVisible()) {
            if (d_ptr->mStackedContainerWidget->isPopupMode()) {
                // 在stackedContainerWidget弹出前，先给tabbar一个QHoverEvent,让tabbar知道鼠标已经移开
                QHoverEvent ehl(QEvent::HoverLeave,
                                d_ptr->mRibbonTabBar->mapToGlobal(QCursor::pos()),
                                d_ptr->mRibbonTabBar->mapToGlobal(QCursor::pos()));
                QApplication::sendEvent(d_ptr->mRibbonTabBar, &ehl);
                if (SARibbonBarLayout* lay = qobject_cast< SARibbonBarLayout* >(layout())) {
                    lay->layoutStackedContainerWidget();
                }
                d_ptr->mStackedContainerWidget->setFocus();
                // exec之前先发射信息号，否则会被exec阻塞
                d_ptr->mStackedContainerWidget->show();
            }
        }
    }
}

/**
 * @brief ribbon tab bar单击
 *
 * 此实现必须在eventfilter中传递stackedwidget的QEvent::MouseButtonDblClick事件到tabbar中，否则会导致双击变两次单击
 *
 * 单击事件仅用于响应点击同一个tab时
 * @param index
 */
void SARibbonBar::onCurrentRibbonTabClicked(int index)
{
    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    if (d_ptr->mTabBarLastClickTime > 0 && ((now - d_ptr->mTabBarLastClickTime) < QApplication::doubleClickInterval())) {
        return;
    }
    d_ptr->mTabBarLastClickTime = now;
    if (index != d_ptr->mRibbonTabBar->currentIndex()) {
        // 点击的标签不一致通过changed槽去处理
        return;
    }
    if (isMinimumMode()) {
        if (!d_ptr->mStackedContainerWidget->isVisible()) {
            if (d_ptr->mStackedContainerWidget->isPopupMode()) {
                qDebug() << "QHoverEvent";
                // 在stackedContainerWidget弹出前，先给tabbar一个QHoverEvent,让tabbar知道鼠标已经移开
                QHoverEvent ehl(QEvent::HoverLeave,
                                d_ptr->mRibbonTabBar->mapToGlobal(QCursor::pos()),
                                d_ptr->mRibbonTabBar->mapToGlobal(QCursor::pos()));
                QApplication::sendEvent(d_ptr->mRibbonTabBar, &ehl);
                // 弹出前都调整一下位置，避免移动后位置异常
                if (SARibbonBarLayout* lay = qobject_cast< SARibbonBarLayout* >(layout())) {
                    lay->layoutStackedContainerWidget();
                }
                d_ptr->mStackedContainerWidget->exec();
            }
        }
    }
}

/**
 * @brief ribbon tab bar双击
 *
 * 默认情况下双击会切换最小和正常模式
 * @param index
 */
void SARibbonBar::onCurrentRibbonTabDoubleClicked(int index)
{
    qDebug() << "onCurrentRibbonTabDoubleClicked";
    Q_UNUSED(index);
    d_ptr->mTabBarLastClickTime = QDateTime::currentMSecsSinceEpoch();  // 更新时间
    if (isEnableTabDoubleClickToMinimumMode()) {
        setMinimumMode(!isMinimumMode());
    }
}

void SARibbonBar::onContextsCategoryPageAdded(SARibbonCategory* category)
{
    Q_ASSERT_X(category != nullptr, "onContextsCategoryPageAdded", "add nullptr page");
    d_ptr->mStackedContainerWidget->addWidget(category);  // 这里stackedWidget用append，其他地方都应该使用insert
}

/**
 * @brief 上下文标签管理的标签的名字发生变换
 * @param category
 * @param title
 */
void SARibbonBar::onContextsCategoryCategoryNameChanged(SARibbonCategory* category, const QString& title)
{
    Q_UNUSED(category);
    Q_UNUSED(title);
    updateCategoryTitleToTabName();
}

/**
 * @brief 标签移动的信号
 * @param from
 * @param to
 */
void SARibbonBar::onTabMoved(int from, int to)
{
    const QSignalBlocker blocker(d_ptr->mStackedContainerWidget);
    // 调整stacked widget的顺序，调整顺序是为了调用categoryPages函数返回的QList<SARibbonCategory *>顺序和tabbar一致
    d_ptr->mStackedContainerWidget->moveWidget(from, to);
}

/**
 * @brief 根据SARibbonCategory*指针查找tabbar的index
 *
 * @param c SARibbonCategory对应的QObject指针
 * @return 如果没有找到，返回-1
 * @note 此函数不会调用SARibbonCategory*的任何方法，因此可以在SARibbonCategory的destroyed槽中调用
 */
int SARibbonBar::tabIndex(SARibbonCategory* obj)
{
    const int size = d_ptr->mRibbonTabBar->count();

    for (int i = 0; i < size; ++i) {
        QVariant v = d_ptr->mRibbonTabBar->tabData(i);
        if (v.isValid()) {
            _SARibbonTabData innervalue = v.value< _SARibbonTabData >();
            if (innervalue.category == obj) {
                return (i);
            }
        }
    }
    // 如果找不到就从stackedwidget中找

    return (-1);
}

/**
 * @brief 把ribbonbar的内容，同步进各个category中
 * @param autoUpdate
 */
void SARibbonBar::synchronousCategoryData(bool autoUpdate)
{
    iterateCategory([ this ](SARibbonCategory* c) -> bool {
        c->setEnableShowPanelTitle(this->isEnableShowPanelTitle());
        c->setPanelTitleHeight(this->panelTitleHeight());
        c->setCategoryAlignment(this->ribbonAlignment());
        c->setPanelLayoutMode(this->panelLayoutMode());
        return true;
    });
    if (autoUpdate) {
        d_ptr->relayout();
        if (SARibbonBarLayout* lay = qobject_cast< SARibbonBarLayout* >(layout())) {
            // 对category发送布局请求
            lay->layoutCategory();
        }
    }
}

/**
 * @brief 把窗口标题文字转换为显示的文字，这里主要针对[*]占位符进行替换
 * @param title
 * @return 如果存在[*]占位符，则根据父窗口的isWindowModified状态进行文字的替换，isWindowModified为true，[*]替换为*，isWindowModified为false，[*]替换为空
 */
QString SARibbonBar::toDisplayTitleText(const QString& title) const
{
    if (!title.contains("[*]")) {
        return title;
    }
    QWidget* w = parentWidget();
    if (!w) {
        return title;
    }
    QString res = title;
    if (w->isWindowModified()) {
        res.replace("[*]", "*");
    } else {
        res.replace("[*]", "");
    }
    return res;
}

/**
 * @brief 获取标题显示的区域
 * @return
 */
QRect SARibbonBar::getWindowTitleRect() const
{
    if (SARibbonBarLayout* lay = qobject_cast< SARibbonBarLayout* >(layout())) {
        return lay->titleRect();
    }
    return QRect();
}

/**
@brief 激活tabbar右边的按钮群
@return 返回右边的按钮群指针
*/
SARibbonButtonGroupWidget* SARibbonBar::activeRightButtonGroup()
{
    SA_D(d);
    if (!(d->mRightButtonGroup)) {
        d->createRightButton();
    }
    d->mRightButtonGroup->show();
    return d->mRightButtonGroup.data();
}

/**
@brief 返回右边的按钮群指针
@return 如果没有创建，返回nullptr
*/
SARibbonButtonGroupWidget* SARibbonBar::rightButtonGroup()
{
    return d_ptr->mRightButtonGroup.data();
}

/**
@brief 激活QuickAccessBar
@return
*/
SARibbonQuickAccessBar* SARibbonBar::activeQuickAccessBar()
{
    SARibbonQuickAccessBar* bar = quickAccessBar();
    if (!bar) {
        bar = d_ptr->createQuickAccessBar();
    }
    bar->show();
    return bar;
}

SARibbonQuickAccessBar* SARibbonBar::quickAccessBar()
{
    return (d_ptr->mQuickAccessBar.data());
}

/**
 * @brief 获取标题栏窗口
 * @return
 */
SARibbonTitleIconWidget* SARibbonBar::titleIconWidget() const
{
    return (d_ptr->mTitleIconWidget.data());
}

/**
 * @brief 设置标题图标可见性
 * @param on 是否可见
 */
void SARibbonBar::setTitleIconVisible(bool on)
{
    SARibbonTitleIconWidget* w = titleIconWidget();
    if (w) {
        w->setVisible(on);
    }
}

/**
 * @brief 标题图标是否可见
 * @return
 */
bool SARibbonBar::isTitleIconVisible() const
{
    SARibbonTitleIconWidget* w = titleIconWidget();
    if (w) {
        return w->isVisibleTo(this);
    }
    return false;
}

/**
 * @brief 设置ribbonbar的风格，此函数会重新设置所有元素，包括button的布局方式，
 * 尤其是从三行变到两行的过程，重设的内容较多
 * @note 此函数会自动触发ResizeEvent，不需要手动调用
 * @note 默认情况下2行模式不换行,如果想在两行模式换行，在调用SARibbonBar::setRibbonStyle后，再SARibbonToolButton::setEnableWordWrap(true)
 * ,同时再调用updateRibbonElementGeometry()刷新布局
 * @param v 样式，见@ref SARibbonBar::RibbonStyle
 */
void SARibbonBar::setRibbonStyle(SARibbonBar::RibbonStyles v)
{
    // 先幅值给变量
    d_ptr->mRibbonStyle = v;
#if SARIBBONBAR_DEBUG_PRINT
    qDebug() << "setRibbonStyle(" << v << ")"                //
             << "\n  isThreeRowStyle=" << isThreeRowStyle()  //
             << "\n  isTwoRowStyle=" << isTwoRowStyle()      //
             << "\n  isLooseStyle=" << isLooseStyle()        //
             << "\n  isCompactStyle=" << isCompactStyle()    //
        ;
#endif
    // 执行判断
    setEnableWordWrap(isThreeRowStyle(v));
    setTabOnTitle(isCompactStyle());
    setEnableShowPanelTitle(isThreeRowStyle(v));
    setPanelLayoutMode(isThreeRowStyle(v) ? SARibbonPanel::ThreeRowMode : SARibbonPanel::TwoRowMode);

    // 此函数会调用setFixedHeight
    synchronousCategoryData(false);  // 这里不急着刷新，下面会继续刷新
    if (SARibbonBarLayout* lay = qobject_cast< SARibbonBarLayout* >(layout())) {
        lay->resetSize();
    }

    Q_EMIT ribbonStyleChanged(d_ptr->mRibbonStyle);
}

/**
@brief 返回当前ribbon的风格
@return 返回当前ribbon的风格
*/
SARibbonBar::RibbonStyles SARibbonBar::currentRibbonStyle() const
{
    return (d_ptr->mRibbonStyle);
}

/**
@brief 切换到对应标签
@param index 标签索引
*/
void SARibbonBar::setCurrentIndex(int index)
{
    d_ptr->mRibbonTabBar->setCurrentIndex(index);
    // onCurrentRibbonTabChanged(index);
}

/**
 * @brief 返回当前的tab索引
 * @return 当前的索引
 */
int SARibbonBar::currentIndex()
{
    return (d_ptr->mRibbonTabBar->currentIndex());
}

/**
 * @brief 确保标签显示出来，tab并切换到对应页
 * @param category 标签指针
 */
void SARibbonBar::raiseCategory(SARibbonCategory* category)
{
    for (int i = 0; i < d_ptr->mRibbonTabBar->count(); ++i) {
        _SARibbonTabData p = d_ptr->mRibbonTabBar->tabData(i).value< _SARibbonTabData >();
        if (p.category == category) {
            d_ptr->mRibbonTabBar->setCurrentIndex(i);
            return;
        }
    }
}

/**
 * @brief 判断当前的样式是否为2行
 * @return
 */
bool SARibbonBar::isTwoRowStyle() const
{
    return (d_ptr->mDefaulePanelLayoutMode == SARibbonPanel::TwoRowMode);
}

/**
 * @brief 判断当前的样式是否为3行
 * @return
 */
bool SARibbonBar::isThreeRowStyle() const
{
    return (d_ptr->mDefaulePanelLayoutMode == SARibbonPanel::ThreeRowMode);
}

/**
 * @brief 判断当前的样式是否为宽松样式
 * @return
 */
bool SARibbonBar::isLooseStyle() const
{
    return (SARibbonBar::isLooseStyle(currentRibbonStyle()));
}

/**
 * @brief 判断当前的样式是否为紧凑样式
 * @return
 */
bool SARibbonBar::isCompactStyle() const
{
    return (SARibbonBar::isCompactStyle(currentRibbonStyle()));
}

/**
 * @brief 设置标题的文字颜色
 *
 * 标题是mainwindow的windowTitle，如果要设置标题，直接调用SARibbonMainWindow::setWindowTitle 进行设置
 *
 * 如果不设置标题颜色，默认是SARibbonBar的qss的color属性
 * @param clr
 * @note 此函数不会刷新，刷新请自行调用repaint
 */
void SARibbonBar::setWindowTitleTextColor(const QColor& clr)
{
    d_ptr->mTitleTextColor = clr;
}

/**
 * @brief 获取标题的文字颜色
 * @return 如果返回的是无效颜色，!QColor::isValid()，说明没有手动设置颜色，颜色将跟随SARibbonBar的qss的文字颜色
 */
QColor SARibbonBar::windowTitleTextColor() const
{
    return d_ptr->mTitleTextColor;
}

/**
 * @brief tabbar 底部会绘制一条线条，此接口定义线条颜色
 * @param clr
 */
void SARibbonBar::setTabBarBaseLineColor(const QColor& clr)
{
    d_ptr->mTabBarBaseLineColor = clr;
}

/**
 * @brief 获取tabbar底部基线颜色
 * @return
 */
QColor SARibbonBar::tabBarBaseLineColor() const
{
    return d_ptr->mTabBarBaseLineColor;
}

/**
 * @brief 更新ribbon的布局数据，此函数适用于一些关键性尺寸变化，换起ribbon下面元素的布局
 *
 * @note 此函数调用较慢，避免在高速要求场合调用
 */
void SARibbonBar::updateRibbonGeometry()
{
    if (SARibbonBarLayout* lay = qobject_cast< SARibbonBarLayout* >(layout())) {
        lay->resetSize();
    }
    iterateCategory([](SARibbonCategory* c) -> bool {
        c->updateItemGeometry();
        return true;
    });
}

/**
 * @brief SARibbonPanel的布局模式
 * @return
 */
SARibbonPanel::PanelLayoutMode SARibbonBar::panelLayoutMode() const
{
    return d_ptr->mDefaulePanelLayoutMode;
}

/**
 * @brief SARibbonPanel的布局模式设置
 * @param m
 */
void SARibbonBar::setPanelLayoutMode(SARibbonPanel::PanelLayoutMode m)
{
    d_ptr->mDefaulePanelLayoutMode = m;
    // 设置布局时，让布局重新计算高度
    if (SARibbonBarLayout* lay = qobject_cast< SARibbonBarLayout* >(layout())) {
        lay->resetSize();
    }
    iterateCategory([ m ](SARibbonCategory* c) -> bool {
        c->setPanelLayoutMode(m);
        return true;
    });
}

/**
 * @brief 设置tab在title上面，这样可以省略title区域
 * @param on
 */
void SARibbonBar::setTabOnTitle(bool on)
{
    if (SARibbonBarLayout* lay = qobject_cast< SARibbonBarLayout* >(layout())) {
        lay->setTabOnTitle(on);
    }
}

/**
 * @brief tab是否在title上面
 * @return
 */
bool SARibbonBar::isTabOnTitle() const
{
    if (SARibbonBarLayout* lay = qobject_cast< SARibbonBarLayout* >(layout())) {
        return lay->isTabOnTitle();
    }
    return false;
}

/**
 * @brief 设置标题的对齐方式
 * @param al
 */
void SARibbonBar::setWindowTitleAligment(Qt::Alignment al)
{
    d_ptr->mTitleAligment = al;
    update();
}

/**
 * @brief 获取标题的对齐方式
 * @return
 */
Qt::Alignment SARibbonBar::windowTitleAligment() const
{
    return d_ptr->mTitleAligment;
}

/**
 * @brief 设置ribbonbar的按钮文字允许换行
 * @param on
 */
void SARibbonBar::setEnableWordWrap(bool on)
{
    d_ptr->mEnableWordWrap = on;
    iterateCategory([ on ](SARibbonCategory* category) -> bool {
        if (category) {
            category->setEnableWordWrap(on);
        }
        return true;
    });
    updateGeometry();
}

/**
 * @brief 判断是否允许换行
 * @return
 */
bool SARibbonBar::isEnableWordWrap() const
{
    return d_ptr->mEnableWordWrap;
}

/**
 * @brief 设置按钮最大宽高比，这个系数决定按钮的最大宽度
 *
 * 按钮的最大宽度为按钮高度*此系数，例如按钮高度为h，那么按钮最大宽度maxw=h*buttonMaximumAspectRatio
 * 如果在此宽度下文字还无法完全显示，那么按钮将不会继续横向扩展，将使用...替代未完全显示的文字
 *
 * @see buttonMaximumAspectRatio
 */
void SARibbonBar::setButtonMaximumAspectRatio(qreal fac)
{
    d_ptr->buttonMaximumAspectRatio = fac;
    iterateCategory([ fac ](SARibbonCategory* category) -> bool {
        if (category) {
            category->setButtonMaximumAspectRatio(fac);
        }
        return true;
    });
}

/**
 * @brief 按钮最大宽高比，这个系数决定按钮的最大宽度
 * @return 按钮最大宽高比
 * @see setButtonMaximumAspectRatio
 */
qreal SARibbonBar::buttonMaximumAspectRatio() const
{
    return d_ptr->buttonMaximumAspectRatio;
}

/**
 * @brief panel标题栏的高度
 * @return
 */
int SARibbonBar::panelTitleHeight() const
{
    if (SARibbonBarLayout* lay = qobject_cast< SARibbonBarLayout* >(layout())) {
        return lay->panelTitleHeight();
    }
    return -1;
}

/**
 * @brief 设置panel的高度
 * @param h
 */
void SARibbonBar::setPanelTitleHeight(int h)
{
    if (SARibbonBarLayout* lay = qobject_cast< SARibbonBarLayout* >(layout())) {
        lay->setPanelTitleHeight(h);
    }
    iterateCategory([ h ](SARibbonCategory* c) -> bool {
        c->setPanelTitleHeight(h);
        return true;
    });
}

/**
 * @brief 是否panel显示标题栏
 * @return
 */
bool SARibbonBar::isEnableShowPanelTitle() const
{
    return d_ptr->mEnableShowPanelTitle;
}

/**
 * @brief 设置显示panel标题
 * @param on
 */
void SARibbonBar::setEnableShowPanelTitle(bool on)
{
    d_ptr->mEnableShowPanelTitle = on;
    iterateCategory([ on ](SARibbonCategory* c) -> bool {
        c->setEnableShowPanelTitle(on);
        return true;
    });
}

/**
 * @brief 设置panel的spacing
 * @param n
 */
void SARibbonBar::setPanelSpacing(int n)
{
    d_ptr->mPanelSpacing = n;
    // 同步当前被SARibbonBar管理的SARibbonCategory的PanelSpacing
    iterateCategory([ n ](SARibbonCategory* category) -> bool {
        if (category) {
            category->setPanelSpacing(n);
        }
        return true;
    });
}

/**
 * @brief panel的spacing
 * @return
 */
int SARibbonBar::panelSpacing() const
{
    return d_ptr->mPanelSpacing;
}

/**
 * @brief 设置panel按钮的icon尺寸，large action不受此尺寸影响
 * @param smallSize
 */
void SARibbonBar::setPanelToolButtonIconSize(const QSize& smallSize, const QSize& largeSize)
{
    d_ptr->mPanelSmallToolButtonSize = smallSize;
    d_ptr->mPanelLargeToolButtonSize = largeSize;
    // 同步当前被SARibbonBar管理的SARibbonCategory的PanelSpacing
    iterateCategory([ smallSize, largeSize ](SARibbonCategory* category) -> bool {
        if (category) {
            category->setPanelToolButtonIconSize(smallSize, largeSize);
        }
        return true;
    });
}

/**
 * @brief panel按钮的icon尺寸，large action不受此尺寸影响
 *
 * @note panel按钮是指panel右下角的功能按钮
 * @return
 */
QPair< QSize, QSize > SARibbonBar::panelToolButtonIconSize() const
{
    return qMakePair(d_ptr->mPanelSmallToolButtonSize, d_ptr->mPanelLargeToolButtonSize);
}

/**
 * @brief 设置panel的大按钮图标尺寸
 * @param largeSize
 */
void SARibbonBar::setPanelLargeIconSize(const QSize& largeSize)
{
    d_ptr->mPanelLargeToolButtonSize = largeSize;
    // 同步当前被SARibbonBar管理的SARibbonCategory的PanelSpacing
    iterateCategory([ largeSize ](SARibbonCategory* category) -> bool {
        if (category) {
            category->setPanelLargeIconSize(largeSize);
        }
        return true;
    });
}

/**
 * @brief panel的大按钮图标尺寸
 * @return
 */
QSize SARibbonBar::panelLargeIconSize() const
{
    return d_ptr->mPanelLargeToolButtonSize;
}

/**
 * @brief 设置panel的小按钮图标尺寸
 * @param smallSize
 */
void SARibbonBar::setPanelSmallIconSize(const QSize& smallSize)
{
    d_ptr->mPanelSmallToolButtonSize = smallSize;
    // 同步当前被SARibbonBar管理的SARibbonCategory的PanelSpacing
    iterateCategory([ smallSize ](SARibbonCategory* category) -> bool {
        if (category) {
            category->setPanelSmallIconSize(smallSize);
        }
        return true;
    });
}

/**
 * @brief panel的小按钮图标尺寸
 * @return
 */
QSize SARibbonBar::panelSmallIconSize() const
{
    return d_ptr->mPanelSmallToolButtonSize;
}

/**
 * @brief ribbonbar内部的StackedWidget
 * 所有的category都放置在StackedWidget中
 * @return
 */
SARibbonStackedWidget* SARibbonBar::ribbonStackedWidget()
{
    return d_ptr->mStackedContainerWidget;
}

/**
 * @brief 设置是否显示标题
 * @param on
 */
void SARibbonBar::setTitleVisible(bool on)
{
    d_ptr->mIsTitleVisible = on;
}

/**
 * @brief 判断标题是否显示
 * @return
 */
bool SARibbonBar::isTitleVisible() const
{
    return d_ptr->mIsTitleVisible;
}

/**
 * @brief 设置标题栏的背景颜色
 *
 * 如果不需要颜色，设置Qt::NoBrush
 * @param bk 背景颜色
 */
void SARibbonBar::setWindowTitleBackgroundBrush(const QBrush& bk)
{
    d_ptr->mTitleBackgroundBrush = bk;
}

/**
 * @brief 标题栏的背景颜色
 *
 * 颜色为Qt::NoBrush时，代表没有背景颜色
 * @default Qt::NoBrush
 * @return
 */
QBrush SARibbonBar::windowTitleBackgroundBrush() const
{
    return d_ptr->mTitleBackgroundBrush;
}

/**
@brief 设置上下文标签的颜色列表

上下文标签显示的时候，会从颜色列表中取颜色进行标签的渲染
@param cls
*/
void SARibbonBar::setContextCategoryColorList(const QList< QColor >& cls)
{
    d_ptr->mContextCategoryColorList = cls;
    if (d_ptr->mContextCategoryColorList.isEmpty()) {
        d_ptr->mContextCategoryColorList = defaultContextCategoryColorList();
    }
    d_ptr->mContextCategoryColorListIndex = 0;
    // 这时需要对已经显示的contextCategoryData的颜色进行重新设置
    for (SARibbonContextCategory* c : qAsConst(d_ptr->mContextCategoryList)) {
        c->setContextColor(d_ptr->getContextCategoryColor());
    }
}

/**
@brief 获取上下文标签的颜色列表
@return
*/
QList< QColor > SARibbonBar::contextCategoryColorList() const
{
    return d_ptr->mContextCategoryColorList;
}

/**
 * @brief 设置contextCategory 标题的颜色
 * @param clr
 */
void SARibbonBar::setContextCategoryTitleTextColor(const QColor& clr)
{
    d_ptr->mContextCategoryTitleTextColor = clr;
}

/**
 * @brief contextCategory 标题的颜色
 * @return
 */
QColor SARibbonBar::contextCategoryTitleTextColor() const
{
    return d_ptr->mContextCategoryTitleTextColor;
}

/**
 * @brief 设置一个函数指针，函数指针输入上下文标签设定的颜色，输出一个高亮颜色，高亮颜色用于绘制上下文标签的高亮部位，例如最顶部的横线
 * @param fp
 */
void SARibbonBar::setContextCategoryColorHighLight(FpContextCategoryHighlight fp)
{
    d_ptr->mFpContextHighlight = fp;
}

/**
@brief 设置ribbon的对齐方式
@param al
*/
void SARibbonBar::setRibbonAlignment(SARibbonAlignment al)
{
    d_ptr->mRibbonAlignment = al;
    synchronousCategoryData();
}

/**
@brief ribbon的对齐方式
@return
*/
SARibbonAlignment SARibbonBar::ribbonAlignment() const
{
    return d_ptr->mRibbonAlignment;
}

/**
 * @brief 此函数会遍历SARibbonBar下的所有Category，执行函数指针bool(SARibbonCategory*)
 * @param fp 函数指针返回false则停止迭代
 * @return 返回false代表没有迭代完所有的category，中途接收到回调函数的false返回而中断迭代
 */
bool SARibbonBar::iterateCategory(FpCategoryIterate fp) const
{
    const QList< SARibbonCategory* > cs = categoryPages(true);
    for (SARibbonCategory* c : cs) {
        if (!fp(c)) {
            return false;
        }
    }
    return true;
}

/**
 * @brief 此函数会遍历SARibbonBar下的所有Category,并迭代所有的panel，执行函数指针bool(SARibbonPanel*)
 * @param fp 函数指针返回false则停止迭代
 * @return 返回false代表没有迭代完所有的category，中途接收到回调函数的false返回而中断迭代
 */
bool SARibbonBar::iteratePanel(FpPanelIterate fp) const
{
    const QList< SARibbonCategory* > cs = categoryPages(true);
    for (SARibbonCategory* c : cs) {
        if (!c->iteratePanel(fp)) {
            return false;
        }
    }
    return true;
}

/**
 * @brief 设置边角widget可见性，对于mdi窗口，会出现TopLeftCorner和TopRightCorner两个corner widget
 * @param on
 * @param c
 */
void SARibbonBar::setCornerWidgetVisible(bool on, Qt::Corner c)
{
    if (QWidget* w = cornerWidget(c)) {
        w->setVisible(on);
    }
}

/**
 * @brief 设置ApplicationButton垂直方向扩充，这样ApplicationButton能占用标题栏和tab栏两个栏的高度
 * @param on
 */
void SARibbonBar::setApplicationButtonVerticalExpansion(bool on)
{
    if (SARibbonBarLayout* lay = qobject_cast< SARibbonBarLayout* >(layout())) {
        lay->setApplicationButtonVerticalExpansion(on);
        lay->invalidate();
    }
}

/**
 * @brief applicationButton是否是在垂直方向扩充
 *
 * 默认为false
 * @return
 */
bool SARibbonBar::isApplicationButtonVerticalExpansion() const
{
    if (SARibbonBarLayout* lay = qobject_cast< SARibbonBarLayout* >(layout())) {
        return lay->isApplicationButtonVerticalExpansion();
    }
    return false;
}

/**
 * @brief 此函数会遍历所有panel，并获取它下面的action
 * @return
 */
QList< QAction* > SARibbonBar::allActions() const
{
    QList< QAction* > res;
    auto fp = [ &res ](SARibbonPanel* panel) -> bool {
        res += panel->actions();
        return true;
    };
    iteratePanel(fp);
    return res;
}

/**
 * @brief 判断当前是否使用的是无边框，而不是native边框
 * @return
 */
bool SARibbonBar::isUseRibbonFrame() const
{
    return d_ptr->isUseRibbonFrame();
}

/**
 * @brief SARibbonBar::eventFilter
 * @param obj
 * @param e
 * @return
 */
bool SARibbonBar::eventFilter(QObject* obj, QEvent* e)
{
    if (obj) {
        // 调整多文档时在窗口模式下的按钮更新
        if ((obj == cornerWidget(Qt::TopLeftCorner)) || (obj == cornerWidget(Qt::TopRightCorner))) {
            if ((QEvent::UpdateLater == e->type()) || (QEvent::MouseButtonRelease == e->type())
                || (QEvent::WindowActivate == e->type())) {
                // 这个是多文档系统按钮的更新
                d_ptr->relayout();
            }
        } else if (obj == d_ptr->mStackedContainerWidget) {
            // 在stack 是popup模式时，点击的是stackedContainerWidget区域外的时候，如果是在ribbonTabBar上点击
            // 那么忽略掉这次点击，把点击下发到ribbonTabBar,这样可以避免stackedContainerWidget在点击ribbonTabBar后
            // 隐藏又显示，出现闪烁
            if ((QEvent::MouseButtonPress == e->type()) || (QEvent::MouseButtonDblClick == e->type())) {
                if (d_ptr->mStackedContainerWidget->isPopupMode()) {
                    QMouseEvent* mouseEvent = static_cast< QMouseEvent* >(e);
                    if (!d_ptr->mStackedContainerWidget->rect().contains(mouseEvent->pos())) {
                        QWidget* clickedWidget = QApplication::widgetAt(mouseEvent->globalPos());
                        if (clickedWidget == d_ptr->mRibbonTabBar) {
                            const QPoint targetPoint = clickedWidget->mapFromGlobal(mouseEvent->globalPos());
                            QMouseEvent* evPress     = new QMouseEvent(mouseEvent->type(),
                                                                   targetPoint,
                                                                   mouseEvent->globalPos(),
                                                                   mouseEvent->button(),
                                                                   mouseEvent->buttons(),
                                                                   mouseEvent->modifiers());
                            QApplication::postEvent(clickedWidget, evPress);
                            return (true);
                        }
                    }
                }
            }
        }
    }
    return (QMenuBar::eventFilter(obj, e));
}

/**
 * @brief 根据情况重置tabbar的宽度，主要针对wps模式
 */
int SARibbonBar::calcMinTabBarWidth() const
{
    // 20200831
    // tabBarWidth的宽度原来为endX - x;，现需要根据实际进行调整
    // 为了把tabbar没有tab的部分不占用，这里的宽度需要根据tab的size来进行设置，让tabbar的长度刚刚好，这样能让出
    // mainwindow的空间，接受鼠标事件，从而实现拖动等操作，否则tabbar占用整个顶栏，鼠标无法点击到mainwindow
    // 计算tab所占用的宽度
    const QMargins& mg = d_ptr->mRibbonTabBar->tabMargin();
    return d_ptr->mRibbonTabBar->sizeHint().width() + (mg.left() + mg.right());
}

/**
 * @brief 正常模式下的高度
 *
 * 有可能SARibbonBar::height和mainBarHeight不相等，这种情况发生在RibbonState::MinimumRibbonMode状态下
 * @return 高度
 */
int SARibbonBar::normalModeMainBarHeight() const
{
    if (SARibbonBarLayout* lay = qobject_cast< SARibbonBarLayout* >(layout())) {
        return lay->normalModeMainBarHeight();
    }
    return -1;
}

/**
@brief 最小模式下的高度
@return
*/
int SARibbonBar::minimumModeMainBarHeight() const
{
    if (SARibbonBarLayout* lay = qobject_cast< SARibbonBarLayout* >(layout())) {
        return lay->minimumModeMainBarHeight();
    }
    return -1;
}

/**
 * @brief 更新所有的category title对应的tab名
 *
 * 此函数会对所有的category的名字和tab进行匹配，如果匹配不上会重新设置tab名
 */
void SARibbonBar::updateCategoryTitleToTabName()
{
    SARibbonTabBar* tab = d_ptr->mRibbonTabBar;
    for (int i = 0; i < tab->count(); ++i) {
        // 鉴于tab不会很多，不考虑效率问题
        QVariant var = tab->tabData(i);
        if (var.isValid()) {
            _SARibbonTabData p = var.value< _SARibbonTabData >();
            if (p.category && p.category->categoryName() != tab->tabText(i)) {
                tab->setTabText(i, p.category->categoryName());
            }
        }
    }
    repaint();
}

/**
 * @brief 告知WindowButtonGroup的尺寸
 * @param s
 */
void SARibbonBar::setSystemButtonGroupSize(const QSize& s)
{
    if (SARibbonBarLayout* lay = qobject_cast< SARibbonBarLayout* >(layout())) {
        lay->setSystemButtonSize(s);
    }
}

/**
 * @brief 更新标题栏的区域位置
 */
// void SARibbonBar::updateTitleRect()
// {
// 	if (!d_ptr->isUseRibbonFrame()) {
// 		d_ptr->mTitleRect = QRect();
// 	}
// 	SARibbonQuickAccessBar* quickAccessBar = d_ptr->mQuickAccessBar.data();
// 	SARibbonTabBar* ribbonTabBar           = d_ptr->mRibbonTabBar.data();
// 	const QMargins border                  = contentsMargins();
// 	const int validTitleBarHeight          = titleBarHeight();
// 	// 计算标题栏区域
// 	if (isCompactStyle()) {
// 		// 两行紧凑模式
// 		int titleStart = ribbonTabBar->geometry().right();
// 		int titleWidth = quickAccessBar->x() - titleStart;
// 		if (titleWidth > 10) {
// 			d_ptr->mTitleRect = QRect(titleStart, border.top(), titleWidth, validTitleBarHeight);
// 		} else {
// 			d_ptr->mTitleRect = QRect();
// 		}
// 	} else {
// 		// 三行宽松模式
// 		const QList< _SAContextCategoryManagerData >& contextCategoryDataList = d_ptr->mCurrentShowingContextCategory;
// 		// 记录当前上下文标签的最右和最左端
// 		int contextRegionLeft  = width();
// 		int contextRegionRight = -1;
// 		for (const _SAContextCategoryManagerData& contextData : contextCategoryDataList) {
// 			QRect contextTitleRect = d_ptr->calcContextCategoryTitleRect(contextData);
// 			// 更新上下文标签的范围，用于控制标题栏的显示
// 			if (contextTitleRect.left() < contextRegionLeft) {
// 				contextRegionLeft = contextTitleRect.left();
// 			}
// 			if (contextTitleRect.right() > contextRegionRight) {
// 				contextRegionRight = contextTitleRect.right();
// 			}
// 		}
// 		int x1 = border.left();
// 		if (d_ptr->mQuickAccessBar) {
// 			x1 = d_ptr->mQuickAccessBar->geometry().right();
// 			x1 += 1;
// 		}
// 		int x2 = width() - d_ptr->mSystemButtonSize.width() - border.right();
// 		if (contextRegionRight < 0) {
// 			//!说明没有上下文标签，contextRegionRight就是默认值-1

// 			d_ptr->mTitleRect = QRect(QPoint(x1, border.top()), QPoint(x2, validTitleBarHeight + border.top()));
// 		} else {
// 			//! 说明有上下文标签，上下文标签会把标题区域切分为两部分，这时候要找出一个更大的标题区域显示标题
// 			int leftwidth  = contextRegionLeft - x1;   // 计算出上下文标签左边标题栏的宽度
// 			int rightwidth = x2 - contextRegionRight;  // 计算出上下文标签右边标题栏的宽度
// 			if (rightwidth > leftwidth) {
// 				// 说明右边的区域大一点，标题显示在右，显示在右边需要减去windowbutton宽度
// 				d_ptr->mTitleRect =
// 					QRect(QPoint(contextRegionRight, border.top()), QPoint(x2, validTitleBarHeight + border.top()));
// 			} else {
// 				// 说明左边的大一点
// 				d_ptr->mTitleRect =
// 					QRect(QPoint(x1, border.top()), QPoint(contextRegionLeft, validTitleBarHeight + border.top()));
// 			}
// 		}
// 	}
// }

/**
 * @brief SARibbonBar::setMainWindowStyles
 * @param s
 */
void SARibbonBar::setMainWindowStyles(SARibbonMainWindowStyles s)
{
    d_ptr->mMainWindowStyle = s;
}

void SARibbonBar::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);
    if (isLooseStyle()) {
        paintInLooseStyle();
    } else {
        paintInCompactStyle();
    }
#if SARIBBONBAR_DEBUG_PRINT
    QPainter p(this);
    if (d_ptr->mQuickAccessBar) {
        SARIBBONBAR_HELP_DRAW_RECT(p, d_ptr->mQuickAccessBar->geometry());
    }
    if (d_ptr->mRibbonTabBar) {
        SARIBBONBAR_HELP_DRAW_RECT(p, d_ptr->mRibbonTabBar->geometry());
    }
    if (d_ptr->mStackedContainerWidget) {
        SARIBBONBAR_HELP_DRAW_RECT(p, d_ptr->mStackedContainerWidget->geometry());
    }
    if (d_ptr->mRightButtonGroup) {
        SARIBBONBAR_HELP_DRAW_RECT(p, d_ptr->mRightButtonGroup->geometry());
    }
#endif
}

void SARibbonBar::paintInLooseStyle()
{
    QPainter p(this);

    //! 1.绘制tabbar下的基线，这个函数仅仅对office2013主题有用，大部分主题都不绘制基线
    paintTabbarBaseLine(p);

    //! 2.显示上下文标签
    const QList< _SAContextCategoryManagerData >& contextCategoryDataList = d_ptr->mCurrentShowingContextCategory;
    for (const _SAContextCategoryManagerData& contextData : contextCategoryDataList) {
        QRect contextTitleRect = d_ptr->calcContextCategoryTitleRect(contextData);
        if (!contextData.tabPageIndex.isEmpty()) {
            // 绘制
            paintContextCategoryTab(p,
                                    contextData.contextCategory->contextTitle(),
                                    contextTitleRect,
                                    contextData.contextCategory->contextColor());
        }
    }

    //! 3.显示标题等
    if (d_ptr->isUseRibbonFrame()) {
        QWidget* parWindow = parentWidget();
        if (parWindow && isTitleVisible()) {
            paintWindowTitle(p, toDisplayTitleText(parWindow->windowTitle()), getWindowTitleRect());
        }
    }
}

void SARibbonBar::paintInCompactStyle()
{
    QPainter p(this);

    //! 1.绘制tabbar下的基线，这个函数仅仅对office2013主题有用，大部分主题都不绘制基线
    paintTabbarBaseLine(p);

    //! 显示上下文标签

    const QList< _SAContextCategoryManagerData >& contextCategoryDataList = d_ptr->mCurrentShowingContextCategory;
    for (const _SAContextCategoryManagerData& contextData : contextCategoryDataList) {
        QRect contextTitleRect = d_ptr->calcContextCategoryTitleRect(contextData);
        if (!contextData.tabPageIndex.isEmpty()) {
            // 绘制
            paintContextCategoryTab(p,
                                    contextData.contextCategory->contextTitle(),
                                    contextTitleRect,
                                    contextData.contextCategory->contextColor());
        }
    }

    //! 显示标题等
    if (d_ptr->isUseRibbonFrame()) {
        QWidget* parWindow = parentWidget();
        if (parWindow && isTitleVisible()) {
            paintWindowTitle(p, toDisplayTitleText(parWindow->windowTitle()), getWindowTitleRect());
        }
    }
}

// void SARibbonBar::resizeStackedContainerWidget()
// {
// 	QMargins border                   = contentsMargins();
// 	const QRect& ribbonTabBarGeometry = d_ptr->mRibbonTabBar->geometry();

// 	int x = border.left();
// 	int y = ribbonTabBarGeometry.bottom() + 1;
// 	int w = width() - border.left() - border.right();
// 	int h = d_ptr->categoryHeight();
// 	if (d_ptr->mStackedContainerWidget->isPopupMode()) {
// 		// 弹出模式时，位置为全局位置
// 		QPoint absPosition = mapToGlobal(QPoint(x, y));
// 		x                  = absPosition.x();
// 		y                  = absPosition.y();
// 	}
// 	d_ptr->mStackedContainerWidget->setFixedSize(QSize(w, h));
// 	d_ptr->mStackedContainerWidget->setGeometry(x, y, w, h);
// }

/**
 * @brief 刷新所有ContextCategoryManagerData，这个在单独一个Category删除时调用
 */
void SARibbonBar::updateContextCategoryManagerData()
{
    const int c = d_ptr->mRibbonTabBar->count();

    for (_SAContextCategoryManagerData& cd : d_ptr->mCurrentShowingContextCategory) {
        cd.tabPageIndex.clear();
        for (int i = 0; i < cd.contextCategory->categoryCount(); ++i) {
            SARibbonCategory* category = cd.contextCategory->categoryPage(i);
            for (int t = 0; t < c; ++t) {
                QVariant v = d_ptr->mRibbonTabBar->tabData(t);
                if (v.isValid()) {
                    _SARibbonTabData d = v.value< _SARibbonTabData >();
                    if (d.category == category) {
                        cd.tabPageIndex.append(t);
                    }
                } else {
                    cd.tabPageIndex.append(-1);
                }
            }
        }
    }
}

/**
 * @brief 绘制上下文标签的背景
 * @param painter 绘图QPainter
 * @param title 上下文标签的title
 * @param contextRect 上下文标签的绘制区域
 * @param color 上下文标签赋予的颜色
 */
void SARibbonBar::paintContextCategoryTab(QPainter& painter, const QString& title, const QRect& contextRect, const QColor& color)
{
    // 绘制上下文标签
    // 首先有5像素的实体粗线位于顶部
    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    painter.drawRect(contextRect);
    const int contextLineWidth = 5;
    // 绘制很线
    QColor highlightColor = color;
    if (d_ptr->mFpContextHighlight) {
        highlightColor = d_ptr->mFpContextHighlight(color);
    }
    painter.fillRect(QRect(contextRect.x(), contextRect.y(), contextRect.width(), contextLineWidth), highlightColor);

    // 只有在office模式下才需要绘制标题
    if (isLooseStyle()) {
        if (!title.isEmpty()) {
            QRect textRect = QRect(contextRect.x(),
                                   contextRect.y() + contextLineWidth,
                                   contextRect.width(),
                                   contextRect.height() - contextLineWidth - d_ptr->mRibbonTabBar->height());
            painter.setPen(contextCategoryTitleTextColor());
            QFontMetrics fontMetrics(painter.font());
            painter.drawText(textRect, Qt::AlignCenter, fontMetrics.elidedText(title, Qt::ElideRight, textRect.width()));
        }
    }
    painter.restore();
}

/**
 * @brief 重写moveevent是为了在移动时调整isPopupMode状态下的stackedContainerWidget位置
 * @param event
 */
void SARibbonBar::moveEvent(QMoveEvent* e)
{
    if (d_ptr->mStackedContainerWidget) {
        if (d_ptr->mStackedContainerWidget->isPopupMode()) {
            // 弹出模式时，窗口发生了移动，同步调整StackedContainerWidget的位置
            if (SARibbonBarLayout* lay = qobject_cast< SARibbonBarLayout* >(layout())) {
                lay->layoutStackedContainerWidget();
            }
        }
    }
    QMenuBar::moveEvent(e);
}

/**
 * @brief 跟踪字体改变
 * @param event
 */
void SARibbonBar::changeEvent(QEvent* e)
{
    if (nullptr == e) {
        return;
    }
    switch (e->type()) {
    case QEvent::FontChange: {
        QFont f                       = font();
        QList< QWidget* > listWidgets = findChildren< QWidget* >();
        for (QWidget* w : listWidgets) {
            w->setFont(f);
        }
        // 字体改变总体布局调整
        if (SARibbonBarLayout* lay = qobject_cast< SARibbonBarLayout* >(layout())) {
            lay->resetSize();
        }
    } break;
    case QEvent::StyleChange: {
        updateRibbonGeometry();
    } break;
    case QEvent::ParentChange: {
        //! 这种是针对先new 一个对象，再设置到MainWindow的情况，例如
        //! SARibbonBar* ribbon = new SARibbonBar();
        //! mainwinodw->setRibbonBar(ribbon);
        //!
        //! 这种方式，构造的时候由于没有设置父窗口，因此，如果在构造函数绑定信号槽就有可能绑定不上（parent为空）
        //! 所以在事件里绑定
        d_ptr->initNewParent(parentWidget());
    } break;
    default:
        break;
    }
    QMenuBar::changeEvent(e);
}

/**
 * @brief 绘制tabbar下的基准线，这个方法仅仅在office2013模式下需要
 * @param painter
 */
void SARibbonBar::paintTabbarBaseLine(QPainter& painter)
{
    if (!d_ptr->mTabBarBaseLineColor.isValid()) {
        return;
    }
    painter.save();
    // 在tabbar下绘制一条线
    const int lineY = d_ptr->mRibbonTabBar->geometry().bottom();
    QPen pen(d_ptr->mTabBarBaseLineColor);
    QMargins border = contentsMargins();

    pen.setWidth(1);
    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);
    painter.drawLine(QPoint(border.left(), lineY), QPoint(width() - border.right() - 1, lineY));
    painter.restore();
}

///
/// \brief 绘制标题栏
/// \param painter
/// \param title 标题
/// \param contextCategoryRegion 当前显示的上下文标签的范围，上下文标签是可以遮挡标题栏的，因此需要知道上下文标签的范围
/// x表示左边界，y表示右边界
///
void SARibbonBar::paintWindowTitle(QPainter& painter, const QString& title, const QRect& titleRegion)
{
    // 如果标题不显示直接跳出
    if (!isTitleVisible()) {
        return;
    }
    painter.save();
    // 先绘制背景
    if (d_ptr->mTitleBackgroundBrush != Qt::NoBrush) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(d_ptr->mTitleBackgroundBrush);
        painter.drawRect(titleRegion);
    }
    // 再绘制文本
    if (d_ptr->mTitleTextColor.isValid()) {
        painter.setPen(d_ptr->mTitleTextColor);
    } else {
        painter.setPen(palette().color(QPalette::WindowText));
    }

    painter.drawText(titleRegion, d_ptr->mTitleAligment, title);
    painter.restore();
}
