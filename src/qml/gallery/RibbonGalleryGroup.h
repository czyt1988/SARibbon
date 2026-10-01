#ifndef RIBBONGALLERYGROUP_H
#define RIBBONGALLERYGROUP_H
#include "SARibbonQmlGlobal.h"
#include "RibbonGalleryItem.h"
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
