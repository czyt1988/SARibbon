#ifndef SARIBBONPANELOPTIONBUTTON_H
#define SARIBBONPANELOPTIONBUTTON_H
#include <QToolButton>
#include "SARibbonGlobal.h"
class QAction;

/**
 * \if ENGLISH
 * @brief Operation button at the bottom right corner of the panel
 * @details This button is associated with an action, and the @ref SARibbonPanel::addOptionAction function is used to generate this button. Normally,
 * @details users do not need to directly operate this class, it is only used for style design
 * @details If you must override this button, you can implement a new OptionButton by overriding
 * @details @ref SARibbonElementFactory::createRibbonPanelOptionButton of @ref SARibbonElementFactory
 * \endif
 *
 * \if CHINESE
 * @brief Panel右下角的操作按钮
 * @details 此按钮和一个action关联，使用@ref SARibbonPanel::addOptionAction 函数用于生成此按钮，正常来说
 * @details 用户并不需要直接操作此类，仅仅用于样式设计
 * @details 如果一定要重载此按钮，可以通过重载@ref SARibbonElementFactory
 * @details 的 @ref SARibbonElementFactory::createRibbonPanelOptionButton来实现新的OptionButton
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonPanelOptionButton : public QToolButton
{
	Q_OBJECT
public:
	/// Constructor for SARibbonPanelOptionButton
	explicit SARibbonPanelOptionButton(QWidget* parent = nullptr);
	/// Destructor for SARibbonPanelOptionButton
	~SARibbonPanelOptionButton();
};

#endif  // SAROBBONPANELOPTIONBUTTON_H
