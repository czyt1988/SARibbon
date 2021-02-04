#include "SARibbonQuickAccessBar.h"
#include "SARibbonButtonGroupWidget.h"
#include "SARibbonSeparatorWidget.h"
class SARibbonQuickAccessBarPrivate
{
public:
        SARibbonButtonGroupWidget *groupWidget;
};

SARibbonQuickAccessBar::SARibbonQuickAccessBar(QWidget *parent)
        : SARibbonCtrlContainer(nullptr, parent)
        , m_d(new SARibbonQuickAccessBarPrivate)
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
        m_d->groupWidget->addSeparator();
}


void SARibbonQuickAccessBar::addAction(QAction *act)
{
        m_d->groupWidget->addAction(act);
}


void SARibbonQuickAccessBar::addWidget(QWidget *w)
{
        m_d->groupWidget->addWidget(w);
}


void SARibbonQuickAccessBar::addMenu(QMenu *m, QToolButton::ToolButtonPopupMode popMode)
{
        m_d->groupWidget->addMenu(m, popMode);
}


void SARibbonQuickAccessBar::initStyleOption(QStyleOption *opt)
{
        opt->initFrom(this);
}
