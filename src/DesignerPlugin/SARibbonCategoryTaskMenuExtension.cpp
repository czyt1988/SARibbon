#include "SARibbonCategoryTaskMenuExtension.h"
#include <QAction>
#include <QDesignerFormWindowInterface>
#include <QDesignerFormEditorInterface>
#include <QDesignerWidgetFactoryInterface>
#include <QDesignerContainerExtension>
#include <QDesignerMetaDataBaseInterface>
#include <QExtensionManager>
#include "SARibbonBar.h"
#include "SARibbonCategory.h"
#include "SARibbonPannel.h"
#include "SARibbonPluginDebugHelper.h"

using namespace SA_PLUGIN;
SARibbonCategoryTaskMenuExtension::SARibbonCategoryTaskMenuExtension(QWidget *w, QObject *p)
    : QObject(p)
    , m_widget(w)
{
    initActions();
}


QList<QAction *> SARibbonCategoryTaskMenuExtension::taskActions() const
{
    return (m_actions);
}


void SARibbonCategoryTaskMenuExtension::initActions()
{
    QAction *removecategory = new QAction(tr("remove category"), this);

    removecategory->setObjectName(QStringLiteral("remove_category"));
    removecategory->setIcon(QIcon(":/icon/icon/addcategory.svg"));
    connect(removecategory, &QAction::triggered, this, &SARibbonCategoryTaskMenuExtension::onRemoveCategory);

    QAction *addPannel = new QAction(tr("add pannel"), this);

    addPannel->setObjectName(QStringLiteral("add_pannel"));
    addPannel->setIcon(QIcon(":/icon/icon/addcategory.svg"));
    connect(addPannel, &QAction::triggered, this, &SARibbonCategoryTaskMenuExtension::onAddPannel);

    m_actions.append(removecategory);
    m_actions.append(addPannel);
}


QDesignerFormWindowInterface *SARibbonCategoryTaskMenuExtension::ribbonCategoryFormWindowInterface() const
{
    QDesignerFormWindowInterface *result = QDesignerFormWindowInterface::findFormWindow(m_widget);

    return (result);
}


QDesignerFormEditorInterface *SARibbonCategoryTaskMenuExtension::ribbonCategoryCore() const
{
    QDesignerFormWindowInterface *fw = ribbonCategoryFormWindowInterface();

    if (nullptr == fw) {
        return (nullptr);
    }
    return (fw->core());
}


QDesignerFormWindowInterface *SARibbonCategoryTaskMenuExtension::ribbonBarFormWindowInterface() const
{
    SARibbonCategory *c = qobject_cast<SARibbonCategory *>(m_widget);

    if ((nullptr == c) || (nullptr == c->ribbonBar())) {
        return (nullptr);
    }
    QDesignerFormWindowInterface *result = QDesignerFormWindowInterface::findFormWindow(c->ribbonBar());

    return (result);
}


QDesignerFormEditorInterface *SARibbonCategoryTaskMenuExtension::ribbonBarCore() const
{
    QDesignerFormWindowInterface *fw = ribbonBarFormWindowInterface();

    if (nullptr == fw) {
        return (nullptr);
    }
    return (fw->core());
}


void SARibbonCategoryTaskMenuExtension::onAddPannel()
{
    SA_PLUGIN_MARK();
    SARibbonCategory *category = qobject_cast<SARibbonCategory *>(m_widget);

    if (nullptr == category) {
        SA_PLUGIN_LOG("null SARibbonCategory");
        return;
    }
    QDesignerFormWindowInterface *fw = ribbonCategoryFormWindowInterface();

    if (nullptr == fw) {
        SA_PLUGIN_LOG("null formWindowInterface");
        return;
    }
    QDesignerFormEditorInterface *core = fw->core();

    if (nullptr == core) {
        SA_PLUGIN_LOG("core is null");
        return;
    }
    SA_PLUGIN_LOG("start add pannel");

    SARibbonPannel *pannel = qobject_cast<SARibbonPannel *>(core->widgetFactory()->createWidget("SARibbonPannel", category));

    core->widgetFactory()->initialize(pannel);

    QDesignerContainerExtension *c = qt_extension<QDesignerContainerExtension *>(core->extensionManager(), category);

    c->addWidget(pannel);
    pannel->setObjectName(QLatin1String("ribbonPannel"));
    fw->ensureUniqueObjectName(pannel);
    core->metaDataBase()->add(pannel);
    fw->emitSelectionChanged();
    pannel->setFocus();
}


void SARibbonCategoryTaskMenuExtension::onRemoveCategory()
{
    SA_PLUGIN_MARK();
    SARibbonCategory *category = qobject_cast<SARibbonCategory *>(m_widget);

    if (nullptr == category) {
        SA_PLUGIN_LOG("null SARibbonCategory");
        return;
    }
    SARibbonBar *bar = category->ribbonBar();

    if (nullptr == bar) {
        SA_PLUGIN_LOG("null SARibbonBar");
        return;
    }
    QDesignerFormWindowInterface *fw = ribbonBarFormWindowInterface();

    if (nullptr == fw) {
        SA_PLUGIN_LOG("null ribbonBarFormWindowInterface");
        return;
    }
    QDesignerFormEditorInterface *core = fw->core();

    if (nullptr == core) {
        SA_PLUGIN_LOG("core is null");
        return;
    }
    core->metaDataBase()->remove(m_widget);
    QDesignerContainerExtension *c = qt_extension<QDesignerContainerExtension *>(core->extensionManager(), bar);

    if (c) {
        for (int i = 0; i < c->count(); ++i)
        {
            if (c->widget(i) == m_widget) {
                c->remove(i);
                break;
            }
        }
        fw->emitSelectionChanged();
        //成功移除窗口后，需要在metaData里面移除
    }
}
