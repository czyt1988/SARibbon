#include "SARibbonApplicationWidget.h"
#include "SARibbonMainWindow.h"
#include <QEvent>
#include <QKeyEvent>
#include <QResizeEvent>
#include "SARibbonMainWindow.h"

SARibbonApplicationWidget::SARibbonApplicationWidget(SARibbonMainWindow* parent) : QWidget(parent)
{
    parent->installEventFilter(this);
    move(0, 0);
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
    return QWidget::eventFilter(obj, ev);
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
            ev->accept();
        }
    }
    return QWidget::keyPressEvent(ev);
}
