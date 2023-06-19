#include "SARibbonCategoryDesignerPlugin.h"
#include "SARibbonMainWindow.h"
#include "SARibbonBar.h"
#include <QExtensionManager>
#include <QDesignerFormEditorInterface>
#include <QDesignerFormWindowManagerInterface>
#include <QExtensionManager>
#include <QDesignerContainerExtension>
#include <QMessageBox>
#include <QDesignerMetaDataBaseInterface>
#include <QDesignerPropertySheetExtension>
#include "SARibbonCategoryContainerFactory.h"
#include "SARibbonPluginDebugHelper.h"
#include "SARibbonCategory.h"
#include "SARibbonBarTaskMenuFactory.h"
using namespace SA_PLUGIN;
SARibbonCategoryDesignerPlugin::SARibbonCategoryDesignerPlugin(QObject *p)
    : QObject(p)
    , m_isInitialized(false)
{
}


bool SARibbonCategoryDesignerPlugin::isContainer() const
{
    return (false);
}


bool SARibbonCategoryDesignerPlugin::isInitialized() const
{
    return (m_isInitialized);
}


QIcon SARibbonCategoryDesignerPlugin::icon() const
{
    return (QIcon(":/icon/icon/addcategory.svg"));
}


QString SARibbonCategoryDesignerPlugin::domXml() const
{
    return ("<ui language=\"c++\" displayname=\"Ribbon Category Page\">\n"
           " <widget class=\"SARibbonCategory\" name=\"ribbonCategoryPage\"/>\n"
           " <customwidgets>\n"
           "   <customwidget>\n"
           "       <class>SARibbonCategory</class>\n"
           "       <extends>QWidget</extends>\n"
           "       <addpagemethod>addPannel</addpagemethod>\n"
           "   </customwidget>\n"
           " </customwidgets>\n"
           "</ui>\n");
}


QString SARibbonCategoryDesignerPlugin::group() const
{
    return (QStringLiteral("SA Ribbon"));
}


QString SARibbonCategoryDesignerPlugin::includeFile() const
{
    return ("SARibbonCategory.h");
}


QString SARibbonCategoryDesignerPlugin::name() const
{
    return (QStringLiteral("SARibbonCategory"));
}


QString SARibbonCategoryDesignerPlugin::toolTip() const
{
    return (tr("RibbonCategory For Ribbon Bar"));
}


QString SARibbonCategoryDesignerPlugin::whatsThis() const
{
    return (tr("RibbonCategory For Ribbon Bar"));
}


QWidget *SARibbonCategoryDesignerPlugin::createWidget(QWidget *parent)
{
    if (parent) {
        SA_PLUGIN_LOG("createWidget parent class:%s", parent->metaObject()->className());
    }else{
        SA_PLUGIN_LOG("createWidget parent is nullptr");
    }
    SARibbonCategory *category = new SARibbonCategory(parent);

    connect(category, &SARibbonCategory::windowTitleChanged
        , this, &SARibbonCategoryDesignerPlugin::onWindowTitleChanged);
    return (category);
}


void SARibbonCategoryDesignerPlugin::initialize(QDesignerFormEditorInterface *core)
{
    if (m_isInitialized) {
        return;
    }
    QExtensionManager *mgr = core->extensionManager();

    if (mgr) {
        mgr->registerExtensions(new SARibbonCategoryContainerFactory(mgr)
            , Q_TYPEID(QDesignerContainerExtension));
        mgr->registerExtensions(new SARibbonBarTaskMenuFactory(mgr)
            , Q_TYPEID(QDesignerTaskMenuExtension));
    }

    m_isInitialized = true;
}


void SARibbonCategoryDesignerPlugin::onWindowTitleChanged(const QString& title)
{
    Q_UNUSED(title);
    if (SARibbonCategory *ribbonPage = qobject_cast<SARibbonCategory *>(sender())) {
        if (QDesignerFormWindowInterface *form = QDesignerFormWindowInterface::findFormWindow(ribbonPage)) {
            QDesignerFormEditorInterface *editor = form->core();
            QExtensionManager *manager = editor->extensionManager();
            QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension *>(manager, ribbonPage);
            const int propertyIndex = sheet->indexOf(QLatin1String("categoryName"));
            if (propertyIndex >= 0) {
                sheet->setChanged(propertyIndex, true);
            }
        }
    }
}
