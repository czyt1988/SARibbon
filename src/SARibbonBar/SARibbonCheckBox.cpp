#include "SARibbonCheckBox.h"
#include <QStyleOption>

SARibbonCheckBox::SARibbonCheckBox(QWidget *parent)
    : QCheckBox(parent)
{
}

SARibbonCheckBox::SARibbonCheckBox(const QString &text, QWidget *parent)
    : QCheckBox(text, parent)
{

}

SARibbonCheckBox::~SARibbonCheckBox()
{
}
