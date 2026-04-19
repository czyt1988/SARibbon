#include "SARibbonButtonLayoutStrategy.h"
#include "SARibbonToolButton.h"
#include "SARibbonPanel.h"
#include "SARibbonQt5Compat.hpp"
#include "SARibbonUtil.h"
#include <QFontMetrics>
#include <QLatin1Char>

// 布局常量定义 (与SARibbonToolButton.cpp中保持一致)
namespace
{
constexpr int MIN_BUTTON_WIDTH               = 16;
constexpr int GLOBAL_STRUT_WIDTH             = 2;
constexpr int GLOBAL_STRUT_HEIGHT            = 2;
constexpr qreal LARGE_BUTTON_HEIGHT_FACTOR   = 4.8;
constexpr qreal LARGE_BUTTON_MIN_WIDTH_RATIO = 0.75;
constexpr int SMALL_BUTTON_HEIGHT_OFFSET     = 2;
constexpr int INDICATOR_HEIGHT_FACTOR_NUM    = 12;
constexpr int INDICATOR_HEIGHT_FACTOR_DEN    = 10;
}

//===================================================================
// SARibbonButtonLayoutContext
//===================================================================

SARibbonButtonLayoutContext::SARibbonButtonLayoutContext(const SARibbonToolButton* btn) : buttonWidget(btn)
{
    if (btn) {
        spacing                  = btn->spacing();
        largeIconSize            = btn->largeIconSize();
        smallIconSize            = btn->smallIconSize();
        const auto& factor       = btn->layoutFactor();
        twoLineHeightFactor      = factor.twoLineHeightFactor;
        oneLineHeightFactor      = factor.oneLineHeightFactor;
        buttonMaximumAspectRatio = factor.buttonMaximumAspectRatio;
        enableWordWrap           = btn->isEnableWordWrap();
        maximumWidth             = btn->maximumWidth();
    }
}

//===================================================================
// SARibbonButtonLayoutStrategy
//===================================================================

bool SARibbonButtonLayoutStrategy::hasIndicator(const QStyleOptionToolButton& opt)
{
    return ((opt.features & QStyleOptionToolButton::MenuButtonPopup) || (opt.features & QStyleOptionToolButton::HasMenu));
}

QSize SARibbonButtonLayoutStrategy::adjustIconSize(const QRect& buttonRect, const QSize& originIconSize)
{
    if (buttonRect.isEmpty() || originIconSize.isEmpty()) {
        return QSize(0, 0);
    }

    QSize iconSize = originIconSize;

    if (iconSize.width() <= buttonRect.width() && iconSize.height() <= buttonRect.height()) {
        return iconSize;
    }

    qreal aspectRatio = static_cast< qreal >(originIconSize.width()) / originIconSize.height();

    if (iconSize.height() > buttonRect.height()) {
        iconSize.setHeight(buttonRect.height());
        iconSize.setWidth(qRound(buttonRect.height() * aspectRatio));
    }

    if (iconSize.width() > buttonRect.width()) {
        iconSize.setWidth(buttonRect.width());
        iconSize.setHeight(qRound(buttonRect.width() / aspectRatio));
    }

    iconSize.setWidth(qMin(iconSize.width(), buttonRect.width()));
    iconSize.setHeight(qMin(iconSize.height(), buttonRect.height()));

    return iconSize;
}

QString SARibbonButtonLayoutStrategy::simplifiedText(const QString& str)
{
    QString res = str;
    res.remove('\n');
    return res;
}

