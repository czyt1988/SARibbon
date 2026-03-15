#ifndef SARIBBONTABBAR_H
#define SARIBBONTABBAR_H
#include "SARibbonGlobal.h"
#include <QTabBar>
#include <QMargins>

/**
 * \if ENGLISH
 * @brief Ribbon tab bar widget for SARibbon framework
 * \endif
 *
 * \if CHINESE
 * @brief SARibbon框架的Ribbon标签栏部件
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonTabBar : public QTabBar
{
	Q_OBJECT
public:
	/// Constructs a SARibbonTabBar instance
	explicit SARibbonTabBar(QWidget* parent = nullptr);
    /// Destructor
    ~SARibbonTabBar();
	/// Gets the tab margin
	const QMargins& tabMargin() const;
	/// Sets the tab margin
	void setTabMargin(const QMargins& tabMargin);

protected:
	/// Estimates the size of a tab
	QSize tabSizeHint(int index) const;

private:
	QMargins m_tabMargin;
};

#endif  // SARIBBONTABBAR_H
