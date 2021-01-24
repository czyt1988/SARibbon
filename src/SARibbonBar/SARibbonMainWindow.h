#ifndef SARIBBONMAINWINDOW_H
#define SARIBBONMAINWINDOW_H
#include "SARibbonGlobal.h"
#include <QMainWindow>
class SARibbonMainWindowPrivate;
class SARibbonBar;

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
public:
    SARibbonMainWindow(QWidget *parent = nullptr, bool useRibbon = true);
    ~SARibbonMainWindow();
    //返回SARibbonBar
    const SARibbonBar *ribbonBar() const;
    SARibbonBar *ribbonBar();

    enum RibbonTheme {
        NormalTheme     ///< 普通主题
        , Office2013    ///< office2013主题
    };
    void setRibbonTheme(RibbonTheme theme);
    RibbonTheme ribbonTheme() const;

	//判断当前是否使用ribbon模式
	bool isUseRibbon() const;

protected:
    void loadTheme(const QString& themeFile);
    virtual void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    virtual bool eventFilter(QObject *obj, QEvent *e) Q_DECL_OVERRIDE;
    virtual bool event(QEvent *e) Q_DECL_OVERRIDE;

private:
    SARibbonMainWindowPrivate *m_d;
};

#endif // SARIBBONMAINWINDOW_H
