#include "SARibbonTabBar.h"
#include <QStyleOptionTab>
#include <QFontMetrics>

/**
 * \if ENGLISH
 * @brief Constructs a SARibbonTabBar instance
 * @param parent Parent widget
 * \endif
 *
 * \if CHINESE
 * @brief 构造一个 SARibbonTabBar 实例
 * @param parent 父窗口部件
 * \endif
 */
SARibbonTabBar::SARibbonTabBar(QWidget* parent) : QTabBar(parent), m_tabMargin(6, 0, 0, 0)
{
	setExpanding(false);
}

/**
 * \if ENGLISH
 * @brief Destructor
 * \endif
 *
 * \if CHINESE
 * @brief 析构函数
 * \endif
 */
SARibbonTabBar::~SARibbonTabBar()
{
}

/**
 * \if ENGLISH
 * @brief Gets the tab margin
 * @return The tab margin
 * \endif
 *
 * \if CHINESE
 * @brief 获取tab边距
 * @return tab边距
 * \endif
 */
const QMargins& SARibbonTabBar::tabMargin() const
{
	return (m_tabMargin);
}

/**
 * \if ENGLISH
 * @brief Sets the tab margin
 * @param tabMargin The tab margin to set
 * \endif
 *
 * \if CHINESE
 * @brief 设置tab边距
 * @param tabMargin 要设置的tab边距
 * \endif
 */
void SARibbonTabBar::setTabMargin(const QMargins& tabMargin)
{
	m_tabMargin = tabMargin;
}

/**
 * \if ENGLISH
 * @brief Estimates the size of a tab
 *
 * Unlike the system default tabbar, SARibbonTabBar's tab height is the same as the tabbar height, and does not consider vertical distribution
 * @param index The index of the tab
 * @return The estimated size of the tab
 * \endif
 *
 * \if CHINESE
 * @brief tab的尺寸预估
 *
 * 有别于系统默认的tabbar，SARibbonTabBar的tab高度和tabbar高度一致，且不考虑纵向分布情况
 * @param index tab的索引
 * @return tab的预估尺寸
 * \endif
 */
QSize SARibbonTabBar::tabSizeHint(int index) const
{
    if (index < 0) {
        return QSize();
    }
	QStyleOptionTab opt;
	initStyleOption(&opt, index);
	int hframe            = style()->pixelMetric(QStyle::PM_TabBarTabHSpace, &opt, this);
	const QFontMetrics fm = fontMetrics();

	int widgetWidth = 0;
	int padding     = 0;
	if (!opt.leftButtonSize.isEmpty()) {
		padding += 4;
		widgetWidth += opt.leftButtonSize.width();
	}
	if (!opt.rightButtonSize.isEmpty()) {
		padding += 4;
		widgetWidth += opt.rightButtonSize.width();
	}
	if (!opt.icon.isNull()) {
		padding += 4;
	}
	const int textWidth = fm.size(Qt::TextShowMnemonic, opt.text).width();
    QSize csz           = QSize(textWidth + opt.iconSize.width() + hframe + widgetWidth + padding, height());
    return style()->sizeFromContents(QStyle::CT_TabBarTab, &opt, csz, this);
}
