#include "SARibbonBarLayout.h"
#include <QStyle>
#include <QApplication>
#include <QScreen>
#include "SARibbonBar.h"
#include "SARibbonTabBar.h"
#include "SARibbonStackedWidget.h"
#include "SARibbonQuickAccessBar.h"
#include "SARibbonButtonGroupWidget.h"
#include "SARibbonApplicationButton.h"
#include "SARibbonCategory.h"
#include "SARibbonElementManager.h"
#include "SARibbonUtil.h"
#include "SARibbonTitleIconWidget.h"

class SARibbonBarLayout::PrivateData
{
public:
    SARibbonBar* ribbonBar;
    QList< QLayoutItem* > items;
    QRect titleRect;
    int titleBarHeight { 30 };    ///< Title bar height
    int tabBarHeight { 28 };      ///< Tab bar height
    int panelTitleHeight { 15 };  ///< Panel title default height
    int categoryHeight { 60 };    ///< Category height

    int maxMinWidth { 1000 };  ///< Maximum minimum width, usually 0.8 of screen width to avoid exceeding screen
    int minWidth { 500 };
    int minHeight { 0 };
    bool isTabOnTitle { false };  ///< Whether tab is on title bar
    std::unique_ptr< int > userDefTitleBarHeight;  ///< User-defined title bar height, normally auto-calculated
    std::unique_ptr< int > userDefTabBarHeight;  ///< User-defined tab bar height, normally auto-calculated
    std::unique_ptr< int > userDefCategoryHeight;  ///< User-defined category height, normally auto-calculated
    QSize systemButtonSize;  ///< System button size (close, maximize, etc.) from SARibbonMainWindow
    bool isApplicationButtonVerticalExpansion {
        false
    };  ///< Whether application button is vertically expanded, which will occupy both title and tab heights

public:
    /**
     * \if ENGLISH
     * @brief Constructor for PrivateData
     * @param bar The SARibbonBar instance
     * \endif
     * 
     * \if CHINESE
     * @brief PrivateData 构造函数
     * @param bar SARibbonBar 实例
     * \endif
     */
    PrivateData(SARibbonBar* bar) : ribbonBar(bar), systemButtonSize(0, 0)
    {
        // Get primary screen size
        QScreen* primaryScreen = QGuiApplication::primaryScreen();
        QRect screenGeometry   = primaryScreen->geometry();
        if (currentRibbonMode() == SARibbonBar::MinimumRibbonMode) {
            minHeight = getActualTitleBarHeight() + (isTabOnTitle ? 0 : getActualTabBarHeight());
        } else {
            minHeight = getActualTitleBarHeight() + getActualCategoryHeight()
                        + (isTabOnTitle ? 0 : getActualTabBarHeight());
        }
        maxMinWidth = screenGeometry.width() * 0.8;  // Screen width
    }

    /**
     * \if ENGLISH
     * @brief Get the ribbon tab bar
     * @return The SARibbonTabBar instance
     * \endif
     * 
     * \if CHINESE
     * @brief 获取功能区标签栏
     * @return SARibbonTabBar 实例
     * \endif
     */
    SARibbonTabBar* ribbonTabBar() const
    {
        return ribbonBar->ribbonTabBar();
    }

    /**
     * \if ENGLISH
     * @brief Get the stacked container widget
     * @return The SARibbonStackedWidget instance
     * \endif
     * 
     * \if CHINESE
     * @brief 获取堆叠容器窗口
     * @return SARibbonStackedWidget 实例
     * \endif
     */
    SARibbonStackedWidget* stackedContainerWidget() const
    {
        return ribbonBar->ribbonStackedWidget();
    }

    /**
     * \if ENGLISH
     * @brief Get the quick access bar
     * @return The SARibbonQuickAccessBar instance
     * \endif
     * 
     * \if CHINESE
     * @brief 获取快速访问栏
     * @return SARibbonQuickAccessBar 实例
     * \endif
     */
    SARibbonQuickAccessBar* quickAccessBar() const
    {
        return ribbonBar->quickAccessBar();
    }

    /**
     * \if ENGLISH
     * @brief Get the right button group
     * @return The SARibbonButtonGroupWidget instance
     * \endif
     * 
     * \if CHINESE
     * @brief 获取右侧按钮组
     * @return SARibbonButtonGroupWidget 实例
     * \endif
     */
    SARibbonButtonGroupWidget* rightButtonGroup() const
    {
        return ribbonBar->rightButtonGroup();
    }

    /**
     * \if ENGLISH
     * @brief Get the title icon widget
     * @return The SARibbonTitleIconWidget instance
     * \endif
     * 
     * \if CHINESE
     * @brief 获取标题图标窗口
     * @return SARibbonTitleIconWidget 实例
     * \endif
     */
    SARibbonTitleIconWidget* titleIconWidget() const
    {
        return ribbonBar->titleIconWidget();
    }

    /**
     * \if ENGLISH
     * @brief Get the application button
     * @return The application button
     * \endif
     * 
     * \if CHINESE
     * @brief 获取应用程序按钮
     * @return 应用程序按钮
     * \endif
     */
    QAbstractButton* applicationButton() const
    {
        return ribbonBar->applicationButton();
    }

    /**
     * \if ENGLISH
     * @brief Get the current ribbon mode
     * @return The current ribbon mode
     * \endif
     * 
     * \if CHINESE
     * @brief 获取当前功能区模式
     * @return 当前功能区模式
     * \endif
     */
    SARibbonBar::RibbonMode currentRibbonMode() const
    {
        return ribbonBar->currentRibbonState();
    }

    /**
     * \if ENGLISH
     * @brief Get the contents margins
     * @return The contents margins
     * \endif
     * 
     * \if CHINESE
     * @brief 获取内容边距
     * @return 内容边距
     * \endif
     */
    QMargins contentsMargins() const
    {
        return ribbonBar->contentsMargins();
    }

    /**
     * \if ENGLISH
     * @brief Get the ribbon style
     * @return The current ribbon style
     * \endif
     * 
     * \if CHINESE
     * @brief 获取功能区样式
     * @return 当前功能区样式
     * \endif
     */
    SARibbonBar::RibbonStyles ribbonStyle() const
    {
        return ribbonBar->currentRibbonStyle();
    }

    /**
     * \if ENGLISH
     * @brief Set the title bar height
     * @param h The height to set
     * \endif
     * 
     * \if CHINESE
     * @brief 设置标题栏高度
     * @param h 要设置的高度
     * \endif
     */
    void setTitleBarHeight(int h)
    {
        if (!userDefTitleBarHeight) {
            userDefTitleBarHeight = std::make_unique< int >(h);
        } else {
            *userDefTitleBarHeight = h;
        }
    }

    /**
     * \if ENGLISH
     * @brief Set the tab bar height
     * @param h The height to set
     * \endif
     * 
     * \if CHINESE
     * @brief 设置标签栏高度
     * @param h 要设置的高度
     * \endif
     */
    void setTabBarHeight(int h)
    {
        if (!userDefTabBarHeight) {
            userDefTabBarHeight = std::make_unique< int >(h);
        } else {
            *userDefTabBarHeight = h;
        }
    }

    /**
     * \if ENGLISH
     * @brief Set the category height
     * @param h The height to set
     * \endif
     * 
     * \if CHINESE
     * @brief 设置类别高度
     * @param h 要设置的高度
     * \endif
     */
    void setCategoryHeight(int h)
    {
        if (!userDefCategoryHeight) {
            userDefCategoryHeight = std::make_unique< int >(h);
        } else {
            *userDefCategoryHeight = h;
        }
    }

    /**
     * \if ENGLISH
     * @brief Get the actual title bar height
     * @return The actual title bar height
     * \endif
     * 
     * \if CHINESE
     * @brief 获取实际标题栏高度
     * @return 实际标题栏高度
     * \endif
     */
    int getActualTitleBarHeight() const
    {
        if (userDefTitleBarHeight) {
            return *userDefTitleBarHeight;
        } else {
            return titleBarHeight;
        }
    }

    /**
     * \if ENGLISH
     * @brief Get the actual tab bar height
     * @return The actual tab bar height
     * \endif
     * 
     * \if CHINESE
     * @brief 获取实际标签栏高度
     * @return 实际标签栏高度
     * \endif
     */
    int getActualTabBarHeight() const
    {
        if (userDefTabBarHeight) {
            return *userDefTabBarHeight;
        } else {
            return tabBarHeight;
        }
    }

