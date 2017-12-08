#include "SARibbonCategoryLayout.h"
#include <QLayoutItem>
#include "SARibbonPannel.h"
#include "SARibbonToolButton.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
class SARibbonReduceActionInfo
{
public:
    QPoint showStartPoint;
    SARibbonPannel* reduceModeShowPannel;
    SARibbonPannel* realShowPannel;
    SARibbonReduceActionInfo()
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

SARibbonCategoryLayout::SARibbonCategoryLayout(SARibbonCategory *parent):QLayout(parent)
{
    setContentsMargins(2,2,2,2);
}

SARibbonCategoryLayout::SARibbonCategoryLayout():QLayout()
{
    setContentsMargins(2,2,2,2);
}

SARibbonCategoryLayout::~SARibbonCategoryLayout()
{
    QLayoutItem *item = Q_NULLPTR;
    while ((item = takeAt(0)))
    {
          delete item;
    }
}

SARibbonCategory *SARibbonCategoryLayout::ribbonCategory()
{
    return qobject_cast<SARibbonCategory*>(parent());
}

void SARibbonCategoryLayout::addItem(QLayoutItem *item)
{
    itemList.append(item);
    invalidate();
}

QLayoutItem *SARibbonCategoryLayout::itemAt(int index) const
{
    return itemList.value(index,Q_NULLPTR);
}

QLayoutItem *SARibbonCategoryLayout::takeAt(int index)
{
    if (index >= 0 && index < itemList.size())
    {
        invalidate();
        QLayoutItem * item = itemList.value(index,Q_NULLPTR);
        if(item)
        {
            if(item->widget())
            {
                item->widget()->hide();
            }
        }
        return itemList.takeAt(index);
    }
    else
      return Q_NULLPTR;
}

int SARibbonCategoryLayout::count() const
{
    return itemList.size();
}


QSize SARibbonCategoryLayout::sizeHint() const
{
    QSize size;
   // QLayoutItem *item;
    for(int i=0;i<itemList.size();++i)
    {
        size = size.expandedTo(itemList[i]->minimumSize()); 
    }
    
    size += QSize(2*margin(), 2*margin());
    return size;
}

QSize SARibbonCategoryLayout::minimumSize() const
{
    return QSize(50,10);
}

Qt::Orientations SARibbonCategoryLayout::expandingDirections() const
{
    return Qt::Horizontal;
}

void SARibbonCategoryLayout::invalidate()
{
    m_isChanged = true;
    QLayout::invalidate();
}




int SARibbonCategoryLayout::buildReduceModePannel(SARibbonPannel *realPannel,int x,int y)
{
    SARibbonCategory* categoryPage = ribbonCategory();
    SARibbonReduceActionInfo info;
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
        QPoint pos = SARibbonCategoryLayout::calcPopupPannelPosition(categoryPage,info.realShowPannel,pannelX);
        //qDebug() << "pannelX:" << pannelX << " pos:" <<pos;
       //info.realShowPannel->move(pos);
        info.realShowPannel->setGeometry(pos.x(),pos.y(),info.realShowPannel->sizeHint().width(),info.realShowPannel->sizeHint().height());
        info.realShowPannel->setVisible(true);
        info.realShowPannel->show();
        info.realShowPannel->setFocus();
        info.realShowPannel->raise();
        info.realShowPannel->activateWindow();
        info.realShowPannel->repaint();
        QEvent event1(QEvent::UpdateRequest);
        QApplication::sendEvent(info.realShowPannel,&event1);
//        qDebug() << "realShowPannel geometry:"<<info.realShowPannel->geometry()
//                 << "\n window flag:"<<info.realShowPannel->windowFlags()
//        << "\n is active:"<<info.realShowPannel->isActiveWindow()
//           << "\n is visible:"<<info.realShowPannel->isVisible()
//              << "\n is visible to parent:"<<info.realShowPannel->isVisibleTo(categoryPage)
//           ;
    });
    m_pannelReduceInfo[realPannel] = info;
    return reducePannel->geometry().right();
}

