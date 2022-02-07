#ifndef SARIBBONCOMBOBOX_H
#define SARIBBONCOMBOBOX_H
#include "SARibbonGlobal.h"
#include <QComboBox>




class SA_RIBBON_EXPORT SARibbonComboBox : public QComboBox
{
	Q_OBJECT
public:
	SARibbonComboBox(QWidget *parent = Q_NULLPTR);
};

#endif // SARIBBONCOMBOBOX_H
