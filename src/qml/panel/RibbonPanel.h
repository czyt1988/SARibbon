#ifndef RIBBONPANEL_H
#define RIBBONPANEL_H
#include "SARibbonQmlGlobal.h"
#include "../SARibbonQmlTypes.h"
#include <SARibbonCore/SARibbonAbstractLayoutItem.h>
#include <SARibbonCore/SARibbonPanelLayoutEngine.h>
#include <QQuickItem>
#include <QVector>

namespace SARibbonQml {

class RibbonToolButton;

/**
 * \if ENGLISH
 * @brief Panel structural host: drives the core PanelLayoutEngine (plan-04 S3)
 * @details updatePolish() is the single layout entry: collect contract items ->
 * engine.layout() -> applyGeometry per item. updatePolish only fires for items
 * attached to a QQuickWindow, so tests must expose the scene first. The visual
 * leaf is created from qrc via the leaf-creation trilogy and pairs back through
 * the panelQmlItem handshake property.
 * \endif
 *
 * \if CHINESE
 * @brief Panel 结构宿主：驱动 core 的 PanelLayoutEngine（计划 04 S3）
 * @details updatePolish() 是唯一布局入口：收集契约 item -> engine.layout() ->
 *          逐项 applyGeometry。updatePolish 只对挂进 QQuickWindow 的 item 触发，
 *          测试须先曝光场景。视觉叶子经 qrc 以"创建三部曲"生成，并通过
 *          panelQmlItem 握手属性配对。
 * \endif
 */
class RibbonPanel : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString panelTitle READ panelTitle WRITE setPanelTitle NOTIFY panelTitleChanged)
    Q_PROPERTY(RibbonEnums::LayoutMode layoutMode READ layoutMode WRITE setLayoutMode NOTIFY layoutModeChanged)
    Q_PROPERTY(QQuickItem* panelQmlItem READ panelQmlItem WRITE setPanelQmlItem NOTIFY panelQmlItemChanged)
    Q_PROPERTY(QRectF titleGeometry READ titleGeometry NOTIFY titleGeometryChanged)
public:
    explicit RibbonPanel(QQuickItem* parent = nullptr);
    ~RibbonPanel() override;

    QString panelTitle() const;
    void setPanelTitle(const QString& t);

    RibbonEnums::LayoutMode layoutMode() const;
    void setLayoutMode(RibbonEnums::LayoutMode mode);

    // handshake property (plan-04 R5): the leaf root assigns itself back
    QQuickItem* panelQmlItem() const;
    void setPanelQmlItem(QQuickItem* item);

    // explicit child registration list (componentComplete of the buttons calls in)
    void registerChildItem(RibbonToolButton* item);
    void unregisterChildItem(RibbonToolButton* item);

    // drop the engine sizeHint cache entry of a button and re-run the layout
    void invalidateChildCache(RibbonToolButton* item);

    // engine-computed title strip rect; the leaf renders the caption inside it
    QRectF titleGeometry() const;

Q_SIGNALS:
    void panelTitleChanged();
    void layoutModeChanged();
    void panelQmlItemChanged();
    void titleGeometryChanged();

protected:
    void updatePolish() override;          // THE layout entry (scene-graph polished)
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry) override;
#else
    void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) override;
#endif
    void itemChange(ItemChange change, const ItemChangeData& data) override;
    void componentComplete() override;

private:
    void runLayout();
    void ensureQmlItem();
    int rowCountForMode() const;

    QString mPanelTitle;
    RibbonEnums::LayoutMode mLayoutMode = RibbonEnums::ThreeRowMode;
    QQuickItem* mPanelQmlItem = nullptr;
    QVector< RibbonToolButton* > mChildButtons;
    SARibbon::Core::SARibbonPanelLayoutEngine mEngine;
    QSize mLastSizeHint;
    int mLastColumnCount = 0;
    int mLastLargeHeight = 0;
    QRect mLastTitleGeometry;
};

}
#endif  // RIBBONPANEL_H
