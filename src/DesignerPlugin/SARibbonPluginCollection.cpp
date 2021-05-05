#include "SARibbonPluginCollection.h"
#include "SARibbonMainWindowDesignerPlugin.h"
using namespace SA_PLUGIN;
SARibbonPluginCollection::SARibbonPluginCollection(QObject *p) : QObject(p)
{
    m_widgets.append(new SARibbonMainWindowDesignerPlugin(this));
}


QList<QDesignerCustomWidgetInterface *> SARibbonPluginCollection::customWidgets() const
{
    return (m_widgets);
}
