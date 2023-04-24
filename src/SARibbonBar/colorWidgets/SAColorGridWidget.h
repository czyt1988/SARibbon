#ifndef SACOLORGRIDWIDGET_H
#define SACOLORGRIDWIDGET_H
#include <QWidget>
#include "SAColorWidgetsGlobal.h"

/**
 * @brief 一个grid的Color布局
 */
class SAColorGridWidget : public QWidget
{
    Q_OBJECT
    SA_DECLARE_PRIVATE(SAColorGridWidget)
    Q_PROPERTY(int spacing READ getSpacing WRITE setSpacing)
public:
    SAColorGridWidget(QWidget* par = nullptr);
    ~SAColorGridWidget();
    //设置列数，行数量会根据列数量来匹配,如果设置-1或者0，说明不限定列数量，这样会只有一行
    void setColumnCount(int c);
    int getColumnCount() const;
    //设置当前的颜色列表
    void setColorList(const QList< QColor >& cls);
    QList< QColor > getColorList() const;
    //间隔
    int getSpacing() const;
    void setSpacing(int v);
    //获取颜色的数量
    int getColorCount() const;
    //图标的尺寸
    void setColorIconSize(const QSize& s);
    QSize getColorIconSize() const;
    //
};

#endif  // SACOLORGRIDWIDGET_H
