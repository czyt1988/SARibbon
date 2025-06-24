#include "SAFramelessHelper.h"
#include <QRect>
#include <QRubberBand>
#include <QMouseEvent>
#include <QHoverEvent>
#include <QApplication>
#include <QDebug>
#include <QScreen>
#include "SARibbonMainWindow.h"
class SAPrivateFramelessWidgetData;

/*****
 * FramelessHelperPrivate
 * 存储界面对应的数据集合，以及是否可移动、可缩放属性
 *****/
class SAFramelessHelper::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SAFramelessHelper)
    friend class SAPrivateFramelessWidgetData;

public:
    PrivateData(SAFramelessHelper* p);
    QHash< QWidget*, SAPrivateFramelessWidgetData* > m_widgetDataHash;
    bool m_bWidgetMovable { true };
    bool m_bWidgetResizable { true };
    bool m_bRubberBandOnResize { true };
    bool m_bRubberBandOnMove { true };
};

SAFramelessHelper::PrivateData::PrivateData(SAFramelessHelper* p) : q_ptr(p)
{
}

/*****
 * CursorPosCalculator
 * 计算鼠标是否位于左、上、右、下、左上角、左下角、右上角、右下角
 *****/
class SAPrivateFramelessCursorPosCalculator
{
public:
    explicit SAPrivateFramelessCursorPosCalculator();
    void reset();
    void recalculate(const QPoint& globalMousePos, const QRect& frameRect);

public:
    bool mIsOnEdges { true };
    bool mIsOnLeftEdge { true };
    bool mIsOnRightEdge { true };
    bool mIsOnTopEdge { true };
    bool mIsOnBottomEdge { true };
    bool mIsOnTopLeftEdge { true };
    bool mIsOnBottomLeftEdge { true };
    bool mIsOnTopRightEdge { true };
    bool mIsOnBottomRightEdge { true };

    static int s_borderWidth;
    static int s_titleHeight;
};

int SAPrivateFramelessCursorPosCalculator::s_borderWidth = 5;
int SAPrivateFramelessCursorPosCalculator::s_titleHeight = 30;

/***** CursorPosCalculator *****/
SAPrivateFramelessCursorPosCalculator::SAPrivateFramelessCursorPosCalculator()
{
    reset();
}

void SAPrivateFramelessCursorPosCalculator::reset()
{
    mIsOnEdges           = false;
    mIsOnLeftEdge        = false;
    mIsOnRightEdge       = false;
    mIsOnTopEdge         = false;
    mIsOnBottomEdge      = false;
    mIsOnTopLeftEdge     = false;
    mIsOnBottomLeftEdge  = false;
    mIsOnTopRightEdge    = false;
    mIsOnBottomRightEdge = false;
}

void SAPrivateFramelessCursorPosCalculator::recalculate(const QPoint& gMousePos, const QRect& frameRect)
{
    int globalMouseX = gMousePos.x();
    int globalMouseY = gMousePos.y();

    int frameX = frameRect.x();
    int frameY = frameRect.y();

    int frameWidth  = frameRect.width();
    int frameHeight = frameRect.height();

    // 使用设备独立像素计算边界
    // mIsOnLeftEdge = (globalMouseX >= frameX && globalMouseX <= frameX + s_borderWidth);
    // mIsOnRightEdge = (globalMouseX >= frameX + frameWidth - s_borderWidth && globalMouseX <= frameX + frameWidth);
    const int border = qRound(qApp->devicePixelRatio() * s_borderWidth);
    mIsOnLeftEdge    = (globalMouseX >= frameX && globalMouseX <= frameX + border);
    mIsOnRightEdge   = (globalMouseX >= frameX + frameWidth - border && globalMouseX <= frameX + frameWidth);

    mIsOnTopEdge = (globalMouseY >= frameY && globalMouseY <= frameY + border);

    mIsOnBottomEdge = (globalMouseY >= frameY + frameHeight - border && globalMouseY <= frameY + frameHeight);

    mIsOnTopLeftEdge     = mIsOnTopEdge && mIsOnLeftEdge;
    mIsOnBottomLeftEdge  = mIsOnBottomEdge && mIsOnLeftEdge;
    mIsOnTopRightEdge    = mIsOnTopEdge && mIsOnRightEdge;
    mIsOnBottomRightEdge = mIsOnBottomEdge && mIsOnRightEdge;

    mIsOnEdges = mIsOnLeftEdge || mIsOnRightEdge || mIsOnTopEdge || mIsOnBottomEdge;
}