int SARibbonButtonLayoutStrategy::estimateTextWidth(int buttonHeight,
                                                    int textHeight,
                                                    const QString& text,
                                                    const QFontMetrics& fm,
                                                    SARibbonButtonLayoutContext& ctx) const
{
    QSize textSize;
    int space        = SA::compat::horizontalAdvance(fm, QLatin1Char(' ')) * 2;
    int hintMaxWidth = qMin(static_cast< int >(buttonHeight * ctx.buttonMaximumAspectRatio), ctx.maximumWidth);

    if (ctx.enableWordWrap) {
        textSize = fm.size(Qt::TextShowMnemonic, text);
        textSize.setWidth(textSize.width() + space);

        if (textSize.height() > fm.lineSpacing() * 1.1) {
            ctx.isTextNeedWrap = true;
            return textSize.width();
        }

        if (textSize.width() <= hintMaxWidth) {
            ctx.isTextNeedWrap = false;
            return textSize.width();
        }

        int alignment = Qt::TextShowMnemonic | Qt::TextWordWrap;
        int minWidth  = textSize.width() / 2;
        int maxWidth  = textSize.width();
        int bestWidth = maxWidth;

        for (int i = 0; i < 10; ++i) {
            int midWidth = (minWidth + maxWidth) / 2;
            QRect textRect(0, 0, midWidth, textHeight);
            textRect = fm.boundingRect(textRect, alignment, text);

            if (textRect.height() <= fm.lineSpacing() * 2) {
                bestWidth = midWidth;
                maxWidth  = midWidth - 1;
            } else {
                minWidth = midWidth + 1;
            }

            if (minWidth > maxWidth) {
                break;
            }
        }

        ctx.isTextNeedWrap = true;
        return bestWidth;
    }

    ctx.isTextNeedWrap = false;
    textSize           = fm.size(Qt::TextShowMnemonic, simplifiedText(text));
    textSize.setWidth(textSize.width() + space);

    if (textSize.width() < hintMaxWidth) {
        return textSize.width();
    }
    return hintMaxWidth;
}

//===================================================================
// SARibbonLargeButtonLayoutStrategy
//===================================================================

void SARibbonLargeButtonLayoutStrategy::calculateDrawRects(const QStyleOptionToolButton& opt,
                                                           SARibbonButtonLayoutRects& rects,
                                                           const SARibbonButtonLayoutContext& ctx) const
{
    rects.clear();

    switch (opt.toolButtonStyle) {
    case Qt::ToolButtonIconOnly:
        calculateIconOnlyRects(opt, rects, ctx);
        break;
    case Qt::ToolButtonTextOnly:
        calculateTextOnlyRects(opt, rects, ctx);
        break;
    default:
        calculateIconAndTextRects(opt, rects, ctx);
        break;
    }
}

/**
 * \if ENGLISH
 * @brief Calculate layout rects for large button with icon only style
 * @param[in] opt Style option for the tool button
 * @param[out] rects Output rectangles collection
 * @param[in] ctx Layout context containing layout parameters
 * @details Supports both LTR and RTL layouts. In RTL mode, indicator is positioned at bottom-left corner.
 * \endif
 * 
 * \if CHINESE
 * @brief 计算仅显示图标的大按钮布局矩形
 * @param[in] opt 工具按钮的样式选项
 * @param[out] rects 输出的矩形集合
 * @param[in] ctx 包含布局参数的布局上下文
 * @details 同时支持LTR和RTL布局。在RTL模式下，指示器位于左下角。
 * \endif
 */
void SARibbonLargeButtonLayoutStrategy::calculateIconOnlyRects(const QStyleOptionToolButton& opt,
                                                               SARibbonButtonLayoutRects& rects,
                                                               const SARibbonButtonLayoutContext& ctx) const
{
    bool hasInd      = hasIndicator(opt);
    int spacing      = ctx.spacing;
    int indicatorLen = ctx.indicatorLength;

    if (hasInd) {
        int indicatorHeight = static_cast< int >(indicatorLen * INDICATOR_HEIGHT_FACTOR_NUM / INDICATOR_HEIGHT_FACTOR_DEN);
        rects.indicatorRect = QRect(opt.rect.left() + spacing,
                                    opt.rect.bottom() - indicatorHeight - spacing,
                                    opt.rect.width() - 2 * spacing,
                                    indicatorHeight);
        rects.iconRect      = QRect(opt.rect.left() + spacing,
                               opt.rect.top() + spacing,
                               opt.rect.width() - 2 * spacing,
                               opt.rect.height() - 2 * spacing - indicatorHeight);
    } else {
        rects.iconRect = QRect(opt.rect.left() + spacing,
                               opt.rect.top() + spacing,
                               opt.rect.width() - 2 * spacing,
                               opt.rect.height() - 2 * spacing);
    }
}

/**
 * \if ENGLISH
 * @brief Calculate layout rects for large button with text only style
 * @param[in] opt Style option for the tool button
 * @param[out] rects Output rectangles collection
 * @param[in] ctx Layout context containing layout parameters
 * @details Supports both LTR and RTL layouts. In RTL mode, text is right-aligned.
 * \endif
 * 
 * \if CHINESE
 * @brief 计算仅显示文本的大按钮布局矩形
 * @param[in] opt 工具按钮的样式选项
 * @param[out] rects 输出的矩形集合
 * @param[in] ctx 包含布局参数的布局上下文
 * @details 同时支持LTR和RTL布局。在RTL模式下，文本右对齐。
 * \endif
 */
