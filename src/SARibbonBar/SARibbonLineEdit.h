#ifndef SARIBBONLINEEDIT_H
#define SARIBBONLINEEDIT_H
#include "SARibbonGlobal.h"
#include <QLineEdit>


/**
 * @brief The SARibbonLineEdit class
 */
class SA_RIBBON_EXPORT SARibbonLineEdit : public QLineEdit
{
    Q_OBJECT
public:
	explicit SARibbonLineEdit(QWidget *parent = nullptr);
	explicit SARibbonLineEdit(const QString &text, QWidget *parent = nullptr);
    ~SARibbonLineEdit();
};

#endif // SARIBBONLINEEDIT_H
