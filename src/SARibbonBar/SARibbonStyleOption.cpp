#include "SARibbonStyleOption.h"
#include <QApplication>
#include <QDir>
#include <QSettings>
#include <QDebug>
SARibbonStyleOption::SARibbonStyleOption(bool isUseSetting):m_tabBarHeightScaling(1.5),m_titleBarHeightScaling(1.8)
{
    if(isUseSetting)
    {
        setParamFromIni("");
    }
    init();
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
    updateMainbarHeight();
}

/**
 * @brief SARibbonStyleOption::setParamFromIni 通过ini文件更改相关参数
 * @param iniPath ini的路径
 */
bool SARibbonStyleOption::setParamFromIni(QString iniPath)
{
    if(iniPath.isEmpty())
    {
        iniPath=qApp->applicationDirPath()+"/setting.ini";
    }

    QFileInfo fileParse(iniPath);

    //目前是不存在直接返回
    if((!fileParse.exists()))
        return false;

    QSettings setting(iniPath,QSettings::IniFormat);
    m_tabBarHeightScaling=setting.value("/SARibbonStyleOption/tabBarHeightScaling").toDouble();
    m_titleBarHeightScaling=setting.value("/SARibbonStyleOption/titleBarHeightScaling").toDouble();

    //先判断存不存在文件夹，不存在直接返回失败（防止写错，造成创建大量文件夹）
    //但是如果不存在ini配置文件，则新建一个
//    QDir folder;
//    if(!folder.exists(fileParse.path()))
//        return false;
    //仔细想了下，创建文件还是先不要考虑，如果以后需要创建的话，应该就需要保存，目前只需要读取

    return true;
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
        return m_ribbonbarHeightOfficeStyleThreeRow * 0.9 - m_tabBarHeight - SARibbonPannel::pannelTitleHeight();
    case SARibbonBar::OfficeStyleTwoRow:
        return m_ribbonbarHeightOfficeStyleThreeRow * 0.9 - SARibbonPannel::pannelTitleHeight();
    default:
        break;
    }
    return m_ribbonbarHeightOfficeStyleThreeRow;
}

void SARibbonStyleOption::init()
{
    QFontMetrics fm = QApplication::fontMetrics();
    int lineSpacing = fm.lineSpacing();

    m_titleBarHeight                     = lineSpacing * m_titleBarHeightScaling;
    m_tabBarHeight                       = lineSpacing * m_tabBarHeightScaling;
    m_ribbonbarHeightOfficeStyleThreeRow = m_titleBarHeight + m_tabBarHeight + (lineSpacing * 1.5) * 3
                                           + SARibbonPannel::pannelTitleHeight()
                                           + SARibbonPannelLayout::pannelContentsMargins().bottom()
                                           + SARibbonPannelLayout::pannelContentsMargins().top();
    updateMainbarHeight();
}

void SARibbonStyleOption::updateMainbarHeight()
{
    m_ribbonbarHeightWPSStyleThreeRow  = calcMainbarHeight(SARibbonBar::WpsLiteStyle);
    m_ribbonbarHeightOfficeStyleTwoRow = calcMainbarHeight(SARibbonBar::OfficeStyleTwoRow);
    m_ribbonbarHeightWPSStyleTwoRow    = calcMainbarHeight(SARibbonBar::WpsLiteStyleTwoRow);
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
