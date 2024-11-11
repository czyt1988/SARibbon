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
	explicit SARibbonTabBar(QWidget* parent = nullptr);
    ~SARibbonTabBar();
	const QMargins& tabMargin() const;
	void setTabMargin(const QMargins& tabMargin);

protected:
	QSize tabSizeHint(int index) const;

private:
	QMargins m_tabMargin;
};

#endif  // SARIBBONTABBAR_H
