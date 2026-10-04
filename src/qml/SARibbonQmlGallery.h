#ifndef RIBBONGALLERY_H
#define RIBBONGALLERY_H
#include "SARibbonQmlGlobal.h"
#include "SARibbonQmlLayoutItemHost.h"
#include "SARibbonQmlTypes.h"
// full definitions, not forward declarations: RibbonGalleryGroup*/
// RibbonGalleryItem* appear in Q_INVOKABLE signatures and signals, so the moc
// output instantiates QMetaType::fromType and silently loses the QObject
// specialization if the types are incomplete where that moc file happens to be
// compiled (NOTES B64)
#include "SARibbonQmlGalleryGroup.h"
#include "SARibbonQmlGalleryItem.h"
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
 *          gallery fulfils). The current group renders as an icon grid whose
 *          caption band follows captionStyle; scroll buttons page the grid and
 *          the more button pops a viewport listing every group. Grid cell sizes
 *          come from the core calcGalleryGridCellSize (moved from the widgets
 *          SARibbonGalleryGroup so both front ends derive identical cells),
 *          and the caption band from the core calcGalleryCellMetrics driven by
 *          captionStyle — the three widgets gallery group styles.
 *          Activation is mediated by triggered (activateItem invokable), so
 *          tests drive it without the popup.
 * \endif
 *
 * \if CHINESE
 * @brief 画廊宿主：在面板内伸展的图标网格
 * @details 对应 widgets 侧 SARibbonGallery：全高（Large 比例）可伸展项
 *          （expandingDirections 返回 Qt::Horizontal；stretchFactor 参与
 *          core 面板引擎的加权额外宽度分配——与 widgets 画廊履行同一契约）。
 *          当前组渲染为图标网格，标题带由 captionStyle 决定（三种样式对应
 *          widgets 的三种画廊组样式）；滚动按钮翻页，更多按钮弹出列出所有组的
 *          视口。网格单元尺寸来自 core 的 calcGalleryGridCellSize，标题带来自
 *          core 的 calcGalleryCellMetrics（均自 widgets SARibbonGalleryGroup
 *          下沉，双前端以相同输入推导相同单元）。激活经 triggered 信号中转
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
    Q_PROPERTY(SARibbonQml::RibbonEnums::GalleryCaptionStyle captionStyle READ captionStyle WRITE setCaptionStyle NOTIFY captionStyleChanged)
    Q_PROPERTY(int currentItemIndex READ currentItemIndex WRITE setCurrentItemIndex NOTIFY currentItemIndexChanged)
    // grid metrics consumed by the visual leaf (re-published on relayout)
    Q_PROPERTY(QSize gridSize READ gridSize NOTIFY gridMetricsChanged)
    Q_PROPERTY(int gridColumns READ gridColumns NOTIFY gridMetricsChanged)
    Q_PROPERTY(int totalRows READ totalRows NOTIFY gridMetricsChanged)
    Q_PROPERTY(int captionHeight READ captionHeight NOTIFY gridMetricsChanged)
    Q_PROPERTY(int cellIconWidth READ cellIconWidth NOTIFY gridMetricsChanged)
    Q_PROPERTY(int cellIconHeight READ cellIconHeight NOTIFY gridMetricsChanged)
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

    // Caption band style of every cell (widgets setGalleryGroupStyle parity)
    SARibbonQml::RibbonEnums::GalleryCaptionStyle captionStyle() const;
    void setCaptionStyle(SARibbonQml::RibbonEnums::GalleryCaptionStyle style);

    // Index of the current (selected) cell in the CURRENT group, -1 for none;
    // refuses disabled and non-selectable entries (Qt::ItemIsSelectable parity)
    int currentItemIndex() const;
    void setCurrentItemIndex(int index);
    Q_INVOKABLE SARibbonQml::RibbonGalleryItem* currentItem() const;

    QSize gridSize() const;
    int gridColumns() const;
    int totalRows() const;
    /// Caption band height reserved at the bottom of each grid cell
    int captionHeight() const;
    /// Icon box width inside each grid cell (widgets setIconSize parity)
    int cellIconWidth() const;
    /// Icon box height inside each grid cell (widgets setIconSize parity)
    int cellIconHeight() const;
    int buttonStripWidth() const;

    // Page the grid (leaf scroll buttons); clamped by setScrollRow
    Q_INVOKABLE void scrollUp();
    Q_INVOKABLE void scrollDown();

    // Activate an entry of the CURRENT group by index (leaf cell click +
    // tests); emits triggered; disabled entries are ignored
    Q_INVOKABLE void activateItem(int index);

    // Leaf hover callback: publishes hovered(item, index) and forwards it to
    // the current group (widgets SARibbonGallery::hovered parity); a negative
    // index means the pointer left the grid
    Q_INVOKABLE void notifyCellHovered(int index);

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
    void captionStyleChanged();
    void currentItemIndexChanged();
    void gridMetricsChanged();
    void triggered(SARibbonQml::RibbonGalleryItem* item, int index);

    /**
     * \if ENGLISH
     * @brief The pointer entered a grid cell (or left the grid)
     * @param item The hovered entry, nullptr when the pointer left the grid
     * @param index Its index in the current group, -1 when the pointer left
     * @details Counterpart of the widgets SARibbonGallery::hovered, which
     *          forwards SARibbonGalleryGroup::hovered (QActionGroup::hovered).
     *          QActionGroup has no "un-hover" notification, so leaving the grid
     *          is reported here as an explicit nullptr/-1 pair — consumers that
     *          show a preview need it to clear the preview again.
     * \endif
     *
     * \if CHINESE
     * @brief 指针进入某个网格单元（或离开网格）
     * @param item 被悬停的条目；指针离开网格时为 nullptr
     * @param index 其在当前组中的下标；指针离开网格时为 -1
     * @details 对应 widgets 的 SARibbonGallery::hovered（转发
     *          SARibbonGalleryGroup::hovered，即 QActionGroup::hovered）。
     *          QActionGroup 没有"取消悬停"通知，因此这里把离开网格显式表达为
     *          nullptr/-1 一对值——需要展示预览的消费者要靠它把预览清掉。
     * \endif
     */
    void hovered(SARibbonQml::RibbonGalleryItem* item, int index);

    /**
     * \if ENGLISH
     * @brief The current (selected) cell changed
     * @param item The new current entry, nullptr when the selection was cleared
     * @param index Its index in the current group, -1 when cleared
     * \endif
     *
     * \if CHINESE
     * @brief 当前（选中）单元发生变化
     * @param item 新的当前条目；选择被清除时为 nullptr
     * @param index 其在当前组中的下标；清除时为 -1
     * \endif
     */
    void currentItemChanged(SARibbonQml::RibbonGalleryItem* item, int index);

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
    // Watch every item of a group so a selectable flip on the current entry can
    // clear the selection (the widgets selection model refuses such an index)
    void watchGroupItems(SARibbonQml::RibbonGalleryGroup* group);
    // Drop the current mark when it became invalid (out of range, disabled or
    // no longer selectable); emits currentItemIndexChanged when it changed
    void validateCurrentItemIndex();

    QVector< SARibbonQml::RibbonGalleryGroup* > mGroups;
    int mCurrentGroupIndex = 0;
    int mStretchFactor     = 0;  ///< 0 = legacy equal share (contract default)
    int mDisplayRow        = 1;  ///< widgets DisplayOneRow parity
    int mGridMinimumWidth  = 80;  ///< widgets example gridMinimumWidth parity
    int mScrollRow         = 0;
    // widgets SARibbonGalleryGroup::IconWithWordWrapText parity: the leaf has
    // always rendered the two-line caption, so this stays the default
    SARibbonQml::RibbonEnums::GalleryCaptionStyle mCaptionStyle = SARibbonQml::RibbonEnums::GalleryIconWithWordWrapText;
    int mCurrentItemIndex = -1;  ///< current (selected) cell of the current group, -1 = none
    QSize mGridSize;
    int mGridColumns    = 1;
    int mTotalRows      = 0;
    int mCaptionHeight  = 0;  ///< caption band height at the cell bottom (core derived)
    int mCellIconWidth  = 0;  ///< icon box width inside the cell (core derived)
    int mCellIconHeight = 0;  ///< icon box height inside the cell (core derived)
};

}

#endif  // RIBBONGALLERY_H
