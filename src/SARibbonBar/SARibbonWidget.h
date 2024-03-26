#ifndef SARIBBONWIDGET_H
#define SARIBBONWIDGET_H
#include "SARibbonGlobal.h"
#include <QWidget>

class SARibbonBar;
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
class SA_RIBBON_EXPORT SARibbonWidget : public QWidget
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonWidget)
	friend class SARibbonBar;
	Q_PROPERTY(SARibbonTheme ribbonTheme READ ribbonTheme WRITE setRibbonTheme)

public:
	SARibbonWidget(QWidget* parent = nullptr);
	~SARibbonWidget() Q_DECL_OVERRIDE;
	// 返回SARibbonBar
	SARibbonBar* ribbonBar() const;
	// 设置ribbonbar
	void setRibbonBar(SARibbonBar* bar);

	// 注意主题在构造函数设置主题会不完全生效，使用QTimer投放到队列最后执行即可
	// QTimer::singleShot(0, this, [ this ]() { this->setRibbonTheme(SARibbonMainWindow::RibbonThemeDark); });
	void setRibbonTheme(SARibbonTheme theme);
	SARibbonTheme ribbonTheme() const;
	// 判断当前是否使用ribbon模式
	bool isUseRibbon() const;
	//设置窗口
	void setWidget(QWidget* w);
	QWidget* widget() const;
    //
    QWidget* takeWidget();
private slots :
	void onPrimaryScreenChanged(QScreen* screen);
};


#endif  // SARIBBONWIDGET_H
