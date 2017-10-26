#include "SARibbonTabBar.h"

SARibbonTabBar::SARibbonTabBar(QWidget *parent):QTabBar(parent)
  ,m_tabMargin(6,0,0,0)
{
    setExpanding(false);
}

QMargins SARibbonTabBar::tabMargin() const
{
    return m_tabMargin;
}

void SARibbonTabBar::setTabMargin(const QMargins &tabMargin)
{
    m_tabMargin = tabMargin;
}
