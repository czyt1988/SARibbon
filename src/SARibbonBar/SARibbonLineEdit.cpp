#include "SARibbonLineEdit.h"
#include <QStyleOption>

SARibbonLineEdit::SARibbonLineEdit(QWidget *parent)
    : QLineEdit(parent)
{
}

SARibbonLineEdit::SARibbonLineEdit(const QString &text, QWidget *parent)
    : QLineEdit(text, parent)
{

}

SARibbonLineEdit::~SARibbonLineEdit()
{
}
