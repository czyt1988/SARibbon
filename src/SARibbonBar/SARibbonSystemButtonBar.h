#ifndef SARIBBONSYSTEMBUTTONBAR_H
#define SARIBBONSYSTEMBUTTONBAR_H
#include "SARibbonGlobal.h"
#include <QFrame>
#include <QToolButton>

/**
 * \if ENGLISH
 * @brief Window maximize/minimize buttons
 * @note There is a SARibbonButtonGroupWidget inside, its ObjectName = SASystemButtonGroup
 * @details If you need qss, you can specialize:
 * @details @code
 * @details SARibbonButtonGroupWidget#SASystemButtonGroup
 * @details @endcode
 * @details SARibbonSystemButtonBar's eventfilter captures mainwindow events, handles mainwindow events through eventfilter to avoid abnormal systembar position caused by user incorrectly inheriting resizeEvent
 * @details Therefore, the main window needs to install event filter
 * @details @code
 * @details MainWindow::MainWindow(){
 * @details    installEventFilter(mWindowButtonGroup);
 * @details }
 * @details @endcode
 * \endif
 *
 * \if CHINESE
 * @brief 窗口的最大最小化按钮
 * @note 内部有个SARibbonButtonGroupWidget，其ObjectName = SASystemButtonGroup
 * @details 如果需要qss，可以进行特化处理:
 * @details @code
 * @details SARibbonButtonGroupWidget#SASystemButtonGroup
 * @details @endcode
 * @details SARibbonSystemButtonBar的eventfilter捕获mainwindow的事件,通过eventerfilter来处理mainwindow的事件，避免用户错误的继承resizeEvent导致systembar的位置异常
 * @details 因此，主窗口需要安装事件过滤器
 * @details @code
 * @details MainWindow::MainWindow(){
 * @details    installEventFilter(mWindowButtonGroup);
 * @details }
 * @details @endcode
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonSystemButtonBar : public QFrame
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonSystemButtonBar)
public:
    /// Constructor for SARibbonSystemButtonBar
    explicit SARibbonSystemButtonBar(QWidget* parent);
    /// Constructor with window flags
    explicit SARibbonSystemButtonBar(QWidget* parent, Qt::WindowFlags flags);
    /// Destructor for SARibbonSystemButtonBar
    ~SARibbonSystemButtonBar();
    /// Setup minimize button
    void setupMinimizeButton(bool on);
    /// Setup maximize button
    void setupMaximizeButton(bool on);
    /// Setup close button
    void setupCloseButton(bool on);
    /// Use the WindowFlag of the top-level widget as WindowFlag
    void updateWindowFlag();
    /// Update window flag with specified flags
    void updateWindowFlag(Qt::WindowFlags flags);
    /// Set button width ratio, final button width will be set according to this ratio
    void setButtonWidthStretch(int close = 4, int max = 3, int min = 3);
    /// Set title bar height
    void setWindowTitleHeight(int h);
    /// Get title bar height
    int windowTitleHeight() const;
    /// Set standard system button width
    void setWindowButtonWidth(int w);
    /// Get standard system button width
    int windowButtonWidth() const;
    /// Set Qt::WindowStates
    void setWindowStates(Qt::WindowStates s);
    /// Get button states only
    Qt::WindowFlags windowButtonFlags() const;
    /// Get minimize button
    QAbstractButton* minimizeButton() const;
    /// Get maximize button
    QAbstractButton* maximizeButton() const;
    /// Get close button
    QAbstractButton* closeButton() const;

    /// Set icon size
    void setIconSize(const QSize& ic);
    /// Get icon size
    QSize iconSize() const;
    /// Generate and add an action
    void addAction(QAction* a);
    /// Add menu action
    void addMenuAction(QAction* menuAction, QToolButton::ToolButtonPopupMode popupMode = QToolButton::InstantPopup);
    /// Add menu action and return the action
    QAction* addMenuAction(QMenu* menu, QToolButton::ToolButtonPopupMode popupMode = QToolButton::InstantPopup);
    /// Add separator
    QAction* addSeparator();
    /// Add widget
    QAction* addWidget(QWidget* w);

    /// Get size hint
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;
    /// Capture mainwindow events
    bool eventFilter(QObject* obj, QEvent* event) override;

protected:
    /// Handle resize event
    virtual void resizeEvent(QResizeEvent* e) Q_DECL_OVERRIDE;

protected Q_SLOTS:
    /// Close window slot
    Q_SLOT void closeWindow();
    /// Minimize window slot
    Q_SLOT void minimizeWindow();
    /// Maximize window slot
    Q_SLOT void maximizeWindow();
};

/**
 * \if ENGLISH
 * @brief System maximize/minimize button
 * \endif
 *
 * \if CHINESE
 * @brief 系统最大最小化按钮
 * \endif
 */
class SARibbonSystemToolButton : public QToolButton
{
    Q_OBJECT
public:
    /// Constructor for SARibbonSystemToolButton
    SARibbonSystemToolButton(QWidget* p = nullptr);
};

#endif  // SARIBBONSYSTEMBUTTONBAR_H
