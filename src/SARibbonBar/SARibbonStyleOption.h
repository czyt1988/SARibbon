#ifndef SARIBBONSTYLEOPTION_H
#define SARIBBONSTYLEOPTION_H
#include "SARibbonGlobal.h"
#include "SARibbonBar.h"
#include <QDebug>
/**
 * @brief 定义了saribbon所有尺寸相关信息，saribbon的建立都基于此类的尺寸，如果想调整，
 * 可以通过 @ref SARibbonElementCreateDelegate（通过SARibbonElementManager单例管理） 的 @ref setRibbonStyleOption 函数设置自己的SARibbonStyleOption
 *
 * @sa SARibbonElementManager
 *
 * 一般SARibbonElementCreateDelegate::setRibbonStyleOption函数最好在ribbonbar构建之前调用
 *
 * @note 此类定义了ribbonbar和pannel的高度信息，并通过字体提前计算好一些布局信息
 *
 * @todo 后续开发通过配置文件定义ribbon的尺寸布局
 */
class SA_RIBBON_EXPORT SARibbonStyleOption
{
public:
    SARibbonStyleOption();

public:
    // ribbonBar的高度
    virtual int ribbonBarHeight(SARibbonBar::RibbonStyle s) const;

    //标题栏的高度，对于wps模式，此参数没有用
    virtual int titleBarHeight() const;

    //标签栏高度
    virtual int tabBarHeight() const;

    //在改变了参数后对需要计算的变量从新计算
    virtual void recalc();

protected:
    //通过已有参数计算pannel的高度
    //    int calcPannelHeight(SARibbonPannel::PannelLayoutMode lm) const;
    //计算ribbon的高度
    int calcMainbarHeight(SARibbonBar::RibbonStyle s) const;

private:
    int m_tabBarHeight;                        ///< ribbon tab 的高度
    int m_titleBarHeight;                      ///< 标题栏高度
    int m_ribbonbarHeightOfficeStyleThreeRow;  ///< office样式的3行高度
    int m_ribbonbarHeightWPSStyleThreeRow;     ///< wps样式3行的高度
    int m_ribbonbarHeightWPSStyleTwoRow;       ///< wps样式2行的高度
    int m_ribbonbarHeightOfficeStyleTwoRow;    ///< office样式2行的高度
};

SA_RIBBON_EXPORT QDebug operator<<(QDebug debug, const SARibbonStyleOption& c);
#endif  // SARIBBONSTYLEOPTION_H
