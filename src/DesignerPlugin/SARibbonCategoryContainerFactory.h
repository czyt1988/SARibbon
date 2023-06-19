#ifndef SARIBBONCATEGORYCONTAINERFACTORY_H
#define SARIBBONCATEGORYCONTAINERFACTORY_H
#include <QExtensionFactory>
namespace SA_PLUGIN {
class SARibbonCategoryContainerFactory : public QExtensionFactory
{
    Q_OBJECT
public:
    explicit SARibbonCategoryContainerFactory(QExtensionManager *p);
protected:
    virtual QObject *createExtension(QObject *object, const QString& iid, QObject *parent) const;
};
}
#endif // SARIBBONCATEGORYCONTAINERFACTORY_H
