#include "SARibbonCategory.h"
#include <QList>
#include <QResizeEvent>
#include <QPainter>
#include <QLinearGradient>
#include <QDebug>
#include <QApplication>
#include <QDesktopWidget>
#include "SARibbonToolButton.h"
#include <QHBoxLayout>
#include "SARibbonSeparatorWidget.h"
#include "SARibbonCategoryLayout.h"
#include "SARibbonElementManager.h"

#include <QList>
#include <QMap>
class SARibbonCategoryProxyPrivate
{
public:
    SARibbonCategory *mParent;
    SARibbonPannel::PannelLayoutMode mDefaultPannelLayoutMode;
    QList<SARibbonPannel *> mPannelLists;
    QList<SARibbonSeparatorWidget *> mSeparatorLists;
    SARibbonCategoryScrollButton *mLeftScrollBtn;
    SARibbonCategoryScrollButton *mRightScrollBtn;
    int mBaseX;
    SARibbonCategoryProxyPrivate(SARibbonCategory *p);
};

SARibbonCategoryScrollButton::SARibbonCategoryScrollButton(Qt::ArrowType arr, QWidget *p)
    : QToolButton(p)
{
    setArrowType(arr);
}


SARibbonCategory::SARibbonCategory(QWidget *parent)
    : QWidget(parent)
    , m_proxy(new SARibbonCategoryProxy(this))
{
    setAutoFillBackground(true);
    setBackgroundBrush(Qt::white);
}


SARibbonCategory::~SARibbonCategory()
{
}


/**
 * @brief 设置pannel的模式
 *
 * 在@ref SARibbonBar 调用@ref SARibbonBar::setRibbonStyle 函数时，会对所有的SARibbonCategory调用此函数
 * 把新的SARibbonPannel::PannelLayoutMode设置进去
 * @param m
 */
void SARibbonCategory::setRibbonPannelLayoutMode(SARibbonPannel::PannelLayoutMode m)
{
    proxy()->setRibbonPannelLayoutMode(m);
}


SARibbonPannel::PannelLayoutMode SARibbonCategory::ribbonPannelLayoutMode() const
{
    return (proxy()->ribbonPannelLayoutMode());
}


/**
 * @brief 添加pannel
 *
 * @note pannel的所有权由SARibbonCategory来管理，请不要在外部对其进行销毁
 * @param title pannel的标题，在office/wps的三行模式下会显示在pannel的下方
 * @return 返回生成的@ref SARibbonPannel 指针
 * @see 对SARibbonPannel的其他操作，参考 @ref SARibbonCategory::takePannel
 */
SARibbonPannel *SARibbonCategory::addPannel(const QString& title)
{
    return (proxy()->addPannel(title));
}


/**
 * @brief 添加pannel
 * @param pannel的所有权SARibbonCategory来管理
 */
void SARibbonCategory::addPannel(SARibbonPannel *pannel)
{
    proxy()->addPannel(pannel);
}


/**
 * @brief 把pannel脱离SARibbonCategory的管理
 * @param 需要提取的pannel
 * @return 成功返回true，否则返回false
 */
bool SARibbonCategory::takePannel(SARibbonPannel *pannel)
{
    return (proxy()->takePannel(pannel));
}


/**
 * @brief 移除Pannel，Category会直接回收SARibbonPannel内存
 * @param pannel 需要移除的pannel
 * @note 移除后pannel为野指针，一般操作完建议把pannel指针设置为nullptr
 *
 * 此操作等同于：
 *
 * @code
 * SARibbonPannel* pannel;
 * ...
 * category->takePannel(pannel);
 * delete pannel;
 * @endcode
 */
bool SARibbonCategory::removePannel(SARibbonPannel *pannel)
{
    return (proxy()->removePannel(pannel));
}


///
/// \brief SARibbonCategory::setBackgroundBrush
/// \param brush
///
void SARibbonCategory::setBackgroundBrush(const QBrush& brush)
{
    proxy()->setBackgroundBrush(brush);
}


//================================
// SARibbonCategoryProxyPrivate
//================================


SARibbonCategoryProxyPrivate::SARibbonCategoryProxyPrivate(SARibbonCategory *p)
    : mParent(p)
    , mDefaultPannelLayoutMode(SARibbonPannel::ThreeRowMode)
    , mLeftScrollBtn(nullptr)
    , mRightScrollBtn(nullptr)
    , mBaseX(0)
{
}


SARibbonCategoryProxy *SARibbonCategory::proxy()
{
    return (m_proxy.data());
}


