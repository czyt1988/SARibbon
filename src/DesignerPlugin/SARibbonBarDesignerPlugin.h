#ifndef SARIBBONBARDESIGNERPLUGIN_H
#define SARIBBONBARDESIGNERPLUGIN_H

#include <QDesignerCustomWidgetInterface>
class QDesignerFormEditorInterface;
class QDesignerFormWindowInterface;
namespace SA_PLUGIN {
/**
 * @brief SARibbonBar对应的插件
 */
class SARibbonBarDesignerPlugin : public QObject,
                  public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
    SARibbonBarDesignerPlugin(QObject *p = nullptr);
public:
    bool isContainer() const Q_DECL_OVERRIDE;
    bool isInitialized() const Q_DECL_OVERRIDE;
    QIcon icon() const Q_DECL_OVERRIDE;
    QString domXml() const Q_DECL_OVERRIDE;
    QString group() const Q_DECL_OVERRIDE;
    QString includeFile() const Q_DECL_OVERRIDE;
    QString name() const Q_DECL_OVERRIDE;
    QString toolTip() const Q_DECL_OVERRIDE;
    QString whatsThis() const Q_DECL_OVERRIDE;
    QWidget *createWidget(QWidget *parent) Q_DECL_OVERRIDE;
    void initialize(QDesignerFormEditorInterface *core) Q_DECL_OVERRIDE;

private slots:
    void onFormWindowAdded(QDesignerFormWindowInterface *formWindow);
    void onFormWindowRemoved(QDesignerFormWindowInterface *formWindow);
    void onWidgetManaged(QWidget *widget);

private:
    bool m_isInitialized;
    QDesignerFormEditorInterface *m_formEditor;
};
}
#endif // SARIBBONBARDESIGNERPLUGIN_H
