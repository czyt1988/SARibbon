#ifndef SARIBBONTOOLBUTTONLAYOUT_H
#define SARIBBONTOOLBUTTONLAYOUT_H
#include <SARibbonCore/SARibbonCoreGlobal.h>
#include <QFontMetrics>
#include <QRect>
#include <QSize>
#include <QString>
#include <Qt>

namespace SARibbon
{
namespace Core
{

/**
 * \if ENGLISH
 * @brief Layout constants shared by both front ends (pure move of the
 *        anonymous SARibbonToolButtonConstants namespace)
 * \endif
 *
 * \if CHINESE
 * @brief 两个前端共享的按钮布局常量（自 widgets 侧匿名
 *        SARibbonToolButtonConstants 命名空间纯搬移）
 * \endif
 */
namespace ToolButtonLayoutConstants
{
constexpr int DEFAULT_SPACING                   = 1;     ///< 默认按钮与边框的间距
constexpr int DEFAULT_INDICATOR_LEN_SMALL       = 12;    ///< 小按钮模式下默认指示器长度
constexpr int DEFAULT_INDICATOR_LEN_LARGE       = 8;     ///< 大按钮模式下默认指示器长度
constexpr int MIN_BUTTON_WIDTH                  = 16;    ///< 按钮最小宽度
constexpr int GLOBAL_STRUT_WIDTH                = 2;     ///< 全局尺寸约束宽度
constexpr int GLOBAL_STRUT_HEIGHT               = 2;     ///< 全局尺寸约束高度
constexpr qreal LARGE_BUTTON_HEIGHT_FACTOR      = 4.8;   ///< 大按钮高度系数 (相对于行间距)
constexpr qreal LARGE_BUTTON_MIN_WIDTH_RATIO    = 0.75;  ///< 大按钮最小宽度比例 (相对于高度)
constexpr int SMALL_BUTTON_HEIGHT_OFFSET        = 2;     ///< 小按钮文本绘制高度偏移
constexpr qreal TWO_LINE_HEIGHT_FACTOR_DEFAULT  = 2.05;  ///< 两行文本高度系数默认值
constexpr qreal ONE_LINE_HEIGHT_FACTOR_DEFAULT  = 1.2;   ///< 单行文本高度系数默认值
constexpr qreal BUTTON_MAX_ASPECT_RATIO_DEFAULT = 1.4;   ///< 按钮最大宽高比默认值
constexpr int INDICATOR_HEIGHT_FACTOR_NUM       = 12;    ///< 指示器高度计算分子
constexpr int INDICATOR_HEIGHT_FACTOR_DEN       = 10;    ///< 指示器高度计算分母 (即1.2倍)
constexpr int UNLIMITED_WIDTH                   = 16777215;  ///< 无宽度上限时的哨兵值（QWIDGETSIZE_MAX 属 QtWidgets）
}

/**
 * \if ENGLISH
 * @brief Tool button text/icon/indicator layout algorithm, shared by the
 *        widgets and the QML front end
 * @details Pure move of the SARibbonToolButton::PrivateData layout functions:
 *          the sizeHint derivation (large buttons get a fixed two-line text
 *          budget and a binary-searched wrap width capped by the aspect ratio,
 *          small buttons are icon + single line + indicator strip), the draw
 *          rects (icon on top, text box hugging the bottom edge, indicator
 *          below the text when it stays on one line and to its right when it
 *          wraps), the text alignment flags and the RTL mirroring. Widget
 *          touchpoints (QStyleOptionToolButton, maximumWidth(), the panel
 *          parent lookup) became plain Input values, so the QML host feeds the
 *          very same numbers and renders the very same geometry.
 * \endif
 *
 * \if CHINESE
 * @brief 工具按钮的图标/文字/指示器布局算法，widgets 与 QML 前端共用
 * @details 自 SARibbonToolButton::PrivateData 的布局函数纯搬移：sizeHint 推导
 *          （大按钮有固定的两行文字预算，换行宽度经二分查找并受宽高比上限约束；
 *          小按钮为图标 + 单行文字 + 指示器条）、绘制矩形（图标在上、文字盒贴底、
 *          指示器在文字单行时位于文字下方、换行时位于文字右侧）、文字对齐标志与
 *          RTL 镜像。widget 触点（QStyleOptionToolButton、maximumWidth()、父 panel
 *          查询）换为 Input 纯值，因此 QML 宿主喂入完全相同的数值即可得到完全相同
 *          的几何。
 * \endif
 */
class SA_RIBBON_CORE_EXPORT SARibbonToolButtonLayout
{
public:
    /// Per-instance tuning factors (widgets SARibbonToolButton::LayoutFactor)
    struct Factors
    {
        qreal twoLineHeightFactor { ToolButtonLayoutConstants::TWO_LINE_HEIGHT_FACTOR_DEFAULT };
        qreal oneLineHeightFactor { ToolButtonLayoutConstants::ONE_LINE_HEIGHT_FACTOR_DEFAULT };
        qreal buttonMaximumAspectRatio { ToolButtonLayoutConstants::BUTTON_MAX_ASPECT_RATIO_DEFAULT };
        qreal largeButtonMinimumWidthRatio { ToolButtonLayoutConstants::LARGE_BUTTON_MIN_WIDTH_RATIO };
    };

