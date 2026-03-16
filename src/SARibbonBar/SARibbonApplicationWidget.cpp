#include "SARibbonApplicationWidget.h"
#include "SARibbonMainWindow.h"
#include <QEvent>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QDebug>
#include <QPainter>
#include "SARibbonMainWindow.h"

/**
 * \if ENGLISH
 * @brief Constructor for SARibbonApplicationWidget
 * @param parent The parent SARibbonMainWindow
 * @details Creates a frameless widget that follows the parent window's size
 * \endif
 * 
 * \if CHINESE
 * @brief SARibbonApplicationWidget 构造函数
 * @param parent 父窗口 SARibbonMainWindow
 * @details 创建一个无框窗口，跟随父窗口大小变化
 * \endif
 */
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

/**
 * \if ENGLISH
 * @brief Resizes the widget to match the parent's size
 * @param parentSize The size of the parent widget
 * \endif
 * 
 * \if CHINESE
 * @brief 将窗口大小调整为父窗口大小
 * @param parentSize 父窗口的大小
 * \endif
 */
void SARibbonApplicationWidget::resizeToParent(const QSize& parentSize)
{
    setGeometry(0, 0, parentSize.width(), parentSize.height());
}

/**
 * \if ENGLISH
 * @brief Event filter to handle parent window events
 * @param obj The object that received the event
 * @param ev The event
 * @return True if the event was handled, false otherwise
 * @details Handles resize events to keep the widget in sync with parent size
 * \endif
 * 
 * \if CHINESE
 * @brief 事件过滤器，处理父窗口事件
 * @param obj 接收事件的对象
 * @param ev 事件
 * @return 如果事件被处理返回true，否则返回false
 * @details 处理 resize 事件，保持窗口与父窗口大小同步
 * \endif
 */
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

/**
 * \if ENGLISH
 * @brief Handles the show event
 * @param event The show event
 * @details Resizes the widget to match parent size and requests focus
 * \endif
 * 
 * \if CHINESE
 * @brief 处理显示事件
 * @param event 显示事件
 * @details 将窗口大小调整为父窗口大小并请求焦点
 * \endif
 */
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
 * \if ENGLISH
 * @brief Handles key press events
 * @param ev The key event
 * @details Hides the widget when Escape key is pressed
 * \endif
 * 
 * \if CHINESE
 * @brief 处理按键事件
 * @param ev 按键事件
 * @details 当按下 Escape 键时隐藏窗口
 * \endif
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