/*****
 * WidgetData
 * 更新鼠标样式、移动窗体、缩放窗体
 *****/
class SAPrivateFramelessWidgetData
{
public:
    explicit SAPrivateFramelessWidgetData(SAFramelessHelper::PrivateData* pd, QWidget* pTopLevelWidget);
    ~SAPrivateFramelessWidgetData();
    QWidget* widget();

    // 处理鼠标事件-划过、按下、释放、移动
    bool handleWidgetEvent(QEvent* event);

    // 更新橡皮筋状态
    void updateRubberBandStatus();

    // 更新鼠标样式
    void updateCursorShape(const QPoint& gMousePos);

    // 重置窗体大小
    void resizeWidget(const QPoint& gMousePos);

    // 移动窗体
    void moveWidget(const QPoint& gMousePos);

    // 处理鼠标按下
    bool handleMousePressEvent(QMouseEvent* event);

    // 处理鼠标释放
    bool handleMouseReleaseEvent(QMouseEvent* event);

    // 处理鼠标移动
    bool handleMouseMoveEvent(QMouseEvent* event);

    // 处理鼠标离开
    bool handleLeaveEvent(QEvent* event);

    // 处理鼠标进入
    bool handleHoverMoveEvent(QHoverEvent* event);

    // 处理鼠标双击事件
    bool handleDoubleClickedMouseEvent(QMouseEvent* event);

    QScreen* currentScreen() const
    {
        return m_pWidget->screen();
    }

    QRect screenAvailableGeometry() const
    {
        if (QScreen* screen = currentScreen()) {
            return screen->availableGeometry();
        }
        return QApplication::primaryScreen()->availableGeometry();
    }

    QRect screenGeometry() const
    {
        if (QScreen* screen = currentScreen()) {
            return screen->geometry();
        }
        return QApplication::primaryScreen()->geometry();
    }

private:
    SAFramelessHelper::PrivateData* d;
    QRubberBand* m_pRubberBand;
    QWidget* m_pWidget;
    QPoint m_ptDragPos;
    SAPrivateFramelessCursorPosCalculator m_pressedMousePos;
    SAPrivateFramelessCursorPosCalculator m_moveMousePos;
    bool m_bLeftButtonPressed;
    bool m_bCursorShapeChanged;
    bool m_bLeftButtonTitlePressed;
    Qt::WindowFlags m_windowFlags;
};

/***** WidgetData *****/
SAPrivateFramelessWidgetData::SAPrivateFramelessWidgetData(SAFramelessHelper::PrivateData* pd, QWidget* pTopLevelWidget)
{
    d                         = pd;
    m_pWidget                 = pTopLevelWidget;
    m_bLeftButtonPressed      = false;
    m_bCursorShapeChanged     = false;
    m_bLeftButtonTitlePressed = false;
    m_pRubberBand             = NULL;

    m_windowFlags = m_pWidget->windowFlags();
    m_pWidget->setMouseTracking(true);
    m_pWidget->setAttribute(Qt::WA_Hover, true);

    updateRubberBandStatus();
}

SAPrivateFramelessWidgetData::~SAPrivateFramelessWidgetData()
{
    m_pWidget->setMouseTracking(false);
    m_pWidget->setWindowFlags(m_windowFlags);
    m_pWidget->setAttribute(Qt::WA_Hover, false);

    delete m_pRubberBand;
    m_pRubberBand = NULL;
}

QWidget* SAPrivateFramelessWidgetData::widget()
{
    return (m_pWidget);
}

