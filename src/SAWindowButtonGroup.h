#ifndef SAWINDOWBUTTONGROUP_H
#define SAWINDOWBUTTONGROUP_H
#include "SARibbonGlobal.h"
#include <QWidget>
class SAWindowButtonGroupPrivate;
///
/// \brief 窗口的最大最小化按钮
///
class SA_RIBBON_EXPORT SAWindowButtonGroup : public QWidget
{
    Q_OBJECT
public:
    SAWindowButtonGroup(QWidget* parent);
    ~SAWindowButtonGroup();
    void setupMinimizeButton(bool on);
    void setupMaximizeButton(bool on);
    void setupCloseButton(bool on);
    void updateWindowFlag();
protected:
    QSize sizeHint();
    virtual bool eventFilter(QObject *watched, QEvent *e);
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

#endif // SAWINDOWBUTTONGROUP_H
