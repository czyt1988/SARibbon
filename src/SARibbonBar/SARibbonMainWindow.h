#ifndef SARIBBONMAINWINDOW_H
#define SARIBBONMAINWINDOW_H
#include "SARibbonGlobal.h"
#include <QMainWindow>
class SARibbonMainWindowPrivate;
class SARibbonBar;
///
/// \brief The SARibbonMainWindow class
///
class SA_RIBBON_EXPORT SARibbonMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    SARibbonMainWindow(QWidget* parent = nullptr,bool useRibbon = true);
    //
    const SARibbonBar* ribbonBar() const;
    SARibbonBar* ribbonBar();
    enum RibbonTheme{
        NormalTheme  ///< 普通主题

    };
    void setRibbonTheme(RibbonTheme theme);
    RibbonTheme ribbonTheme() const;
	//判断当前是否使用ribbon模式
	bool isUseRibbon() const;
protected:
    void loadTheme(const QString &themeFile);
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    bool eventFilter(QObject *obj, QEvent *e);
private:
    SARibbonMainWindowPrivate* m_d;
};

#endif // SARIBBONMAINWINDOW_H
