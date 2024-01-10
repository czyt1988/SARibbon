﻿#ifndef SARIBBONBUTTONGROUPWIDGET_H
#define SARIBBONBUTTONGROUPWIDGET_H
#include "SARibbonGlobal.h"
#include <QToolButton>
#include <QMenu>
#include <QFrame>
class SARibbonControlToolButton;
/**
 * @brief 用于管理一组Action,类似于QToolBar
 */
class SA_RIBBON_EXPORT SARibbonButtonGroupWidget : public QFrame
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonButtonGroupWidget)

public:
    SARibbonButtonGroupWidget(QWidget* parent = Q_NULLPTR);
    ~SARibbonButtonGroupWidget() Q_DECL_OVERRIDE;

    //生成并添加一个action
    QAction* addAction(QAction* a,
                       Qt::ToolButtonStyle buttonStyle = Qt::ToolButtonIconOnly,
                       QToolButton::ToolButtonPopupMode popMode = QToolButton::DelayedPopup);
    QAction* addAction(const QString& text, const QIcon& icon,
                       Qt::ToolButtonStyle buttonStyle = Qt::ToolButtonIconOnly,
                       QToolButton::ToolButtonPopupMode popMode = QToolButton::DelayedPopup);
    QAction* addMenu(QMenu* menu,
                     Qt::ToolButtonStyle buttonStyle = Qt::ToolButtonIconOnly,
                     QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);
    QAction* addSeparator();
    QAction* addWidget(QWidget* w);
    // 从ButtonGroupWidget中把action对应的button提取出来，如果action没有对应的button，就返回nullptr
    SARibbonControlToolButton* actionToRibbonControlToolButton(QAction* action);

    QSize sizeHint() const Q_DECL_OVERRIDE;
    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

    //设置内部控件高度
    void setItemHeight(int h);
    int itemHeight() const;

	//上下margin
	void setItemMargin(int m);
	int itemMargin() const;

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