void SARibbonLargeButtonLayoutStrategy::calculateTextOnlyRects(const QStyleOptionToolButton& opt,
                                                               SARibbonButtonLayoutRects& rects,
                                                               const SARibbonButtonLayoutContext& ctx) const
{
    bool hasInd      = hasIndicator(opt);
    int spacing      = ctx.spacing;
    int indicatorLen = ctx.indicatorLength;

    if (hasInd) {
        int indicatorHeight = static_cast< int >(indicatorLen * INDICATOR_HEIGHT_FACTOR_NUM / INDICATOR_HEIGHT_FACTOR_DEN);
        rects.indicatorRect = QRect(opt.rect.left() + spacing,
                                    opt.rect.bottom() - indicatorHeight - spacing,
                                    opt.rect.width() - 2 * spacing,
                                    indicatorHeight);
        rects.textRect      = QRect(opt.rect.left() + spacing,
                               opt.rect.top() + spacing,
                               opt.rect.width() - 2 * spacing,
                               opt.rect.height() - 2 * spacing - indicatorHeight);
    } else {
        rects.textRect = QRect(opt.rect.left() + spacing,
                               opt.rect.top() + spacing,
                               opt.rect.width() - 2 * spacing,
                               opt.rect.height() - 2 * spacing);
    }
}

/**
 * \if ENGLISH
 * @brief Calculate layout rects for large button with both icon and text
 * @param[in] opt Style option for the tool button
 * @param[out] rects Output rectangles collection
 * @param[in] ctx Layout context containing layout parameters
 * @details Supports both LTR and RTL layouts. In RTL mode:
 * - Indicator is positioned at bottom-left corner
 * - Text is right-aligned
 * - Icon remains centered
 * \endif
 * 
 * \if CHINESE
 * @brief 计算同时显示图标和文本的大按钮布局矩形
 * @param[in] opt 工具按钮的样式选项
 * @param[out] rects 输出的矩形集合
 * @param[in] ctx 包含布局参数的布局上下文
 * @details 同时支持LTR和RTL布局。在RTL模式下：
 * - 指示器位于左下角
 * - 文本右对齐
 * - 图标保持居中
 * \endif
 */
void SARibbonLargeButtonLayoutStrategy::calculateIconAndTextRects(const QStyleOptionToolButton& opt,
                                                                  SARibbonButtonLayoutRects& rects,
                                                                  const SARibbonButtonLayoutContext& ctx) const
{
    int spacing      = ctx.spacing;
    int indicatorLen = ctx.indicatorLength;
    bool hasInd      = hasIndicator(opt);

    if (!hasInd) {
        indicatorLen = 0;
    }

    int textHeight = calculateTextHeight(opt, ctx);

    if (ctx.enableWordWrap && ctx.isTextNeedWrap) {
        rects.textRect = QRect(opt.rect.left() + spacing,
                               opt.rect.bottom() - spacing - textHeight,
                               opt.rect.width() - 2 * spacing - indicatorLen,
                               textHeight);
        if (hasInd) {
            int indX = rects.textRect.right();
            if (SA::saIsRTL()) {
                indX = SA::saMirrorX(indX, opt.rect.width(), indicatorLen);
                rects.textRect.moveLeft(opt.rect.right() - spacing - rects.textRect.width());
            }
            rects.indicatorRect = QRect(
                indX, rects.textRect.y() + rects.textRect.height() / 2, indicatorLen, textHeight / 2);
        } else if (SA::saIsRTL()) {
            rects.textRect.moveLeft(opt.rect.right() - spacing - rects.textRect.width());
        }
    } else if (ctx.enableWordWrap) {
        rects.textRect = QRect(
            opt.rect.left() + spacing, opt.rect.bottom() - spacing - textHeight, opt.rect.width() - 2 * spacing, textHeight);
        if (hasInd) {
            int dy = rects.textRect.height() / 2;
            dy += (dy - indicatorLen) / 2;
            rects.indicatorRect =
                QRect(rects.textRect.left(), rects.textRect.top() + dy, rects.textRect.width(), indicatorLen);
        }
        if (SA::saIsRTL()) {
            rects.textRect.moveLeft(opt.rect.right() - spacing - rects.textRect.width());
        }
    } else {
        int y = opt.rect.bottom() - spacing - textHeight;
        if (hasInd) {
            int indX = opt.rect.right() - indicatorLen - spacing;
            int textX = spacing;
            int textWidth = indX - spacing;
            if (SA::saIsRTL()) {
                indX = spacing;
                textX = indX + indicatorLen;
                textWidth = opt.rect.width() - 2 * spacing - indicatorLen;
            }
            rects.indicatorRect = QRect(indX, y, indicatorLen, textHeight);
            rects.textRect      = QRect(textX, y, textWidth, textHeight);
        } else {
            rects.textRect = QRect(opt.rect.left() + spacing, y, opt.rect.width() - 2 * spacing, textHeight);
            if (SA::saIsRTL()) {
                rects.textRect.moveLeft(opt.rect.right() - spacing - rects.textRect.width());
            }
        }
    }

    rects.iconRect = QRect(spacing, spacing, opt.rect.width() - 2 * spacing, rects.textRect.top() - 2 * spacing);
}

