#include "SARibbonToolButton.h"
#include <QApplication>
#include <QStylePainter>
#include <QStyleOptionToolButton>
#include <QAction>
#include <QDebug>
#include <QStyleOptionFocusRect>
#include <QStyleOption>
#include <QPaintEvent>
#include <QMouseEvent>
#include "SARibbonDrawHelper.h"
#include "SARibbonElementManager.h"
#include "QCursor"

#define SA_WIDTH_HEIGHT_RATIO 1.2
/**
 * @def 开启此宏会打印一些常见信息
 */
#define DebugSARibbonToolButton_TextDrawPrint 0

#define SA_RIBBON_DEBUG_HELP_DRAW

#ifdef SA_RIBBON_DEBUG_HELP_DRAW
#define SARIBBONTOOLBUTTON_DEBUG_DRAW_RECT(p, rect)                                                                    \
    do {                                                                                                               \
        p.save();                                                                                                      \
        p.setPen(Qt::red);                                                                                             \
        p.setBrush(QBrush());                                                                                          \
        p.drawRect(rect);                                                                                              \
        p.restore();                                                                                                   \
    } while (0)

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
#else
#define SARIBBONTOOLBUTTON_DEBUG_DRAW_RECT(p, rect)
#endif

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
    int mSpacing { 3 };             ///< 按钮和边框的距离
    int mIndicatorLen { 8 };        ///< Indicator的长度
    QRect mDrawIconRect;            ///< 记录icon的绘制位置
    QRect mDrawTextRect;            ///< 记录text的绘制位置
    QRect mDrawIndicatorArrowRect;  ///< 记录IndicatorArrow的绘制位置
    QSize mSizeHint { 18, 30 };     ///< 保存计算好的sizehint
public:
    static bool s_isToolButtonTextShift;  ///< 配置SARibbonToolButton文字在点击时是否会轻微改变位置而达到一种类似跳动的效果,@default 默认为false
    static bool s_liteStyleEnableWordWrap;  ///< 在lite模式下是否允许文字换行，如果允许，则图标相对比较小，默认不允许
};

//静态参数初始化
bool SARibbonToolButton::PrivateData::s_isToolButtonTextShift   = false;
bool SARibbonToolButton::PrivateData::s_liteStyleEnableWordWrap = false;

SARibbonToolButton::PrivateData::PrivateData(SARibbonToolButton* p) : q_ptr(p)
{
}

void SARibbonToolButton::PrivateData::updateStatusByMousePosition(const QPoint& pos)
{
    bool isMouseOnSubControl(false);
    if (SARibbonToolButton::LargeButton == mButtonType) {
        if (SARibbonToolButton::Normal == mLargeButtonType) {
            //三行模式
            //大按钮模式下，图标绘制区域是非SubControl(Indicator)
            if (mDrawIconRect.isValid()) {
                //获取Indicator的区域
                isMouseOnSubControl = q_ptr->rect().adjusted(0, mDrawIconRect.height() + 2 * mSpacing, 0, 0).contains(pos);
            }
        } else {
            //两行模式
            if (mDrawIconRect.isValid()) {
                //获取Indicator的区域
                //两行模式icon和text之间没有spacing
                isMouseOnSubControl = q_ptr->rect().adjusted(0, mDrawIconRect.height() + mSpacing, 0, 0).contains(pos);
            }
        }
    } else {
        //小按钮模式就和普通toolbutton一样
        isMouseOnSubControl = !q_ptr->rect().adjusted(0, 0, -mIndicatorLen, 0).contains(pos);
    }

    if (mMouseOnSubControl != isMouseOnSubControl) {
        mMouseOnSubControl = isMouseOnSubControl;
        //从icon变到text过程中刷新一次
        q_ptr->update();
    }
}

