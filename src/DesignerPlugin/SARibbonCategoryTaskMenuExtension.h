#ifndef SARIBBONCATEGORYTASKMENUEXTENSION_H
#define SARIBBONCATEGORYTASKMENUEXTENSION_H
#include <QDesignerTaskMenuExtension>
class QDesignerFormWindowInterface;
class QDesignerFormEditorInterface;
class SARibbonBar;
namespace SA_PLUGIN {
class SARibbonCategoryTaskMenuExtension : public QObject, public QDesignerTaskMenuExtension
{
    Q_OBJECT
    Q_INTERFACES(QDesignerTaskMenuExtension)
public:
    SARibbonCategoryTaskMenuExtension(QWidget *w, QObject *p = nullptr);
    QList<QAction *> taskActions() const;

private:
    void initActions();
    QDesignerFormWindowInterface *ribbonCategoryFormWindowInterface() const;
    QDesignerFormEditorInterface *ribbonCategoryCore() const;
    QDesignerFormWindowInterface *ribbonBarFormWindowInterface() const;
    QDesignerFormEditorInterface *ribbonBarCore() const;

private slots:
    void onAddPannel();
    void onRemoveCategory();

private:
    QList<QAction *> m_actions;
    QWidget *m_widget;
};
}
#endif // SARIBBONCATEGORYTASKMENUEXTENSION_H