    /// Everything the algorithm needs; no front-end type may appear here
    struct Input
    {
        QRect rect;  ///< button rect in its own coordinate system
        Qt::ToolButtonStyle toolButtonStyle { Qt::ToolButtonIconOnly };
        bool isLargeButton { false };  ///< effective type (iconRightText forces small)
        bool enableWordWrap { true };
        bool hasIcon { false };
        bool hasIndicator { false };
        bool isRTL { false };
        QSize iconSize;                              ///< requested icon size (small button path)
        QSize largeIconSize { 32, 32 };              ///< large button icon size / min width base
        QString text;
        QFontMetrics fontMetrics { QFont() };
        int spacing { ToolButtonLayoutConstants::DEFAULT_SPACING };
        int indicatorLen { ToolButtonLayoutConstants::DEFAULT_INDICATOR_LEN_LARGE };
        int panelLargeButtonHeight { -1 };  ///< -1 when the button is not inside a panel
        int maximumWidth { ToolButtonLayoutConstants::UNLIMITED_WIDTH };
        Factors factors;
    };

    struct SizeHintResult
    {
        QSize sizeHint;
        bool isTextNeedWrap { false };
        int textDrawRectHeight { 0 };
        int sizeHintBaseHeight { -1 };  ///< panel height the hint was derived from (-1 = panel independent)
    };

    struct DrawRectResult
    {
        QRect iconRect;
        QRect textRect;
        QRect indicatorArrowRect;
    };

    /// sizeHint + wrap flag + text height (sets SizeHintResult::sizeHintBaseHeight for cache validation)
    static SizeHintResult calcSizeHint(const Input& in);

    /// icon / text / indicator rects inside in.rect, RTL mirrored
    static DrawRectResult calcDrawRects(const Input& in, bool isTextNeedWrap);

    /// Height of the text drawing box (two-line budget for wrapped large buttons)
    static int textDrawRectHeight(const Input& in);

    /// Qt alignment | text flags used when drawing the caption
    static int textAlignment(const Input& in);

    /// Optimal large button text width; *needWrap receives the wrap decision
    static int estimateLargeButtonTextWidth(const Input& in, int buttonHeight, int textHeight, bool* needWrap);

    /// Fit an icon size into the given rect, preserving the aspect ratio
    static QSize adjustIconSize(const QRect& buttonRect, const QSize& originIconSize);

    /// Indicator height derived from its length (1.2x)
    static int indicatorHeight(int indicatorLen);

    /// Strip '\n' only (single line captions)
    static QString simplifiedText(const QString& str);

private:
    static DrawRectResult calcLargeButtonDrawRects(const Input& in, bool isTextNeedWrap);
    static DrawRectResult calcSmallButtonDrawRects(const Input& in);
    static void mirrorRectsX(const Input& in, DrawRectResult& res);
    static int indicatorHeightOf(int indicatorLen);
};

}
}

#endif  // SARIBBONTOOLBUTTONLAYOUT_H
