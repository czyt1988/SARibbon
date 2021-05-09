#ifndef SARIBBONBARCOMMANDS_H
#define SARIBBONBARCOMMANDS_H
#include <QUndoCommand>
#include <QDesignerFormWindowInterface>
#include <QPointer>
#include "SARibbonMainWindow.h"
#include "SARibbonCategory.h"
namespace SA_PLUGIN {
/**
 * @brief 为了支持designer的redo/undo
 */
class SARibbonBarBaseDesignerCommand : public QUndoCommand
{
public:
    SARibbonBarBaseDesignerCommand(const QString& description, QDesignerFormWindowInterface *formWindow);
public:
    QDesignerFormWindowInterface *formWindow() const { return (m_formWindow); }
    QDesignerFormEditorInterface *core() const;

protected:
    QPointer<QDesignerFormWindowInterface> m_formWindow;
};

/**
 * @brief 创建ribbonbar命令
 */
class SARibbonCreateRibbonBarDesignerCommand : public SARibbonBarBaseDesignerCommand
{
public:
    SARibbonCreateRibbonBarDesignerCommand(QDesignerFormWindowInterface *fw, SARibbonMainWindow *mainwindow);
    virtual void redo();
    virtual void undo();

private:
    SARibbonMainWindow *m_mainwindow;
};

/**
 * @brief 添加标签
 */
class SARibbonAddCategoryDesignerCommand : public SARibbonBarBaseDesignerCommand
{
public:
    SARibbonAddCategoryDesignerCommand(QDesignerFormWindowInterface *fw
        , SARibbonMainWindow *mainwindow
        , const QString& categoryName);
    virtual void redo();
    virtual void undo();

private:
    SARibbonMainWindow *m_mainwindow;
    QString m_categoryName;
    SARibbonCategory *m_category;
};
}
#endif // SARIBBONBARCOMMANDS_H
