#ifndef SARIBBONLINEWIDGETCONTAINER_H
#define SARIBBONLINEWIDGETCONTAINER_H

#include <QtCore/qglobal.h>
#include <QWidget>
#include <QLabel>
#include "SARibbonGlobal.h"

/**
 * @brief 一个窗口容器，把窗口放置中间，前面后面都可以设置文本，主要用于放置在pannel上的小窗口
 *
 * 实现如下效果：
 *
 * PrefixLabel|_Widget_|SuffixLabel
 *
 */
class SA_RIBBON_EXPORT SARibbonLineWidgetContainer : public QWidget
{
public:
    SARibbonLineWidgetContainer(QWidget* par = nullptr);

    //设置widget,不允许设置一个nullptr
    void setWidget(QWidget* innerWidget);

    //设置前缀
    void setPrefix(const QString& str);

    //设置后缀
    void setSuffix(const QString& str);

    //前缀文本框
    QLabel* labelPrefix() const;

    //后缀文本框
    QLabel* labelSuffix() const;

private:
    //两个文本
    QLabel* m_labelPrefix;
    QLabel* m_labelSuffix;
    QWidget* m_innerWidget;
};

#endif  // SARIBBONWIDGETCONTAINER_H