bool SAPrivateFramelessWidgetData::handleWidgetEvent(QEvent* event)
{
    switch (event->type()) {
    case QEvent::MouseButtonPress:
        return (handleMousePressEvent(static_cast< QMouseEvent* >(event)));

    case QEvent::MouseButtonRelease:
        return (handleMouseReleaseEvent(static_cast< QMouseEvent* >(event)));

    case QEvent::MouseMove:
        return (handleMouseMoveEvent(static_cast< QMouseEvent* >(event)));

    case QEvent::Leave:
        return (handleLeaveEvent(static_cast< QMouseEvent* >(event)));

    case QEvent::HoverMove:
        return (handleHoverMoveEvent(static_cast< QHoverEvent* >(event)));

    case QEvent::MouseButtonDblClick:
        return (handleDoubleClickedMouseEvent(static_cast< QMouseEvent* >(event)));

    default:
        break;
    }
    return (false);
}

void SAPrivateFramelessWidgetData::updateRubberBandStatus()
{
    if (d->m_bRubberBandOnMove || d->m_bRubberBandOnResize) {
        if (NULL == m_pRubberBand) {
            m_pRubberBand = new QRubberBand(QRubberBand::Rectangle);
        }
    } else {
        delete m_pRubberBand;
        m_pRubberBand = NULL;
    }
}

void SAPrivateFramelessWidgetData::updateCursorShape(const QPoint& gMousePos)
{
    if (m_pWidget->isFullScreen() || m_pWidget->isMaximized()) {
        if (m_bCursorShapeChanged) {
            m_pWidget->unsetCursor();
        }
        return;
    }

    m_moveMousePos.recalculate(gMousePos, m_pWidget->frameGeometry());

    if (m_moveMousePos.mIsOnTopLeftEdge || m_moveMousePos.mIsOnBottomRightEdge) {
        m_pWidget->setCursor(Qt::SizeFDiagCursor);
        m_bCursorShapeChanged = true;
    } else if (m_moveMousePos.mIsOnTopRightEdge || m_moveMousePos.mIsOnBottomLeftEdge) {
        m_pWidget->setCursor(Qt::SizeBDiagCursor);
        m_bCursorShapeChanged = true;
    } else if (m_moveMousePos.mIsOnLeftEdge || m_moveMousePos.mIsOnRightEdge) {
        m_pWidget->setCursor(Qt::SizeHorCursor);
        m_bCursorShapeChanged = true;
    } else if (m_moveMousePos.mIsOnTopEdge || m_moveMousePos.mIsOnBottomEdge) {
        m_pWidget->setCursor(Qt::SizeVerCursor);
        m_bCursorShapeChanged = true;
    } else {
        if (m_bCursorShapeChanged) {
            m_pWidget->unsetCursor();
            m_bCursorShapeChanged = false;
        }
    }
}

void SAPrivateFramelessWidgetData::resizeWidget(const QPoint& gMousePos)
{
    QRect origRect;

    if (d->m_bRubberBandOnResize) {
        origRect = m_pRubberBand->frameGeometry();
    } else {
        origRect = m_pWidget->frameGeometry();
    }

    int left   = origRect.left();
    int top    = origRect.top();
    int right  = origRect.right();
    int bottom = origRect.bottom();

    origRect.getCoords(&left, &top, &right, &bottom);

    int minWidth  = m_pWidget->minimumWidth();
    int minHeight = m_pWidget->minimumHeight();

    if (m_pressedMousePos.mIsOnTopLeftEdge) {
        left = gMousePos.x();
        top  = gMousePos.y();
    } else if (m_pressedMousePos.mIsOnBottomLeftEdge) {
        left   = gMousePos.x();
        bottom = gMousePos.y();
    } else if (m_pressedMousePos.mIsOnTopRightEdge) {
        right = gMousePos.x();
        top   = gMousePos.y();
    } else if (m_pressedMousePos.mIsOnBottomRightEdge) {
        right  = gMousePos.x();
        bottom = gMousePos.y();
    } else if (m_pressedMousePos.mIsOnLeftEdge) {
        left = gMousePos.x();
    } else if (m_pressedMousePos.mIsOnRightEdge) {
        right = gMousePos.x();
    } else if (m_pressedMousePos.mIsOnTopEdge) {
        top = gMousePos.y();
    } else if (m_pressedMousePos.mIsOnBottomEdge) {
        bottom = gMousePos.y();
    }

    QRect newRect(QPoint(left, top), QPoint(right, bottom));

    if (newRect.isValid()) {
        if (minWidth > newRect.width()) {
            if (left != origRect.left()) {
                newRect.setLeft(origRect.left());
            } else {
                newRect.setRight(origRect.right());
            }
        }
        if (minHeight > newRect.height()) {
            if (top != origRect.top()) {
                newRect.setTop(origRect.top());
            } else {
                newRect.setBottom(origRect.bottom());
            }
        }

        // 添加屏幕边界约束
        QRect screenGeom = screenAvailableGeometry();
        if (newRect.left() < screenGeom.left()) {
            newRect.setLeft(screenGeom.left());
        }
        if (newRect.top() < screenGeom.top()) {
            newRect.setTop(screenGeom.top());
        }
        if (newRect.right() > screenGeom.right()) {
            newRect.setRight(screenGeom.right());
        }
        if (newRect.bottom() > screenGeom.bottom()) {
            newRect.setBottom(screenGeom.bottom());
        }

        if (d->m_bRubberBandOnResize) {
            m_pRubberBand->setGeometry(newRect);
        } else {
            m_pWidget->setGeometry(newRect);
        }
    }
}

