#ifndef SARIBBONMAINWINDOW_H
#define SARIBBONMAINWINDOW_H
#include "SARibbonGlobal.h"
#include <QMainWindow>
class SARibbonMainWindowPrivate;
class SARibbonBar;
class SAFramelessHelper;

class SA_RIBBON_EXPORT SARibbonMainWindow : public QMainWindow
{
	Q_OBJECT
		Q_PROPERTY(RibbonTheme ribbonTheme READ ribbonTheme WRITE setRibbonTheme)
public:
	enum RibbonTheme {
		NormalTheme
		, Office2013
	};
	Q_ENUMS(RibbonTheme)
public:
	SARibbonMainWindow(QWidget *parent = nullptr, bool useRibbon = true);
	~SARibbonMainWindow() Q_DECL_OVERRIDE;
	const SARibbonBar *ribbonBar() const;
	SARibbonBar *ribbonBar();

	SAFramelessHelper *framelessHelper();

	void setRibbonTheme(RibbonTheme theme);
	RibbonTheme ribbonTheme() const;

	bool isUseRibbon() const;

	void updateWindowFlag(Qt::WindowFlags flags);

	Qt::WindowFlags windowButtonFlags() const;

	void setMenuWidget(QWidget *menubar);

	void setMenuBar(QMenuBar *menuBar);

protected:
	void loadTheme(const QString& themeFile);
	virtual void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
	virtual bool eventFilter(QObject *obj, QEvent *e) Q_DECL_OVERRIDE;
	virtual bool event(QEvent *e) Q_DECL_OVERRIDE;

private:
	SARibbonMainWindowPrivate *m_d;
};

#endif // SARIBBONMAINWINDOW_H
