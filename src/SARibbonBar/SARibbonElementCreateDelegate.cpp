#include "SARibbonElementCreateDelegate.h"
#include "SARibbonBar.h"
#include "SARibbonApplicationButton.h"
#include "SARibbonTabBar.h"
#include "SARibbonCategory.h"
#include "SARibbonContextCategory.h"
#include "SARibbonPannel.h"
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


SARibbonStyleOption::SARibbonStyleOption()
{
    mainbarHeightOfficeStyleThreeRow = 160;
    mainbarHeightWPSStyleThreeRow = 130;
    mainbarHeightOfficeStyleTwoRow = 134;
    mainbarHeightWPSStyleTwoRow = 104;
    widgetBord = QMargins(5, 1, 5, 5);
    titleBarHight = 30;
    titleTextColor = Qt::black;
    tabBarHight = 25;
    tabBarBaseLineColor = QColor(186, 201, 219);
    applicationButtonWidth = 60;
    galleryFixedHeight = 60;
    galleryMinimumWidth = 120;
    galleryButtonMaximumWidth = 15;
    pannelTitleHeight = 21;
    toolButtonLargeIconSize = QSize(32, 32);
    toolButtonSmallIconSize = QSize(18, 18);
}


SARibbonElementCreateDelegate::SARibbonElementCreateDelegate()
    : m_opt(new SARibbonStyleOption())
{
}


SARibbonElementCreateDelegate::~SARibbonElementCreateDelegate()
{
    if (m_opt) {
        delete m_opt;
    }
    m_opt = nullptr;
}


SARibbonTabBar *SARibbonElementCreateDelegate::createRibbonTabBar(QWidget *parent)
{
    return (new SARibbonTabBar(parent));
}


SARibbonApplicationButton *SARibbonElementCreateDelegate::createRibbonApplicationButton(QWidget *parent)
{
    return (new SARibbonApplicationButton(parent));
}


SARibbonCategory *SARibbonElementCreateDelegate::createRibbonCategory(QWidget *parent)
{
    return (new SARibbonCategory(parent));
}


SARibbonContextCategory *SARibbonElementCreateDelegate::createRibbonContextCategory(QWidget *parent)
{
    return (new SARibbonContextCategory(parent));
}


SARibbonPannel *SARibbonElementCreateDelegate::createRibbonPannel(QWidget *parent)
{
    return (new SARibbonPannel(parent));
}


SARibbonSeparatorWidget *SARibbonElementCreateDelegate::createRibbonSeparatorWidget(int value, QWidget *parent)
{
    return (new SARibbonSeparatorWidget(value, parent));
}


SARibbonSeparatorWidget *SARibbonElementCreateDelegate::createRibbonSeparatorWidget(QWidget *parent)
{
    return (new SARibbonSeparatorWidget(parent));
}


SARibbonGallery *SARibbonElementCreateDelegate::createRibbonGallery(QWidget *parent)
{
    return (new SARibbonGallery(parent));
}


SARibbonGalleryGroup *SARibbonElementCreateDelegate::createRibbonGalleryGroup(QWidget *parent)
{
    return (new SARibbonGalleryGroup(parent));
}


SARibbonToolButton *SARibbonElementCreateDelegate::createRibbonToolButton(QWidget *parent)
{
    return (new SARibbonToolButton(parent));
}


SARibbonStackedWidget *SARibbonElementCreateDelegate::createRibbonStackedWidget(SARibbonBar *parent)
{
    return (new SARibbonStackedWidget(parent));
}


SARibbonControlButton *SARibbonElementCreateDelegate::createHidePannelButton(SARibbonBar *parent)
{
    SARibbonControlButton *btn = new SARibbonControlButton(parent);

    btn->setAutoRaise(false);
    btn->setObjectName(QStringLiteral("SARibbonBarHidePannelButton"));
    btn->setToolButtonStyle(Qt::ToolButtonIconOnly);
    btn->setFixedSize(parent->tabBarHeight()-4, parent->tabBarHeight()-4);
    return (btn);
}


SARibbonButtonGroupWidget *SARibbonElementCreateDelegate::craeteButtonGroupWidget(QWidget *parent)
{
    return (new SARibbonButtonGroupWidget(parent));
}


SARibbonQuickAccessBar *SARibbonElementCreateDelegate::createQuickAccessBar(QWidget *parent)
{
    return (new SARibbonQuickAccessBar(parent));
}


const SARibbonStyleOption& SARibbonElementCreateDelegate::getRibbonStyleOption() const
{
    return (*m_opt);
}


/**
 * @brief 设置style配置类，SARibbonElementCreateDelegate将保留SARibbonStyleOption的所有权
 * @param opt
 */
void SARibbonElementCreateDelegate::setRibbonStyleOption(SARibbonStyleOption *opt)
{
    if (m_opt) {
        delete m_opt;
    }
    m_opt = opt;
}


/**
 * @brief 创建SARibbonPannelOptionButton
 * @param pannel 附属的pannel
 * @return
 * @sa @ref SARibbonPannelOptionButton
 */
SARibbonPannelOptionButton *SARibbonElementCreateDelegate::createRibbonPannelOptionButton(SARibbonPannel *pannel)
{
    return (new SARibbonPannelOptionButton(pannel));
}