void SAPrivateFramelessWidgetData::moveWidget(const QPoint& gMousePos)
{
    if (d->m_bRubberBandOnMove) {
        m_pRubberBand->move(gMousePos - m_ptDragPos);
    } else {
        m_pWidget->move(gMousePos - m_ptDragPos);
    }
}

bool SAPrivateFramelessWidgetData::handleMousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_bLeftButtonPressed      = true;
        m_bLeftButtonTitlePressed = event->pos().y() < m_moveMousePos.s_titleHeight;

        QRect frameRect = m_pWidget->frameGeometry();
        auto gp         = SA_MOUSEEVENT_GLOBALPOS_POINT(event);
        m_pressedMousePos.recalculate(gp, frameRect);

        m_ptDragPos = gp - frameRect.topLeft();

        if (m_pressedMousePos.mIsOnEdges) {
            if (m_pWidget->isMaximized()) {
                // 窗口在最大化状态时，点击边界不做任何处理
                return (false);
            }
            if (d->m_bRubberBandOnResize) {
                m_pRubberBand->setGeometry(frameRect);
                m_pRubberBand->show();
                return (true);
            }
        } else if (d->m_bRubberBandOnMove && m_bLeftButtonTitlePressed) {
            if (m_pWidget->isMaximized()) {
                // 窗口在最大化状态时，点击标题栏不做任何处理
                return (false);
            }
            m_pRubberBand->setGeometry(frameRect);
            m_pRubberBand->show();
            return (true);
        }
    }
    return (false);
}

bool SAPrivateFramelessWidgetData::handleMouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_bLeftButtonPressed      = false;
        m_bLeftButtonTitlePressed = false;
        m_pressedMousePos.reset();
        if (m_pRubberBand && m_pRubberBand->isVisible()) {
            m_pRubberBand->hide();
            m_pWidget->setGeometry(m_pRubberBand->geometry());
            return (true);
        }
    }
    return (false);
}