void SARibbonToolButton::PrivateData::updateDrawRect(const QStyleOptionToolButton& opt)
{
    //先更新IndicatorLen
    mIndicatorLen = q_ptr->style()->pixelMetric(QStyle::PM_MenuButtonIndicator, &opt, q_ptr);
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
            iconRect.adjust(spacing, spacing, -indicatorLen - spacing, -spacing);
            indicatorArrowRect = opt.rect.adjusted(iconRect.right(), spacing, -spacing, -spacing);
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
            textRect.adjust(spacing, spacing, -indicatorLen - spacing, -spacing);
            indicatorArrowRect = opt.rect.adjusted(textRect.right(), spacing, -spacing, -spacing);
        } else {
            textRect           = opt.rect.adjusted(spacing, spacing, -spacing, -spacing);
            indicatorArrowRect = QRect();
        }
        //绘图区域为空
        iconRect = QRect();
    } break;
    default: {
        // icon Beside和under都是一样的
        QRect buttonRect = q_ptr->rect();
        buttonRect.adjust(spacing, spacing, -spacing, -spacing);
        //先设置IconRect
        if (opt.icon.isNull()) {
            //没有图标
            iconRect = QRect();
        } else {
            QSize iconSize = adjustIconSize(buttonRect, opt.iconSize);
            iconRect       = QRect(buttonRect.x(),
                             buttonRect.y() + (buttonRect.height() - iconSize.height()) / 2,
                             iconSize.width(),
                             iconSize.height());
        }
        //后设置TextRect
        if (opt.text.isEmpty()) {
            textRect = QRect();
        } else {
            //分有菜单和没菜单两种情况
            int adjx = iconRect.isValid() ? (iconRect.right() + spacing) : 0;  //在buttonRect上变换，因此如果没有图标是不用偏移spacing
            if (hasIndicator(opt)) {
                textRect           = buttonRect.adjusted(adjx, 0, -indicatorLen, 0);
                indicatorArrowRect = buttonRect.adjusted(textRect.right(), 0, 0, 0);
            } else {
                textRect = buttonRect.adjusted(adjx, 0, 0, 0);  //在buttonRect上变换，因此如果没有图标是不用偏移spacing
                indicatorArrowRect = QRect();
            }
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
    textRect = opt.rect.adjusted(spacing, indicatorArrowRect.top() - textHeight, -spacing, -indicatorArrowRect.top());
    //剩下就是icon区域
    iconRect = opt.rect.adjusted(spacing, spacing, -spacing, -textRect.top() - spacing);
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
        indicatorArrowRect = QRect(textRect.right(), textRect.top(), indicatorLen, textRect.height());
    } else {
        textRect = opt.rect.adjusted(spacing, opt.rect.height() - textHeight - spacing, -spacing, -spacing);
        //计算indicator
        indicatorArrowRect = QRect();
    }

    //剩下就是icon区域
    iconRect = opt.rect.adjusted(spacing, spacing, -spacing, -textRect.top() - spacing);
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
        QSize textSize = opt.fontMetrics.size(Qt::TextShowMnemonic, opt.text);
        textSize.setWidth(textSize.width() + opt.fontMetrics.horizontalAdvance(QLatin1Char(' ')) * 2);
        textSize.setHeight(calcTextDrawRectHeight(opt));
        w += mSpacing;
        w += textSize.width();
        h = qMax(h, (textSize.height() + (2 * mSpacing)));
    }
    }
    if (hasIndicator(opt)) {
        //存在indicator的按钮，宽度尺寸要扩展
        w += mIndicatorLen;
    }
    return QSize(w, h).expandedTo(QApplication::globalStrut());
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
    qDebug() << "calcLargeButtonNormalSizeHint=" << QSize(w, h);
    return QSize(w, h).expandedTo(QApplication::globalStrut());
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
    return QSize(w, h).expandedTo(QApplication::globalStrut());
}

/**
 * @brief 计算文本高度
 * @param opt
 * @return
 */
int SARibbonToolButton::PrivateData::calcTextDrawRectHeight(const QStyleOptionToolButton& opt) const
{
    if (SARibbonToolButton::PrivateData::s_liteStyleEnableWordWrap) {
        return opt.fontMetrics.lineSpacing() * 2.1;
    }
    return opt.fontMetrics.lineSpacing() * 1.2;
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
    return (opt.icon.pixmap(q_ptr->window()->windowHandle(), iconsize, mode, state));
}

int SARibbonToolButton::PrivateData::getTextAlignment() const
{
    int alignment = Qt::TextShowMnemonic;
    if (LargeButton == mButtonType) {
        if (Lite == mLargeButtonType) {
            alignment |= Qt::AlignCenter;
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

SARibbonToolButton::SARibbonToolButton(QWidget* parent) : QToolButton(parent), d_ptr(new PrivateData(this))
{
    setAutoRaise(true);
    setButtonType(SmallButton);
}

SARibbonToolButton::SARibbonToolButton(QAction* defaultAction, QWidget* parent)
    : QToolButton(parent), d_ptr(new PrivateData(this))
{
    setAutoRaise(true);
    setButtonType(SmallButton);
    setDefaultAction(defaultAction);
}

SARibbonToolButton::~SARibbonToolButton()
{
}

void SARibbonToolButton::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);
    QPainter p(this);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    paintButton(p, opt);
    paintIcon(p, opt);
    paintText(p, opt);
    //    switch (d_ptr->mButtonType) {
    //    case LargeButton:
    //        paintLargeButton(event);
    //        return;

    //    case SmallButton:
    //        paintSmallButton(event);
    //        return;

    //    default:
    //        return;
    //    }
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
            return;
        }
    }
    d_ptr->mMenuButtonPressed = false;
    QToolButton::mousePressEvent(e);
}

void SARibbonToolButton::mouseReleaseEvent(QMouseEvent* e)
{
    QToolButton::mouseReleaseEvent(e);
    d_ptr->mMenuButtonPressed = false;
}

void SARibbonToolButton::focusOutEvent(QFocusEvent* e)
{
    QToolButton::focusOutEvent(e);
    d_ptr->mMouseOnSubControl = false;
}

void SARibbonToolButton::leaveEvent(QEvent* e)
{
    d_ptr->mMouseOnSubControl = false;
    QToolButton::leaveEvent(e);
}

