#ifndef SARIBBONMAINWINDOWTASKMENUFACTORY_H
#define SARIBBONMAINWINDOWTASKMENUFACTORY_H
#include <QExtensionFactory>
#include <QDesignerFormWindowInterface>
#include <QDesignerTaskMenuExtension>
namespace SA_PLUGIN {
/**
 * @brief SARibbonMainWindowTaskMenuExtension的工厂
 */
class SARibbonMainWindowTaskMenuFactory : public QExtensionFactory
{
    Q_OBJECT
public:
    SARibbonMainWindowTaskMenuFactory(QExtensionManager *mgr = nullptr);
protected:
    QObject *createExtension(QObject *obj, const QString& iid, QObject *p) const;
};
}

#endif // SARIBBONMAINWINDOWTASKMENUEXTENSIONFACTORY_H
