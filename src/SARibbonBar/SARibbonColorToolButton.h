#ifndef SARIBBONCOLORTOOLBUTTON_H
#define SARIBBONCOLORTOOLBUTTON_H
#include "SARibbonToolButton.h"
#include "SARibbonGlobal.h"
class SAColorMenu;
/**
 * @brief Refer to the color setting button in the office, which can display the color below the icon(参考office的颜色设置按钮，可以显示颜色在图标下方)
 */
class SA_RIBBON_EXPORT SARibbonColorToolButton : public SARibbonToolButton
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonColorToolButton)
public:
    /**
     * @brief 颜色样式
     */
    enum ColorStyle
    {
        ColorUnderIcon,  ///< 颜色在icon下方，这个要求必须设置icon
        ColorFillToIcon  ///< 颜色作为icon，这个模式下在setColor会自动生成一个颜色icon替换掉原来的icon，因此setIcon函数没有作用
    };

public:
    explicit SARibbonColorToolButton(QWidget* parent = nullptr);
    explicit SARibbonColorToolButton(QAction* defaultAction, QWidget* parent = nullptr);
    ~SARibbonColorToolButton();
    // 获取颜色
    QColor color() const;
    // 设置颜色显示方案
    void setColorStyle(ColorStyle s);
    ColorStyle colorStyle() const;
    // 建立标准的颜色菜单
    SAColorMenu* setupStandardColorMenu();
public Q_SLOTS:
    // 设置颜色,会发射colorChanged信号
    void setColor(const QColor& c);
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

protected:
    void paintIcon(QPainter& p, const QStyleOptionToolButton& opt, const QRect& iconDrawRect);
};

#endif  // SARIBBONCOLORTOOLBUTTON_H
