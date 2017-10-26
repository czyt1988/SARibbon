#include "SARibbonButtonGroupWidget.h"
#include <QHBoxLayout>
#include <QDebug>
#include <QMargins>
class SARibbonButtonGroupWidgetPrivate
{
public:
    SARibbonButtonGroupWidget* Parent;
    SARibbonButtonGroupWidgetPrivate(SARibbonButtonGroupWidget* p)
    {
        Parent = p;
    }
    void init()
    {
        QHBoxLayout *layout = new QHBoxLayout;
        layout->setMargin(0);
        layout->setSpacing(0);
        Parent->setLayout(layout);
    }
};

SARibbonButtonGroupWidget::SARibbonButtonGroupWidget(QWidget *parent)
    :QFrame(parent)
    ,m_d(new SARibbonButtonGroupWidgetPrivate(this))
{
    m_d->init();
}

SARibbonButtonGroupWidget::~SARibbonButtonGroupWidget()
{
    delete m_d;
}

void SARibbonButtonGroupWidget::addButton(QAbstractButton *btn)
{
    layout()->addWidget(btn);
    layout()->setAlignment(btn,Qt::AlignCenter);
    btn->setVisible(true);
    update();
}

QSize SARibbonButtonGroupWidget::sizeHint() const
{
    const QMargins& margins = layout()->contentsMargins();
    int w = 0;
    w += margins.right();
    w += margins.left();
    int h = 28;
    const int itemCount = layout()->count();
    for(int i=0;i<itemCount;++i)
    {
        QLayoutItem* item = layout()->itemAt(i);
        QWidget* widget = item->widget();
        if(widget)
        {
            if(widget->isVisible())
            {
                w += widget->size().width() + layout()->spacing();
            }
        }
    }
    return QSize(w,h);
}

QSize SARibbonButtonGroupWidget::minimumSizeHint() const
{
    const QMargins& margins = layout()->contentsMargins();
    int w = 0;
    w += margins.right();
    w += margins.left();
    int h = 28;
    const int itemCount = layout()->count();
    for(int i=0;i<itemCount;++i)
    {
        QLayoutItem* item = layout()->itemAt(i);
        QWidget* widget = item->widget();
        if(widget)
        {
            if(widget->isVisible())
            {
                w += widget->minimumSizeHint().width();
            }
        }
    }
    return QSize(w,h);
}
