#include "SARibbonApplicationButton.h"

SARibbonApplicationButton::SARibbonApplicationButton(QWidget *parent)
    : QPushButton(parent)
{
    setFocusPolicy(Qt::NoFocus);
    setFlat(true);
}


SARibbonApplicationButton::SARibbonApplicationButton(const QString& text, QWidget *parent)
    : QPushButton(text, parent)
{
    setFocusPolicy(Qt::NoFocus);
    setFlat(true);
}


SARibbonApplicationButton::SARibbonApplicationButton(const QIcon& icon, const QString& text, QWidget *parent)
    : QPushButton(icon, text, parent)
{
    setFocusPolicy(Qt::NoFocus);
    setFlat(true);
}
