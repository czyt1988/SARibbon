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
 * \if ENGLISH
 * @brief A tool button that displays color
 * 
 * In ToolButtonIconOnly mode, if no icon is set, the color occupies the entire area:
 * 
 * ┌─────┐
 * │color│
 * └─────┘
 * 
 * If an icon is set in ToolButtonIconOnly mode, the icon is displayed above and the color below:
 * 
 * ┌─────┐
 * │icon │
 * │color│
 * └─────┘
 * 
 * In ToolButtonTextBesideIcon and ToolButtonTextUnderIcon modes, setIconSize can specify the color size,
 * but it only works in ToolButtonTextBesideIcon and ToolButtonTextUnderIcon modes.
 * 
 * If no icon is set (setIcon(QIcon())), iconSize is used as the color block size:
 * 
 * ┌─────────┐
 * │┌─┐      │
 * │└─┘      │
 * └─────────┘
 * 
 * If there is an icon, the color bar will be below the icon, 1/4 of the icon height and the same width as the icon.
 * If it exceeds the size of the control, it will automatically shrink.
 * \endif
 *
 * \if CHINESE
 * @brief 这是一个只显示颜色的toolbutton
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
 * \endif
 */
class SA_COLOR_WIDGETS_API SAColorToolButton : public QToolButton
{
    Q_OBJECT
    SA_COLOR_WIDGETS_DECLARE_PRIVATE(SAColorToolButton)
public:
    /**
     * @brief Color button built-in styles
     */
    enum ColorToolButtonStyle
    {
        WithColorMenu,  ///< Default will build a SAColorMenu
        NoColorMenu     ///< No ColorMenu
    };

public:
    /// Constructor for SAColorToolButton
    explicit SAColorToolButton(QWidget* parent = nullptr);
    /// Constructor for SAColorToolButton with style
    explicit SAColorToolButton(ColorToolButtonStyle style, QWidget* parent = nullptr);
    /// Destructor for SAColorToolButton
    ~SAColorToolButton();
    /// Get color
    QColor color() const;
    /// Set margins
    void setMargins(const QMargins& mg);
    /// Get margins
    QMargins margins() const;
    /// Paint none color
    static void paintNoneColor(QPainter* p, const QRect& colorRect);
    /// Set color tool button style
    void setColorToolButtonStyle(ColorToolButtonStyle s);
    /// Get color tool button style
    ColorToolButtonStyle colorToolButtonStyle() const;
    /// Get colorMenu, note that this function may return nullptr if ColorToolButtonStyle is set to NoColorMenu or if a custom menu is set
    SAColorMenu* colorMenu() const;
    /// Create standard color menu
    SAColorMenu* createColorMenu();
public Q_SLOTS:
    /// Set color, will emit colorChanged signal
    void setColor(const QColor& c);

protected:
    /// Calculate positions of basic components
    virtual void calcRect(const QStyleOptionToolButton& opt, QRect& iconRect, QRect& textRect, QRect& colorRect);
    /// Paint button
    virtual void paintButton(QStylePainter* p, const QStyleOptionToolButton& opt);
    /// Paint icon
    virtual void paintIcon(QStylePainter* p, const QRect& iconRect, const QStyleOptionToolButton& opt);
    /// Paint text
    virtual void paintText(QStylePainter* p, const QRect& textRect, const QStyleOptionToolButton& opt);
    /// Paint color
    virtual void paintColor(QStylePainter* p, const QRect& colorRect, const QColor& color, const QStyleOptionToolButton& opt);

protected:
    /// Handle paint event
    virtual void paintEvent(QPaintEvent* e) Q_DECL_OVERRIDE;
    /// Handle resize event
    virtual void resizeEvent(QResizeEvent* e) Q_DECL_OVERRIDE;
    /// Override sizeHint
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;
private Q_SLOTS:
    /// Handle button clicked
    void onButtonClicked(bool checked = false);
Q_SIGNALS:
    /**
     * \if ENGLISH
     * @brief Signal emitted when color is clicked
     * @param color The clicked color
     * @param checked Checked state
     * \endif
     *
     * \if CHINESE
     * @brief 颜色被点击的响应
     * @param color 颜色
     * @param checked 选中状态
     * \endif
     */
    void colorClicked(const QColor& color, bool checked = false);
    /**
     * \if ENGLISH
     * @brief Signal emitted when color changes
     * @param color The new color
     * \endif
     *
     * \if CHINESE
     * @brief 颜色改变信号
     * @param color 颜色
     * \endif
     */
    void colorChanged(const QColor& color);
};

#endif  // SACOLORTOOLBUTTON_H
