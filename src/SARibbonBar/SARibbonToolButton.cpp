#include "SARibbonToolButton.h"
#include "SARibbonPannel.h"

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

/**
 * @def 定义文字换行时2行文本的矩形高度系数，此系数决定文字区域的高度
 *
 * fontMetrics.lineSpacing*系数 = 文本区域高度
 */
#define SARIBBONTOOLBUTTON_WORDWRAP_TEXT_FACTOR 2.05

/**
 * @def 定义文字不换行时单行文本的矩形高度系数，此系数决定文字区域的高度
 *
 * fontMetrics.lineSpacing*系数 = 文本区域高度
 */
#define SARIBBONTOOLBUTTON_NOWORDWRAP_TEXT_FACTOR 1.2

/**
 * @def 定义小按钮的矩形高度系数，此系数决定文字区域的高度
 *
 * fontMetrics.lineSpacing*系数 = 文本区域高度
 */
#define SARIBBONTOOLBUTTON_SMALLBUTTON_TEXT_FACTOR 1.4

/**
 * @def 文本宽度估算时的宽度比高度系数
 *
 * 超过此系数的宽度时，开始尝试换行，例如按钮高度为h，如果单行文本的宽度大于h*系数，则触发换行估算
 */
#define SARIBBONTOOLBUTTON_WORDWRAP_WIDTH_PER_HEIGHT_RATIO 1.4

/**
 * @def 开启此宏会打印一些常见信息
 */
#define SA_RIBBON_TOOLBUTTON_DEBUG_PRINT 0

#define SARIBBONTOOLBUTTON_DEBUG_DRAW 0

#if SARIBBONTOOLBUTTON_DEBUG_DRAW
#define SARIBBONTOOLBUTTON_DEBUG_DRAW_RECT(p, rect)                                                                    \
    do {                                                                                                               \
        p.save();                                                                                                      \
        p.setPen(Qt::red);                                                                                             \
        p.setBrush(QBrush());                                                                                          \
        p.drawRect(rect);                                                                                              \
        p.restore();                                                                                                   \
    } while (0)

