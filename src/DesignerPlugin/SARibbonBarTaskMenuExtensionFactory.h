#ifndef SARIBBONBARTASKMENUEXTENSIONFACTORY_H
#define SARIBBONBARTASKMENUEXTENSIONFACTORY_H
#include <QExtensionFactory>
#include <QDesignerFormWindowInterface>
#include <QDesignerTaskMenuExtension>
namespace SA_PLUGIN {
class SARibbonBarTaskMenuExtensionFactory : public QExtensionFactory
{
    Q_OBJECT
public:
    SARibbonBarTaskMenuExtensionFactory(QExtensionManager *mgr = nullptr);
protected:
    QObject *createExtension(QObject *obj, const QString& iid, QObject *p) const;
};
}
#endif // SARIBBONBARTASKMENUEXTENSIONFACTORY_H
