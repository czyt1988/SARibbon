#ifndef SARIBBONMAINWINDOW_H
#define SARIBBONMAINWINDOW_H
#include "SARibbonGlobal.h"
#include <QMainWindow>

#if !SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
class SAFramelessHelper;
#endif

class SARibbonBar;
class SAWindowButtonGroup;
class QScreen;
/**
 * @brief 如果要使用SARibbonBar，必须使用此类代替QMainWindow
 *
 * 由于ribbon的风格和传统的Toolbar风格差异较大，
 * SARibbonBar使用需要把原有的QMainWindow替换为SARibbonMainWindow,
 * SARibbonMainWindow是个无边框窗体，继承自QMainWindow（目前使用第三方的无边框方案https://github.com/wangwenx190/framelesshelper），
 * 其构造函数的参数useRibbon用于指定是否使用ribbon风格，默认为true
 *
 * @code
 * SARibbonMainWindow(QWidget* parent = nullptr,bool useRibbon = true);
 * @endcode
 *
 * 如果想换回非ribbon风格，只需要把useRibbon设置为false即可,
 * 成员函数isUseRibbon用于判断当前是否为ribbon模式，这个函数在兼容传统Toolbar风格和ribbon风格时非常有用。
 *
 * @code
 * bool isUseRibbon() const;
 * @endcode
 *
 * @ref SARibbonMainWindow 提供了几种常用的ribbon样式，样式可见@ref RibbonTheme
 * 通过@ref setRibbonTheme 可改变ribbon的样式，用户也可通过qss自己定义自己的样式
 *
 */
class SA_RIBBON_EXPORT SARibbonMainWindow : public QMainWindow
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonMainWindow)
    friend class SARibbonBar;
    Q_PROPERTY(RibbonTheme ribbonTheme READ ribbonTheme WRITE setRibbonTheme)
public:
    /**
     * @brief Ribbon主题，可以通过qss定制ribbon的主题，定制方法可参看源码中office2013.qss
     *
     * 注意，由于有些qss的尺寸，在C++代码中无法获取到，因此针对用户自定义的qss主题，有些尺寸是需要手动设置进去的
     *
     * 例如ribbon tab的margin信息，在QTabBar是无法获取到，而这个影响了SARibbonContextCategory的绘制，
     * 因此，在设置qss后需要针对margin信息重新设置进SARibbonTabBar中
     */
    enum RibbonTheme
    {
        RibbonThemeOffice2013,      ///< office2013主题
        RibbonThemeOffice2016Blue,  ///< office2016-蓝色主题
        RibbonThemeOffice2021Blue,  ///< office2021-蓝色主题
        RibbonThemeWindows7,        ///< win7主题
        RibbonThemeDark,            ///< 暗色主题
        RibbonThemeDark2
    };
    Q_ENUM(RibbonTheme)
public:
    SARibbonMainWindow(QWidget* parent = nullptr, bool useRibbon = true, const Qt::WindowFlags flags = {});
    ~SARibbonMainWindow() Q_DECL_OVERRIDE;
    // 返回SARibbonBar
    SARibbonBar* ribbonBar() const;
    // 设置ribbonbar
    void setRibbonBar(SARibbonBar* bar);
#if !SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
    // 返回SAFramelessHelper
    SAFramelessHelper* framelessHelper();
#else
    // 如果ribbon中有自定义的窗口在标题栏等非点击区域加入后，想能点击，需要调用此接口告知可点击
    void setFramelessHitTestVisible(const QWidget* w, bool visible = true);
#endif
    // 此函数仅用于控制最小最大化和关闭按钮的显示
    void updateWindowFlag(Qt::WindowFlags flags);

    // 注意主题在构造函数设置主题会不完全生效，使用QTimer投放到队列最后执行即可
    // QTimer::singleShot(0, this, [ this ]() { this->setRibbonTheme(SARibbonMainWindow::RibbonThemeDark); });
    void setRibbonTheme(RibbonTheme theme);
    RibbonTheme ribbonTheme() const;
    // 判断当前是否使用ribbon模式
    bool isUseRibbon() const;
    // 把ribbonbar的事件传递到frameless
    virtual bool eventFilter(QObject* obj, QEvent* e) Q_DECL_OVERRIDE;
    // 获取最大化，最小化，关闭按钮所在的bar。可以通过此函数在最大最小化按钮旁边设置内容
    SAWindowButtonGroup* windowButtonBar() const;

protected:
    // 创建ribbonbar的工厂函数
    SARibbonBar* createRibbonBar();
    virtual void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;
    virtual void changeEvent(QEvent* e) Q_DECL_OVERRIDE;
private slots:
    void onPrimaryScreenChanged(QScreen* screen);
};

/**
 * @brief 全局的设置ribbon theme函数
 *
 * 之所以提供此函数，是因为在某些情况下，SARibbonBar不用在SARibbonMainWindow情况下的时候，也需要设置主题，
 * 但主题设置是在SARibbonMainWindow下的为了能在非SARibbonMainWindow下也能设置主题，这里提供@sa sa_set_ribbon_theme函数，
 * 可以这样使用：
 * @code
 * @endcode
 *
 * @param w
 * @param theme
 */
void SA_RIBBON_EXPORT sa_set_ribbon_theme(QWidget* w, SARibbonMainWindow::RibbonTheme theme);

#endif  // SARIBBONMAINWINDOW_H
