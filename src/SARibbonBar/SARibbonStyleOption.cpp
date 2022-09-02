#include "SARibbonStyleOption.h"
#include <QApplication>
#include <QDebug>
SARibbonStyleOption::SARibbonStyleOption()
{
    QFontMetrics fm = QApplication::fontMetrics();
    int lineSpacing = fm.lineSpacing();

    m_titleBarHeight                     = lineSpacing * 1.8;
    m_tabBarHeight                       = lineSpacing * 1.5;
    m_ribbonbarHeightOfficeStyleThreeRow = m_titleBarHeight + m_tabBarHeight + (lineSpacing * 1.5) * 3
                                           + SARibbonPannel::pannelTitleHeight()
                                           + SARibbonPannelLayout::pannelContentsMargins().bottom()
                                           + SARibbonPannelLayout::pannelContentsMargins().top();
    recalc();
}

int SARibbonStyleOption::ribbonBarHeight(SARibbonBar::RibbonStyle s) const
{
    switch (s) {
    case SARibbonBar::OfficeStyle:
        return m_ribbonbarHeightOfficeStyleThreeRow;
    case SARibbonBar::OfficeStyleTwoRow:
        return m_ribbonbarHeightOfficeStyleTwoRow;
    case SARibbonBar::WpsLiteStyle:
        return m_ribbonbarHeightWPSStyleThreeRow;
    case SARibbonBar::WpsLiteStyleTwoRow:
        return m_ribbonbarHeightWPSStyleTwoRow;
    default:
        break;
    }
    return m_ribbonbarHeightOfficeStyleThreeRow;
}

/**
 * @brief 标题栏高度
 * @return
 */
int SARibbonStyleOption::titleBarHeight() const
{
    return m_titleBarHeight;
}

int SARibbonStyleOption::tabBarHeight() const
{
    return m_tabBarHeight;
}

void SARibbonStyleOption::recalc()
{
    m_ribbonbarHeightWPSStyleThreeRow  = calcMainbarHeight(SARibbonBar::WpsLiteStyle);
    m_ribbonbarHeightOfficeStyleTwoRow = calcMainbarHeight(SARibbonBar::OfficeStyleTwoRow);
    m_ribbonbarHeightWPSStyleTwoRow    = calcMainbarHeight(SARibbonBar::WpsLiteStyleTwoRow);
}

/**
 * @brief 计算ribbon的高度
 *
 * @note 调用此函数之前必须调用
 * @param s
 * @return
 */
int SARibbonStyleOption::calcMainbarHeight(SARibbonBar::RibbonStyle s) const
{
    switch (s) {
    case SARibbonBar::WpsLiteStyle:
        // 不是减去m_titleBarHeight原因是绘制wps的样式时，标题栏是存在，只是把bar画在标题栏上，相当于没有bar
        return m_ribbonbarHeightOfficeStyleThreeRow - m_tabBarHeight;
    case SARibbonBar::WpsLiteStyleTwoRow:
        //两行模式把标题栏去掉
        return m_ribbonbarHeightOfficeStyleThreeRow * 0.95 - m_tabBarHeight - SARibbonPannel::pannelTitleHeight();
    case SARibbonBar::OfficeStyleTwoRow:
        return m_ribbonbarHeightOfficeStyleThreeRow * 0.95 - SARibbonPannel::pannelTitleHeight();
    default:
        break;
    }
    return m_ribbonbarHeightOfficeStyleThreeRow;
}

/**
 * @brief 对SARibbonStyleOption输出
 * @param debug
 * @param c
 * @return
 */
QDebug operator<<(QDebug debug, const SARibbonStyleOption& c)
{
    QDebugStateSaver saver(debug);
    Q_UNUSED(saver);
    debug.nospace() << "fontMetrics.lineSpacing=" << QApplication::fontMetrics().lineSpacing()
                    << ",SARibbonStyleOption(titleBarHeight=" << c.titleBarHeight() << ",tabBarHeight=" << c.tabBarHeight()
                    << "\n,ribbonBarHeight(OfficeStyle)=" << c.ribbonBarHeight(SARibbonBar::OfficeStyle)
                    << "\n,ribbonBarHeight(OfficeStyleTwoRow)=" << c.ribbonBarHeight(SARibbonBar::OfficeStyleTwoRow)
                    << "\n,ribbonBarHeight(WpsLiteStyle)=" << c.ribbonBarHeight(SARibbonBar::WpsLiteStyle)
                    << "\n,ribbonBarHeight(WpsLiteStyleTwoRow)=" << c.ribbonBarHeight(SARibbonBar::WpsLiteStyleTwoRow);
    return debug;
}
