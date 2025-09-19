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

/**
 * @brief SARibbonTitleIconWidget::SARibbonTitleIconWidget
 * 构造函数，初始化标题栏图标控件
 * @param parent 父窗口部件
 */
SARibbonTitleIconWidget::SARibbonTitleIconWidget(QWidget* parent)
    : QWidget(parent), m_window(nullptr), m_iconSize(16, 16)
{
    setCursor(Qt::ArrowCursor);
    createContextMenu();
}

/**
 * @brief SARibbonTitleIconWidget::setWindow
 * 设置关联的窗口对象，使用QPointer确保安全性
 * @param window 要关联的窗口
 */
void SARibbonTitleIconWidget::setWindow(QWidget* window)
{
    m_window = window;
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
        QRect iconRect = QRect((width() - iconSize.width()) / 2,
                               (height() - iconSize.height()) / 2,
                               iconSize.width(),
                               iconSize.height());
        m_icon.paint(&painter, iconRect);
    }
}

void SARibbonTitleIconWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_window) {
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
    if (m_contextMenu && m_window) {
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
    QAction* restoreAction = new QAction(tr("还原(R)"), this);
    restoreAction->setShortcut(QKeySequence("Alt+R"));
    restoreAction->setIcon(style()->standardIcon(QStyle::SP_TitleBarNormalButton));
    connect(restoreAction, &QAction::triggered, this, &SARibbonTitleIconWidget::onRestore);

    // 移动菜单项
    QAction* moveAction = new QAction(tr("移动(M)"), this);
    moveAction->setShortcut(QKeySequence("Alt+M"));
    connect(moveAction, &QAction::triggered, this, &SARibbonTitleIconWidget::onMove);

    // 大小菜单项
    QAction* sizeAction = new QAction(tr("大小(S)"), this);
    sizeAction->setShortcut(QKeySequence("Alt+S"));
    connect(sizeAction, &QAction::triggered, this, &SARibbonTitleIconWidget::onSize);

    // 分隔线
    m_contextMenu->addSeparator();

    // 最小化菜单项
    QAction* minimizeAction = new QAction(tr("最小化(N)"), this);
    minimizeAction->setShortcut(QKeySequence("Alt+N"));
    minimizeAction->setIcon(style()->standardIcon(QStyle::SP_TitleBarMinButton));
    connect(minimizeAction, &QAction::triggered, this, &SARibbonTitleIconWidget::onMinimize);

    // 最大化菜单项
    QAction* maximizeAction = new QAction(tr("最大化(X)"), this);
    maximizeAction->setShortcut(QKeySequence("Alt+X"));
    maximizeAction->setIcon(style()->standardIcon(QStyle::SP_TitleBarMaxButton));
    connect(maximizeAction, &QAction::triggered, this, &SARibbonTitleIconWidget::onMaximize);

    // 分隔线
    m_contextMenu->addSeparator();

    // 关闭菜单项
    QAction* closeAction = new QAction(tr("关闭(C)"), this);
    closeAction->setShortcut(QKeySequence("Alt+F4"));
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
    if (m_window) {
        m_window->showNormal();
    }
}

/**
 * @brief SARibbonTitleIconWidget::onMove
 * 启动窗口移动模式
 */
void SARibbonTitleIconWidget::onMove()
{
    if (m_window) {
        // 在实际应用中，这里可以触发窗口移动逻辑
        // 或者通过其他方式实现移动功能
        m_window->setFocus();
    }
}

/**
 * @brief SARibbonTitleIconWidget::onSize
 * 启动窗口大小调整模式
 */
void SARibbonTitleIconWidget::onSize()
{
    if (m_window) {
        // 在实际应用中，这里可以触发窗口大小调整逻辑
        // 或者通过其他方式实现大小调整功能
        m_window->setFocus();
    }
}

/**
 * @brief SARibbonTitleIconWidget::onMinimize
 * 最小化窗口
 */
void SARibbonTitleIconWidget::onMinimize()
{
    if (m_window) {
        m_window->showMinimized();
    }
}

/**
 * @brief SARibbonTitleIconWidget::onMaximize
 * 切换窗口最大化状态
 */
void SARibbonTitleIconWidget::onMaximize()
{
    if (m_window) {
        if (m_window->isMaximized()) {
            m_window->showNormal();
        } else {
            m_window->showMaximized();
        }
    }
}

/**
 * @brief SARibbonTitleIconWidget::onClose
 * 关闭窗口
 */
void SARibbonTitleIconWidget::onClose()
{
    if (m_window) {
        m_window->close();
    }
}
