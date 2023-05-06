#include "SARibbonToolButton.h"
#include <QApplication>
#include <QStylePainter>
#include <QStyleOptionToolButton>
#include <QAction>
#include <QStyleOptionFocusRect>
#include <QStyleOption>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QCursor>
#include "SARibbonDrawHelper.h"
#include "SARibbonElementManager.h"

#define SA_WIDTH_HEIGHT_RATIO 1.2
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
// SARibbonToolButton::PrivateData
//===================================================

class SARibbonToolButton::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonToolButton)
public:
    PrivateData(SARibbonToolButton* p);
    //根据鼠标位置更新按钮的信息
    void updateStatusByMousePosition(const QPoint& pos);
    //更新绘图相关的尺寸
    void updateDrawRect(const QStyleOptionToolButton& opt);
    //更新SizeHint
    void updateSizeHint(const QStyleOptionToolButton& opt);
    //计算涉及到的rect尺寸
    void calcDrawRects(const QStyleOptionToolButton& opt, QRect& iconRect, QRect& textRect, QRect& indicatorArrowRect, int spacing, int indicatorLen) const;
    //计算小按钮模式下的尺寸
    void calcSmallButtonDrawRects(const QStyleOptionToolButton& opt,
                                  QRect& iconRect,
                                  QRect& textRect,
                                  QRect& indicatorArrowRect,
                                  int spacing,
                                  int indicatorLen) const;
    //计算大按钮模式下的尺寸
    void calcLargeButtonNormalDrawRects(const QStyleOptionToolButton& opt,
                                        QRect& iconRect,
                                        QRect& textRect,
                                        QRect& indicatorArrowRect,
                                        int spacing,
                                        int indicatorLen) const;
    void calcLargeButtonLiteDrawRects(const QStyleOptionToolButton& opt,
                                      QRect& iconRect,
                                      QRect& textRect,
                                      QRect& indicatorArrowRect,
                                      int spacing,
                                      int indicatorLen) const;
    //根据按钮的尺寸调节iconsize(注意这里的buttonRect是已经减去mSpacing的情况)
    QSize adjustIconSize(const QRect& buttonRect, const QSize& originIconSize) const;
    //判断是否有Indicator
    bool hasIndicator(const QStyleOptionToolButton& opt) const;
    //计算sizehint
    QSize calcSizeHint(const QStyleOptionToolButton& opt) const;
    QSize calcSmallButtonSizeHint(const QStyleOptionToolButton& opt) const;
    QSize calcLargeButtonNormalSizeHint(const QStyleOptionToolButton& opt) const;
    QSize calcLargeButtonLiteSizeHint(const QStyleOptionToolButton& opt) const;

    //计算文本绘制矩形的高度
    int calcTextDrawRectHeight(const QStyleOptionToolButton& opt) const;
    //估算一个最优的文本宽度
    int estimateLargeButtonTextWidth(int buttonHeight,
                                     int textDrawRectHeight,
                                     const QString& text,
                                     const QFontMetrics& fm,
                                     float widthHeightRatio = SA_WIDTH_HEIGHT_RATIO,
                                     int maxTrycount        = 3) const;
    QPixmap createIconPixmap(const QStyleOptionToolButton& opt, const QSize& iconsize) const;
    //获取文字的对其方式
    int getTextAlignment() const;

public:
    bool mMouseOnSubControl { false };  ///< 这个用于标记MenuButtonPopup模式下，鼠标在文本区域
    bool mMenuButtonPressed { false };  ///< 由于Indicator改变，因此hitButton不能用QToolButton的hitButton
    bool mWordWrap { false };           ///< 标记是否文字换行 @default false
    SARibbonToolButton::RibbonButtonType mButtonType { SARibbonToolButton::LargeButton };
    SARibbonToolButton::LargeButtonType mLargeButtonType { SARibbonToolButton::Normal };
    int mSpacing { 1 };             ///< 按钮和边框的距离
    int mIndicatorLen { 8 };        ///< Indicator的长度
    QRect mDrawIconRect;            ///< 记录icon的绘制位置
    QRect mDrawTextRect;            ///< 记录text的绘制位置
    QRect mDrawIndicatorArrowRect;  ///< 记录IndicatorArrow的绘制位置
    QSize mSizeHint;                ///< 保存计算好的sizehint
public:
    static bool s_liteStyleEnableWordWrap;  ///< 在lite模式下是否允许文字换行，如果允许，则图标相对比较小，默认不允许
};

