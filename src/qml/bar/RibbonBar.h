#ifndef RIBBONBAR_H
#define RIBBONBAR_H
#include "SARibbonQmlGlobal.h"
#include "../host/RibbonQuickHost.h"
#include <SARibbonCore/SARibbonBarGeometryEngine.h>
#include <QRectF>
#include <QVector>

namespace SARibbonQml {

class RibbonCategory;
class RibbonTab;

/**
 * \if ENGLISH
 * @brief Bar structural host (plan-04 S4)
 * @details Bar height from RibbonMetrics; the tab row is laid out by this
 * host (plain sequence, no Repeater — geometry authority stays in C++).
 * Categories declared without a matching RibbonTab get an auto tab whose
 * text follows the category title (the QML counterpart of the widgets
 * addCategoryPage pairing); clicking a tab — host-side mouse handling —
 * switches currentIndex which shows the paired category. The optional
 * application button (label + geometry + click signal, office-2021 look)
 * is rendered by the bar leaf from the rect published here.
 * \endif
 *
 * \if CHINESE
 * @brief Bar 结构宿主（计划 04 S4）
 * @details bar 高度取自 RibbonMetrics；tab 行由本宿主排布（平铺序列，不用
 *          Repeater——几何权威留在 C++）。声明时未配对 RibbonTab 的 category
 *          会得到一个自动 tab（文字跟随 category 标题，对应 widgets 侧
 *          addCategoryPage 的配对语义）；点击 tab（宿主侧鼠标处理）切换
 *          currentIndex 并显示配对的 category。可选的应用按钮（文字+几何+
 *          点击信号，office-2021 外观）由 bar 叶子按本宿主发布的矩形渲染。
 * \endif
 */
class RibbonBar : public RibbonQuickHost
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(QString applicationLabel READ applicationLabel WRITE setApplicationLabel NOTIFY applicationLabelChanged)
    Q_PROPERTY(int tabBarHeight READ tabBarHeight NOTIFY layoutChanged)
    Q_PROPERTY(int titleBarHeight READ titleBarHeight NOTIFY layoutChanged)
    Q_PROPERTY(int categoryRowY READ categoryRowY NOTIFY layoutChanged)
    Q_PROPERTY(QRectF applicationButtonRect READ applicationButtonRect NOTIFY layoutChanged)
public:
    explicit RibbonBar(QQuickItem* parent = nullptr);
    ~RibbonBar() override;

    int currentIndex() const;
    void setCurrentIndex(int idx);

    QString applicationLabel() const;
    void setApplicationLabel(const QString& label);

    // layout values consumed by the visual leaf (re-published on relayout)
    int tabBarHeight() const;
    int titleBarHeight() const;
    int categoryRowY() const;
    QRectF applicationButtonRect() const;

    // Tab pairing: categories without an explicit RibbonTab at their index
    // get an auto-created tab bound to the category title
    void registerCategory(RibbonCategory* category);
    void unregisterCategory(RibbonCategory* category);

    void registerTab(RibbonTab* tab);
    void unregisterTab(RibbonTab* tab);

    // title free area (engine-computed); QML side binds the window title text
    QRectF titleRect() const;

Q_SIGNALS:
    void currentIndexChanged();
    void applicationLabelChanged();
    void layoutChanged();
    void applicationButtonClicked();

protected:
    QUrl leafUrl() const override;
    void componentComplete() override;
    void updatePolish() override;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry) override;
#else
    void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) override;
#endif
    void itemChange(ItemChange change, const ItemChangeData& data) override;

private:
    void relayout();
    void syncTabCount();
    RibbonTab* createAutoTab(int index);

    int mCurrentIndex = 0;
    QString mApplicationLabel;
    QVector< RibbonCategory* > mCategories;
    QVector< RibbonTab* > mTabs;      ///< explicit + auto tabs in row order
    QVector< RibbonTab* > mAutoTabs;  ///< subset owned (and destroyed) by this bar
    QRect mTitleRect;
    QRectF mApplicationButtonRect;
    int mTabBarHeight   = 0;
    int mTitleBarHeight = 0;
    int mCategoryRowY   = 0;
};

}

#endif  // RIBBONBAR_H
