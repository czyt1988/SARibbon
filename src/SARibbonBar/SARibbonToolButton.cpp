#include "SARibbonToolButton.h"
#include "SARibbonPanel.h"

#include <QAction>
#include <QApplication>
#include <QCursor>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QStyleOption>
#include <QStyleOptionFocusRect>
#include <QStyleOptionToolButton>
#include <QStylePainter>
#include <QTextOption>
#include <QApplication>
#include <QScreen>
#include <QProxyStyle>

#include "SARibbonUtil.h"

/**
 * @def 开启此宏会打印一些常见信息
 */
#ifndef SA_RIBBON_TOOLBUTTON_DEBUG_PRINT
#define SA_RIBBON_TOOLBUTTON_DEBUG_PRINT 0
#endif

#ifndef SARIBBONTOOLBUTTON_DEBUG_DRAW
#define SARIBBONTOOLBUTTON_DEBUG_DRAW 0
#endif

#if SARIBBONTOOLBUTTON_DEBUG_DRAW
#ifndef SARIBBONTOOLBUTTON_DEBUG_DRAW_RECT
#define SARIBBONTOOLBUTTON_DEBUG_DRAW_RECT(p, rect)                                                                    \
    do {                                                                                                               \
        p.save();                                                                                                      \
        p.setPen(Qt::red);                                                                                             \
        p.setBrush(QBrush());                                                                                          \
        p.drawRect(rect);                                                                                              \
        p.restore();                                                                                                   \
    } while (0)
#endif
#else
#ifndef SARIBBONTOOLBUTTON_DEBUG_DRAW_RECT
#define SARIBBONTOOLBUTTON_DEBUG_DRAW_RECT(p, rect)
#endif
#endif
namespace SA
{

QDebug operator<<(QDebug debug, const QStyleOptionToolButton& opt)
{
    debug << "==============" << "\nQStyleOption(" << (QStyleOption)opt << ")"
          << "\n  QStyleOptionComplex:"
             "\n     subControls("
          << opt.subControls
          << " ) "
             "\n     activeSubControls("
          << opt.activeSubControls
          << "\n  QStyleOptionToolButton"
             "\n     features("
          << opt.features
          << ")"
             "\n     toolButtonStyle("
          << opt.toolButtonStyle << ")";

    return (debug);
}
}

//===================================================
// SARibbonToolButtonProxyStyle
//===================================================

class SARibbonToolButtonProxyStyle : public QProxyStyle
{
public:
    void drawPrimitive(PrimitiveElement pe, const QStyleOption* opt, QPainter* p, const QWidget* widget = nullptr) const override
    {
        if (pe == PE_IndicatorArrowUp || pe == PE_IndicatorArrowDown || pe == PE_IndicatorArrowRight
            || pe == PE_IndicatorArrowLeft) {
            if (opt->rect.width() <= 1 || opt->rect.height() <= 1) {
                return;
            }

            QRect r  = opt->rect;
            int size = qMin(r.height(), r.width());
            QPixmap pixmap;
            qreal pixelRatio = p->device()->devicePixelRatio();
            int border       = qRound(pixelRatio * (size / 4));
            int sqsize       = qRound(pixelRatio * (2 * (size / 2)));
            QImage image(sqsize, sqsize, QImage::Format_ARGB32_Premultiplied);
            image.fill(Qt::transparent);
            QPainter imagePainter(&image);

            QPolygon a;
            switch (pe) {
            case PE_IndicatorArrowUp:
                a.setPoints(3, border, sqsize / 2, sqsize / 2, border, sqsize - border, sqsize / 2);
                break;
            case PE_IndicatorArrowDown:
                a.setPoints(3, border, sqsize / 2, sqsize / 2, sqsize - border, sqsize - border, sqsize / 2);
                break;
            case PE_IndicatorArrowRight:
                a.setPoints(3, sqsize - border, sqsize / 2, sqsize / 2, border, sqsize / 2, sqsize - border);
                break;
            case PE_IndicatorArrowLeft:
                a.setPoints(3, border, sqsize / 2, sqsize / 2, border, sqsize / 2, sqsize - border);
                break;
            default:
                break;
            }

            int bsx = 0;
            int bsy = 0;

            if (opt->state & State_Sunken) {
                bsx = proxy()->pixelMetric(PM_ButtonShiftHorizontal, opt, widget);
                bsy = proxy()->pixelMetric(PM_ButtonShiftVertical, opt, widget);
            }

            QRect bounds = a.boundingRect();
            int sx       = sqsize / 2 - bounds.center().x() - 1;
            int sy       = sqsize / 2 - bounds.center().y() - 1;
            imagePainter.translate(sx + bsx, sy + bsy);
            imagePainter.setPen(QPen(opt->palette.buttonText().color(), 1.4));
            imagePainter.setBrush(Qt::NoBrush);

            if (!(opt->state & State_Enabled)) {
                imagePainter.translate(1, 1);
                imagePainter.setPen(QPen(opt->palette.light().color(), 1.4));
                imagePainter.drawPolyline(a);
                imagePainter.translate(-1, -1);
                imagePainter.setPen(QPen(opt->palette.mid().color(), 1.4));
            }

            imagePainter.drawPolyline(a);
            imagePainter.end();
            pixmap = QPixmap::fromImage(image);
            pixmap.setDevicePixelRatio(pixelRatio);

            int xOffset = r.x() + (r.width() - size) / 2;
            int yOffset = r.y() + (r.height() - size) / 2;
            p->drawPixmap(xOffset, yOffset, pixmap);
        } else {
            QProxyStyle::drawPrimitive(pe, opt, p, widget);
        }
    }
};

//===================================================
// SARibbonToolButton::PrivateData
//===================================================

class SARibbonToolButton::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonToolButton)
public:
    PrivateData(SARibbonToolButton* p);
    // 根据鼠标位置更新按钮的信息
    void updateStatusByMousePosition(const QPoint& pos);
    // 更新绘图相关的尺寸
    void updateDrawRect(const QStyleOptionToolButton& opt);
    // 更新SizeHint
    void updateSizeHint(const QStyleOptionToolButton& opt);
    // 计算涉及到的rect尺寸
    void calcDrawRects(const QStyleOptionToolButton& opt,
                       QRect& iconRect,
                       QRect& textRect,
                       QRect& indicatorArrowRect,
                       int spacing,
                       int indicatorLen) const;
    // 计算小按钮模式下的尺寸
    void calcSmallButtonDrawRects(const QStyleOptionToolButton& opt,
                                  QRect& iconRect,
                                  QRect& textRect,
                                  QRect& indicatorArrowRect,
                                  int spacing,
                                  int indicatorLen) const;
    // 计算大按钮模式下的尺寸
    void calcLargeButtonDrawRects(const QStyleOptionToolButton& opt,
                                  QRect& iconRect,
                                  QRect& textRect,
                                  QRect& indicatorArrowRect,
                                  int spacing,
                                  int indicatorLen) const;

    // 根据按钮的尺寸调节iconsize(注意这里的buttonRect是已经减去mSpacing的情况)
    QSize adjustIconSize(const QRect& buttonRect, const QSize& originIconSize) const;
    // 判断是否有Indicator
    bool hasIndicator(const QStyleOptionToolButton& opt) const;
    // 计算sizehint
    QSize calcSizeHint(const QStyleOptionToolButton& opt);
    QSize calcSmallButtonSizeHint(const QStyleOptionToolButton& opt);
    QSize calcLargeButtonSizeHint(const QStyleOptionToolButton& opt);

    // 计算文本绘制矩形的高度
    int calcTextDrawRectHeight(const QStyleOptionToolButton& opt) const;
    // 估算一个最优的文本宽度
    int estimateLargeButtonTextWidth(int buttonHeight,
                                     int textDrawRectHeight,
                                     const QString& text,
                                     const QFontMetrics& fm,
                                     int maxTrycount = 3);
    QPixmap createIconPixmap(const QStyleOptionToolButton& opt, const QSize& iconsize) const;
    // 获取文字的对其方式
    int getTextAlignment() const;
    // 确认文字是否确切要换行显示
    bool isTextNeedWrap() const;
    // 获取真实的icon尺寸
    QSize realIconSize() const;
    // 仅仅对\n进行剔除，和QString::simplified不一样
    static QString simplifiedForRibbonButton(const QString& str);

