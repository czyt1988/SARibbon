#include "SARibbonPannelOptionButton.h"
#include <QAction>

SARibbonPannelOptionButton::SARibbonPannelOptionButton(QWidget* parent) : QToolButton(parent)
{
    setAutoRaise(true);
    setCheckable(false);
    setToolButtonStyle(Qt::ToolButtonIconOnly);
    setIconSize(QSize(10, 10));
    static QIcon s_default_icon = QIcon(":/image/resource/ribbonPannelOptionButton.png");
    setIcon(s_default_icon);
}

SARibbonPannelOptionButton::~SARibbonPannelOptionButton()
{
}
