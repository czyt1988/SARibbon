#ifndef SACOLORPALETTEGRIDWIDGET_H
#define SACOLORPALETTEGRIDWIDGET_H
#include <QWidget>
#include "SAColorWidgetsGlobal.h"
/**
 * @brief 类似office的颜色选择窗口，有一排标准色，下面有一个颜色板，有3行浅色，有2行深色
 */
class SAColorPaletteGridWidget : public QWidget
{
    Q_OBJECT
    SA_DECLARE_PRIVATE(SAColorPaletteGridWidget)
public:
    SAColorPaletteGridWidget(QWidget* par = nullptr);
    ~SAColorPaletteGridWidget();
};

#endif  // SACOLORPALETTEGRIDWIDGET_H
