#ifndef SARIBBONPLUGINCOLLECTION_H
#define SARIBBONPLUGINCOLLECTION_H
#include <QDesignerCustomWidgetInterface>
namespace SA_PLUGIN {
/**
 * @brief 所有saribobn的插件集合
 *
 * 参考QDesignerCustomWidgetCollectionInterface
 */
class SARibbonPluginCollection : public QObject, public QDesignerCustomWidgetCollectionInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "SA.SARibbon.SARibbonPluginCollection")
    Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)
public:
    SARibbonPluginCollection(QObject *p = nullptr);
    QList<QDesignerCustomWidgetInterface *> customWidgets() const Q_DECL_OVERRIDE;

private:
    QList<QDesignerCustomWidgetInterface *> m_widgets;
};
}
#endif // SARIBBONPLUGINCOLLECTION_H
