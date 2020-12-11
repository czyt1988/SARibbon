#ifndef SAWINDOWBUTTONGROUP_H
#define SAWINDOWBUTTONGROUP_H
#include "SARibbonGlobal.h"
#include <QWidget>
#include <QToolButton>
class SAWindowButtonGroupPrivate;
///
/// \brief 窗口的最大最小化按钮
///
class SA_RIBBON_EXPORT SAWindowButtonGroup : public QWidget
{
    Q_OBJECT
public:
    SAWindowButtonGroup(QWidget* parent,qreal iconscale=0.5);
    ~SAWindowButtonGroup();
    void setupMinimizeButton(bool on,qreal iconscale = 0.5);
    void setupMaximizeButton(bool on,qreal iconscale = 0.5);
    void setupCloseButton(bool on,qreal iconscale = 0.5);
    void updateWindowFlag(qreal iconscale = 0.5);
protected:
    QSize sizeHint() const override;
    virtual bool eventFilter(QObject *watched, QEvent *e) override;
    virtual void parentResize();
private:
    void updateMaximizeButtonIcon();
protected slots:
    Q_SLOT void closeWindow();
    Q_SLOT void minimizeWindow();
    Q_SLOT void maximizeWindow();
private:
    friend class SAWindowButtonGroupPrivate;
    SAWindowButtonGroupPrivate* m_d;
};

class SAWindowToolButton : public QToolButton
{
    Q_OBJECT
public:
    SAWindowToolButton(QWidget* p = nullptr);
};
#endif // SAWINDOWBUTTONGROUP_H