void SARibbonToolButton::paintSmallButton(QPaintEvent* e)
{
    Q_UNUSED(e);
    QStylePainter p(this);
    QStyleOptionToolButton opt;

    initStyleOption(&opt);

    bool autoRaise = opt.state & QStyle::State_AutoRaise;
    //有Indicator
    if (opt.features & QStyleOptionToolButton::MenuButtonPopup || opt.features & QStyleOptionToolButton::HasMenu) {
        if (opt.activeSubControls & QStyle::SC_ToolButtonMenu) {
            //激活了subcontrol
            //菜单激活,整个按钮都绘制为选中
            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &opt, &p, this);
        } else {
            QStyleOption tool = opt;
            //菜单没有激活，就要判断鼠标所在的区域
            //先绘制一个激活的按钮
            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
            if (tool.state & QStyle::State_MouseOver) {
                if (d_ptr->mMouseOnSubControl) {  //此时鼠标在indecater那
                    //鼠标在文字区，把图标显示为正常
                    tool.rect  = d_ptr->mDrawIconRect;
                    tool.state = (QStyle::State_Raised);  //把图标区域显示为正常
                    if (autoRaise) {
                        style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
                    } else {
                        style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
                    }
                } else {
                    //鼠标在图标区，把文字显示为正常
                    tool.state = (QStyle::State_Raised);  //把图标区域显示为正常
                    tool.rect  = opt.rect.adjusted(d_ptr->mDrawIconRect.width() + d_ptr->mSpacing,
                                                  d_ptr->mSpacing,
                                                  -d_ptr->mSpacing,
                                                  -d_ptr->mSpacing);
                    if (autoRaise) {
                        style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
                    } else {
                        style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
                    }
                }
            }
        }
    } else {
        if (autoRaise) {
            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &opt, &p, this);
        } else {
            style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &opt, &p, this);
        }
    }
    drawMenuButtonPopupIndicatorArrow(p, opt);
    drawIconAndLabel(p, opt);
}

void SARibbonToolButton::paintLargeButton(QPaintEvent* e)
{
    Q_UNUSED(e);
    QPainter p(this);
    QStyleOptionToolButton opt;

    initStyleOption(&opt);
    bool autoRaise = opt.state & QStyle::State_AutoRaise;
    // MenuButtonPopup特殊处理
    if ((opt.subControls & QStyle::SC_ToolButton) && (opt.features & QStyleOptionToolButton::MenuButtonPopup)) {
        //此时按钮的菜单弹出
        //先把整个按钮绘制为选中
        style()->drawPrimitive(QStyle::PE_PanelButtonTool, &opt, &p, this);
        if (!(opt.activeSubControls & QStyle::SC_ToolButtonMenu)) {
            //菜单激活 不做处理
            //菜单没有激活,这时候要把图标域或者文字域绘制为正常模式
            // style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
            if (opt.state & QStyle::State_MouseOver) {
                if (d_ptr->mMouseOnSubControl) {  //此时鼠标在indecater那
                    //鼠标在文字区，把图标显示为正常
                    QStyleOption tool = opt;
                    tool.rect         = d_ptr->mDrawIconRect;
                    tool.state        = (QStyle::State_Raised);  //把图标区域显示为正常
                    if (autoRaise) {
                        style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
                    } else {
                        style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
                    }
                } else {
                    //鼠标在图标区，把文字显示为正常
                    QStyleOption tool = opt;
                    tool.state        = (QStyle::State_Raised);  //把图标区域显示为正常
                    tool.rect.setRect(d_ptr->mDrawTextRect.x(),
                                      d_ptr->mDrawTextRect.y(),
                                      tool.rect.width() - 2 * d_ptr->mSpacing,
                                      d_ptr->mDrawTextRect.height());
                    if (autoRaise) {
                        style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
                    } else {
                        style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
                    }
                }
            }
        }
    } else {
        if (autoRaise) {
            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &opt, &p, this);
        } else {
            style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &opt, &p, this);
        }
    }

    //绘制Focus
    if (opt.state & QStyle::State_HasFocus) {
        QStyleOptionFocusRect fr;
        fr.QStyleOption::operator=(opt);
        fr.rect.adjust(d_ptr->mSpacing, d_ptr->mSpacing, -d_ptr->mSpacing, -d_ptr->mSpacing);
        if (opt.features & QStyleOptionToolButton::MenuButtonPopup) {
            fr.rect.adjust(0, 0, -d_ptr->mIndicatorLen, 0);
        }
        style()->drawPrimitive(QStyle::PE_FrameFocusRect, &fr, &p, this);
    }

    drawIconAndLabel(p, opt);
}

