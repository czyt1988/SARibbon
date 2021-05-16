#include "SARibbonPannelDesignerPlugin.h"
#include "SARibbonPannel.h"
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
    return ("<ui language=\"c++\">\n"
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

    pannel->setAcceptDrops(true);
    return (pannel);
}


void SARibbonPannelDesignerPlugin::initialize(QDesignerFormEditorInterface *core)
{
    if (m_isInitialized) {
        return;
    }

    m_isInitialized = true;
    m_formEditor = core;
}
