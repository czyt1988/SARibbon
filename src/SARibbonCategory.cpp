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
    SARibbonCategory* parent;
#if NOT_USE_LAYOUT
    QList<SARibbonPannel*> pannelLists;
    QList<SARibbonSeparatorWidget*> separatorLists;
    class ReduceActionInfo
    {
    public:
        QPoint showStartPoint;
        SARibbonPannel* reduceModeShowPannel;
        SARibbonPannel* realShowPannel;
        ReduceActionInfo()
            :showStartPoint(0,0)
            ,reduceModeShowPannel(nullptr)
            ,realShowPannel(nullptr)
        {

        }

        void release()
        {
            realShowPannel = nullptr;
            if(reduceModeShowPannel)
            {
                delete reduceModeShowPannel;
                reduceModeShowPannel = nullptr;
            }
        }
    };
    QMap<SARibbonPannel*,ReduceActionInfo> m_pannelReduceInfo;
#endif
};

SARibbonCategory::SARibbonCategory(QWidget *parent)
    :QWidget(parent)
    ,m_proxy(new SARibbonCategoryProxy(this))
{
#if NOT_USE_LAYOUT
#else
    SARibbonCategoryLayout* layout = new SARibbonCategoryLayout(this);
    installEventFilter(layout);
#endif
    setFixedHeight(98);
    setAutoFillBackground(true);
    setBackgroundBrush(Qt::white);
}

SARibbonCategory::~SARibbonCategory()
{
}

///
/// \brief 添加一个面板
/// \param title 面板名称
/// \return
///
SARibbonPannel *SARibbonCategory::addPannel(const QString &title)
{
    return proxy()->addPannel(title);
}

void SARibbonCategory::addPannel(SARibbonPannel *pannel)
{
    proxy()->addPannel(pannel);
}
///
/// \brief SARibbonCategory::setBackgroundBrush
/// \param brush
///
void SARibbonCategory::setBackgroundBrush(const QBrush &brush)
{
    proxy()->setBackgroundBrush(brush);
}

SARibbonCategoryProxy *SARibbonCategory::proxy()
{
    return m_proxy.data();
}

void SARibbonCategory::setProxy(SARibbonCategoryProxy *proxy)
{
    m_proxy.reset(proxy);
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
    :m_d(new SARibbonCategoryProxyPrivate)
{
    m_d->parent = parent;
}

SARibbonCategoryProxy::~SARibbonCategoryProxy()
{
    delete m_d;
}

SARibbonPannel *SARibbonCategoryProxy::addPannel(const QString &title)
{
    SARibbonPannel* pannel = RibbonSubElementDelegate->createRibbonPannel(ribbonCategory());
    pannel->setWindowTitle(title);
    SARibbonSeparatorWidget* seprator = RibbonSubElementDelegate->createRibbonSeparatorWidget(ribbonCategory()->height(),ribbonCategory());
#if NOT_USE_LAYOUT
    m_d->pannelLists.append(pannel);
    m_d->separatorLists.append(seprator);
#else
    ribbonCategory()->layout()->addWidget(pannel);
    ribbonCategory()->layout()->addWidget(seprator);
#endif
    return pannel;
}

void SARibbonCategoryProxy::addPannel(SARibbonPannel *pannel)
{
    pannel->setParent(ribbonCategory());
    SARibbonSeparatorWidget* seprator = RibbonSubElementDelegate->createRibbonSeparatorWidget(ribbonCategory()->height(),ribbonCategory());
#if NOT_USE_LAYOUT
    m_d->pannelLists.append(pannel);
    m_d->separatorLists.append(seprator);
#else
    ribbonCategory()->layout()->addWidget(pannel);
    ribbonCategory()->layout()->addWidget(seprator);
#endif
}

void SARibbonCategoryProxy::setBackgroundBrush(const QBrush &brush)
{
    QPalette p = ribbonCategory()->palette();
    p.setBrush(QPalette::Background,brush);
    ribbonCategory()->setPalette(p);
}


void SARibbonCategoryProxy::resizeEvent(QResizeEvent *event)
{
#if NOT_USE_LAYOUT
    resizePannels(event->size());
#else
    Q_UNUSED(event);
#endif
}

void SARibbonCategoryProxy::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    return;
}
#if NOT_USE_LAYOUT
void SARibbonCategoryProxy::resizePannels(const QSize &categorySize)
{
    int x = 1;
    int y = 2;
    //TODO 此处应有更好的处理方式 QUESTION[SARibbonCategoryLayout::setGeometry-1]
    QList<QPair<QWidget*,QRect> > expandingItem;//记录所有伸展的条目
    QList<QPair<QWidget*,QRect> > widgetItems;//记录所有当前显示的条目
    //计算尺寸，并进行一些处理，如超长隐藏等
    for(int i=0;i<m_d->pannelLists.size();++i)
    {
        SARibbonPannel* pannel =  m_d->pannelLists[i];
        SARibbonSeparatorWidget* separator = m_d->separatorLists[i];
        QSize widSize = pannel->sizeHint();
        int nextX = x + widSize.width();
        if(nextX > categorySize.width())
        {
            //说明超出边界

            //此时无法显示全一个pannel
            if(!m_d->m_pannelReduceInfo.contains(m_d->pannelLists[i]))
            {
                //建立一个最小显示方案
                buildReduceModePannel(pannel,x,y);
            }
            pannel->setReduce(true);
            pannel->setParent(ribbonCategory()->parentWidget());
            SARibbonCategoryProxyPrivate::ReduceActionInfo reduceInfo = m_d->m_pannelReduceInfo.value(pannel);
            QSize reducePannelSize = reduceInfo.reduceModeShowPannel->sizeHint();
            reduceInfo.reduceModeShowPannel->setVisible(true);
            widgetItems.append(qMakePair(reduceInfo.reduceModeShowPannel,QRect(x,y,reducePannelSize.width(),reducePannelSize.height())));
            x = x + reducePannelSize.width();
            pannel->setVisible(false);

            separator->setVisible(true);
            QSize separatorSize = separator->sizeHint();
            widgetItems.append(qMakePair(separator,QRect(x,y,separatorSize.width(),separatorSize.height())));
            x = x + separatorSize.width();
        }
        else
        {
            if(pannel->parentWidget() != ribbonCategory())
            {
                pannel->setParent(ribbonCategory());
            }
            pannel->setReduce(false);
            if(m_d->m_pannelReduceInfo.contains(pannel))
            {
                //把reduce信息删除
                m_d->m_pannelReduceInfo[pannel].release();
                m_d->m_pannelReduceInfo.remove(pannel);
            }
            if(pannel->isExpanding())
            {
                expandingItem.append(qMakePair(pannel,QRect(x,y,widSize.width(),widSize.height())));
            }


            if(!(pannel->isVisible()))
            {
                pannel->setVisible(true);
            }
            widgetItems.append(qMakePair(pannel,QRect(x,y,widSize.width(),widSize.height())));
            x = nextX;
            separator->setVisible(true);
            QSize separatorSize = separator->sizeHint();
            widgetItems.append(qMakePair(separator,QRect(x,y,separatorSize.width(),separatorSize.height())));
            x += separatorSize.width();
        }
    }
    //计算剩余尺寸
    int listSize = widgetItems.size();
    if(expandingItem.size() > 0)
    {
        int resetWidth = (categorySize.width() - x) / expandingItem.size();
        if(resetWidth > 1)
        {
            for(int i = 0;i<expandingItem.size();++i)
            {
                for(int index = 0;index<listSize;++index)
                {
                    if(widgetItems[index].first == expandingItem[i].first)
                    {
                        widgetItems[index].second.adjust(0,0,resetWidth,0);
                        //后续的条目顺延
                        for(int j = index+1;j<listSize;++j)
                        {
                            widgetItems[j].second.adjust(resetWidth,0,resetWidth,0);
                        }
                    }
                }
            }
        }

    }
    //更新尺寸
    for(int i=0;i<listSize;++i)
    {
        widgetItems[i].first->setGeometry( widgetItems[i].second );
    }
    //m_isChanged = false;
}


