#include "SARibbonStyleOption.h"

SARibbonStyleOption::SARibbonStyleOption()
{
    m_ribbonbarHeightOfficeStyleThreeRow = 160;
    m_widgetBord                         = QMargins(5, 1, 5, 5);
    m_titleBarHeight                     = 30;
    m_tabBarHeight                       = 25;
    recalc();
}

void SARibbonStyleOption::setTitleBarHeight(int h)
{
    m_titleBarHeight = h;
}

void SARibbonStyleOption::setRibbonBarHeightOfficeStyleThreeRow(int h)
{
    m_ribbonbarHeightOfficeStyleThreeRow = h;
}

void SARibbonStyleOption::setWidgetBord(const QMargins& m)
{
    m_widgetBord = m;
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

// int SARibbonStyleOption::pannelHeight(SARibbonPannel::PannelLayoutMode lm) const
//{
//    switch (lm) {
//    case SARibbonPannel::ThreeRowMode:
//        return m_pannelHeightThreeRow;
//    case SARibbonPannel::TwoRowMode:
//        return m_pannelHeightTwoRow;
//    default:
//        break;
//    }
//    return m_pannelHeightThreeRow;
//}

/**
 * @brief pannel的内容区域高度，减去了ContentsMargin
 * @param lm
 * @return
 */
// int SARibbonStyleOption::pannelContentsHeight(SARibbonPannel::PannelLayoutMode lm) const
//{
//    switch (lm) {
//    case SARibbonPannel::ThreeRowMode:
//        return m_pannelHeightThreeRow - m_pannelTitleHeight - m_pannelContentsMargins.top() - m_pannelContentsMargins.bottom();
//    case SARibbonPannel::TwoRowMode:
//        return m_pannelHeightTwoRow - m_pannelContentsMargins.top() - m_pannelContentsMargins.bottom();
//    default:
//        break;
//    }
//    return m_pannelHeightThreeRow;
//}

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

/**
 * @brief 整个ribbonbar的四个边框
 * @return
 */
const QMargins& SARibbonStyleOption::widgetBord() const
{
    return m_widgetBord;
}

void SARibbonStyleOption::recalc()
{
    m_ribbonbarHeightWPSStyleThreeRow  = calcMainbarHeight(SARibbonBar::WpsLiteStyle);
    m_ribbonbarHeightOfficeStyleTwoRow = calcMainbarHeight(SARibbonBar::OfficeStyleTwoRow);
    m_ribbonbarHeightWPSStyleTwoRow    = calcMainbarHeight(SARibbonBar::WpsLiteStyleTwoRow);
    //    m_pannelHeightThreeRow             = calcPannelHeight(SARibbonPannel::ThreeRowMode);
    //    m_pannelHeightTwoRow               = calcPannelHeight(SARibbonPannel::TwoRowMode);
}

/**
 * @brief 通过固有参数计算pannel的高度
 * @param lm
 * @return
 */
// int SARibbonStyleOption::calcPannelHeight(SARibbonPannel::PannelLayoutMode lm) const
//{
//    int r = m_ribbonbarHeightWPSStyleThreeRow - m_titleBarHeight - widgetBord().top() - widgetBord().bottom();
//    switch (lm) {
//    case SARibbonPannel::TwoRowMode:
//        r = m_ribbonbarHeightWPSStyleTwoRow - m_titleBarHeight - widgetBord().top() - widgetBord().bottom();
//    case SARibbonPannel::ThreeRowMode:
//        //注意是减m_titleBarHeight不是tabBarHight
//        break;
//    default:
//        break;
//    }
//    return r;
//}

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

SARibbonStyleOption sa_calc_default_ribbon_style_option()
{
    //    QFontMetrics fm = QApplication::fontMetrics();
    return SARibbonStyleOption();
}
