#ifndef SARIBBONMAINWINDOWTASKMENUEXTENSION_H
#define SARIBBONMAINWINDOWTASKMENUEXTENSION_H
#include <QDesignerTaskMenuExtension>
class QDesignerFormWindowInterface;
class QDesignerFormEditorInterface;
class SARibbonBar;
namespace SA_PLUGIN {
class SARibbonMainWindowTaskMenuExtension : public QObject, public QDesignerTaskMenuExtension
{
    Q_OBJECT
    Q_INTERFACES(QDesignerTaskMenuExtension)
public:
    SARibbonMainWindowTaskMenuExtension(QWidget *w, QObject *p = nullptr);
    QList<QAction *> taskActions() const;

private:
    void initActions();
    QDesignerFormWindowInterface *formWindowInterface() const;
    QDesignerFormEditorInterface *core() const;

private slots:
    void onUseRibbon();

private:
    QList<QAction *> m_actions;
    QWidget *m_widget;
    SARibbonBar *m_bar;
    QAction *m_useRibbon;
};
}
#endif // SARIBBONMAINWINDOWTASKMENUEXTENSION_H
