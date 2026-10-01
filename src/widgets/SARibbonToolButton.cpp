#include "SARibbonToolButton.h"
#include "SARibbonPanel.h"
#include "SARibbonPanelLayout.h"

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
#include "SARibbonQt5Compat.hpp"
#include "SARibbonUtil.h"
#include <SARibbonCore/SARibbonToolButtonLayout.h>

/**
 * @def 开启此宏会打印一些常见信息
 */
#ifndef SA_RIBBON_TOOLBUTTON_DEBUG_PRINT
#define SA_RIBBON_TOOLBUTTON_DEBUG_PRINT 0
#endif

#ifndef SARIBBONTOOLBUTTON_DEBUG_DRAW
#define SARIBBONTOOLBUTTON_DEBUG_DRAW 0
#endif

// 布局常量与布局算法已下沉 core（计划 04 / NOTES B49：QML 前端需要完全一致的
// 按钮文字布局，禁止在 QML 侧重写），此处保留 2.x 拼写别名以维持既有引用
namespace SARibbonToolButtonConstants = SARibbon::Core::ToolButtonLayoutConstants;

using SARibbonToolButtonLayout = SARibbon::Core::SARibbonToolButtonLayout;

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

/**
 * \if ENGLISH
 * @brief Custom proxy style that draws arrow indicators directly on the target painter
 * @details This class overrides drawPrimitive to draw arrow indicators (up/down/left/right)
 *          directly on the passed QPainter, eliminating the intermediate QImage allocation,
 *          QPainter creation, and QPixmap::fromImage conversion that the original implementation
 *          performed on every paint call. Qt's painter automatically handles device pixel ratio
 *          scaling, so no manual DPR handling is needed.
 * \endif
 *
 * \if CHINESE
 * @brief 自定义代理样式，直接在目标 QPainter 上绘制箭头指示器
 * @details 此类重写 drawPrimitive，将箭头指示器（上/下/左/右）直接绘制在传入的 QPainter 上，
 *          省去了原实现中每次绘制都进行的 QImage 分配、QPainter 创建和 QPixmap::fromImage 转换。
 *          Qt 的 painter 会自动处理设备像素比缩放，无需手动处理 DPR。
 * \endif
 */
