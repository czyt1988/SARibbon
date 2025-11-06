#ifndef SARIBBONTOOLBUTTON_H
#define SARIBBONTOOLBUTTON_H
#include "SARibbonGlobal.h"
#include <QToolButton>
#include <QDebug>
/**
 * @brief Ribbon interface adapted tool button / Ribbon界面适用的toolButton
 *
 * This button is designed specifically for the Ribbon interface, supporting two display modes: large buttons and small
 * buttons. It also supports automatic text wrapping for large buttons to optimize space usage.
 *
 * 该按钮专为Ribbon界面设计，支持大按钮和小按钮两种显示模式。
 * 图标尺寸会根据按钮尺寸动态调整，无法通过 `setIconSize` 手动设置。
 * 大按钮模式下还支持文字自动换行，以优化空间利用。
 *
 * @note The icon size is dynamically adjusted according to the button size and cannot be set manually via
 * `setIconSize` / `setIconSize` 函数不在起作用，iconsize是根据当前尺寸动态调整的
 */
class SA_RIBBON_EXPORT SARibbonToolButton : public QToolButton
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonToolButton)
public:
    /**
     * @brief Button type enumeration / 按钮样式枚举
     */
    enum RibbonButtonType
    {
        /**
         * @brief Large button type, corresponding to the large button in SARibbonBar / 大按钮类型，此类型对应SARibbonBar的大按钮
         */
        LargeButton,

        /**
         * @brief Small button type, corresponding to the small button in SARibbonBar, equivalent to a normal toolbar
         * button / 小按钮类型，此类型对应SARibbonBar的小按钮，等同于普通工具条的按钮
         */
        SmallButton
    };
    Q_ENUM(RibbonButtonType)

    /**
     * @brief Layout factor structure for fine-tuning button appearance / 用于微调按钮外观的布局系数结构体
     */
    struct LayoutFactor
    {
        /**
         * @brief Coefficient for the height of two-line text in large button mode / 大按钮模式下，两行文本高度系数
         *
         * This coefficient determines the height of the rectangle for two-line text when word wrapping is enabled.
         * Two-line text area height = fontMetrics.lineSpacing * coefficient.
         *
         * 这个系数决定了文字换行时2行文本的矩形高度。
         * 两行文本区域高度 = fontMetrics.lineSpacing*系数
         *
         * @note This value should be greater than 2. / 此值应该大于2
         * @default 2.05
         */
        qreal twoLineHeightFactor { 2.05 };

        /**
         * @brief Coefficient for the height of single-line text in large button mode / 大按钮模式下，单行文本高度系数
         *
         * This coefficient determines the line height for single-line text.
         * Single-line text area height = fontMetrics.lineSpacing * coefficient.
         *
         * 这个系数决定了单行文本的行高度。
         * 单行文本区域高度 = fontMetrics.lineSpacing*系数
         *
         * @note This value should be greater than 1. / 此值应该大于1
         * @default 1.2
         */
        qreal oneLineHeightFactor { 1.2 };

        /**
         * @brief Maximum aspect ratio (width/height) for the button / 按钮最大宽高比
         *
         * The maximum width of the button is determined by its height multiplied by this coefficient.
         * For example, if the button height is `h`, then the maximum width is `maxw = h * buttonMaximumAspectRatio`.
         * If the text cannot be fully displayed within this width, the button will not expand further horizontally,
         * and ellipsis (...) will be used to indicate truncated text.
         *
         * 按钮的最大宽度为按钮高度*此系数，例如按钮高度为h，那么按钮最大宽度maxw=h*buttonMaximumAspectRatio
         * 如果在此宽度下文字还无法完全显示，那么按钮将不会继续横向扩展，将使用...替代未完全显示的文字
         *
         * @default 1.4
         */
        qreal buttonMaximumAspectRatio { 1.4 };
    };

