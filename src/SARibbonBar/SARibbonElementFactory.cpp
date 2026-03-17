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

/**
 * \if ENGLISH
 * @brief Constructor for SARibbonElementFactory
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonElementFactory 构造函数
 * \endif
 */
SARibbonElementFactory::SARibbonElementFactory()
{
}

/**
 * \if ENGLISH
 * @brief Destructor for SARibbonElementFactory
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonElementFactory 析构函数
 * \endif
 */
SARibbonElementFactory::~SARibbonElementFactory()
{
}

/**
 * \if ENGLISH
 * @brief Create SARibbonBar
 * @param parent Parent widget
 * @return SARibbonBar pointer
 * \endif
 *
 * \if CHINESE
 * @brief 创建SARibbonBar
 * @param parent 父窗口
 * @return SARibbonBar指针
 * \endif
 */
SARibbonBar* SARibbonElementFactory::createRibbonBar(QWidget* parent)
{
    return (new SARibbonBar(parent));
}

/**
 * \if ENGLISH
 * @brief Create SARibbonTabBar
 * @param parent Parent widget
 * @return SARibbonTabBar pointer
 * \endif
 *
 * \if CHINESE
 * @brief 创建SARibbonTabBar
 * @param parent 父窗口
 * @return SARibbonTabBar指针
 * \endif
 */
SARibbonTabBar* SARibbonElementFactory::createRibbonTabBar(QWidget* parent)
{
    return (new SARibbonTabBar(parent));
}

/**
 * \if ENGLISH
 * @brief Create SARibbonApplicationButton
 * @param parent Parent widget
 * @return SARibbonApplicationButton pointer
 * \endif
 *
 * \if CHINESE
 * @brief 创建SARibbonApplicationButton
 * @param parent 父窗口
 * @return SARibbonApplicationButton指针
 * \endif
 */
SARibbonApplicationButton* SARibbonElementFactory::createRibbonApplicationButton(QWidget* parent)
{
    return (new SARibbonApplicationButton(parent));
}

/**
 * \if ENGLISH
 * @brief Create SARibbonCategory
 * @param parent Parent widget
 * @return SARibbonCategory pointer
 * \endif
 *
 * \if CHINESE
 * @brief 创建SARibbonCategory
 * @param parent 父窗口
 * @return SARibbonCategory指针
 * \endif
 */
SARibbonCategory* SARibbonElementFactory::createRibbonCategory(QWidget* parent)
{
    return (new SARibbonCategory(parent));
}

/**
 * \if ENGLISH
 * @brief Create SARibbonContextCategory
 * @param parent Parent widget
 * @return SARibbonContextCategory pointer
 * \endif
 *
 * \if CHINESE
 * @brief 创建SARibbonContextCategory
 * @param parent 父窗口
 * @return SARibbonContextCategory指针
 * \endif
 */
SARibbonContextCategory* SARibbonElementFactory::createRibbonContextCategory(QWidget* parent)
{
    return (new SARibbonContextCategory(parent));
}

/**
 * \if ENGLISH
 * @brief Create SARibbonPanel
 * @param parent Parent widget
 * @return SARibbonPanel pointer
 * \endif
 *
 * \if CHINESE
 * @brief 创建SARibbonPanel
 * @param parent 父窗口
 * @return SARibbonPanel指针
 * \endif
 */
SARibbonPanel* SARibbonElementFactory::createRibbonPanel(QWidget* parent)
{
    return (new SARibbonPanel(parent));
}

/**
 * \if ENGLISH
 * @brief Create SARibbonSeparatorWidget
 * @param parent Parent widget
 * @return SARibbonSeparatorWidget pointer
 * \endif
 *
 * \if CHINESE
 * @brief 创建SARibbonSeparatorWidget
 * @param parent 父窗口
 * @return SARibbonSeparatorWidget指针
 * \endif
 */
SARibbonSeparatorWidget* SARibbonElementFactory::createRibbonSeparatorWidget(QWidget* parent)
{
    return (new SARibbonSeparatorWidget(parent));
}

/**
 * \if ENGLISH
 * @brief Create SARibbonGallery
 * @param parent Parent widget
 * @return SARibbonGallery pointer
 * \endif
 *
 * \if CHINESE
 * @brief 创建SARibbonGallery
 * @param parent 父窗口
 * @return SARibbonGallery指针
 * \endif
 */
SARibbonGallery* SARibbonElementFactory::createRibbonGallery(QWidget* parent)
{
    return (new SARibbonGallery(parent));
}

/**
 * \if ENGLISH
 * @brief Create SARibbonGalleryGroup
 * @param parent Parent widget
 * @return SARibbonGalleryGroup pointer
 * \endif
 *
 * \if CHINESE
 * @brief 创建SARibbonGalleryGroup
 * @param parent 父窗口
 * @return SARibbonGalleryGroup指针
 * \endif
 */