QPoint SARibbonCategoryLayout::calcPopupPannelPosition(SARibbonCategory *category, SARibbonPannel *pannel, int x)
{
    QPoint absPosition = category->mapToGlobal(QPoint(x,category->height()));
    QRect r = QApplication::desktop()->availableGeometry(category);
    if((absPosition.x() + pannel->sizeHint().width()) < r.width())
    {
        return absPosition;
    }
    return QPoint(r.width()-  pannel->sizeHint().width(),absPosition.y());
}

void SARibbonCategoryLayout::setGeometry(const QRect &rect)
{
    if(rect == geometry())
    {
        qDebug() <<"warning !!!!rect == geometry() at SARibbonCategoryLayout::setGeometry(const QRect &rect) ";
        return;
    }
    qDebug() <<"void SARibbonCategoryLayout::setGeometry("<< rect;
    QLayout::setGeometry(rect);
    //
    int marginLeft,marginRight,marginTop,marginBottom;
    getContentsMargins(&marginLeft,&marginTop,&marginRight,&marginBottom);
    int x = marginLeft;
    int y = marginTop;
    QLayoutItem *item = Q_NULLPTR;
    //TODO 此处应有更好的处理方式 QUESTION[SARibbonCategoryLayout::setGeometry-1]
    QList<QPair<QWidget*,QRect> > expandingItem;//记录所有伸展的条目
    QList<QPair<QWidget*,QRect> > widgetItems;//记录所有当前显示的条目
    //计算尺寸，并进行一些处理，如超长隐藏等
    int listSize = itemList.size();
    for(int i=0;i<listSize;++i)
    {
        item = itemList[i];
        QWidget *wid = item->widget();
        QSize widSize = wid->sizeHint();
        int nextX = x + widSize.width();

        if(nextX > rect.right())
        {
            //说明超出边界
            if(SARibbonPannel* pannel = qobject_cast<SARibbonPannel*>(wid))
            {//此时无法显示全一个pannel
                if(!m_pannelReduceInfo.contains(pannel))
                {
                    //建立一个最小显示方案
                    buildReduceModePannel(pannel,x,y);
                }
                pannel->setReduce(true);
                SARibbonReduceActionInfo reduceInfo = m_pannelReduceInfo.value(pannel);
                QSize reducePannelSize = reduceInfo.reduceModeShowPannel->sizeHint();
                widgetItems.append(qMakePair(reduceInfo.reduceModeShowPannel,QRect(x,y,reducePannelSize.width(),reducePannelSize.height())));
                reduceInfo.reduceModeShowPannel->setVisible(true);
                x = x + reducePannelSize.width();
            }
            wid->setVisible(false);

        }
        else
        {
            //此时能显示全一个pannel
            if(SARibbonPannel* pannel = qobject_cast<SARibbonPannel*>(wid))
            {
                pannel->setReduce(false);
                //layoutNormalPannel(pannel,x,y,widSize.width(),widSize.height());
                //widgetItems.append(qMakePair(pannel,QRect(x,y,widSize.width(),widSize.height())));
                if(m_pannelReduceInfo.contains(pannel))
                {
                    //把reduce信息删除
                    m_pannelReduceInfo[pannel].release();
                    m_pannelReduceInfo.remove(pannel);
                }
                if(pannel->isExpanding())
                {
                    expandingItem.append(qMakePair(pannel,QRect(x,y,widSize.width(),widSize.height())));
                }
            }

            widgetItems.append(qMakePair(wid,QRect(x,y,widSize.width(),widSize.height())));
            if(!(wid->isVisible()))
            {
                wid->setVisible(true);
            }
            x = nextX;
        }
    }
    //计算剩余尺寸
    listSize = widgetItems.size();
    if(expandingItem.size() > 0)
    {
        int resetWidth = (rect.right() - x) / expandingItem.size();
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
    m_isChanged = false;
}
