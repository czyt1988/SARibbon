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

    // 关联的窗口
    void setWidget(QWidget* window);
    QWidget* widget() const;

    // 图标
    void setIcon(const QIcon& icon);
    QIcon icon() const;

    // 图标大小
    void setIconSize(const QSize& size);
    QSize iconSize() const;

    // 内边距
    void setPadding(int v);
    int padding() const;

    // Enable or disable click handling on the title icon (default enabled)
    void setClickEnabled(bool on = true);
    // Check whether click handling on the title icon is enabled
    bool isClickEnabled() const;

    // Enable or disable the built-in window control menu (default enabled)
    void setMenuEnabled(bool on = true);
    // Check whether the built-in window control menu is enabled
    bool isMenuEnabled() const;

Q_SIGNALS:
    /**
     * \if ENGLISH
     * @brief Emitted when the title icon is clicked while the built-in menu is disabled
     * @details Connect to this signal after calling setMenuEnabled(false) to provide a custom
     * click behavior (for example, showing your own menu).
     * @param button The mouse button that triggered the click, as a Qt::MouseButton value
     * @param globalPos The click position in global coordinates
     * \endif
     *
     * \if CHINESE
     * @brief 内置菜单禁用时，标题图标被点击会发射此信号
     * @details 调用 setMenuEnabled(false) 后连接此信号，即可自定义点击行为（例如弹出自己的菜单）。
     * @param button 触发点击的鼠标按键，取值为 Qt::MouseButton
     * @param globalPos 点击位置（全局坐标）
     * \endif
     */
    void titleIconClicked(int button, const QPoint& globalPos);

public:
    virtual QSize sizeHint() const override;

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

    // 处理图标点击，决定弹出内置菜单还是发射信号
    void handleClick(Qt::MouseButton button, const QPoint& globalPos);

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
    QPointer< QWidget > m_widget;  ///< 关联的窗口对象
    QIcon m_icon;                  ///< 显示的图标
    QSize m_iconSize;              ///< 图标大小
    QMenu* m_contextMenu;          ///< 上下文菜单
    bool m_clickEnabled;           ///< 是否处理点击
    bool m_menuEnabled;            ///< 是否弹出内置窗口菜单
    int mPadding;
};

#endif  // SARIBBONTITLEICONWIDGET_H
