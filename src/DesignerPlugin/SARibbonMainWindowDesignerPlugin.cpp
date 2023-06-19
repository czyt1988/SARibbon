#include "SARibbonMainWindowDesignerPlugin.h"
#include "SARibbonMainWindow.h"
#include "SARibbonBar.h"
#include "SARibbonCategory.h"
#include "SARibbonPannel.h"
#include <QtPlugin>
#include <QDesignerFormEditorInterface>
#include <QDesignerFormWindowInterface>
#include <QDesignerFormWindowCursorInterface>
#include <QExtensionManager>
#include <QMessageBox>
//
#include "SARibbonPluginDebugHelper.h"
#include "SARibbonMainWindowContainerExtension.h"
#include "SARibbonMainWindowContainerFactory.h"
#include "SARibbonMainWindowTaskMenuFactory.h"
using namespace SA_PLUGIN;
SARibbonMainWindowDesignerPlugin::SARibbonMainWindowDesignerPlugin(QObject *p) : QObject(p)
    , m_isInitialized(false)
    , m_formEditor(nullptr)
{
}


/**
 * @brief 主窗口允许其他子窗口放置
 * @return 返回true
 */
bool SARibbonMainWindowDesignerPlugin::isContainer() const
{
    return (true);
}


bool SARibbonMainWindowDesignerPlugin::isInitialized() const
{
    return (m_isInitialized);
}


QIcon SARibbonMainWindowDesignerPlugin::icon() const
{
    return (QPixmap(":/icon/icon/saribbonmainwindow.svg"));
}


QString SARibbonMainWindowDesignerPlugin::domXml() const
{
    SA_PLUGIN_MARK();
    return ("<ui language=\"c++\" displayname=\"SA Ribbon MainWindow\">\n"
           " <widget class=\"SARibbonMainWindow\" name=\"MainWindow\">\n"
           "  <property name=\"geometry\">\n"
           "   <rect>\n"
           "    <x>0</x>\n"
           "    <y>0</y>\n"
           "    <width>800</width>\n"
           "    <height>500</height>\n"
           "   </rect>\n"
           "  </property>\n"
           "  <widget class=\"QWidget\" name=\"centralwidget\"/>\n"
           " </widget>\n"
           "</ui>\n");
}


QString SARibbonMainWindowDesignerPlugin::group() const
{
    return (QStringLiteral("SA Ribbon"));
}


QString SARibbonMainWindowDesignerPlugin::includeFile() const
{
    return ("SARibbonMainWindow.h");
}


QString SARibbonMainWindowDesignerPlugin::name() const
{
    return (QStringLiteral("SARibbonMainWindow"));
}


QString SARibbonMainWindowDesignerPlugin::toolTip() const
{
    return (tr("Ribbon MainWindow"));
}


QString SARibbonMainWindowDesignerPlugin::whatsThis() const
{
    return (tr("Ribbon MainWindow"));
}


QWidget *SARibbonMainWindowDesignerPlugin::createWidget(QWidget *parent)
{
    if (parent) {
        SA_PLUGIN_LOG("createWidget parent class:%s", parent->metaObject()->className());
    }else {
        SA_PLUGIN_LOG("createWidget parent is nullptr");
    }
    SARibbonMainWindow *w = new SARibbonMainWindow(parent, false);

    w->setObjectName(QStringLiteral("SARibbonMainWindow"));
    w->setWindowTitle(QStringLiteral("SARibbonMainWindow"));
    w->setAcceptDrops(true);
    w->setMouseTracking(true);
    w->setRibbonTheme(SARibbonMainWindow::Office2013);
//    SARibbonBar *bar = w->ribbonBar();
//    SARibbonCategory *category = bar->addCategoryPage(tr("Main"));
//    SARibbonPannel *pannel = category->addPannel(tr("pannel 1"));
//    QAction *act = new QAction(w);

//    act->setText(tr("about"));
//    act->setIcon(QIcon(":/icon/icon/about.svg"));
//    act->connect(act, &QAction::triggered, w, [w](bool checked) {
//        Q_UNUSED(checked);
//        QMessageBox::information(w, tr("about")
//        , tr("SARibbon\n"
//        "https://github.com/czyt1988/SARibbon"));
//    });
//    pannel->addLargeAction(act);
    return (w);
}


void SARibbonMainWindowDesignerPlugin::initialize(QDesignerFormEditorInterface *core)
{
    SA_PLUGIN_MARK();
    if (m_isInitialized) {
        return;
    }
    QExtensionManager *mgr = core->extensionManager();

    if (mgr) {
        //注册窗口容器，可以实现子窗口的创建
        mgr->registerExtensions(new SARibbonMainWindowContainerFactory(mgr)
            , Q_TYPEID(QDesignerContainerExtension));
        //注册右键菜单
        mgr->registerExtensions(new SARibbonMainWindowTaskMenuFactory(mgr)
            , Q_TYPEID(QDesignerTaskMenuExtension));
    }
    m_formEditor = core;
    m_isInitialized = true;
}
