#ifndef SARIBBONPANNELITEM_H
#define SARIBBONPANNELITEM_H
#include "SARibbonGlobal.h"
#include <QWidgetItem>
#include <QAction>

class SA_RIBBON_EXPORT SARibbonPannelItem : public QWidgetItem
{
public:

	enum RowProportion {
		None
		, Large
		, Medium
		, Small
	};
	SARibbonPannelItem(QWidget *widget);
	bool isEmpty() const Q_DECL_OVERRIDE;

	short rowIndex;
	int columnIndex;
	QRect itemWillSetGeometry;
	QAction *action;
	bool customWidget;
	SARibbonPannelItem::RowProportion rowProportion;
};
#ifndef SARibbonPannelItemRowProportionPropertyName
#define SARibbonPannelItemRowProportionPropertyName    "SARibbonPannelItem_RowProportion"
#endif
#endif // SARIBBONPANNELITEM_H
