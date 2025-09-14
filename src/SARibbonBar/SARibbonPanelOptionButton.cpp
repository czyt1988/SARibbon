#include "SARibbonPanelOptionButton.h"
#include <QAction>

SARibbonPanelOptionButton::SARibbonPanelOptionButton(QWidget* parent) : QToolButton(parent)
{
	setAutoRaise(true);
	setCheckable(false);
	setToolButtonStyle(Qt::ToolButtonIconOnly);
	setIconSize(QSize(10, 10));
	static QIcon s_default_icon = QIcon(":/SARibbon/image/resource/ribbonPanelOptionButton.png");
	setIcon(s_default_icon);
}

SARibbonPanelOptionButton::~SARibbonPanelOptionButton()
{
}
