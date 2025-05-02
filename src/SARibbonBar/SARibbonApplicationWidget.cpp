#include "SARibbonApplicationWidget.h"
#include "SARibbonMainWindow.h"
#include <QEvent>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QDebug>
#include <QPainter>
#include "SARibbonMainWindow.h"

SARibbonApplicationWidget::SARibbonApplicationWidget(SARibbonMainWindow* parent) : QFrame(parent)
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);  // 去除边框
    parent->installEventFilter(this);
    if (parent) {
        setGeometry(0, 0, parent->width(), parent->height());
    } else {
        setGeometry(0, 0, 300, 300);
    }
}

void SARibbonApplicationWidget::resizeToParent(const QSize& parentSize)
{
    setGeometry(0, 0, parentSize.width(), parentSize.height());
}

bool SARibbonApplicationWidget::eventFilter(QObject* obj, QEvent* ev)
{
    if (obj && ev && (obj == parent())) {
        switch (ev->type()) {
        case QEvent::Resize: {
            // 跟随父窗口的尺寸变化
            resizeToParent(static_cast< QResizeEvent* >(ev)->size());
            break;
        }
        default:
            break;
        }
    }
    return QFrame::eventFilter(obj, ev);
}

void SARibbonApplicationWidget::showEvent(QShowEvent* event)
{
    QWidget* par = parentWidget();
    if (par) {
        resizeToParent(par->size());
    }
    setFocus();  // 显示窗口时请求焦点
    QFrame::showEvent(event);
}

/**
 * @brief 按下exe键隐藏窗口
 * @param ev
 */
void SARibbonApplicationWidget::keyPressEvent(QKeyEvent* ev)
{
    if (ev) {
        if (ev->key() == Qt::Key_Escape) {
            hide();
            qDebug() << "Key_Escape";
            ev->accept();
        }
    }
    return QFrame::keyPressEvent(ev);
}
