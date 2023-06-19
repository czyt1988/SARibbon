#ifndef SARIBBONCATEGORYDESIGNERPLUGIN_H
#define SARIBBONCATEGORYDESIGNERPLUGIN_H


#include <QDesignerCustomWidgetInterface>
class QDesignerFormEditorInterface;
class QDesignerFormWindowInterface;
namespace SA_PLUGIN {
/**
 * @brief SARibbonBar对应的插件
 */
class SARibbonCategoryDesignerPlugin : public QObject,
                       public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
    SARibbonCategoryDesignerPlugin(QObject *p = nullptr);
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
    void onWindowTitleChanged(const QString& title);

private:
    bool m_isInitialized;
    QDesignerFormEditorInterface *m_formEditor;
};
}
#endif // SARIBBONCATEGORYDESIGNERPLUGIN_H