#else
#define SARIBBONTOOLBUTTON_DEBUG_DRAW_RECT(p, rect)
#endif
namespace SA
{

QDebug operator<<(QDebug debug, const QStyleOptionToolButton& opt)
{
    debug << "=============="
          << "\nQStyleOption(" << (QStyleOption)opt << ")"
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
	void drawPrimitive(PrimitiveElement pe, const QStyleOption *opt, QPainter *p, const QWidget *widget = nullptr) const override
	{
		if(pe == PE_IndicatorArrowUp || pe == PE_IndicatorArrowDown || pe == PE_IndicatorArrowRight || pe == PE_IndicatorArrowLeft)
		{
			if (opt->rect.width() <= 1 || opt->rect.height() <= 1)
				return;

			QRect r = opt->rect;
			int size = qMin(r.height(), r.width());
			QPixmap pixmap;
			qreal pixelRatio = p->device()->devicePixelRatio();
			int border = qRound(pixelRatio*(size/4));
			int sqsize = qRound(pixelRatio*(2*(size/2)));
			QImage image(sqsize, sqsize, QImage::Format_ARGB32_Premultiplied);
			image.fill(Qt::transparent);
			QPainter imagePainter(&image);

			QPolygon a;
			switch (pe) {
			case PE_IndicatorArrowUp:
				a.setPoints(3, border, sqsize/2,  sqsize/2, border,  sqsize - border, sqsize/2);
				break;
			case PE_IndicatorArrowDown:
				a.setPoints(3, border, sqsize/2,  sqsize/2, sqsize - border,  sqsize - border, sqsize/2);
				break;
			case PE_IndicatorArrowRight:
				a.setPoints(3, sqsize - border, sqsize/2,  sqsize/2, border,  sqsize/2, sqsize - border);
				break;
			case PE_IndicatorArrowLeft:
				a.setPoints(3, border, sqsize/2,  sqsize/2, border,  sqsize/2, sqsize - border);
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
			int sx = sqsize / 2 - bounds.center().x() - 1;
			int sy = sqsize / 2 - bounds.center().y() - 1;
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

			int xOffset = r.x() + (r.width() - size)/2;
			int yOffset = r.y() + (r.height() - size)/2;
			p->drawPixmap(xOffset, yOffset, pixmap);
		}
		else
		{
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
                                     float widthHeightRatio = SARIBBONTOOLBUTTON_WORDWRAP_WIDTH_PER_HEIGHT_RATIO,
                                     int maxTrycount        = 3);
    QPixmap createIconPixmap(const QStyleOptionToolButton& opt, const QSize& iconsize) const;
    // 获取文字的对其方式
    int getTextAlignment() const;
    // 确认文字是否确切要换行显示
    bool isTextNeedWrap() const;
    // 仅仅对\n进行剔除，和QString::simplified不一样
    static QString simplified(const QString& str);

public:
    bool mMouseOnSubControl { false };  ///< 这个用于标记MenuButtonPopup模式下，鼠标在文本区域
    bool mMenuButtonPressed { false };  ///< 由于Indicator改变，因此hitButton不能用QToolButton的hitButton
    bool mWordWrap { false };           ///< 标记是否文字换行 @default false
    SARibbonToolButton::RibbonButtonType mButtonType { SARibbonToolButton::LargeButton };
    int mSpacing { 1 };              ///< 按钮和边框的距离
    int mIndicatorLen { 8 };         ///< Indicator的长度
    QRect mDrawIconRect;             ///< 记录icon的绘制位置
    QRect mDrawTextRect;             ///< 记录text的绘制位置
    QRect mDrawIndicatorArrowRect;   ///< 记录IndicatorArrow的绘制位置
    QSize mSizeHint;                 ///< 保存计算好的sizehint
    bool mIsTextNeedWrap { false };  ///< 标记文字是否需要换行显示
public:
    static bool s_enableWordWrap;  ///< 在lite模式下是否允许文字换行，如果允许，则图标相对比较小，默认不允许
};

// 静态参数初始化
bool SARibbonToolButton::PrivateData::s_enableWordWrap = false;

SARibbonToolButton::PrivateData::PrivateData(SARibbonToolButton* p) : q_ptr(p)
{
	auto proxy = new SARibbonToolButtonProxyStyle();
	proxy->setParent(p);   // take ownership to avoid memleak
	p->setStyle(proxy);
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
            iconRect           = opt.rect.adjusted(spacing, spacing, -indicatorLen - spacing, -spacing);
            indicatorArrowRect = QRect(opt.rect.right() - indicatorLen - spacing,
                                       iconRect.y(),
                                       indicatorLen,
                                       iconRect.height());
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
            iconRect = QRect(buttonRect.x(), buttonRect.y(), iconSize.width(), qMax(iconSize.height(), buttonRect.height()));
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
                indicatorArrowRect = QRect(buttonRect.right() - indicatorLen + 1,
                                           textRect.y(),
                                           indicatorLen,
                                           textRect.height());
            } else if (iconRect.isValid()) {
                indicatorArrowRect = QRect(buttonRect.right() - indicatorLen + 1,
                                           iconRect.y(),
                                           indicatorLen,
                                           iconRect.height());
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
    // 先获取文字矩形的高度
    int textHeight  = calcTextDrawRectHeight(opt);
    bool hIndicator = hasIndicator(opt);
    if (!hIndicator) {
        // 没有菜单，把len设置为0
        indicatorLen       = 0;
        indicatorArrowRect = QRect();
    }
    // 这里要判断文字是否要换行显示，换行显示的文字的indicatorArrowRect所处的位置不一样
    // 先布置textRect
    if (isEnableWordWrap()) {
        // 在换行模式下
        if (isTextNeedWrap()) {
            // 如果文字的确换行，indicator放在最右边
            textRect = QRect(spacing,
                             opt.rect.bottom() - spacing - textHeight,
                             opt.rect.width() - 2 * spacing - indicatorLen,
                             textHeight);
            if (hIndicator) {
                indicatorArrowRect = QRect(textRect.right(), textRect.y() + textRect.height() / 2, indicatorLen, textHeight / 2);
            }
        } else {
            // 如果文字不需要换行，indicator在下板行
            textRect = QRect(spacing, opt.rect.bottom() - spacing - textHeight, opt.rect.width() - 2 * spacing, textHeight);
            if (hIndicator) {
                int dy = textRect.height() / 2;
                dy += (dy - indicatorLen) / 2;
                indicatorArrowRect = QRect(textRect.x(), textRect.y() + dy, textRect.width(), indicatorLen);
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
            textRect = QRect(spacing, y, opt.rect.width() - 2 * spacing, textHeight);
        }
    }
    // 剩下就是icon区域
    iconRect = QRect(spacing, spacing, opt.rect.width() - 2 * spacing, textRect.top() - 2 * spacing);
}

/**
 * @brief 适应iconsize
 * @param buttonRect
 * @param originIconSize
 * @return
 */
QSize SARibbonToolButton::PrivateData::adjustIconSize(const QRect& buttonRect, const QSize& originIconSize) const
{
    QSize iconSize = originIconSize;
    if (iconSize.height() > buttonRect.height()) {
        // 说明图标的icon过大，要匹配到buttonRect
        iconSize.setHeight(buttonRect.height());
        // 等比例设置宽度
        iconSize.setWidth(originIconSize.width() * iconSize.height() / originIconSize.height());
    }
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
        QSize textSize = opt.fontMetrics.size(Qt::TextShowMnemonic, simplified(opt.text));
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
            QSize textSize = opt.fontMetrics.size(Qt::TextShowMnemonic, simplified(opt.text));
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
    int minW = h * 0.75;  // 最小宽度，在pannel里面的按钮，最小宽度要和icon适应

    if (SARibbonPannel* pannel = qobject_cast< SARibbonPannel* >(q_ptr->parent())) {
        // 对于建立在SARibbonPannel的基础上的大按钮，把高度设置为SARibbonPannel计算的大按钮高度
        h = pannel->largeButtonHeight();
    }
    int textHeight = calcTextDrawRectHeight(opt);
    // 估算字体的宽度作为宽度
    w = estimateLargeButtonTextWidth(h, textHeight, opt.text, opt.fontMetrics);
    w += (2 * mSpacing);
    // 判断是否需要加上indicator
    if (isEnableWordWrap() && isTextNeedWrap()) {
        w += mIndicatorLen;
    }

#if SA_RIBBON_TOOLBUTTON_DEBUG_PRINT && SA_DEBUG_PRINT_SIZE_HINT
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
        if (isEnableWordWrap()) {
            return opt.fontMetrics.lineSpacing() * SARIBBONTOOLBUTTON_WORDWRAP_TEXT_FACTOR + opt.fontMetrics.leading();
        } else {
            return opt.fontMetrics.lineSpacing() * SARIBBONTOOLBUTTON_NOWORDWRAP_TEXT_FACTOR;
        }
    }
    // 小按钮
    return opt.fontMetrics.lineSpacing() * SARIBBONTOOLBUTTON_SMALLBUTTON_TEXT_FACTOR;
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
                                                                  float widthHeightRatio,
                                                                  int maxTrycount)
{
    QSize textSize;
    int space        = SA_FONTMETRICS_WIDTH(fm, (QLatin1Char(' '))) * 2;
    int hintMaxWidth = buttonHeight * widthHeightRatio;  ///< 建议的宽度
    if (isEnableWordWrap()) {
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
    textSize = fm.size(Qt::TextShowMnemonic, simplified(text));
    textSize.setWidth(textSize.width() + space);
    if (textSize.width() < hintMaxWidth) {
        // 范围合理，直接返回
        return textSize.width();
    }
    if (textSize.width() > q_ptr->maximumWidth()) {
        // 超出了极限，就返回极限
        return q_ptr->maximumWidth();
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
    // 添加高分屏支持
    QSize pxiampSize = iconsize - QSize(2, 2);
    return opt.icon.pixmap(pxiampSize, mode, state);
}

int SARibbonToolButton::PrivateData::getTextAlignment() const
{
    int alignment = Qt::TextShowMnemonic;
    if (SARibbonToolButton::LargeButton == mButtonType) {
        if (isEnableWordWrap()) {
            alignment |= Qt::TextWordWrap | Qt::AlignTop | Qt::AlignHCenter;  // 换行的情况下，顶部对齐
        } else {
            alignment |= Qt::AlignCenter;
        }
    } else {
        alignment |= Qt::AlignCenter;
    }
    return alignment;
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
 * @brief 仅仅对\n进行剔除
 * @param str
 * @return
 */
QString SARibbonToolButton::PrivateData::simplified(const QString& str)
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
    QAbstractButton::mousePressEvent(e);
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
 * @brief toolbutton的尺寸确定是先定下字体的尺寸，再定下icon的尺寸，自底向上，保证字体能显示两行
 * @note m_sizeHint的刷新需要注意
 * @return
 */
QSize SARibbonToolButton::sizeHint() const
{
    //    if (!d_ptr->mSizeHint.isValid()) {  // 22是给与sizehint的最小值，如果小于这个值，重新计算一下
    //        QStyleOptionToolButton opt;
    //        initStyleOption(&opt);
    //        d_ptr->updateSizeHint(opt);
    //    }
#if SA_RIBBON_TOOLBUTTON_DEBUG_PRINT && SA_DEBUG_PRINT_SIZE_HINT
    qDebug() << "| | |-SARibbonToolButton::sizeHint";
#endif
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    d_ptr->updateSizeHint(opt);
    return d_ptr->mSizeHint;
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
 * @brief 绘制按钮
 * @param p
 * @param opt
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
 * @brief 绘制图标
 * @param p
 * @param opt
 */
void SARibbonToolButton::paintIcon(QPainter& p, const QStyleOptionToolButton& opt, const QRect& iconDrawRect)
{
    if (!iconDrawRect.isValid()) {
        return;
    }

    QPixmap pm = d_ptr->createIconPixmap(opt, iconDrawRect.size());
    style()->drawItemPixmap(&p, iconDrawRect, Qt::AlignCenter, pm);
    SARIBBONTOOLBUTTON_DEBUG_DRAW_RECT(p, iconDrawRect);
}

/**
 * @brief 绘制文本
 * @param p
 * @param opt
 */
void SARibbonToolButton::paintText(QPainter& p, const QStyleOptionToolButton& opt, const QRect& textDrawRect)
{
    int alignment = d_ptr->getTextAlignment();

    if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &opt, this)) {
        alignment |= Qt::TextHideMnemonic;
    }
    QString text;
    if (isSmallRibbonButton()) {
        text = opt.fontMetrics.elidedText(PrivateData::simplified(opt.text), Qt::ElideRight, textDrawRect.width(), alignment);
    } else {
        if (!isEnableWordWrap()) {
            text = opt.fontMetrics.elidedText(PrivateData::simplified(opt.text), Qt::ElideRight, textDrawRect.width(), alignment);
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
    style()->drawItemText(&p, textDrawRect, alignment, label.palette, label.state & QStyle::State_Enabled, text, QPalette::ButtonText);
    SARIBBONTOOLBUTTON_DEBUG_DRAW_RECT(p, textDrawRect);
}

/**
 * @brief 绘制Indicator
 * @param p
 * @param opt
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

void SARibbonToolButton::actionEvent(QActionEvent* e)
{
    QToolButton::actionEvent(e);
    updateRect();
}

/**
 * @brief 按钮样式
 * @sa setButtonType
 * @return
 */
SARibbonToolButton::RibbonButtonType SARibbonToolButton::buttonType() const
{
    return (d_ptr->mButtonType);
}

/**
 * @brief 设置按钮样式
 * @note 设置按钮样式过程会调用setToolButtonStyle，如果要改变toolButtonStyle,如设置为Qt::ToolButtonIconOnly,需要在此函数之后设置
 * @param buttonType
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

    updateRect();
}

/**
 * @brief 是否是小按钮
 * @return
 */
bool SARibbonToolButton::isSmallRibbonButton() const
{
    return (d_ptr->mButtonType == SmallButton);
}

/**
 * @brief 是否是大按钮
 * @return
 */
bool SARibbonToolButton::isLargeRibbonButton() const
{
    return (d_ptr->mButtonType == LargeButton);
}

QSize SARibbonToolButton::minimumSizeHint() const
{
    return (sizeHint());
}

/**
 * @brief 间距是几个重要矩形的间隔
 * @return
 */
int SARibbonToolButton::spacing() const
{
    return d_ptr->mSpacing;
}

void SARibbonToolButton::updateRect()
{
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    d_ptr->updateDrawRect(opt);
}

/**
 * @brief 设置在lite模式下是否允许文字换行，如果允许，则图标相对比较小，默认不允许
 * @param on
 */
void SARibbonToolButton::setEnableWordWrap(bool on)
{
    SARibbonToolButton::PrivateData::s_enableWordWrap = on;
}

/**
 * @brief 在lite模式下是否允许文字换行
 * @return
 */
bool SARibbonToolButton::isEnableWordWrap()
{
    return SARibbonToolButton::PrivateData::s_enableWordWrap;
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
        updateRect();
    } break;
    default:
        break;
    }

    return (QToolButton::event(e));
}

void SARibbonToolButton::changeEvent(QEvent* e)
{
    if (e) {
        if (e->type() == QEvent::FontChange) {
            // 说明字体改变，需要重新计算和字体相关的信息
            updateRect();
        }
    }
    QToolButton::changeEvent(e);
}
