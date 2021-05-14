#include "SARibbonBarTaskMenuExtensionFactory.h"
#include "SARibbonBarTaskMenuExtension.h"
#include "SARibbonMainWindow.h"
#include "SARibbonBar.h"
#include "SARibbonPluginDebugHelper.h"
using namespace SA_PLUGIN;
SARibbonBarTaskMenuExtensionFactory::SARibbonBarTaskMenuExtensionFactory(QExtensionManager *mgr)
    : QExtensionFactory(mgr)
{
}


QObject *SARibbonBarTaskMenuExtensionFactory::createExtension(QObject *obj, const QString& iid, QObject *p) const
{
    if (obj) {
        SA_PLUGIN_LOG("class name:%s", obj->metaObject()->className());
    }
    if (iid == Q_TYPEID(QDesignerTaskMenuExtension)) {
        if (SARibbonBar *widget = qobject_cast<SARibbonBar *>(obj)) {
            //在ribbonbar上点击
            return (new SARibbonBarTaskMenuExtension(widget, p));
        }
    }

    return (nullptr);
}
