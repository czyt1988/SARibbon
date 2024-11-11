#ifndef SARIBBONSTACKEDWIDGET_H
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
    explicit SARibbonStackedWidget(QWidget* parent);
    ~SARibbonStackedWidget();
    void setPopupMode();
    bool isPopupMode() const;
    void setNormalMode();
    bool isNormalMode() const;
    void exec();

    /**
     * @brief 类似tabbar的moveTab函数，交换两个窗口的index
     * @param from
     * @param to
     * @note 此操作会触发widgetRemoved(int index)信号
     */
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
