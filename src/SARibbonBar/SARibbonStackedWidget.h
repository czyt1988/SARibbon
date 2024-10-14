﻿#ifndef SARIBBONSTACKEDWIDGET_H
#define SARIBBONSTACKEDWIDGET_H
#include <QStackedWidget>
#include "SARibbonGlobal.h"
class QHideEvent;
class QResizeEvent;

/**
 * @brief 有qdialog功能的stackwidget，用于在最小化时stack能像dialog那样弹出来
 */
class SA_RIBBON_EXPORT SARibbonStackedWidget : public QStackedWidget
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonStackedWidget)
public:
    SARibbonStackedWidget(QWidget* parent);
    ~SARibbonStackedWidget();
    void setPopupMode();
    bool isPopupMode() const;
    void setNormalMode();
    bool isNormalMode() const;
    void exec();

    // 设置stacked管理的窗口会随着stacked的大小变化而变化大小
    // 就算不激活也调整大小
    void setAutoResize(bool autoresize);
    bool isAutoResize() const;
    // 移动窗口
    void moveWidget(int from, int to);

protected:
    //    void mouseReleaseEvent(QMouseEvent *e);
    void hideEvent(QHideEvent* e) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent* e) Q_DECL_OVERRIDE;
Q_SIGNALS:
    /**
     * @brief hidWindow
     */
    void hidWindow();
};

#endif  // SARIBBONSTACKEDWIDGET_H
