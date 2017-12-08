#include "SARibbonCtrolContainer.h"
#include <QPainter>
#include <QPaintEvent>
#include <QStylePainter>
#include <QDebug>
#include "SARibbonDrawHelper.h"
class SARibbonCtrolContainerPrivate
{
public:
    SARibbonCtrolContainer* Parent;
    QWidget* containerWidget;
    bool enableDrawIcon;
    bool enableDrawTitle;
    SARibbonCtrolContainerPrivate(SARibbonCtrolContainer* p)
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

SARibbonCtrolContainer::SARibbonCtrolContainer(QWidget *containerWidget, QWidget *parent)
    :QWidget(parent)
    ,m_d(new SARibbonCtrolContainerPrivate(this))
{
    m_d->init(containerWidget);
}

SARibbonCtrolContainer::~SARibbonCtrolContainer()
{
    delete m_d;
}

QSize SARibbonCtrolContainer::sizeHint() const
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
            int textWidth = fontMetrics().width(text);
            containerSizeHint.setWidth(containerSizeHint.width() + textWidth);
        }
    }
    return containerSizeHint;
}

QSize SARibbonCtrolContainer::minimumSizeHint() const
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
            int textWidth = fontMetrics().width(text.at(0));
            containerHint.setWidth(containerHint.width() + (2*textWidth));
        }
    }
    return containerHint;
}

QWidget *SARibbonCtrolContainer::containerWidget()
{
    return m_d->containerWidget;
}

const QWidget *SARibbonCtrolContainer::containerWidget() const
{
    return m_d->containerWidget;
}

void SARibbonCtrolContainer::setEnableShowIcon(bool b)
{
    m_d->enableDrawIcon = b;
    update();
}

void SARibbonCtrolContainer::setEnableShowTitle(bool b)
{
    m_d->enableDrawTitle = b;
    update();
}

void SARibbonCtrolContainer::setContainerWidget(QWidget *w)
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

void SARibbonCtrolContainer::paintEvent(QPaintEvent *e)
{
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
            int textWidth = opt.fontMetrics.width(text);
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
    QWidget::paintEvent(e);
}

void SARibbonCtrolContainer::resizeEvent(QResizeEvent *e)
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
            int textWidth = opt.fontMetrics.width(text);
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

void SARibbonCtrolContainer::initStyleOption(QStyleOption *opt)
{
    opt->initFrom(this);
}
