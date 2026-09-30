#ifndef SARIBBONMDICONTROLSSTYLE_H
#define SARIBBONMDICONTROLSSTYLE_H
#include <QProxyStyle>

/**
 * \if ENGLISH
 * @brief Internal style that paints the MDI corner controls (minimize/restore/close) with sharp vector glyphs
 * @details When a QMdiSubWindow is maximized, Qt moves its private self-drawn controller
 * widget (QMdi::ControllerWidget) into the menu bar corner via QMenuBar::setCornerWidget().
 * SARibbonBar is the menu bar, so this widget appears at the top right corner of the ribbon
 * and is scaled to the tab bar height by SARibbonBarLayout.
 * @details Since Qt6 the Windows style renders these theme parts at half resolution and scales
 * the result up when the requested size reaches twice the native part size (workaround for
 * QTBUG-75927), which makes the buttons look blurry. This style overrides CC_MdiControls and
 * paints the buttons with vector primitives so they stay sharp at any scale factor on both
 * Qt5 and Qt6.
 * @note This is an internal class, it is applied per widget through applyToMdiControllerWidget()
 * \endif
 *
 * \if CHINESE
 * @brief 内部样式，用矢量图元清晰绘制MDI子窗口的最小化/还原/关闭按钮
 * @details 当QMdiSubWindow最大化时，Qt会把其私有自绘控件（QMdi::ControllerWidget）
 * 通过QMenuBar::setCornerWidget()移动到菜单栏角落。由于SARibbonBar就是菜单栏，
 * 该控件会显示在ribbon右上角，并被SARibbonBarLayout缩放到标签栏高度。
 * @details Qt6的Windows样式在请求尺寸达到原生主题部件2倍时，会先以半分辨率绘制再放大
 * （QTBUG-75927的补救方案），导致按钮模糊。此样式重写CC_MdiControls，
 * 用矢量图元绘制按钮，在Qt5/Qt6任意缩放比例下均保持清晰。
 * @note 这是内部类，通过applyToMdiControllerWidget()逐控件应用
 * \endif
 */
class SARibbonMdiControlsStyle : public QProxyStyle
{
public:
    // Constructor
    SARibbonMdiControlsStyle();
    // Paint MDI controls with vector primitives, other controls are delegated to the base style
    void drawComplexControl(ComplexControl control, const QStyleOptionComplex* option, QPainter* painter,
                            const QWidget* widget = nullptr) const Q_DECL_OVERRIDE;
    // Sub control rect of the MDI buttons, other controls are delegated to the base style
    QRect subControlRect(ComplexControl control, const QStyleOptionComplex* option, SubControl subControl,
                         const QWidget* widget = nullptr) const Q_DECL_OVERRIDE;
    // Hit test of the MDI buttons, other controls are delegated to the base style
    SubControl hitTestComplexControl(ComplexControl control, const QStyleOptionComplex* option, const QPoint& position,
                                     const QWidget* widget = nullptr) const Q_DECL_OVERRIDE;
    // Apply this style to the widget if it is Qt's private MDI controller widget
    static void applyToMdiControllerWidget(QWidget* widget);
};

#endif  // SARIBBONMDICONTROLSSTYLE_H
