#ifndef SARIBBONELEMENTFACTORY_H
#define SARIBBONELEMENTFACTORY_H
#include "SARibbonGlobal.h"
#include <QColor>
#include <QMargins>
#include <QSize>
#include <QScopedPointer>
#include "SARibbonBar.h"
#include "SARibbonPanel.h"
class QWidget;
class SARibbonBar;
class SARibbonTabBar;
class SARibbonApplicationButton;
class SARibbonCategory;
class SARibbonContextCategory;
class SARibbonPanel;
class SARibbonSeparatorWidget;
class SARibbonGallery;
class SARibbonGalleryGroup;
class SARibbonToolButton;
class SARibbonButtonGroupWidget;
class SARibbonStackedWidget;
class SARibbonQuickAccessBar;
class SARibbonPanelOptionButton;
class SARibbonSystemButtonBar;
class SARibbonTitleIconWidget;
class SARibbonPanelLabel;
///
/// \brief SARibbon的子元素创建的工厂，SARibbon内部创建子元素都通过SARibbonElementFactory来创建
///
/// 由于SARibbonBar是一个复合控件，很多子窗口组合而成，有些部件的创建如果想继承，那么就需要这个工厂类来处理
/// 如SARibbonCategory，可以重载此类的createRibbonCategory,返回重载的类的实例
///
class SA_RIBBON_EXPORT SARibbonElementFactory
{
public:
    SARibbonElementFactory();
    virtual ~SARibbonElementFactory();
    virtual SARibbonBar* createRibbonBar(QWidget* parent);
    virtual SARibbonTabBar* createRibbonTabBar(QWidget* parent);
    virtual SARibbonApplicationButton* createRibbonApplicationButton(QWidget* parent);
    virtual SARibbonCategory* createRibbonCategory(QWidget* parent);
    virtual SARibbonContextCategory* createRibbonContextCategory(QWidget* parent);
    virtual SARibbonPanel* createRibbonPanel(QWidget* parent);
    virtual SARibbonSeparatorWidget* createRibbonSeparatorWidget(QWidget* parent);
    virtual SARibbonGallery* createRibbonGallery(QWidget* parent);
    virtual SARibbonGalleryGroup* createRibbonGalleryGroup(QWidget* parent);
    virtual SARibbonToolButton* createRibbonToolButton(QWidget* parent);
    virtual SARibbonStackedWidget* createRibbonStackedWidget(SARibbonBar* parent);
    virtual SARibbonButtonGroupWidget* createButtonGroupWidget(QWidget* parent);
    virtual SARibbonQuickAccessBar* createQuickAccessBar(QWidget* parent);
    virtual SARibbonSystemButtonBar* createWindowButtonGroup(QWidget* parent);
    virtual SARibbonPanelOptionButton* createRibbonPanelOptionButton(SARibbonPanel* panel);
    virtual SARibbonTitleIconWidget* createRibbonTitleIconWidget(QWidget* parent);
    virtual SARibbonPanelLabel* createRibbonPanelLabel(QWidget* parent);
};

#endif  // SARIBBONELEMENTFACTORY_H
