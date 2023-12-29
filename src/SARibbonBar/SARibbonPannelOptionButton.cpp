#include "SARibbonPannelOptionButton.h"
#include <QAction>

SARibbonPannelOptionButton::SARibbonPannelOptionButton(QWidget* parent) : QToolButton(parent)
{
    setAutoRaise(true);
    setCheckable(false);
    setToolButtonStyle(Qt::ToolButtonIconOnly);
    setIconSize(QSize(10, 10));
    setIcon(QIcon(":/image/resource/ribbonPannelOptionButton.png"));
}
