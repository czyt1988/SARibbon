#include "SARibbonCategoryContainerFactory.h"
#include "SARibbonCategoryContainerExtension.h"
#include "SARibbonCategory.h"
using namespace SA_PLUGIN;
SARibbonCategoryContainerFactory::SARibbonCategoryContainerFactory(QExtensionManager *p)
    : QExtensionFactory(p)
{
}


QObject *SARibbonCategoryContainerFactory::createExtension(QObject *object, const QString& iid, QObject *parent) const
{
    if (iid != Q_TYPEID(QDesignerContainerExtension)) {
        return (nullptr);
    }

    if (SARibbonCategory *w = qobject_cast<SARibbonCategory *>(object)) {
        return (new SARibbonCategoryContainerExtension(w, parent));
    }

    return (nullptr);
}