bool SARibbonToolButton::hitButton(const QPoint& pos) const
{
    if (QAbstractButton::hitButton(pos)) {
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
#if DebugSARibbonToolButton_TextDrawPrint
    qDebug() << "SARibbonToolButton::resizeEvent, text=" << text() << " obj=" << objectName() << " size=" << e->size();
#endif
    //在resizeevent计算绘图所需的尺寸，避免在绘图过程中实时绘制提高效率
    QToolButton::resizeEvent(e);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    d_ptr->updateDrawRect(opt);
}

/**
 * @brief toolbutton的尺寸确定是先定下字体的尺寸，再定下icon的尺寸，自底向上，保证字体能显示两行
 * @note m_sizeHint的刷新需要注意
 * @return
 */
QSize SARibbonToolButton::sizeHint() const
{
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    d_ptr->updateSizeHint(opt);
    qDebug() << "SARibbonToolButton(" << objectName() << ") sizehint=" << d_ptr->mSizeHint;
    return d_ptr->mSizeHint;
    /*
    SARibbonToolButton* that = const_cast< SARibbonToolButton* >(this);
    that->reCalcSizeHint();
    return d_ptr->mSizeHint;
    */
}

/**
 * @brief 绘制按钮
 * @param p
 * @param opt
 */
void SARibbonToolButton::paintButton(QPainter& p, const QStyleOptionToolButton& opt)
{
    bool autoRaise = opt.state & QStyle::State_AutoRaise;
    if ((opt.subControls & QStyle::SC_ToolButton) && (opt.features & QStyleOptionToolButton::MenuButtonPopup)) {
        //此时按钮的菜单弹出
        //先把整个按钮绘制为选中
        style()->drawPrimitive(QStyle::PE_PanelButtonTool, &opt, &p, this);
        if (!(opt.activeSubControls & QStyle::SC_ToolButtonMenu)) {
            //菜单激活 不做处理
            //菜单没有激活,这时候要把图标域或者文字域绘制为正常模式
            // style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
            if (opt.state & QStyle::State_MouseOver) {
                if (d_ptr->mMouseOnSubControl) {  //此时鼠标在indecater那
                    //鼠标在文字区，把图标显示为正常
                    QStyleOption tool = opt;
                    tool.rect         = d_ptr->mDrawIconRect;
                    tool.state        = (QStyle::State_Raised);  //把图标区域显示为正常
                    if (autoRaise) {
                        style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
                    } else {
                        style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
                    }
                } else {
                    //鼠标在图标区，把文字显示为正常
                    QStyleOption tool = opt;
                    tool.state        = (QStyle::State_Raised);  //把图标区域显示为正常
                    tool.rect.setRect(d_ptr->mDrawTextRect.x(),
                                      d_ptr->mDrawTextRect.y(),
                                      tool.rect.width() - 2 * d_ptr->mSpacing,
                                      d_ptr->mDrawTextRect.height());
                    if (autoRaise) {
                        style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
                    } else {
                        style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
                    }
                }
            }
        }
    } else {
        if (autoRaise) {
            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &opt, &p, this);
        } else {
            style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &opt, &p, this);
        }
    }

    //绘制Focus
    if (opt.state & QStyle::State_HasFocus) {
        QStyleOptionFocusRect fr;
        fr.QStyleOption::operator=(opt);
        fr.rect.adjust(d_ptr->mSpacing, d_ptr->mSpacing, -d_ptr->mSpacing, -d_ptr->mSpacing);
        if (opt.features & QStyleOptionToolButton::MenuButtonPopup) {
            fr.rect.adjust(0, 0, -d_ptr->mIndicatorLen, 0);
        }
        style()->drawPrimitive(QStyle::PE_FrameFocusRect, &fr, &p, this);
    }
}

/**
 * @brief 绘制图标
 * @param p
 * @param opt
 */
void SARibbonToolButton::paintIcon(QPainter& p, const QStyleOptionToolButton& opt)
{
    if (!d_ptr->mDrawIconRect.isValid()) {
        return;
    }

    QPixmap pm = d_ptr->createIconPixmap(opt, d_ptr->mDrawIconRect.size());
    style()->drawItemPixmap(&p, d_ptr->mDrawIconRect, Qt::AlignCenter, pm);
    SARIBBONTOOLBUTTON_DEBUG_DRAW_RECT(p, d_ptr->mDrawIconRect);
    qDebug() << "paintIcon rect=" << d_ptr->mDrawIconRect;
}

/**
 * @brief 绘制文本
 * @param p
 * @param opt
 */
void SARibbonToolButton::paintText(QPainter& p, const QStyleOptionToolButton& opt)
{
    int alignment = d_ptr->getTextAlignment();

    if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &opt, this)) {
        alignment |= Qt::TextHideMnemonic;
    }
    style()->drawItemText(&p, d_ptr->mDrawTextRect, alignment, opt.palette, opt.state & QStyle::State_Enabled, opt.text, QPalette::ButtonText);
    SARIBBONTOOLBUTTON_DEBUG_DRAW_RECT(p, d_ptr->mDrawTextRect);
}

/**
 * @brief 重新计算sizehint，sizehint函数在第一次计算完成后会记录结果，如果文字字体或者内容不变是不需要进行重复计算
 *
 * 正常来讲，高度是不需要管，因为高度是有SARibbonPannelLayout来控制，这个函数主要是为了提供宽度信息，宽度信息给到SARibbonPannelLayout，
 * 然后对button进行布局，最终会触发resize函数，resize函数会再进行一次计算确定具体的尺寸
 *
 * 因此，resize函数确定的尺寸方式必须和reCalcSizeHint保持一致，尤其是涉及字体设置
 *
 * 一般布局前调用sizehint，布局后会调用resizeevent，尤其字体信息会计算两遍，这里后续可以有更好的优化空间
 */
