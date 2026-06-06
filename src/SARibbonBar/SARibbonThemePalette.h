#ifndef SARIBBONTHEMEPALETTE_H
#define SARIBBONTHEMEPALETTE_H
#include "SARibbonGlobal.h"
#include <QColor>
#include <QHash>
#include <QString>
#include <QByteArray>

namespace SA {

/**
 * \if ENGLISH
 * @brief Color palette for SARibbon theme system
 *
 * SARibbonThemePalette manages a set of named color tokens used by the ribbon theming engine.
 * Colors are organized into three layers: key colors (primary design tokens), derived colors
 * (computed from key colors via lighten/darken rules), and fixed colors (absolute values).
 *
 * A palette is loaded from a JSON file or byte array. The JSON structure is as follows:
 *
 * @code
 * {
 *     "name": "office-blue",          // Optional. Theme name for identification only
 *     "isDark": false,                // Optional. Defaults to false if omitted. true = dark theme; false = light theme.
 *                                     // When true, derived rules reverse direction
 *                                     // (darken becomes lighten and vice versa).
 *
 *     "keyColors": {                  // Required. Primary design tokens (name -> hex color string)
 *         "accent": "#225497",        //   Main accent / brand color
 *         "content-bg": "#f1f1f1",    //   Content area background color
 *         "text-color": "#1a1a1a"     //   Default text color
 *         // ... any number of custom token names
 *     },
 *
 *     "derived": {                    // Optional. Colors derived from keyColors via rules
 *         "accent-hover": {           //   Token name for the derived color
 *             "fn": "lighten",        //   Derive function: "lighten" or "darken"
 *             "base": "accent",       //   Source key color token name to derive from
 *             "amount": 15            //   Intensity percentage (e.g. 15 = lighter(115) or darker(115))
 *         },
 *         "accent-pressed": {
 *             "fn": "darken",
 *             "base": "accent",
 *             "amount": 10
 *         }
 *     },
 *
 *     "fixed": {                      // Optional. Absolute colors not tied to keyColors (name -> hex string)
 *         "window-border": "#c0c0c0",
 *         "separator": "#d0d0d0"
 *     }
 * }
 * @endcode
 *
 * Color lookup priority via color(): derived colors are checked first, then keyColors, then fixed.
 *
 * Usage example:
 * @code
 * SA::SARibbonThemePalette palette;
 * palette.loadFromFile(":/themes/light.json");
 * QColor accent = palette.color("accent");
 * QHash<QString, QString> allVars = palette.variables();
 * @endcode
 *
 * @note When isDark() is true, derive rules automatically reverse direction
 * (darken becomes lighten and vice versa) to maintain correct contrast.
 * @see SARibbonBar
 * \endif
 *
 * \if CHINESE
 * @brief SARibbon主题系统的调色板
 *
 * SARibbonThemePalette管理Ribbon主题引擎使用的一组命名颜色标记。
 * 颜色分为三层：键色（主要设计标记）、派生色（通过变亮/变暗规则从键色计算）和固定色（绝对值）。
 *
 * 调色板通过JSON文件或字节数组加载。JSON结构如下：
 *
 * @code
 * {
 *     "name": "office-blue",          // 可选。主题名称，仅作标识用途
 *     "isDark": false,                // 可选。默认为false。true = 深色主题；false = 浅色主题
 *                                     // 为true时，派生规则自动反转方向
 *                                     // （变暗变为变亮，反之亦然）
 *
 *     "keyColors": {                  // 必填。主要设计标记（名称 -> 十六进制颜色字符串）
 *         "accent": "#225497",        //   主色调/品牌色
 *         "content-bg": "#f1f1f1",    //   内容区域背景色
 *         "text-color": "#1a1a1a"     //   默认文字颜色
 *         // ... 可自定义任意数量的标记名
 *     },
 *
 *     "derived": {                    // 可选。通过规则从keyColors派生的颜色
 *         "accent-hover": {           //   派生颜色的标记名
 *             "fn": "lighten",        //   派生函数："lighten"（变亮）或"darken"（变暗）
 *             "base": "accent",       //   源键色标记名，从此颜色进行派生
 *             "amount": 15            //   强度百分比（例如15 = lighter(115)或darker(115)）
 *         },
 *         "accent-pressed": {
 *             "fn": "darken",
 *             "base": "accent",
 *             "amount": 10
 *         }
 *     },
 *
 *     "fixed": {                      // 可选。不依赖键色的绝对颜色（名称 -> 十六进制字符串）
 *         "window-border": "#c0c0c0",
 *         "separator": "#d0d0d0"
 *     }
 * }
 * @endcode
 *
 * color()查找优先级：先查派生色，再查键色，最后查固定色。
 *
 * 使用示例：
 * @code
 * SA::SARibbonThemePalette palette;
 * palette.loadFromFile(":/themes/light.json");
 * QColor accent = palette.color("accent");
 * QHash<QString, QString> allVars = palette.variables();
 * @endcode
 *
 * @note 当isDark()为true时，派生规则自动反转方向（变暗变为变亮，反之亦然），以保持正确的对比度。
 * @see SARibbonBar
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonThemePalette
{
public:
    // Constructor
    SARibbonThemePalette();

    // Load palette from a JSON byte array
    bool loadFromJson(const QByteArray& json);

    // Load palette from a JSON file (filesystem path or Qt resource path)
    bool loadFromFile(const QString& jsonPath);

    // Set the accent key color
    void setAccentColor(const QColor& color);

    // Set the content background key color
    void setContentBgColor(const QColor& color);

    // Set the text key color
    void setTextColor(const QColor& color);

    // Get a color by token name, searching derived, key, then fixed layers
    QColor color(const QString& tokenName) const;

    // Get all color variables as name-to-hex-string pairs
    QHash<QString, QString> variables() const;

    // Check if this palette is a dark theme
    bool isDark() const;

private:
    QHash<QString, QColor> m_keyColors;
    QHash<QString, QColor> m_derivedColors;
    QHash<QString, QColor> m_fixedColors;
    bool m_isDark { false };
};

} // namespace SA
#endif // SARIBBONTHEMEPALETTE_H
