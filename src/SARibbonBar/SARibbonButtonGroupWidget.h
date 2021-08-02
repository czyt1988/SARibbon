#ifndef SARIBBONBUTTONGROUPWIDGET_H
#define SARIBBONBUTTONGROUPWIDGET_H
#include "SARibbonGlobal.h"
#include <QToolButton>
#include <QMenu>
#include <QFrame>
class SARibbonButtonGroupWidgetPrivate;


/**
 * @brief 用于管理一组Action,类似于QToolBar
 */
class SA_RIBBON_EXPORT SARibbonButtonGroupWidget : public QFrame
{
    Q_OBJECT
public:
    SARibbonButtonGroupWidget(QWidget *parent = Q_NULLPTR);
    ~SARibbonButtonGroupWidget() Q_DECL_OVERRIDE;

    //生成并添加一个action
    QAction *addAction(QAction *a);
    QAction *addAction(const QString& text, const QIcon& icon, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);
    QAction *addMenu(QMenu *menu, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);
    QAction *addSeparator();
    QAction *addWidget(QWidget *w);
    QSize sizeHint() const Q_DECL_OVERRIDE;
    QSize minimumSizeHint() const Q_DECL_OVERRIDE;

signals:

    /**
     * @brief 参考QToolBar::actionTriggered的信号
     * @param action
     */
    void actionTriggered(QAction *action);

protected:
    virtual void actionEvent(QActionEvent *e) Q_DECL_OVERRIDE;

private:
    SARibbonButtonGroupWidgetPrivate *m_d;
};

#endif // SARIBBONBUTTONGROUPWIDGET_H
