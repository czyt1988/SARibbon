#ifndef SACOLORGRIDWIDGET_H
#define SACOLORGRIDWIDGET_H
#include <QWidget>
#include "SAColorWidgetsGlobal.h"

class SAColorGridWidget : public QWidget
{
    Q_OBJECT
    SA_DECLARE_PRIVATE(SAColorGridWidget)
    Q_PROPERTY(int spacing READ getSpacing WRITE setSpacing)
public:
    SAColorGridWidget(QWidget* par = nullptr);
    ~SAColorGridWidget();

    //设置当前的颜色列表
    void setColorList(const QList< QColor >& c);
    QList< QColor > getColorList() const;
    //设置grid尺寸
    void setGridSize(int row, int col);
    void setGridSize(const QSize& s);
    QSize getGridSize() const;
    //间隔
    int getSpacing() const;
    void setSpacing(int v);
    //图标的尺寸
    void setIconSize(const QSize& s);
    QSize getIconSize() const;
};

#endif  // SACOLORGRIDWIDGET_H
