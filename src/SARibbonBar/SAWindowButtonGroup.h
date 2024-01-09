#ifndef SAWINDOWBUTTONGROUP_H
#define SAWINDOWBUTTONGROUP_H
#include "SARibbonGlobal.h"
#include <QWidget>
#include <QPushButton>

///
/// \brief 窗口的最大最小化按钮
///
class SA_RIBBON_EXPORT SAWindowButtonGroup : public QWidget
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SAWindowButtonGroup)
public:
    SAWindowButtonGroup(QWidget* parent);
    SAWindowButtonGroup(QWidget* parent, Qt::WindowFlags flags);
    ~SAWindowButtonGroup();
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
    // 设置Qt::WindowStates
    void setWindowStates(Qt::WindowStates s);
    // 仅获取按钮的状态
    Qt::WindowFlags windowButtonFlags() const;

    virtual QSize sizeHint() const Q_DECL_OVERRIDE;

    QAbstractButton* minimizeButton() const;
    QAbstractButton* maximizeButton() const;
    QAbstractButton* closeButton() const;

protected:
    virtual void resizeEvent(QResizeEvent* e) Q_DECL_OVERRIDE;

protected slots:
    Q_SLOT void closeWindow();
    Q_SLOT void minimizeWindow();
    Q_SLOT void maximizeWindow();
};

/**
 * @brief The SAWindowToolButton class
 */
class SAWindowToolButton : public QPushButton
{
    Q_OBJECT
public:
    SAWindowToolButton(QWidget* p = nullptr);
};
#endif  // SAWINDOWBUTTONGROUP_H
