// SARibbonTitleIconWidget.cpp
#include "SARibbonTitleIconWidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QApplication>
#include <QStyle>
#include <QScreen>
#include <QWindow>
#include <QAction>
#include <QDebug>
/**
 * @brief SARibbonTitleIconWidget::SARibbonTitleIconWidget
 * 构造函数，初始化标题栏图标控件
 * @param parent 父窗口部件
 */
SARibbonTitleIconWidget::SARibbonTitleIconWidget(QWidget* parent)
    : QWidget(parent), m_widget(nullptr), m_iconSize(16, 16), mPadding(1)
{
    setCursor(Qt::ArrowCursor);
    createContextMenu();
}

/**
 * @brief SARibbonTitleIconWidget::setWidget
 * 设置关联的窗口对象，使用QPointer确保安全性
 * @param window 要关联的窗口
 */
void SARibbonTitleIconWidget::setWidget(QWidget* window)
{
    m_widget = window;
}

/**
 * @brief 关联的窗口对象
 * @return
 */
QWidget* SARibbonTitleIconWidget::widget() const
{
    return m_widget.data();
}

/**
 * @brief SARibbonTitleIconWidget::setIcon
 * 设置显示的图标
 * @param icon 要显示的图标
 */
void SARibbonTitleIconWidget::setIcon(const QIcon& icon)
{
    m_icon = icon;
    update();
}

/**
 * @brief 图标
 * @return
 */
QIcon SARibbonTitleIconWidget::icon() const
{
    return m_icon;
}

/**
 * @brief SARibbonTitleIconWidget::setIconSize
 * 设置图标大小
 * @param size 图标尺寸
 */
void SARibbonTitleIconWidget::setIconSize(const QSize& size)
{
    m_iconSize = size;
    update();
}

/**
 * @brief 图标大小
 * @return
 */
QSize SARibbonTitleIconWidget::iconSize() const
{
    return m_iconSize;
}

/**
 * @brief 内边距
 * @return
 */
int SARibbonTitleIconWidget::padding() const
{
    return mPadding;
}

/**
 * @brief 设置内边距
 * @param v
 */
void SARibbonTitleIconWidget::setPadding(int v)
{
    mPadding = v;
}

QSize SARibbonTitleIconWidget::sizeHint() const
{
    return QSize(m_iconSize.width() + mPadding, m_iconSize.height() + mPadding);
}

void SARibbonTitleIconWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    // 绘制图标
    if (!m_icon.isNull()) {
        QSize iconSize = m_iconSize.scaled(rect().size(), Qt::KeepAspectRatio);
        iconSize.setWidth(iconSize.width() - 2 * padding());
        iconSize.setHeight(iconSize.height() - 2 * padding());
        iconSize       = iconSize.expandedTo(QSize(8, 8));
        QRect iconRect = QRect(
            (width() - iconSize.width()) / 2, (height() - iconSize.height()) / 2, iconSize.width(), iconSize.height());
        m_icon.paint(&painter, iconRect);
    }
}

void SARibbonTitleIconWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_widget) {
        if (m_contextMenu) {
            QPoint menuPos = mapToGlobal(QPoint(0, height()));
            m_contextMenu->popup(menuPos);
            event->accept();
            return;
        }
    }
    QWidget::mousePressEvent(event);
}

void SARibbonTitleIconWidget::contextMenuEvent(QContextMenuEvent* event)
{
    if (m_contextMenu && m_widget) {
        QPoint menuPos = mapToGlobal(event->pos());
        m_contextMenu->popup(menuPos);
        event->accept();
        return;
    }
    QWidget::contextMenuEvent(event);
}

/**
 * @brief 创建上下文菜单，包含标准的窗口控制选项
 */
void SARibbonTitleIconWidget::createContextMenu()
{
    m_contextMenu = new QMenu(this);
    setupMenuActions();
}

/**
 * @brief 设置菜单动作，包括还原、移动、大小、最小化、最大化和关闭操作
 */
