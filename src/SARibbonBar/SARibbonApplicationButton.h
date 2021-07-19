#ifndef SARIBBONAPPLICATIONBUTTON_H
#define SARIBBONAPPLICATIONBUTTON_H
#include <QPushButton>
#include "SARibbonGlobal.h"

/**
 * @brief The SARibbonApplicationButton class
 */
class SA_RIBBON_EXPORT SARibbonApplicationButton : public QPushButton
{
    Q_OBJECT
public:
    SARibbonApplicationButton(QWidget *parent = nullptr);
    SARibbonApplicationButton(const QString& text, QWidget *parent = nullptr);
    SARibbonApplicationButton(const QIcon& icon, const QString& text, QWidget *parent = nullptr);
};

#endif // SARIBBONAPPLICATIONBUTTON_H
