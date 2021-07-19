#ifndef SARIBBONSEPARATORWIDGET_H
#define SARIBBONSEPARATORWIDGET_H
#include "SARibbonGlobal.h"
#include <QWidget>
#include <QStyleOption>

///
/// \brief 用于显示分割线
///
class SA_RIBBON_EXPORT SARibbonSeparatorWidget : public QWidget
{
    Q_OBJECT
public:
    SARibbonSeparatorWidget(int height, QWidget *parent = nullptr);
    SARibbonSeparatorWidget(QWidget *parent = nullptr);
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;

    //设置分割线的上下距离
    void setTopBottomMargins(int top, int bottom);

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    int m_topMargins;       ///< 顶部间隔,默认4
    int m_bottomMargins;    ///< 底部间隔,默认4
};

#endif // SARIBBONSEPARATORWIDGET_H