public:
    bool mMouseOnSubControl { false };  ///< 这个用于标记MenuButtonPopup模式下，鼠标在文本区域
    bool mMenuButtonPressed { false };  ///< 由于Indicator改变，因此hitButton不能用QToolButton的hitButton
    bool mWordWrap { true };            ///< 标记是否文字换行 @default false
    SARibbonToolButton::RibbonButtonType mButtonType { SARibbonToolButton::LargeButton };
    int mSpacing { 1 };                                      ///< 按钮和边框的距离
    int mIndicatorLen { 8 };                                 ///< Indicator的长度
    QRect mDrawIconRect;                                     ///< 记录icon的绘制位置
    QRect mDrawTextRect;                                     ///< 记录text的绘制位置
    QRect mDrawIndicatorArrowRect;                           ///< 记录IndicatorArrow的绘制位置
    QSize mSizeHint;                                         ///< 保存计算好的sizehint
    QSize mLargeButtonSizeHint;                              ///< 大按钮的尺寸
    bool mIsTextNeedWrap { false };                          ///< 标记文字是否需要换行显示
    SARibbonToolButton::LayoutFactor layoutFactor;           ///< 布局系数
    std::unique_ptr< SARibbonToolButtonProxyStyle > mStyle;  ///< 按钮样式，主要为了绘制箭头
};

SARibbonToolButton::PrivateData::PrivateData(SARibbonToolButton* p) : q_ptr(p)
{
    mStyle = std::make_unique< SARibbonToolButtonProxyStyle >();
}

/**
 * @brief 根据鼠标的位置更新状态，主要用于判断鼠标是否位于subcontrol
 *
 * 此函数主要应用在action menu模式下
 * @param pos
 */
void SARibbonToolButton::PrivateData::updateStatusByMousePosition(const QPoint& pos)
{
    bool isMouseOnSubControl(false);
    if (SARibbonToolButton::LargeButton == mButtonType) {
        isMouseOnSubControl = mDrawTextRect.united(mDrawIndicatorArrowRect).contains(pos);
    } else {
        // 小按钮模式就和普通toolbutton一样
        isMouseOnSubControl = mDrawIndicatorArrowRect.contains(pos);
    }

    if (mMouseOnSubControl != isMouseOnSubControl) {
        mMouseOnSubControl = isMouseOnSubControl;
        // 从icon变到text过程中刷新一次
        q_ptr->update();
    }
}

/**
 * @brief 更新绘图的几个关键尺寸
 *
 * 包括：
 *
 * - DrawIconRect 绘制图标的矩形区域
 *
 * - DrawTextRect 绘制文本的矩形区域
 *
 * - DrawIndicatorArrowRect 绘制菜单下箭头的矩形区域
 *
 * @param opt
 */
void SARibbonToolButton::PrivateData::updateDrawRect(const QStyleOptionToolButton& opt)
{
    if (!mSizeHint.isValid()) {
        updateSizeHint(opt);
    }
    // 先更新IndicatorLen
    mIndicatorLen = q_ptr->style()->pixelMetric(QStyle::PM_MenuButtonIndicator, &opt, q_ptr);
    if (mIndicatorLen < 3) {
        if (SARibbonToolButton::LargeButton == mButtonType) {
            mIndicatorLen = 8;
        } else {
            mIndicatorLen = 12;  // 小按钮模式下设置为10
        }
    }
    calcDrawRects(opt, mDrawIconRect, mDrawTextRect, mDrawIndicatorArrowRect, mSpacing, mIndicatorLen);
}

/**
 * @brief 更新sizehint
 * @param opt
 */
void SARibbonToolButton::PrivateData::updateSizeHint(const QStyleOptionToolButton& opt)
{
    mSizeHint = calcSizeHint(opt);
}

/**
 * @brief 计算绘图的几个关键区域
 * @param opt
 * @param iconRect  绘制图标的矩形区域
 * @param textRect 绘制文本的矩形区域
 * @param indicatorArrowRect 绘制菜单下箭头的矩形区域
 * @param spacing
 * @param indicatorLen
 */
void SARibbonToolButton::PrivateData::calcDrawRects(const QStyleOptionToolButton& opt,
                                                    QRect& iconRect,
                                                    QRect& textRect,
                                                    QRect& indicatorArrowRect,
                                                    int spacing,
                                                    int indicatorLen) const
{
    if (SARibbonToolButton::LargeButton == mButtonType) {
        calcLargeButtonDrawRects(opt, iconRect, textRect, indicatorArrowRect, spacing, indicatorLen);

    } else {
        calcSmallButtonDrawRects(opt, iconRect, textRect, indicatorArrowRect, spacing, indicatorLen);
    }
}

/**
 * @brief 计算小按钮模式下的绘制尺寸
 * @param opt
 * @param iconRect
 * @param textRect
 * @param indicatorArrowRect
 * @param spacing
 * @param indicatorLen
 */
