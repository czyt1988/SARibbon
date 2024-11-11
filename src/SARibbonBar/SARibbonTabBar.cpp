#include "SARibbonTabBar.h"
#include <QStyleOptionTab>
#include <QFontMetrics>

SARibbonTabBar::SARibbonTabBar(QWidget* parent) : QTabBar(parent), m_tabMargin(6, 0, 0, 0)
{
	setExpanding(false);
}

SARibbonTabBar::~SARibbonTabBar()
{
}

const QMargins& SARibbonTabBar::tabMargin() const
{
	return (m_tabMargin);
}

void SARibbonTabBar::setTabMargin(const QMargins& tabMargin)
{
	m_tabMargin = tabMargin;
}

/**
 * @brief tab的尺寸预估
 *
 * 有别于系统默认的tabbar，SARibbonTabBar的tab高度和tabbar高度一致，且不考虑纵向分布情况
 * @param index
 * @return
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
