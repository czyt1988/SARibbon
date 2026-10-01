#ifndef RIBBONLAYOUTITEMHOST_H
#define RIBBONLAYOUTITEMHOST_H
#include "RibbonQuickHost.h"
#include <SARibbonCore/SARibbonAbstractLayoutItem.h>

namespace SARibbonQml {

/**
 * \if ENGLISH
 * @brief Common base of every panel child: QQuickItem + layout contract
 * @details Everything the panel layout engine needs identically from its
 *          children lives here: the hidden/applyGeometry/debugName contract
 *          faces and the large-row-height context plumbing (items with a
 *          Large proportion derive their width hint from the panel's current
 *          large row height, exactly like SARibbonToolButton on the widgets
 *          side). Subclasses only implement sizeHint() (and, rarely,
 *          expandingDirections/stretchFactor for gallery-like items).
 * \endif
 *
 * \if CHINESE
 * @brief 所有面板子项的公共基类：QQuickItem + 布局契约
 * @details 面板布局引擎对所有子项的相同需求集中在此：hidden/applyGeometry/
 *          debugName 契约面，以及大行高上下文的传递（Large 比例的项其宽度
 *          hint 取决于面板当前大行高度，与 widgets 侧 SARibbonToolButton 一致）。
 *          子类只需实现 sizeHint()（画廊类项罕见地再加 expandingDirections/
 *          stretchFactor）。
 * \endif
 */
class SA_RIBBON_QML_EXPORT RibbonLayoutItemHost : public RibbonQuickHost, public SARibbon::Core::SARibbonAbstractLayoutItem
{
    Q_OBJECT
public:
    explicit RibbonLayoutItemHost(QQuickItem* parent = nullptr);
    ~RibbonLayoutItemHost() override;

    // ---- contract implementation shared by all panel children ----
    bool isHidden() const override;                  // !isVisible()
    void applyGeometry(const QRect& rect) override;  // setPosition + setSize
    QString debugName() const override;              // objectName()
    Qt::Orientations expandingDirections() const override;  // none; gallery overrides

    // Panel context (set by RibbonPanel after each engine pass)
    void setLargeButtonHeightContext(int h);
    int largeButtonHeightContext() const { return mLargeButtonHeightContext; }

protected:
    // Drop the engine sizeHint cache entry of this item and re-run the panel
    // layout (called whenever a sizeHint input changed)
    void invalidatePanelLayout();

    // Hook invoked when the large row height context changed; subclasses with
    // a metrics-derived sizeHint recompute it here
    virtual void largeHeightContextChanged();

private:
    int mLargeButtonHeightContext = 0;
};

}

#endif  // RIBBONLAYOUTITEMHOST_H