void SARibbonToolButton::PrivateData::calcSmallButtonDrawRects(const QStyleOptionToolButton& opt,
                                                               QRect& iconRect,
                                                               QRect& textRect,
                                                               QRect& indicatorArrowRect,
                                                               int spacing,
                                                               int indicatorLen) const
{
    switch (opt.toolButtonStyle) {
    case Qt::ToolButtonIconOnly: {
        if (hasIndicator(opt)) {
            // 在仅有图标的小模式显示时，预留一个下拉箭头位置
            iconRect = opt.rect.adjusted(spacing, spacing, -indicatorLen - spacing, -spacing);
            indicatorArrowRect =
                QRect(opt.rect.right() - indicatorLen - spacing, iconRect.y(), indicatorLen, iconRect.height());
        } else {
            iconRect           = opt.rect.adjusted(spacing, spacing, -spacing, -spacing);
            indicatorArrowRect = QRect();
        }
        // 文本区域为空
        textRect = QRect();
    } break;
    case Qt::ToolButtonTextOnly: {
        if (hasIndicator(opt)) {
            // 在仅有图标的小模式显示时，预留一个下拉箭头位置
            textRect = opt.rect.adjusted(spacing, spacing, -indicatorLen - spacing, -spacing);
            indicatorArrowRect = QRect(opt.rect.right() - indicatorLen - spacing, spacing, indicatorLen, textRect.height());
        } else {
            textRect           = opt.rect.adjusted(spacing, spacing, -spacing, -spacing);
            indicatorArrowRect = QRect();
        }
        // 绘图区域为空
        iconRect = QRect();
    } break;
    default: {
        bool hasInd = hasIndicator(opt);
        // icon Beside和under都是一样的
        QRect buttonRect = q_ptr->rect();
        buttonRect.adjust(spacing, spacing, -spacing, -spacing);
        // 先设置IconRect
        if (opt.icon.isNull()) {
            // 没有图标
            iconRect = QRect();
        } else {
            QSize iconSize = adjustIconSize(buttonRect, opt.iconSize);
            iconRect =
                QRect(buttonRect.x(), buttonRect.y(), iconSize.width(), qMax(iconSize.height(), buttonRect.height()));
        }
        // 后设置TextRect
        if (opt.text.isEmpty()) {
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
}

/**
 * @brief 计算大按钮模式下的绘制尺寸（普通）
 * @param opt
 * @param iconRect
 * @param textRect
 * @param indicatorArrowRect
 * @param spacing
 * @param indicatorLen
 */
void SARibbonToolButton::PrivateData::calcLargeButtonDrawRects(const QStyleOptionToolButton& opt,
                                                               QRect& iconRect,
                                                               QRect& textRect,
                                                               QRect& indicatorArrowRect,
                                                               int spacing,
                                                               int indicatorLen) const
{
    //! 3行模式的图标比较大，文字换行情况下，indicator会动态调整

    // 初始化
    iconRect           = QRect();
    textRect           = QRect();
    indicatorArrowRect = QRect();

    // 先获取文字矩形的高度
    int textHeight  = calcTextDrawRectHeight(opt);
    bool hIndicator = hasIndicator(opt);
    if (!hIndicator) {
        // 没有菜单，把len设置为0
        indicatorLen = 0;
    }

    // 这里要判断文字是否要换行显示，换行显示的文字的indicatorArrowRect所处的位置不一样
    if (Qt::ToolButtonIconOnly == opt.toolButtonStyle) {
        // 只有图标
        if (hIndicator) {
            // 如果只有icon，且有indicator，那么indicator在图标下面（注意，这个indicator布局和即有图标和文字是不一样的）
            int indicatorHeight = static_cast< int >(indicatorLen * 1.2);
            // 周边留下spacing距离
            indicatorArrowRect = QRect(opt.rect.left() + spacing,
                                       opt.rect.bottom() - indicatorHeight - spacing,
                                       opt.rect.width() - 2 * spacing,
                                       indicatorHeight);
            // iconRect布满整个按钮
            iconRect = QRect(opt.rect.left() + spacing,
                             opt.rect.top() + spacing,
                             opt.rect.width() - 2 * spacing,
                             opt.rect.height() - 2 * spacing - indicatorHeight);
        } else {
            // iconRect布满整个按钮
            iconRect = QRect(opt.rect.left() + spacing,
                             opt.rect.top() + spacing,
                             opt.rect.width() - 2 * spacing,
                             opt.rect.height() - 2 * spacing);
        }
    } else if (Qt::ToolButtonTextOnly == opt.toolButtonStyle) {
        // 仅有文字，处理方式和仅有图标一样
        if (hIndicator) {
            // 如果只有text，且有indicator，那么indicator在图标下面（注意，这个indicator布局和即有图标和文字是不一样的）
            int indicatorHeight = static_cast< int >(indicatorLen * 1.2);
            // 周边留下spacing距离
            indicatorArrowRect = QRect(opt.rect.left() + spacing,
                                       opt.rect.bottom() - indicatorHeight - spacing,
                                       opt.rect.width() - 2 * spacing,
                                       indicatorHeight);
            // textRect布满整个按钮
            textRect = QRect(opt.rect.left() + spacing,
                             opt.rect.top() + spacing,
                             opt.rect.width() - 2 * spacing,
                             opt.rect.height() - 2 * spacing - indicatorHeight);
        } else {
            // textRect布满整个按钮
            textRect = QRect(opt.rect.left() + spacing,
                             opt.rect.top() + spacing,
                             opt.rect.width() - 2 * spacing,
                             opt.rect.height() - 2 * spacing);
        }
    } else {
        // 先布置textRect
        if (q_ptr->isEnableWordWrap()) {
            // 在换行模式下
            if (isTextNeedWrap()) {
                // 如果文字的确换行，indicator放在最右边
                textRect = QRect(opt.rect.left() + spacing,
                                 opt.rect.bottom() - spacing - textHeight,
                                 opt.rect.width() - 2 * spacing - indicatorLen,  // 注意，这里会减去indicatorLen的宽度
                                 textHeight);
                if (hIndicator) {
                    // indicator在文字的右边
                    indicatorArrowRect =
                        QRect(textRect.right(), textRect.y() + textRect.height() / 2, indicatorLen, textHeight / 2);
                }
            } else {
                // 如果文字不需要换行，由于文字下面会有一行的空白，因此indicator布局在文字下面
                textRect = QRect(opt.rect.left() + spacing,
                                 opt.rect.bottom() - spacing - textHeight,
                                 opt.rect.width() - 2 * spacing,
                                 textHeight);
                if (hIndicator) {
                    int dy = textRect.height() / 2;
                    dy += (dy - indicatorLen) / 2;
                    indicatorArrowRect = QRect(textRect.left(), textRect.top() + dy, textRect.width(), indicatorLen);
                }
            }
        } else {
            // 文字不换行，indicator放在最右边
            int y = opt.rect.bottom() - spacing - textHeight;
            if (hIndicator) {
                // 先布置indicator
                indicatorArrowRect = QRect(opt.rect.right() - indicatorLen - spacing, y, indicatorLen, textHeight);
                textRect           = QRect(spacing, y, indicatorArrowRect.x() - spacing, textHeight);
            } else {
                textRect = QRect(opt.rect.left() + spacing, y, opt.rect.width() - 2 * spacing, textHeight);
            }
        }
        // 剩下就是icon区域
        iconRect = QRect(spacing, spacing, opt.rect.width() - 2 * spacing, textRect.top() - 2 * spacing);
    }
}

/**
 * @brief 适应iconsize
 *
 * 此函数会让originIconSize尽量适配buttonRect的大小
 * @param buttonRect
 * @param originIconSize
 * @return
 */
QSize SARibbonToolButton::PrivateData::adjustIconSize(const QRect& buttonRect, const QSize& originIconSize) const
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

/**
 * @brief 判断是否有Indicator
 * @param opt
 * @return
 */
bool SARibbonToolButton::PrivateData::hasIndicator(const QStyleOptionToolButton& opt) const
{
    return ((opt.features & QStyleOptionToolButton::MenuButtonPopup) || (opt.features & QStyleOptionToolButton::HasMenu));
}

/**
 * @brief 计算sizehint
 *
 * 此函数非常关键，因为所有尺寸计算都是基于原始的rect来的
 * @param opt
 * @return
 */
QSize SARibbonToolButton::PrivateData::calcSizeHint(const QStyleOptionToolButton& opt)
{
    if (SARibbonToolButton::LargeButton == mButtonType) {
        return calcLargeButtonSizeHint(opt);
    }
    return calcSmallButtonSizeHint(opt);
}

QSize SARibbonToolButton::PrivateData::calcSmallButtonSizeHint(const QStyleOptionToolButton& opt)
{
    int w = 0, h = 0;

    switch (opt.toolButtonStyle) {
    case Qt::ToolButtonIconOnly: {
        w = opt.iconSize.width() + 2 * mSpacing;
        h = opt.iconSize.height() + 2 * mSpacing;
    } break;
    case Qt::ToolButtonTextOnly: {
        QSize textSize = opt.fontMetrics.size(Qt::TextShowMnemonic, simplifiedForRibbonButton(opt.text));
        textSize.setWidth(textSize.width() + SA_FONTMETRICS_WIDTH(opt.fontMetrics, (QLatin1Char(' '))) * 2);
        textSize.setHeight(calcTextDrawRectHeight(opt));
        w = textSize.width() + 2 * mSpacing;
        h = textSize.height() + 2 * mSpacing;
    } break;
    default: {
        // 先加入icon的尺寸
        w = opt.iconSize.width() + 2 * mSpacing;
        h = opt.iconSize.height() + 2 * mSpacing;
        // 再加入文本的长度
        if (!opt.text.isEmpty()) {
            QSize textSize = opt.fontMetrics.size(Qt::TextShowMnemonic, simplifiedForRibbonButton(opt.text));
            textSize.setWidth(textSize.width() + SA_FONTMETRICS_WIDTH(opt.fontMetrics, (QLatin1Char(' '))) * 2);
            textSize.setHeight(calcTextDrawRectHeight(opt));
            w += mSpacing;
            w += textSize.width();
            h = qMax(h, (textSize.height() + (2 * mSpacing)));
        } else {
            // 没有文本的时候也要设置一下高度
            QSize textSize = opt.fontMetrics.size(Qt::TextShowMnemonic, " ");
            h              = qMax(h, (textSize.height() + (2 * mSpacing)));
        }
    }
    }
    if (hasIndicator(opt)) {
        // 存在indicator的按钮，宽度尺寸要扩展
        w += mIndicatorLen;
    }
    if (w < 16) {
        w = 16;
    }
    //! Qt6.4 取消了QApplication::globalStrut
    return QSize(w, h).expandedTo(QSize(2, 2));
}

QSize SARibbonToolButton::PrivateData::calcLargeButtonSizeHint(const QStyleOptionToolButton& opt)
{
    int w    = 0;
    int h    = opt.fontMetrics.lineSpacing() * 4.8;  // 3*1.6
    int minW = h * 0.75;  // 最小宽度，在panel里面的按钮，最小宽度要和icon适应

    if (SARibbonPanel* panel = qobject_cast< SARibbonPanel* >(q_ptr->parent())) {
        // 对于建立在SARibbonPanel的基础上的大按钮，把高度设置为SARibbonPanel计算的大按钮高度
        h = panel->largeButtonHeight();
    }
    int textHeight = calcTextDrawRectHeight(opt);
    // 估算字体的宽度作为宽度
    w = estimateLargeButtonTextWidth(h, textHeight, opt.text, opt.fontMetrics);
    w += (2 * mSpacing);
    // 判断是否需要加上indicator
    if (q_ptr->isEnableWordWrap() && isTextNeedWrap()) {
        w += mIndicatorLen;
    }

#if SA_RIBBON_TOOLBUTTON_DEBUG_PRINT
    qDebug() << "| | |-SARibbonToolButton::PrivateData::calcLargeButtonSizeHint,text=" << opt.text
             << "\n| | | |-lineSpacing*4.5=" << opt.fontMetrics.lineSpacing() * 4.5  //
             << "\n| | | |-textHeight=" << textHeight                                //
             << "\n| | | |-mDrawIconRect=" << mDrawIconRect                          //
             << "\n| | | |-minW=" << minW                                            //
             << "\n| | | |-w=" << w                                                  //
        ;
#endif
    //! Qt6.4 取消了QApplication::globalStrut
    return QSize(w, h).expandedTo(QSize(minW, textHeight));
}

/**
 * @brief 计算文本高度
 * @param opt
 * @return
 */
int SARibbonToolButton::PrivateData::calcTextDrawRectHeight(const QStyleOptionToolButton& opt) const
{
    if (SARibbonToolButton::LargeButton == mButtonType) {
        if (q_ptr->isEnableWordWrap()) {
            return opt.fontMetrics.lineSpacing() * layoutFactor.twoLineHeightFactor + opt.fontMetrics.leading();
        } else {
            return opt.fontMetrics.lineSpacing() * layoutFactor.oneLineHeightFactor;
        }
    }
    // 小按钮
    return opt.rect.height() - 2;
}

/**
 * @brief 估算一个最优的文字尺寸，在可以换行的情况下会进行换行，且只会换一行
 * @param buttonHeight 按钮的高度
 * @param textDrawRectHeight 文本绘制的高度
 * @param fm QFontMetrics
 * @param widthHeightRatio 宽高比，宽度/高度的比值，如果大于这个比值，则会进行尝试换行以获取更低的宽度
 * @param maxTrycount 尝试次数
 * @return
 */
int SARibbonToolButton::PrivateData::estimateLargeButtonTextWidth(int buttonHeight,
                                                                  int textDrawRectHeight,
                                                                  const QString& text,
                                                                  const QFontMetrics& fm,
                                                                  int maxTrycount)
{
    QSize textSize;
    int space        = SA_FONTMETRICS_WIDTH(fm, (QLatin1Char(' '))) * 2;
    int hintMaxWidth = qMin(static_cast< int >(buttonHeight * layoutFactor.buttonMaximumAspectRatio),
                            q_ptr->maximumWidth());  ///< 建议的宽度
    if (q_ptr->isEnableWordWrap()) {
        textSize = fm.size(Qt::TextShowMnemonic, text);
        textSize.setWidth(textSize.width() + space);

        if (textSize.height() > fm.lineSpacing() * 1.1) {
            //! 说明文字带有换行符，是用户手动换行，这种情况就直接返回字体尺寸，不进行估算
            mIsTextNeedWrap = true;  // 文字需要换行显示，标记起来
            return textSize.width();
        }

        // 这时候需要估算文本的长度
        if (textSize.width() <= hintMaxWidth) {
            // 范围合理，直接返回
            mIsTextNeedWrap = false;  // 文字不需要换行显示，标记起来
            return textSize.width();
        }

        //! 大于宽高比尝试进行文字换行
        //! 这里先对文本长度逐渐加长估算，一直到和原来长度一致为止
        int trycount  = 0;
        int alignment = Qt::TextShowMnemonic | Qt::TextWordWrap;
        // 对于英文字体，直接宽度减半是无法满足完全显示两行的，需要进行预估
        QRect textRect(0, 0, textSize.width(), textDrawRectHeight);
        do {
            //! 先计算两行文本的紧凑矩形
            //! 从一半开始逐渐递增
            //! 第1次为 w/2 + w/2 * (0/3)
            //! 第2次为 w/2 + w/2 * (1/3)
            //! 第3次为 w/2 + w/2 * (2/3)
            textRect.setWidth(textSize.width() / 2 + (textSize.width() / 2) * (float(trycount) / maxTrycount));
            textRect = fm.boundingRect(textRect, alignment, text);
            if (textRect.height() <= (fm.lineSpacing() * 2)) {
                // 保证在两行
                mIsTextNeedWrap = true;  // 文字需要换行显示，标记起来
                return textRect.width();
            }
            ++trycount;
#if SARIBBONTOOLBUTTON_DEBUG_DRAW
            if (trycount > 1) {
                qDebug() << "estimateLargeButtonTextWidth,origin textSize=" << textSize << ",trycount=" << trycount
                         << ",textRect=" << textRect;
            }
#endif
        } while (trycount < 3);
        // 到这里说明前面的尝试失败，最终使用原始的长度
        return textSize.width();
    }

    //! 说明是不换行

    mIsTextNeedWrap = false;  // 文字不需要换行显示，标记起来
                              // 文字不换行情况下，做simplified处理
    textSize = fm.size(Qt::TextShowMnemonic, simplifiedForRibbonButton(text));
    textSize.setWidth(textSize.width() + space);
    if (textSize.width() < hintMaxWidth) {
        // 范围合理，直接返回
        return textSize.width();
    }
    return hintMaxWidth;
}

QPixmap SARibbonToolButton::PrivateData::createIconPixmap(const QStyleOptionToolButton& opt, const QSize& iconsize) const
{
    if (opt.icon.isNull()) {  // 没有有图标
        return (QPixmap());
    }
    QIcon::State state = (opt.state & QStyle::State_On) ? QIcon::On : QIcon::Off;
    QIcon::Mode mode;
    if (!(opt.state & QStyle::State_Enabled)) {
        mode = QIcon::Disabled;
    } else if ((opt.state & QStyle::State_MouseOver) && (opt.state & QStyle::State_AutoRaise)) {
        mode = QIcon::Active;
    } else {
        mode = QIcon::Normal;
    }
    return SA::iconToPixmap(opt.icon, iconsize, SA::widgetDevicePixelRatio(q_ptr), mode, state);
}

int SARibbonToolButton::PrivateData::getTextAlignment() const
{
    if (q_ptr->toolButtonStyle() == Qt::ToolButtonTextOnly) {
        return Qt::TextShowMnemonic | Qt::AlignCenter;
    }

    if (SARibbonToolButton::LargeButton == mButtonType) {
        return Qt::TextShowMnemonic
               | (q_ptr->isEnableWordWrap() ? (Qt::TextWordWrap | Qt::AlignTop | Qt::AlignHCenter) : Qt::AlignCenter);
    }

    return Qt::TextShowMnemonic | Qt::AlignCenter;
}

/**
 * @brief 确认文字是否确切要换行显示
 * @return
 */
bool SARibbonToolButton::PrivateData::isTextNeedWrap() const
{
    return mIsTextNeedWrap;
}

/**
 * @brief 获取正真的icon尺寸
 * @return
 */
QSize SARibbonToolButton::PrivateData::realIconSize() const
{
    if (mButtonType == SARibbonToolButton::LargeButton) {
        return mLargeButtonSizeHint;
    }
    return q_ptr->smallIconSize();
}

/**
 * @brief 仅仅对\n进行剔除
 * @param str
 * @return
 */
QString SARibbonToolButton::PrivateData::simplifiedForRibbonButton(const QString& str)
{
    QString res = str;
    res.remove('\n');
    return res;
}

//===================================================
// SARibbonToolButton
//===================================================

SARibbonToolButton::SARibbonToolButton(QWidget* parent)
    : QToolButton(parent), d_ptr(new SARibbonToolButton::PrivateData(this))
{
    // 静态设置也是可以，虽然节省内存，但不清楚未来qt是否会针对绘制有潜在的多线程处理的可能性，因此这里还是使用成员变量
    // static SARibbonToolButtonProxyStyle* ss_style = new SARibbonToolButtonProxyStyle();
    // setStyle(ss_style);

    // setStyle方法不会接管样式的所有权，因此要手动删除，这里使用智能指针
    setStyle(d_ptr->mStyle.get());
    setAutoRaise(true);
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    setButtonType(SmallButton);
    setMouseTracking(true);
}

SARibbonToolButton::SARibbonToolButton(QAction* defaultAction, QWidget* parent)
    : QToolButton(parent), d_ptr(new SARibbonToolButton::PrivateData(this))
{
    setAutoRaise(true);
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    setDefaultAction(defaultAction);
    setButtonType(SmallButton);
    setMouseTracking(true);
}

SARibbonToolButton::~SARibbonToolButton()
{
}

/**
 * @brief Sets the layout factor for fine-tuning the button's appearance / 设置布局系数以微调按钮外观
 *
 * This function allows you to customize the button's text height and maximum aspect ratio.
 * After calling this function, the button's geometry will be invalidated to trigger a relayout.
 *
 * 此函数允许您自定义按钮的文本高度和最大宽高比。
 * 调用此函数后，按钮的几何尺寸将被标记为无效，以触发重新布局。
 *
 * Example:
 * @code
 * SARibbonToolButton::LayoutFactor lf;
 * lf.twoLineHeightFactor = 2.2; // Make two-line text taller/让两行文字更高
 * lf.buttonMaximumAspectRatio = 1.6; // Allow a wider button/允许按钮更宽
 * myRibbonButton->setLayoutFactor(lf);
 * @endcode
 *
 * @param fac The new layout factor / 新的布局系数
 * @sa layoutFactor, setButtonMaximumAspectRatio
 */
void SARibbonToolButton::setLayoutFactor(const SARibbonToolButton::LayoutFactor& fac)
{
    d_ptr->layoutFactor = fac;
    // 重新布局
    invalidateSizeHint();
}

/**
 * @brief Gets a const reference to the current layout factor / 获取当前布局系数的常量引用
 * @return A const reference to the layout factor / 布局系数的常量引用
 * @sa setLayoutFactor, setButtonMaximumAspectRatio
 */
const SARibbonToolButton::LayoutFactor& SARibbonToolButton::layoutFactor() const
{
    return d_ptr->layoutFactor;
}

/**
 * @brief Gets a mutable reference to the current layout factor / 获取当前布局系数的可变引用
 * @return A mutable reference to the layout factor / 布局系数的可变引用
 * @sa setLayoutFactor, setButtonMaximumAspectRatio
 */
SARibbonToolButton::LayoutFactor& SARibbonToolButton::layoutFactor()
{
    return d_ptr->layoutFactor;
}

/**
 * @brief Gets the current button type (LargeButton or SmallButton) / 获取当前按钮的类型（大按钮或小按钮）
 * @return The current button type / 当前按钮类型
 * @sa setButtonType
 */
SARibbonToolButton::RibbonButtonType SARibbonToolButton::buttonType() const
{
    return (d_ptr->mButtonType);
}

/**
 * @brief Sets the button type to LargeButton or SmallButton / 设置按钮类型为大按钮或小按钮
 *
 * Changing the button type will invalidate the size hint and trigger a relayout.
 * Note: This function may override the tool button style. If you need to set a specific style (e.g.,
 * Qt::ToolButtonIconOnly), do so after calling this function.
 *
 * 设置按钮类型会令尺寸提示失效并触发重新布局。
 * 注意：此函数可能会覆盖工具按钮样式。如需设置特定样式（例如 Qt::ToolButtonIconOnly），请在此函数调用之后设置。
 *
 * @param buttonType The new button type / 新的按钮类型
 * @sa isLargeRibbonButton, isSmallRibbonButton
 */
void SARibbonToolButton::setButtonType(const RibbonButtonType& buttonType)
{
    d_ptr->mButtonType = buttonType;
    // 计算iconrect
    // 根据字体计算文字的高度

    if (LargeButton == buttonType) {
        setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    } else {
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    }
    invalidateSizeHint();
}

/**
 * @brief Checks if the button is a small ribbon button / 判断按钮是否为小Ribbon按钮
 * @return `true` if the button type is `SmallButton`; otherwise `false` / 如果按钮类型为 `SmallButton` 则返回 `true`；否则返回 `false`
 * @sa isLargeRibbonButton, buttonType
 */
bool SARibbonToolButton::isSmallRibbonButton() const
{
    return (d_ptr->mButtonType == SmallButton);
}

/**
 * @brief Checks if the button is a large ribbon button / 判断按钮是否为大Ribbon按钮
 * @return `true` if the button type is `LargeButton`; otherwise `false` / 如果按钮类型为 `LargeButton` 则返回 `true`；否则返回 `false`
 * @sa isSmallRibbonButton, buttonType
 */
bool SARibbonToolButton::isLargeRibbonButton() const
{
    return (d_ptr->mButtonType == LargeButton);
}

/**
 * @brief Gets the current spacing value / 获取当前的间距值
 *
 * Spacing is the gap between the icon, text, indicator, and the button's border.
 *
 * 间距是图标、文字、指示器与按钮边框之间的间隙。
 *
 * @return The current spacing in pixels / 当前的间距值（像素）
 * @sa setSpacing
 */
int SARibbonToolButton::spacing() const
{
    return d_ptr->mSpacing;
}

/**
 * @brief Sets the spacing between elements and the border / 设置元素与边框之间的间距
 *
 * This spacing affects the layout of the icon, text, and indicator within the button.
 * After calling this function, the button's geometry will be invalidated to trigger a relayout.
 *
 * 此间距会影响按钮内图标、文字和指示器的布局。
 * 调用此函数后，按钮的几何尺寸将被标记为无效，以触发重新布局。
 *
 * @param v The new spacing value in pixels / 新的间距值（像素）
 * @sa spacing
 */
void SARibbonToolButton::setSpacing(int v)
{
    d_ptr->mSpacing = v;
    invalidateSizeHint();
}

/**
 * @brief Forces an update of the internal layout rectangles / 强制更新内部布局矩形
 *
 * This function recalculates the drawing rectangles for the icon, text, and indicator based on the current button
 * size and style. It also invalidates the cached size hint. This is typically called automatically during a resize
 * event.
 *
 * 此函数会根据当前按钮尺寸和样式，重新计算图标、文字和指示器的绘制矩形。同时会使缓存的尺寸提示失效。
 * 此函数通常在调整大小事件中被自动调用。
 *
 * @note This function invalidates the size hint cache but does not call `updateGeometry()`. If you need to trigger
 * a parent layout update, call `updateGeometry()` manually after this function.
 * / 此函数会清除 sizehint 缓存，但不会调用 updateGeometry()。如果需要触发布局更新，应在调用此函数后手动调用 updateGeometry()。
 */
void SARibbonToolButton::updateRect()
{
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    d_ptr->updateDrawRect(opt);
    // 这里不调用invalidateSizeHint();因为nvalidateSizeHint();会调用updateGeometry函数，导致父窗口再次布局
    d_ptr->mSizeHint = QSize();
}

/**
 * @brief Enables or disables automatic text wrapping for large buttons / 为大按钮启用或禁用自动文字换行
 *
 * When enabled, the text in a large button will attempt to wrap onto a second line if it is too long to fit on one line.
 * This is particularly useful for long action names in the Ribbon interface.
 * The button's size hint will be recalculated after calling this function.
 *
 * 启用后，如果大按钮中的文字过长无法在一行内显示，将尝试换行到第二行。
 * 这在Ribbon界面中处理较长的操作名称时非常有用。
 * 调用此函数后，按钮的size hint将被重新计算。
 *
 * Example:
 * @code
 * // Enable word wrap for a button with a potentially long label/为一个可能有长标签的按钮启用文字换行
 * myLongLabelButton->setEnableWordWrap(true);
 * @endcode
 *
 * @param on `true` to enable word wrap; `false` to disable it / `true` 启用换行，`false` 禁用换行
 * @sa isEnableWordWrap
 */
void SARibbonToolButton::setEnableWordWrap(bool on)
{
    d_ptr->mWordWrap = on;
    // 通知父布局需要重新布局
    invalidateSizeHint();
}

/**
 * @brief Checks if automatic text wrapping is enabled / 检查是否启用了自动文字换行
 * @return `true` if word wrap is enabled; otherwise `false` / 如果启用了文字换行则返回 `true`；否则返回 `false`
 * @sa setEnableWordWrap
 */
bool SARibbonToolButton::isEnableWordWrap()
{
    return d_ptr->mWordWrap;
}

/**
 * @brief Sets the button's maximum aspect ratio (width/height) / 设置按钮的最大宽高比
 *
 * This is a convenience function that directly sets the `buttonMaximumAspectRatio` member of the `LayoutFactor`
 * structure. It has the same effect as modifying the structure and calling `setLayoutFactor`.
 *
 * 此函数是直接设置 `LayoutFactor` 结构体中 `buttonMaximumAspectRatio` 成员的便捷方法。
 * 其效果等同于修改结构体后调用 `setLayoutFactor`。
 *
 * @param v The new maximum aspect ratio value / 新的最大宽高比值
 * @sa buttonMaximumAspectRatio, setLayoutFactor
 */
void SARibbonToolButton::setButtonMaximumAspectRatio(qreal v)
{
    d_ptr->layoutFactor.buttonMaximumAspectRatio = v;
    // 重新布局
    invalidateSizeHint();
}

/**
 * @brief Gets the button's maximum aspect ratio (width/height) / 获取按钮的最大宽高比
 * @return The current maximum aspect ratio / 当前的最大宽高比
 * @sa setButtonMaximumAspectRatio, layoutFactor
 */
qreal SARibbonToolButton::buttonMaximumAspectRatio() const
{
    return layoutFactor().buttonMaximumAspectRatio;
}

bool SARibbonToolButton::event(QEvent* e)
{
    switch (e->type()) {
    case QEvent::WindowDeactivate:
        d_ptr->mMouseOnSubControl = false;
        break;
    case QEvent::ActionChanged:
    case QEvent::ActionRemoved:
    case QEvent::ActionAdded: {
        d_ptr->mMouseOnSubControl = false;
        invalidateSizeHint();
    } break;
    default:
        break;
    }

    return (QToolButton::event(e));
}

void SARibbonToolButton::changeEvent(QEvent* e)
{
    if (e) {
        switch (e->type()) {
        case QEvent::FontChange:
        case QEvent::StyleChange:
        case QEvent::LanguageChange: {
            // 说明字体改变，需要重新计算和字体相关的信息
            invalidateSizeHint();
        } break;
        case QEvent::ScreenChangeInternal: {
            invalidateSizeHint();
        }
        default:
            break;
        }
    }
    QToolButton::changeEvent(e);
}

/**
 * @brief 鼠标移动事件
 *
 * 由于Ribbon的Indicator和正常的Toolbutton不一样，因此无法用QStyleOptionToolButton的activeSubControls的状态
 *
 * 因此需要重新捕获鼠标的位置来更新按钮当前的一些状态
 * @param e
 */
void SARibbonToolButton::mouseMoveEvent(QMouseEvent* e)
{
    d_ptr->updateStatusByMousePosition(e->pos());
    QToolButton::mouseMoveEvent(e);
}

/**
 * @brief SARibbonToolButton::mousePressEvent
 * @param e
 */
void SARibbonToolButton::mousePressEvent(QMouseEvent* e)
{
    if ((e->button() == Qt::LeftButton) && (popupMode() == MenuButtonPopup)) {
        d_ptr->updateStatusByMousePosition(e->pos());
        if (d_ptr->mMouseOnSubControl) {
            d_ptr->mMenuButtonPressed = true;
            showMenu();
            // showmenu结束后，在判断当前的鼠标位置是否是在subcontrol
            d_ptr->updateStatusByMousePosition(mapFromGlobal(QCursor::pos()));
            return;
        }
    }
    d_ptr->mMenuButtonPressed = false;
    //! 注意这里要用QAbstractButton的mousePressEvent，而不是QToolButton的mousePressEvent
    //! QToolButton的mousePressEvent主要是为了弹出菜单，这里弹出菜单的方式是不一样的，因此不能执行QToolButton的mousePressEvent
    QToolButton::mousePressEvent(e);
}

void SARibbonToolButton::mouseReleaseEvent(QMouseEvent* e)
{
    d_ptr->mMenuButtonPressed = false;
    QToolButton::mouseReleaseEvent(e);
}

void SARibbonToolButton::focusOutEvent(QFocusEvent* e)
{
    d_ptr->mMouseOnSubControl = false;
    QToolButton::focusOutEvent(e);
}

void SARibbonToolButton::leaveEvent(QEvent* e)
{
    d_ptr->mMouseOnSubControl = false;
    QToolButton::leaveEvent(e);
}

bool SARibbonToolButton::hitButton(const QPoint& pos) const
{
    if (QToolButton::hitButton(pos)) {
        return (!d_ptr->mMenuButtonPressed);
    }
    return (false);
}

/**
 * @brief 在resizeevent计算绘图所需的尺寸，避免在绘图过程中实时绘制提高效率
 * @param e
 */
void SARibbonToolButton::resizeEvent(QResizeEvent* e)
{
    // 在resizeevent计算绘图所需的尺寸，避免在绘图过程中实时绘制提高效率
    QToolButton::resizeEvent(e);
    updateRect();
}

/**
 * @brief Returns the recommended size for the button / 返回按钮的推荐尺寸
 *
 * This size is calculated based on the button's type, text, icon, and current layout factors.
 * The result is cached for performance. The cache is invalidated when relevant properties change.
 *
 * 此尺寸是根据按钮的类型、文字、图标和当前布局系数计算得出的。
 * 为提高性能，计算结果会被缓存。当相关属性改变时，缓存会自动失效。
 *
 * @return The recommended size / 推荐的尺寸
 */
QSize SARibbonToolButton::sizeHint() const
{
#if SA_RIBBON_TOOLBUTTON_DEBUG_PRINT
    qDebug() << "| | |-SARibbonToolButton::sizeHint";
#endif
    if (d_ptr->mSizeHint.isValid()) {
        return d_ptr->mSizeHint;
    }
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    d_ptr->updateSizeHint(opt);
    return d_ptr->mSizeHint;
}

/**
 * @brief Returns the recommended minimum size for the button / 返回按钮的推荐最小尺寸
 *
 * For `SARibbonToolButton`, the minimum size hint is the same as the size hint.
 *
 * 对于 `SARibbonToolButton`，最小尺寸提示与尺寸提示相同。
 *
 * @return The recommended minimum size / 推荐的最小尺寸
 */
QSize SARibbonToolButton::minimumSizeHint() const
{
    return (sizeHint());
}

void SARibbonToolButton::actionEvent(QActionEvent* e)
{
    QToolButton::actionEvent(e);
    invalidateSizeHint();
}

void SARibbonToolButton::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);
    QPainter p(this);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    if (opt.features & QStyleOptionToolButton::MenuButtonPopup || opt.features & QStyleOptionToolButton::HasMenu) {
        // 在菜单弹出消失后，需要通过此方法取消掉鼠标停留
        if (!rect().contains(mapFromGlobal(QCursor::pos()))) {
            opt.state &= ~QStyle::State_MouseOver;
        }
    }
    paintButton(p, opt);
    paintIcon(p, opt, d_ptr->mDrawIconRect);
    paintText(p, opt, d_ptr->mDrawTextRect);
    paintIndicator(p, opt, d_ptr->mDrawIndicatorArrowRect);
}

/**
 * @brief Paints the button's background and frame / 绘制按钮的背景和边框
 *
 * This function handles the special visual effects for the Ribbon style, particularly for the `MenuButtonPopup`
 * mode where the icon and text areas can have different hover states.
 *
 * 此函数处理Ribbon样式的特殊视觉效果，特别是在 `MenuButtonPopup` 模式下，图标和文字区域可以有不同的悬停状态。
 *
 * @param p The painter to use / 用于绘制的painter
 * @param opt The style option for the tool button / 工具按钮的样式选项
 */
void SARibbonToolButton::paintButton(QPainter& p, const QStyleOptionToolButton& opt)
{
    // QStyle::State_Sunken 代表按钮按下去了
    // QStyle::State_On 代表按钮按checked
    // QStyle::State_MouseOver 代表当前鼠标位于按钮上面
    QStyleOption tool = opt;
    bool autoRaise    = opt.state & QStyle::State_AutoRaise;
    // 绘制按钮
    if (autoRaise) {
        // 这个是为了实现按钮点击下去后(QStyle::State_Sunken),能出现选中的状态
        // 先绘制一个鼠标不在按钮上的状态
        if (opt.state & QStyle::State_Sunken) {
            tool.state &= ~QStyle::State_MouseOver;
        }
        style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
    } else {
        style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
    }
    // 针对MenuButtonPopup的ribbon样式的特殊绘制
    if ((opt.subControls & QStyle::SC_ToolButton) && (opt.features & QStyleOptionToolButton::MenuButtonPopup)) {
        if (opt.state & QStyle::State_MouseOver) {                       // 鼠标在按钮上才进行绘制
            if (!(opt.activeSubControls & QStyle::SC_ToolButtonMenu)) {  // 按钮的菜单弹出时不做处理
                if (LargeButton == d_ptr->mButtonType) {                 // 大按钮模式
                    if (d_ptr->mMouseOnSubControl) {                     // 此时鼠标在indecater那
                        // 鼠标在文字区，把图标显示为正常（就是鼠标不放上去的状态）
                        tool.rect = d_ptr->mDrawIconRect;
                        tool.state |= (QStyle::State_Raised);  // 把图标区域显示为正常
                        tool.state &= ~QStyle::State_MouseOver;
                        if (autoRaise) {
                            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
                        } else {
                            style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
                        }
                    } else {
                        // 鼠标在图标区，把文字显示为正常
                        if (!tool.state.testFlag(QStyle::State_Sunken)) {
                            // State_Sunken说明此按钮正在按下，这时候，文本区域不需要绘制，只有在非按下状态才需要绘制
                            tool.state |= (QStyle::State_Raised);  // 把图标区域显示为正常
                            tool.state &= ~QStyle::State_MouseOver;
                            // 文字和Indicator都显示正常
                            tool.rect = d_ptr->mDrawTextRect.united(d_ptr->mDrawIndicatorArrowRect);
                            if (autoRaise) {
                                style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
                            } else {
                                style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
                            }
                        }
                    }
                } else {                              // 小按钮模式
                    if (d_ptr->mMouseOnSubControl) {  // 此时鼠标在indecater那
                        // 鼠标在文字区，把图标和文字显示为正常
                        tool.rect  = d_ptr->mDrawIconRect.united(d_ptr->mDrawTextRect);
                        tool.state = (QStyle::State_Raised);  // 把图标区域显示为正常
                        if (autoRaise) {
                            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
                        } else {
                            style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
                        }
                    } else {
                        // 鼠标在图标区，把文字显示为正常
                        tool.state = (QStyle::State_Raised);  // 把图标区域显示为正常
                        // 文字和Indicator都显示正常
                        tool.rect = d_ptr->mDrawIndicatorArrowRect;
                        if (autoRaise) {
                            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
                        } else {
                            style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
                        }
                    }
                }
            }
        }
    }
    // 绘制Focus
    //     if (opt.state & QStyle::State_HasFocus) {
    //         QStyleOptionFocusRect fr;
    //         fr.QStyleOption::operator=(opt);
    //         fr.rect.adjust(d_ptr->mSpacing, d_ptr->mSpacing, -d_ptr->mSpacing, -d_ptr->mSpacing);
    //         style()->drawPrimitive(QStyle::PE_FrameFocusRect, &fr, &p, this);
    //     }
}

/**
 * @brief Paints the button's icon / 绘制按钮的图标
 *
 * The icon is painted within the specified rectangle, scaled appropriately based on the available space.
 *
 * 图标会在指定的矩形区域内绘制，并根据可用空间进行适当缩放。
 *
 * @param p The painter to use / 用于绘制的painter
 * @param opt The style option for the tool button / 工具按钮的样式选项
 * @param iconDrawRect The rectangle in which to draw the icon / 绘制图标的矩形区域
 */
void SARibbonToolButton::paintIcon(QPainter& p, const QStyleOptionToolButton& opt, const QRect& iconDrawRect)
{
    if (!iconDrawRect.isValid()) {
        return;
    }

    QPixmap pm = d_ptr->createIconPixmap(opt, d_ptr->realIconSize());
    style()->drawItemPixmap(&p, iconDrawRect, Qt::AlignCenter, pm);
    SARIBBONTOOLBUTTON_DEBUG_DRAW_RECT(p, iconDrawRect);
}

/**
 * @brief Paints the button's text / 绘制按钮的文字
 *
 * The text is painted within the specified rectangle, with alignment and elision (truncation with "...") handled
 * according to the button's type and word-wrap setting.
 *
 * 文字会在指定的矩形区域内绘制，其对齐方式和省略（用“...”截断）会根据按钮的类型和文字换行设置进行处理。
 *
 * @param p The painter to use / 用于绘制的painter对象
 * @param opt The style option for the tool button / 工具按钮的样式选项
 * @param textDrawRect The rectangle in which to draw the text / 绘制文字的矩形区域
 */
void SARibbonToolButton::paintText(QPainter& p, const QStyleOptionToolButton& opt, const QRect& textDrawRect)
{
    int alignment = d_ptr->getTextAlignment();

    if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &opt, this)) {
        alignment |= Qt::TextHideMnemonic;
    }
    QString text;
    if (isSmallRibbonButton()) {
        text = opt.fontMetrics.elidedText(
            PrivateData::simplifiedForRibbonButton(opt.text), Qt::ElideRight, textDrawRect.width(), alignment);
    } else {
        if (!isEnableWordWrap()) {
            text = opt.fontMetrics.elidedText(
                PrivateData::simplifiedForRibbonButton(opt.text), Qt::ElideRight, textDrawRect.width(), alignment);
        } else {
            text = opt.text;
        }
    }
    //! 以下内容参考QCommonStyle.cpp
    //! void QCommonStyle::drawComplexControl(ComplexControl cc, const QStyleOptionComplex *opt,QPainter *p, const QWidget *widget) const
    //! case CC_ToolButton:
    QStyle::State bflags = opt.state & ~QStyle::State_Sunken;
    if (bflags & QStyle::State_AutoRaise) {
        if (!(bflags & QStyle::State_MouseOver) || !(bflags & QStyle::State_Enabled)) {
            bflags &= ~QStyle::State_Raised;
        }
    }
    if (opt.state & QStyle::State_Sunken) {
        if (opt.activeSubControls & QStyle::SC_ToolButton) {
            bflags |= QStyle::State_Sunken;
        }
    }
    QStyleOptionToolButton label = opt;
    label.state                  = bflags;
    style()->drawItemText(
        &p, textDrawRect, alignment, label.palette, label.state & QStyle::State_Enabled, text, QPalette::ButtonText);
    SARIBBONTOOLBUTTON_DEBUG_DRAW_RECT(p, textDrawRect);
}

