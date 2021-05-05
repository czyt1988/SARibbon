#include "SARibbonMainWindowContainerFactory.h"
#include "SARibbonMainWindow.h"
#include "SARibbonMainWindowContainerExtension.h"
using namespace SA_PLUGIN;
SARibbonMainWindowContainerFactory::SARibbonMainWindowContainerFactory(QExtensionManager *p) : QExtensionFactory(p)
{
}


QObject *SARibbonMainWindowContainerFactory::createExtension(QObject *object, const QString& iid, QObject *parent) const
{
    if (iid != Q_TYPEID(QDesignerContainerExtension)) {
        return (nullptr);
    }

    if (SARibbonMainWindow *w = qobject_cast<SARibbonMainWindow *>(object)) {
        return (new SARibbonMainWindowContainerExtension(w, parent));
    }

    return (nullptr);
}
