#include "SARibbonPluginCollection.h"
#include "SARibbonMainWindowDesignerPlugin.h"
#include "SARibbonBarDesignerPlugin.h"
using namespace SA_PLUGIN;
SARibbonPluginCollection::SARibbonPluginCollection(QObject *p) : QObject(p)
{
    m_widgets.append(new SARibbonMainWindowDesignerPlugin(this));
    m_widgets.append(new SARibbonBarDesignerPlugin(this));
}


QList<QDesignerCustomWidgetInterface *> SARibbonPluginCollection::customWidgets() const
{
    return (m_widgets);
}
