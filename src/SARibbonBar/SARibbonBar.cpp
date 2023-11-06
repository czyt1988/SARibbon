#include "SARibbonBar.h"
#include "SARibbonApplicationButton.h"
#include "SARibbonButtonGroupWidget.h"
#include "SARibbonControlButton.h"
#include "SARibbonElementManager.h"
#include "SARibbonQuickAccessBar.h"
#include "SARibbonStackedWidget.h"
#include "SARibbonTabBar.h"
#include "SAWindowButtonGroup.h"
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

#define HELP_DRAW_RECT(p, rect)                                                                                        \
    do {                                                                                                               \
        p.save();                                                                                                      \
        QPen _pen(Qt::DashDotDotLine);                                                                                 \
        _pen.setColor(QColor(219, 26, 59));                                                                            \
        p.setPen(_pen);                                                                                                \
        p.setBrush(QBrush());                                                                                          \
        p.drawRect(rect);                                                                                              \
        p.restore();                                                                                                   \
    } while (0)

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
    QAbstractButton* mApplicationButton;
    SARibbonTabBar* mRibbonTabBar;
    SARibbonStackedWidget* mStackedContainerWidget;
    QList< _SAContextCategoryManagerData > mCurrentShowingContextCategory;
    QList< SARibbonContextCategory* > mContextCategoryList;  ///< 存放所有的上下文标签
    QList< _SARibbonTabData > mHidedCategory;
    int mIconRightBorderPosition;  ///< 标题栏x值得最小值，在有图标和快捷启动按钮，此值都需要变化
    SARibbonControlButton* mMinimumCategoryButton;  ///< 隐藏面板按钮
    SARibbonButtonGroupWidget* mRightButtonGroup;   ///< 在tab bar右边的按钮群
    SARibbonQuickAccessBar* mQuickAccessBar;        ///< 快速响应栏
    SARibbonBar::RibbonStyle mRibbonStyle;          ///< ribbon的风格
    SARibbonBar::RibbonStyle mLastShowStyle;        ///< ribbon的风格
    SARibbonBar::RibbonMode mCurrentRibbonMode;     ///< 记录当前模式
    QSize mWindowButtonSize;                        ///< 由SARibbonMainWindow告诉的windowbutton的尺寸
    QList< QColor > mContextCategoryColorList;      ///< contextCategory的色系
    int mContextCategoryColorListIndex;             ///< 记录contextCategory色系索引
    QColor mTitleTextColor;  ///< 标题文字颜色,默认无效，无效的情况下和SARibbonBar的qss:color属性一致
    QColor mTabBarBaseLineColor;   ///< tabbar 底部会绘制一条线条，定义线条颜色
    Qt::Alignment mTitleAligment;  ///< 标题对齐方式
    bool mIsTitleVisible;          ///< 标题是否显示
    PrivateData(SARibbonBar* par)
        : q_ptr(par)
        , mApplicationButton(nullptr)
        , mRibbonTabBar(nullptr)
        , mStackedContainerWidget(nullptr)
        , mIconRightBorderPosition(1)
        , mMinimumCategoryButton(nullptr)
        , mRightButtonGroup(nullptr)
        , mRibbonStyle(SARibbonBar::RibbonStyleLooseThreeRow)
        , mLastShowStyle(SARibbonBar::RibbonStyleLooseThreeRow)
        , mCurrentRibbonMode(SARibbonBar::NormalRibbonMode)
        , mWindowButtonSize(0, 0)
        , mContextCategoryColorListIndex(-1)
        , mTabBarBaseLineColor(186, 201, 219)
        , mTitleAligment(Qt::AlignCenter)
        , mIsTitleVisible(true)
    {
        mContextCategoryColorList << QColor(201, 89, 156)  // 玫红
                                  << QColor(242, 203, 29)  // 黄
                                  << QColor(255, 157, 0)   // 橙
                                  << QColor(14, 81, 167)   // 蓝
                                  << QColor(228, 0, 69)    // 红
                                  << QColor(67, 148, 0)    // 绿
            ;
    }

    void init()
    {
        mApplicationButton = RibbonSubElementDelegate->createRibbonApplicationButton(q_ptr);
        q_ptr->connect(mApplicationButton, &QAbstractButton::clicked, q_ptr, &SARibbonBar::applicationButtonClicked);
        mRibbonTabBar = RibbonSubElementDelegate->createRibbonTabBar(q_ptr);
        mRibbonTabBar->setObjectName(QStringLiteral("objSARibbonTabBar"));
        mRibbonTabBar->setDrawBase(false);
        q_ptr->connect(mRibbonTabBar, &QTabBar::currentChanged, q_ptr, &SARibbonBar::onCurrentRibbonTabChanged);
        q_ptr->connect(mRibbonTabBar, &QTabBar::tabBarClicked, q_ptr, &SARibbonBar::onCurrentRibbonTabClicked);
        q_ptr->connect(mRibbonTabBar, &QTabBar::tabBarDoubleClicked, q_ptr, &SARibbonBar::onCurrentRibbonTabDoubleClicked);
        q_ptr->connect(mRibbonTabBar, &QTabBar::tabMoved, q_ptr, &SARibbonBar::onTabMoved);
        //
        mStackedContainerWidget = RibbonSubElementDelegate->createRibbonStackedWidget(q_ptr);
        mRibbonTabBar->setObjectName(QStringLiteral("objSAStackedContainerWidget"));
        mStackedContainerWidget->connect(mStackedContainerWidget, &SARibbonStackedWidget::hidWindow, q_ptr, &SARibbonBar::onStackWidgetHided);
        mStackedContainerWidget->installEventFilter(q_ptr);
        setNormalMode();
        //
        mQuickAccessBar = RibbonSubElementDelegate->createQuickAccessBar(q_ptr);
        mQuickAccessBar->setObjectName(QStringLiteral("objSARibbonQuickAccessBar"));
        mQuickAccessBar->setIcon(q_ptr->windowIcon());
        //
        mRightButtonGroup = RibbonSubElementDelegate->craeteButtonGroupWidget(q_ptr);
        mRightButtonGroup->setFrameShape(QFrame::NoFrame);
    }

    void setApplicationButton(QAbstractButton* btn)
    {
        if (mApplicationButton) {
            delete mApplicationButton;
        }
        if (btn) {
            if (btn->parent() != q_ptr) {
                btn->setParent(q_ptr);
            }
            btn->move(0, q_ptr->titleBarHeight());
            q_ptr->connect(btn, &QAbstractButton::clicked, q_ptr, &SARibbonBar::applicationButtonClicked);
        }
        mApplicationButton = btn;
    }

    bool isContainContextCategoryInList(SARibbonContextCategory* contextCategory)
    {
        for (int i = 0; i < mCurrentShowingContextCategory.size(); ++i) {
            if (mCurrentShowingContextCategory[ i ] == contextCategory) {
                return (true);
            }
        }
        return (false);
    }

    void setHideMode()
    {
        mCurrentRibbonMode = SARibbonBar::MinimumRibbonMode;
        this->mStackedContainerWidget->setPopupMode();
        this->mStackedContainerWidget->setFocusPolicy(Qt::NoFocus);
        this->mStackedContainerWidget->clearFocus();
        this->mRibbonTabBar->setFocus();
        this->mStackedContainerWidget->hide();
        q_ptr->setFixedHeight(mRibbonTabBar->geometry().bottom());
    }

    void setNormalMode()
    {
        mCurrentRibbonMode = SARibbonBar::NormalRibbonMode;
        this->mStackedContainerWidget->setNormalMode();
        this->mStackedContainerWidget->setFocus();
        this->mStackedContainerWidget->show();
    }

    QColor getContextCategoryColor()
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

    void updateTabData()
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
     * @brief 通过输入高度计算iconSize
     * @param h
     * @return
     */
    static QSize calcIconSizeByHeight(int h)
    {
        if (h - 6 > 20) {
            return QSize(h - 6, h - 6);
        }
        return QSize(h - 4, h - 4);
    }
};

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
    setNativeMenuBar(false);
    // #ifdef Q_OS_MACOS
    //     setNativeMenuBar(false);
    // #endif
    // #ifdef Q_OS_LINUX
    //     setNativeMenuBar(false);
    // #endif
    if (parent) {
        connect(parent, &QWidget::windowTitleChanged, this, &SARibbonBar::onWindowTitleChanged);
        connect(parent, &QWidget::windowIconChanged, this, &SARibbonBar::onWindowIconChanged);
    }
    setRibbonStyle(RibbonStyleLooseThreeRow);
}