    /**
     * \if ENGLISH
     * @brief Get the actual category height
     * @return The actual category height
     * \endif
     * 
     * \if CHINESE
     * @brief 获取实际类别高度
     * @return 实际类别高度
     * \endif
     */
    int getActualCategoryHeight() const
    {
        if (userDefCategoryHeight) {
            return *userDefCategoryHeight;
        } else {
            return categoryHeight;
        }
    }

    /**
     * \if ENGLISH
     * @brief Estimate size hints for various components
     * \endif
     * 
     * \if CHINESE
     * @brief 估算各个组件的尺寸提示
     * \endif
     */
    void estimateSizeHint()
    {
        titleBarHeight = calcDefaultTitleBarHeight();
        // If tabBarHeight is greater than 0, use user-set value
        tabBarHeight   = calcDefaultTabBarHeight();
        categoryHeight = calcCategoryHeight();
    }

    /**
     * \if ENGLISH
     * @brief Get the system tab bar height
     * @return The system tab bar height
     * \endif
     * 
     * \if CHINESE
     * @brief 获取系统标签栏高度
     * @return 系统标签栏高度
     * \endif
     */
    int systemTabBarHeight() const
    {
        QStyle* style = ribbonBar->style();
        return style->pixelMetric(QStyle::PM_TabBarBaseHeight) + style->pixelMetric(QStyle::PM_TabBarTabHSpace)
               + style->pixelMetric(QStyle::PM_TabBarTabOverlap);
    }

    /**
     * \if ENGLISH
     * @brief Calculate default tab bar height
     * @return The calculated tab bar height
     * \endif
     * 
     * \if CHINESE
     * @brief 估算标签栏的高度
     * @return 计算出的标签栏高度
     * \endif
     */
    int calcDefaultTabBarHeight()
    {
        int defaultHeight = systemTabBarHeight();
        int fontHeight = ribbonBar->fontMetrics().lineSpacing();  // Use lineSpacing instead of height for better font compatibility
        int defaultHeight2 = fontHeight * 1.6;
        if (defaultHeight2 < fontHeight + 10) {
            defaultHeight2 = fontHeight + 10;  // To accommodate office2021 theme with 4px bottom bar
        }
        int r = qMax(defaultHeight, defaultHeight2);
        if (r < 20) {
            r = 20;
        }
        return r;
    }

    /**
     * \if ENGLISH
     * @brief Calculate default title bar height
     * @return The calculated title bar height
     * \endif
     * 
     * \if CHINESE
     * @brief 估算标题栏的高度
     * @return 计算出的标题栏高度
     * \endif
     */
    int calcDefaultTitleBarHeight()
    {
        int defaultHeight  = ribbonBar->style()->pixelMetric(QStyle::PM_TitleBarHeight);
        int defaultHeight2 = ribbonBar->fontMetrics().height() * 1.8;
        int r              = qMax(defaultHeight, defaultHeight2);
        if (r < 25) {
            r = 25;
        }
        return r;
    }

    /**
     * \if ENGLISH
     * @brief Calculate category height based on current ribbon style
     * @note 1.6 line height is close to Office's height.
     *       SingleRow mode has no panel title, so panelTitleHeight is not added.
     * @return The calculated category height
     * \endif
     *
     * \if CHINESE
     * @brief 根据当前Ribbon风格估算类别的高度
     * @note 经过对照，1.6行高和office的高度比较接近。
     *       SingleRow模式下面板标题隐藏，因此不添加panelTitleHeight。
     * @return 计算出的类别高度
     * \endif
     */
    int calcCategoryHeight()
    {
        int textH = ribbonBar->fontMetrics().lineSpacing();
        if (ribbonBar->isThreeRowStyle()) {
            // 4.8 = 3*1.6 for three rows
            return textH * 4.8 + panelTitleHeight;
        } else if (ribbonBar->isSingleRowStyle()) {
            // 1.6 for single row, no panel title in single-row mode
            return textH * 1.6;
        } else {
            // 3.2 = 2*1.6 for two rows
            return textH * 3.2 + panelTitleHeight;
        }
    }

    /**
     * \if ENGLISH
     * @brief Calculate main bar height
     * @param tabHegith Tab bar height
     * @param titleHeight Title bar height
     * @param categoryHeight Category height
     * @param tabOnTitle Whether tab is on title
     * @param rMode Ribbon mode
     * @return The calculated main bar height
     * \endif
     * 
     * \if CHINESE
     * @brief 计算主栏高度
     * @param tabHegith 标签栏高度
     * @param titleHeight 标题栏高度
     * @param categoryHeight 类别高度
     * @param tabOnTitle 标签是否在标题栏上
     * @param rMode 功能区模式
     * @return 计算出的主栏高度
     * \endif
     */
    static int
    calcMainBarHeight(int tabHegith, int titleHeight, int categoryHeight, bool tabOnTitle, SARibbonBar::RibbonMode rMode)
    {
        if (rMode == SARibbonBar::MinimumRibbonMode) {
            // Minimum mode, no category height
            if (tabOnTitle) {
                return titleHeight;
            } else {
                return titleHeight + tabHegith;
            }
        } else {
            if (tabOnTitle) {
                return titleHeight + categoryHeight;
            } else {
                return tabHegith + titleHeight + categoryHeight;
            }
        }
        return tabHegith + titleHeight + categoryHeight;
    }

    /**
     * \if ENGLISH
     * @brief Reset the size
     * \endif
     * 
     * \if CHINESE
     * @brief 重新计算尺寸
     * \endif
     */
    void resetSize()
    {
        estimateSizeHint();
        int mainBarHeight = calcMainBarHeight(getActualTabBarHeight(),
                                              getActualTitleBarHeight(),
                                              getActualCategoryHeight(),
                                              isTabOnTitle,
                                              ribbonBar->currentRibbonState());
        // In minimum mode, bar height is tab bar bottom, this adjustment must be after resize event
        // After upgrading to Qt 6, setFixedHeight can cause exceptions because:
        // void QWidget::setFixedHeight(int h) {
        //     // ...
        //     setMinimumSize(minimumSize().width(), h);  // Set minimum height first
        //     setMaximumSize(maximumSize().width(), h);  // Then set maximum height
        //     // ...
        // }
        // If maximumSize().height() is already less than h before calling, setting minimumSize(h) first
        // will make minimum height greater than current maximum height, triggering Qt 6 assertion check.
        // Therefore, reset maximum size limit before calling setFixedHeight:
        int maximumHeight = ribbonBar->maximumHeight();
        if (maximumHeight < mainBarHeight) {
            ribbonBar->setMaximumHeight(QWIDGETSIZE_MAX);
        }
        ribbonBar->setFixedHeight(mainBarHeight);
        minHeight = mainBarHeight;  // minHeight matches mainBarHeight
    }

    /**
     * \if ENGLISH
     * @brief Get main bar height in minimum mode
     * @return Main bar height in minimum mode
     * \endif
     * 
     * \if CHINESE
     * @brief 最小模式下的主栏高度
     * @return 最小模式下的主栏高度
     * \endif
     */
    int minimumModeMainBarHeight()
    {
        return calcMainBarHeight(getActualTabBarHeight(),
                                 getActualTitleBarHeight(),
                                 getActualCategoryHeight(),
                                 isTabOnTitle,
                                 SARibbonBar::MinimumRibbonMode);
    }

    /**
     * \if ENGLISH
     * @brief Get main bar height in normal mode
     * @return Main bar height in normal mode
     * \endif
     * 
     * \if CHINESE
     * @brief 正常模式下的主栏高度
     * @return 正常模式下的主栏高度
     * \endif
     */
    int normalModeMainBarHeight()
    {
        return calcMainBarHeight(getActualTabBarHeight(),
                                 getActualTitleBarHeight(),
                                 getActualCategoryHeight(),
                                 isTabOnTitle,
                                 SARibbonBar::NormalRibbonMode);
    }
};

/**
 * \if ENGLISH
 * @brief Constructor for SARibbonBarLayout
 * @param parent The parent SARibbonBar
 * \endif
 * 
 * \if CHINESE
 * @brief SARibbonBarLayout 构造函数
 * @param parent 父窗口 SARibbonBar
 * \endif
 */
SARibbonBarLayout::SARibbonBarLayout(SARibbonBar* parent) : QLayout(parent), d_ptr(new PrivateData(parent))
{
    init();
}

/**
 * \if ENGLISH
 * @brief Destructor for SARibbonBarLayout
 * \endif
 * 
 * \if CHINESE
 * @brief SARibbonBarLayout 析构函数
 * \endif
 */
SARibbonBarLayout::~SARibbonBarLayout()
{
    while (!d_ptr->items.isEmpty()) {
        QLayoutItem* item = d_ptr->items.takeFirst();
        if (item->widget()) {
            item->widget()->hide();
        }
        delete item;
    }
}

