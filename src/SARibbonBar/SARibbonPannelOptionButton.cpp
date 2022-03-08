#include "SARibbonPannelOptionButton.h"
#include <QAction>

SARibbonPannelOptionButton::SARibbonPannelOptionButton(QWidget* parent) : QToolButton(parent)
{
    setAutoRaise(true);
    setCheckable(false);
    setToolButtonStyle(Qt::ToolButtonIconOnly);
    setFixedSize(16, 16);
    setIconSize(QSize(10, 10));
    setIcon(QIcon(":/image/resource/ribbonPannelOptionButton.png"));
}

void SARibbonPannelOptionButton::connectAction(QAction* action)
{
    connect(this, &SARibbonPannelOptionButton::clicked, action, &QAction::trigger);
}
