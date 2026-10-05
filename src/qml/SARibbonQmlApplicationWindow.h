#ifndef RIBBONAPPLICATIONWINDOW_H
#define RIBBONAPPLICATIONWINDOW_H
#include "SARibbonQmlGlobal.h"
#include <QQuickItem>

namespace SARibbonQml {

/**
 * \if ENGLISH
 * @brief Application window host: office-backstage overlay behind the application button
 * @details The QML counterpart of the widgets SARibbonApplicationWidget: a plain
 *          structural item the user fills with arbitrary content (list views,
 *          buttons, labels). Declared as a child of the bar, it opens as an
 *          overlay window covering a configurable fraction of the window
 *          (coverageRatio: 1.0 fullscreen, 2/3 or any custom value), with a
 *          selectable enter/exit animation (slide from the left etc.). Esc and
 *          outside clicks close it; a top-right close cross is provided for
 *          fullscreen coverage (showCloseButton); the close() invokable lets
 *          inner buttons close it programmatically.
 * \endif
 *
 * \if CHINESE
 * @brief 应用窗口宿主：应用按钮背后的 Office 后台式覆盖层
 * @details 对应 widgets 侧 SARibbonApplicationWidget：一个透明结构项，
 *          用户在其中声明任意内容（列表、按钮、文本）。声明为 bar 的
 *          子项后，点击应用按钮以覆盖窗口形式打开——覆盖比例可配置
 *          （coverageRatio：1.0 全屏、2/3 或任意自定义值），进出动画可选
 *          （从左侧滑出等）。Esc 与点击外部区域关闭；全屏覆盖时提供右上角
 *          关闭叉按钮（showCloseButton）；close() 可调用方法供内部按钮
 *          编程式关闭。
 * \endif
 */
class SA_RIBBON_QML_EXPORT RibbonApplicationWindow : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(bool popupVisible READ isPopupVisible WRITE setPopupVisible NOTIFY popupVisibleChanged)
    Q_PROPERTY(qreal coverageRatio READ coverageRatio WRITE setCoverageRatio NOTIFY coverageRatioChanged)
    Q_PROPERTY(bool showCloseButton READ isShowCloseButton WRITE setShowCloseButton NOTIFY showCloseButtonChanged)
    Q_PROPERTY(AnimationEffect animation READ animation WRITE setAnimation NOTIFY animationChanged)
    Q_PROPERTY(int animationDuration READ animationDuration WRITE setAnimationDuration NOTIFY animationDurationChanged)
public:
    enum AnimationEffect {
        NoAnimation = 0,  ///< appears instantly, no enter/exit transition
        SlideFromLeft,    ///< slides in from beyond the left window edge
        SlideFromRight,   ///< slides in from beyond the right window edge
        Fade              ///< fades in over the covered area
    };
    Q_ENUM(AnimationEffect)
    explicit RibbonApplicationWindow(QQuickItem* parent = nullptr);
    ~RibbonApplicationWindow() override;

    // visibility while shown through the application button (tests + leaf)
    bool isPopupVisible() const;
    void setPopupVisible(bool on);

    // covered fraction of the window width, clamped to [0.05, 1.0]
    qreal coverageRatio() const;
    void setCoverageRatio(qreal ratio);

    // whether the presentation layer offers its top-right close cross
    bool isShowCloseButton() const;
    void setShowCloseButton(bool on);

    // enter/exit animation effect of the presentation layer
    AnimationEffect animation() const;
    void setAnimation(AnimationEffect effect);

    // enter/exit animation duration in milliseconds
    int animationDuration() const;
    void setAnimationDuration(int ms);

    // close from inside the content (buttons etc.); routes through the bar
    Q_INVOKABLE void close();

Q_SIGNALS:
    void popupVisibleChanged();
    void coverageRatioChanged();
    void showCloseButtonChanged();
    void animationChanged();
    void animationDurationChanged();
    void closeRequested();

private:
    qreal mCoverageRatio = 1.0;               ///< covered window-width fraction
    bool mShowCloseButton = true;             ///< top-right cross visibility
    AnimationEffect mAnimation = SlideFromLeft;  ///< enter/exit effect
    int mAnimationDuration = 250;             ///< effect duration in ms
    bool mPopupVisible = false;               ///< shown-through-app-button state
};

}

#endif  // RIBBONAPPLICATIONWINDOW_H
