#ifndef SARIBBONWIDGET_H
#define SARIBBONWIDGET_H
#include "SARibbonGlobal.h"
#include <QWidget>

class SARibbonBar;
class QScreen;
/**
 * @brief 带SARibbonBar的Widget
 *
 * @note 注意，SARibbonWidget利用了布局来把ribbonbar放到最顶部，不要给此widget添加布局，否则会导致布局异常，
 * 正确的做法是把布局的窗口通过setWidget设置进SARibbonWidget
 *
 */
class SA_RIBBON_EXPORT SARibbonWidget : public QWidget
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonWidget)
	friend class SARibbonBar;
	Q_PROPERTY(SARibbonTheme ribbonTheme READ ribbonTheme WRITE setRibbonTheme)

public:
	explicit SARibbonWidget(QWidget* parent = nullptr);
	~SARibbonWidget();
	// 返回SARibbonBar
	SARibbonBar* ribbonBar() const;
	// 设置ribbonbar
	void setRibbonBar(SARibbonBar* bar);

	// 注意主题在构造函数设置主题会不完全生效，使用QTimer投放到队列最后执行即可
    // QTimer::singleShot(0, this, [ this ]() { this->setRibbonTheme(SARibbonTheme::RibbonThemeDark); });
	void setRibbonTheme(SARibbonTheme theme);
	SARibbonTheme ribbonTheme() const;
	// 判断当前是否使用ribbon模式
	bool isUseRibbon() const;
    // 设置窗口
	void setWidget(QWidget* w);
	QWidget* widget() const;
    //
    QWidget* takeWidget();
private Q_SLOTS:
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
void SA_RIBBON_EXPORT sa_set_ribbon_theme(QWidget* w, SARibbonTheme theme);
#endif  // SARIBBONWIDGET_H
