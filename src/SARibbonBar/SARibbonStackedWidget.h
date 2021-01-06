#ifndef SARIBBONSTACKEDWIDGET_H
#define SARIBBONSTACKEDWIDGET_H
#include <QStackedWidget>
#include "SARibbonGlobal.h"
class QHideEvent;
class QResizeEvent;
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

    //设置stacked管理的窗口会随着stacked的大小变化而变化大小
    void setAutoResize(bool autoresize);
    bool isAutoResize() const;

protected:
//    void mouseReleaseEvent(QMouseEvent *e);
    void hideEvent(QHideEvent *e) override;
    void resizeEvent(QResizeEvent *event) override;

signals:
    void hidWindow();

private:
    SARibbonStackedWidgetPrivate *m_d;
};

#endif // SARIBBONSTACKEDWIDGET_H
