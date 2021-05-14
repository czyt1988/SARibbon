#ifndef SARIBBONBARCONTAINERFACTORY_H
#define SARIBBONBARCONTAINERFACTORY_H
#include <QExtensionFactory>
namespace SA_PLUGIN {
class SARibbonBarContainerFactory : public QExtensionFactory
{
    Q_OBJECT
public:
    explicit SARibbonBarContainerFactory(QExtensionManager *p);
protected:
    virtual QObject *createExtension(QObject *object, const QString& iid, QObject *parent) const;
};
}
#endif // SARIBBONBARCONTAINERFACTORY_H