/**
 * \if ENGLISH
 * @brief Initialize the layout
 * \endif
 * 
 * \if CHINESE
 * @brief 初始化布局
 * \endif
 */
void SARibbonBarLayout::init()
{
    // 不需要初始化子控件，它们会从ribbonBar获取
}

/**
 * \if ENGLISH
 * @brief Add a layout item
 * @param item The layout item to add
 * \endif
 * 
 * \if CHINESE
 * @brief 添加布局项
 * @param item 要添加的布局项
 * \endif
 */
void SARibbonBarLayout::addItem(QLayoutItem* item)
{
    d_ptr->items.append(item);
}

/**
 * \if ENGLISH
 * @brief Get the layout item at the specified index
 * @param index The index of the item
 * @return The layout item at the index, or nullptr if not found
 * \endif
 * 
 * \if CHINESE
 * @brief 获取指定索引处的布局项
 * @param index 项的索引
 * @return 指定索引处的布局项，如果未找到则返回 nullptr
 * \endif
 */
QLayoutItem* SARibbonBarLayout::itemAt(int index) const
{
    if (index >= 0 && index < d_ptr->items.size()) {
        return d_ptr->items.at(index);
    }
    return nullptr;
}

/**
 * \if ENGLISH
 * @brief Take and remove the layout item at the specified index
 * @param index The index of the item
 * @return The layout item at the index, or nullptr if not found
 * \endif
 * 
 * \if CHINESE
 * @brief 取出并移除指定索引处的布局项
 * @param index 项的索引
 * @return 指定索引处的布局项，如果未找到则返回 nullptr
 * \endif
 */
QLayoutItem* SARibbonBarLayout::takeAt(int index)
{
    if (index < 0 || index >= d_ptr->items.count()) {
        return nullptr;
    }
    return d_ptr->items.takeAt(index);
}

/**
 * \if ENGLISH
 * @brief Get the number of layout items
 * @return The number of layout items
 * \endif
 * 
 * \if CHINESE
 * @brief 获取布局项的数量
 * @return 布局项的数量
 * \endif
 */
int SARibbonBarLayout::count() const
{
    return d_ptr->items.size();
}

/**
 * \if ENGLISH
 * @brief Get the recommended size for the layout
 * @return The recommended size
 * \endif
 * 
 * \if CHINESE
 * @brief 获取布局的推荐尺寸
 * @return 推荐尺寸
 * \endif
 */
QSize SARibbonBarLayout::sizeHint() const
{
    int height = d_ptr->minHeight;
    int width  = d_ptr->minWidth;
    if (width > d_ptr->maxMinWidth) {
        width = d_ptr->maxMinWidth;
    }
    return QSize(width, height);
}

/**
 * \if ENGLISH
 * @brief Get the minimum size for the layout
 * @return The minimum size
 * \endif
 * 
 * \if CHINESE
 * @brief 获取布局的最小尺寸
 * @return 最小尺寸
 * \endif
 */
QSize SARibbonBarLayout::minimumSize() const
{
    return sizeHint();
}

/**
 * \if ENGLISH
 * @brief Set the geometry of the layout
 * @param rect The rectangle to set
 * \endif
 * 
 * \if CHINESE
 * @brief 设置布局的几何区域
 * @param rect 要设置的矩形区域
 * \endif
 */
void SARibbonBarLayout::setGeometry(const QRect& rect)
{
    QLayout::setGeometry(rect);
    doLayout();
}

/**
 * \if ENGLISH
 * @brief Perform the layout
 * \endif
 * 
 * \if CHINESE
 * @brief 执行布局
 * \endif
 */
void SARibbonBarLayout::doLayout()
{
    if (isLooseStyle()) {
        resizeInLooseStyle();
    } else {
        resizeInCompactStyle();
    }
}

/**
 * \if ENGLISH
 * @brief Check if the current style is loose style
 * @return True if loose style, false otherwise
 * \endif
 * 
 * \if CHINESE
 * @brief 检查当前样式是否为宽松样式
 * @return 如果是宽松样式返回 true，否则返回 false
 * \endif
 */
bool SARibbonBarLayout::isLooseStyle() const
{
    return SARibbonBar::isLooseStyle(d_ptr->ribbonStyle());
}

/**
 * \if ENGLISH
 * @brief Check if the current style is compact style
 * @return True if compact style, false otherwise
 * \endif
 * 
 * \if CHINESE
 * @brief 检查当前样式是否为紧凑样式
 * @return 如果是紧凑样式返回 true，否则返回 false
 * \endif
 */
bool SARibbonBarLayout::isCompactStyle() const
{
    return SARibbonBar::isCompactStyle(d_ptr->ribbonStyle());
}

/**
 * \if ENGLISH
 * @brief Get the title rectangle
 * @return The title rectangle
 * \endif
 * 
 * \if CHINESE
 * @brief 获取标题栏矩形区域
 * @return 标题栏矩形区域
 * \endif
 */
const QRect& SARibbonBarLayout::titleRect() const
{
    return d_ptr->titleRect;
}

/**
 * \if ENGLISH
 * @brief Calculate the minimum tab bar width
 * @return The minimum tab bar width
 * @details This function calculates the minimum width for the tab bar when it is centered. The tab bar does not occupy the entire width, but uses the minimum width to fit its content, leaving space for the main window to receive mouse events for dragging and other operations.
 * \endif
 * 
 * \if CHINESE
 * @brief 计算tabbar的最小尺寸
 * @return tabbar的最小宽度
 * @details 此函数的作用是在tabbar居中布局时，tabbar不是占满整个宽度，而是按照最小尺寸来占据宽度，让tabbar的长度刚刚好，这样能让出mainwindow的空间，接受鼠标事件，从而实现拖动等操作，否则tabbar占用整个顶栏，鼠标无法点击到mainwindow
 * \endif
 */
int SARibbonBarLayout::calcMinTabBarWidth() const
{
    SARibbonTabBar* tabBar = d_ptr->ribbonTabBar();
    if (!tabBar) {
        return 0;
    }
    const QMargins& mg = tabBar->tabMargin();
    return tabBar->sizeHint().width() + (mg.left() + mg.right());
}

/**
 * \if ENGLISH
 * @brief Set the system button size
 * @param size The size to set
 * \endif
 * 
 * \if CHINESE
 * @brief 设置系统按钮尺寸
 * @param size 要设置的尺寸
 * \endif
 */
void SARibbonBarLayout::setSystemButtonSize(const QSize& size)
{
    d_ptr->systemButtonSize = size;
}

/**
 * \if ENGLISH
 * @brief Set whether the tab bar is on the title bar
 * @param on True to place tab on title, false otherwise
 * \endif
 * 
 * \if CHINESE
 * @brief 设置tab是否在标题栏上
 * @param on true 表示 tab 在标题栏上，false 表示不在
 * \endif
 */
void SARibbonBarLayout::setTabOnTitle(bool on)
{
    if (d_ptr->isTabOnTitle != on) {
        d_ptr->isTabOnTitle = on;
        d_ptr->resetSize();
    }
}

/**
 * \if ENGLISH
 * @brief Check if the tab bar is on the title bar
 * @return True if tab is on title, false otherwise
 * \endif
 * 
 * \if CHINESE
 * @brief 检查tab是否在标题栏上
 * @return 如果 tab 在标题栏上返回 true，否则返回 false
 * \endif
 */
bool SARibbonBarLayout::isTabOnTitle() const
{
    return d_ptr->isTabOnTitle;
}

/**
 * \if ENGLISH
 * @brief Get the main bar height in minimum mode
 * @return The main bar height in minimum mode
 * \endif
 * 
 * \if CHINESE
 * @brief 最小模式下的主栏高度
 * @return 最小模式下的主栏高度
 * \endif
 */
int SARibbonBarLayout::minimumModeMainBarHeight() const
{
    return d_ptr->minimumModeMainBarHeight();
}

/**
 * \if ENGLISH
 * @brief Get the main bar height in normal mode
 * @return The main bar height in normal mode
 * @note SARibbonBar::height and mainBarHeight may not be equal in RibbonState::MinimumRibbonMode
 * \endif
 * 
 * \if CHINESE
 * @brief 正常模式下的主栏高度
 * @return 正常模式下的主栏高度
 * @note 有可能SARibbonBar::height和mainBarHeight不相等，这种情况发生在RibbonState::MinimumRibbonMode状态下
 * \endif
 */
int SARibbonBarLayout::normalModeMainBarHeight() const
{
    return d_ptr->normalModeMainBarHeight();
}

