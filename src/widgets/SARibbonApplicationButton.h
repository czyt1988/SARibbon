#ifndef SARIBBONAPPLICATIONBUTTON_H
#define SARIBBONAPPLICATIONBUTTON_H
#include <QToolButton>
#include "SARibbonGlobal.h"

/**
 * @brief The SARibbonApplicationButton class
 *
 * 默认的ApplicationButton,可以通过样式指定不一样的ApplicationButton
 */
class SA_RIBBON_EXPORT SARibbonApplicationButton : public QToolButton
{
    Q_OBJECT
public:
    explicit SARibbonApplicationButton(QWidget* parent = nullptr);
    explicit SARibbonApplicationButton(const QString& text, QWidget* parent = nullptr);
    explicit SARibbonApplicationButton(const QIcon& icon, const QString& text, QWidget* parent = nullptr);
    ~SARibbonApplicationButton();
};

#endif  // SARIBBONAPPLICATIONBUTTON_H
