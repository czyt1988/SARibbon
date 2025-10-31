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
 * 成员函数isUseRibbon用于判断当前是否为ribbon模式，这个函数在兼容传统Toolbar风格和ribbon风格时非常有用
 *
 * 但这个不支持动态切换，因此，实际工程中，你若要进行切换，需要写配置文件，程序在构造时传入模式，
 * 并且，你的程序要做好判断，因为非Ribbon模式下，所有Ribbon相关的接口都会返回空指针
 *
 * @code
 * bool isUseRibbon() const;
 * @endcode
 *
 * @ref SARibbonMainWindow 提供了几种常用的ribbon样式
 * 通过@ref setRibbonTheme 可改变ribbon的样式，用户也可通过qss自己定义自己的样式
 *
 * 如果你自己继承了SARibbonBar，你可以通过@ref setRibbonBar 设置自己的ribbonbar进去
 *
 * 另外有个一个更加高效的方法，来实现自定义的Ribbon，就是继承一个@ref SARibbonElementFactory
 *
 * @code
 * class MyRibbonFactory:public SARibbonElementFactory{
 * ...
 * virtual SARibbonBar* createRibbonBar(QWidget* parent){
 *     return new MyRibbonBar(parent);
 * }
 * };
 * @endcode
 *
 * SARibbonMainWindow生成之前(一般在main函数），设置元件工厂：
 *
 * @code
 * SARibbonElementManager::instance()->setupFactory(new MyRibbonFactory());
 * @endcode
 *
 * 此时，Ribbon的元素会通过MyRibbonFactory的接口来生成
 */
class SA_RIBBON_EXPORT SARibbonMainWindow : public QMainWindow
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonMainWindow)
	friend class SARibbonBar;
	Q_PROPERTY(SARibbonTheme ribbonTheme READ ribbonTheme WRITE setRibbonTheme)

public:
	explicit SARibbonMainWindow(QWidget* parent                = nullptr,
                                SARibbonMainWindowStyles style = SARibbonMainWindowStyleFlag::UseRibbonMenuBar
                                                                 | SARibbonMainWindowStyleFlag::UseRibbonFrame,
                                const Qt::WindowFlags flags = Qt::WindowFlags());
	~SARibbonMainWindow() override;
	// 返回SARibbonBar
	SARibbonBar* ribbonBar() const;
	// 设置ribbonbar
	void setRibbonBar(SARibbonBar* ribbon);
#if !SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
	// 返回SAFramelessHelper
    SAFramelessHelper* framelessHelper() const;
    // 设置缩放时，使用橡皮筋示意而不是立即缩放，这对于大渲染量的软件（如CAD、三维）比较友好
    void setRubberBandOnResize(bool on);
    bool isRubberBandOnResize() const;
#else
	// 如果ribbon中有自定义的窗口在标题栏等非点击区域加入后，想能点击，需要调用此接口告知可点击
	void setFramelessHitTestVisible(QWidget* w, bool visible = true);
#endif
	// 此函数仅用于控制最小最大化和关闭按钮的显示
	void updateWindowFlag(Qt::WindowFlags flags);
	// 注意主题在构造函数设置主题会不完全生效，使用QTimer投放到队列最后执行即可
	// QTimer::singleShot(0, this, [ this ]() { this->setRibbonTheme(SARibbonMainWindow::RibbonThemeDark); });
	void setRibbonTheme(SARibbonTheme theme);
	SARibbonTheme ribbonTheme() const;
	// 判断当前是否使用ribbon模式
	bool isUseRibbon() const;
	// 获取最大化，最小化，关闭按钮所在的bar。可以通过此函数在最大最小化按钮旁边设置内容
	SARibbonSystemButtonBar* windowButtonBar() const;
	// 获取当前mainwidow的样式
	SARibbonMainWindowStyles ribbonMainwindowStyle() const;

    // 把ribbonbar的事件传递到frameless
    virtual bool eventFilter(QObject* obj, QEvent* e) Q_DECL_OVERRIDE;

protected:
	// 创建ribbonbar的工厂函数
	SARibbonBar* createRibbonBar();
private Q_SLOTS:
	void onPrimaryScreenChanged(QScreen* screen);
};

/**
 * @brief 针对SARibbonMainWindow的事件处理器，主要处理systembar的位置调整
 */
class SA_RIBBON_EXPORT SARibbonMainWindowEventFilter : public QObject
{
	Q_OBJECT
public:
	explicit SARibbonMainWindowEventFilter(QObject* par);
	~SARibbonMainWindowEventFilter();
	virtual bool eventFilter(QObject* obj, QEvent* e) override;
};

#endif  // SARIBBONMAINWINDOW_H