/**
 * \if ENGLISH
 * @brief Get the tab bar height
 * @return The tab bar height
 * \endif
 * 
 * \if CHINESE
 * @brief 获取标签栏高度
 * @return 标签栏高度
 * \endif
 */
int SARibbonBarLayout::tabBarHeight() const
{
    return d_ptr->getActualTabBarHeight();
}

/**
 * \if ENGLISH
 * @brief Set the tab bar height
 * @param h The height to set
 * \endif
 * 
 * \if CHINESE
 * @brief 设置标签栏高度
 * @param h 要设置的高度
 * \endif
 */
void SARibbonBarLayout::setTabBarHeight(int h)
{
    d_ptr->setTabBarHeight(h);
}

/**
 * \if ENGLISH
 * @brief Get the title bar height
 * @return The title bar height
 * @sa setTitleBarHeight
 * \endif
 * 
 * \if CHINESE
 * @brief 获取标题栏高度
 * @return 标题栏高度
 * @sa setTitleBarHeight
 * \endif
 */
int SARibbonBarLayout::titleBarHeight() const
{
    return d_ptr->getActualTitleBarHeight();
}

/**
 * \if ENGLISH
 * @brief Set the title bar height
 * @param h The height to set
 * @note This operation will emit titleBarHeightChanged signal
 * @sa titleBarHeight
 * \endif
 * 
 * \if CHINESE
 * @brief 设置标题栏高度
 * @param h 要设置的高度
 * @note 此操作会发射 titleBarHeightChanged 信号
 * @sa titleBarHeight
 * \endif
 */
void SARibbonBarLayout::setTitleBarHeight(int h)
{
    d_ptr->setTitleBarHeight(h);
}

/**
 * \if ENGLISH
 * @brief Get the category height
 * @return The category height
 * \endif
 * 
 * \if CHINESE
 * @brief 获取类别高度
 * @return 类别高度
 * \endif
 */
int SARibbonBarLayout::categoryHeight() const
{
    return d_ptr->getActualCategoryHeight();
}

/**
 * \if ENGLISH
 * @brief Set the category height
 * @param h The height to set
 * \endif
 * 
 * \if CHINESE
 * @brief 设置类别高度
 * @param h 要设置的高度
 * \endif
 */
void SARibbonBarLayout::setCategoryHeight(int h)
{
    d_ptr->setCategoryHeight(h);
}

/**
 * \if ENGLISH
 * @brief Get the panel title height
 * @return The panel title height
 * \endif
 * 
 * \if CHINESE
 * @brief 获取面板标题高度
 * @return 面板标题高度
 * \endif
 */
int SARibbonBarLayout::panelTitleHeight() const
{
    return d_ptr->panelTitleHeight;
}

/**
 * \if ENGLISH
 * @brief Set the panel title height
 * @param h The height to set
 * \endif
 * 
 * \if CHINESE
 * @brief 设置面板标题高度
 * @param h 要设置的高度
 * \endif
 */
void SARibbonBarLayout::setPanelTitleHeight(int h)
{
    d_ptr->panelTitleHeight = h;
}

/**
 * \if ENGLISH
 * @brief Set the window icon
 * @param icon The icon to set
 * \endif
 * 
 * \if CHINESE
 * @brief 设置主窗口左上角的图标
 * @param icon 要设置的图标
 * \endif
 */
void SARibbonBarLayout::setWindowIcon(const QIcon& icon)
{
    titleIconWidget()->setIcon(icon);
}

/**
 * \if ENGLISH
 * @brief Get the window icon
 * @return The window icon
 * \endif
 * 
 * \if CHINESE
 * @brief 获取窗口图标
 * @return 窗口图标
 * \endif
 */
QIcon SARibbonBarLayout::windowIcon() const
{
    return titleIconWidget()->icon();
}

/**
 * \if ENGLISH
 * @brief Set whether the application button is vertically expanded
 * @param on True to expand vertically, false otherwise
 * @details When vertically expanded, the application button will occupy both the title bar and tab bar heights
 * \endif
 * 
 * \if CHINESE
 * @brief 设置ApplicationButton是否垂直方向扩充
 * @param on true 表示垂直扩充，false 表示不扩充
 * @details 垂直扩充时，ApplicationButton能占用标题栏和tab栏两个栏的高度
 * \endif
 */
void SARibbonBarLayout::setApplicationButtonVerticalExpansion(bool on)
{
    d_ptr->isApplicationButtonVerticalExpansion = on;
    if (on) {
        if (SARibbonTitleIconWidget* iconWidget = titleIconWidget()) {
            iconWidget->setVisible(!on);
        }
    }
}

/**
 * \if ENGLISH
 * @brief Check if the application button is vertically expanded
 * @return True if vertically expanded, false otherwise
 * @note Default is false
 * \endif
 * 
 * \if CHINESE
 * @brief 检查applicationButton是否在垂直方向扩充
 * @return 如果垂直扩充返回 true，否则返回 false
 * @note 默认为 false
 * \endif
 */
bool SARibbonBarLayout::isApplicationButtonVerticalExpansion() const
{
    return d_ptr->isApplicationButtonVerticalExpansion;
}

/**
 * \if ENGLISH
 * @brief Get the ribbon bar
 * @return The ribbon bar
 * \endif
 * 
 * \if CHINESE
 * @brief 获取功能区栏
 * @return 功能区栏
 * \endif
 */
SARibbonBar* SARibbonBarLayout::ribbonBar() const
{
    return d_ptr->ribbonBar;
}

/**
 * \if ENGLISH
 * @brief Get the ribbon tab bar
 * @return The ribbon tab bar
 * \endif
 * 
 * \if CHINESE
 * @brief 获取功能区标签栏
 * @return 功能区标签栏
 * \endif
 */
SARibbonTabBar* SARibbonBarLayout::ribbonTabBar() const
{
    return d_ptr->ribbonTabBar();
}

/**
 * \if ENGLISH
 * @brief Get the stacked container widget
 * @return The stacked container widget
 * \endif
 * 
 * \if CHINESE
 * @brief 获取堆叠容器窗口
 * @return 堆叠容器窗口
 * \endif
 */
SARibbonStackedWidget* SARibbonBarLayout::stackedContainerWidget() const
{
    return d_ptr->stackedContainerWidget();
}

/**
 * \if ENGLISH
 * @brief Get the quick access bar
 * @return The quick access bar
 * \endif
 * 
 * \if CHINESE
 * @brief 获取快速访问栏
 * @return 快速访问栏
 * \endif
 */
SARibbonQuickAccessBar* SARibbonBarLayout::quickAccessBar() const
{
    return d_ptr->quickAccessBar();
}

/**
 * \if ENGLISH
 * @brief Get the right button group
 * @return The right button group
 * \endif
 * 
 * \if CHINESE
 * @brief 获取右侧按钮组
 * @return 右侧按钮组
 * \endif
 */
SARibbonButtonGroupWidget* SARibbonBarLayout::rightButtonGroup() const
{
    return d_ptr->rightButtonGroup();
}

/**
 * \if ENGLISH
 * @brief Get the application button
 * @return The application button
 * \endif
 * 
 * \if CHINESE
 * @brief 获取应用程序按钮
 * @return 应用程序按钮
 * \endif
 */
QAbstractButton* SARibbonBarLayout::applicationButton() const
{
    return d_ptr->applicationButton();
}

/**
 * \if ENGLISH
 * @brief Get the title icon widget
 * @return The title icon widget
 * \endif
 * 
 * \if CHINESE
 * @brief 获取标题图标窗口
 * @return 标题图标窗口
 * \endif
 */
SARibbonTitleIconWidget* SARibbonBarLayout::titleIconWidget() const
{
    return d_ptr->titleIconWidget();
}

/**
 * \if ENGLISH
 * @brief Layout the title rectangle
 * \endif
 * 
 * \if CHINESE
 * @brief 布局标题栏矩形区域
 * \endif
 */
