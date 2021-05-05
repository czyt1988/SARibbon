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
#include "SARibbonMainWindowContainerExtension.h"
#include "SARibbonMainWindowContainerFactory.h"
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
    return (QIcon(":/icon/icon/saribbonmainwindow.svg"));
}


QString SARibbonMainWindowDesignerPlugin::domXml() const
{
    return ("<ui language=\"c++\">\n"
           " <widget class=\"SARibbonMainWindow\" name=\"MainWindow\">\n"
           "  <property name=\"geometry\">\n"
           "   <rect>\n"
           "    <x>0</x>\n"
           "    <y>0</y>\n"
           "    <width>800</width>\n"
           "    <height>500</height>\n"
           "   </rect>\n"
           "  </property>\n"
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
    return (tr("Ribbon MainWindow"));
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
    SARibbonMainWindow *w = new SARibbonMainWindow(parent);

    w->setObjectName(QStringLiteral("SARibbonMainWindow"));
    w->setWindowTitle(QStringLiteral("SARibbonMainWindow"));
    SARibbonBar *bar = w->ribbonBar();
    SARibbonCategory *category = bar->addCategoryPage(tr("Main"));
    SARibbonPannel *pannel = category->addPannel(tr("pannel 1"));
    QAction *act = new QAction(w);

    act->setText(tr("abiut"));
    act->setIcon(QIcon(":/icon/icon/about.svg"));
    act->connect(act, &QAction::triggered, w, [w](bool checked) {
        Q_UNUSED(checked);
        QMessageBox::information(w, tr("about")
        , tr("SARibbon\n"
        "https://github.com/czyt1988/SARibbon"));
    });
    pannel->addLargeAction(act);
    return (w);
}


void SARibbonMainWindowDesignerPlugin::initialize(QDesignerFormEditorInterface *core)
{
    if (m_isInitialized) {
        return;
    }
    QExtensionManager *mgr = core->extensionManager();

    if (mgr) {
        mgr->registerExtensions(new SARibbonMainWindowContainerFactory(mgr)
            , Q_TYPEID(QDesignerContainerExtension));
    }
    m_formEditor = core;
    m_isInitialized = true;
}
