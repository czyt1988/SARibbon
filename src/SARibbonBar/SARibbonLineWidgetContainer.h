#ifndef SARIBBONLINEWIDGETCONTAINER_H
#define SARIBBONLINEWIDGETCONTAINER_H

#include <QtCore/qglobal.h>
#include <QWidget>
#include <QLabel>
#include "SARibbonGlobal.h"

/**
 * \if ENGLISH
 * @brief A widget container that places the widget in the middle, with text before and after, mainly used for small widgets on the panel
 * @details Implements the following effect:
 * @details PrefixLabel|_Widget_|SuffixLabel
 * \endif
 *
 * \if CHINESE
 * @brief 一个窗口容器，把窗口放置中间，前面后面都可以设置文本，主要用于放置在panel上的小窗口
 * @details 实现如下效果：
 * @details PrefixLabel|_Widget_|SuffixLabel
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonLineWidgetContainer : public QWidget
{
public:
	/// Constructor for SARibbonLineWidgetContainer
	explicit SARibbonLineWidgetContainer(QWidget* par = nullptr);
	/// Destructor for SARibbonLineWidgetContainer
	~SARibbonLineWidgetContainer();
	/// Set widget (nullptr is not allowed)
	void setWidget(QWidget* innerWidget);

	/// Set prefix
	void setPrefix(const QString& str);

	/// Set suffix
	void setSuffix(const QString& str);

	/// Prefix text box
	QLabel* labelPrefix() const;

	/// Suffix text box
	QLabel* labelSuffix() const;

private:
	// 两个文本
	QLabel* m_labelPrefix;
	QLabel* m_labelSuffix;
	QWidget* m_innerWidget;
};

#endif  // SARIBBONWIDGETCONTAINER_H
