#include "SARibbonApplicationButton.h"

SARibbonApplicationButton::SARibbonApplicationButton(QWidget *parent)
    : QPushButton(parent)
{
    setFocusPolicy(Qt::NoFocus);
    setFlat(true);
}
