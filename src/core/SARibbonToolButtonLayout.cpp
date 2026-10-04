#include <SARibbonCore/SARibbonToolButtonLayout.h>
#include <SARibbonCore/SARibbonCoreUtil.h>
#include <SARibbonCore/SARibbonQt5Compat.hpp>

// 函数体自 src/widgets/SARibbonToolButton.cpp（SARibbonToolButton::PrivateData）纯 move。
// 机械替换表（其余一字不改）：
//   opt.rect                       -> in.rect
//   opt.toolButtonStyle            -> in.toolButtonStyle
//   opt.text                       -> in.text
//   opt.iconSize                   -> in.iconSize
//   opt.icon.isNull()              -> !in.hasIcon
//   opt.fontMetrics                -> in.fontMetrics
//   hasIndicator(opt)              -> in.hasIndicator
//   effectiveButtonType()==LargeButton -> in.isLargeButton
//   q_ptr->isEnableWordWrap()      -> in.enableWordWrap
//   q_ptr->toolButtonStyle()       -> in.toolButtonStyle
//   q_ptr->maximumWidth()          -> in.maximumWidth
//   q_ptr->rect()                  -> in.rect
//   mLargeButtonSizeHint           -> in.largeIconSize
//   panelLargeButtonHeight()       -> in.panelLargeButtonHeight
//   layoutFactor                   -> in.factors
//   mSpacing/mIndicatorLen         -> in.spacing/in.indicatorLen
//   SA::saIsRTL()                  -> in.isRTL
//   mIsTextNeedWrap/mSizeHintBaseHeight -> 返回值字段（由调用方缓存）
//   调试打印宏块不随迁（默认编译关闭）
namespace SARibbon
{
namespace Core
{

namespace TBLC = ToolButtonLayoutConstants;

SARibbonToolButtonLayout::SizeHintResult SARibbonToolButtonLayout::calcSizeHint(const Input& in)
{
    SizeHintResult res;
    res.textDrawRectHeight = textDrawRectHeight(in);
    if (in.isLargeButton) {
        int w = 0;
        int h = qRound(in.fontMetrics.lineSpacing() * TBLC::LARGE_BUTTON_HEIGHT_FACTOR);
        // 最小宽度，在panel里面的按钮，最小宽度要和icon适应；比例可通过largeButtonMinimumWidthRatio调整，
        // 小于等于0时取消高度比例约束，仅以icon宽度作为下限，宽度由icon和文字内容决定。
        // 注意：minW必须基于字体行高推算的h计算，不能基于panelLargeButtonHeight：
        // sizeHint可能在panel尚未获得真实几何时被查询（如隐藏category被QStackedLayout::sizeHint
        // 遍历），此时largeButtonHeight()是任意值，而脏sizeHint会被按钮mSizeHint与面板的
        // 按钮sizeHint缓存双层固化，导致大按钮宽度异常收缩（v2.9.4回归缺陷，约束对引擎同样成立）
        qreal minWRatio = in.factors.largeButtonMinimumWidthRatio;
        int minW        = 0;
        if (minWRatio > 0.0) {
            minW = qRound(h * minWRatio);
        } else {
            minW = in.largeIconSize.width() + (2 * in.spacing);
        }

        // 记录本次计算所依据的大按钮高度，panel高度变化后缓存的sizeHint必须失效，
        // 否则宽高比和换行判断会一直沿用旧高度算出来的结果
        res.sizeHintBaseHeight = in.panelLargeButtonHeight;
        int textHeight         = res.textDrawRectHeight;
        if (res.sizeHintBaseHeight >= 0) {
            // 对于建立在SARibbonPanel的基础上的大按钮，把高度设置为SARibbonPanel计算的大按钮高度
            h = res.sizeHintBaseHeight;
        }
        // 估算字体的宽度作为宽度
        w = estimateLargeButtonTextWidth(in, h, textHeight, &res.isTextNeedWrap);
        w += (2 * in.spacing);
        // 判断是否需要加上indicator
        if (in.enableWordWrap && res.isTextNeedWrap) {
            w += in.indicatorLen;
        }
        //! Qt6.4 取消了QApplication::globalStrut
        res.sizeHint = QSize(w, h)
                           .expandedTo(QSize(minW, textHeight)
                                           .expandedTo(QSize(TBLC::GLOBAL_STRUT_WIDTH, TBLC::GLOBAL_STRUT_HEIGHT)));
        return res;
    }

    // ---- small button ----
    int w = 0, h = 0;
    res.sizeHintBaseHeight = -1;  // 小按钮的尺寸不依赖panel几何
    res.isTextNeedWrap     = false;

    switch (in.toolButtonStyle) {
    case Qt::ToolButtonIconOnly: {
        w = in.iconSize.width() + 2 * in.spacing;
        h = in.iconSize.height() + 2 * in.spacing;
    } break;
    case Qt::ToolButtonTextOnly: {
        QSize textSize = in.fontMetrics.size(Qt::TextShowMnemonic, simplifiedText(in.text));
        textSize.setWidth(textSize.width() + SA::compat::horizontalAdvance(in.fontMetrics, (QLatin1Char(' '))) * 2);
        textSize.setHeight(res.textDrawRectHeight);
        w = textSize.width() + 2 * in.spacing;
        h = textSize.height() + 2 * in.spacing;
    } break;
    default: {
        // 先加入icon的尺寸
        w = in.iconSize.width() + 2 * in.spacing;
        h = in.iconSize.height() + 2 * in.spacing;
        // 再加入文本的长度
        if (!in.text.isEmpty()) {
            QSize textSize = in.fontMetrics.size(Qt::TextShowMnemonic, simplifiedText(in.text));
            textSize.setWidth(textSize.width() + SA::compat::horizontalAdvance(in.fontMetrics, (QLatin1Char(' '))) * 2);
            textSize.setHeight(res.textDrawRectHeight);
            w += in.spacing;
            w += textSize.width();
            h = qMax(h, (textSize.height() + (2 * in.spacing)));
        } else {
            // 没有文本的时候也要设置一下高度
            QSize textSize = in.fontMetrics.size(Qt::TextShowMnemonic, " ");
            h              = qMax(h, (textSize.height() + (2 * in.spacing)));
        }
    }
    }
    if (in.hasIndicator) {
        // 存在indicator的按钮，宽度尺寸要扩展
        w += in.indicatorLen;
    }
    if (w < TBLC::MIN_BUTTON_WIDTH) {
        w = TBLC::MIN_BUTTON_WIDTH;
    }
    res.sizeHint = QSize(w, h).expandedTo(QSize(TBLC::GLOBAL_STRUT_WIDTH, TBLC::GLOBAL_STRUT_HEIGHT));
    return res;
}

int SARibbonToolButtonLayout::textDrawRectHeight(const Input& in)
{
    if (in.isLargeButton) {
        if (in.enableWordWrap) {
            return in.fontMetrics.lineSpacing() * in.factors.twoLineHeightFactor + in.fontMetrics.leading();
        } else {
            return in.fontMetrics.lineSpacing() * in.factors.oneLineHeightFactor;
        }
    }
    // 小按钮
    return in.rect.height() - TBLC::SMALL_BUTTON_HEIGHT_OFFSET;
}

int SARibbonToolButtonLayout::estimateLargeButtonTextWidth(const Input& in, int buttonHeight, int textHeight, bool* needWrap)
{
    QSize textSize;
    const QFontMetrics& fm = in.fontMetrics;
    int space              = SA::compat::horizontalAdvance(fm, (QLatin1Char(' '))) * 2;
    int hintMaxWidth       = qMin(static_cast< int >(buttonHeight * in.factors.buttonMaximumAspectRatio),
                            in.maximumWidth);  ///< 建议的宽度
    bool textNeedWrap      = false;

    if (in.enableWordWrap) {
        textSize = fm.size(Qt::TextShowMnemonic, in.text);
        textSize.setWidth(textSize.width() + space);

        if (textSize.height() > fm.lineSpacing() * 1.1) {
            //! 说明文字带有换行符，是用户手动换行，这种情况就直接返回字体尺寸，不进行估算
            textNeedWrap = true;  // 文字需要换行显示，标记起来
            if (needWrap) {
                *needWrap = textNeedWrap;
            }
            return textSize.width();
        }

        // 这时候需要估算文本的长度
        if (textSize.width() <= hintMaxWidth) {
            // 范围合理，直接返回
            textNeedWrap = false;  // 文字不需要换行显示，标记起来
            if (needWrap) {
                *needWrap = textNeedWrap;
            }
            return textSize.width();
        }

        //! 大于宽高比尝试进行文字换行
        //! 使用二分查找找到最优宽度
        int alignment = Qt::TextShowMnemonic | Qt::TextWordWrap;
        int minWidth  = textSize.width() / 2;  // 最小尝试宽度（一半）
        int maxWidth  = textSize.width();      // 最大宽度（原始宽度）
        int bestWidth = maxWidth;              // 最佳宽度

        // 两行判定基准用单行实测高度而非 lineSpacing：部分字体（如 Microsoft YaHei UI）的
        // boundingRect 多行高度按单行高度累计（2 行 = 2×单行高），大于 2×lineSpacing，
        // 用 lineSpacing*2 判定会使二分查找永远判为"超过两行"、宽度退化为单行全宽，
        // buttonMaximumAspectRatio 宽度上限对这些字体静默失效
        const int twoLineHeightBudget = 2 * textSize.height() + 2;

        // 二分查找，最多10次迭代
        for (int i = 0; i < 10; ++i) {
            int midWidth = (minWidth + maxWidth) / 2;
            QRect textRect(0, 0, midWidth, textHeight);
            textRect = fm.boundingRect(textRect, alignment, in.text);

            if (textRect.height() <= twoLineHeightBudget) {
                // 可以在两行内显示，尝试更小的宽度
                bestWidth = midWidth;
                maxWidth  = midWidth - 1;
            } else {
                // 需要更多行，尝试更大的宽度
                minWidth = midWidth + 1;
            }

            if (minWidth > maxWidth) {
                break;
            }
        }

        textNeedWrap = true;  // 文字需要换行显示，标记起来
        if (needWrap) {
            *needWrap = textNeedWrap;
        }
        return bestWidth;
    }

    //! 说明是不换行
    textNeedWrap = false;  // 文字不需要换行显示，标记起来
    if (needWrap) {
        *needWrap = textNeedWrap;
    }
    // 文字不换行情况下，做simplified处理
    textSize = fm.size(Qt::TextShowMnemonic, simplifiedText(in.text));
    textSize.setWidth(textSize.width() + space);
    if (textSize.width() < hintMaxWidth) {
        // 范围合理，直接返回
        return textSize.width();
    }
    return hintMaxWidth;
}

SARibbonToolButtonLayout::DrawRectResult SARibbonToolButtonLayout::calcDrawRects(const Input& in, bool isTextNeedWrap)
{
    if (in.isLargeButton) {
        return calcLargeButtonDrawRects(in, isTextNeedWrap);
    }
    return calcSmallButtonDrawRects(in);
}

SARibbonToolButtonLayout::DrawRectResult SARibbonToolButtonLayout::calcSmallButtonDrawRects(const Input& in)
{
    DrawRectResult res;
    QRect& iconRect           = res.iconRect;
    QRect& textRect           = res.textRect;
    QRect& indicatorArrowRect = res.indicatorArrowRect;
    const int spacing         = in.spacing;
    const int indicatorLen    = in.indicatorLen;

    switch (in.toolButtonStyle) {
    case Qt::ToolButtonIconOnly: {
        if (in.hasIndicator) {
            // 在仅有图标的小模式显示时，预留一个下拉箭头位置
            iconRect = in.rect.adjusted(spacing, spacing, -indicatorLen - spacing, -spacing);
            indicatorArrowRect =
                QRect(in.rect.right() - indicatorLen - spacing, iconRect.y(), indicatorLen, iconRect.height());
        } else {
            iconRect           = in.rect.adjusted(spacing, spacing, -spacing, -spacing);
            indicatorArrowRect = QRect();
        }
        // 文本区域为空
        textRect = QRect();
    } break;
    case Qt::ToolButtonTextOnly: {
        if (in.hasIndicator) {
            // 在仅有图标的小模式显示时，预留一个下拉箭头位置
            textRect = in.rect.adjusted(spacing, spacing, -indicatorLen - spacing, -spacing);
            indicatorArrowRect = QRect(in.rect.right() - indicatorLen - spacing, spacing, indicatorLen, textRect.height());
        } else {
            textRect           = in.rect.adjusted(spacing, spacing, -spacing, -spacing);
            indicatorArrowRect = QRect();
        }
        // 绘图区域为空
        iconRect = QRect();
    } break;
    default: {
        bool hasInd = in.hasIndicator;
        // icon Beside和under都是一样的
        QRect buttonRect = in.rect;
        buttonRect.adjust(spacing, spacing, -spacing, -spacing);
        // 先设置IconRect
        if (!in.hasIcon) {
            // 没有图标
            iconRect = QRect();
        } else {
            QSize iconSize = adjustIconSize(buttonRect, in.iconSize);
            iconRect =
                QRect(buttonRect.x(), buttonRect.y(), iconSize.width(), qMax(iconSize.height(), buttonRect.height()));
        }
        // 后设置TextRect
        if (in.text.isEmpty()) {
            textRect = QRect();
        } else {
            // 分有菜单和没菜单两种情况
            int adjx = iconRect.isValid() ? (iconRect.width() + spacing)
                                          : 0;  // 在buttonRect上变换，因此如果没有图标是不用偏移spacing
            if (hasInd) {
                textRect = buttonRect.adjusted(adjx, 0, -indicatorLen, 0);
            } else {
                textRect = buttonRect.adjusted(adjx, 0, 0, 0);  // 在buttonRect上变换，因此如果没有图标是不用偏移spacing
            }
        }
        // 最后设置Indicator
        if (hasInd) {
            if (textRect.isValid()) {
                indicatorArrowRect =
                    QRect(buttonRect.right() - indicatorLen + 1, textRect.y(), indicatorLen, textRect.height());
            } else if (iconRect.isValid()) {
                indicatorArrowRect =
                    QRect(buttonRect.right() - indicatorLen + 1, iconRect.y(), indicatorLen, iconRect.height());
            } else {
                indicatorArrowRect = buttonRect;
            }
        } else {
            indicatorArrowRect = QRect();
        }
    }
    }

    // RTL：指示器移到左侧，图标/文字从右侧开始，x 坐标在按钮宽度内镜像
    mirrorRectsX(in, res);
    return res;
}

SARibbonToolButtonLayout::DrawRectResult SARibbonToolButtonLayout::calcLargeButtonDrawRects(const Input& in,
                                                                                           bool isTextNeedWrap)
{
    //! 3行模式的图标比较大，文字换行情况下，indicator会动态调整
    DrawRectResult res;
    QRect& iconRect           = res.iconRect;
    QRect& textRect           = res.textRect;
    QRect& indicatorArrowRect = res.indicatorArrowRect;
    const int spacing         = in.spacing;
    int indicatorLen          = in.indicatorLen;

    // 先获取文字矩形的高度
    int textHeight = textDrawRectHeight(in);
    bool hIndicator = in.hasIndicator;
    if (!hIndicator) {
        // 没有菜单，把len设置为0
        indicatorLen = 0;
    }

    // 这里要判断文字是否要换行显示，换行显示的文字的indicatorArrowRect所处的位置不一样
    if (Qt::ToolButtonIconOnly == in.toolButtonStyle) {
        // 只有图标
        if (hIndicator) {
            // 如果只有icon，且有indicator，那么indicator在图标下面（注意，这个indicator布局和即有图标和文字是不一样的）
            int indicatorHeight = indicatorHeightOf(indicatorLen);
            // 周边留下spacing距离
            indicatorArrowRect = QRect(in.rect.left() + spacing,
                                       in.rect.bottom() - indicatorHeight - spacing,
                                       in.rect.width() - 2 * spacing,
                                       indicatorHeight);
            // iconRect布满整个按钮
            iconRect = QRect(in.rect.left() + spacing,
                             in.rect.top() + spacing,
                             in.rect.width() - 2 * spacing,
                             in.rect.height() - 2 * spacing - indicatorHeight);
        } else {
            // iconRect布满整个按钮
            iconRect = QRect(in.rect.left() + spacing,
                             in.rect.top() + spacing,
                             in.rect.width() - 2 * spacing,
                             in.rect.height() - 2 * spacing);
        }
    } else if (Qt::ToolButtonTextOnly == in.toolButtonStyle) {
        // 仅有文字，处理方式和仅有图标一样
        if (hIndicator) {
            // 如果只有text，且有indicator，那么indicator在图标下面（注意，这个indicator布局和即有图标和文字是不一样的）
            int indicatorHeight = indicatorHeightOf(indicatorLen);
            // 周边留下spacing距离
            indicatorArrowRect = QRect(in.rect.left() + spacing,
                                       in.rect.bottom() - indicatorHeight - spacing,
                                       in.rect.width() - 2 * spacing,
                                       indicatorHeight);
            // textRect布满整个按钮
            textRect = QRect(in.rect.left() + spacing,
                             in.rect.top() + spacing,
                             in.rect.width() - 2 * spacing,
                             in.rect.height() - 2 * spacing - indicatorHeight);
        } else {
            // textRect布满整个按钮
            textRect = QRect(in.rect.left() + spacing,
                             in.rect.top() + spacing,
                             in.rect.width() - 2 * spacing,
                             in.rect.height() - 2 * spacing);
        }
    } else {
        // 先布置textRect
        if (in.enableWordWrap) {
            // 在换行模式下
            if (isTextNeedWrap) {
                // 如果文字的确换行，indicator放在最右边
                textRect = QRect(in.rect.left() + spacing,
                                 in.rect.bottom() - spacing - textHeight,
                                 in.rect.width() - 2 * spacing - indicatorLen,  // 注意，这里会减去indicatorLen的宽度
                                 textHeight);
                if (hIndicator) {
                    // indicator在文字的右边
                    indicatorArrowRect =
                        QRect(textRect.right() + 1, textRect.y() + textRect.height() / 2, indicatorLen, textHeight / 2);
                }
            } else {
                // 如果文字不需要换行，由于文字下面会有一行的空白，因此indicator布局在文字下面
                textRect = QRect(in.rect.left() + spacing,
                                 in.rect.bottom() - spacing - textHeight,
                                 in.rect.width() - 2 * spacing,
                                 textHeight);
                if (hIndicator) {
                    int dy = textRect.height() / 2;
                    dy += (dy - indicatorLen) / 2;
                    indicatorArrowRect = QRect(textRect.left(), textRect.top() + dy, textRect.width(), indicatorLen);
                }
            }
        } else {
            // 文字不换行，indicator放在最右边
            int y = in.rect.bottom() - spacing - textHeight;
            if (hIndicator) {
                // 先布置indicator
                indicatorArrowRect = QRect(in.rect.right() - indicatorLen - spacing, y, indicatorLen, textHeight);
                textRect           = QRect(spacing, y, indicatorArrowRect.x() - spacing, textHeight);
            } else {
                textRect = QRect(in.rect.left() + spacing, y, in.rect.width() - 2 * spacing, textHeight);
            }
        }
        // 剩下就是icon区域
        iconRect = QRect(in.rect.left() + spacing, spacing, in.rect.width() - 2 * spacing, textRect.top() - 2 * spacing);
    }

    // RTL：指示器移到左下角，文字右对齐；全宽矩形对称，镜像保持居中
    mirrorRectsX(in, res);
    return res;
}

void SARibbonToolButtonLayout::mirrorRectsX(const Input& in, DrawRectResult& res)
{
    if (!in.isRTL) {
        return;
    }
    const int containerWidth = in.rect.width();
    const int rectLeft       = in.rect.x();
    auto mirror              = [containerWidth, rectLeft](QRect& r) {
        if (!r.isValid()) {
            return;
        }
        int relX      = r.x() - rectLeft;
        int mirroredX = SA::saMirrorX(relX, containerWidth, r.width());
        r.moveLeft(rectLeft + mirroredX);
    };
    mirror(res.iconRect);
    mirror(res.textRect);
    mirror(res.indicatorArrowRect);
}

int SARibbonToolButtonLayout::textAlignment(const Input& in)
{
    if (Qt::ToolButtonTextOnly == in.toolButtonStyle) {
        return Qt::TextShowMnemonic | Qt::AlignCenter;
    }

    if (in.isLargeButton) {
        return Qt::TextShowMnemonic
               | (in.enableWordWrap ? (Qt::TextWordWrap | Qt::AlignTop | Qt::AlignHCenter) : Qt::AlignCenter);
    }

    return Qt::TextShowMnemonic | Qt::AlignCenter;
}

QSize SARibbonToolButtonLayout::adjustIconSize(const QRect& buttonRect, const QSize& originIconSize)
{
    // 边界检查
    if (buttonRect.isEmpty() || originIconSize.isEmpty()) {
        return QSize(0, 0);
    }

    QSize iconSize = originIconSize;

    // 如果图标已经小于等于按钮区域，则直接返回
    if (iconSize.width() <= buttonRect.width() && iconSize.height() <= buttonRect.height()) {
        return iconSize;
    }

    // 计算宽高比
    qreal aspectRatio = static_cast< qreal >(originIconSize.width()) / originIconSize.height();

    // 先按按钮高度调整
    if (iconSize.height() > buttonRect.height()) {
        iconSize.setHeight(buttonRect.height());
        iconSize.setWidth(qRound(buttonRect.height() * aspectRatio));
    }

    // 再检查宽度是否超限
    if (iconSize.width() > buttonRect.width()) {
        iconSize.setWidth(buttonRect.width());
        iconSize.setHeight(qRound(buttonRect.width() / aspectRatio));
    }

    // 确保不会超过按钮边界
    iconSize.setWidth(qMin(iconSize.width(), buttonRect.width()));
    iconSize.setHeight(qMin(iconSize.height(), buttonRect.height()));

    return iconSize;
}

int SARibbonToolButtonLayout::indicatorHeight(int indicatorLen)
{
    return indicatorHeightOf(indicatorLen);
}

int SARibbonToolButtonLayout::indicatorHeightOf(int indicatorLen)
{
    return static_cast< int >(indicatorLen * TBLC::INDICATOR_HEIGHT_FACTOR_NUM / TBLC::INDICATOR_HEIGHT_FACTOR_DEN);
}

QString SARibbonToolButtonLayout::simplifiedText(const QString& str)
{
    QString res = str;
    res.remove('\n');
    return res;
}

}
}
