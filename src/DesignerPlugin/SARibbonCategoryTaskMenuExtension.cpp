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


QDesignerFormWindowInterface *SARibbonCategoryTaskMenuExtension::formWindowInterface() const
{
    QDesignerFormWindowInterface *result = QDesignerFormWindowInterface::findFormWindow(m_widget);

    return (result);
}


QDesignerFormEditorInterface *SARibbonCategoryTaskMenuExtension::core() const
{
    QDesignerFormWindowInterface *fw = formWindowInterface();

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
    QDesignerFormWindowInterface *fw = formWindowInterface();

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
    QDesignerContainerExtension *c = qt_extension<QDesignerContainerExtension *>(core->extensionManager(), category);
    SARibbonPannel *pannel = qobject_cast<SARibbonPannel *>(core->widgetFactory()->createWidget("SARibbonPannel", category));

    core->widgetFactory()->initialize(pannel);
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

    QDesignerFormWindowInterface *fw = formWindowInterface();

    if (nullptr == fw) {
        SA_PLUGIN_LOG("null formWindowInterface");
        return;
    }
    QDesignerFormEditorInterface *core = fw->core();

    if (nullptr == core) {
        SA_PLUGIN_LOG("core is null");
        return;
    }
    core->metaDataBase()->remove(category);
    QDesignerContainerExtension *c = qt_extension<QDesignerContainerExtension *>(core->extensionManager(), category->parent());

    for (int i = 0; i < c->count(); ++i)
    {
        if (c->widget(i) == category) {
            SA_PLUGIN_LOG("category success remove");
            c->remove(i);
            break;
        }
    }
    fw->emitSelectionChanged();
}
