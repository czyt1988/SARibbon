#include "SARibbonCheckBox.h"
#include <QStyleOption>
SARibbonCheckBox::SARibbonCheckBox(QWidget *parent)
    :SARibbonCtrolContainer(nullptr,parent)
{
    m_w = new QCheckBox(this);
    setContainerWidget(m_w);
}

QCheckBox *SARibbonCheckBox::checkBox()
{
    return m_w;
}

void SARibbonCheckBox::initStyleOption(QStyleOption *opt)
{
    opt->initFrom(this);
}
