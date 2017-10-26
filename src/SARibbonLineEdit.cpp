#include "SARibbonLineEdit.h"
#include <QStyleOption>
SARibbonLineEdit::SARibbonLineEdit(QWidget *parent)
    :SARibbonCtrolContainer(nullptr,parent)
{
    m_w = new QLineEdit(this);
    setContainerWidget(m_w);
}

QLineEdit *SARibbonLineEdit::lineEdit()
{
    return m_w;
}

void SARibbonLineEdit::initStyleOption(QStyleOption *opt)
{
    opt->initFrom(this);
}
