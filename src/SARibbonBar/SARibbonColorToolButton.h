#ifndef SARIBBONCOLORTOOLBUTTON_H
#define SARIBBONCOLORTOOLBUTTON_H
#include "SARibbonToolButton.h"
#include "SARibbonGlobal.h"
class SAColorMenu;
/**
 * \if ENGLISH
 * @brief Color tool button similar to Office color setting buttons
 * @details This button can display color below the icon or use color as the icon itself
 * \endif
 *
 * \if CHINESE
 * @brief 参考office的颜色设置按钮，可以显示颜色在图标下方
 * @details 此按钮可以在图标下方显示颜色，或使用颜色作为图标本身
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonColorToolButton : public SARibbonToolButton
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonColorToolButton)
public:
    /**
     * \if ENGLISH
     * @brief Color display style
     * \endif
     *
     * \if CHINESE
     * @brief 颜色样式
     * \endif
     */
    enum ColorStyle
    {
        ColorUnderIcon,  ///< 颜色在icon下方，这个要求必须设置icon
        ColorFillToIcon  ///< 颜色作为icon，这个模式下在setColor会自动生成一个颜色icon替换掉原来的icon，因此setIcon函数没有作用
    };

public:
    // Constructor for SARibbonColorToolButton
    explicit SARibbonColorToolButton(QWidget* parent = nullptr);
    // Constructor with default action
    explicit SARibbonColorToolButton(QAction* defaultAction, QWidget* parent = nullptr);
    // Destructor for SARibbonColorToolButton
    ~SARibbonColorToolButton();
    // Get the current color
    QColor color() const;
    // Set the color display style
    void setColorStyle(ColorStyle s);
    // Get the current color display style
    ColorStyle colorStyle() const;
    // Set up a standard color menu
    SAColorMenu* setupStandardColorMenu();
public Q_SLOTS:
    // Set the color, emits colorChanged signal
    void setColor(const QColor& c);
private Q_SLOTS:
    void onButtonClicked(bool checked = false);
Q_SIGNALS:
    /**
     * \if ENGLISH
     * @brief Signal emitted when color is clicked
     * @param color The clicked color
     * @param checked Whether the button is checked
     * \endif
     *
     * \if CHINESE
     * @brief 颜色被点击的响应
     * @param color 点击的颜色
     * @param checked 按钮是否被选中
     * \endif
     */
    void colorClicked(const QColor& color, bool checked = false);
    /**
     * \if ENGLISH
     * @brief Signal emitted when color is changed
     * @param color The new color
     * \endif
     *
     * \if CHINESE
     * @brief 颜色改变信号
     * @param color 新的颜色
     * \endif
     */
    void colorChanged(const QColor& color);

protected:
    /// Override createIconPixmap to add color under the icon
    QPixmap createIconPixmap(const QStyleOptionToolButton& opt, const QSize& iconSize) const override;
};

#endif  // SARIBBONCOLORTOOLBUTTON_H
