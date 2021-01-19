﻿#ifndef SARIBBONELEMENTCREATEDELEGATE_H
#define SARIBBONELEMENTCREATEDELEGATE_H
#include "SARibbonGlobal.h"
#include <QColor>
#include <QMargins>
class QWidget;
class SARibbonBar;
class SARibbonTabBar;
class SARibbonApplicationButton;
class SARibbonCategory;
class SARibbonContextCategory;
class SARibbonPannel;
class SARibbonSeparatorWidget;
class SARibbonGallery;
class SARibbonGalleryGroup;
class SARibbonToolButton;
class SARibbonControlButton;
class SARibbonButtonGroupWidget;
class SARibbonStackedWidget;
class SARibbonQuickAccessBar;
class SARibbonPannelOptionButton;

/**
 * @brief 定义了saribbon所有尺寸相关信息，saribbon的建立都基于此类的尺寸，如果想调整，
 * 可以重载 @ref SARibbonElementCreateDelegate 的 @ref getRibbonStyleOption 函数
 * 会重新
 */
class SA_RIBBON_EXPORT SARibbonStyleOption {
public:
    SARibbonStyleOption();
    virtual ~SARibbonStyleOption();
    int mainbarHeightOfficeStyleThreeRow;   ///< OfficeStyle 样式下的mainbar高度
    int mainbarHeightWPSStyleThreeRow;      ///< WpsLiteStyle 样式下的mainbar高度
    int mainbarHeightOfficeStyleTwoRow;     ///< OfficeStyleTwoRow 样式下的mainbar高度
    int mainbarHeightWPSStyleTwoRow;        ///< WpsLiteStyleTwoRow 样式下的mainbar高度
    int titleBarHight;                      ///< 标题栏高度
    int tabBarHight;                        ///< ribbon tab 的高度
    QColor titleTextColor;                  ///< 标题颜色
    QMargins widgetBord;                    ///< 整个ribbonbar的四个边框，默认为0
    QColor tabBarBaseLineColor;             ///< tabbar 底部线条颜色
};

///
/// \brief SARibbon的子元素创建的代理，SARibbon内部创建子元素都通过SARibbonElementCreateDelegate来创建
/// 如果有些子元素重载，如SARibbonCategory，可以重载此类的createRibbonCategory,返回重载的类来进行重载
///
class SA_RIBBON_EXPORT SARibbonElementCreateDelegate
{
public:
    SARibbonElementCreateDelegate();
    virtual ~SARibbonElementCreateDelegate();
    virtual SARibbonTabBar *createRibbonTabBar(QWidget *parent);
    virtual SARibbonApplicationButton *createRibbonApplicationButton(QWidget *parent);
    virtual SARibbonCategory *createRibbonCategory(QWidget *parent);
    virtual SARibbonContextCategory *createRibbonContextCategory(QWidget *parent);
    virtual SARibbonPannel *createRibbonPannel(QWidget *parent);
    virtual SARibbonSeparatorWidget *createRibbonSeparatorWidget(int value, QWidget *parent);
    virtual SARibbonSeparatorWidget *createRibbonSeparatorWidget(QWidget *parent);
    virtual SARibbonGallery *createRibbonGallery(QWidget *parent);
    virtual SARibbonGalleryGroup *createRibbonGalleryGroup(QWidget *parent);
    virtual SARibbonToolButton *createRibbonToolButton(QWidget *parent);
    virtual SARibbonStackedWidget *createRibbonStackedWidget(SARibbonBar *parent);

    //创建隐藏ribbon的按钮代理函数
    virtual SARibbonControlButton *createHidePannelButton(SARibbonBar *parent);
    virtual SARibbonButtonGroupWidget *craeteButtonGroupWidget(QWidget *parent);
    virtual SARibbonQuickAccessBar *createQuickAccessBar(QWidget *parent);

    //
    virtual const SARibbonStyleOption& getRibbonStyleOption() const;
    virtual void setRibbonStyleOption(SARibbonStyleOption *opt);

    //创建SARibbonPannelOptionButton
    virtual SARibbonPannelOptionButton *createRibbonPannelOptionButton(SARibbonPannel *pannel);

private:
    SARibbonStyleOption *m_opt;
};

#endif // SARIBBONELEMENTCREATEDELEGATE_H
