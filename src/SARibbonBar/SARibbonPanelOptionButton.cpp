#include "SARibbonPanelOptionButton.h"
#include <QAction>

/**
 * \if ENGLISH
 * @brief Constructor for SARibbonPanelOptionButton
 * @param parent Parent widget
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonPanelOptionButton 构造函数
 * @param parent 父窗口
 * \endif
 */
SARibbonPanelOptionButton::SARibbonPanelOptionButton(QWidget* parent) : QToolButton(parent)
{
	setAutoRaise(true);
	setCheckable(false);
	setToolButtonStyle(Qt::ToolButtonIconOnly);
	setIconSize(QSize(10, 10));
	static QIcon s_default_icon = QIcon(":/SARibbon/image/resource/ribbonPanelOptionButton.png");
	setIcon(s_default_icon);
}

/**
 * \if ENGLISH
 * @brief Destructor for SARibbonPanelOptionButton
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonPanelOptionButton 析构函数
 * \endif
 */
SARibbonPanelOptionButton::~SARibbonPanelOptionButton()
{
}
