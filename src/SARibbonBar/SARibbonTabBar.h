#ifndef SARIBBONTABBAR_H
#define SARIBBONTABBAR_H
#include "SARibbonGlobal.h"
#include <QTabBar>
#include <QMargins>

/**
 * @brief The SARibbonTabBar class
 */
class SA_RIBBON_EXPORT SARibbonTabBar : public QTabBar
{
    Q_OBJECT
public:
    SARibbonTabBar(QWidget *parent = Q_NULLPTR);
    const QMargins& tabMargin() const;
    void setTabMargin(const QMargins& tabMargin);

private:
    QMargins m_tabMargin;
};

#endif // SARIBBONTABBAR_H
