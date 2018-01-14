#ifndef SARIBBONELEMENTCREATEDELEGATE_H
#define SARIBBONELEMENTCREATEDELEGATE_H
#include "SARibbonGlobal.h"
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
///
/// \brief SARibbon的子元素创建的代理，SARibbon内部创建子元素都通过SARibbonElementCreateDelegate来创建
/// 如果有些子元素重载，如SARibbonCategory，可以重载此类的createRibbonCategory,返回重载的类来进行重载
///
class SA_RIBBON_EXPORT SARibbonElementCreateDelegate
{
public:
    SARibbonElementCreateDelegate();
    virtual ~SARibbonElementCreateDelegate();
    virtual SARibbonTabBar* createRibbonTabBar(QWidget* parent);
    virtual SARibbonApplicationButton* createRibbonApplicationButton(QWidget* parent);
    virtual SARibbonCategory* createRibbonCategory(QWidget* parent);
    virtual SARibbonContextCategory* createRibbonContextCategory(QWidget* parent);
    virtual SARibbonPannel* createRibbonPannel(QWidget* parent);
    virtual SARibbonSeparatorWidget* createRibbonSeparatorWidget(int value,QWidget* parent);
    virtual SARibbonGallery* createRibbonGallery(QWidget* parent);
    virtual SARibbonGalleryGroup* createRibbonGalleryGroup(QWidget* parent);
    virtual SARibbonToolButton* createRibbonToolButton(QWidget* parent);
    virtual SARibbonStackedWidget* createRibbonStackedWidget(SARibbonBar* parent);
    //创建隐藏ribbon的按钮代理函数
    virtual SARibbonControlButton* createHidePannelButton(SARibbonBar* parent);
    virtual SARibbonButtonGroupWidget* craeteButtonGroupWidget(QWidget* parent);
    virtual SARibbonQuickAccessBar* createQuickAccessBar(QWidget* parent);
};

#endif // SARIBBONELEMENTCREATEDELEGATE_H
