#ifndef SARIBBONLINEWIDGETCONTAINER_H
#define SARIBBONLINEWIDGETCONTAINER_H

#include <QtCore/qglobal.h>
#include <QWidget>
#include <QLabel>
#include "SARibbonGlobal.h"


class SA_RIBBON_EXPORT SARibbonLineWidgetContainer : public QWidget
{
public:
	SARibbonLineWidgetContainer(QWidget *par = nullptr);
	void setWidget(QWidget *innerWidget);

	void setPrefix(const QString& str);

	void setSuffix(const QString& str);

	QLabel *labelPrefix() const;
	QLabel *labelSuffix() const;

private:
	QLabel *m_labelPrefix;
	QLabel *m_labelSuffix;
	QWidget *m_innerWidget;
};

#endif // SARIBBONWIDGETCONTAINER_H
