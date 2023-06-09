#include "SARibbonElementManager.h"

SARibbonElementManager::SARibbonElementManager()
{
    mFactory.reset(new SARibbonElementFactory());
}

SARibbonElementManager::~SARibbonElementManager()
{
}

SARibbonElementManager* SARibbonElementManager::instance()
{
    static SARibbonElementManager s_instance;

    return (&(s_instance));
}

SARibbonElementFactory* SARibbonElementManager::factory()
{
    return (mFactory.data());
}

void SARibbonElementManager::setupFactory(SARibbonElementFactory* delegate)
{
    mFactory.reset(delegate);
}
