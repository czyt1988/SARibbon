#include "SARibbonBarCommands.h"
#include <QObject>
#include "SARibbonBar.h"
#ifndef TR
#define TR(str)    QObject::tr(str)
#endif
using namespace SA_PLUGIN;
SARibbonBarBaseDesignerCommand::SARibbonBarBaseDesignerCommand(const QString& description, QDesignerFormWindowInterface *formWindow)
    : QUndoCommand(description)
{
}


QDesignerFormEditorInterface *SARibbonBarBaseDesignerCommand::core() const
{
    if (QDesignerFormWindowInterface *fw = formWindow()) {
        return (fw->core());
    }
    return (nullptr);
}


//////////////////////////////////////////////////
SARibbonCreateRibbonBarDesignerCommand::SARibbonCreateRibbonBarDesignerCommand(
    QDesignerFormWindowInterface *fw
    , SARibbonMainWindow *mainwindow) : SARibbonBarBaseDesignerCommand(TR("use ribbon"), fw)
    , m_mainwindow(mainwindow)
{
}


void SARibbonCreateRibbonBarDesignerCommand::redo()
{
}


void SARibbonCreateRibbonBarDesignerCommand::undo()
{
}


//////////////////////////////////////////////////

SARibbonAddCategoryDesignerCommand::SARibbonAddCategoryDesignerCommand(
    QDesignerFormWindowInterface *fw
    , SARibbonMainWindow *mainwindow
    , const QString& categoryName) : SARibbonBarBaseDesignerCommand(TR("add category"), fw)
    , m_mainwindow(mainwindow)
    , m_categoryName(categoryName)
{
}


void SARibbonAddCategoryDesignerCommand::redo()
{
    m_mainwindow->ribbonBar()->addCategoryPage(m_categoryName);
}


void SARibbonAddCategoryDesignerCommand::undo()
{
}
