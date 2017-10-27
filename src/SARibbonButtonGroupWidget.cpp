#include "SARibbonButtonGroupWidget.h"
#include <QHBoxLayout>
#include <QDebug>
#include <QMargins>
#include <QChildEvent>
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
    btn->installEventFilter(this);
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

void SARibbonButtonGroupWidget::childEvent(QChildEvent *e)
{
    QObject* child = e->child();
    if (e && (e->type() == QEvent::ChildAdded) && child && child->isWidgetType())
    {
        QWidget *childWidget = qobject_cast<QWidget*>(e->child());
        // Handle the case where the child has already it's visibility set before
        // being added to the widget
        if (childWidget->testAttribute(Qt::WA_WState_ExplicitShowHide) &&
            childWidget->testAttribute(Qt::WA_WState_Hidden))
        {
          // if the widget has explicitly set to hidden, then mark it as such
            childWidget->setProperty("visibilityToParent", false);
        }
        setChildVisibility(childWidget);
    }
    this->QFrame::childEvent(e);
}

bool SARibbonButtonGroupWidget::eventFilter(QObject *child, QEvent *e)
{
    if (e->type() == QEvent::ShowToParent)
    {
        child->setProperty("visibilityToParent", true);
    }
    else if(e->type() == QEvent::HideToParent)
    {
        child->setProperty("visibilityToParent", false);
    }
    return this->QFrame::eventFilter(child, e);
}

void SARibbonButtonGroupWidget::setChildVisibility(QWidget *childWidget)
{
    if (!testAttribute(Qt::WA_WState_Created))
    {
        return;
    }
    bool visible = isVisible();
    if (childWidget->testAttribute(Qt::WA_WState_Created) ||
      !visible)
    {
        childWidget->setVisible(visible);
    }
    if ((!childWidget->property("visibilityToParent").isValid() ||
          childWidget->property("visibilityToParent").toBool()))
    {
        childWidget->setAttribute(Qt::WA_WState_ExplicitShowHide, false);
    }
}
