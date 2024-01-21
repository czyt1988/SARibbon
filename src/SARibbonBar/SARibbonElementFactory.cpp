#include "SARibbonElementFactory.h"
#include <QApplication>
#include <QFontMetrics>
#include "SARibbonApplicationButton.h"
#include "SARibbonTabBar.h"
#include "SARibbonCategory.h"
#include "SARibbonContextCategory.h"
#include "SARibbonSeparatorWidget.h"
#include "SARibbonGallery.h"
#include "SARibbonGalleryGroup.h"
#include "SARibbonToolButton.h"
#include "SARibbonControlButton.h"
#include "SARibbonButtonGroupWidget.h"
#include "SARibbonStackedWidget.h"
#include "SARibbonQuickAccessBar.h"
#include "SARibbonPannelOptionButton.h"
#include "SARibbonPannelLayout.h"
#include "SARibbonSystemButtonBar.h"

SARibbonElementFactory::SARibbonElementFactory()
{
}

SARibbonElementFactory::~SARibbonElementFactory()
{
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

SARibbonPannel* SARibbonElementFactory::createRibbonPannel(QWidget* parent)
{
    return (new SARibbonPannel(parent));
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

SARibbonControlButton* SARibbonElementFactory::createRibbonControlButton(QWidget* parent)
{
    return (new SARibbonControlButton(parent));
}

SARibbonControlToolButton* SARibbonElementFactory::createRibbonControlToolButton(QWidget* parent)
{
    return (new SARibbonControlToolButton(parent));
}

SARibbonStackedWidget* SARibbonElementFactory::createRibbonStackedWidget(SARibbonBar* parent)
{
    return (new SARibbonStackedWidget(parent));
}

SARibbonButtonGroupWidget* SARibbonElementFactory::craeteButtonGroupWidget(QWidget* parent)
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
 * @brief 创建SARibbonPannelOptionButton
 * @param pannel 附属的pannel
 * @return
 * @sa SARibbonPannelOptionButton
 */
SARibbonPannelOptionButton* SARibbonElementFactory::createRibbonPannelOptionButton(SARibbonPannel* pannel)
{
    return (new SARibbonPannelOptionButton(pannel));
}