void SARibbonBarLayout::layoutTitleRect()
{
    SARibbonBar* ribbon                    = d_ptr->ribbonBar;
    SARibbonQuickAccessBar* quickAccessBar = d_ptr->quickAccessBar();
    SARibbonTabBar* ribbonTabBar           = d_ptr->ribbonTabBar();
    const QMargins border                  = d_ptr->contentsMargins();
    const int validTitleBarHeight          = d_ptr->getActualTitleBarHeight();

    // 计算标题栏区域
    if (SA::saIsRTL()) {
        /// RTL mode: title rect positions are mirrored
        if (isCompactStyle()) {
            // 紧凑模式 RTL: title bar in the remaining space left of tabbar
            // In RTL, titleEnd is the left edge of tabbar (mirrored from right)
            int titleEnd   = ribbonTabBar->geometry().left();
            int titleWidth = quickAccessBar ? (titleEnd - quickAccessBar->geometry().right())
                                            : (titleEnd - border.left());
            if (titleWidth > 10) {
                d_ptr->titleRect = QRect(titleEnd - titleWidth, border.top(), titleWidth, validTitleBarHeight);
            } else {
                d_ptr->titleRect = QRect();
            }
        } else {
            // 三行宽松模式 RTL
            const int tabX = ribbonTabBar->geometry().x();
            // In RTL, contextRegionLeft becomes the leftmost (visual rightmost) tab boundary
            // and contextRegionRight becomes the rightmost (visual leftmost) tab boundary
            // We swap the variable semantics to reflect the mirrored geometry
            int contextRegionLeft  = -1;   // In RTL: the left boundary of context region (visual rightmost)
            int contextRegionRight = ribbon->width();  // In RTL: the right boundary of context region (visual leftmost)

            QList< int > visibleContextIndex = ribbon->currentVisibleContextCategoryTabIndexs();
            if (!visibleContextIndex.empty()) {
                // In RTL, first context tab rect's left() is visually the rightmost edge
                int edgeVal = ribbonTabBar->tabRect(visibleContextIndex.first()).left() + tabX;
                if (edgeVal > contextRegionLeft) {
                    contextRegionLeft = edgeVal;
                }
                // In RTL, last context tab rect's right() is visually the leftmost edge
                edgeVal = d_ptr->ribbonTabBar()->tabRect(visibleContextIndex.last()).right() + tabX;
                if (edgeVal < contextRegionRight) {
                    contextRegionRight = edgeVal;
                }
            }

            // In RTL, x1 is the right side of the quickAccessBar (which is now on the right)
            int x1 = border.left();
            if (quickAccessBar) {
                // In RTL, quickAccessBar is at right side, so title area starts from its left edge
                x1 = quickAccessBar->geometry().x();
            }
            // In RTL, x2 is the left side (system buttons are at left)
            int x2 = border.left() + d_ptr->systemButtonSize.width();

            if (contextRegionLeft < 0) {
                // No context category: title between system buttons (left) and quickAccessBar (right)
                d_ptr->titleRect = QRect(QPoint(x2, border.top()), QPoint(x1, validTitleBarHeight + border.top()));
            } else {
                int leftwidth  = contextRegionRight - x2;
                int rightwidth = x1 - contextRegionLeft;
                if (leftwidth > rightwidth) {
                    d_ptr->titleRect =
                        QRect(QPoint(x2, border.top()), QPoint(contextRegionRight, validTitleBarHeight + border.top()));
                } else {
                    d_ptr->titleRect =
                        QRect(QPoint(contextRegionLeft, border.top()), QPoint(x1, validTitleBarHeight + border.top()));
                }
            }
        }
    } else {
        /// LTR mode: original title rect logic unchanged
        if (isCompactStyle()) {
            // 紧凑模式,紧凑模式的标题栏在tabbar的剩余空间中
            int titleStart = ribbonTabBar->geometry().right();
            int titleWidth = quickAccessBar ? (quickAccessBar->x() - titleStart)
                                            : (ribbon->width() - titleStart - d_ptr->systemButtonSize.width());
            if (titleWidth > 10) {
                d_ptr->titleRect = QRect(titleStart, border.top(), titleWidth, validTitleBarHeight);
            } else {
                // 标题栏过小，就不显示
                d_ptr->titleRect = QRect();
            }
        } else {
            const int tabX = ribbonTabBar->geometry().x();
            // 三行宽松模式
            int contextRegionLeft  = ribbon->width();
            int contextRegionRight = -1;

            // 使用上下文标签的视觉数据
            // 上下文标签会占用宽松模式下的标题栏位置，因此，要计算此时标题栏应该在哪里显示
            QList< int > visibleContextIndex = ribbon->currentVisibleContextCategoryTabIndexs();
            if (!visibleContextIndex.empty()) {
                int edgeVal = ribbonTabBar->tabRect(visibleContextIndex.first()).left() + tabX;
                if (edgeVal < contextRegionLeft) {
                    contextRegionLeft = edgeVal;
                }
                edgeVal = d_ptr->ribbonTabBar()->tabRect(visibleContextIndex.last()).right() + tabX;
                if (edgeVal > contextRegionRight) {
                    contextRegionRight = edgeVal;
                }
            }

            int x1 = border.left();
            if (quickAccessBar) {
                x1 = quickAccessBar->geometry().right() + 1;
            }
            int x2 = ribbon->width() - d_ptr->systemButtonSize.width() - border.right();

            if (contextRegionRight < 0) {
                // 说明没有上下文标签，那么标题直接放在quickAccessBar到systembar之间
                d_ptr->titleRect = QRect(QPoint(x1, border.top()), QPoint(x2, validTitleBarHeight + border.top()));
            } else {
                int leftwidth  = contextRegionLeft - x1;
                int rightwidth = x2 - contextRegionRight;
                if (rightwidth > leftwidth) {
                    d_ptr->titleRect =
                        QRect(QPoint(contextRegionRight, border.top()), QPoint(x2, validTitleBarHeight + border.top()));
                } else {
                    d_ptr->titleRect =
                        QRect(QPoint(x1, border.top()), QPoint(contextRegionLeft, validTitleBarHeight + border.top()));
                }
            }
        }
    }
}

/**
 * \if ENGLISH
 * @brief Reset the size
 * \endif
 * 
 * \if CHINESE
 * @brief 重置尺寸
 * \endif
 */
void SARibbonBarLayout::resetSize()
{
    d_ptr->resetSize();
}

/**
 * \if ENGLISH
 * @brief Layout the stacked container widget
 * \endif
 * 
 * \if CHINESE
 * @brief 布局堆叠容器窗口
 * \endif
 */
void SARibbonBarLayout::layoutStackedContainerWidget()
{
    SARibbonBar* ribbon                  = ribbonBar();
    SARibbonStackedWidget* stackedWidget = stackedContainerWidget();
    SARibbonTabBar* tabBar               = ribbonTabBar();
    if (!ribbon || !stackedWidget || !tabBar) {
        return;
    }
    QMargins border            = d_ptr->contentsMargins();
    QRect ribbonTabBarGeometry = tabBar->geometry();

    int x = border.left();
    int y = ribbonTabBarGeometry.bottom() + 1;
    int w = ribbon->width() - border.left() - border.right();
    int h = d_ptr->getActualCategoryHeight();

    if (stackedWidget->isPopupMode()) {
        QPoint absPosition = ribbon->mapToGlobal(QPoint(x, y));
        x                  = absPosition.x();
        y                  = absPosition.y();
    }
    // 受布局影响，这里不能使用stackedWidget->setGeometry(x, y, w, h);
    // stackedWidget->setGeometry(x, y, w, h);
    //    stackedWidget->move(x, y);
    //    stackedWidget->setFixedSize(QSize(w, h));
    stackedWidget->setNormalGeometry(QRect(x, y, w, h));
}

/**
 * \if ENGLISH
 * @brief Layout the category
 * @details This function is called when adjusting the alignment of the category. Since the size of StackedContainerWidget does not change when the alignment changes, but the category needs to be re-layouted, a request needs to be emitted.
 * \endif
 * 
 * \if CHINESE
 * @brief 让category重新布局
 * @details 这个函数在调整category的对其方式的时候调用，由于对其方式改变StackedContainerWidget的尺寸没有改变，但category要重新布局,因此需要发射一个
 * \endif
 */
void SARibbonBarLayout::layoutCategory()
{
    if (SARibbonStackedWidget* stackedWidget = stackedContainerWidget()) {
        stackedWidget->layoutRequestInnerWidgets();
    }
}

/**
 * \if ENGLISH
 * @brief Resize in loose style
 * \endif
 * 
 * \if CHINESE
 * @brief 宽松模式下调整尺寸
 * \endif
 */