void SARibbonToolButton::reCalcSizeHint(QSize s)
{

    QStyleOptionToolButton opt;

    initStyleOption(&opt);
    d_ptr->mIndicatorLen = style()->pixelMetric(QStyle::PM_MenuButtonIndicator, &opt, this);

    // QToolButton的sizeHint已经考虑了菜单箭头的位置
    //从源码看，QToolButton的sizeHint是不会考虑换行的
    if (LargeButton == buttonType()) {
        //计算最佳大小
        if (SARibbonPannel* pannel = qobject_cast< SARibbonPannel* >(parent())) {
            //对于建立在SARibbonPannel的基础上的大按钮，把高度设置为SARibbonPannel计算的大按钮高度
            s.setHeight(pannel->largeHeight());
        }
        if (Normal == d_ptr->mLargeButtonType
            || (Lite == d_ptr->mLargeButtonType && SARibbonToolButton::PrivateData::s_liteStyleEnableWordWrap)) {
            //普通模式下才涉及到文字换行，或者lite模式下指定了文字换行
            if (s.width() > s.height() * SA_WIDTH_HEIGHT_RATIO) {
                //说明是一个长方形按钮
                //文本对齐方式
                //如果宽度大于高度，就看看换行是否能满足
                QFontMetrics fm = fontMetrics();
                QRect textRange(0, 0, s.width(), s.height());

                //! 1 先获取最优的文字textRange
                {
                    const int maxTrycount = 3;
                    int trycount          = 0;
                    int alignment         = Qt::TextShowMnemonic | Qt::TextWordWrap;
                    do {
                        //先计算两行文本的紧凑矩形
                        //从一半开始逐渐递增
                        textRange.setWidth(s.width() / 2 + (s.width() / 2) * (float(trycount) / maxTrycount));
                        textRange = fm.boundingRect(textRange, alignment, text());
                        if (textRange.height() <= (fm.lineSpacing() * 2)) {
                            //保证在两行
                            d_ptr->mWordWrap = (textRange.height() > fm.lineSpacing());
                            break;
                        }
                        ++trycount;
                    } while (trycount < 3);
                }
                //左右留2像素
                s.setWidth(textRange.width() + d_ptr->mSpacing * 2);
                //! 2再判断箭头来调整textRange
                //把区域设置给size
                //确认是否换行
                if ((opt.features & QStyleOptionToolButton::MenuButtonPopup) || (opt.features & QStyleOptionToolButton::HasMenu)) {
                    //如果有菜单
                    if (largeButtonType() == Lite) {
                        // lite模式下都要偏移
                        s.rwidth() += d_ptr->mIndicatorLen;

                    } else {
                        // Normal模式下，仅对换行的偏移
                        if (d_ptr->mWordWrap) {
                            s.rwidth() += d_ptr->mIndicatorLen;
                        }
                    }
                }
            } else {
                d_ptr->mWordWrap = text().contains('\n');
                if ((opt.features & QStyleOptionToolButton::MenuButtonPopup) || (opt.features & QStyleOptionToolButton::HasMenu)) {
                    //如果有菜单
                    if (d_ptr->mWordWrap) {
                        s.rwidth() += d_ptr->mIndicatorLen;
                    }
                }
            }
        } else {
            //否则就是lite模式，只允许1行，有菜单就偏移
            if (s.width() > s.height() * 1.5) {
                //过于宽的按钮，把文字用...来替代
                s.rwidth() = s.height() * 1.5;
            }
            if ((opt.features & QStyleOptionToolButton::MenuButtonPopup) || (opt.features & QStyleOptionToolButton::HasMenu)) {
                //如果有菜单
                s.rwidth() += d_ptr->mIndicatorLen;
            }
        }
        //无需在这里进行计算，在resizeevent里进行计算
        //! 3在这时候能确定m_textRect,m_iconRect
        //        that->m_textRect = textRange.moveBottomLeft(QPoint(s.width() - m_iconAndTextSpace, s.height() -
        //        m_iconAndTextSpace)); that->calcIconRect(QRect(0, 0, s.width(), s.height()), m_textRect, opt);
    } else {
        // InstantPopup在qtoolbutton不会添加控件来放下箭头，这里处理的和MenuButtonPopup一致
        // 在仅有图标的小模式显示时，预留一个下拉箭头位置
        if (Qt::ToolButtonIconOnly == toolButtonStyle()) {
            if (opt.features & QStyleOptionToolButton::MenuButtonPopup || opt.features & QStyleOptionToolButton::HasMenu) {
                //如果有菜单
                s.rwidth() += d_ptr->mIndicatorLen;
            }
        }
    }
    d_ptr->mSizeHint = s;
}

/**
 * @brief SARibbonToolButton::drawIconAndLabel
 *
 * 在LargeButtonType == Normal模式下，icon占大按钮的一半区域，在wps模式下，icon占大按钮的60%，文字占40%，且文字不换行
 * @param p
 * @param opt
 */
