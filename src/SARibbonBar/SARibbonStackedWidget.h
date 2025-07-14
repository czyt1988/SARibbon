#ifndef SARIBBONSTACKEDWIDGET_H
#define SARIBBONSTACKEDWIDGET_H
#include <QStackedWidget>
#include "SARibbonGlobal.h"
class QHideEvent;
class QResizeEvent;

/**
 * @class SARibbonStackedWidget
 * @brief 具有弹出式菜单功能的堆叠窗口部件
 *
 * 此类扩展了 QStackedWidget，添加了两种工作模式：
 * 1. 正常模式：作为常规的堆叠部件，嵌入在父窗口布局中
 * 2. 弹出模式：作为独立的弹出窗口，可用于实现类似菜单的显示效果
 *
 * 在弹出模式下，窗口将显示为无边框面板，并支持模态事件循环（exec()方法），
 * 这使得它可以像菜单或对话框一样使用。
 *
 * @note 切换模式时会自动处理窗口状态和事件循环
 * @sa QStackedWidget
 */
class SA_RIBBON_EXPORT SARibbonStackedWidget : public QStackedWidget
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonStackedWidget)
public:
    explicit SARibbonStackedWidget(QWidget* parent);
    ~SARibbonStackedWidget();
    // 设置弹出模式
    void setPopupMode();

    // 检查当前是否处于弹出模式
    bool isPopupMode() const;

    // 设置正常模式 和普通的stackwidget一样
    void setNormalMode();

    // 检查当前是否处于正常模式
    bool isNormalMode() const;

    // 在弹出模式下以模态方式运行事件循环
    void exec();

    // 类似tabbar的moveTab函数，交换两个窗口的index
    void moveWidget(int from, int to);

protected:
    //    void mouseReleaseEvent(QMouseEvent *e);
    void hideEvent(QHideEvent* e) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent* e) Q_DECL_OVERRIDE;
Q_SIGNALS:
    /**
     * @brief 隐藏窗口信号
     */
    void hidWindow();
};

#endif  // SARIBBONSTACKEDWIDGET_H
