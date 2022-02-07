#ifndef SARIBBONPANNELOPTIONBUTTON_H
#define SARIBBONPANNELOPTIONBUTTON_H
#include <QToolButton>
#include "SARibbonGlobal.h"
class QAction;

class SA_RIBBON_EXPORT SARibbonPannelOptionButton : public QToolButton
{
	Q_OBJECT
public:
	SARibbonPannelOptionButton(QWidget *parent = Q_NULLPTR);
	void connectAction(QAction *action);
};

#endif // SAROBBONPANNELOPTIONBUTTON_H
