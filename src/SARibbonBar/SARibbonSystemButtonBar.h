#ifndef SARIBBONSYSTEMBUTTONBAR_H
#define SARIBBONSYSTEMBUTTONBAR_H
#include "SARibbonGlobal.h"
#include <QFrame>
#include <QToolButton>

/**
 * @brief 窗口的最大最小化按钮
 * @note 内部有个SARibbonButtonGroupWidget，其ObjectName = SASystemButtonGroup
 * 如果需要qss，可以进行特化处理:
 *
 * @code
 * SARibbonButtonGroupWidget#SASystemButtonGroup
 * @endcode
 *
 * SARibbonSystemButtonBar的eventfilter捕获mainwindow的事件,通过eventerfilter来处理mainwindow的事件，避免用户错误的继承resizeEvent导致systembar的位置异常
 *
 * 因此，主窗口需要安装事件过滤器
 *
 * @code
 * MainWindow::MainWindow(){
 *    installEventFilter(mWindowButtonGroup);
 * }
 * @endcode
 */
class SA_RIBBON_EXPORT SARibbonSystemButtonBar : public QFrame
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonSystemButtonBar)
public:
    explicit SARibbonSystemButtonBar(QWidget* parent);
    explicit SARibbonSystemButtonBar(QWidget* parent, Qt::WindowFlags flags);
    ~SARibbonSystemButtonBar();
    void setupMinimizeButton(bool on);
    void setupMaximizeButton(bool on);
    void setupCloseButton(bool on);
    // 以最顶层的widget的WindowFlag作为WindowFlag
    void updateWindowFlag();
    void updateWindowFlag(Qt::WindowFlags flags);
    // 设置按钮的宽度比例,最终按钮宽度将按照此比例进行设置
    void setButtonWidthStretch(int close = 4, int max = 3, int min = 3);
    // 标题栏高度
    void setWindowTitleHeight(int h);
    int windowTitleHeight() const;
    // 设置标准系统按钮的宽度
    void setWindowButtonWidth(int w);
    int windowButtonWidth() const;
    // 设置Qt::WindowStates
    void setWindowStates(Qt::WindowStates s);
    // 仅获取按钮的状态
    Qt::WindowFlags windowButtonFlags() const;
    // 三个标准系统窗口按钮
    QAbstractButton* minimizeButton() const;
    QAbstractButton* maximizeButton() const;
    QAbstractButton* closeButton() const;

    // 图标尺寸
    void setIconSize(const QSize& ic);
    QSize iconSize() const;
    // 生成并添加一个action
    void addAction(QAction* a);
    void addMenuAction(QAction* menuAction, QToolButton::ToolButtonPopupMode popupMode = QToolButton::InstantPopup);
    QAction* addMenuAction(QMenu* menu, QToolButton::ToolButtonPopupMode popupMode = QToolButton::InstantPopup);
    QAction* addSeparator();
    QAction* addWidget(QWidget* w);

    virtual QSize sizeHint() const Q_DECL_OVERRIDE;
    // 捕获mainwindow的事件
    bool eventFilter(QObject* obj, QEvent* event) override;

protected:
    virtual void resizeEvent(QResizeEvent* e) Q_DECL_OVERRIDE;

protected Q_SLOTS:
    Q_SLOT void closeWindow();
    Q_SLOT void minimizeWindow();
    Q_SLOT void maximizeWindow();
};

/**
 * @brief The SARibbonSystemToolButton class/系统最大最小化按钮
 */
class SARibbonSystemToolButton : public QToolButton
{
    Q_OBJECT
public:
    SARibbonSystemToolButton(QWidget* p = nullptr);
};

#endif  // SARIBBONSYSTEMBUTTONBAR_H