//静态参数初始化
bool SARibbonToolButton::PrivateData::s_liteStyleEnableWordWrap = false;

SARibbonToolButton::PrivateData::PrivateData(SARibbonToolButton* p) : q_ptr(p)
{
}

void SARibbonToolButton::PrivateData::updateStatusByMousePosition(const QPoint& pos)
{
    bool isMouseOnSubControl(false);
    if (SARibbonToolButton::LargeButton == mButtonType) {
        isMouseOnSubControl = mDrawTextRect.united(mDrawIndicatorArrowRect).contains(pos);
    } else {
        //小按钮模式就和普通toolbutton一样
        isMouseOnSubControl = mDrawIndicatorArrowRect.contains(pos);
    }

    if (mMouseOnSubControl != isMouseOnSubControl) {
        mMouseOnSubControl = isMouseOnSubControl;
        //从icon变到text过程中刷新一次
        q_ptr->update();
    }
}

void SARibbonToolButton::PrivateData::updateDrawRect(const QStyleOptionToolButton& opt)
{
    if (!mSizeHint.isValid()) {
        updateSizeHint(opt);
    }
    //先更新IndicatorLen
    mIndicatorLen = q_ptr->style()->pixelMetric(QStyle::PM_MenuButtonIndicator, &opt, q_ptr);
    if (mIndicatorLen < 3) {
        if (SARibbonToolButton::LargeButton == mButtonType) {
            mIndicatorLen = 8;
        } else {
            mIndicatorLen = 12;  //小按钮模式下设置为10
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

void SARibbonToolButton::PrivateData::calcDrawRects(const QStyleOptionToolButton& opt,
                                                    QRect& iconRect,
                                                    QRect& textRect,
                                                    QRect& indicatorArrowRect,
                                                    int spacing,
                                                    int indicatorLen) const
{
    if (SARibbonToolButton::LargeButton == mButtonType) {
        if (SARibbonToolButton::Normal == mLargeButtonType) {
            calcLargeButtonNormalDrawRects(opt, iconRect, textRect, indicatorArrowRect, spacing, indicatorLen);
        } else {
            calcLargeButtonLiteDrawRects(opt, iconRect, textRect, indicatorArrowRect, spacing, indicatorLen);
        }
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
            indicatorArrowRect = QRect(opt.rect.right() - indicatorLen - spacing, iconRect.y(), indicatorLen, iconRect.height());
        } else {
            iconRect           = opt.rect.adjusted(spacing, spacing, -spacing, -spacing);
            indicatorArrowRect = QRect();
        }
        //文本区域为空
        textRect = QRect();
    } break;
    case Qt::ToolButtonTextOnly: {
        if (hasIndicator(opt)) {
            // 在仅有图标的小模式显示时，预留一个下拉箭头位置
            textRect           = opt.rect.adjusted(spacing, spacing, -indicatorLen - spacing, -spacing);
            indicatorArrowRect = QRect(opt.rect.right() - indicatorLen - spacing, spacing, indicatorLen, textRect.height());
        } else {
            textRect           = opt.rect.adjusted(spacing, spacing, -spacing, -spacing);
            indicatorArrowRect = QRect();
        }
        //绘图区域为空
        iconRect = QRect();
    } break;
    default: {
        bool hasInd = hasIndicator(opt);
        // icon Beside和under都是一样的
        QRect buttonRect = q_ptr->rect();
        buttonRect.adjust(spacing, spacing, -spacing, -spacing);
        //先设置IconRect
        if (opt.icon.isNull()) {
            //没有图标
            iconRect = QRect();
        } else {
            QSize iconSize = adjustIconSize(buttonRect, opt.iconSize);
            iconRect = QRect(buttonRect.x(), buttonRect.y(), iconSize.width(), qMax(iconSize.height(), buttonRect.height()));
        }
        //后设置TextRect
        if (opt.text.isEmpty()) {
            textRect = QRect();
        } else {
            //分有菜单和没菜单两种情况
            int adjx = iconRect.isValid() ? (iconRect.width() + spacing) : 0;  //在buttonRect上变换，因此如果没有图标是不用偏移spacing
            if (hasInd) {
                textRect = buttonRect.adjusted(adjx, 0, -indicatorLen, 0);
            } else {
                textRect = buttonRect.adjusted(adjx, 0, 0, 0);  //在buttonRect上变换，因此如果没有图标是不用偏移spacing
            }
        }
        //最后设置Indicator
        if (hasInd) {
            if (textRect.isValid()) {
                indicatorArrowRect = QRect(buttonRect.right() - indicatorLen + 1, textRect.y(), indicatorLen, textRect.height());
            } else if (iconRect.isValid()) {
                indicatorArrowRect = QRect(buttonRect.right() - indicatorLen + 1, iconRect.y(), indicatorLen, iconRect.height());
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
void SARibbonToolButton::PrivateData::calcLargeButtonNormalDrawRects(const QStyleOptionToolButton& opt,
                                                                     QRect& iconRect,
                                                                     QRect& textRect,
                                                                     QRect& indicatorArrowRect,
                                                                     int spacing,
                                                                     int indicatorLen) const
{
    int textHeight = calcTextDrawRectHeight(opt);
    //普通模式下，先从indicator开始
    // 普通模式下，是否有菜单都会有indicator的占用位置
    indicatorArrowRect = opt.rect.adjusted(spacing, opt.rect.height() - indicatorLen - spacing, -spacing, -spacing);
    //计算文本
    textRect = opt.rect.adjusted(spacing, indicatorArrowRect.top() - textHeight, -spacing, -indicatorArrowRect.height() - spacing);
    //剩下就是icon区域
    iconRect.setTopLeft(QPoint(spacing, spacing));
    iconRect.setBottomRight(textRect.topRight() - QPoint(0, spacing));
}
/**
 * @brief 计算大按钮模式下的绘制尺寸（紧凑）
 * @param opt
 * @param iconRect
 * @param textRect
 * @param indicatorArrowRect
 * @param spacing
 * @param indicatorLen
 */
void SARibbonToolButton::PrivateData::calcLargeButtonLiteDrawRects(const QStyleOptionToolButton& opt,
                                                                   QRect& iconRect,
                                                                   QRect& textRect,
                                                                   QRect& indicatorArrowRect,
                                                                   int spacing,
                                                                   int indicatorLen) const
{
    // lite模式下，先从indicator开始
    //计算文本
    int textHeight = calcTextDrawRectHeight(opt);
    if (hasIndicator(opt)) {
        textRect = opt.rect.adjusted(spacing, opt.rect.height() - textHeight - spacing, -indicatorLen - spacing, -spacing);
        //计算indicator
        indicatorArrowRect = QRect(textRect.right() + 1, textRect.top(), indicatorLen, textRect.height());
    } else {
        textRect = opt.rect.adjusted(spacing, opt.rect.height() - textHeight - spacing, -spacing, -spacing);
        //计算indicator
        indicatorArrowRect = QRect();
    }

    //剩下就是icon区域
    iconRect = opt.rect.adjusted(spacing, spacing, -spacing, -textRect.height() - 2 * spacing);
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
        //说明图标的icon过大，要匹配到buttonRect
        iconSize.setHeight(buttonRect.height());
        //等比例设置宽度
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
QSize SARibbonToolButton::PrivateData::calcSizeHint(const QStyleOptionToolButton& opt) const
{
    if (SARibbonToolButton::LargeButton == mButtonType) {
        if (SARibbonToolButton::Normal == mLargeButtonType) {
            return calcLargeButtonNormalSizeHint(opt);
        } else {
            return calcLargeButtonLiteSizeHint(opt);
        }
    }
    return calcSmallButtonSizeHint(opt);
}

QSize SARibbonToolButton::PrivateData::calcSmallButtonSizeHint(const QStyleOptionToolButton& opt) const
{
    int w = 0, h = 0;

    switch (opt.toolButtonStyle) {
    case Qt::ToolButtonIconOnly: {
        w = opt.iconSize.width() + 2 * mSpacing;
        h = opt.iconSize.height() + 2 * mSpacing;
    } break;
    case Qt::ToolButtonTextOnly: {
        QSize textSize = opt.fontMetrics.size(Qt::TextShowMnemonic, opt.text);
        textSize.setWidth(textSize.width() + opt.fontMetrics.horizontalAdvance(QLatin1Char(' ')) * 2);
        textSize.setHeight(calcTextDrawRectHeight(opt));
        w = textSize.width() + 2 * mSpacing;
        h = textSize.height() + 2 * mSpacing;
    } break;
    default: {
        //先加入icon的尺寸
        w = opt.iconSize.width() + 2 * mSpacing;
        h = opt.iconSize.height() + 2 * mSpacing;
        //再加入文本的长度
        if (!opt.text.isEmpty()) {
            QSize textSize = opt.fontMetrics.size(Qt::TextShowMnemonic, opt.text);
            textSize.setWidth(textSize.width() + opt.fontMetrics.horizontalAdvance(QLatin1Char(' ')) * 2);
            textSize.setHeight(calcTextDrawRectHeight(opt));
            w += mSpacing;
            w += textSize.width();
            h = qMax(h, (textSize.height() + (2 * mSpacing)));
        }
    }
    }
    if (hasIndicator(opt)) {
        //存在indicator的按钮，宽度尺寸要扩展
        w += mIndicatorLen;
    }
    if (w < 16) {
        w = 16;
    }
    //! Qt6.4 取消了QApplication::globalStrut
    return QSize(w, h).expandedTo(QSize(2, 2));
}

QSize SARibbonToolButton::PrivateData::calcLargeButtonNormalSizeHint(const QStyleOptionToolButton& opt) const
{
    int w = 0, h = opt.fontMetrics.lineSpacing() * 4.5;  // 3*1.5
    if (SARibbonPannel* pannel = qobject_cast< SARibbonPannel* >(q_ptr->parent())) {
        //对于建立在SARibbonPannel的基础上的大按钮，把高度设置为SARibbonPannel计算的大按钮高度
        h = pannel->largeHeight();
    }
    //估算字体的宽度作为宽度
    w = estimateLargeButtonTextWidth(h, calcTextDrawRectHeight(opt), opt.text, opt.fontMetrics);
    w += (2 * mSpacing);
    if (w < 22) {
        w = 22;
    }
    //! Qt6.4 取消了QApplication::globalStrut
    return QSize(w, h).expandedTo(QSize(2, 2));
}

QSize SARibbonToolButton::PrivateData::calcLargeButtonLiteSizeHint(const QStyleOptionToolButton& opt) const
{
    int w = 0, h = opt.fontMetrics.lineSpacing() * 4.5;  // 3*1.5
    if (SARibbonPannel* pannel = qobject_cast< SARibbonPannel* >(q_ptr->parent())) {
        //对于建立在SARibbonPannel的基础上的大按钮，把高度设置为SARibbonPannel计算的大按钮高度
        h = pannel->largeHeight();
    }
    //估算字体的宽度作为宽度
    w = estimateLargeButtonTextWidth(h, calcTextDrawRectHeight(opt), opt.text, opt.fontMetrics);
    w += (2 * mSpacing);
    if (hasIndicator(opt)) {
        //存在indicator的按钮，宽度尺寸要扩展
        w += mIndicatorLen;
    }
    if (w < 22) {
        w = 22;
    }
    //! Qt6.4 取消了QApplication::globalStrut
    return QSize(w, h).expandedTo(QSize(2, 2));
}

/**
 * @brief 计算文本高度
 * @param opt
 * @return
 */
int SARibbonToolButton::PrivateData::calcTextDrawRectHeight(const QStyleOptionToolButton& opt) const
{
    if (SARibbonToolButton::LargeButton == mButtonType) {
        if (SARibbonToolButton::Lite == mLargeButtonType) {
            if (SARibbonToolButton::PrivateData::s_liteStyleEnableWordWrap) {
                return opt.fontMetrics.lineSpacing() * 2.05;
            }
            return opt.fontMetrics.lineSpacing() * 1.3;
        }
        return opt.fontMetrics.lineSpacing() * 2.05;
    }
    //小按钮
    return opt.fontMetrics.lineSpacing() * 1.4;
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
                                                                  int maxTrycount) const
{
    QSize textSize = fm.size(Qt::TextShowMnemonic, text);
    textSize.setWidth(textSize.width() + fm.horizontalAdvance(QLatin1Char(' ')) * 2);

    if (textSize.width() < buttonHeight * widthHeightRatio) {
        //范围合理，直接返回
        return textSize.width();
    }
    //大于宽高比尝试进行文字换行
    int trycount  = 0;
    int alignment = Qt::TextShowMnemonic | Qt::TextWordWrap;
    //对于英文字体，直接宽度减半是无法满足完全显示两行的，需要进行预估
    QRect textRect(0, 0, textSize.width(), textDrawRectHeight);
    do {
        //先计算两行文本的紧凑矩形
        //从一半开始逐渐递增
        textRect.setWidth(textSize.width() / 2 + (textSize.width() / 2) * (float(trycount) / maxTrycount));
        textRect = fm.boundingRect(textRect, alignment, text);
        if (textRect.height() <= (fm.lineSpacing() * 2)) {
            //保证在两行
            break;
        }
        ++trycount;
        if (trycount > 1) {
            qDebug() << "estimateLargeButtonTextWidth,origin textSize=" << textSize << ",trycount=" << trycount
                     << ",textRect=" << textRect;
        }
    } while (trycount < 3);
    return textRect.width();
}

QPixmap SARibbonToolButton::PrivateData::createIconPixmap(const QStyleOptionToolButton& opt, const QSize& iconsize) const
{
    if (opt.icon.isNull()) {  //没有有图标
        return (QPixmap());
    }
    QIcon::State state = opt.state & QStyle::State_On ? QIcon::On : QIcon::Off;
    QIcon::Mode mode;
    if (!(opt.state & QStyle::State_Enabled)) {
        mode = QIcon::Disabled;
    } else if ((opt.state & QStyle::State_MouseOver) && (opt.state & QStyle::State_AutoRaise)) {
        mode = QIcon::Active;
    } else {
        mode = QIcon::Normal;
    }
    // return (opt.icon.pixmap(this->window()->windowHandle(), opt.rect.size().boundedTo(realConSize), mode, state));
    if (mLargeButtonType == SARibbonToolButton::Lite && !s_liteStyleEnableWordWrap) {
        // lite模式，图标不用过于饱满,减去2px
        return (opt.icon.pixmap(iconsize - QSize(2, 2), mode, state));
    }
    return (opt.icon.pixmap(iconsize, mode, state));
}

int SARibbonToolButton::PrivateData::getTextAlignment() const
{
    int alignment = Qt::TextShowMnemonic;
    if (LargeButton == mButtonType) {
        if (Lite == mLargeButtonType) {
            alignment |= (Qt::AlignLeft | Qt::AlignVCenter);
            if (isLiteStyleEnableWordWrap()) {
                alignment |= Qt::TextWordWrap;
            }
        } else {
            alignment |= Qt::TextWordWrap | Qt::AlignHCenter | Qt::AlignTop;  //文字是顶部对齐;
        }
    } else {
        alignment |= Qt::AlignLeft | Qt::AlignVCenter;
    }
    return alignment;
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
#if SA_RIBBON_TOOLBUTTON_DEBUG_PRINT
    qDebug() << "SARibbonToolButton::resizeEvent, text=" << text() << " obj=" << objectName() << " size=" << e->size();
#endif
    //在resizeevent计算绘图所需的尺寸，避免在绘图过程中实时绘制提高效率
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
    //    if (!d_ptr->mSizeHint.isValid() || d_ptr->mSizeHint.width() <= 22) {  // 22是给与sizehint的最小值，如果小于这个值，重新计算一下
    //        QStyleOptionToolButton opt;
    //        initStyleOption(&opt);
    //        d_ptr->updateSizeHint(opt);
    //    }
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
        //在菜单弹出消失后，需要通过此方法取消掉鼠标停留
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
    bool autoRaise = opt.state & QStyle::State_AutoRaise;
    //绘制按钮
    if (autoRaise) {
        style()->drawPrimitive(QStyle::PE_PanelButtonTool, &opt, &p, this);
    } else {
        style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &opt, &p, this);
    }
    //针对MenuButtonPopup的ribbon样式的特殊绘制
    if ((opt.subControls & QStyle::SC_ToolButton) && (opt.features & QStyleOptionToolButton::MenuButtonPopup)) {
        if (opt.state & QStyle::State_MouseOver) {                       //鼠标在按钮上才进行绘制
            if (!(opt.activeSubControls & QStyle::SC_ToolButtonMenu)) {  //按钮的菜单弹出时不做处理
                if (LargeButton == d_ptr->mButtonType) {                 //大按钮模式
                    if (d_ptr->mMouseOnSubControl) {                     //此时鼠标在indecater那
                        //鼠标在文字区，把图标显示为正常
                        QStyleOption tmp = opt;
                        tmp.rect         = d_ptr->mDrawIconRect;
                        tmp.state        = (QStyle::State_Raised);  //把图标区域显示为正常
                        if (autoRaise) {
                            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tmp, &p, this);
                        } else {
                            style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tmp, &p, this);
                        }
                    } else {
                        //鼠标在图标区，把文字显示为正常
                        QStyleOption tmp = opt;
                        tmp.state        = (QStyle::State_Raised);  //把图标区域显示为正常
                        //文字和Indicator都显示正常
                        tmp.rect = d_ptr->mDrawTextRect.united(d_ptr->mDrawIndicatorArrowRect);
                        if (autoRaise) {
                            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tmp, &p, this);
                        } else {
                            style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tmp, &p, this);
                        }
                    }
                } else {                              //小按钮模式
                    if (d_ptr->mMouseOnSubControl) {  //此时鼠标在indecater那
                        //鼠标在文字区，把图标和文字显示为正常
                        QStyleOption tmp = opt;
                        tmp.rect         = d_ptr->mDrawIconRect.united(d_ptr->mDrawTextRect);
                        tmp.state        = (QStyle::State_Raised);  //把图标区域显示为正常
                        if (autoRaise) {
                            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tmp, &p, this);
                        } else {
                            style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tmp, &p, this);
                        }
                    } else {
                        //鼠标在图标区，把文字显示为正常
                        QStyleOption tmp = opt;
                        tmp.state        = (QStyle::State_Raised);  //把图标区域显示为正常
                        //文字和Indicator都显示正常
                        tmp.rect = d_ptr->mDrawIndicatorArrowRect;
                        if (autoRaise) {
                            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tmp, &p, this);
                        } else {
                            style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tmp, &p, this);
                        }
                    }
                }
            }
        }
    }
    //绘制Focus
    //    if (opt.state & QStyle::State_HasFocus) {
    //        QStyleOptionFocusRect fr;
    //        fr.QStyleOption::operator=(opt);
    //        fr.rect.adjust(d_ptr->mSpacing, d_ptr->mSpacing, -d_ptr->mSpacing, -d_ptr->mSpacing);
    //        style()->drawPrimitive(QStyle::PE_FrameFocusRect, &fr, &p, this);
    //    }
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
    if (Lite == d_ptr->mLargeButtonType && !isLiteStyleEnableWordWrap()) {
        // lite 模式，文字不换行
        //显示的内容需要进行省略处理
        style()->drawItemText(&p,
                              textDrawRect,
                              alignment,
                              opt.palette,
                              opt.state & QStyle::State_Enabled,
                              opt.fontMetrics.elidedText(opt.text, Qt::ElideRight, textDrawRect.width(), alignment),
                              QPalette::ButtonText);
    } else {
        style()->drawItemText(&p, textDrawRect, alignment, opt.palette, opt.state & QStyle::State_Enabled, opt.text, QPalette::ButtonText);
    }
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

