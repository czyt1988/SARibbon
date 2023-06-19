#include "SARibbonPannelDesignerPlugin.h"
#include <QExtensionManager>
#include "SARibbonPannel.h"
#include "SARibbonBarTaskMenuFactory.h"
#include <QDesignerFormEditorInterface>
using namespace SA_PLUGIN;
SARibbonPannelDesignerPlugin::SARibbonPannelDesignerPlugin(QObject *p)
    : QObject(p)
    , m_isInitialized(false)
{
}


bool SARibbonPannelDesignerPlugin::isContainer() const
{
    return (false);
}


bool SARibbonPannelDesignerPlugin::isInitialized() const
{
    return (m_isInitialized);
}


QIcon SARibbonPannelDesignerPlugin::icon() const
{
    return (QIcon(":/icon/icon/addcategory.svg"));
}


QString SARibbonPannelDesignerPlugin::domXml() const
{
    return ("<ui language=\"c++\" displayname=\"Ribbon Pannel\">\n"
           " <widget class=\"SARibbonPannel\" name=\"ribbonPannel\"/>\n"
           " <customwidgets>\n"
           "   <customwidget>\n"
           "       <class>SARibbonPannel</class>\n"
           "       <extends>QWidget</extends>\n"
           "   </customwidget>\n"
           " </customwidgets>\n"
           "</ui>\n");
}


QString SARibbonPannelDesignerPlugin::group() const
{
    return (QStringLiteral("SA Ribbon"));
}


QString SARibbonPannelDesignerPlugin::includeFile() const
{
    return ("SARibbonPannel.h");
}


QString SARibbonPannelDesignerPlugin::name() const
{
    return (QStringLiteral("SARibbonPannel"));
}


QString SARibbonPannelDesignerPlugin::toolTip() const
{
    return (tr("Pannel For Category"));
}


QString SARibbonPannelDesignerPlugin::whatsThis() const
{
    return (tr("Pannel For Category"));
}


QWidget *SARibbonPannelDesignerPlugin::createWidget(QWidget *parent)
{
    SARibbonPannel *pannel = new SARibbonPannel(parent);
    static int s_pannel_count = 0;

    pannel->setPannelName(QStringLiteral("pannel %1").arg(s_pannel_count));
//    pannel->setAcceptDrops(true);
    ++s_pannel_count;
    return (pannel);
}


void SARibbonPannelDesignerPlugin::initialize(QDesignerFormEditorInterface *core)
{
    if (m_isInitialized) {
        return;
    }
    QExtensionManager *mgr = core->extensionManager();

    if (mgr) {
        mgr->registerExtensions(new SARibbonBarTaskMenuFactory(mgr)
            , Q_TYPEID(QDesignerTaskMenuExtension));
    }
    m_isInitialized = true;
    m_formEditor = core;
}