bool SAPrivateFramelessWidgetData::handleMouseMoveEvent(QMouseEvent* event)
{
    QPoint p = SA_MOUSEEVENT_GLOBALPOS_POINT(event);
    if (m_bLeftButtonPressed) {
        if (d->m_bWidgetResizable && m_pressedMousePos.mIsOnEdges) {
            if (m_pWidget->isMaximized()) {
                // 窗口在最大化状态时，点击边界不做任何处理
                return (false);
            }
            // 限制在屏幕边界内调整
            QRect screenGeom = screenGeometry();
            p.setX(qBound(screenGeom.left() + 1, p.x(), screenGeom.right() - 1));
            p.setY(qBound(screenGeom.top() + 1, p.y(), screenGeom.bottom() - 1));

            resizeWidget(p);
            return (true);
        } else if (d->m_bWidgetMovable && m_bLeftButtonTitlePressed) {
            if (m_pWidget->isMaximized()) {
                // 动态计算恢复位置
                QRect normalGeometry = m_pWidget->normalGeometry();
                if (normalGeometry.isEmpty()) {
                    normalGeometry = screenAvailableGeometry().adjusted(100, 100, -100, -100);
                }

                // 计算鼠标在标题栏的相对位置
                double relativeX = static_cast< double >(event->pos().x()) / m_pWidget->width();
                int newX         = p.x() - relativeX * normalGeometry.width();

                // 确保窗口不会移出屏幕
                QRect screenGeom = screenAvailableGeometry();
                newX             = qBound(screenGeom.left(), newX, screenGeom.right() - normalGeometry.width());

                int newY = p.y() - event->pos().y() / 2;  // 保留部分标题栏可见

                m_pWidget->showNormal();
                m_pWidget->move(newX, newY);

                // 更新拖动位置
                m_ptDragPos = QPoint(normalGeometry.width() * relativeX, event->pos().y());
                return true;
            }
            // 多屏幕边界检查
            QRect screenGeom = screenGeometry();
            QPoint newPos    = p - m_ptDragPos;
            QRect widgetGeom = m_pWidget->frameGeometry();
            widgetGeom.moveTopLeft(newPos);

            // 确保窗口不会移出当前屏幕
            if (widgetGeom.left() < screenGeom.left()) {
                newPos.setX(screenGeom.left());
            }
            if (widgetGeom.top() < screenGeom.top()) {
                newPos.setY(screenGeom.top());
            }
            if (widgetGeom.right() > screenGeom.right()) {
                newPos.setX(screenGeom.right() - widgetGeom.width());
            }
            if (widgetGeom.bottom() > screenGeom.bottom()) {
                newPos.setY(screenGeom.bottom() - widgetGeom.height());
            }

            if (d->m_bRubberBandOnMove) {
                m_pRubberBand->move(newPos);
            } else {
                m_pWidget->move(newPos);
            }
            return true;
        }
        return (false);
    } else if (d->m_bWidgetResizable) {
        updateCursorShape(p);
    }
    return (false);
}

bool SAPrivateFramelessWidgetData::handleLeaveEvent(QEvent* event)
{
    Q_UNUSED(event)
    if (!m_bLeftButtonPressed) {
        m_pWidget->unsetCursor();
        return (true);
    }
    return (false);
}

bool SAPrivateFramelessWidgetData::handleHoverMoveEvent(QHoverEvent* event)
{
    if (d->m_bWidgetResizable) {
        updateCursorShape(m_pWidget->mapToGlobal(SA_HOVEREVENT_POS_POINT(event)));
    }
    return (false);
}

bool SAPrivateFramelessWidgetData::handleDoubleClickedMouseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        if (m_pWidget) {
            SARibbonMainWindow* mainwindow = qobject_cast< SARibbonMainWindow* >(m_pWidget);
            if (mainwindow) {
                if (mainwindow->windowFlags() & Qt::WindowMaximizeButtonHint) {
                    // 在最大化按钮显示时才进行shownormal处理
                    bool titlePressed = event->pos().y() < m_moveMousePos.s_titleHeight;
                    if (titlePressed) {
                        // 多屏幕支持：获取当前屏幕的可用区域
                        QRect screenGeom = screenAvailableGeometry();
                        if (m_pWidget->isMaximized()) {
                            m_pWidget->showNormal();
                            // 居中显示
                            m_pWidget->move(screenGeom.center() - m_pWidget->rect().center());
                        } else {
                            m_pWidget->showMaximized();
                        }
                        return (true);
                    }
                }
            }
        }
    }
    return (false);
}

//===================================================
// SAFramelessHelper
//===================================================
SAFramelessHelper::SAFramelessHelper(QObject* parent) : QObject(parent), d_ptr(new SAFramelessHelper::PrivateData(this))
{
    d_ptr->m_bWidgetMovable      = true;
    d_ptr->m_bWidgetResizable    = true;
    d_ptr->m_bRubberBandOnResize = false;
    d_ptr->m_bRubberBandOnMove   = false;
    if (parent) {
        QWidget* w = qobject_cast< QWidget* >(parent);
        if (w) {
            w->setWindowFlags(w->windowFlags() | Qt::FramelessWindowHint);
            setWidgetMovable(true);       // 设置窗体可移动
            setWidgetResizable(true);     // 设置窗体可缩放
            setRubberBandOnMove(false);   // 设置橡皮筋效果-可移动
            setRubberBandOnResize(true);  // 设置橡皮筋效果-可缩放
            activateOn(w);                // 激活当前窗体
        }
    }
}

