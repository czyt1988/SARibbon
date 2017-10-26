#include "SARibbonComboBox.h"
#include <QStyleOption>
#include <QDebug>
SARibbonComboBox::SARibbonComboBox(QWidget *parent)
    :SARibbonCtrolContainer(nullptr,parent)
{
    m_w = new QComboBox(this);
    setContainerWidget(m_w);
}

QComboBox *SARibbonComboBox::comboBox()
{
    return m_w;
}

void SARibbonComboBox::initStyleOption(QStyleOption *opt)
{
    opt->initFrom(this);
}