void SARibbonToolButton::changeEvent(QEvent* e)
{
    if (e) {
        if (e->type() == QEvent::FontChange) {
            // todo 说明字体改变，需要重新计算和字体相关的信息
            updateRect();
            //            calcIconAndTextRect(opt);
        }
    }
    QToolButton::changeEvent(e);
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
    //计算iconrect
    //根据字体计算文字的高度

    if (LargeButton == buttonType) {
        setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    } else {
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    }

    updateRect();
}

QSize SARibbonToolButton::minimumSizeHint() const
{
    return (sizeHint());
}

void SARibbonToolButton::setLargeButtonType(SARibbonToolButton::LargeButtonType type)
{
    d_ptr->mLargeButtonType = type;
    updateRect();
}

SARibbonToolButton::LargeButtonType SARibbonToolButton::largeButtonType() const
{
    return (d_ptr->mLargeButtonType);
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
    //    d_ptr->updateSizeHint(opt);
    d_ptr->updateDrawRect(opt);
}

/**
 * @brief 设置在lite模式下是否允许文字换行，如果允许，则图标相对比较小，默认不允许
 * @param on
 * @note 此函数的调用最好在ribbonbar布局之前设置，且调用之后需要调用@sa SARibbonStyleOption::recalc 刷新
 * @code
 * RibbonSubElementStyleOpt.recalc();
 * @endcode
 */
void SARibbonToolButton::setLiteStyleEnableWordWrap(bool on)
{
    SARibbonToolButton::PrivateData::s_liteStyleEnableWordWrap = on;
}

/**
 * @brief 在lite模式下是否允许文字换行
 * @return
 */
bool SARibbonToolButton::isLiteStyleEnableWordWrap()
{
    return SARibbonToolButton::PrivateData::s_liteStyleEnableWordWrap;
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