void SARibbonToolButton::drawIconAndLabel(QPainter& p, QStyleOptionToolButton& opt)
{
    if (LargeButton == d_ptr->mButtonType) {
        //绘制图标和文字
        QRect textRect = adjustedTextRect(opt);
        bool hasArrow  = opt.features & QStyleOptionToolButton::Arrow;
        if (((!hasArrow && opt.icon.isNull()) && !opt.text.isEmpty()) || (opt.toolButtonStyle == Qt::ToolButtonTextOnly)) {
            //没有箭头 且 没图标 有文字 || 只有文字模式
            int alignment = Qt::AlignCenter | Qt::TextShowMnemonic | Qt::TextWordWrap;  //纯文本下，居中对齐,换行
            if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &opt, this)) {
                alignment |= Qt::TextHideMnemonic;
            }
            p.setFont(opt.font);
            style()->drawItemText(&p, textRect, alignment, opt.palette, opt.state & QStyle::State_Enabled, opt.text, QPalette::ButtonText);
        } else {
            //非纯文本
            if (opt.toolButtonStyle != Qt::ToolButtonIconOnly) {
                //文本加图标情况
                p.setFont(opt.font);
                int alignment = Qt::TextShowMnemonic | Qt::TextWordWrap;  // 换行
                if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &opt, this)) {
                    alignment |= Qt::TextHideMnemonic;
                }
                //文字在icon下
                //先绘制图标
                if (!hasArrow) {

                    QPixmap pm = d_ptr->createIconPixmap(opt, d_ptr->mDrawIconRect.size());
                    style()->drawItemPixmap(&p, d_ptr->mDrawIconRect, Qt::AlignCenter, pm);
                } else {
                    drawArrow(style(), &opt, d_ptr->mDrawIconRect, &p, this);
                }

                SARIBBONTOOLBUTTON_DEBUG_DRAW_RECT(p, textRect);

                if (Normal == d_ptr->mLargeButtonType) {
                    alignment |= Qt::AlignHCenter | Qt::AlignTop;  //文字是顶部对齐
                } else {
                    alignment |= Qt::AlignCenter;
                }

                //再绘制文本，对于Normal模式下的Largebutton，如果有菜单，且m_isWordWrap是true，箭头将在文本旁边
                if (Lite == d_ptr->mLargeButtonType && !SARibbonToolButton::PrivateData::s_liteStyleEnableWordWrap) {
                    // lite 模式，文字不换行
                    //显示的内容需要进行省略处理
                    opt.text = fontMetrics().elidedText(opt.text, Qt::ElideRight, textRect.width(), Qt::TextShowMnemonic);
                }
                style()->drawItemText(&p,
                                      QStyle::visualRect(opt.direction, opt.rect, textRect),
                                      alignment,
                                      opt.palette,
                                      opt.state & QStyle::State_Enabled,
                                      opt.text,
                                      QPalette::ButtonText);
            } else {
                //只有图标情况
                if (hasArrow) {
                    drawArrow(style(), &opt, opt.rect, &p, this);
                } else {
                    QPixmap pm = d_ptr->createIconPixmap(opt, d_ptr->mDrawIconRect.size());
                    style()->drawItemPixmap(&p, d_ptr->mDrawIconRect, Qt::AlignCenter, pm);
                }
            }
            //绘制sub control 的下拉箭头
            if (opt.features & QStyleOptionToolButton::MenuButtonPopup || opt.features & QStyleOptionToolButton::HasMenu) {
                opt.rect = calcIndicatorArrowDownRect(opt);

                SARIBBONTOOLBUTTON_DEBUG_DRAW_RECT(p, opt.rect);
                style()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &opt, &p, this);
            }
        }
    } else {
        //小图标
        if (!opt.icon.isNull()) {
            QPixmap pm = d_ptr->createIconPixmap(opt, d_ptr->mDrawIconRect.size());

            if (opt.toolButtonStyle != Qt::ToolButtonIconOnly) {
                p.save();
                p.setFont(opt.font);

                QRect pr      = d_ptr->mDrawIconRect;                         //图标区域
                QRect tr      = opt.rect.adjusted(pr.width() + 2, 0, -1, 0);  //文本区域
                int alignment = Qt::TextShowMnemonic;
                //快捷键的下划线
                if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &opt, this)) {
                    alignment |= Qt::TextHideMnemonic;
                }

                if (opt.toolButtonStyle == Qt::ToolButtonTextUnderIcon) {
                    // ribbonbutton在小图标下，不支持ToolButtonTextUnderIcon
                } else {
                    style()->drawItemPixmap(&p, QStyle::visualRect(opt.direction, opt.rect, pr), Qt::AlignCenter, pm);
                    alignment |= Qt::AlignLeft | Qt::AlignVCenter;
                }
                style()->drawItemText(&p,
                                      QStyle::visualRect(opt.direction, opt.rect, tr),
                                      alignment,
                                      opt.palette,
                                      opt.state & QStyle::State_Enabled,
                                      opt.text,
                                      QPalette::ButtonText);
                p.restore();
            } else {
                style()->drawItemPixmap(&p, d_ptr->mDrawIconRect, Qt::AlignCenter, pm);
            }
        } else {  // 只有文字
            int alignment = Qt::TextShowMnemonic;
            //快捷键的下划线
            if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &opt, this)) {
                alignment |= Qt::TextHideMnemonic;
            }
            style()->drawItemText(&p,
                                  QStyle::visualRect(opt.direction, opt.rect, opt.rect.adjusted(2, 1, -2, -1)),
                                  alignment,
                                  opt.palette,
                                  opt.state & QStyle::State_Enabled,
                                  opt.text,
                                  QPalette::ButtonText);
        }

        //绘制sub control 的下拉箭头
        if (opt.features & QStyleOptionToolButton::MenuButtonPopup || opt.features & QStyleOptionToolButton::HasMenu) {
            QStyleOptionToolButton tool = opt;
            tool.rect                   = calcIndicatorArrowDownRect(tool);

            SARIBBONTOOLBUTTON_DEBUG_DRAW_RECT(p, tool.rect);
            style()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &tool, &p, this);
        }
    }
}

/**
 * @brief 绘制MenuButtonPopup模式下的菜单箭头
 * @param p
 * @param opt
 */
void SARibbonToolButton::drawMenuButtonPopupIndicatorArrow(QPainter& p, const QStyleOptionToolButton& opt)
{
    if (LargeButton == d_ptr->mButtonType) {

    } else {
        if (opt.features & QStyleOptionToolButton::MenuButtonPopup) {
            QStyleOption tool = opt;
            tool.rect         = style()->subControlRect(QStyle::CC_ToolButton, &opt, QStyle::SC_ToolButtonMenu, this);
            style()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &tool, &p, this);
        }
    }
}

/**
 * @brief 获取icon绘制区域
 * @param opt
 * @return
 */
QRect SARibbonToolButton::getIconDrawRect(const QStyleOptionToolButton& opt)
{
    return d_ptr->mDrawIconRect;
}

/**
 * @brief 获取Indicator区域，Indicator区域包含了text
 * @param opt
 * @return
 */
