#ifndef SACOLORGRIDWIDGET_H
#define SACOLORGRIDWIDGET_H
#include <QWidget>
#include <functional>
#include "SAColorWidgetsGlobal.h"
class QAbstractButton;
class SAColorToolButton;
/**
 * \if ENGLISH
 * @brief A grid-based color layout widget
 * @details This widget displays a grid of color buttons like:
 * @details □□□□□□□□□
 * @details □□□□□□□□□
 * @details □□□□□□□□□
 * \endif
 *
 * \if CHINESE
 * @brief 一个grid的Color布局
 * @details 一个形如下面的颜色grid：
 * @details □□□□□□□□□
 * @details □□□□□□□□□
 * @details □□□□□□□□□
 * \endif
 */
class SA_COLOR_WIDGETS_API SAColorGridWidget : public QWidget
{
    Q_OBJECT
    SA_COLOR_WIDGETS_DECLARE_PRIVATE(SAColorGridWidget)
    Q_PROPERTY(int spacing READ spacing WRITE setSpacing)
public:
    using FunColorBtn = std::function< void(SAColorToolButton*) >;

public:
    /// Constructor for SAColorGridWidget
    SAColorGridWidget(QWidget* par = nullptr);
    /// Destructor for SAColorGridWidget
    ~SAColorGridWidget();
    /// Set column count, row count will be determined by column count
    void setColumnCount(int c);
    /// Get column count
    int columnCount() const;
    /// Set current color list
    void setColorList(const QList< QColor >& cls);
    /// Get color list
    QList< QColor > getColorList() const;
    /// Get spacing
    int spacing() const;
    /// Set spacing
    void setSpacing(int v);
    /// Get color count
    int colorCount() const;
    /// Set color icon size
    void setColorIconSize(const QSize& s);
    /// Get color icon size
    QSize colorIconSize() const;
    /// Set whether colors are checkable
    void setColorCheckable(bool on = true);
    /// Check if colors are checkable
    bool isColorCheckable() const;
    /// Get currently checked color
    QColor currentCheckedColor() const;
    /// Get color button by index
    SAColorToolButton* colorButton(int index) const;
    /// Set vertical spacing
    void setVerticalSpacing(int v);
    /// Get vertical spacing
    int verticalSpacing() const;
    /// Set horizontal spacing
    void setHorizontalSpacing(int v);
    /// Get horizontal spacing
    int horizontalSpacing() const;
    /// Clear checked state, no color will be selected
    void clearCheckedState();
    /// Iterate through all color buttons
    void iterationColorBtns(FunColorBtn fn);
    /// Set row minimum height
    void setRowMinimumHeight(int row, int minSize);
    /// Set horizontal spacer to right
    void setHorizontalSpacerToRight(bool on = true);
private Q_SLOTS:
    void onButtonClicked(QAbstractButton* btn);
    void onButtonPressed(QAbstractButton* btn);
    void onButtonReleased(QAbstractButton* btn);
    void onButtonToggled(QAbstractButton* btn, bool on);

Q_SIGNALS:
    /**
     * \if ENGLISH
     * @brief Signal emitted when a color is clicked
     * @param c Clicked color
     * \endif
     *
     * \if CHINESE
     * @brief 颜色被点击时发出的信号
     * @param c 被点击的颜色
     * \endif
     */
    void colorClicked(const QColor& c);
    
    /**
     * \if ENGLISH
     * @brief Signal emitted when a color is pressed
     * @param c Pressed color
     * \endif
     *
     * \if CHINESE
     * @brief 颜色被按下时发出的信号
     * @param c 被按下的颜色
     * \endif
     */
    void colorPressed(const QColor& c);
    
    /**
     * \if ENGLISH
     * @brief Signal emitted when a color is released
     * @param c Released color
     * \endif
     *
     * \if CHINESE
     * @brief 颜色被释放时发出的信号
     * @param c 被释放的颜色
     * \endif
     */
    void colorReleased(const QColor& c);
    
    /**
     * \if ENGLISH
     * @brief Signal emitted when a color is toggled (for checkable mode)
     * @param c Toggled color
     * @param on Toggle state
     * \endif
     *
     * \if CHINESE
     * @brief 对于check模式，check的颜色触发的信号
     * @param c 被切换的颜色
     * @param on 切换状态
     * \endif
     */
    void colorToggled(const QColor& c, bool on);

public:
    /// Get size hint
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;
};
namespace SA
{
/// Get standard color list (10 colors in total)
SA_COLOR_WIDGETS_API QList< QColor > getStandardColorList();
}
#endif  // SACOLORGRIDWIDGET_H
