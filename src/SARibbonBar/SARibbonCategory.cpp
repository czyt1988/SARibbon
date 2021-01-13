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
    SARibbonCategory *parent;
    SARibbonPannel::PannelLayoutMode m_defaultPannelLayoutMode;
    QList<SARibbonPannel *> pannelLists;
    QList<SARibbonSeparatorWidget *> separatorLists;
    class ReduceActionInfo
    {
public:
        QPoint showStartPoint;
        SARibbonPannel *reduceModeShowPannel;
        SARibbonPannel *realShowPannel;
        ReduceActionInfo()
            : showStartPoint(0, 0)
            , reduceModeShowPannel(nullptr)
            , realShowPannel(nullptr)
        {
        }


        void release()
        {
            realShowPannel = nullptr;
            if (reduceModeShowPannel) {
                delete reduceModeShowPannel;
                reduceModeShowPannel = nullptr;
            }
        }
    };
    QMap<SARibbonPannel *, ReduceActionInfo> m_pannelReduceInfo;

    SARibbonCategoryProxyPrivate(SARibbonCategory *p) :
        parent(p)
        , m_defaultPannelLayoutMode(SARibbonPannel::ThreeRowMode)
    {
    }
};

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
}


SARibbonCategoryProxy::~SARibbonCategoryProxy()
{
    delete m_d;
}


SARibbonPannel *SARibbonCategoryProxy::addPannel(const QString& title)
{
    SARibbonPannel *pannel = RibbonSubElementDelegate->createRibbonPannel(ribbonCategory());

    pannel->setWindowTitle(title);
    pannel->setPannelLayoutMode(ribbonPannelLayoutMode());
    SARibbonSeparatorWidget *seprator = RibbonSubElementDelegate->createRibbonSeparatorWidget(ribbonCategory()->height(), ribbonCategory());

    m_d->pannelLists.append(pannel);
    m_d->separatorLists.append(seprator);
    return (pannel);
}


void SARibbonCategoryProxy::addPannel(SARibbonPannel *pannel)
{
    if (pannel->parent() != ribbonCategory()) {
        pannel->setParent(ribbonCategory());
    }
    pannel->setPannelLayoutMode(ribbonPannelLayoutMode());
    SARibbonSeparatorWidget *seprator = RibbonSubElementDelegate->createRibbonSeparatorWidget(ribbonCategory()->height(), ribbonCategory());

    m_d->pannelLists.append(pannel);
    m_d->separatorLists.append(seprator);
}


bool SARibbonCategoryProxy::takePannel(SARibbonPannel *pannel)
{
    if (m_d->pannelLists.removeOne(pannel)) {
        return (true);
    }
    //还有一种可能，此时pannel处于缩减状态
    auto i = m_d->m_pannelReduceInfo.find(pannel);

    if (i != m_d->m_pannelReduceInfo.end()) {
        SARibbonCategoryProxyPrivate::ReduceActionInfo info = m_d->m_pannelReduceInfo.take(pannel);
        info.release();
        return (true);
    }
    return (false);
}


