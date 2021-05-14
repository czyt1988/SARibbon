#include "SARibbonBarTaskMenuExtension.h"
#include <QAction>
#include <QDesignerFormWindowInterface>
#include <QDesignerFormEditorInterface>
#include <QDesignerWidgetFactoryInterface>
#include <QDesignerContainerExtension>
#include <QDesignerMetaDataBaseInterface>
#include <QExtensionManager>
#include "SARibbonMainWindow.h"
#include "SARibbonBar.h"
#include "SARibbonCategory.h"
#include "SARibbonPluginDebugHelper.h"
using namespace SA_PLUGIN;
SARibbonBarTaskMenuExtension::SARibbonBarTaskMenuExtension(QWidget *w, QObject *p)
    : QObject(p)
    , m_widget(w)
{
    initActions();
}


QList<QAction *> SARibbonBarTaskMenuExtension::taskActions() const
{
    return (m_actions);
}


void SARibbonBarTaskMenuExtension::initActions()
{
    QAction *addCategory = new QAction(tr("add category"), this);

    addCategory->setObjectName(QStringLiteral("add_category"));
    addCategory->setIcon(QIcon(":/icon/icon/addcategory.svg"));
    connect(addCategory, &QAction::triggered, this, &SARibbonBarTaskMenuExtension::onAddCategory);

    m_actions.append(addCategory);
}


QDesignerFormWindowInterface *SARibbonBarTaskMenuExtension::formWindowInterface() const
{
    QDesignerFormWindowInterface *result = QDesignerFormWindowInterface::findFormWindow(m_widget);

    return (result);
}


QDesignerFormEditorInterface *SARibbonBarTaskMenuExtension::core() const
{
    QDesignerFormWindowInterface *fw = formWindowInterface();

    if (nullptr == fw) {
        return (nullptr);
    }
    return (fw->core());
}


void SARibbonBarTaskMenuExtension::onAddCategory()
{
    SA_PLUGIN_MARK();
    SARibbonBar *bar = qobject_cast<SARibbonBar *>(m_widget);

    if (nullptr == bar) {
        SA_PLUGIN_LOG("null SARibbonBar");
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
    SA_PLUGIN_LOG("start add category");
    //在QMainWindow下创建SARibbonBar，将调用
    QDesignerContainerExtension *c = qt_extension<QDesignerContainerExtension *>(core->extensionManager(), bar);

    //添加bar，这时候会触发SARibbonMainWindowContainerExtension::addWidget(QWidget *widget)函数
    SARibbonCategory *category = qobject_cast<SARibbonCategory *>(core->widgetFactory()->createWidget("SARibbonCategory", bar));

    core->widgetFactory()->initialize(category);
    c->addWidget(category);
    category->setObjectName(QLatin1String("ribbonCategory"));
    fw->ensureUniqueObjectName(category);
    core->metaDataBase()->add(category);
    fw->emitSelectionChanged();
    category->setFocus();
}


void SARibbonBarTaskMenuExtension::onAddContextCategory()
{
}
