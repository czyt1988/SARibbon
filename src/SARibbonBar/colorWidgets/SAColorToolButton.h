#ifndef SACOLORTOOLBUTTON_H
#define SACOLORTOOLBUTTON_H
#include <QToolButton>
#include "SAColorWidgetsGlobal.h"
class QPaintEvent;
class QResizeEvent;
class QPainter;
class QStylePainter;
class SAColorMenu;
/**
 * @brief 这是一个只显示颜色的toolbutton
 *
 *
 * 在ToolButtonIconOnly模式下，如果没有setIcon,则颜色占用所有区域，如下图所示
 *
 * ┌─────┐
 * │color│
 * └─────┘
 *
 * 如果在ToolButtonIconOnly模式下有图标，图标在上面显示，下面显示颜色，如下图所示
 *
 * ┌─────┐
 * │icon │
 * │color│
 * └─────┘
 *
 * 在ToolButtonTextBesideIcon，ToolButtonTextUnderIcon下，setIconSize 可以指定颜色的大小，
 * 但只在ToolButtonTextBesideIcon，ToolButtonTextUnderIcon下有效
 *
 * 如果没有设置图标，也就是setIcon(QIcon()),iconSize作为颜色块的大小
 *
 * ┌─────────┐
 * │┌─┐      │
 * │└─┘      │
 * └─────────┘
 *
 * 如果有图标，颜色条会在图标下方，为图标高度的1/4 为图标宽度一致，如若超过控件的大小，会自动缩小体积
 *
 */
class SA_COLOR_WIDGETS_API SAColorToolButton : public QToolButton
{
    Q_OBJECT
    SA_COLOR_WIDGETS_DECLARE_PRIVATE(SAColorToolButton)
public:
    /**
     * @brief 颜色按钮的内置样式
     * @param parent
     * @return
     */
    enum ColorToolButtonStyle
    {
        WithColorMenu,  ///< 默认会构建一个SAColorMenu
        NoColorMenu     ///< 没有ColorMenu
    };

public:
    explicit SAColorToolButton(QWidget* parent = nullptr);
    explicit SAColorToolButton(ColorToolButtonStyle style, QWidget* parent = nullptr);
    ~SAColorToolButton();
    // 获取颜色
    QColor color() const;
    // 设置Margins
    void setMargins(const QMargins& mg);
    QMargins margins() const;
    // 绘制无颜色
    static void paintNoneColor(QPainter* p, const QRect& colorRect);
    // 设置颜色按钮的样式
    void setColorToolButtonStyle(ColorToolButtonStyle s);
    ColorToolButtonStyle colorToolButtonStyle() const;
    // 获取colorMenu，注意，这个函数很有可能会返回nullptr，如果ColorToolButtonStyle设置为NoColorMenu或者自己设置了菜单，此函数返回nullptr
    SAColorMenu* colorMenu() const;
    // 建立标准的颜色菜单
    SAColorMenu* createColorMenu();
public Q_SLOTS:
    // 设置颜色,会发射colorChanged信号
    void setColor(const QColor& c);

protected:
    // 获取关键的三个rect位置
    virtual void calcRect(const QStyleOptionToolButton& opt, QRect& iconRect, QRect& textRect, QRect& colorRect);
    virtual void paintButton(QStylePainter* p, const QStyleOptionToolButton& opt);
    virtual void paintIcon(QStylePainter* p, const QRect& iconRect, const QStyleOptionToolButton& opt);
    virtual void paintText(QStylePainter* p, const QRect& textRect, const QStyleOptionToolButton& opt);
    virtual void paintColor(QStylePainter* p, const QRect& colorRect, const QColor& color, const QStyleOptionToolButton& opt);

protected:
    virtual void paintEvent(QPaintEvent* e) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent* e) Q_DECL_OVERRIDE;
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;
private Q_SLOTS:
    void onButtonClicked(bool checked = false);
Q_SIGNALS:
    /**
     * @brief 颜色被点击的响应
     * @param color
     */
    void colorClicked(const QColor& color, bool checked = false);
    /**
     * @brief 颜色改变信号
     * @param color
     */
    void colorChanged(const QColor& color);
};

#endif  // SACOLORTOOLBUTTON_H
