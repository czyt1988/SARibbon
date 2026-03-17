#ifndef SACOLORPALETTEGRIDWIDGET_H
#define SACOLORPALETTEGRIDWIDGET_H
#include <QWidget>
#include "SAColorWidgetsGlobal.h"
class QMenu;
class SAColorToolButton;
/**
 * \if ENGLISH
 * @brief Office-like color selection widget with standard colors and color palette
 * @details Has a row of standard colors, below is a color palette with 3 rows of light colors and 2 rows of dark colors
 * \endif
 *
 * \if CHINESE
 * @brief 类似office的颜色选择窗口，有一排标准色，下面有一个颜色板，有3行浅色，有2行深色
 * \endif
 */
class SA_COLOR_WIDGETS_API SAColorPaletteGridWidget : public QWidget
{
    Q_OBJECT
    SA_COLOR_WIDGETS_DECLARE_PRIVATE(SAColorPaletteGridWidget)
public:
    /// Constructor for SAColorPaletteGridWidget
    SAColorPaletteGridWidget(QWidget* par = nullptr);
    /// Constructor for SAColorPaletteGridWidget with specified colors
    SAColorPaletteGridWidget(const QList< QColor >& cls, QWidget* par = nullptr);
    /// Destructor for SAColorPaletteGridWidget
    ~SAColorPaletteGridWidget();
    /// Set color list maintained by the widget
    void setColorList(const QList< QColor >& cls);
    /// Get color list
    QList< QColor > colorList() const;
    /// Set color shade factor, default is { 180, 160, 140, 75, 50 }
    void setFactor(const QList< int >& factor);
    /// Get factor list
    QList< int > factor() const;
    /// Set color icon size
    void setColorIconSize(const QSize& s);
    /// Get color icon size
    QSize colorIconSize() const;
    /// Set whether colors are checkable
    void setColorCheckable(bool on = true);
    /// Check if colors are checkable
    bool isColorCheckable() const;
private Q_SLOTS:
    void onMainColorClicked(const QColor& c);
    void onPaletteColorClicked(const QColor& c);

private:
    void init();
Q_SIGNALS:
    /**
     * \if ENGLISH
     * @brief Signal emitted when a color is clicked
     * @param c Clicked color
     * \endif
     *
     * \if CHINESE
     * @brief 对于check模式，check的颜色触发的信号
     * @param c 点击的颜色
     * \endif
     */
    void colorClicked(const QColor& c);
};

#endif  // SACOLORPALETTEGRIDWIDGET_H