public:
    explicit SARibbonToolButton(QWidget* parent = nullptr);
    explicit SARibbonToolButton(QAction* defaultAction, QWidget* parent = nullptr);
    ~SARibbonToolButton();

    // Gets the current button type (LargeButton or SmallButton) / 获取当前按钮的类型（大按钮或小按钮）
    RibbonButtonType buttonType() const;
    // Sets the button type to LargeButton or SmallButton / 设置按钮类型为大按钮或小按钮
    void setButtonType(const RibbonButtonType& buttonType);

    // Checks if the button is a small ribbon button / 判断按钮是否为小Ribbon按钮
    bool isSmallRibbonButton() const;
    //  Checks if the button is a large ribbon button / 判断按钮是否为大Ribbon按钮
    bool isLargeRibbonButton() const;

    // Gets the current spacing value / 获取当前的间距值
    int spacing() const;
    // Sets the spacing between elements and the border / 设置元素与边框之间的间距
    void setSpacing(int v);

    // Forces an update of the internal layout rectangles / 强制更新内部布局矩形
    void updateRect();

    // Sets the layout factor for fine-tuning the button's appearance / 设置布局系数以微调按钮外观
    void setLayoutFactor(const LayoutFactor& fac);
    // layout factor for fine-tuning the button's appearance / 布局系数以微调按钮外观
    const LayoutFactor& layoutFactor() const;
    LayoutFactor& layoutFactor();

    // Enables or disables automatic text wrapping for large buttons / 为大按钮启用或禁用自动文字换行
    void setEnableWordWrap(bool on);
    // Checks if automatic text wrapping is enabled / 检查是否启用了自动文字换行
    bool isEnableWordWrap();

    // Sets the button's maximum aspect ratio (width/height) / 设置按钮的最大宽高比
    void setButtonMaximumAspectRatio(qreal v = 1.4);
    // Gets the button's maximum aspect ratio (width/height) / 获取按钮的最大宽高比
    qreal buttonMaximumAspectRatio() const;

    // Invalidates the cached size hint / 使缓存的size hint失效
    void invalidateSizeHint();

    // 大按钮尺寸
    void setLargeIconSize(const QSize& largeSize);
    QSize largeIconSize() const;

    // 小按钮尺寸
    void setSmallIconSize(const QSize& smallSize);
    QSize smallIconSize() const;

public:
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;
    virtual QSize minimumSizeHint() const Q_DECL_OVERRIDE;

protected:
    virtual void paintEvent(QPaintEvent* e) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent* e) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QMouseEvent* e) Q_DECL_OVERRIDE;
    virtual void mousePressEvent(QMouseEvent* e) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QMouseEvent* e) Q_DECL_OVERRIDE;
    virtual void focusOutEvent(QFocusEvent* e) Q_DECL_OVERRIDE;
    virtual void leaveEvent(QEvent* e) Q_DECL_OVERRIDE;
    virtual bool hitButton(const QPoint& pos) const Q_DECL_OVERRIDE;
    virtual bool event(QEvent* e) Q_DECL_OVERRIDE;
    virtual void changeEvent(QEvent* e) Q_DECL_OVERRIDE;
    virtual void actionEvent(QActionEvent* e) Q_DECL_OVERRIDE;

    // Paints the button's background and frame / 绘制按钮的背景和边框
    virtual void paintButton(QPainter& p, const QStyleOptionToolButton& opt);
    // Paints the button's icon / 绘制按钮的图标
    virtual void paintIcon(QPainter& p, const QStyleOptionToolButton& opt, const QRect& iconDrawRect);
    // Paints the button's text / 绘制按钮的文字
    virtual void paintText(QPainter& p, const QStyleOptionToolButton& opt, const QRect& textDrawRect);
    // Paints the button's indicator (e.g., dropdown arrow) / 绘制按钮的指示器（例如下拉箭头）
    virtual void paintIndicator(QPainter& p, const QStyleOptionToolButton& opt, const QRect& indicatorDrawRect);

private:
    static void drawArrow(const QStyle* style,
                          const QStyleOptionToolButton* toolbutton,
                          const QRect& rect,
                          QPainter* painter,
                          const QWidget* widget = 0);
};

namespace SA
{
QDebug operator<<(QDebug debug, const QStyleOptionToolButton& opt);
}
#endif  // SARIBBONTOOLBUTTON_H
