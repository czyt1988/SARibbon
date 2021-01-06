﻿#include "SARibbonBar.h"
#include <QApplication>
#include <QPainter>
#include "SARibbonApplicationButton.h"
#include "SARibbonTabBar.h"
#include <QSet>
#include <QResizeEvent>
#include "SARibbonStackedWidget.h"
#include <QVariant>
#include <QLinearGradient>
#include <QDebug>
#include "SARibbonElementManager.h"
#include "SARibbonControlButton.h"
#include <QApplication>
#include <QDesktopWidget>
#include <SARibbonButtonGroupWidget.h>
#include <QAction>
#include "SARibbonQuickAccessBar.h"
#include <QStyleOptionMenuItem>

#define HELP_DRAW_RECT(p, rect)			    \
    do{					    \
        p.save();			    \
        QPen _pen(Qt::DashDotDotLine);	    \
        _pen.setColor(QColor(219, 26, 59)); \
        p.setPen(_pen);			    \
        p.setBrush(QBrush());		    \
        p.drawRect(rect);		    \
        p.restore();			    \
    }while(0)

class ContextCategoryManagerData
{
public:
    SARibbonContextCategory *contextCategory;
    QList<int> tabPageIndex;
    bool operator ==(const SARibbonContextCategory *contextPage)
    {
        return (this->contextCategory == contextPage);
    }
};

class SARibbonBarPrivate
{
public:
    SARibbonBar *MainClass;
    QAbstractButton *applitionButton;
    SARibbonTabBar *ribbonTabBar;
    SARibbonStackedWidget *stackedContainerWidget;
    QList<ContextCategoryManagerData> currentShowingContextCategory;
    int iconRightBorderPosition;                                    ///< 标题栏x值得最小值，在有图标和快捷启动按钮，此值都需要变化
    SARibbonControlButton *hidePannelButton;                        ///< 隐藏面板按钮
    SARibbonButtonGroupWidget *tabBarRightSizeButtonGroupWidget;    ///< 在tab bar旁边的button group widget                                    ///< tabbar底部的线条颜色
    SARibbonQuickAccessBar *quickAccessBar;                         ///< 快速响应栏
    SARibbonBar::RibbonStyle ribbonStyle;                           ///< ribbon的风格
    SARibbonBar::RibbonStyle lastShowStyle;                         ///< ribbon的风格
    SARibbonBar::RibbonState currentRibbonMode;                     ///< 记录当前模式
    SARibbonBarPrivate(SARibbonBar *par)
        : applitionButton(nullptr)
        , ribbonTabBar(nullptr)
        , stackedContainerWidget(nullptr)
        , iconRightBorderPosition(1)
        , hidePannelButton(nullptr)
        , tabBarRightSizeButtonGroupWidget(nullptr)
        , ribbonStyle(SARibbonBar::OfficeStyle)
        , lastShowStyle(SARibbonBar::OfficeStyle)
        , currentRibbonMode(SARibbonBar::NormalRibbonMode)
    {
        MainClass = par;
    }


    void init()
    {
        applitionButton = RibbonSubElementDelegate->createRibbonApplicationButton(MainClass);
        applitionButton->setGeometry(RibbonSubElementStyleOpt.widgetBord.left(), RibbonSubElementStyleOpt.titleBarHight+RibbonSubElementStyleOpt.widgetBord.top(), 56, 30);
        MainClass->connect(applitionButton, &QAbstractButton::clicked
            , MainClass, &SARibbonBar::applitionButtonClicked);
        //
        ribbonTabBar = RibbonSubElementDelegate->createRibbonTabBar(MainClass);
        ribbonTabBar->setObjectName(QStringLiteral("objSARibbonTabBar"));
        ribbonTabBar->setDrawBase(false);
        ribbonTabBar->setGeometry(applitionButton->geometry().right()
            , RibbonSubElementStyleOpt.titleBarHight+RibbonSubElementStyleOpt.widgetBord.top()
            , MainClass->width(), RibbonSubElementStyleOpt.tabBarHight);
        MainClass->connect(ribbonTabBar, &QTabBar::currentChanged
            , MainClass, &SARibbonBar::onCurrentRibbonTabChanged);
        MainClass->connect(ribbonTabBar, &QTabBar::tabBarClicked
            , MainClass, &SARibbonBar::onCurrentRibbonTabClicked);
        MainClass->connect(ribbonTabBar, &QTabBar::tabBarDoubleClicked
            , MainClass, &SARibbonBar::onCurrentRibbonTabDoubleClicked);
        //
        stackedContainerWidget = RibbonSubElementDelegate->createRibbonStackedWidget(MainClass);
        ribbonTabBar->setObjectName(QStringLiteral("objSAStackedContainerWidget"));
        stackedContainerWidget->setGeometry(RibbonSubElementStyleOpt.widgetBord.left()
            , ribbonTabBar->geometry().bottom()+1
            , MainClass->width()-RibbonSubElementStyleOpt.widgetBord.left()-RibbonSubElementStyleOpt.widgetBord.right()
            , MainClass->height()-ribbonTabBar->geometry().bottom()-2-RibbonSubElementStyleOpt.widgetBord.bottom());
        stackedContainerWidget->connect(stackedContainerWidget, &SARibbonStackedWidget::hidWindow
            , MainClass, &SARibbonBar::onStackWidgetHided);
        stackedContainerWidget->installEventFilter(MainClass);
        setNormalMode();
        //
        quickAccessBar = RibbonSubElementDelegate->createQuickAccessBar(MainClass);
        quickAccessBar->setObjectName(QStringLiteral("objSARibbonQuickAccessBar"));
    }