SARibbonGalleryGroup* SARibbonElementFactory::createRibbonGalleryGroup(QWidget* parent)
{
    return (new SARibbonGalleryGroup(parent));
}

/**
 * \if ENGLISH
 * @brief Create SARibbonToolButton
 * @param parent Parent widget
 * @return SARibbonToolButton pointer
 * \endif
 *
 * \if CHINESE
 * @brief 创建SARibbonToolButton
 * @param parent 父窗口
 * @return SARibbonToolButton指针
 * \endif
 */
SARibbonToolButton* SARibbonElementFactory::createRibbonToolButton(QWidget* parent)
{
    return (new SARibbonToolButton(parent));
}

/**
 * \if ENGLISH
 * @brief Create SARibbonStackedWidget
 * @param parent SARibbonBar parent
 * @return SARibbonStackedWidget pointer
 * \endif
 *
 * \if CHINESE
 * @brief 创建SARibbonStackedWidget
 * @param parent SARibbonBar父窗口
 * @return SARibbonStackedWidget指针
 * \endif
 */
SARibbonStackedWidget* SARibbonElementFactory::createRibbonStackedWidget(SARibbonBar* parent)
{
    return (new SARibbonStackedWidget(parent));
}

/**
 * \if ENGLISH
 * @brief Create SARibbonButtonGroupWidget
 * @param parent Parent widget
 * @return SARibbonButtonGroupWidget pointer
 * \endif
 *
 * \if CHINESE
 * @brief 创建SARibbonButtonGroupWidget
 * @param parent 父窗口
 * @return SARibbonButtonGroupWidget指针
 * \endif
 */
SARibbonButtonGroupWidget* SARibbonElementFactory::createButtonGroupWidget(QWidget* parent)
{
    return (new SARibbonButtonGroupWidget(parent));
}

/**
 * \if ENGLISH
 * @brief Create SARibbonQuickAccessBar
 * @param parent Parent widget
 * @return SARibbonQuickAccessBar pointer
 * \endif
 *
 * \if CHINESE
 * @brief 创建SARibbonQuickAccessBar
 * @param parent 父窗口
 * @return SARibbonQuickAccessBar指针
 * \endif
 */
SARibbonQuickAccessBar* SARibbonElementFactory::createQuickAccessBar(QWidget* parent)
{
    return (new SARibbonQuickAccessBar(parent));
}

/**
 * \if ENGLISH
 * @brief Create SARibbonSystemButtonBar
 * @param parent Parent widget
 * @return SARibbonSystemButtonBar pointer
 * \endif
 *
 * \if CHINESE
 * @brief 创建SARibbonSystemButtonBar
 * @param parent 父窗口
 * @return SARibbonSystemButtonBar指针
 * \endif
 */
SARibbonSystemButtonBar* SARibbonElementFactory::createWindowButtonGroup(QWidget* parent)
{
    return (new SARibbonSystemButtonBar(parent));
}

/**
 * \if ENGLISH
 * @brief Create SARibbonPanelOptionButton
 * @param panel Associated panel
 * @return SARibbonPanelOptionButton pointer
 * @sa SARibbonPanelOptionButton
 * \endif
 *
 * \if CHINESE
 * @brief 创建SARibbonPanelOptionButton
 * @param panel 附属的panel
 * @return SARibbonPanelOptionButton指针
 * @sa SARibbonPanelOptionButton
 * \endif
 */
SARibbonPanelOptionButton* SARibbonElementFactory::createRibbonPanelOptionButton(SARibbonPanel* panel)
{
    return (new SARibbonPanelOptionButton(panel));
}

/**
 * \if ENGLISH
 * @brief Create SARibbonTitleIconWidget
 * @param parent Parent widget
 * @return SARibbonTitleIconWidget pointer
 * \endif
 *
 * \if CHINESE
 * @brief 创建SARibbonTitleIconWidget
 * @param parent 父窗口
 * @return SARibbonTitleIconWidget指针
 * \endif
 */
SARibbonTitleIconWidget* SARibbonElementFactory::createRibbonTitleIconWidget(QWidget* parent)
{
    return (new SARibbonTitleIconWidget(parent));
}

/**
 * \if ENGLISH
 * @brief Create SARibbonPanelLabel
 * @param parent Parent widget
 * @return SARibbonPanelLabel pointer
 * \endif
 *
 * \if CHINESE
 * @brief 创建SARibbonPanelLabel
 * @param parent 父窗口
 * @return SARibbonPanelLabel指针
 * \endif
 */
SARibbonPanelLabel* SARibbonElementFactory::createRibbonPanelLabel(QWidget* parent)
{
    return (new SARibbonPanelLabel(parent));
}
