#ifndef SARIBBONAPPLICATIONWIDGET_H
#define SARIBBONAPPLICATIONWIDGET_H
#include "SARibbonGlobal.h"
#include <QFrame>
class QShowEvent;
class QKeyEvent;
class SARibbonMainWindow;
/**
 * @brief 这个窗口是用于applicationButton点击后弹出类似office系列软件的文件窗口，此窗口会把整个MainWindow覆盖，
 * 同时会接管边框的一些消息
 */
class SA_RIBBON_EXPORT SARibbonApplicationWidget : public QFrame
{
    Q_OBJECT
public:
    SARibbonApplicationWidget(SARibbonMainWindow* parent);

protected:
    virtual void resizeToParent(const QSize& parentSize);

protected:
    bool eventFilter(QObject* obj, QEvent* ev) override;
    void showEvent(QShowEvent* event) override;
    void keyPressEvent(QKeyEvent* ev) override;
};

#endif  // SARIBBONAPPLICATIONWIDGET_H