QRect SARibbonToolButton::getIndicatorDrawRect(const QStyleOptionToolButton& opt)
{
    return d_ptr->mDrawTextRect;
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
            QStyleOptionToolButton opt;
            initStyleOption(&opt);
            d_ptr->updateSizeHint(opt);
            d_ptr->updateDrawRect(opt);
            //            calcIconAndTextRect(opt);
        }
    }
    QToolButton::changeEvent(e);
}

void SARibbonToolButton::actionEvent(QActionEvent* e)
{
    QToolButton::actionEvent(e);
    // action的文字发生变更时要重新计算尺寸--sizehint实时计算情况下，此处不需要这样设置
    //    if (e) {
    //        // action发生变化
    //        if (e->action()) {
    //            if (!e->action()->text().isEmpty()) {
    //                qDebug() << text() << ":actionEvent,sizehint=" << m_sizeHint;
    //                reCalcSizeHint();
    //            }
    //        }
    //    }
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
        setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    } else {
        setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    }
    setMouseTracking(true);
}

QSize SARibbonToolButton::minimumSizeHint() const
{
    return (sizeHint());
}

void SARibbonToolButton::setLargeButtonType(SARibbonToolButton::LargeButtonType type)
{
    d_ptr->mLargeButtonType = type;
}

SARibbonToolButton::LargeButtonType SARibbonToolButton::largeButtonType() const
{
    return (d_ptr->mLargeButtonType);
}

void SARibbonToolButton::reCalcSizeHint()
{
    QSize s = QToolButton::sizeHint();
    reCalcSizeHint(s);
}

/**
 * @brief 设置按钮点击时文字会闪烁一下
 * @param on
 */
void SARibbonToolButton::setToolButtonTextShift(bool on)
{
    SARibbonToolButton::PrivateData::s_isToolButtonTextShift = on;
}

/**
 * @brief 判断按钮点击时文字是否会闪烁一下
 * @return
 */
