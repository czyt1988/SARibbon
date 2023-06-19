#ifndef SARIBBONBARTASKMENUEXTENSION_H
#define SARIBBONBARTASKMENUEXTENSION_H
#include <QDesignerTaskMenuExtension>
class QDesignerFormWindowInterface;
class QDesignerFormEditorInterface;
namespace SA_PLUGIN {
class SARibbonBarTaskMenuExtension : public QObject, public QDesignerTaskMenuExtension
{
    Q_OBJECT
    Q_INTERFACES(QDesignerTaskMenuExtension)
public:
    SARibbonBarTaskMenuExtension(QWidget *w, QObject *p = nullptr);
    QList<QAction *> taskActions() const;

private:
    void initActions();
    QDesignerFormWindowInterface *formWindowInterface() const;
    QDesignerFormEditorInterface *core() const;

private slots:
    void onAddCategory();
    void onAddContextCategory();

private:
    QList<QAction *> m_actions;
    QWidget *m_widget;
};
}
#endif // SARIBBONBARTASKMENUEXTENSION_H
