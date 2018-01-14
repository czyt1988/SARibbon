#include "SARibbonQuickAccessBar.h"
#include "SARibbonButtonGroupWidget.h"
#include "SARibbonSeparatorWidget.h"
class SARibbonQuickAccessBarPrivate
{
public:
    SARibbonButtonGroupWidget* groupWidget;
};

SARibbonQuickAccessBar::SARibbonQuickAccessBar(QWidget *parent)
    :SARibbonCtrlContainer(nullptr,parent)
    ,m_d(new SARibbonQuickAccessBarPrivate)
{
    m_d->groupWidget = new SARibbonButtonGroupWidget(this);
    setContainerWidget(m_d->groupWidget);
}

SARibbonQuickAccessBar::~SARibbonQuickAccessBar()
{
    delete m_d;
}

void SARibbonQuickAccessBar::addSeparator()
{
    SARibbonSeparatorWidget* w = new SARibbonSeparatorWidget(this);
    m_d->groupWidget->addWidget(w);
}

SARibbonToolButton* SARibbonQuickAccessBar::addButton(QAction *act)
{
    return m_d->groupWidget->addButton(act);
}

void SARibbonQuickAccessBar::addWidget(QWidget *w)
{
    m_d->groupWidget->addWidget(w);
}

void SARibbonQuickAccessBar::initStyleOption(QStyleOption *opt)
{
    opt->initFrom(this);
}