bool SARibbonCategoryProxy::removePannel(SARibbonPannel *pannel)
{
    if (takePannel(pannel)) {
        delete pannel;
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
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
    qDebug() << "SARibbonCategoryProxy::resizePannels";
#endif
    int x = 1;
    int y = 1;
    //TODO 此处应有更好的处理方式 QUESTION[SARibbonCategoryLayout::setGeometry-1]
    QList<QPair<QWidget *, QRect> > expandingItem;  //记录所有伸展的条目
    QList<QPair<QWidget *, QRect> > widgetItems;    //记录所有当前显示的条目

    //计算尺寸，并进行一些处理，如超长隐藏等
    for (int i = 0; i < m_d->pannelLists.size(); ++i)
    {
        SARibbonPannel *pannel = m_d->pannelLists[i];
        SARibbonSeparatorWidget *separator = m_d->separatorLists[i];
        separator->setFixedHeight(categorySize.height());
        QSize widSize = pannel->sizeHint();
        //在Category里的所有的窗体高度都和Category一致
        widSize.setHeight(categorySize.height()-1);

        int nextX = x + widSize.width();
        if (nextX > categorySize.width()) {
            //说明超出边界

            //此时无法显示全一个pannel
            if (!m_d->m_pannelReduceInfo.contains(m_d->pannelLists[i])) {
                //建立一个最小显示方案
                buildReduceModePannel(pannel, x, y);
            }
            pannel->setReduce(true);
            //此时的parent要设置为mainwindow
            pannel->setParent(ribbonCategory()->parentWidget());
            SARibbonCategoryProxyPrivate::ReduceActionInfo reduceInfo = m_d->m_pannelReduceInfo.value(pannel);
            QSize reducePannelSize = reduceInfo.reduceModeShowPannel->sizeHint();
            reducePannelSize.setHeight(categorySize.height()-1);
            reduceInfo.reduceModeShowPannel->setVisible(true);
            widgetItems.append(qMakePair(reduceInfo.reduceModeShowPannel, QRect(x, y, reducePannelSize.width(), reducePannelSize.height())));
            x = x + reducePannelSize.width();
            pannel->setVisible(false);

            separator->setVisible(true);
            QSize separatorSize = separator->sizeHint();
            widgetItems.append(qMakePair(separator, QRect(x, y, separatorSize.width(), separatorSize.height())));
            x = x + separatorSize.width();
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
            qDebug()	<< QStringLiteral("超长的pannel，建立了Reduce模式：")<< pannel->windowTitle()
                    << " x:" << x
                    << " y:" << y
                    << " size:" << widSize
            ;
#endif
        }else {
            if (pannel->isReduce()) {
                if (pannel->parentWidget() != ribbonCategory()) {
                    pannel->setParent(ribbonCategory());
                }
                pannel->setReduce(false);
                pannel->setVisible(true);
            }

            if (m_d->m_pannelReduceInfo.contains(pannel)) {
                //把reduce信息删除
                m_d->m_pannelReduceInfo[pannel].release();
                m_d->m_pannelReduceInfo.remove(pannel);
            }
            if (pannel->isExpanding()) {
                expandingItem.append(qMakePair(pannel, QRect(x, y, widSize.width(), widSize.height())));
            }
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
            qDebug()	<< QStringLiteral("正常的pannel：")<< pannel->windowTitle()
                    << " x:" << x
                    << " y:" << y
                    << " size:" << widSize
            ;
#endif
            widgetItems.append(qMakePair(pannel, QRect(x, y, widSize.width(), widSize.height())));
            x = nextX;
            separator->setVisible(true);
            QSize separatorSize = separator->sizeHint();
            widgetItems.append(qMakePair(separator, QRect(x, y, separatorSize.width(), separatorSize.height())));
            x += separatorSize.width();
        }
    }
    //计算剩余尺寸
    int listSize = widgetItems.size();

    if (expandingItem.size() > 0) {
        int resetWidth = (categorySize.width() - x) / expandingItem.size();
        if (resetWidth > 1) {
            for (int i = 0; i < expandingItem.size(); ++i)
            {
                for (int index = 0; index < listSize; ++index)
                {
                    if (widgetItems[index].first == expandingItem[i].first) {
                        widgetItems[index].second.adjust(0, 0, resetWidth, 0);
                        //后续的条目顺延
                        for (int j = index+1; j < listSize; ++j)
                        {
                            widgetItems[j].second.adjust(resetWidth, 0, resetWidth, 0);
                        }
                    }
                }
            }
        }
    }
    //更新尺寸
    for (int i = 0; i < listSize; ++i)
    {
        widgetItems[i].first->setGeometry(widgetItems[i].second);
    }
    //m_isChanged = false;
}


const QList<SARibbonPannel *>& SARibbonCategoryProxy::pannelList() const
{
    return (m_d->pannelLists);
}


QList<SARibbonPannel *>& SARibbonCategoryProxy::pannelList()
{
    return (m_d->pannelLists);
}


bool SARibbonCategoryProxy::isReduce(SARibbonPannel *pannle)
{
    return (m_d->m_pannelReduceInfo.contains(pannle));
}


SARibbonCategory *SARibbonCategoryProxy::ribbonCategory()
{
    return (m_d->parent);
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
    if (m_d->m_defaultPannelLayoutMode == m) {
        return;
    }
    m_d->m_defaultPannelLayoutMode = m;
    for (SARibbonPannel *p : m_d->pannelLists)
    {
        p->setPannelLayoutMode(m);
    }
}


SARibbonPannel::PannelLayoutMode SARibbonCategoryProxy::ribbonPannelLayoutMode() const
{
    return (m_d->m_defaultPannelLayoutMode);
}


int SARibbonCategoryProxy::buildReduceModePannel(SARibbonPannel *realPannel, int x, int y)
{
    SARibbonCategory *categoryPage = ribbonCategory();
    SARibbonCategoryProxyPrivate::ReduceActionInfo info;

    info.realShowPannel = realPannel;
    SARibbonPannel *reducePannel = RibbonSubElementDelegate->createRibbonPannel(categoryPage);

    reducePannel->setPannelLayoutMode(ribbonPannelLayoutMode());

    reducePannel->setWindowTitle(realPannel->windowTitle());
    reducePannel->setWindowIcon(realPannel->windowIcon());
    SARibbonToolButton *btn = reducePannel->addLargeToolButton(realPannel->windowTitle(), realPannel->windowIcon()
        , QToolButton::InstantPopup);

    info.reduceModeShowPannel = reducePannel;
    reducePannel->move(x, y);

    connect(btn, &SARibbonToolButton::clicked
        , this, [categoryPage, info](bool on) {
        Q_UNUSED(on);
        int pannelX = info.reduceModeShowPannel->geometry().x();
        QPoint pos = SARibbonCategoryProxy::calcPopupPannelPosition(categoryPage, info.realShowPannel, pannelX);
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
        //qDebug() << "pannelX:" << pannelX << " pos:" <<pos;
        //info.realShowPannel->move(pos);
#endif
        info.realShowPannel->setReduce(true);
        info.realShowPannel->setGeometry(pos.x(), pos.y(), info.realShowPannel->sizeHint().width(), info.realShowPannel->sizeHint().height());
        info.realShowPannel->show();
        info.realShowPannel->setFocus();
        info.realShowPannel->raise();
        info.realShowPannel->activateWindow();
        info.realShowPannel->repaint();
    });
    m_d->m_pannelReduceInfo[realPannel] = info;
    return (reducePannel->geometry().right());
}


QPoint SARibbonCategoryProxy::calcPopupPannelPosition(SARibbonCategory *category, SARibbonPannel *pannel, int x)
{
    QPoint absPosition = category->mapToGlobal(QPoint(x, category->height()));
    QRect r = QApplication::desktop()->availableGeometry(category);

    if ((absPosition.x() + pannel->sizeHint().width()) < r.width()) {
        return (absPosition);
    }
    return (QPoint(r.width()-  pannel->sizeHint().width(), absPosition.y()));
}
