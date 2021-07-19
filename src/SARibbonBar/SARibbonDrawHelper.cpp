#include "SARibbonDrawHelper.h"


SARibbonDrawHelper::SARibbonDrawHelper()
{
}


QPixmap SARibbonDrawHelper::iconToPixmap(const QIcon& icon, QWidget *widget, const QStyleOption *opt, const QSize& icoSize)
{
    QIcon::Mode mode;
    QIcon::State state;

    if (!(opt->state & QStyle::State_Enabled)) {
        mode = QIcon::Disabled;
    }else if ((opt->state & QStyle::State_MouseOver) && (opt->state & QStyle::State_AutoRaise)) {
        mode = QIcon::Active;
    }else{
        mode = QIcon::Normal;
    }

    if (opt->state & QStyle::State_Selected || opt->state & QStyle::State_On) {
        state = QIcon::On;
    }else{
        state = QIcon::Off;
    }
    return (icon.pixmap(widget->window()->windowHandle(), icoSize, mode, state));
}


void SARibbonDrawHelper::drawIcon(const QIcon& icon, QPainter *painter, const QStyleOption *opt
    , int x, int y, int width, int height)
{
    QIcon::Mode mode;
    QIcon::State state;

    if (!(opt->state & QStyle::State_Enabled)) {
        mode = QIcon::Disabled;
    }else if ((opt->state & QStyle::State_MouseOver) && (opt->state & QStyle::State_AutoRaise)) {
        mode = QIcon::Active;
    }else{
        mode = QIcon::Normal;
    }

    if (opt->state & QStyle::State_Selected || opt->state & QStyle::State_On) {
        state = QIcon::On;
    }else{
        state = QIcon::Off;
    }

    icon.paint(painter, x, y, width, height, Qt::AlignCenter
        , mode, state);
}


void SARibbonDrawHelper::drawIcon(const QIcon& icon, QPainter *painter, const QStyleOption *opt, const QRect& rect)
{
    QIcon::Mode mode;
    QIcon::State state;

    if (!(opt->state & QStyle::State_Enabled)) {
        mode = QIcon::Disabled;
    }else if ((opt->state & QStyle::State_MouseOver) && (opt->state & QStyle::State_AutoRaise)) {
        mode = QIcon::Active;
    }else{
        mode = QIcon::Normal;
    }

    if (opt->state & QStyle::State_Selected || opt->state & QStyle::State_On) {
        state = QIcon::On;
    }else{
        state = QIcon::Off;
    }

    icon.paint(painter, rect, Qt::AlignCenter
        , mode, state);
}


QSize SARibbonDrawHelper::iconActualSize(const QIcon& icon, const QStyleOption *opt, const QSize& iconSize)
{
    QIcon::Mode mode;
    QIcon::State state;

    if (!(opt->state & QStyle::State_Enabled)) {
        mode = QIcon::Disabled;
    }else if ((opt->state & QStyle::State_MouseOver) && (opt->state & QStyle::State_AutoRaise)) {
        mode = QIcon::Active;
    }else{
        mode = QIcon::Normal;
    }

    if (opt->state & QStyle::State_Selected || opt->state & QStyle::State_On) {
        state = QIcon::On;
    }else{
        state = QIcon::Off;
    }
    return (icon.actualSize(iconSize, mode, state));
}


void SARibbonDrawHelper::drawText(const QString& text, QStylePainter *painter, const QStyleOption *opt, Qt::Alignment al, int x, int y, int width, int height)
{
    painter->drawItemText(QRect(x, y, width, height)
        , al
        , opt->palette
        , opt->state & QStyle::State_Enabled
        , text
        );
}


void SARibbonDrawHelper::drawText(const QString& text, QStylePainter *painter, const QStyleOption *opt, Qt::Alignment al, const QRect& rect)
{
    painter->drawItemText(rect
        , al
        , opt->palette
        , opt->state & QStyle::State_Enabled
        , text
        );
}
