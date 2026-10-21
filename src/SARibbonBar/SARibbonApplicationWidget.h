#ifndef SARIBBONAPPLICATIONWIDGET_H
#define SARIBBONAPPLICATIONWIDGET_H
#include "SARibbonGlobal.h"
#include <QFrame>
class QShowEvent;
class QHideEvent;
class QKeyEvent;
class QColor;
class SARibbonMainWindow;
/**
 * \if ENGLISH
 * @brief Widget for application button popup
 * @details This widget is used to display a file window similar to Office software when the application button is clicked.
 * By default it covers the entire MainWindow; the coverage can be changed via @ref setWidthRatio (for example a 2/3 width
 * panel anchored to the left edge).
 * @details An optional translucent mask (see @ref setMaskWidgetEnabled) can cover the remaining area of the main window;
 * clicking the mask hides this widget. Pressing Escape also hides it.
 * @details When shown, the widget can slide in from the left edge with a fade-in of the mask (see @ref setAnimationEnabled).
 * @note SARibbonMainWindow::setApplicationWidget can be used to bind this widget to the application button so that
 * clicking the button toggles its visibility.
 * \endif
 *
 * \if CHINESE
 * @brief 应用程序按钮弹出窗口
 * @details 这个窗口是用于applicationButton点击后弹出类似office系列软件的文件窗口。
 * 默认会把整个MainWindow覆盖，可通过 @ref setWidthRatio 改变覆盖比例（例如2/3宽度并贴住左边缘）。
 * @details 可启用半透明遮罩（见 @ref setMaskWidgetEnabled ）盖住主窗口剩余区域，点击遮罩将关闭本窗口，按Escape键亦可关闭。
 * @details 显示时可启用从左侧滑入的动画，遮罩同步淡入（见 @ref setAnimationEnabled ）。
 * @note 可通过 SARibbonMainWindow::setApplicationWidget 把本窗口绑定到应用按钮，实现点击按钮切换显示/隐藏。
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonApplicationWidget : public QFrame
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonApplicationWidget)
    Q_PROPERTY(qreal widthRatio READ widthRatio WRITE setWidthRatio)
public:
    // Constructor for SARibbonApplicationWidget
    SARibbonApplicationWidget(SARibbonMainWindow* parent);
    // Destructor for SARibbonApplicationWidget
    ~SARibbonApplicationWidget() override;
    // Set the width ratio of the widget against its parent window, valid range (0.0, 1.0], 1.0 means covering the whole parent window (default). Invalid values fall back to 1.0
    void setWidthRatio(qreal ratio);
    // Get the width ratio
    qreal widthRatio() const;
    // Enable/disable the translucent mask that covers the rest of the parent window; clicking the mask hides this widget (default off)
    void setMaskWidgetEnabled(bool on = true);
    // Check whether the mask is enabled
    bool isMaskWidgetEnabled() const;
    // Set the mask color (alpha included), the default is translucent black
    void setMaskColor(const QColor& color);
    // Get the mask color
    QColor maskColor() const;
    // Enable/disable the slide-in animation when showing (default on)
    void setAnimationEnabled(bool on = true);
    // Check whether the animation is enabled
    bool isAnimationEnabled() const;
    // Set the animation duration in milliseconds, default is 250
    void setAnimationDuration(int ms);
    // Get the animation duration
    int animationDuration() const;

protected:
    // Resizes the widget to match the parent's size multiplied by widthRatio, anchored to the top-left corner
    virtual void resizeToParent(const QSize& parentSize);

protected:
    // Event filter to handle parent window events
    bool eventFilter(QObject* obj, QEvent* ev) override;
    // Handles the show event
    void showEvent(QShowEvent* event) override;
    // Handles the hide event: stops animations and hides the mask
    void hideEvent(QHideEvent* event) override;
    // Handles key press events
    void keyPressEvent(QKeyEvent* ev) override;
};

#endif  // SARIBBONAPPLICATIONWIDGET_H
