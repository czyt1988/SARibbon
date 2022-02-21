#include "SARibbonBarTaskMenuFactory.h"
#include "SARibbonBarTaskMenuExtension.h"
#include "SARibbonMainWindow.h"
#include "SARibbonBar.h"
#include "SARibbonPluginDebugHelper.h"
#include "SARibbonCategoryTaskMenuExtension.h"
#include "SARibbonPannelTaskMenuExtension.h"
#include "SARibbonPannel.h"
using namespace SA_PLUGIN;
SARibbonBarTaskMenuFactory::SARibbonBarTaskMenuFactory(QExtensionManager *mgr)
    : QExtensionFactory(mgr)
{
}


QObject *SARibbonBarTaskMenuFactory::createExtension(QObject *obj, const QString& iid, QObject *p) const
{
    if (obj) {
        SA_PLUGIN_LOG("class name:%s", obj->metaObject()->className());
    }
    if (iid == Q_TYPEID(QDesignerTaskMenuExtension)) {
        if (SARibbonBar *bar = qobject_cast<SARibbonBar *>(obj)) {
            //在ribbonbar上点击
            return (new SARibbonBarTaskMenuExtension(bar, p));
        }else if (SARibbonCategory *category = qobject_cast<SARibbonCategory *>(obj)) {
            //在SARibbonCategory上点击
            return (new SARibbonCategoryTaskMenuExtension(category, p));
        }else if (SARibbonPannel *pannel = qobject_cast<SARibbonPannel *>(obj)) {
            //在SARibbonPannel上点击
            return (new SARibbonPannelTaskMenuExtension(pannel, p));
        }
    }

    return (nullptr);
}
