#ifndef SARIBBONELEMENTMANAGER_H
#define SARIBBONELEMENTMANAGER_H
#include "SARibbonElementFactory.h"
#include "SARibbonGlobal.h"

/**
   @brief 此类是一个全局单例，用于管理SARibbonElementFactory

    如果你有自己的某个部件要重写，首先你需要有自己的ElementFactory
    @code
    class MyRibbonElementFactory : public SARibbonElementFactory{
    public:
        ...
        virtual SARibbonPannel* createRibbonPannel(QWidget* parent){
            return new MyRibbonPannel(parent);
        }
    };
    @endcode

    然后，你需要在ribbonbar创建之前把自己的ElementFactory设置进去

    这个一般会在main函数中进行

    @code
    SARibbonElementManager::instance()->setupFactory(new MyRibbonElementFactory);
    @endcode

    这样，SARibbon创建的pannel就是你自己重写的MyRibbonPannel

 */
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
#ifndef RibbonSubElementFactory
#define RibbonSubElementFactory SARibbonElementManager::instance()->factory()
#endif

#endif  // SARIBBONELEMENTMANAGER_H
