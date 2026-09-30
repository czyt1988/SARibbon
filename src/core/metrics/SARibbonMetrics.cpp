#include <SARibbonCore/SARibbonMetrics.h>

namespace SARibbon
{
namespace Core
{

SARibbonMetrics::SARibbonMetrics() : mFontMetrics(QFont())
{
}

SARibbonMetrics::SARibbonMetrics(const QFontMetrics& fm, qreal devicePixelRatio)
    : mFontMetrics(fm), mDevicePixelRatio(devicePixelRatio)
{
}

void SARibbonMetrics::setFontMetrics(const QFontMetrics& fm)
{
    mFontMetrics = fm;
}

void SARibbonMetrics::setDevicePixelRatio(qreal dpr)
{
    mDevicePixelRatio = dpr;
}

void SARibbonMetrics::setStylePixelMetrics(int pmTabBarBaseHeight, int pmTabBarTabHSpace, int pmTabBarTabOverlap, int pmTitleBarHeight)
{
    mPmTabBarBaseHeight = pmTabBarBaseHeight;
    mPmTabBarTabHSpace  = pmTabBarTabHSpace;
    mPmTabBarTabOverlap = pmTabBarTabOverlap;
    mPmTitleBarHeight   = pmTitleBarHeight;
}

int SARibbonMetrics::systemTabBarHeight() const
{
    return mPmTabBarBaseHeight + mPmTabBarTabHSpace + mPmTabBarTabOverlap;
}

/**
 * @brief 估算标签栏的高度（自 SARibbonBarLayout.cpp 纯 move）
 * @return 计算出的标签栏高度
 */
int SARibbonMetrics::calcDefaultTabBarHeight() const
{
    int defaultHeight = systemTabBarHeight();
    int fontHeight = mFontMetrics.lineSpacing();  // Use lineSpacing instead of height for better font compatibility
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
 * @brief 估算标题栏的高度（自 SARibbonBarLayout.cpp 纯 move）
 * @return 计算出的标题栏高度
 */
int SARibbonMetrics::calcDefaultTitleBarHeight() const
{
    int defaultHeight  = mPmTitleBarHeight;
    int defaultHeight2 = mFontMetrics.height() * 1.8;
    int r              = qMax(defaultHeight, defaultHeight2);
    if (r < 25) {
        r = 25;
    }
    return r;
}

/**
 * @brief 根据当前Ribbon风格估算类别的高度（自 SARibbonBarLayout.cpp 纯 move）
 * @note 经过对照，1.6行高和office的高度比较接近。
 *       SingleRow模式下面板标题隐藏，因此不添加panelTitleHeight。
 * @return 计算出的类别高度
 */
int SARibbonMetrics::calcCategoryHeight(bool isThreeRowStyle, bool isSingleRowStyle) const
{
    int textH = mFontMetrics.lineSpacing();
    if (isThreeRowStyle) {
        // 4.8 = 3*1.6 for three rows
        return textH * 4.8 + panelTitleHeight;
    } else if (isSingleRowStyle) {
        // 1.8 for single row, no panel title in single-row mode
        return textH * 2;
    } else {
        // 3.2 = 2*1.6 for two rows
        return textH * 3.2 + panelTitleHeight;
    }
}

/**
 * @brief 计算主栏高度（自 SARibbonBarLayout.cpp 静态纯函数纯 move）
 * @param tabBarHeight Tab bar height
 * @param titleHeight Title bar height
 * @param categoryHeight Category height
 * @param tabOnTitle Whether tab is on title
 * @param minimumMode Whether the ribbon is in minimum mode
 * @return 计算出的主栏高度
 */
int SARibbonMetrics::calcMainBarHeight(int tabBarHeight, int titleHeight, int categoryHeight, bool tabOnTitle, bool minimumMode)
{
    if (minimumMode) {
        // Minimum mode, no category height
        if (tabOnTitle) {
            return titleHeight;
        } else {
            return titleHeight + tabBarHeight;
        }
    } else {
        if (tabOnTitle) {
            return titleHeight + categoryHeight;
        } else {
            return tabBarHeight + titleHeight + categoryHeight;
        }
    }
}

void SARibbonMetrics::estimateSizeHint(bool isThreeRowStyle, bool isSingleRowStyle)
{
    titleBarHeight = calcDefaultTitleBarHeight();
    // If tabBarHeight is greater than 0, use user-set value
    tabBarHeight   = calcDefaultTabBarHeight();
    categoryHeight = calcCategoryHeight(isThreeRowStyle, isSingleRowStyle);
}

int SARibbonMetrics::getActualTitleBarHeight() const
{
    if (mUserDefTitleBarHeight) {
        return *mUserDefTitleBarHeight;
    } else {
        return titleBarHeight;
    }
}

int SARibbonMetrics::getActualTabBarHeight() const
{
    if (mUserDefTabBarHeight) {
        return *mUserDefTabBarHeight;
    } else {
        return tabBarHeight;
    }
}

int SARibbonMetrics::getActualCategoryHeight() const
{
    if (mUserDefCategoryHeight) {
        return *mUserDefCategoryHeight;
    } else {
        return categoryHeight;
    }
}

void SARibbonMetrics::setTitleBarHeight(int h)
{
    mUserDefTitleBarHeight = h;
}

void SARibbonMetrics::setTabBarHeight(int h)
{
    mUserDefTabBarHeight = h;
}

void SARibbonMetrics::setCategoryHeight(int h)
{
    mUserDefCategoryHeight = h;
}

QFontMetrics SARibbonMetrics::fontMetrics() const
{
    return mFontMetrics;
}

qreal SARibbonMetrics::devicePixelRatio() const
{
    return mDevicePixelRatio;
}

}
}