QSize SARibbonLargeButtonLayoutStrategy::calculateSizeHint(const QStyleOptionToolButton& opt,
                                                           const SARibbonButtonLayoutContext& ctx) const
{
    int w    = 0;
    int h    = opt.fontMetrics.lineSpacing() * LARGE_BUTTON_HEIGHT_FACTOR;
    int minW = static_cast< int >(h * LARGE_BUTTON_MIN_WIDTH_RATIO);

    if (ctx.buttonWidget) {
        if (SARibbonPanel* panel = qobject_cast< SARibbonPanel* >(ctx.buttonWidget->parent())) {
            h = panel->largeButtonHeight();
        }
    }

    int textHeight = calculateTextHeight(opt, ctx);

    SARibbonButtonLayoutContext mutableCtx = ctx;
    w                                      = estimateTextWidth(h, textHeight, opt.text, opt.fontMetrics, mutableCtx);
    w += (2 * ctx.spacing);

    if (ctx.enableWordWrap && mutableCtx.isTextNeedWrap) {
        w += ctx.indicatorLength;
    }

    return QSize(w, h).expandedTo(QSize(minW, textHeight)).expandedTo(QSize(GLOBAL_STRUT_WIDTH, GLOBAL_STRUT_HEIGHT));
}

int SARibbonLargeButtonLayoutStrategy::calculateTextHeight(const QStyleOptionToolButton& opt,
                                                           const SARibbonButtonLayoutContext& ctx) const
{
    if (ctx.enableWordWrap) {
        return opt.fontMetrics.lineSpacing() * ctx.twoLineHeightFactor + opt.fontMetrics.leading();
    } else {
        return opt.fontMetrics.lineSpacing() * ctx.oneLineHeightFactor;
    }
}

//===================================================================
// SARibbonSmallButtonLayoutStrategy
//===================================================================

void SARibbonSmallButtonLayoutStrategy::calculateDrawRects(const QStyleOptionToolButton& opt,
                                                           SARibbonButtonLayoutRects& rects,
                                                           const SARibbonButtonLayoutContext& ctx) const
{
    rects.clear();

    switch (opt.toolButtonStyle) {
    case Qt::ToolButtonIconOnly:
        calculateIconOnlyRects(opt, rects, ctx);
        break;
    case Qt::ToolButtonTextOnly:
        calculateTextOnlyRects(opt, rects, ctx);
        break;
    default:
        calculateIconAndTextRects(opt, rects, ctx);
        break;
    }
}

/**
 * \if ENGLISH
 * @brief Calculate layout rects for small button with icon only style
 * @param[in] opt Style option for the tool button
 * @param[out] rects Output rectangles collection
 * @param[in] ctx Layout context containing layout parameters
 * @details Supports both LTR and RTL layouts. In RTL mode:
 * - Indicator is positioned at left edge
 * - Icon is positioned at right side of the indicator
 * \endif
 * 
 * \if CHINESE
 * @brief 计算仅显示图标的小按钮布局矩形
 * @param[in] opt 工具按钮的样式选项
 * @param[out] rects 输出的矩形集合
 * @param[in] ctx 包含布局参数的布局上下文
 * @details 同时支持LTR和RTL布局。在RTL模式下：
 * - 指示器位于左边缘
 * - 图标位于指示器的右侧
 * \endif
 */
