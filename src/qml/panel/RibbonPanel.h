#ifndef RIBBONPANEL_H
#define RIBBONPANEL_H
#include "SARibbonQmlGlobal.h"
#include "../host/RibbonQuickHost.h"
#include "../SARibbonQmlTypes.h"
#include <SARibbonCore/SARibbonAbstractLayoutItem.h>
#include <SARibbonCore/SARibbonPanelLayoutEngine.h>
#include <QVector>

namespace SARibbonQml {

class RibbonLayoutItemHost;

/**
 * \if ENGLISH
 * @brief Panel structural host: drives the core PanelLayoutEngine (plan-04 S3)
 * @details updatePolish() is the single layout entry: collect contract items ->
 *          engine.layout() -> applyGeometry per item. updatePolish only fires for items
 *          attached to a QQuickWindow, so tests must expose the scene first. The visual
 *          leaf is created from qrc via the leaf-creation trilogy and pairs back through
 *          the inherited qmlLeaf handshake property.
 *          Children are accepted generically: every RibbonLayoutItemHost child
 *          (tool button, control container, gallery, separator, ...) joins the
 *          engine pass through the shared contract base, mirroring the widgets
 *          side where the panel layout drives QWidgetItem-like wrappers.
 * \endif
 *
 * \if CHINESE
 * @brief Panel 结构宿主：驱动 core 的 PanelLayoutEngine（计划 04 S3）
 * @details updatePolish() 是唯一布局入口：收集契约项 -> engine.layout() ->
 *          逐项 applyGeometry。updatePolish 只对挂进 QQuickWindow 的 item 触发，
 *          测试须先曝光场景。视觉叶子经 qrc 以"创建三部曲"生成，并通过继承的
 *          qmlLeaf 握手属性配对。
 *          子项注册是泛化的：任何 RibbonLayoutItemHost 子项（工具按钮、控件
 *          容器、画廊、分隔符……）都经共享契约基类加入引擎布局，对应 widgets
 *          侧 panel 布局驱动 QWidgetItem 包装器的做法。
 * \endif
 */
class RibbonPanel : public RibbonQuickHost
{
    Q_OBJECT
    Q_PROPERTY(QString panelTitle READ panelTitle WRITE setPanelTitle NOTIFY panelTitleChanged)
    Q_PROPERTY(RibbonEnums::LayoutMode layoutMode READ layoutMode WRITE setLayoutMode NOTIFY layoutModeChanged)
    Q_PROPERTY(bool enableShowPanelTitle READ enableShowPanelTitle WRITE setEnableShowPanelTitle NOTIFY enableShowPanelTitleChanged)
    Q_PROPERTY(QRectF titleGeometry READ titleGeometry NOTIFY titleGeometryChanged)
public:
    explicit RibbonPanel(QQuickItem* parent = nullptr);
    ~RibbonPanel() override;

    QString panelTitle() const;
    void setPanelTitle(const QString& t);

    RibbonEnums::LayoutMode layoutMode() const;
    void setLayoutMode(RibbonEnums::LayoutMode mode);

    // Panel title strip visibility (bar ribbonStyle propagation; single-row
    // styles hide it — widgets setEnableShowPanelTitle parity)
    bool enableShowPanelTitle() const;
    void setEnableShowPanelTitle(bool on);

    // Style push from the category (bar ribbonStyle propagation chain):
    // row mode + title visibility land here, word wrap / icon-right flags
    // forward to the registered tool buttons
    void applyRibbonStyle(RibbonEnums::LayoutMode mode, bool showPanelTitle, bool wordWrap, bool iconRightText);

    // explicit child registration list (itemChange of the children calls in);
    // accepts every layout item host type
    void registerChildItem(RibbonLayoutItemHost* item);
    void unregisterChildItem(RibbonLayoutItemHost* item);

    // drop the engine sizeHint cache entry of an item and re-run the layout
    void invalidateChildCache(SARibbon::Core::SARibbonAbstractLayoutItem* item);

    // engine-computed title strip rect; the leaf renders the caption inside it
    QRectF titleGeometry() const;

Q_SIGNALS:
    void panelTitleChanged();
    void layoutModeChanged();
    void enableShowPanelTitleChanged();
    void titleGeometryChanged();

protected:
    QUrl leafUrl() const override;
    void updatePolish() override;  // THE layout entry (scene-graph polished)
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry) override;
#else
    void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) override;
#endif
    void itemChange(ItemChange change, const ItemChangeData& data) override;
    void componentComplete() override;

private:
    void runLayout();
    int rowCountForMode() const;

    QString mPanelTitle;
    RibbonEnums::LayoutMode mLayoutMode = RibbonEnums::ThreeRowMode;
    bool mEnableShowPanelTitle = true;
    bool mWordWrap             = true;
    bool mIconRightText        = false;
    QVector< RibbonLayoutItemHost* > mChildItems;
    SARibbon::Core::SARibbonPanelLayoutEngine mEngine;
    QSize mLastSizeHint;
    int mLastColumnCount = 0;
    int mLastLargeHeight = 0;
    QRect mLastTitleGeometry;
};

}

#endif  // RIBBONPANEL_H