SARibbonBar::~SARibbonBar()
{
}
/**
 * @brief 判断样式是否为两行
 * @param s
 * @return 两行返回true，返回false代表当前是3行
 */
bool SARibbonBar::isTwoRowStyle(SARibbonBar::RibbonStyle s)
{
    return ((s & 0xFF00) > 0);
}

/**
 * @brief 判断是否是office样式
 * @param s
 * @return Office样式返回true，否则就是wps样式
 */
bool SARibbonBar::isOfficeStyle(SARibbonBar::RibbonStyle s)
{
    return ((s & 0xFF) == 0);
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
 * @brief 返回applicationButton
 * @return 默认的applicationButton是@ref SARibbonApplicationButton 生成，通过@ref setApplicationButton 可设置为其他button
 */
QAbstractButton* SARibbonBar::applicationButton()
{
    return (d_ptr->mApplicationButton);
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
    if (btn) {
        if (btn->objectName().isEmpty()) {
            btn->setObjectName(QStringLiteral("SARibbonApplicationButton"));
        }
        btn->setVisible(true);
        // btn->setGeometry(applicationButtonGeometry());
    }
    // 无论设置为什么都触发resize
    QApplication::postEvent(this, new QResizeEvent(size(), size()));
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
    SARibbonCategory* category = RibbonSubElementDelegate->createRibbonCategory(this);

    // catagory->setFixedHeight(categoryHeight());
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
    int index = d_ptr->mRibbonTabBar->addTab(category->categoryName());

    category->setRibbonPannelLayoutMode(isTwoRowStyle() ? SARibbonPannel::TwoRowMode : SARibbonPannel::ThreeRowMode);

    _SARibbonTabData tabdata;

    tabdata.category = category;
    tabdata.index    = index;
    category->setRibbonBar(this);
    d_ptr->mRibbonTabBar->setTabData(index, QVariant::fromValue(tabdata));

    d_ptr->mStackedContainerWidget->insertWidget(index, category);
    connect(category, &QWidget::windowTitleChanged, this, &SARibbonBar::onCategoryWindowTitleChanged);
    QApplication::postEvent(this, new QResizeEvent(size(), size()));
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
    SARibbonCategory* category = RibbonSubElementDelegate->createRibbonCategory(this);

    category->setObjectName(title);
    category->setCategoryName(title);
    insertCategoryPage(category, index);
    return (category);
}

void SARibbonBar::insertCategoryPage(SARibbonCategory* category, int index)
{
    if (nullptr == category) {
        return;
    }
    category->setRibbonPannelLayoutMode(isTwoRowStyle() ? SARibbonPannel::TwoRowMode : SARibbonPannel::ThreeRowMode);
    int i = d_ptr->mRibbonTabBar->insertTab(index, category->categoryName());

    _SARibbonTabData tabdata;

    tabdata.category = category;
    tabdata.index    = i;
    d_ptr->mRibbonTabBar->setTabData(i, QVariant::fromValue(tabdata));
    d_ptr->mStackedContainerWidget->insertWidget(index, category);

    connect(category, &QWidget::windowTitleChanged, this, &SARibbonBar::onCategoryWindowTitleChanged);
    // 更新index信息
    d_ptr->updateTabData();
    QApplication::postEvent(this, new QResizeEvent(size(), size()));
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
    // 移除完后需要重绘
    repaint();
    QApplication::postEvent(this, new QResizeEvent(size(), size()));
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
    SARibbonContextCategory* context = RibbonSubElementDelegate->createRibbonContextCategory(this);

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
    if (currentRibbonStyle() == RibbonStyleCompactThreeRow) {
        resizeInWpsLiteStyle();
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
        category->setRibbonPannelLayoutMode(isTwoRowStyle() ? SARibbonPannel::TwoRowMode : SARibbonPannel::ThreeRowMode);
        // 切换模式后会改变高度，上下文标签显示时要保证显示出来
        //  category->setFixedHeight(categoryHeight());
        int index = d_ptr->mRibbonTabBar->addTab(category->categoryName());
        contextCategoryData.tabPageIndex.append(index);

        _SARibbonTabData tabdata;
        tabdata.category = category;
        tabdata.index    = index;
        d_ptr->mRibbonTabBar->setTabData(index, QVariant::fromValue(tabdata));
    }
    d_ptr->mCurrentShowingContextCategory.append(contextCategoryData);
    // 由于上下文都是在最后追加，不需要调用updateTabData();
    QApplication::postEvent(this, new QResizeEvent(size(), size()));
}

/**
 * @brief 隐藏上下文标签
 * @param context 上下文标签指针
 */
void SARibbonBar::hideContextCategory(SARibbonContextCategory* context)
{
    bool needResize = false;
    int indexOffset = 0;

    for (int i = 0; i < d_ptr->mCurrentShowingContextCategory.size(); ++i) {
        if (d_ptr->mCurrentShowingContextCategory[ i ].contextCategory == context) {
            const QList< int >& indexs = d_ptr->mCurrentShowingContextCategory[ i ].tabPageIndex;
            for (int j = indexs.size() - 1; j >= 0; --j) {
                d_ptr->mRibbonTabBar->removeTab(indexs[ j ]);
                ++indexOffset;
            }
            // 注意，再删除ContextCategory后，tab的序号就会改变，这时，这个tab后面的都要调整它的序号
            needResize = true;
            d_ptr->mCurrentShowingContextCategory.removeAt(i);
            // 移除了ContextCategory后需要break
            break;
        }
    }
    if (needResize) {
        d_ptr->updateTabData();
        QApplication::postEvent(this, new QResizeEvent(size(), size()));
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
    QApplication::postEvent(this, new QResizeEvent(size(), size()));
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
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
    qDebug() << "SARibbonBar::setHideMode " << isMinimum;
#endif
    if (isMinimum) {
        d_ptr->setHideMode();
    } else {
        d_ptr->setNormalMode();
    }
    QResizeEvent resizeEvent(size(), size());
    QApplication::sendEvent(this, &resizeEvent);
    // 发射信号
    emit ribbonModeChanged(isMinimum ? MinimumRibbonMode : NormalRibbonMode);
}

///
/// \brief 当前ribbon是否是隐藏模式
/// \return
///
bool SARibbonBar::isMinimumMode() const
{
    return (d_ptr->mStackedContainerWidget->isPopupMode());
}

///
/// \brief 设置显示隐藏ribbon按钮
///
void SARibbonBar::showMinimumModeButton(bool isShow)
{
    if (isShow) {
        activeRightButtonGroup();
        if (nullptr == d_ptr->mMinimumCategoryButton) {
            d_ptr->mMinimumCategoryButton = RibbonSubElementDelegate->createHidePannelButton(this);
            d_ptr->mMinimumCategoryButton->ensurePolished();  // 载入样式图标
            QAction* action = new QAction(d_ptr->mMinimumCategoryButton);
            action->setIcon(style()->standardIcon(isMinimumMode() ? QStyle::SP_TitleBarUnshadeButton : QStyle::SP_TitleBarShadeButton,
                                                  0,
                                                  d_ptr->mMinimumCategoryButton));
            connect(action, &QAction::triggered, this, [ = ]() {
                this->setMinimumMode(!isMinimumMode());
                action->setIcon(style()->standardIcon(isMinimumMode() ? QStyle::SP_TitleBarUnshadeButton : QStyle::SP_TitleBarShadeButton,
                                                      0,
                                                      d_ptr->mMinimumCategoryButton));
            });
            d_ptr->mMinimumCategoryButton->setDefaultAction(action);
            d_ptr->mRightButtonGroup->addWidget(d_ptr->mMinimumCategoryButton);
            update();
        }
    } else {
        if (nullptr != d_ptr->mMinimumCategoryButton) {
            d_ptr->mMinimumCategoryButton->hide();
            d_ptr->mMinimumCategoryButton->deleteLater();
            d_ptr->mMinimumCategoryButton = nullptr;
        }
    }
    QResizeEvent resizeEvent(size(), size());

    QApplication::sendEvent(this, &resizeEvent);
}

///
/// \brief 是否显示隐藏ribbon按钮
/// \return
///
bool SARibbonBar::haveShowMinimumModeButton() const
{
    return (nullptr != d_ptr->mMinimumCategoryButton);
}

int SARibbonBar::tabBarHeight() const
{
    return (RibbonSubElementStyleOpt.tabBarHeight());
}

int SARibbonBar::titleBarHeight() const
{
    return (RibbonSubElementStyleOpt.titleBarHeight());
}

void SARibbonBar::onWindowTitleChanged(const QString& title)
{
    Q_UNUSED(title);
    update();
}

void SARibbonBar::onWindowIconChanged(const QIcon& i)
{
    if (quickAccessBar()) {
        quickAccessBar()->setIcon(i);
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
    /*
    SARibbonCategory* w = qobject_cast< SARibbonCategory* >(sender());

    if (nullptr == w) {
        return;
    }
    for (int i = 0; i < d_ptr->mRibbonTabBar->count(); ++i) {
        //鉴于tab不会很多，不考虑效率问题
        QVariant var = d_ptr->mRibbonTabBar->tabData(i);
        if (var.isValid()) {
            _SARibbonTabData p = var.value< _SARibbonTabData >();
            if (w == p.category) {
                d_ptr->mRibbonTabBar->setTabText(i, title);
            }
        }
    }
    */
}

///
/// \brief ribbon的显示界面隐藏
///
void SARibbonBar::onStackWidgetHided()
{
    // m_d->ribbonTabBar->repaint();
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
    if (category) {
        if (d_ptr->mStackedContainerWidget->currentWidget() != category) {
            d_ptr->mStackedContainerWidget->setCurrentWidget(category);
        }
        if (isMinimumMode()) {
            d_ptr->mRibbonTabBar->clearFocus();
            if (!d_ptr->mStackedContainerWidget->isVisible()) {
                if (d_ptr->mStackedContainerWidget->isPopupMode()) {
                    // 在stackedContainerWidget弹出前，先给tabbar一个QHoverEvent,让tabbar知道鼠标已经移开
                    QHoverEvent ehl(QEvent::HoverLeave,
                                    d_ptr->mRibbonTabBar->mapToGlobal(QCursor::pos()),
                                    d_ptr->mRibbonTabBar->mapToGlobal(QCursor::pos()));
                    QApplication::sendEvent(d_ptr->mRibbonTabBar, &ehl);
                    resizeStackedContainerWidget();
                    d_ptr->mStackedContainerWidget->setFocus();
                    d_ptr->mStackedContainerWidget->exec();
                    // 在最小模式下，每次显示完stackedContainerWidget后把tab的
                    // 的index设置为-1，这样每次点击都会触发onCurrentRibbonTabChanged
                }
            } else {
            }
        }
    }
    emit currentRibbonTabChanged(index);
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
    if (index != d_ptr->mRibbonTabBar->currentIndex()) {
        // 点击的标签不一致通过changed槽去处理
        return;
    }
    if (this->isMinimumMode()) {
        if (!this->d_ptr->mStackedContainerWidget->isVisible()) {
            if (this->d_ptr->mStackedContainerWidget->isPopupMode()) {
                // 在stackedContainerWidget弹出前，先给tabbar一个QHoverEvent,让tabbar知道鼠标已经移开
                QHoverEvent ehl(QEvent::HoverLeave,
                                d_ptr->mRibbonTabBar->mapToGlobal(QCursor::pos()),
                                d_ptr->mRibbonTabBar->mapToGlobal(QCursor::pos()));
                QApplication::sendEvent(d_ptr->mRibbonTabBar, &ehl);
                // 弹出前都调整一下位置，避免移动后位置异常
                resizeStackedContainerWidget();
                this->d_ptr->mStackedContainerWidget->setFocus();
                this->d_ptr->mStackedContainerWidget->exec();
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
    Q_UNUSED(index);
    setMinimumMode(!isMinimumMode());
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

void SARibbonBar::resizeAll()
{
    if (isOfficeStyle()) {
        resizeInOfficeStyle();
    } else {
        resizeInWpsLiteStyle();
    }
    update();
}

void SARibbonBar::synchronousCategoryLayoutMode()
{
    // 根据样式调整SARibbonCategory的布局形式
    QList< SARibbonCategory* > categorys = categoryPages();

    for (SARibbonCategory* c : qAsConst(categorys)) {
        c->setRibbonPannelLayoutMode(SARibbonBar::isTwoRowStyle(currentRibbonStyle()) ? SARibbonPannel::TwoRowMode
                                                                                      : SARibbonPannel::ThreeRowMode);
    }

    // 根据样式调整bar的高度
    if (NormalRibbonMode == currentRibbonState()) {
        setFixedHeight(mainBarHeight());
    }
}

void SARibbonBar::activeRightButtonGroup()
{
    if (nullptr == d_ptr->mRightButtonGroup) {
        d_ptr->mRightButtonGroup = RibbonSubElementDelegate->craeteButtonGroupWidget(this);
        d_ptr->mRightButtonGroup->setFrameShape(QFrame::NoFrame);
    }
    d_ptr->mRightButtonGroup->show();
}

SARibbonButtonGroupWidget* SARibbonBar::rightButtonGroup()
{
    activeRightButtonGroup();
    return (d_ptr->mRightButtonGroup);
}

SARibbonQuickAccessBar* SARibbonBar::quickAccessBar()
{
    return (d_ptr->mQuickAccessBar);
}

/**
 * @brief 设置ribbonbar的风格，此函数会重新设置所有元素，包括button的布局方式，
 * 尤其是从三行变到两行的过程，重设的内容较多
 * @note 此函数会自动触发ResizeEvent，不需要手动调用
 * @note 默认情况下2行模式不换行,如果想在两行模式换行，在调用SARibbonBar::setRibbonStyle后，再SARibbonToolButton::setEnableWordWrap(true)
 * ,同时再调用updateRibbonElementGeometry()刷新布局
 * @param v 样式，见@ref SARibbonBar::RibbonStyle
 */
void SARibbonBar::setRibbonStyle(SARibbonBar::RibbonStyle v)
{
    d_ptr->mRibbonStyle   = v;
    d_ptr->mLastShowStyle = v;
    d_ptr->mQuickAccessBar->setEnableShowIcon(isOfficeStyle(v));
    // 默认情况下2行模式不换行
    // 如果想在两行模式换行，在调用SARibbonBar::setRibbonStyle后，再SARibbonToolButton::setEnableWordWrap(true)
    SARibbonToolButton::setEnableWordWrap(!isTwoRowStyle(v));

    synchronousCategoryLayoutMode();
    QSize oldSize = size();
    QSize newSize(oldSize.width(), mainBarHeight());
    QResizeEvent es(newSize, oldSize);

    QApplication::sendEvent(this, &es);
    if (MinimumRibbonMode == currentRibbonState()) {
        // 处于最小模式下时，bar的高度为tabbar的bottom,这个调整必须在resize event之后
        setFixedHeight(d_ptr->mRibbonTabBar->geometry().bottom());
    }
    emit ribbonStyleChanged(d_ptr->mRibbonStyle);
}

/**
   @brief 返回当前ribbon的风格
   @return 返回当前ribbon的风格
 */
SARibbonBar::RibbonStyle SARibbonBar::currentRibbonStyle() const
{
    return (d_ptr->mRibbonStyle);
}

SARibbonBar::RibbonMode SARibbonBar::currentRibbonState() const
{
    return (d_ptr->mCurrentRibbonMode);
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
    int index = d_ptr->mStackedContainerWidget->indexOf(category);

    if (index >= 0) {
        setCurrentIndex(index);
    }
}

/**
 * @brief 判断当前的样式是否为两行
 * @return
 */
bool SARibbonBar::isTwoRowStyle() const
{
    return (SARibbonBar::isTwoRowStyle(d_ptr->mRibbonStyle));
}

/**
 * @brief 判断当前的样式是否为office样式
 * @return
 */
bool SARibbonBar::isOfficeStyle() const
{
    return (SARibbonBar::isOfficeStyle(currentRibbonStyle()));
}

/**
 * @brief 告诉saribbonbar，window button的尺寸,此值由SARibbonMainWindow传入，
 * 告诉最大，最小，关闭按钮的大小，在显示标题栏的时候好计算尺寸
 * @param size
 */
void SARibbonBar::setWindowButtonSize(const QSize& size)
{
    d_ptr->mWindowButtonSize = size;
}

/**
 * @brief 设置标题的文字颜色
 *
 * 标题时mainwindow的windowTitle，如果要设置标题，直接调用SARibbonMainWindow::setWindowTitle 进行设置
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
    //    updateRibbonElementGeometry();
    //    QList< SARibbonCategory* > categorys = categoryPages();
    //    for (SARibbonCategory* c : qAsConst(categorys)) {
    //        c->updateItemGeometry();
    //    }
    //    //重新调整尺寸
    //    resizeAll();
    //
    //    auto fnUpdate = [](QWidget* w) {
    //        if (w) {
    //            if (w->layout()) {
    //                w->layout()->invalidate();
    //            }
    //            w->updateGeometry();
    //            w->adjustSize();
    //        }
    //    };
    //    fnUpdate(d_ptr->mQuickAccessBar);
    //    fnUpdate(d_ptr->mRightButtonGroup);
    updateGeometry();
    QList< SARibbonCategory* > categorys = categoryPages();
    for (SARibbonCategory* c : qAsConst(categorys)) {
        c->updateItemGeometry();
    }

    //! 直接给一个resizeevent，让所有刷新
    QResizeEvent* e = new QResizeEvent(size(), QSize());
    QApplication::postEvent(this, e);
}

/**
 * @brief 设置标题的对齐方式
 * @param al
 */
void SARibbonBar::setWindowTitleAligment(Qt::Alignment al)
{
    d_ptr->mTitleAligment = al;
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
    SARibbonToolButton::setEnableWordWrap(on);
    updateRibbonGeometry();
}

/**
 * @brief 判断是否允许换行
 * @return
 */
bool SARibbonBar::isEnableWordWrap() const
{
    return SARibbonToolButton::isEnableWordWrap();
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

bool SARibbonBar::eventFilter(QObject* obj, QEvent* e)
{
    if (obj) {
        // 调整多文档时在窗口模式下的按钮更新
        if ((obj == cornerWidget(Qt::TopLeftCorner)) || (obj == cornerWidget(Qt::TopRightCorner))) {
            if ((QEvent::UpdateLater == e->type()) || (QEvent::MouseButtonRelease == e->type())
                || (QEvent::WindowActivate == e->type())) {
                QApplication::postEvent(this, new QResizeEvent(size(), size()));
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
        //        if (SAWindowButtonGroup *g = qobject_cast<SAWindowButtonGroup *>(obj)) {
        //            if (e->type() == QEvent::Resize) {
        //                m_d->windowButtonSize = g->size();
        //            }
        //        }
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
    int mintabBarWidth = 0;

    for (int i = 0; i < d_ptr->mRibbonTabBar->count(); ++i) {
        mintabBarWidth += d_ptr->mRibbonTabBar->tabRect(i).width();
    }
    mintabBarWidth += (mg.left() + mg.right());
    return (mintabBarWidth);
}

/**
 * @brief mainBarHeight的计算高度
 *
 * 有可能SARibbonBar::height和mainBarHeight不相等，这种情况发生在RibbonState::MinimumRibbonMode状态下
 * @return 高度
 */
int SARibbonBar::mainBarHeight() const
{
    return RibbonSubElementStyleOpt.ribbonBarHeight(currentRibbonStyle());
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
}

void SARibbonBar::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);
    if (isOfficeStyle()) {
        paintInNormalStyle();
    } else {
        paintInWpsLiteStyle();
    }
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
    QPainter p(this);
    HELP_DRAW_RECT(p, m_d->quickAccessBar->geometry());
    HELP_DRAW_RECT(p, m_d->ribbonTabBar->geometry());
    HELP_DRAW_RECT(p, m_d->stackedContainerWidget->geometry());
#endif
}

void SARibbonBar::paintInNormalStyle()
{
    QPainter p(this);

    //!
    paintBackground(p);
    //! 显示上下文标签
    p.save();
    QList< _SAContextCategoryManagerData > contextCategoryDataList = d_ptr->mCurrentShowingContextCategory;
    bool isCurrentSelectContextCategoryPage                        = false;

    QPoint contextCategoryRegion(width(), -1);
    QMargins border = contentsMargins();
    for (int i = 0; i < contextCategoryDataList.size(); ++i) {
        QRect contextTitleRect;
        QList< int > indexs = contextCategoryDataList[ i ].tabPageIndex;
        QColor clr          = contextCategoryDataList[ i ].contextCategory->contextColor();
        if (!indexs.isEmpty()) {
            contextTitleRect = d_ptr->mRibbonTabBar->tabRect(indexs.first());
            QRect endRect    = d_ptr->mRibbonTabBar->tabRect(indexs.last());
            contextTitleRect.setRight(endRect.right());
            contextTitleRect.translate(d_ptr->mRibbonTabBar->x(), d_ptr->mRibbonTabBar->y());
            contextTitleRect.setHeight(d_ptr->mRibbonTabBar->height() - 1);  // 减1像素，避免tabbar基线覆盖
            contextTitleRect -= d_ptr->mRibbonTabBar->tabMargin();
            // 把区域顶部扩展到窗口顶部
            contextTitleRect.setTop(border.top());
            // 绘制
            paintContextCategoryTab(p, contextCategoryDataList[ i ].contextCategory->contextTitle(), contextTitleRect, clr);
            // 更新上下文标签的范围，用于控制标题栏的显示
            if (contextTitleRect.left() < contextCategoryRegion.x()) {
                contextCategoryRegion.setX(contextTitleRect.left());
            }
            if (contextTitleRect.right() > contextCategoryRegion.y()) {
                contextCategoryRegion.setY(contextTitleRect.right());
            }
        }
        isCurrentSelectContextCategoryPage = indexs.contains(d_ptr->mRibbonTabBar->currentIndex());
        if (isCurrentSelectContextCategoryPage) {
            QPen pen;
            pen.setColor(clr);
            pen.setWidth(1);
            p.setPen(pen);
            p.setBrush(Qt::NoBrush);
            p.drawRect(d_ptr->mStackedContainerWidget->geometry());
            isCurrentSelectContextCategoryPage = false;
        }
    }
    p.restore();
    //! 显示标题等
    QWidget* parWindow = parentWidget();

    if (parWindow) {
        QRect titleRegion;
        if (contextCategoryRegion.y() < 0) {
            titleRegion.setRect(d_ptr->mQuickAccessBar->geometry().right() + 1,
                                border.top(),
                                width() - d_ptr->mIconRightBorderPosition - border.right()
                                    - d_ptr->mWindowButtonSize.width() - d_ptr->mQuickAccessBar->geometry().right() - 1,
                                titleBarHeight());
        } else {
            int leftwidth = contextCategoryRegion.x() - d_ptr->mQuickAccessBar->geometry().right() - d_ptr->mIconRightBorderPosition;
            int rightwidth = width() - contextCategoryRegion.y() - d_ptr->mWindowButtonSize.width();
            //            if (width() - contextCategoryRegion.y() > contextCategoryRegion.x()-x) {
            if (rightwidth > leftwidth) {
                // 说明右边的区域大一点，标题显示在右，显示在右边需要减去windowbutton宽度
                titleRegion.setRect(contextCategoryRegion.y(), border.top(), rightwidth, titleBarHeight());
            } else {
                // 说明左边的大一点
                titleRegion.setRect(d_ptr->mIconRightBorderPosition + d_ptr->mQuickAccessBar->geometry().right(),
                                    border.top(),
                                    leftwidth,
                                    titleBarHeight());
            }
        }
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
        p.save();
        p.setBrush(QColor(255, 0, 0, 120));
        p.drawRect(titleRegion);
        p.restore();
#endif
        paintWindowTitle(p, parWindow->windowTitle(), titleRegion);
    }
}

void SARibbonBar::paintInWpsLiteStyle()
{
    QPainter p(this);
    //!
    paintBackground(p);
    //! 显示上下文标签
    p.save();
    QList< _SAContextCategoryManagerData > contextCategoryDataList = d_ptr->mCurrentShowingContextCategory;
    bool isCurrentSelectContextCategoryPage                        = false;
    QMargins border                                                = contentsMargins();
    for (int i = 0; i < contextCategoryDataList.size(); ++i) {
        QRect contextTitleRect;
        QList< int > indexs = contextCategoryDataList[ i ].tabPageIndex;
        QColor clr          = contextCategoryDataList[ i ].contextCategory->contextColor();
        if (!indexs.isEmpty()) {
            contextTitleRect = d_ptr->mRibbonTabBar->tabRect(indexs.first());
            QRect endRect    = d_ptr->mRibbonTabBar->tabRect(indexs.last());
            contextTitleRect.setRight(endRect.right());
            contextTitleRect.translate(d_ptr->mRibbonTabBar->x(), d_ptr->mRibbonTabBar->y());
            contextTitleRect.setHeight(d_ptr->mRibbonTabBar->height() - 1);
            contextTitleRect -= d_ptr->mRibbonTabBar->tabMargin();
            // 把区域顶部扩展到窗口顶部
            contextTitleRect.setTop(border.top());
            // 绘制
            paintContextCategoryTab(p, QString(), contextTitleRect, clr);
        }
        isCurrentSelectContextCategoryPage = indexs.contains(d_ptr->mRibbonTabBar->currentIndex());
        if (isCurrentSelectContextCategoryPage) {
            QPen pen;
            pen.setColor(clr);
            pen.setWidth(1);
            p.setPen(pen);
            p.setBrush(Qt::NoBrush);
            p.drawRect(d_ptr->mStackedContainerWidget->geometry());
            isCurrentSelectContextCategoryPage = false;
        }
    }
    p.restore();
    //! 显示标题等

    QWidget* parWindow = parentWidget();

    if (parWindow) {
        int start = d_ptr->mRibbonTabBar->x() + d_ptr->mRibbonTabBar->width();
        int width = d_ptr->mQuickAccessBar->x() - start;
        if (width > 20) {
            QRect titleRegion(start, border.top(), width, titleBarHeight());
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
            p.save();
            p.setBrush(QColor(255, 0, 0, 120));
            p.drawRect(titleRegion);
            p.restore();
#endif
            paintWindowTitle(p, parWindow->windowTitle(), titleRegion);
        }
    }
}

void SARibbonBar::resizeStackedContainerWidget()
{
    QMargins border                   = contentsMargins();
    const QRect& ribbonTabBarGeometry = d_ptr->mRibbonTabBar->geometry();
    int x                             = border.left();
    int y                             = ribbonTabBarGeometry.bottom() + 1;
    int w                             = width() - border.left() - border.right();
    int h                             = mainBarHeight() - ribbonTabBarGeometry.bottom() - border.bottom() - 1;
    if (d_ptr->mStackedContainerWidget->isPopupMode()) {
        // 弹出模式时，位置为全局位置
        QPoint absPosition = mapToGlobal(QPoint(x, y));
        x                  = absPosition.x();
        y                  = absPosition.y();
    }
    d_ptr->mStackedContainerWidget->setFixedSize(QSize(w, h));
    d_ptr->mStackedContainerWidget->setGeometry(x, y, w, h);
}

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
void SARibbonBar::paintContextCategoryTab(QPainter& painter, const QString& title, QRect contextRect, const QColor& color)
{
    // 绘制上下文标签
    // 首先有5像素的实体粗线位于顶部
    QMargins border = contentsMargins();
    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    painter.drawRect(QRect(contextRect.x(), border.top(), contextRect.width(), 5));

    // 剩下把颜色变亮90%
    QColor gColor = color.lighter(190);

    // 减去之前的5像素
    contextRect -= QMargins(0, 5, 0, 0);
    painter.fillRect(contextRect, gColor);

    // 只有在office模式下才需要绘制标题
    if (isOfficeStyle()) {
        if (!title.isEmpty()) {
            contextRect.setBottom(d_ptr->mRibbonTabBar->geometry().top());
            painter.setPen(color);
            painter.drawText(contextRect, Qt::AlignCenter, title);
        }
    }
    painter.restore();
}

void SARibbonBar::resizeEvent(QResizeEvent* e)
{
    Q_UNUSED(e);
    if (isOfficeStyle()) {
        resizeInOfficeStyle();
    } else {
        resizeInWpsLiteStyle();
    }
    update();
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
            resizeStackedContainerWidget();
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
        updateRibbonGeometry();
    } break;
    case QEvent::Show: {
        updateRibbonGeometry();
    } break;
    default:
        break;
    }
    QMenuBar::changeEvent(e);
}

void SARibbonBar::resizeInOfficeStyle()
{
    synchronousCategoryLayoutMode();
    QMargins border = contentsMargins();
    int x           = border.left();
    int y           = border.top();

    // cornerWidget - TopLeftCorner
    const int validTitleBarHeight = titleBarHeight();
    const int tabH                = tabBarHeight();

    x += d_ptr->mIconRightBorderPosition + 5;
    if (QWidget* connerL = cornerWidget(Qt::TopLeftCorner)) {
        if (connerL->isVisible()) {
            QSize connerSize = connerL->sizeHint();
            if (connerSize.height() < validTitleBarHeight) {
                int detal = (validTitleBarHeight - connerSize.height()) / 2;
                connerL->setGeometry(x, y + detal, connerSize.width(), connerSize.height());
            } else {
                connerL->setGeometry(x, y, connerSize.width(), validTitleBarHeight);
            }
            x = connerL->geometry().right() + 5;
        }
    }
    // quick access bar定位
    if (d_ptr->mQuickAccessBar) {
        if (d_ptr->mQuickAccessBar->isVisible()) {
            if (d_ptr->mQuickAccessBar->height() != validTitleBarHeight) {
                d_ptr->mQuickAccessBar->setFixedHeight(validTitleBarHeight);
            }
            QSize quickAccessBarSize = d_ptr->mQuickAccessBar->sizeHint();
            // 上下留1px的边线
            d_ptr->mQuickAccessBar->setGeometry(x, y + 1, quickAccessBarSize.width(), validTitleBarHeight - 2);
            // 变更iconsize
            QSize btnIconSize = PrivateData::calcIconSizeByHeight(validTitleBarHeight - 2);
            if (btnIconSize != d_ptr->mQuickAccessBar->iconSize()) {
                d_ptr->mQuickAccessBar->setIconSize(btnIconSize);
            }
        }
    }
    // 第二行，开始布局applicationButton，tabbar，tabBarRightSizeButtonGroupWidget，TopRightCorner
    x = border.left();
    y += validTitleBarHeight;
    // applicationButton 定位
    if (d_ptr->mApplicationButton) {
        if (d_ptr->mApplicationButton->isVisible()) {
            d_ptr->mApplicationButton->setGeometry(x, y, d_ptr->mApplicationButton->size().width(), tabH);
            x = d_ptr->mApplicationButton->geometry().right();
        }
    }
    // top right是一定要配置的，对于多文档窗口，子窗口的缩放等按钮就是通过这个窗口实现，
    // 由于这个窗口一定要在最右，因此先对这个窗口进行布局
    // cornerWidget - TopRightCorner
    // 获取最右边的位置
    int endX = width() - border.right();

    if (QWidget* connerW = cornerWidget(Qt::TopRightCorner)) {
        if (connerW->isVisible()) {
            QSize connerSize = connerW->sizeHint();
            endX -= connerSize.width();
            if (connerSize.height() < tabH) {
                int detal = (tabH - connerSize.height()) / 2;
                connerW->setGeometry(endX, y + detal, connerSize.width(), connerSize.height());
            } else {
                connerW->setGeometry(endX, y, connerSize.width(), tabH);
            }
        }
    }
    // applicationButton和TopRightCorner完成定位，才可以定位tab bar
    // tab bar 定位

    // tabBar 右边的附加按钮组，这里一般会附加一些类似登录等按钮组
    // 20231106 把visible的判断去掉 && d_ptr->mRightButtonGroup->isVisible()
    if (d_ptr->mRightButtonGroup) {
        QSize wSize = d_ptr->mRightButtonGroup->sizeHint();
        endX -= wSize.width();
        // 上下留1px的边线
        d_ptr->mRightButtonGroup->setGeometry(endX, y + 1, wSize.width(), tabH - 2);
        // 变更iconsize
        QSize btnIconSize = PrivateData::calcIconSizeByHeight(tabH - 2);
        if (btnIconSize != d_ptr->mRightButtonGroup->iconSize()) {
            d_ptr->mRightButtonGroup->setIconSize(btnIconSize);
        }
    }
    // 最后确定tabbar宽度
    int tabBarWidth = endX - x;
    d_ptr->mRibbonTabBar->setGeometry(x, y, tabBarWidth, tabH);

    resizeStackedContainerWidget();
}

void SARibbonBar::resizeInWpsLiteStyle()
{
    synchronousCategoryLayoutMode();
    QMargins border = contentsMargins();
    int x           = border.left();
    int y           = border.top();

    const int validTitleBarHeight = titleBarHeight();

    // 先布局右边内容
    //  cornerWidget - TopRightCorner
    int endX = width() - border.right() - d_ptr->mWindowButtonSize.width();

    if (QWidget* connerW = cornerWidget(Qt::TopRightCorner)) {
        if (connerW->isVisible()) {
            QSize connerSize = connerW->sizeHint();
            endX -= connerSize.width();
            if (connerSize.height() < validTitleBarHeight) {
                int detal = (validTitleBarHeight - connerSize.height()) / 2;
                connerW->setGeometry(endX, y + detal, connerSize.width(), connerSize.height());
            } else {
                connerW->setGeometry(endX, y, connerSize.width(), validTitleBarHeight);
            }
        }
    }

    // tabBar 右边的附加按钮组
    // 20231106 把visible的判断去掉 && d_ptr->mRightButtonGroup->isVisible()
    if (d_ptr->mRightButtonGroup) {
        QSize wSize = d_ptr->mRightButtonGroup->sizeHint();
        endX -= wSize.width();
        // 上下留1px的边线
        d_ptr->mRightButtonGroup->setGeometry(endX, y + 1, wSize.width(), validTitleBarHeight - 2);
        // 变更iconsize
        QSize btnIconSize = PrivateData::calcIconSizeByHeight(validTitleBarHeight - 2);
        if (btnIconSize != d_ptr->mRightButtonGroup->iconSize()) {
            d_ptr->mRightButtonGroup->setIconSize(btnIconSize);
        }
    }
    // quick access bar定位
    if (d_ptr->mQuickAccessBar) {
        if (d_ptr->mQuickAccessBar->isVisible()) {
            QSize quickAccessBarSize = d_ptr->mQuickAccessBar->sizeHint();
            endX -= quickAccessBarSize.width();
            // 上下留1px的边线
            d_ptr->mQuickAccessBar->setGeometry(endX, y + 1, quickAccessBarSize.width(), validTitleBarHeight - 2);
            // 变更iconsize
            QSize btnIconSize = PrivateData::calcIconSizeByHeight(validTitleBarHeight - 2);
            if (btnIconSize != d_ptr->mQuickAccessBar->iconSize()) {
                d_ptr->mQuickAccessBar->setIconSize(btnIconSize);
            }
        }
    }
    // cornerWidget - TopLeftCorner
    if (QWidget* connerL = cornerWidget(Qt::TopLeftCorner)) {
        if (connerL->isVisible()) {
            QSize connerSize = connerL->sizeHint();
            endX -= connerSize.width();
            if (connerSize.height() < validTitleBarHeight) {
                int detal = (validTitleBarHeight - connerSize.height()) / 2;
                connerL->setGeometry(endX, y + detal, connerSize.width(), connerSize.height());
            } else {
                connerL->setGeometry(endX, y, connerSize.width(), validTitleBarHeight);
            }
        }
    }

    // tab 的y值需要重新计算
    int tabH = tabBarHeight();

    if (tabH > validTitleBarHeight) {
        // 这种直接把tabH设置为validTitleBarHeight
        tabH = validTitleBarHeight;
    }
    y = y + validTitleBarHeight - tabH;  // 如果tabH较小，则下以，让tab底部和title的底部对齐

    // applicationButton 定位，与TabBar同高
    if (d_ptr->mApplicationButton) {
        if (d_ptr->mApplicationButton->isVisible()) {
            d_ptr->mApplicationButton->setGeometry(x, y, d_ptr->mApplicationButton->size().width(), tabH);
            x = d_ptr->mApplicationButton->geometry().right() + 2;
        }
    }
    // tab bar 定位 wps模式下applicationButton的右边就是tab bar
    int tabBarWidth = endX - x;
    // 20200831
    // tabBarWidth的宽度原来为endX - x;，现需要根据实际进行调整
    // 为了把tabbar没有tab的部分不占用，这里的宽度需要根据tab的size来进行设置，让tabbar的长度刚刚好，这样能让出
    // mainwindow的空间，接受鼠标事件，从而实现拖动等操作，否则tabbar占用整个顶栏，鼠标无法点击到mainwindow
    // 计算tab所占用的宽度
    int mintabBarWidth = calcMinTabBarWidth();

    if (mintabBarWidth < tabBarWidth) {
        tabBarWidth = mintabBarWidth;
    }
    d_ptr->mRibbonTabBar->setGeometry(x, y, tabBarWidth, tabH);

    // 调整整个stackedContainer
    resizeStackedContainerWidget();
}

void SARibbonBar::paintBackground(QPainter& painter)
{
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
    if (d_ptr->mTitleTextColor.isValid()) {
        painter.setPen(d_ptr->mTitleTextColor);
    }
    painter.drawText(titleRegion, d_ptr->mTitleAligment, title);
    painter.restore();
}
