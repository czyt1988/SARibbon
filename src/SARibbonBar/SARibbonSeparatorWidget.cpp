#include "SARibbonSeparatorWidget.h"
#include <QStylePainter>
#include <QPainter>
#include <QDebug>

SARibbonSeparatorWidget::SARibbonSeparatorWidget(QWidget* parent) : QFrame(parent)
{
    setFrameShape(QFrame::VLine);
    setFrameShadow(QFrame::Plain);
    setLineWidth(1);
    setMidLineWidth(1);
}

QSize SARibbonSeparatorWidget::sizeHint() const
{
    QSize sh = QFrame::sizeHint();
    sh.setWidth(1);
    return sh;
}
