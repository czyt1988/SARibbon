#ifndef SARIBBONABSTRACTLAYOUTHOST_H
#define SARIBBONABSTRACTLAYOUTHOST_H
#include <SARibbonCore/SARibbonCoreGlobal.h>

namespace SARibbon
{
namespace Core
{

class SARibbonMetrics;

/**
 * \if ENGLISH
 * @brief Host side of the layout contract (plan 02 S4.1)
 * @details Only one pure virtual: engines get isRTL / margins / spacing as plain
 * Input values instead of querying the host, keeping the engines stateless.
 * \endif
 *
 * \if CHINESE
 * @brief 布局契约的宿主侧（计划 02 S4.1）
 * @details 仅一个纯虚：isRTL/边距/间距一律作为引擎 Input 值传入，
 * 引擎不查询宿主，保持无状态。
 * \endif
 */
class SA_RIBBON_CORE_EXPORT SARibbonAbstractLayoutHost
{
public:
    virtual ~SARibbonAbstractLayoutHost();
    virtual const SARibbonMetrics& metrics() const = 0;
};

}
}

#endif  // SARIBBONABSTRACTLAYOUTHOST_H
