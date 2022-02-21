#include "SARibbonBarCommands.h"
#include <QApplication>
#include <QObject>
#include <QDesignerPropertySheetExtension>
#include <QDesignerFormEditorInterface>
#include <QDesignerObjectInspectorInterface>
#include <QDesignerActionEditorInterface>
#include <QExtensionManager>
#include <QLabel>
#include <QToolBar>
#include <QMenu>
#include "SARibbonPluginDebugHelper.h"
#include "SARibbonBar.h"
#ifndef TR
#define TR(str)    QObject::tr(str)
#endif
using namespace SA_PLUGIN;
SARibbonBarBaseDesignerCommand::SARibbonBarBaseDesignerCommand(const QString& description, QDesignerFormWindowInterface *formWindow, QUndoCommand *parent)
    : QUndoCommand(description, parent)
    , m_formWindow(formWindow)
{
}


void SARibbonBarBaseDesignerCommand::undo()
{
    cheapUpdate();
}


void SARibbonBarBaseDesignerCommand::redo()
{
    cheapUpdate();
}


QDesignerFormWindowInterface *SARibbonBarBaseDesignerCommand::formWindow() const
{
    return (m_formWindow);
}


QDesignerFormEditorInterface *SARibbonBarBaseDesignerCommand::core() const
{
    if (QDesignerFormWindowInterface *fw = formWindow()) {
        return (fw->core());
    }
    return (nullptr);
}


QDesignerPropertySheetExtension *SARibbonBarBaseDesignerCommand::propertySheet(QObject *object) const
{
    return (qt_extension<QDesignerPropertySheetExtension *>(formWindow()->core()->extensionManager(), object));
}


void SARibbonBarBaseDesignerCommand::cheapUpdate()
{
    if (core()->objectInspector()) {
        core()->objectInspector()->setFormWindow(formWindow());
    }

    if (core()->actionEditor()) {
        core()->actionEditor()->setFormWindow(formWindow());
    }
}


void SARibbonBarBaseDesignerCommand::updateBuddies(QDesignerFormWindowInterface *form, const QString& old_name, const QString& new_name)
{
    QExtensionManager *extensionManager = form->core()->extensionManager();

    using LabelList = QList<QLabel *>;

    const LabelList label_list = form->findChildren<QLabel *>();

    if (label_list.empty()) {
        return;
    }

    const QString buddyProperty = QStringLiteral("buddy");
    const QByteArray oldNameU8 = old_name.toUtf8();
    const QByteArray newNameU8 = new_name.toUtf8();

    const LabelList::const_iterator cend = label_list.constEnd();

    for (LabelList::const_iterator it = label_list.constBegin(); it != cend; ++it)
    {
        if (QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension *>(extensionManager, *it)) {
            const int idx = sheet->indexOf(buddyProperty);
            if (idx != -1) {
                const QByteArray oldBuddy = sheet->property(idx).toByteArray();
                if (oldBuddy == oldNameU8) {
                    sheet->setProperty(idx, newNameU8);
                }
            }
        }
    }
}


//////////////////////////////////////////////////



//////////////////////////////////////////////////


SARibbonAddActionCommand::SARibbonAddActionCommand(QDesignerFormWindowInterface *fw, QAction *act)
    : SARibbonBarBaseDesignerCommand(QApplication::translate("Command", "Add Action"), fw)
{
    SA_PLUGIN_MARK();
    m_action = act;
}


void SARibbonAddActionCommand::redo()
{
    SA_PLUGIN_MARK();
    SA_PLUGIN_LOG("formWindow():%d,m_action:%d,core():%d", formWindow(), m_action, core());
    core()->actionEditor()->setFormWindow(formWindow());
    core()->actionEditor()->manageAction(m_action);
}


void SARibbonAddActionCommand::undo()
{
    SA_PLUGIN_MARK();
    core()->actionEditor()->setFormWindow(formWindow());
    core()->actionEditor()->unmanageAction(m_action);
}


//////////////////////////////////////////////////


SARibbonPannelInsertActionCommand::SARibbonPannelInsertActionCommand(const QString& text, QDesignerFormWindowInterface *formWindow)
    : SARibbonBarBaseDesignerCommand(text, formWindow), m_parentWidget(nullptr),
    m_action(nullptr),
    m_beforeAction(nullptr),
    m_update(false)
{
    SA_PLUGIN_MARK();
}


