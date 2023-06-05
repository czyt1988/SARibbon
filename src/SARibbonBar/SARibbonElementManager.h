#ifndef SARIBBONELEMENTMANAGER_H
#define SARIBBONELEMENTMANAGER_H
#include "SARibbonElementFactory.h"
#include "SARibbonGlobal.h"

///
/// \brief 此类是一个全局单例，用于管理SARibbonElementCreateDelegate
///
class SA_RIBBON_EXPORT SARibbonElementManager
{
protected:
    SARibbonElementManager();

public:
    virtual ~SARibbonElementManager();
    static SARibbonElementManager* instance();
    SARibbonElementFactory* factory();
    void setupFactory(SARibbonElementFactory* fac);

private:
    QScopedPointer< SARibbonElementFactory > mFactory;
};
#ifndef RibbonSubElementMgr
#define RibbonSubElementMgr SARibbonElementManager::instance()
#endif
#ifndef RibbonSubElementDelegate
#define RibbonSubElementDelegate SARibbonElementManager::instance()->factory()
#endif
#ifndef RibbonSubElementStyleOpt
#define RibbonSubElementStyleOpt SARibbonElementManager::instance()->factory()->getRibbonStyleOption()
#endif
#endif  // SARIBBONELEMENTMANAGER_H
