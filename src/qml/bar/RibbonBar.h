#ifndef RIBBONBAR_H
#define RIBBONBAR_H
#include "SARibbonQmlGlobal.h"
#include <SARibbonCore/SARibbonBarGeometryEngine.h>
#include <QQuickItem>
#include <QVector>

namespace SARibbonQml {

class RibbonCategory;
class RibbonTab;

/**
 * \if ENGLISH
 * @brief Bar structural host (plan-04 S4)
 * @details Bar height from RibbonMetrics; title free area from the core
 * BarGeometryEngine::layoutTitleRect; the tab row is laid out by this host
 * (plain sequence, no Repeater — geometry authority stays in C++).
 * \endif
 *
 * \if CHINESE
 * @brief Bar 结构宿主（计划 04 S4）
 * @details bar 高度取自 RibbonMetrics；标题空闲区经 core 的
 * BarGeometryEngine::layoutTitleRect；tab 行由本宿主排布（平铺序列，不用
 * Repeater——几何权威留在 C++）。
 * \endif
 */
class RibbonBar : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(QQuickItem* barQmlItem READ barQmlItem WRITE setBarQmlItem NOTIFY barQmlItemChanged)
public:
    explicit RibbonBar(QQuickItem* parent = nullptr);

    int currentIndex() const;
    void setCurrentIndex(int idx);

    QQuickItem* barQmlItem() const;
    void setBarQmlItem(QQuickItem* item);

    void registerCategory(RibbonCategory* category);
    void unregisterCategory(RibbonCategory* category);

    void registerTab(RibbonTab* tab);
    void unregisterTab(RibbonTab* tab);

    // title free area (engine-computed); QML side binds the window title text
    QRectF titleRect() const;

Q_SIGNALS:
    void currentIndexChanged();
    void barQmlItemChanged();

protected:
    void updatePolish() override;
    void itemChange(ItemChange change, const ItemChangeData& data) override;

private:
    void relayout();

    int mCurrentIndex = 0;
    QQuickItem* mBarQmlItem = nullptr;
    QVector< RibbonCategory* > mCategories;
    QVector< RibbonTab* > mTabs;
    QRect mTitleRect;
};

}
#endif  // RIBBONBAR_H