const SARibbonCategoryProxy *SARibbonCategory::proxy() const
{
    return (m_proxy.data());
}


/**
 * @brief 设置SARibbonCategory的代理
 *
 * 由于SARibbonCategory是由SARibbonBar生成的，如果用户想做修改，继承SARibbonCategory是没有用的
 * 因此可通过代理设置进当前的SARibbonCategory，SARibbonCategory的大部分工作都是由代理完成
 * 这样替代了继承SARibbonCategory的方式
 * @param proxy
 */
void SARibbonCategory::setProxy(SARibbonCategoryProxy *proxy)
{
    m_proxy.reset(proxy);
}


/**
 * @brief 返回Category下的所有pannel
 * @return
 */
const QList<SARibbonPannel *>& SARibbonCategory::pannelList() const
{
    return (m_proxy->pannelList());
}


/**
 * @brief 返回Category下的所有pannel
 * @return
 */
QList<SARibbonPannel *>& SARibbonCategory::pannelList()
{
    return (m_proxy->pannelList());
}


void SARibbonCategory::resizeEvent(QResizeEvent *event)
{
    proxy()->resizeEvent(event);
    QWidget::resizeEvent(event);
}


void SARibbonCategory::paintEvent(QPaintEvent *event)
{
    proxy()->paintEvent(event);
    QWidget::paintEvent(event);
}


//////////////////////////////////////////////////////////////////

SARibbonCategoryProxy::SARibbonCategoryProxy(SARibbonCategory *parent)
    : m_d(new SARibbonCategoryProxyPrivate(parent))
{
    m_d->mLeftScrollBtn = new SARibbonCategoryScrollButton(Qt::LeftArrow, parent);
    m_d->mRightScrollBtn = new SARibbonCategoryScrollButton(Qt::RightArrow, parent);
    m_d->mLeftScrollBtn->setVisible(false);
    m_d->mRightScrollBtn->setVisible(false);
    connect(m_d->mLeftScrollBtn, &QPushButton::clicked, this, &SARibbonCategoryProxy::onLeftScrollButtonClicked);
    connect(m_d->mRightScrollBtn, &QPushButton::clicked, this, &SARibbonCategoryProxy::onRightScrollButtonClicked);
}


SARibbonCategoryProxy::~SARibbonCategoryProxy()
{
    delete m_d->mLeftScrollBtn;
    delete m_d->mRightScrollBtn;
    delete m_d;
}


SARibbonPannel *SARibbonCategoryProxy::addPannel(const QString& title)
{
    SARibbonPannel *pannel = RibbonSubElementDelegate->createRibbonPannel(ribbonCategory());

    pannel->setWindowTitle(title);
    pannel->setPannelLayoutMode(ribbonPannelLayoutMode());
    SARibbonSeparatorWidget *seprator = RibbonSubElementDelegate->createRibbonSeparatorWidget(ribbonCategory()->height(), ribbonCategory());

    pannel->installEventFilter(this);
    m_d->mPannelLists.append(pannel);
    m_d->mSeparatorLists.append(seprator);
    return (pannel);
}


void SARibbonCategoryProxy::addPannel(SARibbonPannel *pannel)
{
    if (pannel->parent() != ribbonCategory()) {
        pannel->setParent(ribbonCategory());
    }
    pannel->setPannelLayoutMode(ribbonPannelLayoutMode());
    SARibbonSeparatorWidget *seprator = RibbonSubElementDelegate->createRibbonSeparatorWidget(ribbonCategory()->height(), ribbonCategory());
    //建立监控，监控show,hide事件，用于触发重新布局
    pannel->installEventFilter(this);
    m_d->mPannelLists.append(pannel);
    m_d->mSeparatorLists.append(seprator);
}


bool SARibbonCategoryProxy::takePannel(SARibbonPannel *pannel)
{
    if (m_d->mPannelLists.removeOne(pannel)) {
        resizePannels(m_d->mParent->size());
        //一定要移除事件监控
        pannel->removeEventFilter(this);
        return (true);
    }
    return (false);
}


bool SARibbonCategoryProxy::removePannel(SARibbonPannel *pannel)
{
    if (takePannel(pannel)) {
        delete pannel;
        resizePannels(m_d->mParent->size());
        return (true);
    }
    return (false);
}


void SARibbonCategoryProxy::setBackgroundBrush(const QBrush& brush)
{
    QPalette p = ribbonCategory()->palette();

    p.setBrush(QPalette::Background, brush);
    ribbonCategory()->setPalette(p);
}