/**
 * @brief Paints the button's indicator (e.g., dropdown arrow) / 绘制按钮的指示器（例如下拉箭头）
 *
 * The indicator is painted within the specified rectangle if the button has a menu (i.e., features include
 * `MenuButtonPopup` or `HasMenu`).
 *
 * 如果按钮有菜单（即特性包含 `MenuButtonPopup` 或 `HasMenu`），则会在指定的矩形区域内绘制指示器。
 *
 * @param p The painter to use / 用于绘制的painter对象
 * @param opt The style option for the tool button / 工具按钮的样式选项
 * @param indicatorDrawRect The rectangle in which to draw the indicator / 绘制指示器的矩形区域
 */
void SARibbonToolButton::paintIndicator(QPainter& p, const QStyleOptionToolButton& opt, const QRect& indicatorDrawRect)
{
    if (!indicatorDrawRect.isValid() || !d_ptr->hasIndicator(opt)) {
        return;
    }

    QStyleOption tool = opt;
    tool.rect         = indicatorDrawRect;
    style()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &tool, &p, this);
    SARIBBONTOOLBUTTON_DEBUG_DRAW_RECT(p, indicatorDrawRect);
}

/**
 * @brief Invalidates the cached size hint / 使缓存的size hint失效
 *
 * This function clears the internally cached `sizeHint()` value and calls `updateGeometry()`,
 * which notifies the layout system that this widget needs to be relayouted.
 * It is called automatically when properties affecting the size (like text, font, or button type) change.
 *
 * 此函数会清除内部缓存的 `sizeHint()` 值并调用 `updateGeometry()`，
 * 通知布局系统此控件需要重新布局。
 * 当影响尺寸的属性（如文字、字体或按钮类型）发生变化时，会自动调用此函数。
 */
