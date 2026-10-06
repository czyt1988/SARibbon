#ifndef RIBBONSEPARATOR_H
#define RIBBONSEPARATOR_H
#include "SARibbonQmlGlobal.h"
#include "SARibbonQmlLayoutItemHost.h"
#include "SARibbonQmlTypes.h"

namespace SARibbonQml {

/**
 * \if ENGLISH
 * @brief Panel separator host: a thin vertical line between panel items
 * @details The QML counterpart of the widgets SARibbonSeparatorWidget:
 *          participates in the panel engine as a Large-proportion item
 *          (full body height, own column, widgets addSeparator parity);
 *          the leaf draws a 1px theme-colored line inset from the edges.
 * \endif
 *
 * \if CHINESE
 * @brief 面板分隔符宿主：面板项之间的细竖线
 * @details 对应 widgets 侧 SARibbonSeparatorWidget：以 Large 比例参与
 *          面板引擎（全高、独占一列，与 widgets addSeparator 一致）；
 *          叶子在上下内缩的区域内画一条 1px 主题色竖线。
 * \endif
 */
class SA_RIBBON_QML_EXPORT RibbonSeparator : public RibbonLayoutItemHost
{
    Q_OBJECT
public:
    explicit RibbonSeparator(QQuickItem* parent = nullptr);
    ~RibbonSeparator() override;

    // ---- contract implementation ----
    QSize sizeHint() const override;

protected:
    void componentComplete() override;
    QUrl leafUrl() const override;
    void largeHeightContextChanged() override;

private:
    QSize mCachedSizeHint;
};

}

#endif  // RIBBONSEPARATOR_H
