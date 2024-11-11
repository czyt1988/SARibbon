#ifndef SARIBBONPANNELOPTIONBUTTON_H
#define SARIBBONPANNELOPTIONBUTTON_H
#include <QToolButton>
#include "SARibbonGlobal.h"
class QAction;

/**
 * @brief Pannel右下角的操作按钮
 *
 * 此按钮和一个action关联，使用@ref SARibbonPannel::addOptionAction 函数用于生成此按钮，正常来说
 * 用户并不需要直接操作此类，仅仅用于样式设计
 * 如果一定要重载此按钮，可以通过重载@ref SARibbonElementFactory
 * 的 @ref SARibbonElementFactory::createRibbonPannelOptionButton来实现新的OptionButton
 *
 */
class SA_RIBBON_EXPORT SARibbonPannelOptionButton : public QToolButton
{
    Q_OBJECT
public:
    explicit SARibbonPannelOptionButton(QWidget* parent = nullptr);
    ~SARibbonPannelOptionButton();
};

#endif  // SAROBBONPANNELOPTIONBUTTON_H