void SARibbonPannelInsertActionCommand::init(QWidget *parentWidget, QAction *action, QAction *beforeAction, bool update)
{
    SA_PLUGIN_MARK();
    Q_ASSERT(m_parentWidget == nullptr);
    Q_ASSERT(m_action == nullptr);

    m_parentWidget = parentWidget;
    m_action = action;
    m_beforeAction = beforeAction;
    m_update = update;
}


void SARibbonPannelInsertActionCommand::redo()
{
    SA_PLUGIN_MARK();
    insertAction();
}


void SARibbonPannelInsertActionCommand::undo()
{
    SA_PLUGIN_MARK();
    removeAction();
}


void SARibbonPannelInsertActionCommand::insertAction()
{
    SA_PLUGIN_MARK();
    Q_ASSERT(m_action != nullptr);
    Q_ASSERT(m_parentWidget != nullptr);

    if (m_beforeAction) {
        pannel()->insertAction(m_beforeAction, m_action);
    }else{
        pannel()->addAction(m_action);
    }

    if (m_update) {
        cheapUpdate();
//        if (QMenu *menu = m_action->menu()) {
//            selectUnmanagedObject(menu);
//        }else{
//            selectUnmanagedObject(m_action);
//        }
//        PropertyHelper::triggerActionChanged(m_action); // Update Used column in action editor.
    }
}


void SARibbonPannelInsertActionCommand::removeAction()
{
    SA_PLUGIN_MARK();
    Q_ASSERT(m_action != nullptr);
    Q_ASSERT(m_parentWidget != nullptr);

    m_parentWidget->removeAction(m_action);

    if (m_update) {
        cheapUpdate();
//        selectUnmanagedObject(m_parentWidget);
//        PropertyHelper::triggerActionChanged(m_action); // Update Used column in action editor.
    }
}


SARibbonPannel *SARibbonPannelInsertActionCommand::pannel() const
{
    return (qobject_cast<SARibbonPannel *>(m_parentWidget));
}


//////////////////////////////////////////////////

static SARibbonRemoveActionCommand::ActionData findActionIn(QAction *action)
{
    SARibbonRemoveActionCommand::ActionData result;
    // We only want menus and toolbars, no toolbuttons.
    const QWidgetList& associatedWidgets = action->associatedWidgets();

    for (QWidget *widget : associatedWidgets)
    {
        if (qobject_cast<const QMenu *>(widget) ||
            qobject_cast<const QToolBar *>(widget) ||
            qobject_cast<const SARibbonPannel *>(widget)) {
            const QList<QAction *> actionList = widget->actions();
            const int size = actionList.size();
            for (int i = 0; i < size; ++i)
            {
                if (actionList.at(i) == action) {
                    QAction *before = nullptr;
                    if (i + 1 < size) {
                        before = actionList.at(i + 1);
                    }
                    result.append(SARibbonRemoveActionCommand::ActionDataItem(before, widget));
                    break;
                }
            }
        }
    }
    return (result);
}


SARibbonRemoveActionCommand::SARibbonRemoveActionCommand(QDesignerFormWindowInterface *formWindow)
    : SARibbonBarBaseDesignerCommand(QApplication::translate("Command", "Remove action"), formWindow)
    , m_action(nullptr)
{
}


void SARibbonRemoveActionCommand::init(QAction *action)
{
    Q_ASSERT(m_action == nullptr);
    m_action = action;

    m_actionData = findActionIn(action);
}


void SARibbonRemoveActionCommand::redo()
{
    QDesignerFormWindowInterface *fw = formWindow();

    for (const ActionDataItem& item : qAsConst(m_actionData))
    {
        item.widget->removeAction(m_action);
    }
    // Notify components (for example, signal slot editor)
    //    if (qdesigner_internal::FormWindowBase *fwb = qobject_cast<qdesigner_internal::FormWindowBase *>(fw)) {
    //        fwb->emitObjectRemoved(m_action);
    //    }
    emit fw->objectRemoved(m_action);

    core()->actionEditor()->setFormWindow(fw);
    core()->actionEditor()->unmanageAction(m_action);
    if (!m_actionData.empty()) {
        core()->objectInspector()->setFormWindow(fw);
    }
}


void SARibbonRemoveActionCommand::undo()
{
    core()->actionEditor()->setFormWindow(formWindow());
    core()->actionEditor()->manageAction(m_action);
    for (const ActionDataItem& item : qAsConst(m_actionData))
    {
        item.widget->insertAction(item.before, m_action);
    }
    if (!m_actionData.empty()) {
        core()->objectInspector()->setFormWindow(formWindow());
    }
}