    void setApplitionButton(QAbstractButton *btn)
    {
        if (applitionButton) {
            delete applitionButton;
        }
        applitionButton = btn;
        if (applitionButton) {
            applitionButton->move(0, RibbonSubElementStyleOpt.titleBarHight);
            MainClass->connect(applitionButton, &QAbstractButton::clicked
                , MainClass, &SARibbonBar::applitionButtonClicked);
        }
    }


    bool isContainContextCategoryInList(SARibbonContextCategory *contextCategory)
    {
        for (int i = 0; i < currentShowingContextCategory.size(); ++i)
        {
            if (currentShowingContextCategory[i] == contextCategory) {
                return (true);
            }
        }
        return (false);
    }


    void setHideMode()
    {
        currentRibbonMode = SARibbonBar::MinimumRibbonMode;
        this->stackedContainerWidget->setPopupMode();
        this->stackedContainerWidget->setFocusPolicy(Qt::NoFocus);
        this->stackedContainerWidget->clearFocus();
        this->ribbonTabBar->setFocus();
        this->stackedContainerWidget->hide();
        MainClass->setFixedHeight(ribbonTabBar->geometry().bottom());
    }


    void setNormalMode()
    {
        currentRibbonMode = SARibbonBar::NormalRibbonMode;
        this->stackedContainerWidget->setNormalMode();
        this->stackedContainerWidget->setFocus();
        this->stackedContainerWidget->show();
    }
};


/**
 * @brief 判断样式是否为两行
 * @param s
 * @return
 */
bool SARibbonBar::isTwoRowStyle(SARibbonBar::RibbonStyle s)
{
    return ((s & 0xFF00) > 0);
}


/**
 * @brief 判断是否是office样式
 * @param s
 * @return
 */
bool SARibbonBar::isOfficeStyle(SARibbonBar::RibbonStyle s)
{
    return ((s & 0xFF) == 0);
}


SARibbonBar::SARibbonBar(QWidget *parent) : QMenuBar(parent)
    , m_d(new SARibbonBarPrivate(this))
{
    m_d->init();
    setFixedHeight(mainBarHeight());
    connect(parent, &QWidget::windowTitleChanged, this, &SARibbonBar::onWindowTitleChanged);
    connect(parent, &QWidget::windowIconChanged, this, &SARibbonBar::onWindowIconChanged);
}


QAbstractButton *SARibbonBar::applitionButton()
{
    return (m_d->applitionButton);
}


void SARibbonBar::setApplitionButton(QAbstractButton *btn)
{
    m_d->setApplitionButton(btn);
    if (btn) {
        if (btn->objectName().isEmpty()) {
            btn->setObjectName(QStringLiteral("SARibbonApplitionButton"));
        }
    }
    repaint();
}


SARibbonTabBar *SARibbonBar::ribbonTabBar()
{
    return (m_d->ribbonTabBar);
}


///
/// \brief 添加一个标签
/// \param title 标签名字
/// \return 返回一个窗口容器，在Category里可以添加其他控件
/// \see SARibbonCategory
///
SARibbonCategory *SARibbonBar::addCategoryPage(const QString& title)
{
    SARibbonCategory *catagory = RibbonSubElementDelegate->createRibbonCategory(this);

    //catagory->setFixedHeight(categoryHeight());
    catagory->setWindowTitle(title);
    int index = m_d->ribbonTabBar->addTab(title);

    catagory->setRibbonPannelLayoutMode(isTwoRowStyle() ? SARibbonPannel::TwoRowMode : SARibbonPannel::ThreeRowMode);

    m_d->ribbonTabBar->setTabData(index, QVariant((quint64)catagory));
    m_d->stackedContainerWidget->addWidget(catagory);
    connect(catagory, &QWidget::windowTitleChanged, this, &SARibbonBar::onCategoryWindowTitleChanged);
    if (currentRibbonStyle() == WpsLiteStyle) {
        resizeInWpsLiteStyle();
    }
    return (catagory);
}


/**
 * @brief 获取当前显示的所有的SARibbonCategory，不含未显示的SARibbonContextCategory的SARibbonCategory，
 * 如果SARibbonContextCategory已经显示，则SARibbonContextCategory里的SARibbonCategory也会返回
 * @return
 */
QList<SARibbonCategory *> SARibbonBar::categoryPages() const
{
    int c = m_d->stackedContainerWidget->count();
    QList<SARibbonCategory *> res;

    for (int i = 0; i < c; ++i)
    {
        SARibbonCategory *w = qobject_cast<SARibbonCategory *>(m_d->stackedContainerWidget->widget(i));
        if (w) {
            res.append(w);
        }
    }
    return (res);
}


SARibbonContextCategory *SARibbonBar::addContextCategory(const QString& title, const QColor& color, const QVariant& id)
{
    SARibbonContextCategory *context = RibbonSubElementDelegate->createRibbonContextCategory(this);

    context->setContextTitle(title);
    context->setId(id);
    context->setContextColor(color);
    connect(context, &SARibbonContextCategory::categoryPageAdded
        , this, &SARibbonBar::onContextsCategoryPageAdded);
    if (currentRibbonStyle() == WpsLiteStyle) {
        resizeInWpsLiteStyle();
    }
    return (context);
}


