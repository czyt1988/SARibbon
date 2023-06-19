#include "SARibbonCategoryContainerExtension.h"
#include <QDesignerFormWindowInterface>
#include "SARibbonCategory.h"
#include "SARibbonPannel.h"
#include "SARibbonPluginDebugHelper.h"
using namespace SA_PLUGIN;
SARibbonCategoryContainerExtension::SARibbonCategoryContainerExtension(SARibbonCategory *c, QObject *p)
    : QObject(p)
    , m_category(c)
{
}


void SARibbonCategoryContainerExtension::addWidget(QWidget *widget)
{
    if (widget) {
        SA_PLUGIN_LOG("widget class name:%s", widget->metaObject()->className());
    }else{
        SA_PLUGIN_LOG("widget nullptr");
    }
    if (SARibbonPannel *pannel = qobject_cast<SARibbonPannel *>(widget)) {
        if (pannel->pannelName().isEmpty()) {
            QString title = QObject::tr("pannel %1").arg(count());
            pannel->setPannelName(title);
        }
        m_category->addPannel(pannel);
        pannel->show();
        QDesignerFormWindowInterface::findFormWindow(m_category)->manageWidget(pannel);
    }
}


int SARibbonCategoryContainerExtension::count() const
{
    return (m_category->pannelCount());
}


int SARibbonCategoryContainerExtension::currentIndex() const
{
    return (m_category->pannelCount() > 0 ? 0 : -1);
}


void SARibbonCategoryContainerExtension::insertWidget(int index, QWidget *widget)
{
    Q_UNUSED(index);
    Q_UNUSED(widget);
}


void SARibbonCategoryContainerExtension::remove(int index)
{
    m_category->removePannel(index);
}


void SARibbonCategoryContainerExtension::setCurrentIndex(int index)
{
    Q_UNUSED(index);
}


QWidget *SARibbonCategoryContainerExtension::widget(int index) const
{
    return (m_category->pannelByIndex(index));
}
