#ifndef SARIBBONCOREUTIL_H
#define SARIBBONCOREUTIL_H
#include "SARibbonCoreGlobal.h"
#include <QColor>
#include <QSize>
#include <QIcon>
#include <QPixmap>

// 计划 02 S1：SARibbonUtil 中无 widget 依赖的函数下沉 core（函数体纯 move，一字不改）。
// namespace SA 维持不变（2.x 既有 API 拼写，v2 §3.5：仅新增类型入 SARibbon::Core）。
// QSS 渲染（replaceQssTokens/getBuiltInRibbonThemeQss）与 widgetDevicePixelRatio 留 widgets（v2 §3.2）。
namespace SA
{

// 让颜色鲜艳
QColor SA_RIBBON_CORE_EXPORT makeColorVibrant(const QColor& c, int saturationDelta = 150, int valueDelta = 30);

// 按照指定的新高度，保持宽高比缩放 QSize
QSize SA_RIBBON_CORE_EXPORT scaleSizeByHeight(const QSize& originalSize, int newHeight);

// 按照指定的新高度，宽高比为1:factor缩放 QSize。
QSize SA_RIBBON_CORE_EXPORT scaleSizeByHeight(const QSize& originalSize, int newHeight, qreal factor);

// 按照指定的新宽度，保持宽高比缩放 QSize
QSize SA_RIBBON_CORE_EXPORT scaleSizeByWidth(const QSize& originalSize, int newWidth);

// 提供类似QIcon::pixmap(const QSize &size, qreal devicePixelRatio, Mode mode, State state) const（Qt6新增）的兼容函数
QPixmap SA_RIBBON_CORE_EXPORT iconToPixmap(const QIcon& icon,
                                           const QSize& size,
                                           qreal devicePixelRatio,
                                           QIcon::Mode mode   = QIcon::Normal,
                                           QIcon::State state = QIcon::Off);

/**
 * \if ENGLISH
 * @brief Check if the application layout direction is Right-to-Left (RTL)
 * @return true if layout direction is Qt::RightToLeft, false otherwise
 * \endif
 *
 * \if CHINESE
 * @brief 检查应用程序布局方向是否为从右到左（RTL）
 * @note core 版改用 QGuiApplication::layoutDirection()（QtGui，行为与 widgets 版等价）；
 * 三个布局引擎一律经 Input.isRTL 入参取该值，不得直接调用本函数（确定性层禁区）
 * \endif
 */
bool SA_RIBBON_CORE_EXPORT saIsRTL();

/**
 * \if ENGLISH
 * @brief Mirror X coordinate for RTL layout support
 * @param x The original X coordinate
 * @param containerWidth The width of the container
 * @param elementWidth The width of the element
 * @return containerWidth - x - elementWidth when RTL, x unchanged when LTR
 * \endif
 *
 * \if CHINESE
 * @brief 为 RTL 布局支持镜像 X 坐标
 * @param x 原始 X 坐标
 * @param containerWidth 容器宽度
 * @param elementWidth 元素宽度
 * @return RTL 时返回 containerWidth - x - elementWidth，LTR 时返回 x 不变
 * \endif
 */
int SA_RIBBON_CORE_EXPORT saMirrorX(int x, int containerWidth, int elementWidth);

// Check if the operating system uses dark mode (cross-platform)
bool SA_RIBBON_CORE_EXPORT isOperatingSystemInDarkMode();

// Enable or disable automatic switching from the default theme to RibbonThemeDark when the operating system is in dark mode (enabled by default)
void SA_RIBBON_CORE_EXPORT setEnableSystemDarkModeAutoSwitch(bool on);

// Query whether automatic theme switching by operating system dark mode is enabled
bool SA_RIBBON_CORE_EXPORT isEnableSystemDarkModeAutoSwitch();

/**
 * \if ENGLISH
 * @brief Gallery grid cell size shared by both front ends
 * @param galleryHeight The gallery body height (cell height derives from it)
 * @param displayRow Visible grid rows, clamped to [1, 3]
 * @param gridMinimumWidth Minimum cell width (<= 0 disables the lower bound)
 * @param gridMaximumWidth Maximum cell width (<= 0 disables the upper bound)
 * @return (width, height) of one grid cell
 * @details Moved from SARibbonGalleryGroup::recalcGridSize so the QML gallery
 *          derives identical cells from identical inputs.
 * \endif
 *
 * \if CHINESE
 * @brief 画廊网格单元尺寸（双前端共用）
 * @param galleryHeight 画廊主体高度（单元高度由此推导）
 * @param displayRow 可见网格行数，钳制到 [1, 3]
 * @param gridMinimumWidth 单元最小宽度（<= 0 关闭下限）
 * @param gridMaximumWidth 单元最大宽度（<= 0 关闭上限）
 * @return 单个网格单元的 (width, height)
 * @details 自 SARibbonGalleryGroup::recalcGridSize 下沉，QML 画廊以相同
 *          输入推导出相同单元。
 * \endif
 */
QSize SA_RIBBON_CORE_EXPORT calcGalleryGridCellSize(int galleryHeight,
                                                    int displayRow,
                                                    int gridMinimumWidth,
                                                    int gridMaximumWidth);

}

#endif  // SARIBBONCOREUTIL_H
