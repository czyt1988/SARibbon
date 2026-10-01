#ifndef RIBBONGALLERY_H
#define RIBBONGALLERY_H
#include "SARibbonQmlGlobal.h"
#include "../host/RibbonLayoutItemHost.h"
#include "../SARibbonQmlTypes.h"
#include <QQmlListProperty>
#include <QSize>
#include <QVector>

namespace SARibbonQml {

class RibbonGalleryGroup;

/**
 * \if ENGLISH
 * @brief Gallery host: grid of icon cells that expands inside its panel
 * @details The QML counterpart of the widgets SARibbonGallery: a full-height
 *          (Large proportion) expanding item (expandingDirections returns
 *          Qt::Horizontal; stretchFactor feeds the core panel engine's
 *          weighted extra-width distribution — same contract the widgets
 *          gallery fulfils). The current group renders as an icon-over-
 *          wrapped-text grid; scroll buttons page the grid and the more
 *          button pops a viewport listing every group. Grid cell sizes come
 *          from the core calcGalleryGridCellSize (moved from the widgets
 *          SARibbonGalleryGroup so both front ends derive identical cells).
 *          Activation is mediated by triggered (activateItem invokable), so
 *          tests drive it without the popup.
 * \endif
 *
 * \if CHINESE
 * @brief 画廊宿主：在面板内伸展的图标网格
 * @details 对应 widgets 侧 SARibbonGallery：全高（Large 比例）可伸展项
 *          （expandingDirections 返回 Qt::Horizontal；stretchFactor 参与
 *          core 面板引擎的加权额外宽度分配——与 widgets 画廊履行同一契约）。
 *          当前组渲染为图标在上、折行文字在下的网格；滚动按钮翻页，更多
 *          按钮弹出列出所有组的视口。网格单元尺寸来自 core 的
 *          calcGalleryGridCellSize（自 widgets SARibbonGalleryGroup 下沉，
 *          双前端以相同输入推导相同单元）。激活经 triggered 信号中转
 *          （activateItem 可调用方法），测试无需弹出即可驱动。
 * \endif
 */
class SA_RIBBON_QML_EXPORT RibbonGallery : public RibbonLayoutItemHost
{
    Q_OBJECT
    Q_CLASSINFO("DefaultProperty", "groups")
    Q_PROPERTY(QQmlListProperty< SARibbonQml::RibbonGalleryGroup > groups READ groups NOTIFY groupsChanged)
    Q_PROPERTY(int currentGroupIndex READ currentGroupIndex WRITE setCurrentGroupIndex NOTIFY currentGroupIndexChanged)
    Q_PROPERTY(int stretchFactor READ stretchFactor WRITE setStretchFactor NOTIFY stretchFactorChanged)
    Q_PROPERTY(int displayRow READ displayRow WRITE setDisplayRow NOTIFY displayRowChanged)
    Q_PROPERTY(int gridMinimumWidth READ gridMinimumWidth WRITE setGridMinimumWidth NOTIFY gridMinimumWidthChanged)
    Q_PROPERTY(int scrollRow READ scrollRow WRITE setScrollRow NOTIFY scrollRowChanged)
    // grid metrics consumed by the visual leaf (re-published on relayout)
    Q_PROPERTY(QSize gridSize READ gridSize NOTIFY gridMetricsChanged)
    Q_PROPERTY(int gridColumns READ gridColumns NOTIFY gridMetricsChanged)
    Q_PROPERTY(int totalRows READ totalRows NOTIFY gridMetricsChanged)
    Q_PROPERTY(int buttonStripWidth READ buttonStripWidth CONSTANT)
public:
    explicit RibbonGallery(QQuickItem* parent = nullptr);
    ~RibbonGallery() override;

    QQmlListProperty< SARibbonQml::RibbonGalleryGroup > groups();
    int groupCount() const;
    // QML-facing (the leaf resolves its current group through it)
    Q_INVOKABLE SARibbonQml::RibbonGalleryGroup* groupAt(int index) const;

    int currentGroupIndex() const;
    void setCurrentGroupIndex(int idx);

    // doubles as the contract stretchFactor() override (feeds the engine's
    // weighted extra-width distribution)
    int stretchFactor() const override;
    void setStretchFactor(int factor);

    // Visible grid rows, clamped to [1,3] (widgets displayRow parity)
    int displayRow() const;
    void setDisplayRow(int rows);

    int gridMinimumWidth() const;
    void setGridMinimumWidth(int w);

    // First visible grid row (clamped against totalRows - displayRow)
    int scrollRow() const;
    void setScrollRow(int row);

    QSize gridSize() const;
    int gridColumns() const;
    int totalRows() const;
    int buttonStripWidth() const;

    // Page the grid (leaf scroll buttons); clamped by setScrollRow
    Q_INVOKABLE void scrollUp();
    Q_INVOKABLE void scrollDown();

    // Activate an entry of the CURRENT group by index (leaf cell click +
    // tests); emits triggered; disabled entries are ignored
    Q_INVOKABLE void activateItem(int index);

    // ---- contract implementation (engine inputs/outputs) ----
    QSize sizeHint() const override;
    Qt::Orientations expandingDirections() const override;  // Qt::Horizontal

Q_SIGNALS:
    void groupsChanged();
    void currentGroupIndexChanged();
    void stretchFactorChanged();
    void displayRowChanged();
    void gridMinimumWidthChanged();
    void scrollRowChanged();
    void gridMetricsChanged();
    void triggered(SARibbonQml::RibbonGalleryItem* item, int index);

protected:
    QUrl leafUrl() const override;
    void componentComplete() override;
    void largeHeightContextChanged() override;
    void applyGeometry(const QRect& rect) override;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry) override;
#else
    void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) override;
#endif

private:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    using ListIndex = qsizetype;
#else
    using ListIndex = int;
#endif
    static void appendGroupCb(QQmlListProperty< SARibbonQml::RibbonGalleryGroup >* prop, SARibbonQml::RibbonGalleryGroup* group);
    static ListIndex groupCountCb(QQmlListProperty< SARibbonQml::RibbonGalleryGroup >* prop);
    static SARibbonQml::RibbonGalleryGroup* groupAtCb(QQmlListProperty< SARibbonQml::RibbonGalleryGroup >* prop, ListIndex index);
    static void clearGroupsCb(QQmlListProperty< SARibbonQml::RibbonGalleryGroup >* prop);

    void updateGridMetrics();
    void emitGroupsChanged();

    QVector< SARibbonQml::RibbonGalleryGroup* > mGroups;
    int mCurrentGroupIndex = 0;
    int mStretchFactor     = 0;  ///< 0 = legacy equal share (contract default)
    int mDisplayRow        = 3;
    int mGridMinimumWidth  = 80;  ///< widgets example gridMinimumWidth parity
    int mScrollRow         = 0;
    QSize mGridSize;
    int mGridColumns = 1;
    int mTotalRows   = 0;
};

}

#endif  // RIBBONGALLERY_H
