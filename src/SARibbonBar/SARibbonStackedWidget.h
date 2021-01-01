#ifndef SARIBBONSTACKEDWIDGET_H
#define SARIBBONSTACKEDWIDGET_H
#include <QStackedWidget>
#include "SARibbonGlobal.h"
class SARibbonStackedWidgetPrivate;

/**
 * @brief 有qdialog功能的stackwidget，用于在最小化时stack能像dialog那样弹出来
 */
class SA_RIBBON_EXPORT SARibbonStackedWidget : public QStackedWidget
{
    Q_OBJECT
public:
    SARibbonStackedWidget(QWidget *parent);
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
    SARibbonStackedWidgetPrivate *m_d;
};

#endif // SARIBBONSTACKEDWIDGET_H
