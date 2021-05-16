#include "SARibbonBarContainerExtension.h"
#include "SARibbonPluginDebugHelper.h"
#include "SARibbonBar.h"
#include "SARibbonCategory.h"
#include <QDesignerFormWindowInterface>
using namespace SA_PLUGIN;
SARibbonBarContainerExtension::SARibbonBarContainerExtension(SARibbonBar *bar, QObject *p)
    : QObject(p)
    , m_ribbonbar(bar)
{
}


void SARibbonBarContainerExtension::addWidget(QWidget *widget)
{
    if (widget) {
        SA_PLUGIN_LOG("widget class name:%s", widget->metaObject()->className());
    }else{
        SA_PLUGIN_LOG("widget nullptr");
    }
    if (SARibbonCategory *category = qobject_cast<SARibbonCategory *>(widget)) {
        if (category->windowTitle().isEmpty()) {
            QString title = QObject::tr("Category %1").arg(count());
            category->setCategoryName(title);
        }
        m_ribbonbar->addCategoryPage(category);
        QDesignerFormWindowInterface::findFormWindow(m_ribbonbar)->manageWidget(category);
        m_ribbonbar->setCurrentIndex(m_ribbonbar->categoryPages().size()-1);
    }
}


int SARibbonBarContainerExtension::count() const
{
    return (m_ribbonbar->categoryPages().size());
}


int SARibbonBarContainerExtension::currentIndex() const
{
    return (m_ribbonbar->currentIndex());
}


void SARibbonBarContainerExtension::insertWidget(int index, QWidget *widget)
{
    Q_UNUSED(index);
    Q_UNUSED(widget);
}


void SARibbonBarContainerExtension::remove(int index)
{
    SA_PLUGIN_LOG("remove category:%d", index);
    SARibbonCategory *category = m_ribbonbar->categoryByIndex(index);

    if (category) {
        m_ribbonbar->removeCategory(category);
        category->hide();
        //这里不能调用category.deletelater,会奔溃
        QDesignerFormWindowInterface *fw = QDesignerFormWindowInterface::findFormWindow(m_ribbonbar);
        category->setParent(fw);
    }
}


void SARibbonBarContainerExtension::setCurrentIndex(int index)
{
    m_ribbonbar->setCurrentIndex(index);
}


QWidget *SARibbonBarContainerExtension::widget(int index) const
{
    return (m_ribbonbar->categoryByIndex(index));
}