class SARibbonToolButtonProxyStyle : public QProxyStyle
{
public:
    /**
     * \if ENGLISH
     * @brief Draw primitive elements with optimized arrow indicator rendering
     * @details For arrow indicators (PE_IndicatorArrowUp/Down/Left/Right), the polygon is drawn
     *          directly on the target painter \a p using logical coordinates. Pen width (1.4) is
     *          in logical pixels, producing slightly thicker lines on high-DPI displays for better
     *          visibility. For all other primitive elements, the base QProxyStyle is used.
     * @param pe The primitive element to draw
     * @param opt Style option containing rect, state, and palette
     * @param p Target painter (operates in logical coordinates)
     * @param widget The widget being painted
     * \endif
     *
     * \if CHINESE
     * @brief 绘制基本元素，优化了箭头指示器的渲染
     * @details 对于箭头指示器（PE_IndicatorArrowUp/Down/Left/Right），直接在目标 painter \a p 上
     *          使用逻辑坐标绘制多边形。画笔宽度（1.4）以逻辑像素为单位，在高 DPI 屏幕上线条
     *          略粗以提升可见性。对于其他基本元素，使用基类 QProxyStyle 的实现。
     * @param pe 要绘制的基本元素
     * @param opt 包含矩形、状态和调色板的样式选项
     * @param p 目标 painter（以逻辑坐标工作）
     * @param widget 正在绘制的控件
     * \endif
     */
    void drawPrimitive(PrimitiveElement pe, const QStyleOption* opt, QPainter* p, const QWidget* widget = nullptr) const override
    {
        if (pe == PE_IndicatorArrowUp || pe == PE_IndicatorArrowDown || pe == PE_IndicatorArrowRight
            || pe == PE_IndicatorArrowLeft) {
            if (opt->rect.width() <= 1 || opt->rect.height() <= 1) {
                return;
            }

            QRect r    = opt->rect;
            int size   = qMin(r.height(), r.width());
            int border = size / 4;
            int sqsize = 2 * (size / 2);

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

            int xOffset = r.x() + (r.width() - size) / 2;
            int yOffset = r.y() + (r.height() - size) / 2;

            p->save();
            p->translate(xOffset + sx + bsx, yOffset + sy + bsy);
            p->setPen(QPen(opt->palette.buttonText().color(), 1.4));
            p->setBrush(Qt::NoBrush);

            if (!(opt->state & State_Enabled)) {
                p->translate(1, 1);
                p->setPen(QPen(opt->palette.light().color(), 1.4));
                p->drawPolyline(a);
                p->translate(-1, -1);
                p->setPen(QPen(opt->palette.mid().color(), 1.4));
            }

            p->drawPolyline(a);
            p->restore();
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
    // 把widget侧状态收敛为core布局算法的纯值输入
    SARibbonToolButtonLayout::Input layoutInput(const QStyleOptionToolButton& opt) const;
    // 判断是否有Indicator
    bool hasIndicator(const QStyleOptionToolButton& opt) const;
    // 计算sizehint
    QSize calcSizeHint(const QStyleOptionToolButton& opt);
    QSize calcSmallButtonSizeHint(const QStyleOptionToolButton& opt);
    QSize calcLargeButtonSizeHint(const QStyleOptionToolButton& opt);
    // 获取当前panel计算出来的大按钮高度，-1表示按钮不在SARibbonPanel中
    int panelLargeButtonHeight() const;
    // 判断缓存的sizeHint是否依然有效（其所依赖的大按钮高度没有变化）
    bool isSizeHintUpToDate() const;

    QPixmap createIconPixmap(const QStyleOptionToolButton& opt, const QSize& iconsize) const;
    // 获取文字的对其方式
    int getTextAlignment() const;
    // 确认文字是否确切要换行显示
    bool isTextNeedWrap() const;
    // 获取真实的icon尺寸
    QSize realIconSize() const;
    // 仅仅对\n进行剔除，和QString::simplified不一样
    static QString simplifiedForRibbonButton(const QString& str);
    // 获取有效的按钮类型（当enableIconRightText为true时，强制返回SmallButton）
    SARibbonToolButton::RibbonButtonType effectiveButtonType() const;

public:
    bool mMouseOnSubControl { false };  ///< 这个用于标记MenuButtonPopup模式下，鼠标在文本区域
    bool mMenuButtonPressed { false };  ///< 由于Indicator改变，因此hitButton不能用QToolButton的hitButton
    bool mWordWrap { true };            ///< 标记是否文字换行 @default false
    bool enableIconRightText { false }; ///< 是否启用图标右侧文字模式
    SARibbonToolButton::RibbonButtonType mButtonType { SARibbonToolButton::LargeButton };
    int mSpacing { SARibbonToolButtonConstants::DEFAULT_SPACING };                   ///< 按钮和边框的距离
    int mIndicatorLen { SARibbonToolButtonConstants::DEFAULT_INDICATOR_LEN_LARGE };  ///< Indicator的长度
    QRect mDrawIconRect;                                                             ///< 记录icon的绘制位置
    QRect mDrawTextRect;                                                             ///< 记录text的绘制位置
    QRect mDrawIndicatorArrowRect;                                                   ///< 记录IndicatorArrow的绘制位置
    QSize mSizeHint;                                                                 ///< 保存计算好的sizehint
    int mSizeHintBaseHeight { -1 };  ///< 计算mSizeHint时依据的大按钮高度，-1表示不依赖panel几何
    QSize mLargeButtonSizeHint { 32, 32 };                                           ///< 大按钮的尺寸
    bool mIsTextNeedWrap { false };                                                  ///< 标记文字是否需要换行显示
    SARibbonToolButton::LayoutFactor layoutFactor;                                   ///< 布局系数
    std::unique_ptr< SARibbonToolButtonProxyStyle > mStyle;                          ///< 按钮样式，主要为了绘制箭头

    // 图标缓存相关
    mutable QPixmap mCachedIconPixmap;                      ///< 缓存的图标pixmap
    mutable QSize mCachedIconSize;                          ///< 缓存的图标尺寸
    mutable QIcon::Mode mCachedIconMode { QIcon::Normal };  ///< 缓存的图标模式
    mutable QIcon::State mCachedIconState { QIcon::Off };   ///< 缓存的图标状态
    mutable bool mIconCacheValid { false };                 ///< 图标缓存是否有效

    void invalidateIconCache()
    {
        mIconCacheValid = false;
    }
};

SARibbonToolButton::PrivateData::PrivateData(SARibbonToolButton* p) : q_ptr(p)
{
    mStyle = std::make_unique< SARibbonToolButtonProxyStyle >();
}

SARibbonToolButton::RibbonButtonType SARibbonToolButton::PrivateData::effectiveButtonType() const
{
    if (enableIconRightText) {
        return SARibbonToolButton::SmallButton;
    }
    return mButtonType;
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
    if (SARibbonToolButton::LargeButton == effectiveButtonType()) {
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
    if (!mSizeHint.isValid() || !isSizeHintUpToDate()) {
        updateSizeHint(opt);
    }
    // 先更新IndicatorLen
    mIndicatorLen = q_ptr->style()->pixelMetric(QStyle::PM_MenuButtonIndicator, &opt, q_ptr);
    if (mIndicatorLen < 3) {
        if (SARibbonToolButton::LargeButton == effectiveButtonType()) {
            mIndicatorLen = SARibbonToolButtonConstants::DEFAULT_INDICATOR_LEN_LARGE;
        } else {
            mIndicatorLen = SARibbonToolButtonConstants::DEFAULT_INDICATOR_LEN_SMALL;
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
    Q_UNUSED(spacing)        // 间距与指示器长度经 layoutInput 传入 core
    Q_UNUSED(indicatorLen)
    // 布局算法在 core（SARibbonToolButtonLayout），widgets 与 QML 共用同一份实现
    const SARibbonToolButtonLayout::DrawRectResult r =
        SARibbonToolButtonLayout::calcDrawRects(layoutInput(opt), mIsTextNeedWrap);
    iconRect           = r.iconRect;
    textRect           = r.textRect;
    indicatorArrowRect = r.indicatorArrowRect;
}

/**
 * \if ENGLISH
 * @brief Collect every widget-side state the core layout algorithm needs
 * @details The core unit takes plain values only (no QStyleOptionToolButton, no
 *          widget pointer), so the widget touchpoints are resolved here: the
 *          effective button type (iconRightText forces small), the indicator
 *          presence (MenuButtonPopup/HasMenu features), the panel large button
 *          height, the layout factors and the RTL flag.
 * \endif
 *
 * \if CHINESE
 * @brief 收集 core 布局算法需要的全部 widget 侧状态
 * @details core 单元只接受纯值（不含 QStyleOptionToolButton 与 widget 指针），
 *          因此 widget 触点在此解析：有效按钮类型（iconRightText 强制小按钮）、
 *          指示器存在性（MenuButtonPopup/HasMenu 特征）、panel 大按钮高度、
 *          布局系数与 RTL 标志。
 * \endif
 */
SARibbonToolButtonLayout::Input SARibbonToolButton::PrivateData::layoutInput(const QStyleOptionToolButton& opt) const
{
    SARibbonToolButtonLayout::Input in;
    in.rect                   = opt.rect;
    in.toolButtonStyle        = opt.toolButtonStyle;
    in.isLargeButton          = (SARibbonToolButton::LargeButton == effectiveButtonType());
    in.enableWordWrap         = q_ptr->isEnableWordWrap();
    in.hasIcon                = !opt.icon.isNull();
    in.hasIndicator           = hasIndicator(opt);
    in.isRTL                  = SA::saIsRTL();
    in.iconSize               = opt.iconSize;
    in.largeIconSize          = mLargeButtonSizeHint;
    in.text                   = opt.text;
    in.fontMetrics            = opt.fontMetrics;
    in.spacing                = mSpacing;
    in.indicatorLen           = mIndicatorLen;
    in.panelLargeButtonHeight = panelLargeButtonHeight();
    in.maximumWidth           = q_ptr->maximumWidth();
    in.factors.twoLineHeightFactor         = layoutFactor.twoLineHeightFactor;
    in.factors.oneLineHeightFactor         = layoutFactor.oneLineHeightFactor;
    in.factors.buttonMaximumAspectRatio    = layoutFactor.buttonMaximumAspectRatio;
    in.factors.largeButtonMinimumWidthRatio = layoutFactor.largeButtonMinimumWidthRatio;
    return in;
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
 * @brief 获取当前panel计算出来的大按钮高度
 * @return 按钮所在panel的大按钮高度，不在panel中时返回-1
 */
int SARibbonToolButton::PrivateData::panelLargeButtonHeight() const
{
    if (SARibbonPanel* panel = qobject_cast< SARibbonPanel* >(q_ptr->parent())) {
        return panel->largeButtonHeight();
    }
    return (-1);
}

/**
 * @brief 判断缓存的sizeHint是否依然有效
 *
 * 大按钮的宽高比（buttonMaximumAspectRatio）和换行判断都基于panel给出的大按钮高度，
 * 因此panel高度变化后缓存必须失效，否则会一直沿用旧高度算出来的宽度。
 * 典型场景：category在加入SARibbonBar之前就被填充完毕（此时panel还是顶层窗口的默认尺寸），
 * 或者用户调整了category/panel标题的高度。
 *
 * @return 缓存仍然有效返回true
 */
bool SARibbonToolButton::PrivateData::isSizeHintUpToDate() const
{
    if (mSizeHintBaseHeight < 0) {
        // 不依赖panel几何（小按钮，或不在panel中），无需校验
        return true;
    }
    return (mSizeHintBaseHeight == panelLargeButtonHeight());
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
    if (SARibbonToolButton::LargeButton == effectiveButtonType()) {
        return calcLargeButtonSizeHint(opt);
    }
    return calcSmallButtonSizeHint(opt);
}

QSize SARibbonToolButton::PrivateData::calcSmallButtonSizeHint(const QStyleOptionToolButton& opt)
{
    const SARibbonToolButtonLayout::SizeHintResult r = SARibbonToolButtonLayout::calcSizeHint(layoutInput(opt));
    mSizeHintBaseHeight = r.sizeHintBaseHeight;
    mIsTextNeedWrap     = r.isTextNeedWrap;
    return r.sizeHint;
}

QSize SARibbonToolButton::PrivateData::calcLargeButtonSizeHint(const QStyleOptionToolButton& opt)
{
    // 宽高比上限、两行文字预算、二分换行宽度估算全部在 core，QML 前端复用同一实现
    const SARibbonToolButtonLayout::SizeHintResult r = SARibbonToolButtonLayout::calcSizeHint(layoutInput(opt));
    mSizeHintBaseHeight = r.sizeHintBaseHeight;
    mIsTextNeedWrap     = r.isTextNeedWrap;
    return r.sizeHint;
}

QPixmap SARibbonToolButton::PrivateData::createIconPixmap(const QStyleOptionToolButton& opt, const QSize& iconsize) const
{
    if (opt.icon.isNull()) {  // 没有图标
        return QPixmap();
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

    // 检查缓存是否有效
    if (mIconCacheValid && mCachedIconSize == iconsize && mCachedIconMode == mode && mCachedIconState == state) {
        return mCachedIconPixmap;
    }

    // 生成新的pixmap并缓存
    mCachedIconPixmap = SA::iconToPixmap(opt.icon, iconsize, SA::widgetDevicePixelRatio(q_ptr), mode, state);
    mCachedIconSize   = iconsize;
    mCachedIconMode   = mode;
    mCachedIconState  = state;
    mIconCacheValid   = true;

    return mCachedIconPixmap;
}

int SARibbonToolButton::PrivateData::getTextAlignment() const
{
    // 对齐标志由 core 统一给出（大按钮换行时 AlignTop|AlignHCenter + TextWordWrap）
    SARibbonToolButtonLayout::Input in;
    in.toolButtonStyle = q_ptr->toolButtonStyle();
    in.isLargeButton   = (SARibbonToolButton::LargeButton == effectiveButtonType());
    in.enableWordWrap  = q_ptr->isEnableWordWrap();
    return SARibbonToolButtonLayout::textAlignment(in);
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
    if (effectiveButtonType() == SARibbonToolButton::LargeButton) {
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
    return SARibbonToolButtonLayout::simplifiedText(str);
}

//===================================================
// SARibbonToolButton
//===================================================

/**
 * \if ENGLISH
 * @brief Constructor for SARibbonToolButton
 * @param parent Parent widget
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonToolButton构造函数
 * @param parent 父窗口部件
 * \endif
 */
SARibbonToolButton::SARibbonToolButton(QWidget* parent)
    : QToolButton(parent), d_ptr(new SARibbonToolButton::PrivateData(this))
{
    // 静态设置也是可以，虽然节省内存，但不清楚未来qt是否会针对绘制有潜在的多线程处理的可能性，因此这里还是使用成员变量
    // static SARibbonToolButtonProxyStyle* ss_style = new SARibbonToolButtonProxyStyle();
    // setStyle(ss_style);

    // setStyle方法不会接管样式的所有权，因此要手动删除，这里使用智能指针
    // 注意：setStyle不会获取所有权，所以使用get()是安全的，样式对象生命周期由unique_ptr管理
    setStyle(d_ptr->mStyle.get());
    setAutoRaise(true);
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    setButtonType(SmallButton);
    setMouseTracking(true);
}

/**
 * \if ENGLISH
 * @brief Constructor for SARibbonToolButton with default action
 * @param defaultAction Default action for the button
 * @param parent Parent widget
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonToolButton构造函数（带默认动作）
 * @param defaultAction 按钮的默认动作
 * @param parent 父窗口部件
 * \endif
 */
SARibbonToolButton::SARibbonToolButton(QAction* defaultAction, QWidget* parent)
    : QToolButton(parent), d_ptr(new SARibbonToolButton::PrivateData(this))
{
    setAutoRaise(true);
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    setDefaultAction(defaultAction);
    setButtonType(SmallButton);
    setMouseTracking(true);
}

/**
 * \if ENGLISH
 * @brief Destructor for SARibbonToolButton
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonToolButton析构函数
 * \endif
 */
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
    // 触发重绘以应用新的布局因子
    update();
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
/**
 * \if ENGLISH
 * @brief Get the current button type
 * @return Current button type (LargeButton or SmallButton)
 * \endif
 *
 * \if CHINESE
 * @brief 获取当前按钮的类型
 * @return 当前按钮类型（大按钮或小按钮）
 * \endif
 */
SARibbonToolButton::RibbonButtonType SARibbonToolButton::buttonType() const
{
    return (d_ptr->mButtonType);
}

/**
 * \if ENGLISH
 * @brief Sets the button type to LargeButton or SmallButton
 *
 * Changing the button type will invalidate the size hint and trigger a relayout.
 * Note: This function may override the tool button style. If you need to set a specific style (e.g.,
 * Qt::ToolButtonIconOnly), do so after calling this function.
 * @param buttonType The new button type
 * @sa isLargeRibbonButton, isSmallRibbonButton
 * \endif
 *
 * \if CHINESE
 * @brief 设置按钮类型为大按钮或小按钮
 *
 * 设置按钮类型会令尺寸提示失效并触发重新布局。
 * 注意：此函数可能会覆盖工具按钮样式。如需设置特定样式（例如 Qt::ToolButtonIconOnly），请在此函数调用之后设置。
 * @param buttonType 新的按钮类型
 * @sa isLargeRibbonButton, isSmallRibbonButton
 * \endif
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
bool SARibbonToolButton::isEnableWordWrap() const
{
    return d_ptr->mWordWrap;
}

/**
 * \if ENGLISH
 * @brief Set whether text is displayed to the right of the icon
 * @param on If true, the button uses horizontal layout (icon-left, text-right),
 *           regardless of the current RibbonButtonType.
 *           When false, the button uses the default layout based on RibbonButtonType.
 * @details When this mode is enabled, LargeButton type buttons are rendered with
 *           the SmallButton horizontal layout strategy. The button's mButtonType
 *           remains unchanged internally.
 * \endif
 *
 * \if CHINESE
 * @brief 设置文字是否显示在图标右侧
 * @param on 如果为true，按钮使用水平布局（图标在左，文字在右），
 *           不受当前RibbonButtonType的影响。
 *           当为false时，按钮根据RibbonButtonType使用默认布局。
 * @details 启用此模式时，LargeButton类型的按钮使用SmallButton的水平布局策略渲染。
 *           按钮的mButtonType在内部保持不变。
 * \endif
 */
void SARibbonToolButton::setEnableIconRightText(bool on)
{
    SA_D(d);
    if (d->enableIconRightText == on) {
        return;
    }
    d->enableIconRightText = on;
    // When enableIconRightText changes, update size policy to match effective type
    if (on) {
        // Force horizontal layout - use SmallButton-style size policy
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    } else {
        // Restore size policy based on actual button type
        if (LargeButton == d->mButtonType) {
            setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
        } else {
            setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
        }
    }
    invalidateSizeHint();
}

bool SARibbonToolButton::isEnableIconRightText() const
{
    SA_DC(d);
    return d->enableIconRightText;
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

/**
 * @brief Sets the minimum width ratio (relative to height) for large buttons / 设置大按钮的最小宽度比例（相对于高度）
 *
 * This is a convenience function that directly sets the `largeButtonMinimumWidthRatio` member of the
 * `LayoutFactor` structure. It has the same effect as modifying the structure and calling `setLayoutFactor`.
 *
 * 此函数是直接设置 `LayoutFactor` 结构体中 `largeButtonMinimumWidthRatio` 成员的便捷方法。
 * 其效果等同于修改结构体后调用 `setLayoutFactor`。
 *
 * @param v The new minimum width ratio value / 新的最小宽度比例值，小于等于0时仅以icon宽度作为下限
 * @sa largeButtonMinimumWidthRatio, setLayoutFactor
 */
void SARibbonToolButton::setLargeButtonMinimumWidthRatio(qreal v)
{
    d_ptr->layoutFactor.largeButtonMinimumWidthRatio = v;
    // 重新布局
    invalidateSizeHint();
}

/**
 * @brief Gets the minimum width ratio (relative to height) for large buttons / 获取大按钮的最小宽度比例
 * @return The current minimum width ratio / 当前的最小宽度比例
 * @sa setLargeButtonMinimumWidthRatio, layoutFactor
 */
qreal SARibbonToolButton::largeButtonMinimumWidthRatio() const
{
    return layoutFactor().largeButtonMinimumWidthRatio;
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
        // invalidateSizeHint is handled in actionEvent(); keep only sub-control reset here
        d_ptr->mMouseOnSubControl = false;
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
        case QEvent::LayoutDirectionChange: {
            // 布局方向改变（如 LTR→RTL），重新计算绘制矩形和尺寸提示
            invalidateSizeHint();
            update();
        } break;
        case QEvent::ScreenChangeInternal:
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        case QEvent::DevicePixelRatioChange:
#endif
        {
            invalidateSizeHint();
            break;
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
    bool oldMouseOnSubControl = d_ptr->mMouseOnSubControl;
    d_ptr->updateStatusByMousePosition(SA::compat::eventPos(e));
    // 如果鼠标进入/离开子控件，图标状态可能改变，使缓存失效
    if (oldMouseOnSubControl != d_ptr->mMouseOnSubControl) {
        d_ptr->invalidateIconCache();
    }
    QToolButton::mouseMoveEvent(e);
}

/**
 * @brief SARibbonToolButton::mousePressEvent
 * @param e
 */
void SARibbonToolButton::mousePressEvent(QMouseEvent* e)
{
    if ((e->button() == Qt::LeftButton) && (popupMode() == MenuButtonPopup)) {
        d_ptr->updateStatusByMousePosition(SA::compat::eventPos(e));
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
    d_ptr->invalidateIconCache();  // 鼠标离开，图标状态改变
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
    if (d_ptr->mSizeHint.isValid() && d_ptr->isSizeHintUpToDate()) {
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
    d_ptr->invalidateIconCache();  // action改变，图标可能改变
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
                if (LargeButton == d_ptr->effectiveButtonType()) {                 // 大按钮模式
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
                        tool.state |= (QStyle::State_Raised);  // 把图标区域显示为正常
                        tool.state &= ~QStyle::State_MouseOver;
                        if (autoRaise) {
                            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
                        } else {
                            style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
                        }
                    } else {
                        // 鼠标在图标区，把文字显示为正常
                        tool.state |= (QStyle::State_Raised);  // 把图标区域显示为正常
                        tool.state &= ~QStyle::State_MouseOver;
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

    QPixmap pm = createIconPixmap(opt, d_ptr->realIconSize());
    style()->drawItemPixmap(&p, iconDrawRect, Qt::AlignCenter, pm);
    SARIBBONTOOLBUTTON_DEBUG_DRAW_RECT(p, iconDrawRect);
}

/**
 * @brief 创建图标pixmap，子类可以重写此函数以自定义图标绘制
 */
QPixmap SARibbonToolButton::createIconPixmap(const QStyleOptionToolButton& opt, const QSize& iconSize) const
{
    return d_ptr->createIconPixmap(opt, iconSize);
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
    if (d_ptr->effectiveButtonType() == SARibbonToolButton::SmallButton) {
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
        &p, textDrawRect, alignment, label.palette, label.state & QStyle::State_Enabled, text, foregroundRole());
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
    // Notify parent panel layout to invalidate this button's cache entry
    // This avoids stale cached sizeHints when button properties change
    if (SARibbonPanel* panel = qobject_cast< SARibbonPanel* >(parentWidget())) {
        if (SARibbonPanelLayout* lay = panel->panelLayout()) {
            lay->invalidateButtonSizeHintCache(this);
        }
    }
    updateGeometry();
}

void SARibbonToolButton::invalidateIconCache()
{
    d_ptr->invalidateIconCache();
}

void SARibbonToolButton::setIcon(const QIcon& icon)
{
    invalidateIconCache();
    QToolButton::setIcon(icon);
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