void SARibbonTitleIconWidget::setupMenuActions()
{
    // 还原菜单项
    QAction* restoreAction = new QAction(tr("Restore(R)"), this);  // cn:还原
    restoreAction->setIcon(style()->standardIcon(QStyle::SP_TitleBarNormalButton));
    connect(restoreAction, &QAction::triggered, this, &SARibbonTitleIconWidget::onRestore);

    // 移动菜单项
    QAction* moveAction = new QAction(tr("Move(M)"), this);  // cn:移动
    connect(moveAction, &QAction::triggered, this, &SARibbonTitleIconWidget::onMove);

    // 大小菜单项
    QAction* sizeAction = new QAction(tr("Size(S)"), this);  // cn:大小
    connect(sizeAction, &QAction::triggered, this, &SARibbonTitleIconWidget::onSize);

    // 分隔线
    m_contextMenu->addSeparator();

    // 最小化菜单项
    QAction* minimizeAction = new QAction(tr("Minimize(N)"), this);  // cn:最小化
    minimizeAction->setIcon(style()->standardIcon(QStyle::SP_TitleBarMinButton));
    connect(minimizeAction, &QAction::triggered, this, &SARibbonTitleIconWidget::onMinimize);

    // 最大化菜单项
    QAction* maximizeAction = new QAction(tr("Maximize(X)"), this);  // cn:最小化
    maximizeAction->setIcon(style()->standardIcon(QStyle::SP_TitleBarMaxButton));
    connect(maximizeAction, &QAction::triggered, this, &SARibbonTitleIconWidget::onMaximize);

    // 分隔线
    m_contextMenu->addSeparator();

    // 关闭菜单项
    QAction* closeAction = new QAction(tr("Close(C)"), this);  // cn:关闭
    closeAction->setIcon(style()->standardIcon(QStyle::SP_TitleBarCloseButton));
    connect(closeAction, &QAction::triggered, this, &SARibbonTitleIconWidget::onClose);

    // 添加到菜单
    m_contextMenu->addAction(restoreAction);
    m_contextMenu->addAction(moveAction);
    m_contextMenu->addAction(sizeAction);
    m_contextMenu->addAction(minimizeAction);
    m_contextMenu->addAction(maximizeAction);
    m_contextMenu->addAction(closeAction);
}

/**
 * @brief SARibbonTitleIconWidget::onRestore
 * 还原窗口到正常大小
 */
void SARibbonTitleIconWidget::onRestore()
{
    if (m_widget) {
        m_widget->showNormal();
    }
}

/**
 * @brief SARibbonTitleIconWidget::onMove
 * 启动窗口移动模式
 */
void SARibbonTitleIconWidget::onMove()
{
    if (m_widget) {
        // 在实际应用中，这里可以触发窗口移动逻辑
        // 或者通过其他方式实现移动功能
        m_widget->setFocus();
    }
}

/**
 * @brief SARibbonTitleIconWidget::onSize
 * 启动窗口大小调整模式
 */
void SARibbonTitleIconWidget::onSize()
{
    if (m_widget) {
        // 在实际应用中，这里可以触发窗口大小调整逻辑
        // 或者通过其他方式实现大小调整功能
        m_widget->setFocus();
    }
}

/**
 * @brief SARibbonTitleIconWidget::onMinimize
 * 最小化窗口
 */
void SARibbonTitleIconWidget::onMinimize()
{
    if (m_widget) {
        m_widget->showMinimized();
    }
}

/**
 * @brief SARibbonTitleIconWidget::onMaximize
 * 切换窗口最大化状态
 */
void SARibbonTitleIconWidget::onMaximize()
{
    if (m_widget) {
        if (m_widget->isMaximized()) {
            m_widget->showNormal();
        } else {
            m_widget->showMaximized();
        }
    }
}

/**
 * @brief SARibbonTitleIconWidget::onClose
 * 关闭窗口
 */
void SARibbonTitleIconWidget::onClose()
{
    if (m_widget) {
        m_widget->close();
    }
}
