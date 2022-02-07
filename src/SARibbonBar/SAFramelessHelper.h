#ifndef SAFRAMELESSHELPER_H
#define SAFRAMELESSHELPER_H


#include "SARibbonGlobal.h"
#include <QObject>

class QWidget;
class FramelessHelperPrivate;

class SA_RIBBON_EXPORT SAFramelessHelper : public QObject
{
	Q_OBJECT

public:
	explicit SAFramelessHelper(QObject *parent);
	~SAFramelessHelper();

	void activateOn(QWidget *topLevelWidget);


	void removeFrom(QWidget *topLevelWidget);


	void setWidgetMovable(bool movable);


	void setWidgetResizable(bool resizable);


	void setRubberBandOnMove(bool movable);


	void setRubberBandOnResize(bool resizable);


	void setBorderWidth(int width);


	void setTitleHeight(int height);
	bool widgetResizable();
	bool widgetMovable();
	bool rubberBandOnMove();
	bool rubberBandOnResisze();
	uint borderWidth();
	uint titleHeight();

protected:

	virtual bool eventFilter(QObject *obj, QEvent *event);

private:
	FramelessHelperPrivate *d;
};

#endif // FRAMELESSHELPER_H