#endif


SARibbonCategory *SARibbonCategoryProxy::ribbonCategory()
{
    return m_d->parent;
}


#if NOT_USE_LAYOUT
int SARibbonCategoryProxy::buildReduceModePannel(SARibbonPannel *realPannel, int x, int y)
{
    SARibbonCategory* categoryPage = ribbonCategory();
    SARibbonCategoryProxyPrivate::ReduceActionInfo info;
    info.realShowPannel = realPannel;
    SARibbonPannel* reducePannel = new SARibbonPannel(categoryPage);
    reducePannel->setWindowTitle(realPannel->windowTitle());
    reducePannel->setWindowIcon(realPannel->windowIcon());
    SARibbonToolButton* btn = reducePannel->addLargeToolButton(realPannel->windowTitle(),realPannel->windowIcon()
                                    ,QToolButton::InstantPopup);
    info.reduceModeShowPannel = reducePannel;
    reducePannel->move(x,y);

    connect(btn,&SARibbonToolButton::clicked
                                  ,this,[categoryPage,info](bool on){
        Q_UNUSED(on);
        int pannelX = info.reduceModeShowPannel->geometry().x();
        QPoint pos = SARibbonCategoryProxy::calcPopupPannelPosition(categoryPage,info.realShowPannel,pannelX);
        //qDebug() << "pannelX:" << pannelX << " pos:" <<pos;
       //info.realShowPannel->move(pos);
        info.realShowPannel->setReduce(true);
        info.realShowPannel->setGeometry(pos.x(),pos.y(),info.realShowPannel->sizeHint().width(),info.realShowPannel->sizeHint().height());
        info.realShowPannel->show();
        info.realShowPannel->setFocus();
        info.realShowPannel->raise();
        info.realShowPannel->activateWindow();
        info.realShowPannel->repaint();
    });
    m_d->m_pannelReduceInfo[realPannel] = info;
    return reducePannel->geometry().right();
}

QPoint SARibbonCategoryProxy::calcPopupPannelPosition(SARibbonCategory *category, SARibbonPannel *pannel, int x)
{
    QPoint absPosition = category->mapToGlobal(QPoint(x,category->height()));
    QRect r = QApplication::desktop()->availableGeometry(category);
    if((absPosition.x() + pannel->sizeHint().width()) < r.width())
    {
        return absPosition;
    }
    return QPoint(r.width()-  pannel->sizeHint().width(),absPosition.y());
}
#endif
