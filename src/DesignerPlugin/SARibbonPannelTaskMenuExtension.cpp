#include "SARibbonPannelTaskMenuExtension.h"
#include <QAction>
#include <QDesignerFormWindowInterface>
#include <QDesignerFormEditorInterface>
#include <QDesignerWidgetFactoryInterface>
#include <QDesignerContainerExtension>
#include <QDesignerMetaDataBaseInterface>
#include <QExtensionManager>
#include <QDesignerPropertyEditorInterface>
#include <QDesignerActionEditorInterface>
#include <QDesignerDynamicPropertySheetExtension>
#include <QDesignerIntegrationInterface>
#include "SARibbonBar.h"
#include "SARibbonCategory.h"
#include "SARibbonPannel.h"
#include "SARibbonPluginDebugHelper.h"
#include "SARibbonBarCommands.h"
using namespace SA_PLUGIN;
SARibbonPannelTaskMenuExtension::SARibbonPannelTaskMenuExtension(QWidget *w, QObject *p)
    : QObject(p)
    , m_widget(w)
{
    initActions();
}


QList<QAction *> SARibbonPannelTaskMenuExtension::taskActions() const
{
    QAction *addMediumAction = m_actions[1];
    SARibbonPannel *p = pannel();

    addMediumAction->setDisabled(p->pannelLayoutMode() == SARibbonPannel::TwoRowMode);
    return (m_actions);
}


SARibbonPannel *SARibbonPannelTaskMenuExtension::pannel() const
{
    return (qobject_cast<SARibbonPannel *>(m_widget));
}


void SARibbonPannelTaskMenuExtension::initActions()
{
    QAction *addLargeAction = new QAction(tr("Add Large action"), this);

    addLargeAction->setObjectName(QStringLiteral("addLargeAction"));
    connect(addLargeAction, &QAction::triggered, this, &SARibbonPannelTaskMenuExtension::onAddLargeAction);

    QAction *addSmallAction = new QAction(tr("Add Small action"), this);

    addSmallAction->setObjectName(QStringLiteral("addSmallAction"));
    connect(addSmallAction, &QAction::triggered, this, &SARibbonPannelTaskMenuExtension::onAddSmallAction);

    QAction *addMediumAction = new QAction(tr("Add Medium action"), this);

    addMediumAction->setObjectName(QStringLiteral("addMediumAction"));
    connect(addMediumAction, &QAction::triggered, this, &SARibbonPannelTaskMenuExtension::onAddMediumAction);

    m_actions.append(addLargeAction);
    m_actions.append(addMediumAction);
    m_actions.append(addSmallAction);
}


QDesignerFormWindowInterface *SARibbonPannelTaskMenuExtension::ribbonPannelFormWindowInterface() const
{
    return (QDesignerFormWindowInterface::findFormWindow(m_widget));
}


QDesignerFormEditorInterface *SARibbonPannelTaskMenuExtension::ribbonPannelCore() const
{
    QDesignerFormWindowInterface *fw = ribbonPannelFormWindowInterface();

    if (nullptr == fw) {
        return (nullptr);
    }
    return (fw->core());
}


QDesignerFormWindowInterface *SARibbonPannelTaskMenuExtension::ribbonCategoryFormWindowInterface() const
{
    SARibbonCategory *c = qobject_cast<SARibbonCategory *>(m_widget->parent());

    if ((nullptr == c) || (nullptr == c->ribbonBar())) {
        return (nullptr);
    }
    return (QDesignerFormWindowInterface::findFormWindow(c));
}


QDesignerFormEditorInterface *SARibbonPannelTaskMenuExtension::ribbonCategoryCore() const
{
    QDesignerFormWindowInterface *fw = ribbonCategoryFormWindowInterface();

    if (nullptr == fw) {
        return (nullptr);
    }
    return (fw->core());
}


QDesignerFormWindowInterface *SARibbonPannelTaskMenuExtension::ribbonBarFormWindowInterface() const
{
    SARibbonCategory *c = qobject_cast<SARibbonCategory *>(m_widget->parent());

    if ((nullptr == c) || (nullptr == c->ribbonBar())) {
        return (nullptr);
    }
    return (QDesignerFormWindowInterface::findFormWindow(c->ribbonBar()));
}


QDesignerFormEditorInterface *SARibbonPannelTaskMenuExtension::ribbonBarCore() const
{
    QDesignerFormWindowInterface *fw = ribbonBarFormWindowInterface();

    if (nullptr == fw) {
        return (nullptr);
    }
    return (fw->core());
}


QAction *SARibbonPannelTaskMenuExtension::createAction(QDesignerFormWindowInterface *fw, const QString& objectName, bool separator)
{
    SA_PLUGIN_MARK();
    QAction *action = new QAction(fw);

    fw->core()->widgetFactory()->initialize(action);
    if (separator) {
        action->setSeparator(true);
    }

    action->setObjectName(objectName);
    action->setText(objectName);
    fw->ensureUniqueObjectName(action);

    SARibbonAddActionCommand *cmd = new  SARibbonAddActionCommand(fw, action);

    fw->commandHistory()->push(cmd);
    //把动态属性告诉designer，让designer添加动态属性


    return (action);
}


void SARibbonPannelTaskMenuExtension::addActionRowProportionDynamicProperty(QAction *act, int rowProportion)
{
    SA_PLUGIN_MARK();
    SARibbonPannel::setActionRowProportionProperty(act, static_cast<SARibbonPannelItem::RowProportion>(rowProportion));
    QDesignerFormWindowInterface *afw = QDesignerFormWindowInterface::findFormWindow(act);

    if (nullptr == afw) {
        SA_PLUGIN_LOG("can not find act QDesignerFormWindowInterface");
        return;
    }
    QDesignerIntegrationInterface *intf = afw->core()->integration();

    if (nullptr == intf) {
        SA_PLUGIN_LOG("can not find QDesignerIntegrationInterface");
        return;
    }
    SA_PLUGIN_LOG("addDynamicProperty");

    intf->addDynamicProperty(SARibbonPannelItemRowProportionPropertyName, rowProportion);
}


void SARibbonPannelTaskMenuExtension::addRibbonAction(int actionType)
{
    SA_PLUGIN_MARK();
    QDesignerFormWindowInterface *fw = ribbonPannelFormWindowInterface();

    if (fw == nullptr) {
        SA_PLUGIN_LOG("QDesignerFormWindowInterface::findFormWindow is nullptr,m_widget is %d", m_widget);
    }
    fw->beginCommand(tr("Add Action"));
    QAction *action = createAction(fw, QStringLiteral("action"), false);

    addActionRowProportionDynamicProperty(action, actionType);
    SARibbonPannelInsertActionCommand *cmd = new SARibbonPannelInsertActionCommand(QStringLiteral("insert action"),
        fw);
    QAction *previous = nullptr;

    cmd->init(m_widget, action, previous);
    fw->commandHistory()->push(cmd);
    fw->endCommand();
}


void SARibbonPannelTaskMenuExtension::onAddLargeAction()
{
    SA_PLUGIN_MARK();
    addRibbonAction(SARibbonPannelItem::Large);
}


void SARibbonPannelTaskMenuExtension::onAddSmallAction()
{
    SA_PLUGIN_MARK();
    addRibbonAction(SARibbonPannelItem::Small);
}


void SARibbonPannelTaskMenuExtension::onAddMediumAction()
{
    SA_PLUGIN_MARK();
    addRibbonAction(SARibbonPannelItem::Medium);
}