SAFramelessHelper::~SAFramelessHelper()
{
    QList< QWidget* > keys = d_ptr->m_widgetDataHash.keys();
    int size               = keys.size();

    for (int i = 0; i < size; ++i) {
        delete d_ptr->m_widgetDataHash.take(keys[ i ]);
    }
}

bool SAFramelessHelper::eventFilter(QObject* obj, QEvent* event)
{
    switch (event->type()) {
    case QEvent::MouseMove:
    case QEvent::HoverMove:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::Leave: {
        SAPrivateFramelessWidgetData* data = d_ptr->m_widgetDataHash.value(static_cast< QWidget* >(obj));
        if (data) {
            return (data->handleWidgetEvent(event));
        }
    } break;
    case QEvent::ScreenChangeInternal:
        // 处理屏幕切换事件：
        if (SAPrivateFramelessWidgetData* data = d_ptr->m_widgetDataHash.value(static_cast< QWidget* >(obj))) {
            data->updateCursorShape(QCursor::pos());
        }
        break;
    default:
        break;
    }
    return (QObject::eventFilter(obj, event));
}

void SAFramelessHelper::activateOn(QWidget* topLevelWidget)
{
    if (!d_ptr->m_widgetDataHash.contains(topLevelWidget)) {
        SAPrivateFramelessWidgetData* data = new SAPrivateFramelessWidgetData(d_ptr.get(), topLevelWidget);
        d_ptr->m_widgetDataHash.insert(topLevelWidget, data);

        topLevelWidget->installEventFilter(this);
    }
}

void SAFramelessHelper::removeFrom(QWidget* topLevelWidget)
{
    SAPrivateFramelessWidgetData* data = d_ptr->m_widgetDataHash.take(topLevelWidget);

    if (data) {
        topLevelWidget->removeEventFilter(this);
        delete data;
    }
}

void SAFramelessHelper::setRubberBandOnMove(bool movable)
{
    d_ptr->m_bRubberBandOnMove                  = movable;
    QList< SAPrivateFramelessWidgetData* > list = d_ptr->m_widgetDataHash.values();

    foreach (SAPrivateFramelessWidgetData* data, list) {
        data->updateRubberBandStatus();
    }
}

void SAFramelessHelper::setWidgetMovable(bool movable)
{
    d_ptr->m_bWidgetMovable = movable;
}

void SAFramelessHelper::setWidgetResizable(bool resizable)
{
    d_ptr->m_bWidgetResizable = resizable;
}

void SAFramelessHelper::setRubberBandOnResize(bool resizable)
{
    d_ptr->m_bRubberBandOnResize                = resizable;
    QList< SAPrivateFramelessWidgetData* > list = d_ptr->m_widgetDataHash.values();

    foreach (SAPrivateFramelessWidgetData* data, list) {
        data->updateRubberBandStatus();
    }
}

void SAFramelessHelper::setBorderWidth(int width)
{
    if (width > 0) {
        SAPrivateFramelessCursorPosCalculator::s_borderWidth = qRound(width * qApp->devicePixelRatio());
    }
}

void SAFramelessHelper::setTitleHeight(int height)
{
    if (height > 0) {
        SAPrivateFramelessCursorPosCalculator::s_titleHeight = qRound(height * qApp->devicePixelRatio());
    }
}

bool SAFramelessHelper::widgetMovable()
{
    return (d_ptr->m_bWidgetMovable);
}

bool SAFramelessHelper::widgetResizable()
{
    return (d_ptr->m_bWidgetResizable);
}

bool SAFramelessHelper::rubberBandOnMove()
{
    return (d_ptr->m_bRubberBandOnMove);
}

bool SAFramelessHelper::rubberBandOnResisze()
{
    return (d_ptr->m_bRubberBandOnResize);
}

uint SAFramelessHelper::borderWidth()
{
    return (SAPrivateFramelessCursorPosCalculator::s_borderWidth);
}

uint SAFramelessHelper::titleHeight()
{
    return (SAPrivateFramelessCursorPosCalculator::s_titleHeight);
}
