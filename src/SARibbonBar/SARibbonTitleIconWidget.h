#ifndef SARIBBONTITLEICONWIDGET_H
#define SARIBBONTITLEICONWIDGET_H
#include "SARibbonGlobal.h"
#include <QWidget>
#include <QMenu>
#include <QIcon>
#include <QPointer>
/**
 * @brief Ribbon风格的标题栏图标控件
 * 模拟Windows原生窗口左上角的程序图标功能，支持右键菜单和窗口控制操作
 */
class SA_RIBBON_EXPORT SARibbonTitleIconWidget : public QWidget
{
    Q_OBJECT
public:
    // 构造函数
    explicit SARibbonTitleIconWidget(QWidget* parent = nullptr);

    // 设置关联的窗口
    void setWindow(QWidget* window);

    // 设置图标
    void setIcon(const QIcon& icon);

    // 设置图标大小
    void setIconSize(const QSize& size);
    int padding() const;
    void setPadding(int v);

public:
    virtual QSize sizeHint() const;

protected:
    // 绘制事件处理
    void paintEvent(QPaintEvent* event) override;

    // 鼠标按下事件处理
    void mousePressEvent(QMouseEvent* event) override;

    // 右键菜单事件处理
    void contextMenuEvent(QContextMenuEvent* event) override;

private:
    // 创建上下文菜单
    void createContextMenu();

    // 设置菜单动作
    void setupMenuActions();

private Q_SLOTS:
    // 还原窗口
    void onRestore();

    // 移动窗口
    void onMove();

    // 调整窗口大小
    void onSize();

    // 最小化窗口
    void onMinimize();

    // 最大化窗口
    void onMaximize();

    // 关闭窗口
    void onClose();

private:
    QPointer< QWidget > m_window;  ///< 关联的窗口对象
    QIcon m_icon;                  ///< 显示的图标
    QSize m_iconSize;              ///< 图标大小
    QMenu* m_contextMenu;          ///< 上下文菜单
    int mPadding;
};

#endif  // SARIBBONTITLEICONWIDGET_H
