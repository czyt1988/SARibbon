#ifndef SARIBBONMAINWINDOWCONTAINERFACTORY_H
#define SARIBBONMAINWINDOWCONTAINERFACTORY_H
#include <QExtensionFactory>
namespace SA_PLUGIN {
class SARibbonMainWindowContainerFactory : public QExtensionFactory
{
    Q_OBJECT
public:
    explicit SARibbonMainWindowContainerFactory(QExtensionManager *p);
protected:
    virtual QObject *createExtension(QObject *object, const QString& iid, QObject *parent) const;
};
}
#endif // SARIBBONMAINWINDOWCONTAINERFACTORY_H
