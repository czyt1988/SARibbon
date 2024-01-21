#ifndef SARIBBONBUTTONGROUPWIDGET_H
#define SARIBBONBUTTONGROUPWIDGET_H
#include "SARibbonGlobal.h"
#include <QToolButton>
#include <QMenu>
#include <QFrame>
class SARibbonControlButton;
/**
 * @brief 用于管理一组Action,类似于QToolBar
 */
class SA_RIBBON_EXPORT SARibbonButtonGroupWidget : public QFrame
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonButtonGroupWidget)
public:
    using FpButtonIterate = std::function< bool(SARibbonControlButton*) >;

public:
    SARibbonButtonGroupWidget(QWidget* parent = Q_NULLPTR);
    ~SARibbonButtonGroupWidget() Q_DECL_OVERRIDE;

    // 图标尺寸
    void setIconSize(const QSize& ic);
    QSize iconSize() const;
    // 生成并添加一个action
    QAction* addAction(QAction* a,
                       Qt::ToolButtonStyle buttonStyle          = Qt::ToolButtonIconOnly,
                       QToolButton::ToolButtonPopupMode popMode = QToolButton::DelayedPopup);
    QAction* addAction(const QString& text,
                       const QIcon& icon,
                       Qt::ToolButtonStyle buttonStyle          = Qt::ToolButtonIconOnly,
                       QToolButton::ToolButtonPopupMode popMode = QToolButton::DelayedPopup);
    QAction* addMenu(QMenu* menu,
                     Qt::ToolButtonStyle buttonStyle          = Qt::ToolButtonIconOnly,
                     QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);
    QAction* addSeparator();
    QAction* addWidget(QWidget* w);
    // 从ButtonGroupWidget中把action对应的button提取出来，如果action没有对应的button，就返回nullptr
    SARibbonControlButton* actionToRibbonControlToolButton(QAction* action);

    QSize sizeHint() const Q_DECL_OVERRIDE;
    QSize minimumSizeHint() const Q_DECL_OVERRIDE;

public:
    bool iterate(FpButtonIterate fp);
signals:

    /**
     * @brief 参考QToolBar::actionTriggered的信号
     * @param action
     */
    void actionTriggered(QAction* action);

protected:
    virtual void actionEvent(QActionEvent* e) Q_DECL_OVERRIDE;
};

#endif  // SARIBBONBUTTONGROUPWIDGET_H