void SARibbonCategoryProxy::resizeEvent(QResizeEvent *event)
{
    resizePannels(event->size());
}


void SARibbonCategoryProxy::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    return;
}


void SARibbonCategoryProxy::resizePannels(const QSize& categorySize)
{
    const int dy = 0;
    //原始的尺寸
    QList<QPair<QWidget *, QSize> > primitiveWidgetSize = allVisibleCategoryWidgetSizes();
    const int size = primitiveWidgetSize.size();
    int totalWidth = 0;

    for (int i = 0; i < size; ++i)
    {
        //求取总宽度
        totalWidth += primitiveWidgetSize[i].second.width();
        //强制设置所有窗口的高度
        primitiveWidgetSize[i].second.setHeight(categorySize.height() - dy);
    }
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
//    qDebug()	<< "SARibbonCategoryProxy::resizePannels,name:" << m_d->mParent->windowTitle()
//            << " categorySize:" << categorySize
//            << " totalWidth:" << totalWidth
//            << " primitiveWidgetSize size:" << primitiveWidgetSize.size()
//            << " m_d->mBaseX:" << m_d->mBaseX
//               ;
#endif
    //判断是否超过总长度
    if (totalWidth > categorySize.width()) {
        //超过总长度，需要显示滚动按钮
        if (0 == m_d->mBaseX) {
            //已经移动到最左，需要可以向右移动
            m_d->mRightScrollBtn->setVisible(true);
            m_d->mLeftScrollBtn->setVisible(false);
            m_d->mRightScrollBtn->raise();
            m_d->mRightScrollBtn->show();
        }else if (m_d->mBaseX <= (categorySize.width() - totalWidth)) {
            //已经移动到最右，需要可以向左移动
            m_d->mRightScrollBtn->setVisible(false);
            m_d->mLeftScrollBtn->setVisible(true);
            m_d->mLeftScrollBtn->raise();
            m_d->mLeftScrollBtn->show();
        }else {
            //移动到中间两边都可以动
            m_d->mRightScrollBtn->setVisible(true);
            m_d->mLeftScrollBtn->setVisible(true);
            m_d->mRightScrollBtn->raise();
            m_d->mLeftScrollBtn->raise();
            m_d->mRightScrollBtn->show();
            m_d->mLeftScrollBtn->show();
        }
    }else{
        m_d->mRightScrollBtn->setVisible(false);
        m_d->mLeftScrollBtn->setVisible(false);
        //这个是避免一开始totalWidth > categorySize.width()，通过滚动按钮调整了m_d->mBaseX
        //随之调整了窗体尺寸，调整后totalWidth < categorySize.width()导致category在原来位置
        //无法显示，必须这里把mBaseX设置为0
        m_d->mBaseX = 0;
    }
    //调整两个滚动按钮的位置
    m_d->mLeftScrollBtn->setGeometry(0, 0, 12, categorySize.height());
    m_d->mRightScrollBtn->setGeometry(categorySize.width()-12, 0, 12, categorySize.height());
    //调整窗口位置
    int x = m_d->mBaseX;
    for (int i = 0; i < size; ++i)
    {
        QWidget *w = primitiveWidgetSize[i].first;
        QSize s = primitiveWidgetSize[i].second;
        if (!w->isHidden()) {
            w->setFixedSize(s);
            w->move(x, dy);
            //w->setGeometry(x, dy, s.width(), categorySize.height() - dy);
        }
        x += s.width() + 1;
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
//        qDebug()	<< "==> w:" << w->metaObject()->className()
//                << " isVisible "<< w->isVisible()
//                << " is isHidden:" << w->isHidden()
//                << " Geometry" << w->geometry()
//        ;
#endif
    }
}


void SARibbonCategoryProxy::resizePannels()
{
    resizePannels(m_d->mParent->size());
}


const QList<SARibbonPannel *>& SARibbonCategoryProxy::pannelList() const
{
    return (m_d->mPannelLists);
}


QList<SARibbonPannel *>& SARibbonCategoryProxy::pannelList()
{
    return (m_d->mPannelLists);
}


