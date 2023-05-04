#include "SARibbonColorToolButton.h"
//===================================================
// SARibbonColorToolButton::PrivateData
//===================================================
const int c_ribbonbutton_color_height = 6;  ///< 颜色块的高度
class SARibbonColorToolButton::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonColorToolButton)
public:
    PrivateData(SARibbonColorToolButton* p);

public:
    QColor mColor;  ///< 记录颜色
};

SARibbonColorToolButton::PrivateData::PrivateData(SARibbonColorToolButton* p) : q_ptr(p)
{
}

//===================================================
// SARibbonColorToolButton
//===================================================

SARibbonColorToolButton::SARibbonColorToolButton(QWidget* parent)
    : SARibbonToolButton(parent), d_ptr(new SARibbonColorToolButton::PrivateData(this))
{
    connect(this, &SARibbonColorToolButton::clicked, this, &SARibbonColorToolButton::onButtonClicked);
}

SARibbonColorToolButton::SARibbonColorToolButton(QAction* defaultAction, QWidget* parent)
    : SARibbonToolButton(defaultAction, parent), d_ptr(new SARibbonColorToolButton::PrivateData(this))
{
    connect(this, &SARibbonColorToolButton::clicked, this, &SARibbonColorToolButton::onButtonClicked);
}

SARibbonColorToolButton::~SARibbonColorToolButton()
{
}

/**
 * @brief 获取按钮维护的颜色
 * @return
 */
QColor SARibbonColorToolButton::getColor() const
{
    return d_ptr->mColor;
}

/**
 * @brief 设置按钮的颜色
 * @note 会发射@sa colorChanged 信号
 * @param c
 */
void SARibbonColorToolButton::setColor(const QColor& c)
{
    if (d_ptr->mColor != c) {
        d_ptr->mColor = c;
        repaint();
        emit colorChanged(c);
    }
}

void SARibbonColorToolButton::onButtonClicked(bool checked)
{
    emit colorClicked(d_ptr->mColor, checked);
}

/**
 * @brief 重写paintIcon函数，把颜色加到icon下面
 * @param p
 * @param opt
 * @param iconDrawRect
 */
void SARibbonColorToolButton::paintIcon(QPainter& p, const QStyleOptionToolButton& opt, const QRect& iconDrawRect)
{
    if (!iconDrawRect.isValid()) {
        return;
    }
    //这里预留颜色的高度为6px(c_ribbonbutton_color_height)
    QRect realIconDrawRect = iconDrawRect.adjusted(0, 0, 0, -c_ribbonbutton_color_height - 2);
}
