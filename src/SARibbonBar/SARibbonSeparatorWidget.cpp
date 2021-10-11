#include "SARibbonSeparatorWidget.h"
#include <QStylePainter>
#include <QPainter>
#include <QDebug>

SARibbonSeparatorWidget::SARibbonSeparatorWidget(int height, QWidget *parent)
    : QWidget(parent)
    , m_topMargins(4)
    , m_bottomMargins(4)
{
    setFixedSize(6, height);
}


SARibbonSeparatorWidget::SARibbonSeparatorWidget(QWidget *parent)
    : QWidget(parent)
    , m_topMargins(4)
    , m_bottomMargins(4)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    setFixedWidth(6);
}


QSize SARibbonSeparatorWidget::sizeHint() const
{
    return (QSize(6, height()));
}


/**
 * @brief 设置分割线的上下距离
 * @param top 上边界，默认为4
 * @param bottom 下边界，默认为4
 */
void SARibbonSeparatorWidget::setTopBottomMargins(int top, int bottom)
{
    m_topMargins = top;
    m_bottomMargins = bottom;
}


void SARibbonSeparatorWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    painter.setPen(palette().window().color().darker(114));
    int x1 = rect().center().x();

    painter.drawLine(QPoint(x1, rect().top() + m_topMargins), QPoint(x1, rect().bottom() - m_bottomMargins));
}
