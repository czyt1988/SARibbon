#ifndef SACOLORPALETTEGRIDWIDGET_H
#define SACOLORPALETTEGRIDWIDGET_H
#include <QWidget>
#include "SAColorWidgetsGlobal.h"
class QMenu;
class SAColorToolButton;
/**
 * @brief 类似office的颜色选择窗口，有一排标准色，下面有一个颜色板，有3行浅色，有2行深色
 */
class SA_COLOR_WIDGETS_API SAColorPaletteGridWidget : public QWidget
{
    Q_OBJECT
    SA_COLOR_WIDGETS_DECLARE_PRIVATE(SAColorPaletteGridWidget)
public:
    SAColorPaletteGridWidget(QWidget* par = nullptr);
    SAColorPaletteGridWidget(const QList< QColor >& cls, QWidget* par = nullptr);
    ~SAColorPaletteGridWidget();
    // 设置窗口维护的colorList
    void setColorList(const QList< QColor >& cls);
    QList< QColor > colorList() const;
    // 设置颜色深浅比例factor，默认为{ 180, 160, 140, 75, 50 }
    void setFactor(const QList< int >& factor);
    QList< int > factor() const;
    // 设置iconsize
    void setColorIconSize(const QSize& s);
    QSize colorIconSize() const;
    // 设置颜色是否是checkable
    void setColorCheckable(bool on = true);
    bool isColorCheckable() const;
private Q_SLOTS:
    void onMainColorClicked(const QColor& c);
    void onPaletteColorClicked(const QColor& c);

private:
    void init();
Q_SIGNALS:
    /**
     * @brief 对于check模式，check的颜色触发的信号
     * @param c
     * @param on
     */
    void colorClicked(const QColor& c);
};

#endif  // SACOLORPALETTEGRIDWIDGET_H