///
/// \brief 显示上下文标签
/// \param context 上下文标签指针
///
void SARibbonBar::showContextCategory(SARibbonContextCategory *context)
{
    if (isContextCategoryVisible(context)) {
        return;
    }
    ContextCategoryManagerData contextCategoryData;

    contextCategoryData.contextCategory = context;
    for (int i = 0; i < context->categoryCount(); ++i)
    {
        SARibbonCategory *category = context->categoryPage(i);
        category->setRibbonPannelLayoutMode(isTwoRowStyle() ? SARibbonPannel::TwoRowMode : SARibbonPannel::ThreeRowMode);
        //切换模式后会改变高度，上下文标签显示时要保证显示出来
        //category->setFixedHeight(categoryHeight());
        int index = m_d->ribbonTabBar->addTab(category->windowTitle());
        contextCategoryData.tabPageIndex.append(index);
        m_d->ribbonTabBar->setTabData(index, QVariant((quint64)category));
    }
    m_d->currentShowingContextCategory.append(contextCategoryData);
    if (currentRibbonStyle() == WpsLiteStyle) {
        resizeInWpsLiteStyle();
    }
    repaint();
}


void SARibbonBar::hideContextCategory(SARibbonContextCategory *context)
{
    for (int i = 0; i < m_d->currentShowingContextCategory.size(); ++i)
    {
        if (m_d->currentShowingContextCategory[i].contextCategory == context) {
            const QList<int>& indexs = m_d->currentShowingContextCategory[i].tabPageIndex;
            for (int j = indexs.size()-1; j >= 0; --j)
            {
                m_d->ribbonTabBar->removeTab(indexs[j]);
            }
            m_d->currentShowingContextCategory.removeAt(i);
        }
    }
    if (currentRibbonStyle() == WpsLiteStyle) {
        resizeInWpsLiteStyle();
    }
    repaint();
}


/**
 * @brief 判断上下文是否在显示状态
 * @param context
 * @return 在显示状态返回true
 */
bool SARibbonBar::isContextCategoryVisible(SARibbonContextCategory *context)
{
    return (m_d->isContainContextCategoryInList(context));
}


void SARibbonBar::setContextCategoryVisible(SARibbonContextCategory *context, bool visible)
{
    if (visible) {
        showContextCategory(context);
    }else {
        hideContextCategory(context);
    }
}


///
/// \brief 设置为隐藏/显示模式
/// \param isHide
///
void SARibbonBar::setHideMode(bool isHide)
{
    if (isHide) {
        m_d->setHideMode();
    }else {
        m_d->setNormalMode();
    }
    QResizeEvent resizeEvent(size(), size());

    QApplication::sendEvent(this, &resizeEvent);
}


///
/// \brief 当前ribbon是否是隐藏模式
/// \return
///
bool SARibbonBar::isRibbonBarHideMode() const
{
    return (m_d->stackedContainerWidget->isPopupMode());
}


///
/// \brief 设置显示隐藏ribbon按钮
///
void SARibbonBar::showHideModeButton(bool isShow)
{
    if (isShow) {
        activeTabBarRightButtonGroup();
        if (nullptr == m_d->hidePannelButton) {
            m_d->hidePannelButton = RibbonSubElementDelegate->createHidePannelButton(this);
            QAction *action = new QAction(m_d->hidePannelButton);
            action->setCheckable(true);
            action->setChecked(isRibbonBarHideMode());
            action->setIcon(QIcon(":/icon/icon/save.png"));
            connect(action, &QAction::triggered, this, [this](bool on) {
                this->setHideMode(on);
            });
            m_d->hidePannelButton->setDefaultAction(action);
            m_d->tabBarRightSizeButtonGroupWidget->addButton(m_d->hidePannelButton);
            update();
        }
    }else {
        if (nullptr != m_d->hidePannelButton) {
            m_d->hidePannelButton->hide();
            m_d->hidePannelButton->deleteLater();
            m_d->hidePannelButton = nullptr;
        }
    }
    QResizeEvent resizeEvent(size(), size());

    QApplication::sendEvent(this, &resizeEvent);
}


///
/// \brief 是否显示隐藏ribbon按钮
/// \return
///
bool SARibbonBar::isShowHideModeButton() const
{
    return (nullptr != m_d->hidePannelButton);
}


int SARibbonBar::tabBarHeight() const
{
    return (RibbonSubElementStyleOpt.tabBarHight);
}


int SARibbonBar::titleBarHeight() const
{
    return (RibbonSubElementStyleOpt.titleBarHight);
}


void SARibbonBar::onWindowTitleChanged(const QString& title)
{
    Q_UNUSED(title);
    update();
}


void SARibbonBar::onWindowIconChanged(const QIcon& icon)
{
    if (!icon.isNull()) {
        int iconMinSize = RibbonSubElementStyleOpt.titleBarHight - 6;
        QSize s = icon.actualSize(QSize(iconMinSize, iconMinSize));
        m_d->iconRightBorderPosition = RibbonSubElementStyleOpt.widgetBord.left()+s.width();
    }
    update();
}


void SARibbonBar::onCategoryWindowTitleChanged(const QString& title)
{
    QWidget *w = qobject_cast<QWidget *>(sender());

    for (int i = 0; i < m_d->ribbonTabBar->count(); ++i)
    {
        //鉴于tab不会很多，不考虑效率问题
        QVariant var = m_d->ribbonTabBar->tabData(i);
        QWidget *category = nullptr;
        if (var.isValid()) {
            quint64 p = var.value<quint64>();
            category = (QWidget *)p;
            if (w == category) {
                m_d->ribbonTabBar->setTabText(i, title);
            }
        }
    }
}


///
/// \brief ribbon的显示界面隐藏
///
void SARibbonBar::onStackWidgetHided()
{
    m_d->ribbonTabBar->setCurrentIndex(-1);
}


