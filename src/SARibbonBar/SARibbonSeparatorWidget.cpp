#include "SARibbonSeparatorWidget.h"
#include <QStylePainter>
#include <QPainter>
#include <QDebug>
SARibbonSeparatorWidget::SARibbonSeparatorWidget(int height, QWidget *parent)
    :QWidget(parent)
{
    setFixedSize(6,height);
}

SARibbonSeparatorWidget::SARibbonSeparatorWidget(QWidget *parent)
    :QWidget(parent)
{
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
    //qDebug() << palette().background().color().darker(114).name();
    painter.setPen(palette().background().color().darker(114));
    int x1 = rect().center().x();
    painter.drawLine(QPoint(x1, rect().top() + margin), QPoint(x1, rect().bottom() - margin));
}