void SARibbonBarLayout::resizeInLooseStyle()
{

    QMargins border = d_ptr->contentsMargins();
    int x           = border.left();
    int y           = border.top();

    const int validTitleBarHeight = d_ptr->getActualTitleBarHeight();
    const int titleBarControlHeight = validTitleBarHeight - 2;  // 标题栏上的控件高度是标题栏高度-2，上下各减1px
    const int tabH                = d_ptr->getActualTabBarHeight();
    const int tabBarControlHeight = tabH;  // tabbar上面的控件高度是tabbar高度-2，上下各减1px
    const bool isAppBtnVExpan     = d_ptr->isApplicationButtonVerticalExpansion;
    int barMinWidth               = 0;  ///< 记录ribbonBar的最小宽度，这个用于给推荐宽度

    SARibbonBar* ribbon                      = ribbonBar();
    SARibbonTabBar* tabBar                   = ribbonTabBar();
    QAbstractButton* appBtn                  = applicationButton();
    SARibbonQuickAccessBar* qb               = quickAccessBar();
    SARibbonTitleIconWidget* titleIcon       = titleIconWidget();
    SARibbonButtonGroupWidget* rightBtnGroup = rightButtonGroup();

    if (SA::saIsRTL()) {
        /// RTL mode: widgets are mirrored — start from right edge, retreat leftward
        x = ribbon->width() - border.right();

        /// 1. 布局corner widget (TopLeftCorner becomes right side in RTL)
        if (QWidget* connerL = ribbon->cornerWidget(Qt::TopLeftCorner)) {
            if (connerL->isVisibleTo(ribbon)) {
                QSize connerSize = connerL->sizeHint();
                connerSize       = SA::scaleSizeByHeight(connerSize, validTitleBarHeight);
                x -= connerSize.width();
                connerL->setGeometry(x, y, connerSize.width(), connerSize.height());
                x -= 5;  // spacing
            }
        }

        /// 2. 布局图标窗口或app button (mirrored to right edge)
        if (isAppBtnVExpan) {
            //! 如果Application button是纵向扩展模式，那么不显示titleicon
            //! 纵向扩展的Application button将占用title和tab的高度
            if (appBtn && appBtn->isVisibleTo(ribbon)) {
                QSize appBtnSize = appBtn->sizeHint();
                int appHeight    = titleBarControlHeight + tabH;
                appHeight -= 2;
                appBtnSize = SA::scaleSizeByHeight(appBtnSize, appHeight, 1.5);
                x -= appBtnSize.width();
                appBtn->setGeometry(x, y + 1, appBtnSize.width(), appBtnSize.height());
            }
        } else {
            //! Application button不是纵向扩展，显示icon
            if (titleIcon && titleIcon->isVisibleTo(ribbon)) {
                QSize titleiconSizeHint = titleIcon->sizeHint();
                titleiconSizeHint.scale(titleBarControlHeight, titleBarControlHeight, Qt::KeepAspectRatio);
                x -= titleiconSizeHint.width();
                titleIcon->setGeometry(x, y + 1, titleiconSizeHint.width(), titleiconSizeHint.height());
            }
        }

        /// 3. 布局quick access bar (positioned at right side in RTL)
        if (qb && qb->isVisibleTo(ribbon)) {
            QSize quickAccessBarSize = qb->sizeHint();
            x -= quickAccessBarSize.width();
            qb->setGeometry(x, y + 1, quickAccessBarSize.width(), titleBarControlHeight);
        }

        /// titleBar上的元素布局完成，开始布局第二行 — start from right edge
        x = ribbon->width() - border.right();
        y += validTitleBarHeight;

        /// 4. 布局 applicationButton（第二行，mirrored to right edge）
        if (appBtn && appBtn->isVisibleTo(ribbon)) {
            if (isAppBtnVExpan) {
                ///! 如果是纵向扩展的Application button则只更新x位置，在第2步已经设定了位置
                x = appBtn->geometry().left() - 1;  // retreat past the appBtn left edge
            } else {
                QSize appBtnSize = appBtn->sizeHint();
                appBtnSize       = SA::scaleSizeByHeight(appBtnSize, tabBarControlHeight, 1.5);
                x -= appBtnSize.width();
                appBtn->setGeometry(x, y + 1, appBtnSize.width(), appBtnSize.height());
                barMinWidth += appBtnSize.width();
            }
        }

        /// 5. 布局corner widget (TopRightCorner becomes left side in RTL)
        /// 由于这个窗口一定要在最左（RTL镜像后），因此先对这个窗口进行布局
        int endX = border.left();

        if (QWidget* connerTR = ribbon->cornerWidget(Qt::TopRightCorner)) {
            if (connerTR->isVisibleTo(ribbon)) {
                QSize connerSize = connerTR->sizeHint();
                connerSize       = SA::scaleSizeByHeight(connerSize, tabBarControlHeight);
                connerTR->setGeometry(endX, y + 1, connerSize.width(), connerSize.height());
                endX += connerSize.width();
                barMinWidth += connerSize.width();
            }
        }

        /// 6. rightButtonGroup positioned at left side in RTL (swapped with QuickAccessBar)
        if (rightBtnGroup && rightBtnGroup->isVisibleTo(ribbon)) {
            QSize rightBtnGroupSize = rightBtnGroup->sizeHint();
            endX += 1;  // 先偏移1px
            d_ptr->rightButtonGroup()->setGeometry(endX, y + 1, rightBtnGroupSize.width(), tabBarControlHeight);
            endX += rightBtnGroupSize.width();
            barMinWidth += rightBtnGroupSize.width();
        }

        /// 7. 布局tabbar — remaining area between endX(left) and x(right)
        int tabBarWidth = x - endX;
        if (tabBarWidth < 10) {
            tabBarWidth = 10;
        }

        int mintabBarWidth = calcMinTabBarWidth();
        barMinWidth += mintabBarWidth;
        if (tabBar) {
            if (ribbon->ribbonAlignment() == SARibbonAlignment::AlignLeft) {
                // AlignLeft in RTL means tabs start from the right edge (visual right)
                tabBar->setGeometry(endX, y, tabBarWidth, tabH);
            } else if (ribbon->ribbonAlignment() == SARibbonAlignment::AlignRight) {
                // AlignRight in RTL means tabs start from the left edge (visual left)
                if (mintabBarWidth < tabBarWidth) {
                    tabBarWidth = mintabBarWidth;
                }
                tabBar->setGeometry(x - tabBarWidth, y, tabBarWidth, tabH);
            } else {
                // AlignCenter: center the tab bar in RTL
                if (mintabBarWidth >= tabBarWidth) {
                    tabBar->setGeometry(endX, y, tabBarWidth, tabH);
                } else {
                    int xoffset = (tabBarWidth - mintabBarWidth) / 2;
                    tabBar->setGeometry(endX + xoffset, y, mintabBarWidth, tabH);
                }
            }
        }

        d_ptr->minWidth = barMinWidth;
        layoutTitleRect();
        layoutStackedContainerWidget();
    } else {
        /// LTR mode: original layout logic unchanged

        /// 1. 布局corner widget
        if (QWidget* connerL = ribbon->cornerWidget(Qt::TopLeftCorner)) {
            if (connerL->isVisibleTo(ribbon)) {
                QSize connerSize = connerL->sizeHint();
                connerSize       = SA::scaleSizeByHeight(connerSize, validTitleBarHeight);
                connerL->setGeometry(x, y, connerSize.width(), connerSize.height());
                x = connerL->geometry().right() + 5;
            }
        }

        /// 2. 布局图标窗口或app button
        if (isAppBtnVExpan) {
            //! 如果Application button是纵向扩展模式，那么不显示titleicon
            //! 纵向扩展的Application button将占用title和tab的高度
            if (appBtn && appBtn->isVisibleTo(ribbon)) {
                QSize appBtnSize = appBtn->sizeHint();
                // 纵向扩展模式，appBtn的高度撑满titleBarControlHeight+tabH
                int appHeight = titleBarControlHeight + tabH;
                appHeight -= 2;  // 上下留1px
                appBtnSize = SA::scaleSizeByHeight(appBtnSize, appHeight, 1.5);
                appBtn->setGeometry(x, y + 1, appBtnSize.width(), appBtnSize.height());
                x = appBtn->geometry().right();
            }
        } else {
            //! Application button不是纵向扩展，显示icon
            if (titleIcon && titleIcon->isVisibleTo(ribbon)) {
                QSize titleiconSizeHint = titleIcon->sizeHint();
                titleiconSizeHint.scale(titleBarControlHeight, titleBarControlHeight, Qt::KeepAspectRatio);
                titleIcon->setGeometry(x, y + 1, titleiconSizeHint.width(), titleiconSizeHint.height());
                x += titleiconSizeHint.width();
            }
        }

        /// 3. 布局quick access bar
        if (qb && qb->isVisibleTo(ribbon)) {
            QSize quickAccessBarSize = qb->sizeHint();
            //! 这里不用SA::scaleSizeByHeight缩减quickAccessBar的比例：
            //! quickAccessBarSize = SA::scaleSizeByHeight(quickAccessBarSize, titleBarControlHeight);
            //! 原因是，如果quickAccessBar最后是一个widget，如果长度不足，这个widget是不会显示出来，假如默认QToolBar的高度是32，
            //! 而titlebar的高度一是28，肯定会导致宽度缩减，如果宽度缩减，且最后是一个窗口，那么这个窗口会不显示
            //! 针对工具栏类型的窗口，宽度高度都不进行比例缩减
            // 上下留1px的边线
            qb->setGeometry(x, y + 1, quickAccessBarSize.width(), titleBarControlHeight);
        }

        /// titleBar上的元素布局完成，开始布局第二行
        x = border.left();
        y += validTitleBarHeight;  // 此时，y值在titlebar下面

        /// 4. 布局 applicationButton（第二行）
        if (appBtn && appBtn->isVisibleTo(ribbon)) {
            if (isAppBtnVExpan) {
                ///! 如果是纵向扩展的Application button则只更新x位置，在第2步已经设定了位置
                x = appBtn->geometry().right();
            } else {
                QSize appBtnSize = appBtn->sizeHint();
                // appBtnSize的sizehint是根据文字宽度来推荐，如果按高度来扩展，会显得有点大，直接设置高度，又显得有点小
                // 因此宽高不按1:1比例扩展，按1:1.5比例扩展，也就是高度扩展3倍，宽度扩展3/1.5倍
                // 目前看这个比例相对比较协调
                appBtnSize = SA::scaleSizeByHeight(appBtnSize, tabBarControlHeight, 1.5);
                appBtn->setGeometry(x, y + 1, appBtnSize.width(), appBtnSize.height());
                x = appBtn->geometry().right();
                // 累加到最小宽度中
                barMinWidth += appBtnSize.width();
            }
        }

        /// 5. 布局右上角corner widget
        /// cornerWidget(Qt::TopRightCorner)是一定要配置的，对于多文档窗口，子窗口的缩放恢复按钮就是通过这个窗口实现，
        /// 由于这个窗口一定要在最右，因此先对这个窗口进行布局
        int endX = ribbon->width() - border.right();

        if (QWidget* connerTR = ribbon->cornerWidget(Qt::TopRightCorner)) {
            if (connerTR->isVisibleTo(ribbon)) {
                QSize connerSize = connerTR->sizeHint();
                connerSize       = SA::scaleSizeByHeight(connerSize, tabBarControlHeight);
                endX -= connerSize.width();
                connerTR->setGeometry(endX, y + 1, connerSize.width(), connerSize.height());
                // 累加到最小宽度中
                barMinWidth += connerSize.width();
            }
        }

        /// 6. tabBar右边的附加按钮组rightButtonGroup，这里一般会附加一些类似登录等按钮组
        if (rightBtnGroup && rightBtnGroup->isVisibleTo(ribbon)) {
            QSize rightBtnGroupSize = rightBtnGroup->sizeHint();
            //! 这里不用SA::scaleSizeByHeight缩减rightButtonGroup的比例：
            //! rightBtnGroupSize       = SA::scaleSizeByHeight(rightBtnGroupSize, tabBarControlHeight);
            //! 针对工具栏类型的窗口，宽度高度都不进行比例缩减
            endX -= 1;  // 先偏移1px
            endX -= rightBtnGroupSize.width();
            d_ptr->rightButtonGroup()->setGeometry(endX, y + 1, rightBtnGroupSize.width(), tabBarControlHeight);
            // 累加到最小宽度中
            barMinWidth += rightBtnGroupSize.width();
        }

        /// 7. 布局tabbar，此时已经确定了applicationButton的位置，以及最右边rightButtonGroup的位置，剩下的区域都给tabbar
        int tabBarWidth = endX - x;
        // 这里极度压缩有可能导致负数因此不能小于0
        if (tabBarWidth < 10) {
            tabBarWidth = 10;
        }

        int mintabBarWidth = calcMinTabBarWidth();
        // 累加到最小宽度中
        barMinWidth += mintabBarWidth;
        if (tabBar) {
            if (ribbon->ribbonAlignment() == SARibbonAlignment::AlignLeft) {
                // 左对齐的tabbar，直接设置位置
                tabBar->setGeometry(x, y, tabBarWidth, tabH);
            } else if (ribbon->ribbonAlignment() == SARibbonAlignment::AlignRight) {
                // 右对齐的情况下，Tab要靠右显示
                if (mintabBarWidth >= tabBarWidth) {
                    // 这时tabbar没有右对齐的必要性，空间位置不够了
                    tabBar->setGeometry(x, y, tabBarWidth, tabH);
                } else {
                    // 说明tabbar的宽度有右对齐的可能性
                    tabBar->setGeometry(endX - mintabBarWidth, y, mintabBarWidth, tabH);
                }
            } else {
                // 居中对齐的情况下，Tab要居中显示
                // 得到tab的推荐尺寸
                if (mintabBarWidth >= tabBarWidth) {
                    // 这时tabbar没有居中对齐的必要性，空间位置不够了
                    tabBar->setGeometry(x, y, tabBarWidth, tabH);
                } else {
                    // 说明tabbar的宽度有居中的可能性
                    int xoffset = (tabBarWidth - mintabBarWidth) / 2;
                    tabBar->setGeometry(x + xoffset, y, mintabBarWidth, tabH);
                }
            }
        }

        d_ptr->minWidth = barMinWidth;

        // 5. 更新标题区域
        layoutTitleRect();

        // 6. 调整 stackedContainerWidget
        layoutStackedContainerWidget();
    }
}

