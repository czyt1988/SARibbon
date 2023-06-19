#include "SARibbonBarDesignerPlugin.h"
#include "SARibbonMainWindow.h"
#include "SARibbonBar.h"
#include <QExtensionManager>
#include <QDesignerFormEditorInterface>
#include <QDesignerFormWindowManagerInterface>
#include <QExtensionManager>
#include <QDesignerContainerExtension>
#include <QMessageBox>
#include <QDesignerMetaDataBaseInterface>
#include "SARibbonPluginDebugHelper.h"
#include "SARibbonBarContainerFactory.h"
#include "SARibbonBarTaskMenuFactory.h"
using namespace SA_PLUGIN;
SARibbonBarDesignerPlugin::SARibbonBarDesignerPlugin(QObject *p)
    : QObject(p)
    , m_isInitialized(false)
{
}


bool SARibbonBarDesignerPlugin::isContainer() const
{
    return (false);
}


bool SARibbonBarDesignerPlugin::isInitialized() const
{
    return (m_isInitialized);
}


QIcon SARibbonBarDesignerPlugin::icon() const
{
    return (QIcon(":/icon/icon/saribbonbar.svg"));
}


/**
 * @brief addCategoryPage这个是关键，制定了子窗口的调用方法
 * @return
 */
QString SARibbonBarDesignerPlugin::domXml() const
{
    return (QString("<ui language=\"c++\">\n"
           " <widget class=\"%1\" name=\"ribbonbar\"/>\n"
           " <customwidgets>\n"
           "   <customwidget>\n"
           "       <class>SARibbonBar</class>\n"
           "       <extends>QMenuBar</extends>\n"
           "       <addpagemethod>addCategoryPage</addpagemethod>\n"
           "   </customwidget>\n"
           " </customwidgets>\n"
           "</ui>\n").arg(name()));
}


QString SARibbonBarDesignerPlugin::group() const
{
    return (QStringLiteral("SA Ribbon"));
}


QString SARibbonBarDesignerPlugin::includeFile() const
{
    return ("SARibbonBar.h");
}


QString SARibbonBarDesignerPlugin::name() const
{
    return (QStringLiteral("SARibbonBar"));
}


QString SARibbonBarDesignerPlugin::toolTip() const
{
    return (tr("Ribbon Bar"));
}


QString SARibbonBarDesignerPlugin::whatsThis() const
{
    return (tr("Ribbon Bar"));
}


QWidget *SARibbonBarDesignerPlugin::createWidget(QWidget *parent)
{
    if (parent) {
        SA_PLUGIN_LOG("createWidget parent class:%s", parent->metaObject()->className());
    }else{
        SA_PLUGIN_LOG("createWidget parent is nullptr");
    }
    SARibbonBar *bar = new SARibbonBar(parent);

    bar->setAcceptDrops(true);
    return (bar);
}


void SARibbonBarDesignerPlugin::initialize(QDesignerFormEditorInterface *core)
{
    if (m_isInitialized) {
        return;
    }
    QExtensionManager *mgr = core->extensionManager();

    if (mgr) {
        mgr->registerExtensions(new SARibbonBarContainerFactory(mgr)
            , Q_TYPEID(QDesignerContainerExtension));
        mgr->registerExtensions(new SARibbonBarTaskMenuFactory(mgr)
            , Q_TYPEID(QDesignerTaskMenuExtension));
    }
    m_formEditor = core;
    m_isInitialized = true;
    //此段非常关键，当用户拖曳SARibbonBar到mainwindow时，会触发QDesignerFormWindowManagerInterface::formWindowAdded
    //此时要判断是否能建立SARibbonBar
    QDesignerFormWindowManagerInterface *fw = core->formWindowManager();

    connect(fw, &QDesignerFormWindowManagerInterface::formWindowAdded,
        this, &SARibbonBarDesignerPlugin::onFormWindowAdded);
}


void SARibbonBarDesignerPlugin::onFormWindowAdded(QDesignerFormWindowInterface *formWindow)
{
    SA_PLUGIN_MARK();
    connect(formWindow, &QDesignerFormWindowInterface::widgetManaged
        , this, &SARibbonBarDesignerPlugin::onWidgetManaged);
}


void SARibbonBarDesignerPlugin::onFormWindowRemoved(QDesignerFormWindowInterface *formWindow)
{
    SA_PLUGIN_MARK();
    disconnect(formWindow, &QDesignerFormWindowInterface::widgetManaged
        , this, &SARibbonBarDesignerPlugin::onWidgetManaged);
}


/**
 * @brief 所有的窗体添加都会触发此槽函数
 * @param widget
 */
void SARibbonBarDesignerPlugin::onWidgetManaged(QWidget *widget)
{
    if (widget) {
        SA_PLUGIN_LOG("onWidgetManaged name:%s", widget->metaObject()->className());
    }else{
        SA_PLUGIN_LOG("onWidgetManaged nullptr widget");
    }
    if (widget->metaObject()->className() == QLatin1String("SARibbonBar")) {
        QDesignerFormWindowInterface *formWindow = static_cast<QDesignerFormWindowInterface *>(sender());
        QDesignerFormEditorInterface *core = formWindow->core();
        QDesignerContainerExtension *container = qt_extension<QDesignerContainerExtension *>(core->extensionManager(), formWindow->mainContainer());
        formWindow->unmanageWidget(widget);
        SARibbonMainWindow *mw = qobject_cast<SARibbonMainWindow *>(formWindow->mainContainer());
        if (mw == nullptr) {
            QMessageBox::critical(
                formWindow->mainContainer(),
                QObject::tr("Can't add Ribbon Bar"),
                QObject::tr("You can add Ribbon Bar onto SARibbonMainWindow only."));
            widget->deleteLater();
            return;
        }

//        for (int i = 0; i < container->count(); ++i)
//        {
//            QWidget *w = container->widget(i);
//            if (w->metaObject()->className() == QLatin1String("SARibbonBar")) {
//                QMessageBox::critical(
//                    formWindow->mainContainer(),
//                    QObject::tr("Can't add Ribbon Bar"),
//                    QObject::tr("Only one instance of the Ribbon Bar can be adding to the main form."));
//                widget->deleteLater();
//                return;
//            }
//        }
//        container->addWidget(widget);
//        formWindow->core()->metaDataBase()->add(widget);
    }
}
