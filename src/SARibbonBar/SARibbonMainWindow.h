#ifndef SARIBBONMAINWINDOW_H
#define SARIBBONMAINWINDOW_H
#include "SARibbonGlobal.h"
#include <QMainWindow>
class SARibbonBar;
class SAFramelessHelper;

/**
 * @brief 如果要使用SARibbonBar，必须使用此类代替QMainWindow
 *
 * 由于ribbon的风格和传统的Toolbar风格差异较大，
 * SARibbonBar使用需要把原有的QMainWindow替换为SARibbonMainWindow,
 * SARibbonMainWindow是个无边框窗体，继承自QMainWindow，其构造函数的参数useRibbon
 * 用于指定是否使用ribbon风格，默认为true
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
        NormalTheme,  ///< 普通主题
        Office2013    ///< office2013主题
    };
    Q_ENUM(RibbonTheme)
public:
    SARibbonMainWindow(QWidget* parent = nullptr, bool useRibbon = true);
    ~SARibbonMainWindow() Q_DECL_OVERRIDE;
    //返回SARibbonBar
    const SARibbonBar* ribbonBar() const;
    SARibbonBar* ribbonBar();

    //返回SAFramelessHelper
    SAFramelessHelper* framelessHelper();

    void setRibbonTheme(RibbonTheme theme);
    RibbonTheme ribbonTheme() const;

    //判断当前是否使用ribbon模式
    bool isUseRibbon() const;

    //此函数仅用于控制最小最大化和关闭按钮的显示
    void updateWindowFlag(Qt::WindowFlags flags);

    //获取系统按钮的状态
    Qt::WindowFlags windowButtonFlags() const;

    //覆写setMenuWidget
    void setMenuWidget(QWidget* menubar);

    //覆写setMenuBar
    void setMenuBar(QMenuBar* menuBar);

protected:
    //创建ribbonbar的工厂函数
    SARibbonBar* createRibbonBar();
    void loadTheme(const QString& themeFile);
    virtual void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;
    virtual bool eventFilter(QObject* obj, QEvent* e) Q_DECL_OVERRIDE;
    virtual bool event(QEvent* e) Q_DECL_OVERRIDE;
};

#endif  // SARIBBONMAINWINDOW_H
