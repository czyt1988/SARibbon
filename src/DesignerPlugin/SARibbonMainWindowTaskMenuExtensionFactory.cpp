#include "SARibbonMainWindowTaskMenuExtensionFactory.h"
#include "SARibbonMainWindowTaskMenuExtension.h"
#include "SARibbonMainWindow.h"
using namespace SA_PLUGIN;
SARibbonMainWindowTaskMenuExtensionFactory::SARibbonMainWindowTaskMenuExtensionFactory(QExtensionManager *mgr)
    : QExtensionFactory(mgr)
{
}


QObject *SARibbonMainWindowTaskMenuExtensionFactory::createExtension(QObject *obj, const QString& iid, QObject *p) const
{
    SARibbonMainWindow *widget = qobject_cast<SARibbonMainWindow *>(obj);

    if (widget && (iid == Q_TYPEID(QDesignerTaskMenuExtension))) {
        return (new SARibbonMainWindowTaskMenuExtension(widget, p));
    }else{
        return (nullptr);
    }
}
