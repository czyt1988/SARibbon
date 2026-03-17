#include "SARibbonElementManager.h"

/**
 * \if ENGLISH
 * @brief Constructor for SARibbonElementManager
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonElementManager 构造函数
 * \endif
 */
SARibbonElementManager::SARibbonElementManager()
{
    mFactory.reset(new SARibbonElementFactory());
}

/**
 * \if ENGLISH
 * @brief Destructor for SARibbonElementManager
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonElementManager 析构函数
 * \endif
 */
SARibbonElementManager::~SARibbonElementManager()
{
}

/**
 * \if ENGLISH
 * @brief Get the singleton instance of SARibbonElementManager
 * @return SARibbonElementManager singleton instance
 * \endif
 *
 * \if CHINESE
 * @brief 获取SARibbonElementManager的单例实例
 * @return SARibbonElementManager单例实例
 * \endif
 */
SARibbonElementManager* SARibbonElementManager::instance()
{
    static SARibbonElementManager s_instance;

    return (&(s_instance));
}

/**
 * \if ENGLISH
 * @brief Get the current SARibbonElementFactory
 * @return SARibbonElementFactory pointer
 * \endif
 *
 * \if CHINESE
 * @brief 获取当前的SARibbonElementFactory
 * @return SARibbonElementFactory指针
 * \endif
 */
SARibbonElementFactory* SARibbonElementManager::factory()
{
    return (mFactory.data());
}

/**
 * \if ENGLISH
 * @brief Setup the SARibbonElementFactory
 * @param fac SARibbonElementFactory pointer
 * \endif
 *
 * \if CHINESE
 * @brief 设置SARibbonElementFactory
 * @param fac SARibbonElementFactory指针
 * \endif
 */
void SARibbonElementManager::setupFactory(SARibbonElementFactory* fac)
{
    mFactory.reset(fac);
}
