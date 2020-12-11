#include "SARibbonCtrlContainer.h"
#include <QPainter>
#include <QPaintEvent>
#include <QStylePainter>
#include <QDebug>
#include "SARibbonDrawHelper.h"
class SARibbonCtrlContainerPrivate
{
public:
    SARibbonCtrlContainer* Parent;
    QWidget* containerWidget;
    bool enableDrawIcon;
    bool enableDrawTitle;
    SARibbonCtrlContainerPrivate(SARibbonCtrlContainer* p)
        :containerWidget(Q_NULLPTR)
        ,enableDrawIcon(true)
        ,enableDrawTitle(true)
    {
        Parent = p;
    }
    void init(QWidget* w)
    {
        containerWidget = w;
        if(w)
        {
            containerWidget->setParent(Parent);
        }
        Parent->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
    }
};

SARibbonCtrlContainer::SARibbonCtrlContainer(QWidget *containerWidget, QWidget *parent)
    :QWidget(parent)
    ,m_d(new SARibbonCtrlContainerPrivate(this))
{
    m_d->init(containerWidget);
}

SARibbonCtrlContainer::~SARibbonCtrlContainer()
{
    delete m_d;
}

QSize SARibbonCtrlContainer::sizeHint() const
{
    if(nullptr == m_d->containerWidget)
    {
        return QWidget::sizeHint();
    }
    QSize containerSizeHint = m_d->containerWidget->sizeHint();
    if(m_d->enableDrawIcon)
    {
        QIcon icon = windowIcon();
        if(!icon.isNull())
        {
            containerSizeHint.setWidth(containerSizeHint.width() + containerSizeHint.height());
        }
    }
    if(m_d->enableDrawTitle)
    {
        QString text = windowTitle();
        if(!text.isEmpty())
        {
#if QT_VERSION >= QT_VERSION_CHECK(5,11,0)
            int textWidth = fontMetrics().horizontalAdvance(text);
#else
            int textWidth = fontMetrics().width(text);
#endif
            containerSizeHint.setWidth(containerSizeHint.width() + textWidth);
        }
    }
    return containerSizeHint;
}

QSize SARibbonCtrlContainer::minimumSizeHint() const
{
    if(nullptr == m_d->containerWidget)
    {
        return QWidget::minimumSizeHint();
    }
    QSize containerHint = m_d->containerWidget->minimumSizeHint();
    if(m_d->enableDrawIcon)
    {
        QIcon icon = windowIcon();
        if(!icon.isNull())
        {
            containerHint.setWidth(containerHint.width() + containerHint.height());
        }
    }
    if(m_d->enableDrawTitle)
    {
        QString text = windowTitle();
        if(!text.isEmpty())
        {
#if QT_VERSION >= QT_VERSION_CHECK(5,11,0)
            int textWidth = fontMetrics().horizontalAdvance(text.at(0));
#else
            int textWidth = fontMetrics().width(text.at(0));
#endif
            containerHint.setWidth(containerHint.width() + (2*textWidth));
        }
    }
    return containerHint;
}

QWidget *SARibbonCtrlContainer::containerWidget()
{
    return m_d->containerWidget;
}

const QWidget *SARibbonCtrlContainer::containerWidget() const
{
    return m_d->containerWidget;
}

void SARibbonCtrlContainer::setEnableShowIcon(bool b)
{
    m_d->enableDrawIcon = b;
    update();
}

void SARibbonCtrlContainer::setEnableShowTitle(bool b)
{
    m_d->enableDrawTitle = b;
    update();
}

void SARibbonCtrlContainer::setContainerWidget(QWidget *w)
{
    if(m_d->containerWidget)
    {
        m_d->containerWidget->hide();
        m_d->containerWidget->deleteLater();
    }
    if(w)
    {
        w->setParent(this);
    }
    m_d->containerWidget = w;
}

void SARibbonCtrlContainer::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QStylePainter painter(this);

    // draw the combobox frame, focusrect and selected etc.
    QStyleOption opt;
    initStyleOption(&opt);
    const int widgetHeight = height();
    int x = 0;
    //绘制图标
    if(m_d->enableDrawIcon)
    {
        QIcon icon = windowIcon();
        if(!icon.isNull())
        {
            //绘制图标
            QSize iconSize = SARibbonDrawHelper::iconActualSize(icon,&opt,QSize(widgetHeight,widgetHeight));
            SARibbonDrawHelper::drawIcon(icon,&painter,&opt,x,0,widgetHeight,widgetHeight);
            x += iconSize.width() + 4;
        }
    }
    //绘制文字
    if(m_d->enableDrawTitle)
    {
        QString text = windowTitle();
        if(!text.isEmpty())
        {
#if QT_VERSION >= QT_VERSION_CHECK(5,11,0)
            int textWidth = opt.fontMetrics.horizontalAdvance(text);
#else
            int textWidth = opt.fontMetrics.width(text);
#endif
            if(textWidth > (opt.rect.width() - widgetHeight - x))
            {
                textWidth = opt.rect.width() - widgetHeight - x;
                text = opt.fontMetrics.elidedText(text,Qt::ElideRight,textWidth);
            }
            if(textWidth > 0)
            {
                SARibbonDrawHelper::drawText(text,&painter,&opt
                                             ,Qt::AlignLeft|Qt::AlignVCenter
                                             ,QRect(x,0,textWidth,opt.rect.height()));
            }

        }
    }
   // QWidget::paintEvent(e);
}

void SARibbonCtrlContainer::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e);
    QStyleOption opt;
    initStyleOption(&opt);
    int x = 0;
    const int widgetHeight = height();
    if(m_d->enableDrawIcon)
    {
        QIcon icon = windowIcon();
        if(!icon.isNull())
        {
            QSize iconSize = SARibbonDrawHelper::iconActualSize(icon,&opt,QSize(widgetHeight,widgetHeight));
            x += iconSize.width();
            x += 4;
        }
    }
    //绘制文字
    if(m_d->enableDrawTitle)
    {
        QString text = windowTitle();
        if(!text.isEmpty())
        {
#if QT_VERSION >= QT_VERSION_CHECK(5,11,0)
            int textWidth = opt.fontMetrics.horizontalAdvance(text);
#else
            int textWidth = opt.fontMetrics.width(text);
#endif
            if(textWidth > (opt.rect.width() - widgetHeight - x))
            {
                textWidth = opt.rect.width() - widgetHeight - x;
                text = opt.fontMetrics.elidedText(text,Qt::ElideRight,textWidth);
            }
            if(textWidth > 0)
            {
                x += textWidth;
                x += 2;
            }
        }
    }
    if(m_d->containerWidget)
    {
        m_d->containerWidget->setGeometry(x,0,width()-x,height());
    }

}

void SARibbonCtrlContainer::initStyleOption(QStyleOption *opt)
{
    opt->initFrom(this);
}
