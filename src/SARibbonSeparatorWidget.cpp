#include "SARibbonSeparatorWidget.h"
#include <QStylePainter>
#include <QPainter>
SARibbonSeparatorWidget::SARibbonSeparatorWidget(int height, QWidget *parent)
    :QWidget(parent)
{
    setFixedSize(6,height);
}

QSize SARibbonSeparatorWidget::sizeHint() const
{
    return QSize(6,height());
}

void SARibbonSeparatorWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    int margin = 3;
    painter.setPen(palette().background().color().darker(114));
    int x1 = rect().center().x();
    painter.drawLine(QPoint(x1, rect().top() + margin), QPoint(x1, rect().bottom() - margin));
}