///
/// \brief ribbon tab bar改变
/// \param index
///
void SARibbonBar::onCurrentRibbonTabChanged(int index)
{
    QVariant var = m_d->ribbonTabBar->tabData(index);
    SARibbonCategory *category = nullptr;

    if (var.isValid()) {
        quint64 p = var.value<quint64>();
        category = (SARibbonCategory *)p;
    }
    if (category) {
        if (m_d->stackedContainerWidget->currentWidget() != category) {
            m_d->stackedContainerWidget->setCurrentWidget(category);
        }
        if (isRibbonBarHideMode()) {
            if (!m_d->stackedContainerWidget->isVisible()) {
                if (m_d->stackedContainerWidget->isPopupMode()) {
                    m_d->stackedContainerWidget->setFocus();
                    m_d->stackedContainerWidget->exec();
                }
            }
        }
    }
    emit currentRibbonTabChanged(index);
}


///
/// \brief ribbon tab bar点击
/// \param index
///
void SARibbonBar::onCurrentRibbonTabClicked(int index)
{
    if (isRibbonBarHideMode()) {
        if (!m_d->stackedContainerWidget->isVisible()) {
            if (m_d->stackedContainerWidget->isPopupMode()) {
                m_d->ribbonTabBar->setCurrentIndex(index);
            }
        }
    }
}


///
/// \brief ribbon tab bar双击
/// \param index
///
void SARibbonBar::onCurrentRibbonTabDoubleClicked(int index)
{
    Q_UNUSED(index);
    setHideMode(!isRibbonBarHideMode());
}


void SARibbonBar::onContextsCategoryPageAdded(SARibbonCategory *category)
{
    Q_ASSERT_X(category != nullptr, "onContextsCategoryPageAdded", "add nullptr page");
    m_d->stackedContainerWidget->addWidget(category);
}


void SARibbonBar::updateRibbonElementGeometry()
{
    //根据样式调整SARibbonCategory的布局形式
    QList<SARibbonCategory *> categorys = categoryPages();

    for (SARibbonCategory *c : categorys)
    {
        c->setRibbonPannelLayoutMode(SARibbonBar::isTwoRowStyle(currentRibbonStyle()) ? SARibbonPannel::TwoRowMode : SARibbonPannel::ThreeRowMode);
    }

    //根据样式调整bar的高度
    if (NormalRibbonMode == currentRibbonState()) {
        setFixedHeight(mainBarHeight());
    }
}


SARibbonButtonGroupWidget *SARibbonBar::activeTabBarRightButtonGroup()
{
    if (nullptr == m_d->tabBarRightSizeButtonGroupWidget) {
        m_d->tabBarRightSizeButtonGroupWidget = RibbonSubElementDelegate->craeteButtonGroupWidget(this);
        m_d->tabBarRightSizeButtonGroupWidget->setFrameShape(QFrame::NoFrame);
        m_d->tabBarRightSizeButtonGroupWidget->show();
    }
    if (!m_d->tabBarRightSizeButtonGroupWidget->isVisible()) {
        m_d->tabBarRightSizeButtonGroupWidget->setVisible(true);
    }
    return (m_d->tabBarRightSizeButtonGroupWidget);
}


SARibbonQuickAccessBar *SARibbonBar::quickAccessBar()
{
    return (m_d->quickAccessBar);
}


void SARibbonBar::setRibbonStyle(SARibbonBar::RibbonStyle v)
{
    m_d->ribbonStyle = v;
    m_d->lastShowStyle = v;
    updateRibbonElementGeometry();
    QSize oldSize = size();
    QSize newSize(oldSize.width(), mainBarHeight());

    QApplication::postEvent(this, new QResizeEvent(newSize, oldSize));
}


SARibbonBar::RibbonStyle SARibbonBar::currentRibbonStyle() const
{
    return (m_d->ribbonStyle);
}


SARibbonBar::RibbonState SARibbonBar::currentRibbonState() const
{
    return (m_d->currentRibbonMode);
}


int SARibbonBar::unusableTitleRegion() const
{
    return (RibbonSubElementStyleOpt.unusableTitleRegion);
}


///
/// \brief 切换到对应标签
/// \param index 标签索引
///
void SARibbonBar::setCurrentIndex(int index)
{
	m_d->ribbonTabBar->setCurrentIndex(index);
	//onCurrentRibbonTabChanged(index);
}


///
/// \brief 确保标签显示出来，tab并切换到对应页
/// \param category 标签指针
///
void SARibbonBar::raiseCategory(SARibbonCategory *category)
{
	int index = m_d->stackedContainerWidget->indexOf(category);

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
    return (SARibbonBar::isTwoRowStyle(m_d->ribbonStyle));
}


/**
 * @brief 判断当前的样式是否为office样式
 * @return
 */
bool SARibbonBar::isOfficeStyle() const
{
    return (SARibbonBar::isOfficeStyle(currentRibbonStyle()));
}


