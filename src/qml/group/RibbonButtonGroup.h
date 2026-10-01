#ifndef RIBBONBUTTONGROUP_H
#define RIBBONBUTTONGROUP_H
#include "SARibbonQmlGlobal.h"
#include "../host/RibbonButtonRowHost.h"

namespace SARibbonQml {

/**
 * \if ENGLISH
 * @brief Right button group: small buttons grouped on the title row's right
 * @details The QML counterpart of the widgets SARibbonButtonGroupWidget: a
 *          RibbonButtonRowHost the bar right-aligns on the title row (before
 *          the system-button strip), typically used for help/visibility
 *          toggles like the widgets example's right button group.
 * \endif
 *
 * \if CHINESE
 * @brief 右侧按钮组：标题行右侧成组的小按钮
 * @details 对应 widgets 侧 SARibbonButtonGroupWidget：一个
 *          RibbonButtonRowHost，由 bar 右对齐摆在标题行（系统按钮区之前），
 *          典型用法即 widgets 示例右侧按钮组的 help/可见性切换。
 * \endif
 */
class SA_RIBBON_QML_EXPORT RibbonButtonGroup : public RibbonButtonRowHost
{
    Q_OBJECT
public:
    explicit RibbonButtonGroup(QQuickItem* parent = nullptr);
    ~RibbonButtonGroup() override;
};

}

#endif  // RIBBONBUTTONGROUP_H
