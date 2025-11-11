#include "SARibbonElementFactory.h"
#include <QApplication>
#include <QFontMetrics>
#include "SARibbonApplicationButton.h"
#include "SARibbonBar.h"
#include "SARibbonTabBar.h"
#include "SARibbonCategory.h"
#include "SARibbonContextCategory.h"
#include "SARibbonSeparatorWidget.h"
#include "SARibbonGallery.h"
#include "SARibbonGalleryGroup.h"
#include "SARibbonToolButton.h"
#include "SARibbonButtonGroupWidget.h"
#include "SARibbonStackedWidget.h"
#include "SARibbonQuickAccessBar.h"
#include "SARibbonPanelOptionButton.h"
#include "SARibbonPanelLayout.h"
#include "SARibbonSystemButtonBar.h"
#include "SARibbonTitleIconWidget.h"

SARibbonElementFactory::SARibbonElementFactory()
{
}

SARibbonElementFactory::~SARibbonElementFactory()
{
}

SARibbonBar* SARibbonElementFactory::createRibbonBar(QWidget* parent)
{
    return (new SARibbonBar(parent));
}

SARibbonTabBar* SARibbonElementFactory::createRibbonTabBar(QWidget* parent)
{
    return (new SARibbonTabBar(parent));
}

SARibbonApplicationButton* SARibbonElementFactory::createRibbonApplicationButton(QWidget* parent)
{
    return (new SARibbonApplicationButton(parent));
}

SARibbonCategory* SARibbonElementFactory::createRibbonCategory(QWidget* parent)
{
    return (new SARibbonCategory(parent));
}

SARibbonContextCategory* SARibbonElementFactory::createRibbonContextCategory(QWidget* parent)
{
    return (new SARibbonContextCategory(parent));
}

SARibbonPanel* SARibbonElementFactory::createRibbonPanel(QWidget* parent)
{
    return (new SARibbonPanel(parent));
}

SARibbonSeparatorWidget* SARibbonElementFactory::createRibbonSeparatorWidget(QWidget* parent)
{
    return (new SARibbonSeparatorWidget(parent));
}

SARibbonGallery* SARibbonElementFactory::createRibbonGallery(QWidget* parent)
{
    return (new SARibbonGallery(parent));
}

SARibbonGalleryGroup* SARibbonElementFactory::createRibbonGalleryGroup(QWidget* parent)
{
    return (new SARibbonGalleryGroup(parent));
}

SARibbonToolButton* SARibbonElementFactory::createRibbonToolButton(QWidget* parent)
{
    return (new SARibbonToolButton(parent));
}

SARibbonStackedWidget* SARibbonElementFactory::createRibbonStackedWidget(SARibbonBar* parent)
{
    return (new SARibbonStackedWidget(parent));
}

SARibbonButtonGroupWidget* SARibbonElementFactory::createButtonGroupWidget(QWidget* parent)
{
    return (new SARibbonButtonGroupWidget(parent));
}

SARibbonQuickAccessBar* SARibbonElementFactory::createQuickAccessBar(QWidget* parent)
{
    return (new SARibbonQuickAccessBar(parent));
}

SARibbonSystemButtonBar* SARibbonElementFactory::createWindowButtonGroup(QWidget* parent)
{
    return (new SARibbonSystemButtonBar(parent));
}

/**
 * @brief 创建SARibbonPanelOptionButton
 * @param panel 附属的panel
 * @return
 * @sa SARibbonPanelOptionButton
 */
SARibbonPanelOptionButton* SARibbonElementFactory::createRibbonPanelOptionButton(SARibbonPanel* panel)
{
    return (new SARibbonPanelOptionButton(panel));
}

SARibbonTitleIconWidget* SARibbonElementFactory::createRibbonTitleIconWidget(QWidget* parent)
{
    return (new SARibbonTitleIconWidget(parent));
}

/**
 * @brief 创建SARibbonPanelLabel
 * @param parent
 * @return
 */
SARibbonPanelLabel* SARibbonElementFactory::createRibbonPanelLabel(QWidget* parent)
{
    return (new SARibbonPanelLabel(parent));
}