void SARibbonSmallButtonLayoutStrategy::calculateIconOnlyRects(const QStyleOptionToolButton& opt,
                                                               SARibbonButtonLayoutRects& rects,
                                                               const SARibbonButtonLayoutContext& ctx) const
{
    int spacing      = ctx.spacing;
    int indicatorLen = ctx.indicatorLength;

    if (hasIndicator(opt)) {
        if (SA::saIsRTL()) {
            rects.indicatorRect = QRect(opt.rect.left() + spacing, opt.rect.top() + spacing, indicatorLen, opt.rect.height() - 2 * spacing);
            rects.iconRect = opt.rect.adjusted(indicatorLen + spacing, spacing, -spacing, -spacing);
        } else {
            rects.iconRect = opt.rect.adjusted(spacing, spacing, -indicatorLen - spacing, -spacing);
            rects.indicatorRect =
                QRect(opt.rect.right() - indicatorLen - spacing, rects.iconRect.y(), indicatorLen, rects.iconRect.height());
        }
    } else {
        rects.iconRect = opt.rect.adjusted(spacing, spacing, -spacing, -spacing);
    }
}

/**
 * \if ENGLISH
 * @brief Calculate layout rects for small button with text only style
 * @param[in] opt Style option for the tool button
 * @param[out] rects Output rectangles collection
 * @param[in] ctx Layout context containing layout parameters
 * @details Supports both LTR and RTL layouts. In RTL mode:
 * - Indicator is positioned at left edge
 * - Text is positioned at right side of the indicator
 * \endif
 * 
 * \if CHINESE
 * @brief 计算仅显示文本的小按钮布局矩形
 * @param[in] opt 工具按钮的样式选项
 * @param[out] rects 输出的矩形集合
 * @param[in] ctx 包含布局参数的布局上下文
 * @details 同时支持LTR和RTL布局。在RTL模式下：
 * - 指示器位于左边缘
 * - 文本位于指示器的右侧
 * \endif
 */
void SARibbonSmallButtonLayoutStrategy::calculateTextOnlyRects(const QStyleOptionToolButton& opt,
                                                               SARibbonButtonLayoutRects& rects,
                                                               const SARibbonButtonLayoutContext& ctx) const
{
    int spacing      = ctx.spacing;
    int indicatorLen = ctx.indicatorLength;

    if (hasIndicator(opt)) {
        if (SA::saIsRTL()) {
            rects.indicatorRect = QRect(opt.rect.left() + spacing, spacing, indicatorLen, opt.rect.height() - 2 * spacing);
            rects.textRect = opt.rect.adjusted(indicatorLen + spacing, spacing, -spacing, -spacing);
        } else {
            rects.textRect = opt.rect.adjusted(spacing, spacing, -indicatorLen - spacing, -spacing);
            rects.indicatorRect =
                QRect(opt.rect.right() - indicatorLen - spacing, spacing, indicatorLen, rects.textRect.height());
        }
    } else {
        rects.textRect = opt.rect.adjusted(spacing, spacing, -spacing, -spacing);
    }
}

/**
 * \if ENGLISH
 * @brief Calculate layout rects for small button with both icon and text
 * @param[in] opt Style option for the tool button
 * @param[out] rects Output rectangles collection
 * @param[in] ctx Layout context containing layout parameters
 * @details Supports both LTR and RTL layouts. In RTL mode:
 * - Indicator is positioned at left edge
 * - Text is positioned at left side, next to indicator
 * - Icon is positioned at right edge
 * \endif
 * 
 * \if CHINESE
 * @brief 计算同时显示图标和文本的小按钮布局矩形
 * @param[in] opt 工具按钮的样式选项
 * @param[out] rects 输出的矩形集合
 * @param[in] ctx 包含布局参数的布局上下文
 * @details 同时支持LTR和RTL布局。在RTL模式下：
 * - 指示器位于左边缘
 * - 文本位于左侧，紧邻指示器
 * - 图标位于右边缘
 * \endif
 */
