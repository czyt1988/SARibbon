#ifndef RIBBONQUICKACCESSBAR_H
#define RIBBONQUICKACCESSBAR_H
#include "SARibbonQmlGlobal.h"
#include "../host/RibbonButtonRowHost.h"

namespace SARibbonQml {

/**
 * \if ENGLISH
 * @brief Quick access bar: a row of small buttons riding the title row
 * @details The QML counterpart of the widgets SARibbonQuickAccessBar: a
 *          RibbonButtonRowHost placed by the bar after the application
 *          button; its width feeds TitleRectInput.hasQuickAccessBar.
 * \endif
 *
 * \if CHINESE
 * @brief 快速访问栏：标题行上的一排小按钮
 * @details 对应 widgets 侧 SARibbonQuickAccessBar：一个 RibbonButtonRowHost，
 *          由 bar 摆放在应用按钮之后；其宽度进入
 *          TitleRectInput.hasQuickAccessBar 的预留。
 * \endif
 */
class SA_RIBBON_QML_EXPORT RibbonQuickAccessBar : public RibbonButtonRowHost
{
    Q_OBJECT
public:
    explicit RibbonQuickAccessBar(QQuickItem* parent = nullptr);
    ~RibbonQuickAccessBar() override;
};

}

#endif  // RIBBONQUICKACCESSBAR_H
