#ifndef SARIBBONSTACKEDWIDGET_H
#define SARIBBONSTACKEDWIDGET_H
#include <QStackedWidget>
#include "SARibbonGlobal.h"
class QHideEvent;
class QResizeEvent;
class SARibbonStackedWidgetPrivate;



class SA_RIBBON_EXPORT SARibbonStackedWidget : public QStackedWidget
{
	Q_OBJECT
public:
	SARibbonStackedWidget(QWidget *parent);
	~SARibbonStackedWidget();
	void setPopupMode();
	bool isPopupMode() const;
	void setNormalMode();
	bool isNormalMode() const;
	void exec();

	void setAutoResize(bool autoresize);
	bool isAutoResize() const;
	void moveWidget(int from, int to);

protected:
	//    void mouseReleaseEvent(QMouseEvent *e);
	void hideEvent(QHideEvent *e) override;

signals:
	void hidWindow();

private:
	SARibbonStackedWidgetPrivate *m_d;
};

#endif // SARIBBONSTACKEDWIDGET_H
