﻿#ifndef SARIBBONSTYLEOPTION_H
#define SARIBBONSTYLEOPTION_H
#include "SARibbonGlobal.h"
#include "SARibbonBar.h"

/**
 * @brief 定义了saribbon所有尺寸相关信息，saribbon的建立都基于此类的尺寸，如果想调整，
 * 可以通过 @ref SARibbonElementCreateDelegate 的 @ref setRibbonStyleOption 函数设置自己的SARibbonStyleOption
 *
 * 一般SARibbonElementCreateDelegate::setRibbonStyleOption函数最好在ribbonbar构建之前调用
 *
 * @note 此类定义了ribbonbar和pannel的高度信息，并通过字体提前计算好一些布局信息，
 * 以便给SARibbonToolButton使用，所以都定义在这里
 *
 * @todo 后续开发通过配置文件定义ribbon的尺寸布局
 */
class SA_RIBBON_EXPORT SARibbonStyleOption
{
public:
    SARibbonStyleOption();

public:
    //所有设置函数设置完后都建议调用recalc
    //设置标题栏高度
    void setTitleBarHeight(int h);

    //设置office 3行模式的高度，其他模式的高度会根据这个高度以及titleBarHight，tabBarHight，widgetBord计算出来
    void setRibbonBarHeightOfficeStyleThreeRow(int h);

    //设置边界偏移
    void setWidgetBord(const QMargins& m);

public:
    // ribbonBar的高度
    int ribbonBarHeight(SARibbonBar::RibbonStyle s) const;

    // pannel的高度（含标题）
    // int pannelHeight(SARibbonPannel::PannelLayoutMode lm) const;

    //获取pannel内容区域的高度
    // int pannelContentsHeight(SARibbonPannel::PannelLayoutMode lm) const;

    //标题栏的高度，对于wps模式，此参数没有用
    int titleBarHeight() const;

    //标签栏高度
    int tabBarHeight() const;

    // saribbonbar和窗体四周间距
    const QMargins& widgetBord() const;

    const QMargins& pannelContentsMargins() const;

public:
    //在改变了参数后对需要计算的变量从新计算
    void recalc();

    //计算和字体相关的高度，此函数在设置了字体后需要调用
    void recalcFontRefSize(const QFont& f);

protected:
    //通过已有参数计算pannel的高度
    //    int calcPannelHeight(SARibbonPannel::PannelLayoutMode lm) const;
    //计算ribbon的高度
    int calcMainbarHeight(SARibbonBar::RibbonStyle s) const;

private:
    QMargins m_widgetBord;                     ///< 整个ribbonbar的四个边框
    int m_tabBarHeight;                        ///< ribbon tab 的高度
    int m_titleBarHeight;                      ///< 标题栏高度
    int m_ribbonbarHeightOfficeStyleThreeRow;  ///< office样式的3行高度
    int m_ribbonbarHeightWPSStyleThreeRow;     ///< wps样式3行的高度
    int m_ribbonbarHeightWPSStyleTwoRow;       ///< wps样式2行的高度
    int m_ribbonbarHeightOfficeStyleTwoRow;    ///< office样式2行的高度
    //    int m_pannelHeightThreeRow;  ///< pannel在3行模式的高度，通过calcPannelHeight计算出来，前提是已知ribbonBarHeight
    //    int m_pannelHeightTwoRow;  ///< pannel在2行模式的高度，通过calcPannelHeight计算出来，前提是已知ribbonBarHeight
    /**
     * @brief SARibbonPannel标题栏的高度,pannel title hight必须这里定义，这样才能计算出pannel的高度，
     * 这样才能计算出两行模式和三行模式下的高度，因此，pannel的高度和pannel的margins必须这里定义
     *
     */
    int m_pannelTitleHeight;
    QMargins m_pannelContentsMargins;  ///< 整个ribbonbar的四个边框
};

/**
 * @brief 通过系统字体信息计算默认的ribbon style option
 * @return
 */
SA_RIBBON_EXPORT SARibbonStyleOption sa_calc_default_ribbon_style_option();

#endif  // SARIBBONSTYLEOPTION_H