/**
 * \if ENGLISH
 * @brief Resize in compact style
 * \endif
 * 
 * \if CHINESE
 * @brief 紧凑模式下调整尺寸
 * \endif
 */
void SARibbonBarLayout::resizeInCompactStyle()
{
    QMargins border               = d_ptr->contentsMargins();
    int x                         = border.left();
    int y                         = border.top();
    int barMinWidth               = 0;  ///< 记录ribbonBar的最小宽度，这个用于给推荐宽度
    const int validTitleBarHeight = d_ptr->getActualTitleBarHeight();
    const int titleBarControlHeight = validTitleBarHeight - 2;  // 标题栏上的控件高度是标题栏高度-2，上下各减1px

    SARibbonBar* ribbon                      = ribbonBar();
    SARibbonTabBar* tabBar                   = ribbonTabBar();
    QAbstractButton* appBtn                  = applicationButton();
    SARibbonQuickAccessBar* qb               = quickAccessBar();
    SARibbonTitleIconWidget* titleIcon       = titleIconWidget();
    SARibbonButtonGroupWidget* rightBtnGroup = rightButtonGroup();

    if (SA::saIsRTL()) {
        /// RTL mode in compact style: start from right edge, retreat leftward
        x = ribbon->width() - border.right();

        /// 1. 布局corner widget - TopLeftCorner becomes right side in RTL
        if (QWidget* connerL = ribbon->cornerWidget(Qt::TopLeftCorner)) {
            if (connerL->isVisibleTo(ribbon)) {
                QSize connerSize = connerL->sizeHint();
                connerSize       = SA::scaleSizeByHeight(connerSize, validTitleBarHeight);
                x -= connerSize.width();
                connerL->setGeometry(x, y, connerSize.width(), connerSize.height());
                barMinWidth += connerSize.width();
            }
        }

        /// 2. 布局图标窗口 (mirrored to right edge)
        if (titleIcon && titleIcon->isVisibleTo(ribbon)) {
            QSize titleiconSizeHint = titleIcon->sizeHint();
            titleiconSizeHint.scale(titleBarControlHeight, titleBarControlHeight, Qt::KeepAspectRatio);
            x -= titleiconSizeHint.width();
            titleIcon->setGeometry(x, y + 1, titleiconSizeHint.width(), titleiconSizeHint.height());
            barMinWidth += titleiconSizeHint.width();
        }

        /// 3. 布局applicationButton (mirrored to right edge)
        if (appBtn && appBtn->isVisibleTo(ribbon)) {
            QSize appBtnSize = appBtn->sizeHint();
            appBtnSize       = SA::scaleSizeByHeight(appBtnSize, titleBarControlHeight, 1.5);
            x -= appBtnSize.width();
            appBtn->setGeometry(x, y + 1, appBtnSize.width(), appBtnSize.height());
            barMinWidth += appBtnSize.width();
        }

        /// Right-side elements now advance from left edge (system buttons, corners, rightBtnGroup, quickAccessBar)
        int endX = border.left();
        if (ribbon->isUseRibbonFrame()) {
            // System buttons at left edge in RTL
            endX += d_ptr->systemButtonSize.width();
        }

        /// 4. 布局corner widget - TopRightCorner becomes left side in RTL
        if (QWidget* connerTR = ribbon->cornerWidget(Qt::TopRightCorner)) {
            if (connerTR->isVisibleTo(ribbon)) {
                QSize connerSize = connerTR->sizeHint();
                connerSize       = SA::scaleSizeByHeight(connerSize, validTitleBarHeight);
                connerTR->setGeometry(endX, y, connerSize.width(), connerSize.height());
                endX += connerSize.width();
                barMinWidth += connerSize.width();
            }
        }

        /// 5. rightButtonGroup positioned at left side in RTL (swapped with QuickAccessBar)
        if (rightBtnGroup && rightBtnGroup->isVisibleTo(ribbon)) {
            QSize rightBtnGroupSize = rightBtnGroup->sizeHint();
            d_ptr->rightButtonGroup()->setGeometry(endX, y + 1, rightBtnGroupSize.width(), titleBarControlHeight);
            endX += rightBtnGroupSize.width();
            barMinWidth += rightBtnGroupSize.width();
        }

        /// 6. quickAccessBar positioned at left side in RTL (swapped with rightButtonGroup)
        if (qb && qb->isVisibleTo(ribbon)) {
            QSize quickAccessBarSize = qb->sizeHint();
            qb->setGeometry(endX, y + 1, quickAccessBarSize.width(), titleBarControlHeight);
            endX += quickAccessBarSize.width();
            barMinWidth += quickAccessBarSize.width();
        }

        /// 7. 布局 tab bar — remaining area between endX (left) and x (right)
        int tabH = d_ptr->getActualTabBarHeight();
        if (tabH > titleBarControlHeight) {
            tabH = titleBarControlHeight;
        }
        int tabBarWidth = x - endX;
        if (tabBarWidth < 10) {
            tabBarWidth = 10;
        }
        int mintabBarWidth = calcMinTabBarWidth();
        barMinWidth += mintabBarWidth;
        if (tabBar) {
            y = border.top() + (titleBarControlHeight - tabH);
            if (ribbon->ribbonAlignment() == SARibbonAlignment::AlignLeft) {
                // AlignLeft in RTL means tabs start from the right edge (visual right)
                if (mintabBarWidth < tabBarWidth) {
                    tabBarWidth = mintabBarWidth;
                }
                tabBar->setGeometry(x - tabBarWidth, y, tabBarWidth, tabH);
            } else if (ribbon->ribbonAlignment() == SARibbonAlignment::AlignRight) {
                // AlignRight in RTL means tabs start from the left edge (visual left)
                if (mintabBarWidth < tabBarWidth) {
                    tabBarWidth = mintabBarWidth;
                }
                tabBar->setGeometry(endX, y, tabBarWidth, tabH);
            } else {
                // AlignCenter: center the tab bar in RTL
                if (mintabBarWidth >= tabBarWidth) {
                    tabBar->setGeometry(endX, y, tabBarWidth, tabH);
                } else {
                    int xoffset = (tabBarWidth - mintabBarWidth) / 2;
                    tabBar->setGeometry(endX + xoffset, y, mintabBarWidth, tabH);
                }
            }
        }

        d_ptr->minWidth = barMinWidth;
        layoutTitleRect();
        layoutStackedContainerWidget();
    } else {
        /// LTR mode: original compact style layout logic unchanged

        /// 1.  布局corner widget - TopLeftCorner
        if (QWidget* connerL = ribbon->cornerWidget(Qt::TopRightCorner)) {
            if (connerL->isVisibleTo(ribbon)) {
                QSize connerSize = connerL->sizeHint();
                connerSize       = SA::scaleSizeByHeight(connerSize, validTitleBarHeight);
                connerL->setGeometry(x, y, connerSize.width(), connerSize.height());
                x = connerL->geometry().right();
                // 累加到最小宽度中
                barMinWidth += connerSize.width();
            }
        }

        /// 2. 布局图标窗口
        if (titleIcon && titleIcon->isVisibleTo(ribbon)) {
            QSize titleiconSizeHint = titleIcon->sizeHint();
            titleiconSizeHint.scale(titleBarControlHeight, titleBarControlHeight, Qt::KeepAspectRatio);
            titleIcon->setGeometry(x, y + 1, titleiconSizeHint.width(), titleiconSizeHint.height());
            x = titleIcon->geometry().right();
            barMinWidth += titleiconSizeHint.width();
        }

        /// 3. 布局applicationButton
        if (appBtn && appBtn->isVisibleTo(ribbon)) {
            QSize appBtnSize = appBtn->sizeHint();
            appBtnSize       = SA::scaleSizeByHeight(appBtnSize, titleBarControlHeight, 1.5);
            appBtn->setGeometry(x, y + 1, appBtnSize.width(), appBtnSize.height());
            x = appBtn->geometry().right();
            // 累加到最小宽度中
            barMinWidth += appBtnSize.width();
        }

        int endX = ribbon->width() - border.right();
        if (ribbon->isUseRibbonFrame()) {
            endX -= d_ptr->systemButtonSize.width();
        }

        /// 4. 布局右上角corner widget
        if (QWidget* connerTR = ribbon->cornerWidget(Qt::TopRightCorner)) {
            if (connerTR->isVisibleTo(ribbon)) {
                QSize connerSize = connerTR->sizeHint();
                connerSize       = SA::scaleSizeByHeight(connerSize, validTitleBarHeight);
                endX -= connerSize.width();
                connerTR->setGeometry(endX, y, connerSize.width(), connerSize.height());
                // 累加到最小宽度中
                barMinWidth += connerSize.width();
            }
        }

        /// 5. 布局右边按钮组rightButtonGroup
        if (rightBtnGroup && rightBtnGroup->isVisibleTo(ribbon)) {
            QSize rightBtnGroupSize = rightBtnGroup->sizeHint();
            // rightBtnGroupSize       = SA::scaleSizeByHeight(rightBtnGroupSize, titleBarControlHeight);
            endX -= rightBtnGroupSize.width();
            d_ptr->rightButtonGroup()->setGeometry(endX, y + 1, rightBtnGroupSize.width(), titleBarControlHeight);
            // 累加到最小宽度中
            barMinWidth += rightBtnGroupSize.width();
        }

        /// 6. 布局quick access bar
        if (qb && qb->isVisibleTo(ribbon)) {
            QSize quickAccessBarSize = qb->sizeHint();
            // quickAccessBarSize       = SA::scaleSizeByHeight(quickAccessBarSize, titleBarControlHeight);
            endX -= quickAccessBarSize.width();
            qb->setGeometry(endX, y + 1, quickAccessBarSize.width(), titleBarControlHeight);
            // 累加到最小宽度中
            barMinWidth += quickAccessBarSize.width();
        }

        /// 7. 布局 tab bar
        int tabH = d_ptr->getActualTabBarHeight();
        if (tabH > titleBarControlHeight) {
            // 这种直接把tabH设置为validTitleBarHeight
            tabH = titleBarControlHeight;
        }
        int tabBarWidth = endX - x;
        // 这里极度压缩有可能导致负数因此不能小于0
        if (tabBarWidth < 10) {
            tabBarWidth = 10;
        }
        // 紧凑模式下，tabbar要用最小宽度布局，否则会遮挡标题栏的点击，导致无法移动
        int mintabBarWidth = calcMinTabBarWidth();
        // 累加到最小宽度中
        barMinWidth += mintabBarWidth;
        if (tabBar) {
            y = border.top() + (titleBarControlHeight - tabH);
            if (ribbon->ribbonAlignment() == SARibbonAlignment::AlignLeft) {
                // 左对齐的tabbar，直接设置位置
                if (mintabBarWidth < tabBarWidth) {
                    tabBarWidth = mintabBarWidth;
                }
                tabBar->setGeometry(x, y, tabBarWidth, tabH);
            } else {
                // 居中对齐的情况下，Tab要居中显示
                // 得到tab的推荐尺寸
                if (mintabBarWidth >= tabBarWidth) {
                    // 这时tabbar没有居中对齐的必要性，空间位置不够了
                    tabBar->setGeometry(x, y, tabBarWidth, tabH);
                } else {
                    // 说明tabbar的宽度有居中的可能性
                    int xoffset = (tabBarWidth - mintabBarWidth) / 2;
                    tabBar->setGeometry(x + xoffset, y, mintabBarWidth, tabH);
                }
            }
        }

        d_ptr->minWidth = barMinWidth;

        // 5. 更新标题区域
        layoutTitleRect();
        // 6. 调整 stackedContainerWidget
        layoutStackedContainerWidget();
    }
}
