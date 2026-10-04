#ifndef SARIBBONELEMENTFACTORYINTERFACE_H
#define SARIBBONELEMENTFACTORYINTERFACE_H
#include <SARibbonCore/SARibbonCoreGlobal.h>

namespace SARibbon
{
namespace Core
{

/**
 * \if ENGLISH
 * @brief Placeholder for the element factory interface (plan 02 S4.3, D6/D7 deferred)
 * @details The real 17-create-function SARibbonElementFactory is deeply coupled to
 * widgets return types; interfacing it into core has no consumer until QML needs it
 * (D7 gate, Tier 2). This header pins the seam for plan 04 / 3.1+.
 * \endif
 *
 * \if CHINESE
 * @brief 元素工厂接口占位（计划 02 S4.3，D6/D7 降级决策）
 * @details 真实的 17 个 create 函数的 SARibbonElementFactory 返回值深度耦合
 * widgets 类型；QML 需要前（D7 gate，Tier 2）core 无消费者。本头文件为
 * 计划 04 / 3.1+ 预留衔接点。
 * \endif
 */
class SARibbonElementFactoryInterface
{
public:
    virtual ~SARibbonElementFactoryInterface() = default;
};

}
}

#endif  // SARIBBONELEMENTFACTORYINTERFACE_H
