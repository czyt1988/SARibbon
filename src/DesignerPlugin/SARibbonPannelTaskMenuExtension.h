#ifndef SARIBBONPANNELTASKMENUEXTENSION_H
#define SARIBBONPANNELTASKMENUEXTENSION_H
#include <QDesignerTaskMenuExtension>
class QDesignerFormWindowInterface;
class QDesignerFormEditorInterface;
class SARibbonBar;
class SARibbonPannel;
namespace SA_PLUGIN {
class SARibbonPannelTaskMenuExtension : public QObject, public QDesignerTaskMenuExtension
{
    Q_OBJECT
    Q_INTERFACES(QDesignerTaskMenuExtension)
public:
    SARibbonPannelTaskMenuExtension(QWidget *w, QObject *p = nullptr);
    QList<QAction *> taskActions() const;

private:
    SARibbonPannel *pannel() const;
    void initActions();
    QDesignerFormWindowInterface *ribbonPannelFormWindowInterface() const;
    QDesignerFormEditorInterface *ribbonPannelCore() const;
    QDesignerFormWindowInterface *ribbonCategoryFormWindowInterface() const;
    QDesignerFormEditorInterface *ribbonCategoryCore() const;
    QDesignerFormWindowInterface *ribbonBarFormWindowInterface() const;
    QDesignerFormEditorInterface *ribbonBarCore() const;
    QAction *createAction(QDesignerFormWindowInterface *fw, const QString& objectName, bool separator);
    void addActionRowProportionDynamicProperty(QAction *act, int rowProportion);
    void addRibbonAction(int actionType);

private slots:
    void onAddLargeAction();
    void onAddSmallAction();
    void onAddMediumAction();

private:
    QList<QAction *> m_actions;
    QWidget *m_widget;
};
}
#endif // SARIBBONPANNELTASKMENUEXTENSION_H
