#ifndef SARIBBONCHECKBOX_H
#define SARIBBONCHECKBOX_H
#include "SARibbonGlobal.h"
#include <QCheckBox>
#include "SARibbonCtrolContainer.h"
class SA_RIBBON_EXPORT SARibbonCheckBox : public SARibbonCtrolContainer
{
public:
    SARibbonCheckBox(QWidget *parent = Q_NULLPTR);
    QCheckBox* checkBox();
protected:
    void initStyleOption(QStyleOption* opt) Q_DECL_OVERRIDE;
private:
    QCheckBox* m_w;
};

#endif // SARIBBONCHECKBOX_H