bool SARibbonBar::eventFilter(QObject *obj, QEvent *e)
{
    if (obj) {
        //调整多文档时在窗口模式下的按钮更新
        if ((obj == cornerWidget(Qt::TopLeftCorner)) || (obj == cornerWidget(Qt::TopRightCorner))) {
            if ((QEvent::UpdateLater == e->type()) ||
                (QEvent::MouseButtonRelease == e->type()) ||
                (QEvent::WindowActivate == e->type())) {
                QApplication::postEvent(this, new QResizeEvent(size(), size()));
            }
        }else if (obj == m_d->stackedContainerWidget) {
            //在stack 是popup模式时，点击的是stackedContainerWidget区域外的时候，如果是在ribbonTabBar上点击
            //那么忽略掉这次点击，把点击下发到ribbonTabBar,这样可以避免stackedContainerWidget在点击ribbonTabBar后
            //隐藏又显示，出现闪烁
            if (QEvent::MouseButtonPress == e->type()) {
                if (m_d->stackedContainerWidget->isPopupMode()) {
                    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(e);
                    if (!m_d->stackedContainerWidget->rect().contains(mouseEvent->pos())) {
                        QWidget *clickedWidget = QApplication::widgetAt(mouseEvent->globalPos());
                        if (clickedWidget == m_d->ribbonTabBar) {
                            const QPoint targetPoint = clickedWidget->mapFromGlobal(mouseEvent->globalPos());
                            QMouseEvent *evPress = new QMouseEvent(mouseEvent->type(), targetPoint, mouseEvent->globalPos(), mouseEvent->button(), mouseEvent->buttons(), mouseEvent->modifiers());
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
    //20200831
    //tabBarWidth的宽度原来为endX - x;，现需要根据实际进行调整
    //为了把tabbar没有tab的部分不占用，这里的宽度需要根据tab的size来进行设置，让tabbar的长度刚刚好，这样能让出
    //mainwindow的空间，接受鼠标事件，从而实现拖动等操作，否则tabbar占用整个顶栏，鼠标无法点击到mainwindow
    //计算tab所占用的宽度
    const QMargins& mg = m_d->ribbonTabBar->tabMargin();
    int mintabBarWidth = 0;

    for (int i = 0; i < m_d->ribbonTabBar->count(); ++i)
    {
        mintabBarWidth += m_d->ribbonTabBar->tabRect(i).width();
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
    switch (currentRibbonStyle())
    {
    case OfficeStyle:
        return (RibbonSubElementStyleOpt.mainbarHeightOfficeStyleThreeRow);

    case WpsLiteStyle:
        return (RibbonSubElementStyleOpt.mainbarHeightWPSStyleThreeRow);

    case OfficeStyleTwoRow:
        //之前如果是3行，要改变所有的Category行数
        return (RibbonSubElementStyleOpt.mainbarHeightOfficeStyleTwoRow);

    case WpsLiteStyleTwoRow:
        return (RibbonSubElementStyleOpt.mainbarHeightWPSStyleTwoRow);

    default:
        break;
    }
    return (RibbonSubElementStyleOpt.mainbarHeightOfficeStyleThreeRow);
}


void SARibbonBar::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    switch (m_d->ribbonStyle)
    {
    case OfficeStyle:
        paintInNormalStyle();
        break;

    case WpsLiteStyle:
        paintInWpsLiteStyle();
        break;

    default:
        paintInNormalStyle();
        break;
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
    QList<ContextCategoryManagerData> contextCategoryDataList = m_d->currentShowingContextCategory;
    bool isCurrentSelectContextCategoryPage = false;

    QPoint contextCategoryRegion = QPoint(width(), -1);

    for (int i = 0; i < contextCategoryDataList.size(); ++i)
    {
        QRect contextTitleRect;
        QList<int> indexs = contextCategoryDataList[i].tabPageIndex;
        QColor clr = contextCategoryDataList[i].contextCategory->contextColor();
        if (!indexs.isEmpty()) {
            contextTitleRect = m_d->ribbonTabBar->tabRect(indexs.first());
            QRect endRect = m_d->ribbonTabBar->tabRect(indexs.last());
            contextTitleRect.setRight(endRect.right());
            contextTitleRect.translate(m_d->ribbonTabBar->x(), m_d->ribbonTabBar->y());
            contextTitleRect.setHeight(m_d->ribbonTabBar->height());
            contextTitleRect -= m_d->ribbonTabBar->tabMargin();
            //把区域顶部扩展到窗口顶部
            contextTitleRect.setTop(RibbonSubElementStyleOpt.widgetBord.top());
            //绘制
            paintContextCategoryTab(p
                , contextCategoryDataList[i].contextCategory->contextTitle()
                , contextTitleRect
                , clr);
            //更新上下文标签的范围，用于控制标题栏的显示
            if (contextTitleRect.left() < contextCategoryRegion.x()) {
                contextCategoryRegion.setX(contextTitleRect.left());
            }
            if (contextTitleRect.right() > contextCategoryRegion.y()) {
                contextCategoryRegion.setY(contextTitleRect.right());
            }
        }
        isCurrentSelectContextCategoryPage = indexs.contains(m_d->ribbonTabBar->currentIndex());
        if (isCurrentSelectContextCategoryPage) {
            QPen pen;
            pen.setColor(clr);
            pen.setWidth(1);
            p.setPen(pen);
            p.setBrush(Qt::NoBrush);
            p.drawRect(m_d->stackedContainerWidget->geometry());
            isCurrentSelectContextCategoryPage = false;
        }
    }
    p.restore();
    //! 显示标题等
    QWidget *parWindow = parentWidget();

    if (parWindow) {
        QRect titleRegion;
        if (contextCategoryRegion.y() < 0) {
            titleRegion.setRect(m_d->quickAccessBar->geometry().right()
                , RibbonSubElementStyleOpt.widgetBord.top()
                , width()- m_d->iconRightBorderPosition - RibbonSubElementStyleOpt.widgetBord.right()
                , RibbonSubElementStyleOpt.titleBarHight);
        }else {
            int x = m_d->quickAccessBar->geometry().right();
            if (width() - contextCategoryRegion.y() > contextCategoryRegion.x()-x) {
                //说明左边的区域大一点，标题显示在坐标
                titleRegion.setRect(contextCategoryRegion.y()
                    , RibbonSubElementStyleOpt.widgetBord.top()
                    , width()-x-RibbonSubElementStyleOpt.unusableTitleRegion
                    , RibbonSubElementStyleOpt.titleBarHight);
            }else {
                //说明右边的大一点
                titleRegion.setRect(m_d->iconRightBorderPosition
                    , RibbonSubElementStyleOpt.widgetBord.top()
                    , contextCategoryRegion.x()-m_d->iconRightBorderPosition-RibbonSubElementStyleOpt.unusableTitleRegion
                    , RibbonSubElementStyleOpt.titleBarHight);
            }
        }

        paintWindowTitle(p, parWindow->windowTitle(), titleRegion);
        paintWindowIcon(p, parWindow->windowIcon());
    }
//    QStyleOptionMenuItem menuOpt;
//    menuOpt.palette = palette();
//    menuOpt.state = QStyle::State_None;
//    menuOpt.menuItemType = QStyleOptionMenuItem::EmptyArea;
//    menuOpt.checkType = QStyleOptionMenuItem::NotCheckable;
//    menuOpt.rect = rect();
//    menuOpt.menuRect = rect();
//    style()->drawControl(QStyle::CE_MenuBarEmptyArea, &menuOpt, &p, this);
    //    QWidget::paintEvent(e);
}


void SARibbonBar::paintInWpsLiteStyle()
{
    QPainter p(this);

    //!
    paintBackground(p);
    //! 显示上下文标签
    p.save();
    QList<ContextCategoryManagerData> contextCategoryDataList = m_d->currentShowingContextCategory;
    bool isCurrentSelectContextCategoryPage = false;

    for (int i = 0; i < contextCategoryDataList.size(); ++i)
    {
        QRect contextTitleRect;
        QList<int> indexs = contextCategoryDataList[i].tabPageIndex;
        QColor clr = contextCategoryDataList[i].contextCategory->contextColor();
        if (!indexs.isEmpty()) {
            contextTitleRect = m_d->ribbonTabBar->tabRect(indexs.first());
            QRect endRect = m_d->ribbonTabBar->tabRect(indexs.last());
            contextTitleRect.setRight(endRect.right());
            contextTitleRect.translate(m_d->ribbonTabBar->x(), m_d->ribbonTabBar->y());
            contextTitleRect.setHeight(m_d->ribbonTabBar->height());
            contextTitleRect -= m_d->ribbonTabBar->tabMargin();
            //把区域顶部扩展到窗口顶部
            contextTitleRect.setTop(RibbonSubElementStyleOpt.widgetBord.top());
            //绘制
            paintContextCategoryTab(p
                , QString()
                , contextTitleRect
                , clr);
        }
        isCurrentSelectContextCategoryPage = indexs.contains(m_d->ribbonTabBar->currentIndex());
        if (isCurrentSelectContextCategoryPage) {
            QPen pen;
            pen.setColor(clr);
            pen.setWidth(1);
            p.setPen(pen);
            p.setBrush(Qt::NoBrush);
            p.drawRect(m_d->stackedContainerWidget->geometry());
            isCurrentSelectContextCategoryPage = false;
        }
    }
    p.restore();
    //! 显示标题等


    QWidget *parWindow = parentWidget();

    if (parWindow) {
        int start = m_d->ribbonTabBar->x() +  m_d->ribbonTabBar->width();
        int width = m_d->quickAccessBar->x() - start;
        if (width > 20) {
            QRect titleRegion(start
                , RibbonSubElementStyleOpt.widgetBord.top()
                , width
                , RibbonSubElementStyleOpt.titleBarHight);
            paintWindowTitle(p, parWindow->windowTitle(), titleRegion);
            paintWindowIcon(p, parWindow->windowIcon());
        }
    }
}


void SARibbonBar::paintContextCategoryTab(QPainter& painter, const QString& title, QRect contextRect, const QColor& color)
{
    //绘制上下文标签
    //首先有5像素的实体
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    painter.drawRect(QRect(contextRect.x(), RibbonSubElementStyleOpt.widgetBord.top(), contextRect.width(), 5));
    int yStart = contextRect.y()+5;

    //剩下的是渐变颜色
    contextRect -= QMargins(0, 5, 0, 0);
    QColor gColor = color;
    QLinearGradient lineGradient;

    lineGradient.setStart(contextRect.x(), yStart);
    lineGradient.setFinalStop(contextRect.x(), contextRect.bottom());
    gColor.setAlpha(150);
    lineGradient.setColorAt(0, gColor);
    gColor.setAlpha(0);
    lineGradient.setColorAt(0.9, gColor);
    painter.fillRect(contextRect, lineGradient);
    //绘制标题
    //TODO 判断如果颜色很接近，需要替换为白色
    if (!title.isEmpty()) {
        contextRect.setBottom(m_d->ribbonTabBar->geometry().top());
        painter.setPen(Qt::black);
        painter.setBrush(Qt::NoBrush);
        painter.drawText(contextRect, Qt::AlignCenter, title);
    }
}


void SARibbonBar::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e);
    if (isOfficeStyle()) {
        resizeInOfficeStyle();
    }else {
        resizeInWpsLiteStyle();
    }
}

/**
 * @brief 重写moveevent是为了在移动时调整isPopupMode状态下的stackedContainerWidget位置
 * @param event
 */
void SARibbonBar::moveEvent(QMoveEvent *event)
{
    if(m_d->stackedContainerWidget)
    {
        if (m_d->stackedContainerWidget->isPopupMode()) {
            //弹出模式时，高度
            QPoint absPosition = mapToGlobal(QPoint(RibbonSubElementStyleOpt.widgetBord.left(), m_d->ribbonTabBar->geometry().bottom()+1));
            m_d->stackedContainerWidget->setGeometry(QRect(absPosition.x(), absPosition.y()
                , width()-RibbonSubElementStyleOpt.widgetBord.left()-RibbonSubElementStyleOpt.widgetBord.right()
                , mainBarHeight()-m_d->ribbonTabBar->geometry().bottom()-2-RibbonSubElementStyleOpt.widgetBord.bottom()));
        }
    }
    QWidget::moveEvent(event);
}


void SARibbonBar::resizeInOfficeStyle()
{
//    RibbonStyle s = m_d->lastShowStyle;

//    if (!SARibbonBar::isOfficeStyle(s)) {
//        //强制设置为office style
//        s = static_cast<RibbonStyle>(s & 0xFF00);
//    }
//    m_d->lastShowStyle = s;
    updateRibbonElementGeometry();
    int x = RibbonSubElementStyleOpt.widgetBord.left();
    int y = RibbonSubElementStyleOpt.widgetBord.top();

    //cornerWidget - TopLeftCorner
    x += m_d->iconRightBorderPosition+5;
    if (QWidget *connerL = cornerWidget(Qt::TopLeftCorner)) {
        if (connerL->isVisible()) {
            QSize connerSize = connerL->sizeHint();
            int detal = (RibbonSubElementStyleOpt.titleBarHight - connerSize.height()) / 2;
            connerL->setGeometry(x, y+detal
                , connerSize.width(), connerSize.height());
            x = connerL->geometry().right() + 5;
        }
    }
    //quick access bar定位
    if (m_d->quickAccessBar) {
        if (m_d->quickAccessBar->isVisible()) {
            QSize quickAccessBarSize = m_d->quickAccessBar->sizeHint();
            m_d->quickAccessBar->setGeometry(x, y
                , quickAccessBarSize.width(), RibbonSubElementStyleOpt.titleBarHight);
            x = m_d->quickAccessBar->geometry().right() + 5;
        }
    }
    x = RibbonSubElementStyleOpt.widgetBord.left();
    y = RibbonSubElementStyleOpt.titleBarHight+RibbonSubElementStyleOpt.widgetBord.top();
    //applitionButton 定位
    if (m_d->applitionButton) {
        if (m_d->applitionButton->isVisible()) {
            m_d->applitionButton->move(x, y);
            x = m_d->applitionButton->geometry().right();
        }
    }
    //cornerWidget - TopRightCorner
    int endX = width()-RibbonSubElementStyleOpt.widgetBord.right();

    if (QWidget *connerW = cornerWidget(Qt::TopRightCorner)) {
        if (connerW->isVisible()) {
            QSize connerSize = connerW->sizeHint();
            int detal = (RibbonSubElementStyleOpt.titleBarHight - connerSize.height()) / 2;
            endX -= connerSize.width();
            connerW->setGeometry(endX, y+detal
                , connerSize.width(), connerSize.height());
        }
    }
    //tab bar 定位
    //tabBar 右边的附加按钮组
    if (m_d->tabBarRightSizeButtonGroupWidget && m_d->tabBarRightSizeButtonGroupWidget->isVisible()) {
        QSize wSize = m_d->tabBarRightSizeButtonGroupWidget->sizeHint();
        endX -= wSize.width();
        m_d->tabBarRightSizeButtonGroupWidget->setGeometry(endX
            , y
            , wSize.width()
            , RibbonSubElementStyleOpt.tabBarHight-2);
    }
    int tabBarWidth = endX - x;

    m_d->ribbonTabBar->setGeometry(x
        , y
        , tabBarWidth
        , RibbonSubElementStyleOpt.tabBarHight);


    if (m_d->stackedContainerWidget->isPopupMode()) {
        //弹出模式时，高度
        QPoint absPosition = mapToGlobal(QPoint(RibbonSubElementStyleOpt.widgetBord.left(), m_d->ribbonTabBar->geometry().bottom()+1));
        m_d->stackedContainerWidget->setGeometry(QRect(absPosition.x(), absPosition.y()
            , width()-RibbonSubElementStyleOpt.widgetBord.left()-RibbonSubElementStyleOpt.widgetBord.right()
            , mainBarHeight()-m_d->ribbonTabBar->geometry().bottom()-2-RibbonSubElementStyleOpt.widgetBord.bottom()));
    }else {
        m_d->stackedContainerWidget->setGeometry(RibbonSubElementStyleOpt.widgetBord.left()
            , m_d->ribbonTabBar->geometry().bottom()+1
            , width()-RibbonSubElementStyleOpt.widgetBord.left()-RibbonSubElementStyleOpt.widgetBord.right()
            , mainBarHeight()-m_d->ribbonTabBar->geometry().bottom()-2-RibbonSubElementStyleOpt.widgetBord.bottom());
    }
}


void SARibbonBar::resizeInWpsLiteStyle()
{
//    RibbonStyle s = m_d->lastShowStyle;

//    if (SARibbonBar::isOfficeStyle(s)) {
//        //强制设置为wps style
//        s = static_cast<RibbonStyle>(s & 0xFF01);
//    }
//    m_d->lastShowStyle = s;
    updateRibbonElementGeometry();
    //m_d->lastShowStyle = WpsLiteStyle;
    int x = RibbonSubElementStyleOpt.widgetBord.left();
    int y = RibbonSubElementStyleOpt.widgetBord.top();
    int validTitleBarHeight = RibbonSubElementStyleOpt.titleBarHight - RibbonSubElementStyleOpt.widgetBord.top();

    //applitionButton 定位
    if (m_d->applitionButton) {
        if (m_d->applitionButton->isVisible()) {
            m_d->applitionButton->move(x, y);
            x = m_d->applitionButton->geometry().right();
        }
    }
    //cornerWidget - TopRightCorner
    int endX = width() - RibbonSubElementStyleOpt.widgetBord.right()-RibbonSubElementStyleOpt.unusableTitleRegion;

    if (QWidget *connerW = cornerWidget(Qt::TopRightCorner)) {
        if (connerW->isVisible()) {
            QSize connerSize = connerW->sizeHint();
            int detal = (validTitleBarHeight - connerSize.height()) / 2;
            endX -= connerSize.width();
            connerW->setGeometry(endX, y+detal
                , connerSize.width(), connerSize.height());
        }
    }
    //quick access bar定位
    if (m_d->quickAccessBar) {
        if (m_d->quickAccessBar->isVisible()) {
            QSize quickAccessBarSize = m_d->quickAccessBar->sizeHint();
            endX -= quickAccessBarSize.width();
            m_d->quickAccessBar->setGeometry(endX, y
                , quickAccessBarSize.width(), validTitleBarHeight);
        }
    }
    //cornerWidget - TopLeftCorner
    if (QWidget *connerL = cornerWidget(Qt::TopLeftCorner)) {
        if (connerL->isVisible()) {
            QSize connerSize = connerL->sizeHint();
            int detal = (validTitleBarHeight - connerSize.height()) / 2;
            endX -= connerSize.width();
            connerL->setGeometry(endX, y+detal
                , connerSize.width(), connerSize.height());
        }
    }
    //tab bar 定位 wps模式下applitionButton的右边就是tab bar
    //tabBar 右边的附加按钮组
    if (m_d->tabBarRightSizeButtonGroupWidget && m_d->tabBarRightSizeButtonGroupWidget->isVisible()) {
        QSize wSize = m_d->tabBarRightSizeButtonGroupWidget->sizeHint();
        endX -= wSize.width();
        m_d->tabBarRightSizeButtonGroupWidget->setGeometry(endX
            , y
            , wSize.width()
            , validTitleBarHeight);
    }


    int tabBarHight = validTitleBarHeight - 2;
    int tabBarWidth = endX - x;
    //20200831
    //tabBarWidth的宽度原来为endX - x;，现需要根据实际进行调整
    //为了把tabbar没有tab的部分不占用，这里的宽度需要根据tab的size来进行设置，让tabbar的长度刚刚好，这样能让出
    //mainwindow的空间，接受鼠标事件，从而实现拖动等操作，否则tabbar占用整个顶栏，鼠标无法点击到mainwindow
    //计算tab所占用的宽度
    int mintabBarWidth = calcMinTabBarWidth();

    if (mintabBarWidth < tabBarWidth) {
        tabBarWidth = mintabBarWidth;
    }
    y += 2;
    m_d->ribbonTabBar->setGeometry(x
        , y
        , tabBarWidth
        , tabBarHight);

    //调整整个stackedContainer
    if (m_d->stackedContainerWidget->isPopupMode()) {
        QPoint absPosition = mapToGlobal(QPoint(RibbonSubElementStyleOpt.widgetBord.left(), m_d->ribbonTabBar->geometry().bottom()+1));
        m_d->stackedContainerWidget->setGeometry(QRect(absPosition.x(), absPosition.y()
            , width()-RibbonSubElementStyleOpt.widgetBord.left()-RibbonSubElementStyleOpt.widgetBord.right()
            , mainBarHeight()-m_d->ribbonTabBar->geometry().bottom()-2-RibbonSubElementStyleOpt.widgetBord.bottom()));
    }else {
        m_d->stackedContainerWidget->setGeometry(RibbonSubElementStyleOpt.widgetBord.left()
            , m_d->ribbonTabBar->geometry().bottom()+1
            , width()-RibbonSubElementStyleOpt.widgetBord.left()-RibbonSubElementStyleOpt.widgetBord.right()
            , mainBarHeight()-m_d->ribbonTabBar->geometry().bottom()-2-RibbonSubElementStyleOpt.widgetBord.bottom());
    }
}


void SARibbonBar::paintBackground(QPainter& painter)
{
    painter.save();
    QPalette pl = palette();

    painter.setBrush(pl.background());
    painter.drawRect(rect());
    //在tabbar下绘制一条线
    const int lineY = m_d->ribbonTabBar->geometry().bottom();
    QPen pen(RibbonSubElementStyleOpt.tabBarBaseLineColor);

    pen.setWidth(1);
    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);
    painter.drawLine(QPoint(RibbonSubElementStyleOpt.widgetBord.left(), lineY), QPoint(width() - RibbonSubElementStyleOpt.widgetBord.right(), lineY));
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
    painter.save();
    painter.setPen(RibbonSubElementStyleOpt.titleTextColor);
    painter.drawText(titleRegion, Qt::AlignCenter, title);
    painter.restore();
}


void SARibbonBar::paintWindowIcon(QPainter& painter, const QIcon& icon)
{
    painter.save();
    if (!icon.isNull()) {
        int iconMinSize = RibbonSubElementStyleOpt.titleBarHight - 6;
        icon.paint(&painter, RibbonSubElementStyleOpt.widgetBord.left()+3, RibbonSubElementStyleOpt.widgetBord.top()+3
            , iconMinSize, iconMinSize);
        m_d->iconRightBorderPosition = RibbonSubElementStyleOpt.widgetBord.left()+iconMinSize;
    }else {
        m_d->iconRightBorderPosition = RibbonSubElementStyleOpt.widgetBord.left();
    }
    painter.restore();
}