void SARibbonToolButton::invalidateSizeHint()
{
    d_ptr->mSizeHint = QSize();
    updateGeometry();
}

/**
 * @brief 大按钮的尺寸
 * @param largeSize
 */
void SARibbonToolButton::setLargeIconSize(const QSize& largeSize)
{
    d_ptr->mLargeButtonSizeHint = largeSize;
}

/**
 * @brief 大按钮的尺寸
 * @return
 */
QSize SARibbonToolButton::largeIconSize() const
{
    return d_ptr->mLargeButtonSizeHint;
}

/**
 * @brief 小按钮尺寸
 * @param smallSize
 */
void SARibbonToolButton::setSmallIconSize(const QSize& smallSize)
{
    setIconSize(smallSize);
}

/**
 * @brief 小按钮尺寸
 * @return
 */
QSize SARibbonToolButton::smallIconSize() const
{
    return iconSize();
}

void SARibbonToolButton::drawArrow(const QStyle* style,
                                   const QStyleOptionToolButton* toolbutton,
                                   const QRect& rect,
                                   QPainter* painter,
                                   const QWidget* widget)
{
    QStyle::PrimitiveElement pe;

    switch (toolbutton->arrowType) {
    case Qt::LeftArrow:
        pe = QStyle::PE_IndicatorArrowLeft;
        break;

    case Qt::RightArrow:
        pe = QStyle::PE_IndicatorArrowRight;
        break;

    case Qt::UpArrow:
        pe = QStyle::PE_IndicatorArrowUp;
        break;

    case Qt::DownArrow:
        pe = QStyle::PE_IndicatorArrowDown;
        break;

    default:
        return;
    }
    QStyleOption arrowOpt = *toolbutton;

    arrowOpt.rect = rect;
    style->drawPrimitive(pe, &arrowOpt, painter, widget);
}
