#ifndef SARIBBONCHECKBOX_H
#define SARIBBONCHECKBOX_H
#include "SARibbonGlobal.h"
#include <QCheckBox>

/**
 * @brief The SARibbonCheckBox class
 */
class SA_RIBBON_EXPORT SARibbonCheckBox : public QCheckBox
{
    Q_OBJECT
public:
	explicit SARibbonCheckBox(QWidget *parent = nullptr);
	explicit SARibbonCheckBox(const QString &text, QWidget *parent = nullptr);
    ~SARibbonCheckBox();
};

#endif // SARIBBONCHECKBOX_H
