#include "SARibbonElementManager.h"


SARibbonElementManager::SARibbonElementManager() : m_delegate(nullptr)
{
    m_delegate = new SARibbonElementCreateDelegate();
}


SARibbonElementManager::~SARibbonElementManager()
{
    if (m_delegate) {
        delete m_delegate;
    }
    m_delegate = nullptr;
}


SARibbonElementManager *SARibbonElementManager::instance()
{
    static SARibbonElementManager s_instance;

    return (&(s_instance));
}


SARibbonElementCreateDelegate *SARibbonElementManager::delegate()
{
    return (m_delegate);
}


void SARibbonElementManager::setupDelegate(SARibbonElementCreateDelegate *delegate)
{
    if (m_delegate) {
        delete m_delegate;
    }
    m_delegate = delegate;
}
