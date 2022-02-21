#ifndef SARIBBONBARCOMMANDS_H
#define SARIBBONBARCOMMANDS_H
#include <QUndoCommand>
#include <QDesignerFormWindowInterface>
#include <QPointer>
#include "SARibbonMainWindow.h"
#include "SARibbonCategory.h"
#include "SARibbonPannel.h"
class QDesignerPropertySheetExtension;
namespace SA_PLUGIN {
/**
 * @brief 为了支持designer的redo/undo做的cmd基类，参考Qt Designer 源码的@sa QDesignerFormWindowCommand
 */
class SARibbonBarBaseDesignerCommand : public QUndoCommand
{
public:
    SARibbonBarBaseDesignerCommand(const QString& description,
        QDesignerFormWindowInterface *formWindow,
        QUndoCommand *parent = nullptr);
public:
    void undo() override;
    void redo() override;
    QDesignerFormWindowInterface *formWindow() const;
    QDesignerFormEditorInterface *core() const;
    QDesignerPropertySheetExtension *propertySheet(QObject *object) const;

    void cheapUpdate();

    static void updateBuddies(QDesignerFormWindowInterface *form,
        const QString& old_name,
        const QString& new_name);

protected:
    QPointer<QDesignerFormWindowInterface> m_formWindow;
};



/**
 * @brief 添加action
 */
class SARibbonAddActionCommand : public SARibbonBarBaseDesignerCommand
{
public:
    SARibbonAddActionCommand(QDesignerFormWindowInterface *fw, QAction *act);
    virtual void redo();
    virtual void undo();

private:
    QAction *m_action;
};

/**
 * @brief SARibbonPannel添加/插入Action命令
 */
class SARibbonPannelInsertActionCommand : public SARibbonBarBaseDesignerCommand
{
public:
    SARibbonPannelInsertActionCommand(const QString& text,
        QDesignerFormWindowInterface *formWindow
        );
public:
    void init(QWidget *parentWidget, QAction *action, QAction *beforeAction = nullptr, bool update = true);
    void redo() override;
    void undo() override;

protected:
    void insertAction();
    void removeAction();
    SARibbonPannel *pannel() const;

private:
    QWidget *m_parentWidget;
    QAction *m_action;
    QAction *m_beforeAction;
    bool m_update;
};

class SARibbonRemoveActionCommand : public SARibbonBarBaseDesignerCommand
{
public:
    explicit SARibbonRemoveActionCommand(QDesignerFormWindowInterface *formWindow);
    void init(QAction *action);
    void redo() override;
    void undo() override;

    struct ActionDataItem {
        ActionDataItem(QAction *_before = nullptr, QWidget *_widget = nullptr)
            : before(_before), widget(_widget) {}
        QAction *	before;
        QWidget *	widget;
    };
    using ActionData = QList<ActionDataItem>;
private:
    QAction *m_action;
    ActionData m_actionData;
};
}
#endif // SARIBBONBARCOMMANDS_H
