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
    explicit SARibbonButtonGroupWidget(QWidget* parent = nullptr);
    ~SARibbonButtonGroupWidget();

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

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public:
    bool iterate(FpButtonIterate fp);
Q_SIGNALS:

    /**
     * @brief 参考QToolBar::actionTriggered的信号
     * @param action
     */
    void actionTriggered(QAction* action);

protected:
    virtual void actionEvent(QActionEvent* e) override;
};

#endif  // SARIBBONBUTTONGROUPWIDGET_H
