#include "SARibbonMainWindowTaskMenuFactory.h"
#include "SARibbonMainWindowTaskMenuExtension.h"
#include "SARibbonBarTaskMenuExtension.h"
#include "SARibbonMainWindow.h"
#include "SARibbonBar.h"
using namespace SA_PLUGIN;
SARibbonMainWindowTaskMenuFactory::SARibbonMainWindowTaskMenuFactory(QExtensionManager *mgr)
    : QExtensionFactory(mgr)
{
}


QObject *SARibbonMainWindowTaskMenuFactory::createExtension(QObject *obj, const QString& iid, QObject *p) const
{
    if (iid == Q_TYPEID(QDesignerTaskMenuExtension)) {
        if (SARibbonMainWindow *widget = qobject_cast<SARibbonMainWindow *>(obj)) {
            //在SARibbonMainWindow上触发
            return (new SARibbonMainWindowTaskMenuExtension(widget, p));
        }
//        else if (SARibbonBar *widget = qobject_cast<SARibbonBar *>(obj)) {
//            //在ribbonbar上点击
//            return (new SARibbonBarTaskMenuExtension(widget, p));
//        }
    }

    return (nullptr);
}
