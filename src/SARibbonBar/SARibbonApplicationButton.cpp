#include "SARibbonApplicationButton.h"

#define SARIBBON_APPLICATION_BUTTON_MINIMUM_WIDTH 40
SARibbonApplicationButton::SARibbonApplicationButton(QWidget* parent) : QPushButton(parent)
{
    setFocusPolicy(Qt::NoFocus);
    setFlat(true);
    setMinimumWidth(SARIBBON_APPLICATION_BUTTON_MINIMUM_WIDTH);
}

SARibbonApplicationButton::SARibbonApplicationButton(const QString& text, QWidget* parent) : QPushButton(text, parent)
{
    setFocusPolicy(Qt::NoFocus);
    setFlat(true);
    setMinimumWidth(SARIBBON_APPLICATION_BUTTON_MINIMUM_WIDTH);
}

SARibbonApplicationButton::SARibbonApplicationButton(const QIcon& icon, const QString& text, QWidget* parent)
    : QPushButton(icon, text, parent)
{
    setFocusPolicy(Qt::NoFocus);
    setFlat(true);
    setMinimumWidth(SARIBBON_APPLICATION_BUTTON_MINIMUM_WIDTH);
}
