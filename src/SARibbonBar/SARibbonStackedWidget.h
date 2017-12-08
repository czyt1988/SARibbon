#ifndef SARIBBONSTACKEDWIDGET_H
#define SARIBBONSTACKEDWIDGET_H
#include <QStackedWidget>
#include "SARibbonGlobal.h"
class SARibbonStackedWidgetPrivate;
class SA_RIBBON_EXPORT SARibbonStackedWidget : public QStackedWidget
{
    Q_OBJECT
public:
    SARibbonStackedWidget(QWidget* parent);
    ~SARibbonStackedWidget();
    void setPopupMode();
    bool isPopupMode() const;
    void setNormalMode();
    bool isNormalMode() const;
    void exec();
protected:
//    void mouseReleaseEvent(QMouseEvent *e);
    void hideEvent(QHideEvent *e);
signals:
    void hidWindow();
private:
    SARibbonStackedWidgetPrivate* m_d;
};

#endif // SARIBBONSTACKEDWIDGET_H
