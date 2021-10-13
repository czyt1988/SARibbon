#include "SARibbonLineWidgetContainer.h"
#include <QHBoxLayout>

SARibbonLineWidgetContainer::SARibbonLineWidgetContainer(QWidget *par) : QWidget(par)
    , m_innerWidget(nullptr)
{
    m_labelPrefix = new QLabel(this);
    m_labelSuffix = new QLabel(this);
    QHBoxLayout *lay = new QHBoxLayout();

    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);
    lay->addWidget(m_labelPrefix);
    lay->addWidget(m_labelSuffix);
    setLayout(lay);
}


void SARibbonLineWidgetContainer::setWidget(QWidget *innerWidget)
{
    QHBoxLayout *lay = static_cast<QHBoxLayout *>(layout());

    if (m_innerWidget) {
        lay->replaceWidget(m_innerWidget, innerWidget);
    }else{
        lay->insertWidget(1, innerWidget);
    }
    m_innerWidget = innerWidget;
}


void SARibbonLineWidgetContainer::setPrefix(const QString& str)
{
    m_labelPrefix->setText(str);
}


void SARibbonLineWidgetContainer::setSuffix(const QString& str)
{
    m_labelSuffix->setText(str);
}


QLabel *SARibbonLineWidgetContainer::labelPrefix() const
{
    return (m_labelPrefix);
}


QLabel *SARibbonLineWidgetContainer::labelSuffix() const
{
    return (m_labelSuffix);
}
