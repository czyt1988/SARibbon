#ifndef SAFRAMELESSHELPER_H
#define SAFRAMELESSHELPER_H


#include "SARibbonGlobal.h"
#include <QObject>

class QWidget;
class FramelessHelperPrivate;

class SA_RIBBON_EXPORT SAFramelessHelper : public QObject
{
    Q_OBJECT

public:
    explicit SAFramelessHelper(QObject *parent);
    ~SAFramelessHelper();
    // 激活窗体
    void activateOn(QWidget *topLevelWidget);

    // 移除窗体
    void removeFrom(QWidget *topLevelWidget);

    // 设置窗体移动
    void setWidgetMovable(bool movable);

    // 设置窗体缩放
    void setWidgetResizable(bool resizable);

    // 设置橡皮筋移动
    void setRubberBandOnMove(bool movable);

    // 设置橡皮筋缩放
    void setRubberBandOnResize(bool resizable);

    // 设置边框的宽度
    void setBorderWidth(int width);

    // 设置标题栏高度
    void setTitleHeight(int height);
    bool widgetResizable();
    bool widgetMovable();
    bool rubberBandOnMove();
    bool rubberBandOnResisze();
    uint borderWidth();
    uint titleHeight();

protected:
    // 事件过滤，进行移动、缩放等
    virtual bool eventFilter(QObject *obj, QEvent *event);

private:
    FramelessHelperPrivate *d;
};

#endif // FRAMELESSHELPER_H
