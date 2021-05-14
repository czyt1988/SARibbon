#ifndef SARIBBONMAINWINDOWTASKMENUEXTENSIONFACTORY_H
#define SARIBBONMAINWINDOWTASKMENUEXTENSIONFACTORY_H
#include <QExtensionFactory>
#include <QDesignerFormWindowInterface>
#include <QDesignerTaskMenuExtension>
namespace SA_PLUGIN {
/**
 * @brief SARibbonMainWindowTaskMenuExtension的工厂
 */
class SARibbonMainWindowTaskMenuExtensionFactory : public QExtensionFactory
{
    Q_OBJECT
public:
    SARibbonMainWindowTaskMenuExtensionFactory(QExtensionManager *mgr = nullptr);
protected:
    QObject *createExtension(QObject *obj, const QString& iid, QObject *p) const;
};
}

#endif // SARIBBONMAINWINDOWTASKMENUEXTENSIONFACTORY_H