bool SARibbonCategoryProxy::eventFilter(QObject *watched, QEvent *event)
{
    if (nullptr == watched) {
        return (false);
    }
    SARibbonPannel *pannel = qobject_cast<SARibbonPannel *>(watched);

    if (pannel) {
        switch (event->type())
        {

        case QEvent::HideToParent:
        {
            //隐藏和显示都要重新布局
            int i = m_d->mPannelLists.indexOf(pannel);
            if ((i >= 0) && (i < m_d->mSeparatorLists.size())) {
                m_d->mSeparatorLists[i]->hide();
            }
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
            qDebug()	<< "SARibbonCategoryProxy::eventFilter pannel:"
                    << pannel->windowTitle()
                    << " HideToParent Event"
            ;
#endif
            resizePannels();
        }
        break;

        case QEvent::ShowToParent:
        {
            //隐藏和显示都要重新布局
            int i = m_d->mPannelLists.indexOf(pannel);
            if ((i >= 0) && (i < m_d->mSeparatorLists.size())) {
                m_d->mSeparatorLists[i]->show();
            }
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
            qDebug()	<< "SARibbonCategoryProxy::eventFilter pannel:"
                    << pannel->windowTitle()
                    << " ShowToParent Event"
            ;
#endif
            resizePannels();
        }
        break;

        default:
            break;
        }
    }
    return (false);
}


void SARibbonCategoryProxy::onLeftScrollButtonClicked()
{
    int width = m_d->mParent->width();
    QList<QPair<QWidget *, QSize> > w = allVisibleCategoryWidgetSizes();
    //求总宽
    int totalWidth = 0;
    for (QPair<QWidget *, QSize> r : w)
    {
        totalWidth += r.second.width();
    }
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
    qDebug() << "= SARibbonCategoryProxy::onLeftScrollButtonClicked totalWidth:"
             << totalWidth
             << " width:"<<width
             << " m_d->mBaseX" << m_d->mBaseX
                ;
#endif
    if (totalWidth > width) {
        int tmp = m_d->mBaseX + width;
        if (tmp > 0) {
            tmp = 0;
        }
        m_d->mBaseX = tmp;
    }else{
        m_d->mBaseX = 0;
    }
    resizePannels(m_d->mParent->size());
}


void SARibbonCategoryProxy::onRightScrollButtonClicked()
{
    int width = m_d->mParent->width();
    QList<QPair<QWidget *, QSize> > w = allVisibleCategoryWidgetSizes();
    //求总宽
    int totalWidth = 0;

    for (QPair<QWidget *, QSize> r : w)
    {
        totalWidth += r.second.width();
    }
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
    qDebug() << "= SARibbonCategoryProxy::onRightScrollButtonClicked totalWidth:"
             << totalWidth
             << " width:"<<width
             << " m_d->mBaseX" << m_d->mBaseX
                ;
#endif
    if (totalWidth > width) {
        int tmp = m_d->mBaseX - width;
        if (tmp < (width - totalWidth)) {
            tmp = width - totalWidth;
        }
        m_d->mBaseX = tmp;
    }else {
        m_d->mBaseX = 0;
    }
    resizePannels(m_d->mParent->size());
}


QList<QPair<QWidget *, QSize> > SARibbonCategoryProxy::allVisibleCategoryWidgetSizes() const
{
    QList<QPair<QWidget *, QSize> > res;

    for (int i = 0; i < m_d->mPannelLists.size(); ++i)
    {
        SARibbonPannel *pannel = m_d->mPannelLists[i];
        SARibbonSeparatorWidget *separator = m_d->mSeparatorLists[i];
        if (!pannel->isHidden()) {
            res.append(qMakePair(pannel, pannel->sizeHint()));
            res.append(qMakePair(separator, separator->sizeHint()));
        }
    }
    return (res);
}


SARibbonCategory *SARibbonCategoryProxy::ribbonCategory()
{
    return (m_d->mParent);
}


/**
 * @brief 设置pannel的模式
 *
 * 在@ref SARibbonBar调用@ref SARibbonBar::setRibbonStyle 函数时，会对所有的SARibbonCategory调用此函数
 * 把新的SARibbonPannel::PannelLayoutMode设置进去
 *
 * 此函数为SARibbonCategory::setRibbonPannelLayoutMode的代理，
 * 在SARibbonCategory类中，此函数为protected，主要在SARibbonBar::setRibbonStyle时才会触发
 * @param m
 */
void SARibbonCategoryProxy::setRibbonPannelLayoutMode(SARibbonPannel::PannelLayoutMode m)
{
    if (m_d->mDefaultPannelLayoutMode == m) {
        return;
    }
    m_d->mDefaultPannelLayoutMode = m;
    for (SARibbonPannel *p : m_d->mPannelLists)
    {
        p->setPannelLayoutMode(m);
    }
}


SARibbonPannel::PannelLayoutMode SARibbonCategoryProxy::ribbonPannelLayoutMode() const
{
    return (m_d->mDefaultPannelLayoutMode);
}
