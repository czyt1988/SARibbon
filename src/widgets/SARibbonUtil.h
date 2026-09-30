#ifndef SARIBBONUTIL_H
#define SARIBBONUTIL_H
#include "SARibbonGlobal.h"
#include <QColor>
#include <QSize>
#include <QIcon>
#include <SARibbonCore/SARibbonCoreUtil.h>

// 计划 02 S1：无 widget 依赖的函数已下沉 core（<SARibbonCore/SARibbonCoreUtil.h>，
// namespace SA 不变，SA::makeColorVibrant 等拼写零变化）；本文件保留 widgets 专属：
// widgetDevicePixelRatio（QWidget/QScreen）与 QSS 渲染两函数（v2 §3.2 决策）。
namespace SA
{

// 获取窗口当前所在屏幕的dpr
qreal widgetDevicePixelRatio(QWidget* w);

// Forward declaration
class SARibbonThemePalette;

// Replace {{token}} and {{token|opacity(value)}} patterns in QSS templates with actual color values
QString SA_RIBBON_EXPORT replaceQssTokens(const QString& templateQss, const SARibbonThemePalette& palette);

/**
 * \if ENGLISH
 * @brief Get the complete QSS stylesheet string for a built-in ribbon theme
 * @details Loads the base QSS, theme template, and default palette, then returns the fully
 * resolved stylesheet with all tokens replaced. Useful for debugging, inspecting generated
 * QSS, or as a starting point for custom theme overrides.
 * \endif
 *
 * \if CHINESE
 * @brief 获取指定内置ribbon主题的完整QSS样式表字符串
 * @details 加载基础QSS、主题模板和默认调色板，返回所有标记已替换的完整样式表。
 * 适用于调试、检查生成的QSS，或作为自定义主题覆盖的起点。
 * \endif
 */
QString SA_RIBBON_EXPORT getBuiltInRibbonThemeQss(SARibbonTheme theme);

}
#endif  // SARIBBONUTIL_H
