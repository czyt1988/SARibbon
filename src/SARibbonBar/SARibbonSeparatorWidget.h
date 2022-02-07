#ifndef SARIBBONSEPARATORWIDGET_H
#define SARIBBONSEPARATORWIDGET_H
#include "SARibbonGlobal.h"
#include <QWidget>
#include <QStyleOption>


class SA_RIBBON_EXPORT SARibbonSeparatorWidget : public QWidget
{
	Q_OBJECT
public:
	SARibbonSeparatorWidget(int height, QWidget *parent = nullptr);
	SARibbonSeparatorWidget(QWidget *parent = nullptr);
	virtual QSize sizeHint() const Q_DECL_OVERRIDE;


	void setTopBottomMargins(int top, int bottom);

protected:
	void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
	int m_topMargins;
	int m_bottomMargins;
};

#endif // SARIBBONSEPARATORWIDGET_H
