#include "SARibbonBarContainerFactory.h"
#include "SARibbonBarContainerExtension.h"
#include "SARibbonBar.h"
using namespace SA_PLUGIN;
SARibbonBarContainerFactory::SARibbonBarContainerFactory(QExtensionManager *p) : QExtensionFactory(p)
{
}


QObject *SARibbonBarContainerFactory::createExtension(QObject *object, const QString& iid, QObject *parent) const
{
    if (iid != Q_TYPEID(QDesignerContainerExtension)) {
        return (nullptr);
    }

    if (SARibbonBar *w = qobject_cast<SARibbonBar *>(object)) {
        return (new SARibbonBarContainerExtension(w, parent));
    }

    return (nullptr);
}
