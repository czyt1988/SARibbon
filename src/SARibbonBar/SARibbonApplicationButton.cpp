#include "SARibbonApplicationButton.h"

#define SARIBBON_APPLICATION_BUTTON_MINIMUM_WIDTH 40
SARibbonApplicationButton::SARibbonApplicationButton(QWidget* parent) : QToolButton(parent)
{
    setFocusPolicy(Qt::NoFocus);
    setAutoRaise(true);
    setPopupMode(QToolButton::InstantPopup);
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    setMinimumWidth(SARIBBON_APPLICATION_BUTTON_MINIMUM_WIDTH);
}

SARibbonApplicationButton::SARibbonApplicationButton(const QString& text, QWidget* parent) : QToolButton(parent)
{
    setFocusPolicy(Qt::NoFocus);
    setAutoRaise(true);
    setPopupMode(QToolButton::InstantPopup);
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    setText(text);
    setMinimumWidth(SARIBBON_APPLICATION_BUTTON_MINIMUM_WIDTH);
}

SARibbonApplicationButton::SARibbonApplicationButton(const QIcon& icon, const QString& text, QWidget* parent)
    : QToolButton(parent)
{
    setFocusPolicy(Qt::NoFocus);
    setAutoRaise(true);
    setPopupMode(QToolButton::InstantPopup);
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    setIcon(icon);
    setText(text);
    setMinimumWidth(SARIBBON_APPLICATION_BUTTON_MINIMUM_WIDTH);
}