bool SARibbonToolButton::isToolButtonTextShift()
{
    return SARibbonToolButton::PrivateData::s_isToolButtonTextShift;
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

/**
 * @brief 根据设定计算图标的绘制区域
 * @param opt
 * @return
 */
void SARibbonToolButton::calcIconAndTextRect(const QStyleOptionToolButton& opt)
{
    d_ptr->mIndicatorLen = style()->pixelMetric(QStyle::PM_MenuButtonIndicator, &opt, this);
    if (LargeButton == d_ptr->mButtonType) {
        d_ptr->mDrawIconRect = opt.rect.adjusted(d_ptr->mSpacing, d_ptr->mSpacing, -d_ptr->mSpacing, -d_ptr->mSpacing);
        if (opt.toolButtonStyle != Qt::ToolButtonIconOnly) {
            // sizehint已经保证宽度能显示两行
            // icon 的区域就是文字以外的区域，如果m_isEnableWordWrap=true，就保证有两行文本的空间
            int maxheight = 30;
            if (Normal == d_ptr->mLargeButtonType
                || (Lite == d_ptr->mLargeButtonType && SARibbonToolButton::PrivateData::s_liteStyleEnableWordWrap)) {
                maxheight = opt.rect.height() - (opt.fontMetrics.lineSpacing() * 2)  //减去2行文本
                            - 2 * d_ptr->mSpacing                                    //减去赏析边距
                            - d_ptr->mSpacing  //这里减去m_iconAndTextSpace，m_iconAndTextSpace是icon和text的分隔距离
                        ;
            } else {
                maxheight = opt.rect.height() - (opt.fontMetrics.lineSpacing() * 1.2)  //减去1行文本
                            - 2 * d_ptr->mSpacing                                      //减去赏析边距
                            - d_ptr->mSpacing  //这里减去m_iconAndTextSpace，m_iconAndTextSpace是icon和text的分隔距离
                        ;
            }
            d_ptr->mDrawIconRect.setHeight(maxheight);

            //没有菜单
            d_ptr->mDrawTextRect.setRect(d_ptr->mSpacing,
                                         d_ptr->mDrawIconRect.bottom() + d_ptr->mSpacing,
                                         d_ptr->mDrawIconRect.width(),
                                         opt.rect.bottom() - d_ptr->mDrawIconRect.bottom() - d_ptr->mSpacing  // 高度是矩形底部减去icon底部再减去2*Space
            );
            //保证能显示两行文本
            //下面确定文字区域
            if ((opt.features & QStyleOptionToolButton::MenuButtonPopup) || (opt.features & QStyleOptionToolButton::HasMenu)) {
                //有菜单且换行,宽度偏移ARROW_WIDTH
                if (d_ptr->mWordWrap) {
                    d_ptr->mDrawTextRect.adjust(0, 0, -d_ptr->mIndicatorLen, 0);
                } else if (Lite == d_ptr->mLargeButtonType && !SARibbonToolButton::PrivateData::s_liteStyleEnableWordWrap) {
                    //在lite模式下，不允许换行的时候，也需要偏移下三角
                    d_ptr->mDrawTextRect.adjust(0, 0, -d_ptr->mIndicatorLen, 0);
                }
            }

        } else {
            d_ptr->mDrawTextRect = QRect();
        }
    } else {
        //小按钮
        if (opt.toolButtonStyle == Qt::ToolButtonIconOnly) {
            // InstantPopup在qtoolbutton不会添加控件来放下箭头，这里处理的和MenuButtonPopup一致
            // 在仅有图标的小模式显示时，预留一个下拉箭头位置
            d_ptr->mDrawIconRect = opt.rect.adjusted(d_ptr->mSpacing, d_ptr->mSpacing, -d_ptr->mSpacing, -d_ptr->mSpacing);
            if (opt.features & QStyleOptionToolButton::MenuButtonPopup || opt.features & QStyleOptionToolButton::HasMenu) {
                d_ptr->mDrawIconRect.adjust(0, 0, -d_ptr->mIndicatorLen, 0);
            }
            //确定文本区域
            d_ptr->mDrawTextRect = QRect();
        } else {
            d_ptr->mDrawIconRect = QRect(d_ptr->mSpacing,
                                         d_ptr->mSpacing,
                                         qMax(opt.rect.height(), opt.iconSize.width()) - 2 * d_ptr->mSpacing,
                                         opt.rect.height() - 2 * d_ptr->mSpacing);
            d_ptr->mDrawTextRect.setRect(d_ptr->mDrawIconRect.right() + d_ptr->mSpacing,
                                         d_ptr->mSpacing,
                                         opt.rect.right() - d_ptr->mDrawIconRect.right() - 2 * d_ptr->mSpacing,
                                         opt.rect.height() - 2 * d_ptr->mSpacing);
            //确定文本区域
            if ((opt.features & QStyleOptionToolButton::MenuButtonPopup) || (opt.features & QStyleOptionToolButton::HasMenu)) {
                //有菜单,宽度偏移ARROW_WIDTH
                d_ptr->mDrawTextRect.adjust(0, 0, -d_ptr->mIndicatorLen, 0);
            }
        }
    }

    //纯文本的文字位置
    if ((Qt::ToolButtonTextOnly == toolButtonStyle()) || icon().isNull()) {
        d_ptr->mDrawTextRect = opt.rect.adjusted(d_ptr->mSpacing, d_ptr->mSpacing, -d_ptr->mSpacing, -d_ptr->mSpacing);
    }
}

/**
 * @brief 根据设定计算文本显示区域
 * @param opt
 * @return
 */
QRect SARibbonToolButton::adjustedTextRect(const QStyleOptionToolButton& opt) const
{

    QRect rect = d_ptr->mDrawTextRect;

    //这两个偏移量是有些按钮在点击时文字会出现一个类似抖动的效果，但据观察office2013的ribbon是不会动的，因此把这个效果取消
    if (SARibbonToolButton::PrivateData::s_isToolButtonTextShift) {
        int shiftX = 0;
        int shiftY = 0;

        if (opt.state & (QStyle::State_Sunken | QStyle::State_On)) {
            shiftX = style()->pixelMetric(QStyle::PM_ButtonShiftHorizontal, &opt, this);
            shiftY = style()->pixelMetric(QStyle::PM_ButtonShiftVertical, &opt, this);
        }

        rect.translate(shiftX, shiftY);
    }
    return (rect);
}

/**
 * @brief sub control 的下拉箭头的位置
 *
 * 对于大模式，如果文字换行，箭头会在最右侧，如果文字不换行，箭头就在原来文字第二行的位置
 * @param opt
 * @return
 * @note IndicatorArrow尺寸的计算依赖text，前置需要先调用calcIconAndTextRect
 */
QRect SARibbonToolButton::calcIndicatorArrowDownRect(const QStyleOptionToolButton& opt)
{
    //预留ARROW_WIDTHpx绘制箭头，1px的上下边界
    QRect rect = opt.rect;
    if (LargeButton == d_ptr->mButtonType) {
        if ((Lite == d_ptr->mLargeButtonType && !SARibbonToolButton::PrivateData::s_liteStyleEnableWordWrap)) {
            //首先判断是否为lite且不允许换行
            rect.setRect(d_ptr->mDrawTextRect.right(),
                         d_ptr->mDrawTextRect.top(),
                         opt.rect.right() - d_ptr->mSpacing - d_ptr->mDrawTextRect.right(),
                         d_ptr->mDrawTextRect.height());
        } else if (d_ptr->mWordWrap) {
            //如果不是lite，且允许换行，那么就在文本第二行最后端
            //菜单的下拉箭头位于第二行文本的矩形区域
            rect.setRect(d_ptr->mDrawTextRect.right(),
                         d_ptr->mDrawTextRect.top() + d_ptr->mDrawTextRect.height() / 2,
                         opt.rect.right() - d_ptr->mSpacing - d_ptr->mDrawTextRect.right(),
                         d_ptr->mDrawTextRect.height() / 2);

        } else {
            //都不是的情况下就第二行文本位置
            rect.setRect(d_ptr->mDrawTextRect.left(),
                         d_ptr->mDrawTextRect.bottom() - d_ptr->mIndicatorLen,  // 这里ARROW_WIDTH也作为高度
                         d_ptr->mDrawTextRect.width(),
                         d_ptr->mIndicatorLen);
        }
    } else {
        rect.setRect(rect.width() - d_ptr->mIndicatorLen - d_ptr->mSpacing,
                     d_ptr->mSpacing,
                     d_ptr->mIndicatorLen,  // 箭头宽度
                     rect.height() - 2 * d_ptr->mSpacing);
    }
    return (rect);
}

bool SARibbonToolButton::event(QEvent* e)
{
    switch (e->type()) {
    case QEvent::WindowDeactivate:
    case QEvent::ActionChanged:
    case QEvent::ActionRemoved:
    case QEvent::ActionAdded:
        d_ptr->mMouseOnSubControl = false;
        break;

    default:
        break;
    }

    return (QToolButton::event(e));
}
