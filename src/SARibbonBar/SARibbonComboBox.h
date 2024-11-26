#ifndef SARIBBONCOMBOBOX_H
#define SARIBBONCOMBOBOX_H
#include "SARibbonGlobal.h"
#include <QComboBox>



/**
 * @brief QComboBox的Ribbon显示，可以显示QIcon和windowTitle在左侧
 */
class SA_RIBBON_EXPORT SARibbonComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit SARibbonComboBox(QWidget *parent = nullptr);
    ~SARibbonComboBox();
};

#endif // SARIBBONCOMBOBOX_H
