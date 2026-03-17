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
/**
 * \if ENGLISH
 * @brief Factory class for creating SARibbon sub-elements
 * @details SARibbon creates all sub-elements through SARibbonElementFactory
 * @details Since SARibbonBar is a composite control composed of many sub-windows, some parts need this factory class to handle inheritance
 * @details For example, SARibbonCategory can be overloaded by overriding createRibbonCategory to return an instance of the overloaded class
 * \endif
 *
 * \if CHINESE
 * @brief SARibbon的子元素创建的工厂，SARibbon内部创建子元素都通过SARibbonElementFactory来创建
 * @details 由于SARibbonBar是一个复合控件，很多子窗口组合而成，有些部件的创建如果想继承，那么就需要这个工厂类来处理
 * @details 如SARibbonCategory，可以重载此类的createRibbonCategory,返回重载的类的实例
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonElementFactory
{
public:
    /// Constructor for SARibbonElementFactory
    SARibbonElementFactory();
    /// Destructor for SARibbonElementFactory
    virtual ~SARibbonElementFactory();
    /// Create SARibbonBar
    virtual SARibbonBar* createRibbonBar(QWidget* parent);
    /// Create SARibbonTabBar
    virtual SARibbonTabBar* createRibbonTabBar(QWidget* parent);
    /// Create SARibbonApplicationButton
    virtual SARibbonApplicationButton* createRibbonApplicationButton(QWidget* parent);
    /// Create SARibbonCategory
    virtual SARibbonCategory* createRibbonCategory(QWidget* parent);
    /// Create SARibbonContextCategory
    virtual SARibbonContextCategory* createRibbonContextCategory(QWidget* parent);
    /// Create SARibbonPanel
    virtual SARibbonPanel* createRibbonPanel(QWidget* parent);
    /// Create SARibbonSeparatorWidget
    virtual SARibbonSeparatorWidget* createRibbonSeparatorWidget(QWidget* parent);
    /// Create SARibbonGallery
    virtual SARibbonGallery* createRibbonGallery(QWidget* parent);
    /// Create SARibbonGalleryGroup
    virtual SARibbonGalleryGroup* createRibbonGalleryGroup(QWidget* parent);
    /// Create SARibbonToolButton
    virtual SARibbonToolButton* createRibbonToolButton(QWidget* parent);
    /// Create SARibbonStackedWidget
    virtual SARibbonStackedWidget* createRibbonStackedWidget(SARibbonBar* parent);
    /// Create SARibbonButtonGroupWidget
    virtual SARibbonButtonGroupWidget* createButtonGroupWidget(QWidget* parent);
    /// Create SARibbonQuickAccessBar
    virtual SARibbonQuickAccessBar* createQuickAccessBar(QWidget* parent);
    /// Create SARibbonSystemButtonBar
    virtual SARibbonSystemButtonBar* createWindowButtonGroup(QWidget* parent);
    /// Create SARibbonPanelOptionButton
    virtual SARibbonPanelOptionButton* createRibbonPanelOptionButton(SARibbonPanel* panel);
    /// Create SARibbonTitleIconWidget
    virtual SARibbonTitleIconWidget* createRibbonTitleIconWidget(QWidget* parent);
    /// Create SARibbonPanelLabel
    virtual SARibbonPanelLabel* createRibbonPanelLabel(QWidget* parent);
};

#endif  // SARIBBONELEMENTFACTORY_H
