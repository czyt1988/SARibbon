﻿#ifndef SAWINDOWBUTTONGROUP_H
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
    SAWindowButtonGroup(QWidget *parent);
    SAWindowButtonGroup(QWidget *parent, Qt::WindowFlags flags);
    ~SAWindowButtonGroup();
    void setupMinimizeButton(bool on);
    void setupMaximizeButton(bool on);
    void setupCloseButton(bool on);
    void updateWindowFlag();

    //设置按钮的宽度比例,最终按钮宽度将按照此比例进行设置
    void setButtonWidthStretch(int close = 4, int max = 3, int min = 3);

    //设置按钮的缩放比例
    void setIconScale(qreal iconscale = 0.5);

    //设置Qt::WindowStates
    void setWindowStates(Qt::WindowStates s);

protected:
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;
    virtual bool eventFilter(QObject *watched, QEvent *e) Q_DECL_OVERRIDE;
    virtual void parentResize();
    virtual void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;

private:
    //void updateMaximizeButtonIcon();

protected slots:
    Q_SLOT void closeWindow();
    Q_SLOT void minimizeWindow();
    Q_SLOT void maximizeWindow();

private:
    friend class SAWindowButtonGroupPrivate;
    SAWindowButtonGroupPrivate *m_d;
};

class SAWindowToolButton : public QToolButton
{
    Q_OBJECT
public:
    SAWindowToolButton(QWidget *p = nullptr);
};
#endif // SAWINDOWBUTTONGROUP_H
