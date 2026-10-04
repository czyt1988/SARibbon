#ifndef RIBBONGALLERYGROUP_H
#define RIBBONGALLERYGROUP_H
#include "SARibbonQmlGlobal.h"
#include "SARibbonQmlGalleryItem.h"
#include <QQmlListProperty>
#include <QObject>
#include <QVector>

namespace SARibbonQml {

/**
 * \if ENGLISH
 * @brief Declarative gallery group: titled collection of gallery entries
 * @details The QML counterpart of the widgets SARibbonGalleryGroup: pure
 *          model data (group title + items). Grid metrics live on the
 *          gallery host (geometry authority); rendering on the leaf.
 * \endif
 *
 * \if CHINESE
 * @brief 声明式画廊组：带标题的画廊条目集合
 * @details 对应 widgets 侧 SARibbonGalleryGroup：纯模型数据（组标题 +
 *          条目）。网格度量的权威在画廊宿主；渲染在叶子。
 * \endif
 */
class SA_RIBBON_QML_EXPORT RibbonGalleryGroup : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("DefaultProperty", "items")
    Q_PROPERTY(QString groupTitle READ groupTitle WRITE setGroupTitle NOTIFY groupTitleChanged)
    Q_PROPERTY(QQmlListProperty< SARibbonQml::RibbonGalleryItem > items READ items NOTIFY itemsChanged)
public:
    explicit RibbonGalleryGroup(QObject* parent = nullptr);

    QString groupTitle() const;
    void setGroupTitle(const QString& t);

    QQmlListProperty< SARibbonQml::RibbonGalleryItem > items();
    int itemCount() const;
    SARibbonQml::RibbonGalleryItem* itemAt(int index) const;
    void appendItem(SARibbonQml::RibbonGalleryItem* item);
    void clearItems();

Q_SIGNALS:
    void groupTitleChanged();
    void itemsChanged();

    /**
     * \if ENGLISH
     * @brief The pointer entered one of this group's cells
     * @param item The hovered entry, nullptr when the pointer left the grid
     * @param index Its index in this group, -1 when the pointer left
     * @details Counterpart of the widgets SARibbonGalleryGroup::hovered
     *          (QActionGroup::hovered). A group is pure model data here and
     *          owns no view, so the gallery host emits this on the group's
     *          behalf while it forwards the same event as its own hovered —
     *          the widgets direction (group emits, gallery forwards) is
     *          inverted, but both signals fire, so either connection point
     *          works. Only the CURRENT group reports hover.
     * \endif
     *
     * \if CHINESE
     * @brief 指针进入本组的某个单元
     * @param item 被悬停的条目；指针离开网格时为 nullptr
     * @param index 其在本组中的下标；指针离开网格时为 -1
     * @details 对应 widgets 的 SARibbonGalleryGroup::hovered（即
     *          QActionGroup::hovered）。这里的组是纯模型数据、不持有视图，因此
     *          由画廊宿主代它发出本信号，同时宿主也发出自己的 hovered 转发同一
     *          事件——widgets 的方向（组发、画廊转发）被反转，但两个信号都会发，
     *          连接哪一侧都能收到。只有**当前组**会上报悬停。
     * \endif
     */
    void hovered(SARibbonQml::RibbonGalleryItem* item, int index);

private:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    using ListIndex = qsizetype;
#else
    using ListIndex = int;
#endif
    static void appendItemCb(QQmlListProperty< SARibbonQml::RibbonGalleryItem >* prop, SARibbonQml::RibbonGalleryItem* item);
    static ListIndex itemCountCb(QQmlListProperty< SARibbonQml::RibbonGalleryItem >* prop);
    static SARibbonQml::RibbonGalleryItem* itemAtCb(QQmlListProperty< SARibbonQml::RibbonGalleryItem >* prop, ListIndex index);
    static void clearItemsCb(QQmlListProperty< SARibbonQml::RibbonGalleryItem >* prop);

    QString mGroupTitle;
    QVector< SARibbonQml::RibbonGalleryItem* > mItems;
};

}

#endif  // RIBBONGALLERYGROUP_H
