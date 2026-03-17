#ifndef SARIBBONMAINWINDOW_H
#define SARIBBONMAINWINDOW_H
#include "SARibbonGlobal.h"
#include <QMainWindow>

#if !SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
class SAFramelessHelper;
#endif
class QAction;
class SARibbonBar;
class SARibbonSystemButtonBar;
class QScreen;
/**
 * \if ENGLISH
 * @brief Must use this class instead of QMainWindow to use SARibbonBar
 * @details Due to the significant difference between ribbon style and traditional Toolbar style,
 * @details SARibbonBar usage requires replacing the original QMainWindow with SARibbonMainWindow,
 * @details SARibbonMainWindow is a borderless window inherited from QMainWindow (currently using a third-party borderless solution https://github.com/wangwenx190/framelesshelper),
 * @details The useRibbon parameter in its constructor is used to specify whether to use ribbon style, defaulting to true
 * @code
 * SARibbonMainWindow(QWidget* parent = nullptr, bool useRibbon = true);
 * @endcode
 * @details If you want to switch back to non-ribbon style, just set useRibbon to false,
 * @details The isUseRibbon member function is used to determine whether the current mode is ribbon mode, which is very useful when compatible with traditional Toolbar style and ribbon style
 * @details However, this does not support dynamic switching. Therefore, in actual projects, if you want to switch, you
 * need to write a configuration file and pass the mode during program construction,
 * @details And your program must make good judgments, because in non-Ribbon mode, all Ribbon-related interfaces will return null pointers
 * @code
 * bool isUseRibbon() const;
 * @endcode
 * @details @ref SARibbonMainWindow provides several common ribbon styles
 * @details The ribbon style can be changed through @ref setRibbonTheme, and users can also define their own styles through qss
 * @details If you have inherited SARibbonBar yourself, you can set your own ribbonbar through @ref setRibbonBar
 * @details There is also a more efficient way to implement a custom Ribbon, which is to inherit a @ref SARibbonElementFactory
 * @code
 * class MyRibbonFactory:public SARibbonElementFactory{
 * ...
 * virtual SARibbonBar* createRibbonBar(QWidget* parent){
 *     return new MyRibbonBar(parent);
 * }
 * };
 * @endcode
 * @details Before SARibbonMainWindow is generated (usually in the main function), set the element factory:
 * @code
 * SARibbonElementManager::instance()->setupFactory(new MyRibbonFactory());
 * @endcode
 * @details At this point, Ribbon elements will be generated through the MyRibbonFactory interface
 * \endif
 *
 * \if CHINESE
 * @brief 如果要使用SARibbonBar，必须使用此类代替QMainWindow
 * @details 由于ribbon的风格和传统的Toolbar风格差异较大，
 * @details SARibbonBar使用需要把原有的QMainWindow替换为SARibbonMainWindow,
 * @details SARibbonMainWindow是个无边框窗体，继承自QMainWindow（目前使用第三方的无边框方案https://github.com/wangwenx190/framelesshelper），
 * @details 其构造函数的参数useRibbon用于指定是否使用ribbon风格，默认为true
 * @code
 * SARibbonMainWindow(QWidget* parent = nullptr,bool useRibbon = true);
 * @endcode
 * @details 如果想换回非ribbon风格，只需要把useRibbon设置为false即可,
 * @details 成员函数isUseRibbon用于判断当前是否为ribbon模式，这个函数在兼容传统Toolbar风格和ribbon风格时非常有用
 * @details 但这个不支持动态切换，因此，实际工程中，你若要进行切换，需要写配置文件，程序在构造时传入模式，
 * @details 并且，你的程序要做好判断，因为非Ribbon模式下，所有Ribbon相关的接口都会返回空指针
 * @code
 * bool isUseRibbon() const;
 * @endcode
 * @details @ref SARibbonMainWindow 提供了几种常用的ribbon样式
 * @details 通过@ref setRibbonTheme 可改变ribbon的样式，用户也可通过qss自己定义自己的样式
 * @details 如果你自己继承了SARibbonBar，你可以通过@ref setRibbonBar 设置自己的ribbonbar进去
 * @details 另外有个一个更加高效的方法，来实现自定义的Ribbon，就是继承一个@ref SARibbonElementFactory
 * @code
 * class MyRibbonFactory:public SARibbonElementFactory{
 * ...
 * virtual SARibbonBar* createRibbonBar(QWidget* parent){
 *     return new MyRibbonBar(parent);
 * }
 * };
 * @endcode
 * @details SARibbonMainWindow生成之前(一般在main函数），设置元件工厂：
 * @code
 * SARibbonElementManager::instance()->setupFactory(new MyRibbonFactory());
 * @endcode
 * @details 此时，Ribbon的元素会通过MyRibbonFactory的接口来生成
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonMainWindow : public QMainWindow
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonMainWindow)
    friend class SARibbonBar;
    Q_PROPERTY(SARibbonTheme ribbonTheme READ ribbonTheme WRITE setRibbonTheme)

public:
    /// Constructor for SARibbonMainWindow
    explicit SARibbonMainWindow(QWidget* parent                = nullptr,
                                SARibbonMainWindowStyles style = SARibbonMainWindowStyleFlag::UseRibbonMenuBar
                                                                 | SARibbonMainWindowStyleFlag::UseRibbonFrame,
                                const Qt::WindowFlags flags = Qt::WindowFlags());
    /// Destructor for SARibbonMainWindow
    ~SARibbonMainWindow() override;
    /// Return SARibbonBar
    SARibbonBar* ribbonBar() const;
    /// Set ribbonbar
    void setRibbonBar(SARibbonBar* ribbon);
#if !SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
    /// Return SAFramelessHelper
    SAFramelessHelper* framelessHelper() const;
    /// Set to use rubber band indication instead of immediate scaling during resizing, which is more friendly for software with large rendering (such as CAD, 3D)
    void setRubberBandOnResize(bool on);
    /// Check if rubber band is used on resize
    bool isRubberBandOnResize() const;
#else
    /// If there are custom windows in the ribbon added to non-clickable areas such as the title bar, and you want them
    /// to be clickable, you need to call this interface to inform them that they are clickable
    void setFramelessHitTestVisible(QWidget* w, bool visible = true);
#endif
    /// This function is only used to control the display of minimize, maximize and close buttons
    void updateWindowFlag(Qt::WindowFlags flags);
    /// Note: Setting the theme in the constructor will not take full effect, use QTimer to put it at the end of the queue to execute
    /// QTimer::singleShot(0, this, [ this ]() { this->setRibbonTheme(SARibbonMainWindow::RibbonThemeDark); });
    void setRibbonTheme(SARibbonTheme theme);
    /// Get ribbon theme
    SARibbonTheme ribbonTheme() const;
    /// Determine whether the current mode is ribbon mode
    bool isUseRibbon() const;
    /// Get the bar where the maximize, minimize, and close buttons are located. You can set content next to the maximize and minimize buttons through this function
    SARibbonSystemButtonBar* windowButtonBar() const;
    /// Get the current mainwindow style
    SARibbonMainWindowStyles ribbonMainwindowStyle() const;

    /// Pass ribbonbar events to frameless
    virtual bool eventFilter(QObject* obj, QEvent* e) Q_DECL_OVERRIDE;

protected:
    /// Factory function to create ribbonbar
    SARibbonBar* createRibbonBar();
private Q_SLOTS:
    /// Handle primary screen changed event
    void onPrimaryScreenChanged(QScreen* screen);
};

/**
 * \if ENGLISH
 * @brief Event handler for SARibbonMainWindow, mainly handles systembar position adjustment
 * \endif
 *
 * \if CHINESE
 * @brief 针对SARibbonMainWindow的事件处理器，主要处理systembar的位置调整
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonMainWindowEventFilter : public QObject
{
    Q_OBJECT
public:
    /// Constructor for SARibbonMainWindowEventFilter
    explicit SARibbonMainWindowEventFilter(QObject* par);
    /// Destructor for SARibbonMainWindowEventFilter
    ~SARibbonMainWindowEventFilter();
    /// Event filter
    virtual bool eventFilter(QObject* obj, QEvent* e) override;
};

#endif  // SARIBBONMAINWINDOW_H
