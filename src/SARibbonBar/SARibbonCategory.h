#ifndef SARIBBONCATEGORY_H
#define SARIBBONCATEGORY_H
#include "SARibbonGlobal.h"
#include <QWidget>
#include "SARibbonPannel.h"
#include <QScopedPointer>
#include <QPushButton>
#include <QWheelEvent>
class SARibbonCategoryProxyPrivate;
class SARibbonCategoryPrivate;
class QHBoxLayout;
class QWheelEvent;


/**
 * @brief 一项ribbon tab页
 * @note SARibbonCategory的windowTitle影响了其在SARibbonBar的标签显示，
 * 如果要改标签名字，直接调用SARibbonCategory的setWindowTitle函数
 */
class SA_RIBBON_EXPORT SARibbonCategory : public QWidget
{
    Q_OBJECT
    friend class SARibbonBar;
public:
    SARibbonCategory(QWidget *parent);
    ~SARibbonCategory();

    SARibbonPannel::PannelLayoutMode ribbonPannelLayoutMode() const;

    //添加pannel
    SARibbonPannel *addPannel(const QString& title);

    //添加pannel
    void addPannel(SARibbonPannel *pannel);

    //把pannel从Category中移除，不会销毁，此时pannel的所有权归还操作者
    bool takePannel(SARibbonPannel *pannel);

    //移除Pannel，Category会直接回收SARibbonPannel内存
    bool removePannel(SARibbonPannel *pannel);

    //设置背景
    void setBackgroundBrush(const QBrush& brush);

    //返回所有的Pannel
    QList<SARibbonPannel *> pannelList() const;

    //
    QSize sizeHint() const Q_DECL_OVERRIDE;

protected slots:
    void onLeftScrollButtonClicked();
    void onRightScrollButtonClicked();

protected:
    //事件处理
    bool event(QEvent *e) Q_DECL_OVERRIDE;

    //
    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;

    //设置pannel的模式
    void setRibbonPannelLayoutMode(SARibbonPannel::PannelLayoutMode m);
    bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;

    //处理滚轮事件
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;

private:
    SARibbonCategoryPrivate *m_d;
};


/**
 * @brief SARibbonCategory无法完全显示时，显示的调整按钮
 *
 * 重新定义是为了防止被外部的样式影响,同时可以使用SARibbonCategoryScrollButton的样式定义
 */
class SA_RIBBON_EXPORT SARibbonCategoryScrollButton : public QToolButton
{
    Q_OBJECT
public:
    using QToolButton::QToolButton;
    SARibbonCategoryScrollButton(Qt::ArrowType arr, QWidget *p = nullptr);
};

#endif // SARIBBONCATEGORY_H
