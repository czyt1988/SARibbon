#ifndef SARIBBONBARTASKMENUFACTORY_H
#define SARIBBONBARTASKMENUFACTORY_H
#include <QExtensionFactory>
#include <QDesignerFormWindowInterface>
#include <QDesignerTaskMenuExtension>
namespace SA_PLUGIN {
class SARibbonBarTaskMenuFactory : public QExtensionFactory
{
    Q_OBJECT
public:
    SARibbonBarTaskMenuFactory(QExtensionManager *mgr = nullptr);
protected:
    QObject *createExtension(QObject *obj, const QString& iid, QObject *p) const;
};
}
#endif // SARIBBONBARTASKMENUEXTENSIONFACTORY_H
