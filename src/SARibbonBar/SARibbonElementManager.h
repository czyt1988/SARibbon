#ifndef SARIBBONELEMENTMANAGER_H
#define SARIBBONELEMENTMANAGER_H
#include "SARibbonElementFactory.h"
#include "SARibbonGlobal.h"

/**
 * \if ENGLISH
 * @brief Global singleton class for managing SARibbonElementFactory
 * @details If you want to override a component, you first need to create your own ElementFactory
 * @code
 * class MyRibbonElementFactory : public SARibbonElementFactory{
 * public:
 *     ...
 *     virtual SARibbonPanel* createRibbonPanel(QWidget* parent){
 *         return new MyRibbonPanel(parent);
 *     }
 * };
 * @endcode
 * @details Then you need to set your own ElementFactory before creating the ribbonbar
 * @details This is typically done in the main function
 * @code
 * SARibbonElementManager::instance()->setupFactory(new MyRibbonElementFactory);
 * @endcode
 * @details This way, the panel created by SARibbon will be your overridden MyRibbonPanel
 * \endif
 *
 * \if CHINESE
 * @brief 此类是一个全局单例，用于管理SARibbonElementFactory
 * @details 如果你有自己的某个部件要重写，首先你需要有自己的ElementFactory
 * @code
 * class MyRibbonElementFactory : public SARibbonElementFactory{
 * public:
 *     ...
 *     virtual SARibbonPanel* createRibbonPanel(QWidget* parent){
 *         return new MyRibbonPanel(parent);
 *     }
 * };
 * @endcode
 * @details 然后，你需要在ribbonbar创建之前把自己的ElementFactory设置进去
 * @details 这个一般会在main函数中进行
 * @code
 * SARibbonElementManager::instance()->setupFactory(new MyRibbonElementFactory);
 * @endcode
 * @details 这样，SARibbon创建的panel就是你自己重写的MyRibbonPanel
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonElementManager
{
protected:
	/// Constructor for SARibbonElementManager
	SARibbonElementManager();

public:
	/// Destructor for SARibbonElementManager
	virtual ~SARibbonElementManager();
	/// Get the singleton instance
	static SARibbonElementManager* instance();
	/// Get the current factory
	SARibbonElementFactory* factory();
	/// Set the factory
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
