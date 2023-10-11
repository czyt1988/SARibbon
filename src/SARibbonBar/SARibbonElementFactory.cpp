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

SARibbonElementFactory::SARibbonElementFactory() : mStyleOption(new SARibbonStyleOption())
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

SARibbonSeparatorWidget* SARibbonElementFactory::createRibbonSeparatorWidget(int value, QWidget* parent)
{
    return (new SARibbonSeparatorWidget(value, parent));
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

SARibbonStackedWidget* SARibbonElementFactory::createRibbonStackedWidget(SARibbonBar* parent)
{
    return (new SARibbonStackedWidget(parent));
}

SARibbonControlButton* SARibbonElementFactory::createHidePannelButton(SARibbonBar* parent)
{
    SARibbonControlButton* btn = new SARibbonControlButton(parent);

    btn->setAutoRaise(false);
    btn->setObjectName(QStringLiteral("SARibbonBarHidePannelButton"));
    btn->setToolButtonStyle(Qt::ToolButtonIconOnly);
    btn->setFixedSize(parent->tabBarHeight() - 4, parent->tabBarHeight() - 4);
    return (btn);
}

SARibbonButtonGroupWidget* SARibbonElementFactory::craeteButtonGroupWidget(QWidget* parent)
{
    return (new SARibbonButtonGroupWidget(parent));
}

SARibbonQuickAccessBar* SARibbonElementFactory::createQuickAccessBar(QWidget* parent)
{
    return (new SARibbonQuickAccessBar(parent));
}

SARibbonStyleOption& SARibbonElementFactory::getRibbonStyleOption()
{
    return (*mStyleOption);
}

/**
 * @brief 设置style配置
 * @param opt
 */
void SARibbonElementFactory::setRibbonStyleOption(SARibbonStyleOption* opt)
{
    mStyleOption.reset(opt);
}

/**
 * @brief 创建SARibbonPannelOptionButton
 * @param pannel 附属的pannel
 * @return
 * @sa @ref SARibbonPannelOptionButton
 */
SARibbonPannelOptionButton* SARibbonElementFactory::createRibbonPannelOptionButton(SARibbonPannel* pannel)
{
    return (new SARibbonPannelOptionButton(pannel));
}