void SARibbonSmallButtonLayoutStrategy::calculateIconAndTextRects(const QStyleOptionToolButton& opt,
                                                                  SARibbonButtonLayoutRects& rects,
                                                                  const SARibbonButtonLayoutContext& ctx) const
{
    bool hasInd      = hasIndicator(opt);
    int spacing      = ctx.spacing;
    int indicatorLen = ctx.indicatorLength;

    QRect buttonRect = opt.rect;
    buttonRect.adjust(spacing, spacing, -spacing, -spacing);

    if (!opt.icon.isNull()) {
        QSize iconSize = adjustIconSize(buttonRect, opt.iconSize);
        int iconX = buttonRect.x();
        if (SA::saIsRTL()) {
            iconX = buttonRect.right() - iconSize.width();
        }
        rects.iconRect =
            QRect(iconX, buttonRect.y(), iconSize.width(), qMax(iconSize.height(), buttonRect.height()));
    }

    if (!opt.text.isEmpty()) {
        int adjx = rects.iconRect.isValid() ? (rects.iconRect.width() + spacing) : 0;
        int textX = rects.iconRect.isValid() ? (SA::saIsRTL() ? buttonRect.x() : rects.iconRect.right() + spacing) : buttonRect.x();
        int textRightMargin = hasInd ? indicatorLen : 0;
        int textWidth = SA::saIsRTL() ? (rects.iconRect.isValid() ? (rects.iconRect.x() - spacing - textX - textRightMargin) : (buttonRect.width() - textRightMargin)) : (buttonRect.width() - adjx - textRightMargin);
        
        rects.textRect = QRect(textX, buttonRect.y(), textWidth, buttonRect.height());
    }

    if (hasInd) {
        int indX = buttonRect.right() - indicatorLen + 1;
        if (SA::saIsRTL()) {
            indX = buttonRect.left();
        }
        if (rects.textRect.isValid()) {
            rects.indicatorRect =
                QRect(indX, rects.textRect.y(), indicatorLen, rects.textRect.height());
        } else if (rects.iconRect.isValid()) {
            rects.indicatorRect =
                QRect(indX, rects.iconRect.y(), indicatorLen, rects.iconRect.height());
        } else {
            rects.indicatorRect = buttonRect;
        }
    }
}

QSize SARibbonSmallButtonLayoutStrategy::calculateSizeHint(const QStyleOptionToolButton& opt,
                                                           const SARibbonButtonLayoutContext& ctx) const
{
    int w = 0, h = 0;
    int spacing = ctx.spacing;

    switch (opt.toolButtonStyle) {
    case Qt::ToolButtonIconOnly:
        w = opt.iconSize.width() + 2 * spacing;
        h = opt.iconSize.height() + 2 * spacing;
        break;
    case Qt::ToolButtonTextOnly: {
        QSize textSize = opt.fontMetrics.size(Qt::TextShowMnemonic, simplifiedText(opt.text));
        textSize.setWidth(textSize.width() + SA::compat::horizontalAdvance(opt.fontMetrics, QLatin1Char(' ')) * 2);
        textSize.setHeight(calculateTextHeight(opt, ctx));
        w = textSize.width() + 2 * spacing;
        h = textSize.height() + 2 * spacing;
    } break;
    default:
        w = opt.iconSize.width() + 2 * spacing;
        h = opt.iconSize.height() + 2 * spacing;
        if (!opt.text.isEmpty()) {
            QSize textSize = opt.fontMetrics.size(Qt::TextShowMnemonic, simplifiedText(opt.text));
            textSize.setWidth(textSize.width() + SA::compat::horizontalAdvance(opt.fontMetrics, QLatin1Char(' ')) * 2);
            textSize.setHeight(calculateTextHeight(opt, ctx));
            w += spacing + textSize.width();
            h = qMax(h, textSize.height() + 2 * spacing);
        } else {
            QSize textSize = opt.fontMetrics.size(Qt::TextShowMnemonic, QStringLiteral(" "));
            h              = qMax(h, textSize.height() + 2 * spacing);
        }
    }

    if (hasIndicator(opt)) {
        w += ctx.indicatorLength;
    }

    if (w < MIN_BUTTON_WIDTH) {
        w = MIN_BUTTON_WIDTH;
    }

    return QSize(w, h).expandedTo(QSize(GLOBAL_STRUT_WIDTH, GLOBAL_STRUT_HEIGHT));
}

int SARibbonSmallButtonLayoutStrategy::calculateTextHeight(const QStyleOptionToolButton& opt,
                                                           const SARibbonButtonLayoutContext& ctx) const
{
    Q_UNUSED(ctx)
    return opt.rect.height() - SMALL_BUTTON_HEIGHT_OFFSET;
}

//===================================================================
// SARibbonButtonLayoutStrategyFactory
//===================================================================

std::unique_ptr< SARibbonButtonLayoutStrategy > SARibbonButtonLayoutStrategyFactory::createStrategy(SARibbonButtonType type)
{
    switch (type) {
    case SARibbonButtonType::LargeButton:
        return std::make_unique< SARibbonLargeButtonLayoutStrategy >();
    case SARibbonButtonType::SmallButton:
    default:
        return std::make_unique< SARibbonSmallButtonLayoutStrategy >();
    }
}
