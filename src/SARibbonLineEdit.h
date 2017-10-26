#ifndef SARIBBONLINEEDIT_H
#define SARIBBONLINEEDIT_H
#include "SARibbonGlobal.h"
#include <QLineEdit>
#include "SARibbonCtrolContainer.h"


class SA_RIBBON_EXPORT SARibbonLineEdit : public SARibbonCtrolContainer
{
public:
    SARibbonLineEdit(QWidget *parent = Q_NULLPTR);
    QLineEdit* lineEdit();
protected:
    void initStyleOption(QStyleOption* opt) Q_DECL_OVERRIDE;
private:
    QLineEdit* m_w;
};

#endif // SARIBBONLINEEDIT_H
